# General python package installations
pip install numpy pandas matplotlib geopandas shapely pyproj fiona contextily


# Summary in the Terminal
To get the summary output manually, run
python analysis/data_analysis.py output_data/raw/scenario_01_log.csv
in the terminal.

# Generate Visuals
To get the final visuals in ouput_data/processed run
python analysis/visualize_processed.py output_data/processed
in the terminal.

# Generate mp4
Ensure you are in the analysis folder (cd analysis)

Install ffmpeg:
sudo apt update
sudo apt install -y ffmpeg

run 
pyhton map_roads.py
in the terminal.
