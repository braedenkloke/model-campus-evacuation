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
import re

from pyproj import Transformer
from shapely.geometry import Point, LineString

# MANUAL MARKER POINTS for parking lots
P1NAME = "P1"
P2NAME = "P2"
P3NAME = "P3"
P4NAME = "P4"
RAVENSRDEXITNAME = "Ravensrd Exit"
STADIUMEXITNAME = "Stadium Exit"
#Set location
P1Coord = (45.3813, -75.7021)
P2Coord = (45.3839, -75.6964)
P3Coord = (45.3847, -75.6919)
P4Coord = (45.3857, -75.6950)
RAVENSRDEXITCOORD = (45.3851, -75.6903)
STADIUMEXITCOORD = (45.3881, -75.6927)

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

def add_marker_node_and_connect(Gp, latlon, connect_to_node=None, name="Marker", edge_name="Marker connector"):
    """
    Adds a new node exactly at latlon, and connects it to the nearest road node
    """
    lat, lon = latlon
    x, y = to_proj.transform(lon, lat)

    # pick what road node to connect to
    if connect_to_node is None:
        connect_to_node = ox.distance.nearest_nodes(Gp, X=x, Y=y)

    # create the marker node
    marker_id = _new_node_id(Gp)
    Gp.add_node(
        marker_id,
        x=float(x),
        y=float(y),
        lon=float(lon),
        lat=float(lat),
        street_node=name
    )

    # build connector geometry + length in metres (projected CRS)
    x2, y2 = Gp.nodes[connect_to_node]["x"], Gp.nodes[connect_to_node]["y"]
    geom = LineString([(x, y), (x2, y2)])
    length_m = float(geom.length)

    # attributes for the connector edge
    attrs = {
        "u": marker_id,
        "v": connect_to_node,
        "name": edge_name,
        "highway": "service",
        "oneway": False,
        "geometry": geom,
        "length": length_m,
    }

    # add both directions so routing works either way (since your graph is directed)
    Gp.add_edge(marker_id, connect_to_node, **attrs)
    Gp.add_edge(connect_to_node, marker_id, **{**attrs, "u": connect_to_node, "v": marker_id})

    return marker_id, connect_to_node


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
inserted_node = split_nearest_edge_with_point(G_simp_proj, P1Coord, name=P1NAME)
inserted_node = split_nearest_edge_with_point(G_simp_proj, P2Coord, name=P2NAME)
inserted_node = split_nearest_edge_with_point(G_simp_proj, P4Coord, name=P4NAME)

p3_marker_node, p3_nearest_road_node = add_marker_node_and_connect(G_simp_proj, P3Coord, connect_to_node=None, name=P3NAME, edge_name="P3 connector")
p3_marker_node, p3_nearest_road_node = add_marker_node_and_connect(G_simp_proj, RAVENSRDEXITCOORD, connect_to_node=None, name=RAVENSRDEXITNAME, edge_name="Ravensrd Exit connector")
p3_marker_node, p3_nearest_road_node = add_marker_node_and_connect(G_simp_proj, STADIUMEXITCOORD, connect_to_node=None, name=STADIUMEXITNAME, edge_name="Stadium Exit connector")

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

def parse_maxspeed_to_kph(val):
    if val is None:
        return 40
    if isinstance(val, list) and len(val) > 0:
        val = val[0]
    if isinstance(val, (int, float)):
        return float(val)
    if not isinstance(val, str):
        return 40

    s = val.strip().lower()
    if s in ("", "unknown", "none", "signals", "variable"):
        return 40

    if ";" in s:
        s = s.split(";")[0].strip()

    is_mph = "mph" in s
    m = re.search(r"(\d+(\.\d+)?)", s)
    if not m:
        return 40

    num = float(m.group(1))
    return num * 1.60934 if is_mph else num

def path_speed_limit_kph(Gp, node_path):
    speeds = []

    for u, v in zip(node_path[:-1], node_path[1:]):
        edict = Gp.get_edge_data(u, v)
        if not edict:
            continue

        # pick the parallel edge with smallest length
        best = None
        best_len = float("inf")
        for k, d in edict.items():
            L = float(d.get("length", float("inf")))
            if L < best_len:
                best_len = L
                best = d

        sp = parse_maxspeed_to_kph(best.get("maxspeed") if best else None)
        if sp is not None:
            speeds.append(sp)

    return (min(speeds) if speeds else None)



# SIM-ROAD LENGTHS PART

# ALL "places" in coupled model
PLACES = {
    "Library Rd & P1": (45.3813, -75.7007),
    "Library Rd & University Dr": (45.3793, -75.7005),
    "Campus Ave & Library Rd": (45.3855, -75.6964),
    "Campus Ave & P2": (45.3839, -75.6964),
    "Campus Ave & University Dr": (45.3825, -75.6958),
    "Raven Rd & University Dr": (45.3840, -75.6940),
    "P3 & Raven Rd": (45.3847, -75.6919),
    "Bronson Ave & Raven Rd": (45.3851, -75.6903),
    "Colonel By Dr & University Dr": (45.3790, -75.7008),
    "P4 & University Dr": (45.3857, -75.6950),
    "Stadium Way & University Dr": (45.3875, -75.6956),
    "P5 & Stadium Way": (45.3876, -75.6950),
    "Bronson Ave & Stadium Way": (45.3881, -75.6927),
    "Roundabout": (45.3889, -75.6960),
    "Bronson Ave & University Dr": (45.3896, -75.6945),
    "Campus Ave & P6": (45.3886, -75.6970),

}

