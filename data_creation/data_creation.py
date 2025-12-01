
#Must have osmx installed use - pip3 install osmnx
#may have to creat virtual environemnt if on unbuntu 
# ..../data_creation$ python3 -m venv venv
# ..../data_creation$ source venv/bin/activate
# (venv) ..../data_creation$ pip install osmnx
import osmnx as ox
import geopandas as gpd
import pandas as pd

#creat custom polygon if needed
from shapely.geometry import Polygon

#this custom polygon took very long time run would suggestusing only box not polygon (keeping for future testing)
#coords = [
#    (-75.70202448159323, 45.37810322062981),
 #   (-75.68826431331505, 45.38397927498423),
  # (-75.699600458617, 45.38801690098822),
   # (-75.6988612850446, 45.38538129178215),
    #(-75.69911416718756, 45.383746068171774),
#    (-75.70006192285037, 45.38251419944251),
 #   (-75.70109682806529, 45.38085380672004),
  #  (-75.70202448159323, 45.37810322062981)
#]

#custom_polygon = Polygon(coords)

# Download official Carleton University polygon
print("Downloading Carleton University polygon...")
#get geocode (geodataframe) for place specified we should be able to alter to any well known place
campus_shape = ox.geocode_to_gdf("Carleton University, Ottawa, Ontario, Canada")
campus_polygon = campus_shape.geometry.iloc[0]
#tried expanding polygon only adds unecessary streets

# Download drive network
print("Downloading drive network inside expanded campus boundary...")
G = ox.graph_from_polygon(
    campus_polygon,
    network_type="drive"
)

# Project to metric CRS for consolidation
G_proj = ox.project_graph(G)

# Consolidate intersections, merges nearby nodes within 12m
print("Consolidating intersections...")
G_simp_proj = ox.consolidate_intersections(
    G_proj,
    tolerance=12,  # within ~12m
    rebuild_graph=True
)

# Reproject back to lat/lon
G_simp = ox.project_graph(G_simp_proj, to_crs="EPSG:4326")

# Convert to GeoDataFrames
nodes_simp, edges_simp = ox.graph_to_gdfs(G_simp)

# EDGE ATTRIBUTES
edges_simp["name"] = edges_simp["name"].fillna("Unnamed")
edges_simp["maxspeed"] = edges_simp["maxspeed"].fillna("Unknown")
edges_simp["oneway"] = edges_simp["oneway"].fillna(False)

edges_simp["maxspeed"] = edges_simp["maxspeed"].apply(
    lambda x: x[0] if isinstance(x, list) and len(x) > 0 else x
)

# Identify major intersections (3+ connecting streets)
print("Extracting major intersections...")
major = nodes_simp[nodes_simp["street_count"] >= 3].copy()

# Build mapping of nodes → connected streets, speeds, oneway
print("Extracting connected street names, speed limits, and one-way info...")

node_to_streets = {node: set() for node in major.index}
node_to_speeds = {node: set() for node in major.index}
node_to_oneway = {node: set() for node in major.index}

for (u, v, key), row in edges_simp.iterrows():
    street_name = row["name"]
    speed = str(row["maxspeed"])
    is_oneway = str(row["oneway"])

    for node in (u, v):
        if node in node_to_streets:
            node_to_streets[node].add(street_name)
            node_to_speeds[node].add(speed)
            node_to_oneway[node].add(is_oneway)

# Attach fields to major intersections
major["connected_streets"] = major.index.map(
    lambda n: ", ".join(sorted(node_to_streets[n]))
)
major["speed_limits"] = major.index.map(
    lambda n: ", ".join(sorted(node_to_speeds[n]))
)
major["oneway"] = major.index.map(
    lambda n: ", ".join(sorted(node_to_oneway[n]))
)

# Export GeoJSON
print("Exporting GeoJSON...")
edges_simp.to_file("carleton_campus_car_roads.geojson", driver="GeoJSON")
major.to_file("carleton_campus_major_intersections.geojson", driver="GeoJSON")

# Export CSVs
print("Exporting CSVs...")

# Extract u,v from MultiIndex
start_nodes = edges_simp.index.get_level_values(0)
end_nodes = edges_simp.index.get_level_values(1)

edges_df = pd.DataFrame({
    "start_node": start_nodes,
    "end_node": end_nodes,
    "street_name": edges_simp["name"].values,
    "length_m": edges_simp["length"].values,
    "maxspeed": edges_simp["maxspeed"].values,
    "oneway": edges_simp["oneway"].values,
    "geometry_wkt": edges_simp.geometry.to_wkt().values
})
edges_df.to_csv("carleton_campus_car_edges.csv", index=False)

intersections_df = pd.DataFrame({
    "node_id": major.index,
    "lat": major.geometry.y,
    "lon": major.geometry.x,
    "connected_streets": major["connected_streets"],
    "speed_limits": major["speed_limits"],
    "oneway": major["oneway"]
})
intersections_df.to_csv("carleton_campus_major_intersections.csv", index=False)

print("\nAll files exported successfully!")
