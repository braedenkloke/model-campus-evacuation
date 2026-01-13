#Must have osmx installed use - pip3 install osmnx
#may have to creat virtual environemnt if on unbuntu
# ..../data_creation$ python3 -m venv venv
# ..../data_creation$ source venv/bin/activate
# (venv) ..../data_creation$ pip install osmnx
# (venv) ..../data_creation$ pip install scikit-learn --no-cache-dir


import osmnx as ox
import pandas as pd
import csv
import networkx as nx

from pyproj import Transformer
from shapely.geometry import Point, LineString

# MANUAL MARKER POINTS for parking lots
MARKER_NAME = "Manual Marker"
#Set location
MARKER_LATLON = (45.3813, -75.7021)

# Tolerance if marker is basically on an endpoint, don’t split
ENDPOINT_TOL_M = 1.0

# Download campus + network
print("Downloading Carleton University polygon...")
campus_shape = ox.geocode_to_gdf("Carleton University, Ottawa, Ontario, Canada")
campus_polygon = campus_shape.geometry.iloc[0]

print("Downloading drive network inside expanded campus boundary...")
G = ox.graph_from_polygon(campus_polygon, network_type="drive")

# Project to metric
G_proj = ox.project_graph(G)

print("Consolidating intersections...")
G_simp_proj = ox.consolidate_intersections(
    G_proj,
    tolerance=12,  # within 12m
    rebuild_graph=True
)

# Insert a node at MARKER_LATLON by splitting 1 edge
proj_crs = G_simp_proj.graph["crs"]
to_proj = Transformer.from_crs("EPSG:4326", proj_crs, always_xy=True)
to_ll = Transformer.from_crs(proj_crs, "EPSG:4326", always_xy=True)

def _edge_linestring(Gp, u, v, key, data):
    geom = data.get("geometry", None)
    if geom is not None:
        return geom
    # If missing, make straight segment from nodes
    xu, yu = Gp.nodes[u]["x"], Gp.nodes[u]["y"]
    xv, yv = Gp.nodes[v]["x"], Gp.nodes[v]["y"]
    return LineString([(xu, yu), (xv, yv)])

def _new_node_id(Gp):
    # OSMnx node ids are ints
    return int(max(Gp.nodes)) + 1

def cut_line_at_distance(line: LineString, dist: float):
    """
    Cut a LineString at distance 'dist' from its start.
    Returns (line1, line2) as LineStrings. If dist is at/near ends, returns (None, None).
    """
    if dist <= 0.0 or dist >= line.length:
        return None, None

    coords = list(line.coords)
    acc = 0.0

    for i in range(len(coords) - 1):
        p0 = Point(coords[i])
        p1 = Point(coords[i + 1])
        seg_len = p0.distance(p1)

        if acc + seg_len >= dist:
            t = (dist - acc) / seg_len if seg_len != 0 else 0.0
            x = coords[i][0] + t * (coords[i + 1][0] - coords[i][0])
            y = coords[i][1] + t * (coords[i + 1][1] - coords[i][1])

            cut_pt = (x, y)

            # Build first part
            first_coords = coords[: i + 1] + [cut_pt]
            # Build second part
            second_coords = [cut_pt] + coords[i + 1 :]

            if len(first_coords) < 2 or len(second_coords) < 2:
                return None, None

            return LineString(first_coords), LineString(second_coords)

        acc += seg_len

    return None, None


def split_nearest_edge_with_point(Gp, latlon, name="Inserted Node"):
    lat, lon = latlon
    x, y = to_proj.transform(lon, lat)
    pt = Point(x, y)

    # nearest edge (u, v, key)
    u, v, key = ox.distance.nearest_edges(Gp, X=x, Y=y)

    data = Gp.get_edge_data(u, v, key)
    geom = _edge_linestring(Gp, u, v, key, data)

    # snap point onto the edge geometry
    snapped = geom.interpolate(geom.project(pt))
    d_along = float(geom.project(snapped))  # distance along the line

    # if snapped is basically at an endpoint, reuse endpoint node
    u_pt = Point(Gp.nodes[u]["x"], Gp.nodes[u]["y"])
    v_pt = Point(Gp.nodes[v]["x"], Gp.nodes[v]["y"])
    if snapped.distance(u_pt) <= ENDPOINT_TOL_M:
        return u
    if snapped.distance(v_pt) <= ENDPOINT_TOL_M:
        return v

    # Cut the line by distance
    g1, g2 = cut_line_at_distance(geom, d_along)
    if g1 is None or g2 is None:
        #if the snapped point is extremely close to an existing node, use it
        nearest_existing = ox.distance.nearest_nodes(Gp, X=snapped.x, Y=snapped.y)
        if Point(Gp.nodes[nearest_existing]["x"], Gp.nodes[nearest_existing]["y"]).distance(snapped) <= ENDPOINT_TOL_M:
            return nearest_existing
        raise RuntimeError("Could not cut the nearest edge into two valid segments.")

    # Create new node
    new_id = _new_node_id(Gp)
    lon_new, lat_new = to_ll.transform(snapped.x, snapped.y)

    Gp.add_node(
        new_id,
        x=float(snapped.x),
        y=float(snapped.y),
        lon=float(lon_new),
        lat=float(lat_new),
        street_node=name
    )

    # Collect ALL parallel edges u->v and v->u
    edges_to_split = []

    if Gp.has_edge(u, v):
        for k, d in Gp.get_edge_data(u, v).items():
            edges_to_split.append((u, v, k, d))

    if Gp.has_edge(v, u):
        for k, d in Gp.get_edge_data(v, u).items():
            edges_to_split.append((v, u, k, d))

    # Remove all of them
    for a, b, k, _ in edges_to_split:
        Gp.remove_edge(a, b, k)


    def add_split_edge_from_template(a, b, geom_piece, template_attrs):
        attrs = dict(template_attrs)
        attrs["geometry"] = geom_piece
        attrs["length"] = float(geom_piece.length)
        attrs["u"] = a
        attrs["v"] = b
        Gp.add_edge(a, b, **attrs)

    # Decide which piece attaches to u vs v
    g1_start = Point(list(g1.coords)[0])
    g1_end = Point(list(g1.coords)[-1])

    if min(g1_start.distance(u_pt), g1_end.distance(u_pt)) <= min(g1_start.distance(v_pt), g1_end.distance(v_pt)):
        first, second = g1, g2
    else:
        first, second = g2, g1

    # Re-add split edges for EACH original direction
    for a, b, k, d in edges_to_split:
        base_attrs = dict(d)

        if a == u and b == v:
            add_split_edge_from_template(u, new_id, first, base_attrs)
            add_split_edge_from_template(new_id, v, second, base_attrs)
        else:
            # reverse direction
            add_split_edge_from_template(v, new_id, second, base_attrs)
            add_split_edge_from_template(new_id, u, first, base_attrs)

    return new_id


