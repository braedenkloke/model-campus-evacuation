import numpy as np
import pandas as pd
import geopandas as gpd
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.animation import FuncAnimation
from matplotlib.collections import LineCollection
from pathlib import Path
import contextily as ctx
import argparse

def create_heatmap_animation(heatmap_data_filename):
    """
    Creates a heatmap animation from heatmap data.

    heatmap_data_filename: Filename of heatmap matrix CSV
    """
    HERE = Path(__file__).resolve().parent
    ROOT = HERE.parent
    OUTPUT_DATA_DIR = "output_data/processed"

    GEOJSON_PATH = ROOT / "data_creation" / "carleton_campus_car_roads.geojson"
    gdf = gpd.read_file(GEOJSON_PATH)

    # GeoJSON has nested properties; in geopandas they become columns directly (u, v, key, sim_roads, geometry, etc.)
    gdf["edge_id"] = gdf.apply(lambda r: f"osm_{int(r['u'])}_{int(r['v'])}_{int(r['key'])}", axis=1)

    # Build mapping from sim_road name to list of edge_ids
    sim_to_edges = {}
    for row in gdf.itertuples(index=False):
        sim_roads = getattr(row, "sim_roads", None)
        eid = getattr(row, "edge_id")
        
        if sim_roads is None:
            continue
        if isinstance(sim_roads, float):
            continue
        if len(sim_roads) == 0:
            continue

        for sr in sim_roads:
            sim_to_edges.setdefault(sr, []).append(eid)

    # Load heat matrix
    HEAT_PATH = ROOT / "output_data" / "processed" / heatmap_data_filename
    heat = pd.read_csv(HEAT_PATH)
    heat.columns = heat.columns.str.strip()

    times = heat["time"].to_numpy()
    T = len(times)

    # Creates dictionary with key=edge_id, value=np.array of length T with occupancy values for that edge over time
    edge_series = {eid: np.zeros(T, dtype=float) for eid in gdf["edge_id"].tolist()}

    missing_cols = []
    for col in heat.columns:
        if col == "time":
            continue

        sim_road_name = col
        if sim_road_name not in sim_to_edges:
            missing_cols.append(sim_road_name)
            continue

        vals = heat[col].to_numpy(dtype=float)
        for eid in sim_to_edges[sim_road_name]:
            edge_series[eid] += vals  

    # Background map
    plot_gdf = gdf[["edge_id", "geometry"]].copy().to_crs(epsg=3857)

    # Prepare segments for plotting
    segments = []
    edge_ids = plot_gdf["edge_id"].tolist()

    for geom in plot_gdf.geometry:
        if geom is None:
            segments.append(np.array([[0, 0], [0, 0]]))
        elif geom.geom_type == "LineString":
            segments.append(np.array(geom.coords))
        elif geom.geom_type == "MultiLineString":
            segments.append(np.array(list(geom.geoms)[0].coords))
        else:
            segments.append(np.array([[0, 0], [0, 0]]))

    # Build matrix M[time, edge_index]
    M = np.column_stack([edge_series[eid] for eid in edge_ids])  # shape (T, E)

    # Animate heatmap on the road network
    fig, ax = plt.subplots(figsize=(10, 10))
    ax.set_axis_off()

    vmin = float(np.min(M))
    vmax = float(np.max(M)) if float(np.max(M)) > vmin else (vmin + 1.0)
    norm = mpl.colors.Normalize(vmin=vmin, vmax=vmax)
    cmap = mpl.colormaps["RdYlBu_r"]

    # Set bounds
    minx, miny, maxx, maxy = plot_gdf.total_bounds
    ax.set_xlim(minx, maxx)
    ax.set_ylim(miny, maxy)

    # Gets OSM tiles for the background map
    # Needs internet access
    ctx.add_basemap(
        ax,
        crs=plot_gdf.crs,
        source=ctx.providers.OpenStreetMap.Mapnik
    )

    # Draw heat layer on top
    lc = LineCollection(segments, linewidths=2.5)
    lc.set_colors([cmap(norm(v)) for v in M[0]])
    ax.add_collection(lc)

    time_text = ax.text(0.02, 0.98, "", transform=ax.transAxes, va="top", ha="left", fontsize=14)

    # Colourbar
    sm = mpl.cm.ScalarMappable(norm=norm, cmap=cmap)
    sm.set_array([])
    cbar = fig.colorbar(sm, ax=ax, fraction=0.03, pad=0.02)
    cbar.set_label("Cars/100m") 

    def update(i):
        lc.set_colors([cmap(norm(v)) for v in M[i]])
        time_text.set_text(f"t = {times[i]}")
        return lc, time_text

    anim = FuncAnimation(fig, update, frames=T, interval=200, blit=False)
    out_filename,_ = heatmap_data_filename.split(".")
    out_path = f"{OUTPUT_DATA_DIR}/{out_filename}_animation.mp4"
    anim.save(out_path, writer="ffmpeg", fps=8)
    print(out_path)

    plt.close(fig)

def execute_script():
    parser = argparse.ArgumentParser()
    parser.add_argument("-i, --input", dest="input", default="simple_heatmap_matrix.csv")

    args = parser.parse_args()
    create_heatmap_animation(args.input)

if __name__ == "__main__":
    execute_script()
