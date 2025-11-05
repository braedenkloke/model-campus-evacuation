# !/bin/sh

source build_sim.sh
./bin/manufacturing_system
source extract_devs_miner_logs.sh
echo "Done."