print("Splitting nearest road edge at marker...")
inserted_node = split_nearest_edge_with_point(G_simp_proj, MARKER_LATLON, name=MARKER_NAME)
print(f"Inserted/snap node id: {inserted_node}")

# Reproject back to lat/lon
G_simp = ox.project_graph(G_simp_proj, to_crs="EPSG:4326")

nodes_simp, edges_simp = ox.graph_to_gdfs(G_simp)

# EDGE ATTRIBUTES
edges_simp["name"] = edges_simp.get("name", None)
edges_simp["name"] = edges_simp["name"].fillna("Unnamed") if "name" in edges_simp.columns else "Unnamed"

edges_simp["maxspeed"] = edges_simp.get("maxspeed", None)
if "maxspeed" in edges_simp.columns:
    edges_simp["maxspeed"] = edges_simp["maxspeed"].fillna("Unknown")
    edges_simp["maxspeed"] = edges_simp["maxspeed"].apply(
        lambda x: x[0] if isinstance(x, list) and len(x) > 0 else x
    )
else:
    edges_simp["maxspeed"] = "Unknown"

edges_simp["oneway"] = edges_simp.get("oneway", False)
if "oneway" in edges_simp.columns:
    edges_simp["oneway"] = edges_simp["oneway"].fillna(False)
else:
    edges_simp["oneway"] = False

print("Exporting GeoJSON...")
edges_simp.to_file("carleton_campus_car_roads.geojson", driver="GeoJSON")

print("Exporting CSVs...")

# Extract u,v
start_nodes = edges_simp.index.get_level_values(0)
end_nodes = edges_simp.index.get_level_values(1)

edges_df = pd.DataFrame({
    "start_node": start_nodes,
    "end_node": end_nodes,
    "street_name": edges_simp["name"].values,
    "length_m": edges_simp["length"].values if "length" in edges_simp.columns else None,
    "maxspeed": edges_simp["maxspeed"].values,
    "oneway": edges_simp["oneway"].values,
    "geometry_wkt": edges_simp.geometry.to_wkt().values
})
edges_df.to_csv("carleton_campus_car_edges.csv", index=False)

print("\nAll files exported successfully!")


# SIM-ROAD LENGTHS PART

# ALL "places" in coupled model
PLACES = {
    "Library Rd & P1": (45.3813, -75.7007),
    "Library Rd & University Dr": (45.3793, -75.7005),
}

SIM_ROADS = [
    ("Library Rd & P1", "Library Rd & University Dr",
     "Library Rd & P1 to Library Rd & University Dr"),
]

to_proj = Transformer.from_crs("EPSG:4326", proj_crs, always_xy=True)

def nearest_node_projected(Gp, latlon):
    lat, lon = latlon
    x, y = to_proj.transform(lon, lat)
    return ox.distance.nearest_nodes(Gp, X=x, Y=y)

place_node = {name: nearest_node_projected(G_simp_proj, latlon) for name, latlon in PLACES.items()}

print("Snapped nodes:", place_node)

out_csv = "sim_road_lengths.csv"
with open(out_csv, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["ROAD", "FROM_PLACE", "TO_PLACE", "FROM_NODE", "TO_NODE", "LENGTH_M"])

    for a, b, sim_name in SIM_ROADS:
        src = place_node[a]
        dst = place_node[b]

        try:
            length = nx.shortest_path_length(G_simp_proj, source=src, target=dst, weight="length")
            writer.writerow([sim_name, a, b, int(src), int(dst), float(length)])
        except nx.NetworkXNoPath:
            Gu = G_simp_proj.to_undirected()
            length = nx.shortest_path_length(Gu, source=src, target=dst, weight="length")
            writer.writerow([sim_name, a, b, int(src), int(dst), float(length)])

print(f"\nWrote {out_csv} for {len(SIM_ROADS)} simulation roads.")
