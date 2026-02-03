# Carleton University Road Network & Simulation Data Generator
This script downloads, processes, and exports a drivable road network for Carleton University (Ottawa, Canada) using OpenStreetMap data.

## Environment Setup
Python 3.8+ recommended
```
python3 -m venv venv
source venv/bin/activate
pip install osmnx
pip install scikit-learn --no-cache-dir

```


## Usage
To execute the data creation run
```
cd data_creation
python3 your_script_name.py
```

## Outputs
There will be a 3 file output

1. carleton_campus_car_roads.geojson
    -A GeoJSON file containing:
        Simplified drivable road network
        Geometry, road names, speed limits, and one-way attributes
        Useful for visualization

2. sim_road_lengths.csv
    -Contains shortest-path distances and speed limits between predefined campus locations

3. sim_road_to_osm_edges.csv
    - Maps each simulation road to the underlying OpenStreetMap edges.
        Columns:
            ROAD – Simulation road name
            EDGE_IDS – Pipe-separated OSM edge IDs (osm_u_v_key)

        Useful for heatmaps, edge-based aggregation, or coupling with traffic models