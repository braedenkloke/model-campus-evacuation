
#Must have osmx installed use - pip3 install osmnx
#may have to creat virtual environemnt if on unbuntu 
# ..../data_creation$ python3 -m venv venv
# ..../data_creation$ source venv/bin/activate
# (venv) ..../data_creation$ pip install osmnx
import osmnx as ox
import geopandas as gpd
import pandas as pd

# Download official Carleton University polygon
print("Downloading Carleton University polygon...")
#get geocode (geodataframe) for place specified we should be able to alter to any well known place
campus_shape = ox.geocode_to_gdf("Carleton University, Ottawa, Ontario, Canada")
campus_polygon = campus_shape.geometry.iloc[0]

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

# Export GeoJSON
print("Exporting GeoJSON...")
edges_simp.to_file("carleton_campus_car_roads.geojson", driver="GeoJSON")

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

print("\nAll files exported successfully!")