SIM_ROADS = [
    ("Library Rd & P1", "Library Rd & University Dr",
     "Library Rd & P1 to Library Rd & University Dr"),
    ("Library Rd & P1", "Campus Ave & Library Rd",
     "Library Rd & P1 to Campus Ave & Library Rd"),
    ("Library Rd & University Dr", "Library Rd & P1",
     "Library Rd & University Dr to Library Rd & P1"),
    ("Campus Ave & Library Rd", "Library Rd & P1",
     "Campus Ave & Library Rd to Library Rd & P1"),
    ("Campus Ave & Library Rd", "Campus Ave & P2",
     "Campus Ave & Library Rd to Campus Ave & P2"),
    ("Campus Ave & P2", "Campus Ave & University Dr",
     "Campus Ave & P2 to Campus Ave & University Dr"),
    ("Campus Ave & University Dr", "Library Rd & University Dr",
     "Campus Ave & University Dr to Library Rd & University Dr"),
    ("Library Rd & University Dr", "Campus Ave & University Dr",
      "Library Rd & University Dr to Campus Ave & University Dr"),
    ("Campus Ave & University Dr", "Raven Rd & University Dr",
      "Campus Ave & University Dr to Raven Rd & University Dr"),
    ("Raven Rd & University Dr", "Campus Ave & University Dr",
      "Raven Rd & University Dr to Campus Ave & University Dr"),
    ("Raven Rd & University Dr", "P3 & Raven Rd",
      "Raven Rd & University Dr to P3 & Raven Rd"),
    ("P3 & Raven Rd", "Raven Rd & University Dr",
      "P3 & Raven Rd to Raven Rd & University Dr"),
    ("P3 & Raven Rd", "Bronson Ave & Raven Rd",
      "P3 & Raven Rd to Bronson Ave & Raven Rd"),
    ("Library Rd & University Dr", "Colonel By Dr & University Dr",
      "Library Rd & University Dr to Colonel By Dr & University Dr"),
    ("Raven Rd & University Dr", "P4 & University Dr",
      "Raven Rd & University Dr to P4 & University Dr"),
    ("P4 & University Dr", "Raven Rd & University Dr",
      "P4 & University Dr to Raven Rd & University Dr"),
    ("P4 & University Dr", "Stadium Way & University Dr",
      "P4 & University Dr to Stadium Way & University Dr"),
    ("Stadium Way & University Dr", "P4 & University Dr",
      "Stadium Way & University Dr to P4 & University Dr"),
    ("Stadium Way & University Dr", "P5 & Stadium Way",
      "Stadium Way & University Dr to P5 & Stadium Way"),
    ("P5 & Stadium Way", "Stadium Way & University Dr",
      "P5 & Stadium Way to Stadium Way & University Dr"),
    ("P5 & Stadium Way", "Bronson Ave & Stadium Way",
      "P5 & Stadium Way to Bronson Ave & Stadium Way"),
    ("Stadium Way & University Dr", "Roundabout",
      "Stadium Way & University Dr to Roundabout"),
    ("Roundabout", "Stadium Way & University Dr",
      "Roundabout to Stadium Way & University Dr"),
    ("Roundabout", "Bronson Ave & University Dr",
      "Roundabout to Bronson Ave & University Dr"),
    ("Roundabout", "Campus Ave & P6",
      "Roundabout to Campus Ave & P6"),
    ("Campus Ave & P6", "Roundabout",
      "Campus Ave & P6 to Roundabout"),
    ("Campus Ave & P6", "Campus Ave & Library Rd",
      "Campus Ave & P6 to Campus Ave & Library Rd"),
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
    writer.writerow(["ROAD", "FROM_PLACE", "TO_PLACE", "FROM_NODE", "TO_NODE", "LENGTH_M", "SPEED_KPH"])

    for a, b, sim_name in SIM_ROADS:
        src = place_node[a]
        dst = place_node[b]

        try:
            node_path = nx.shortest_path(G_simp_proj, source=src, target=dst, weight="length")
            length = nx.shortest_path_length(G_simp_proj, source=src, target=dst, weight="length")
            speed_kph = path_speed_limit_kph(G_simp_proj, node_path)
        except nx.NetworkXNoPath:
            Gu = G_simp_proj.to_undirected()
            node_path = nx.shortest_path(Gu, source=src, target=dst, weight="length")
            length = nx.shortest_path_length(Gu, source=src, target=dst, weight="length")
            speed_kph = path_speed_limit_kph(Gu, node_path)

        writer.writerow([
            sim_name, a, b, int(src), int(dst), float(length),
            ("Unknown" if speed_kph is None else float(speed_kph))
        ])


print(f"\nWrote {out_csv} for {len(SIM_ROADS)} simulation roads.")
