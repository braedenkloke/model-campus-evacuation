# Campus Evacuation DEVS Model and Simulation
[DEVS](https://en.wikipedia.org/wiki/DEVS) model and simulation of a university campus evacuation using [Cadmium](https://devssim.carleton.ca/). 

## Usage
To interactively create a scenario and execute a simulation, run
```
python integration.py
```

Alternatively, to execute a simulation independently, run
```
source build_sim.sh
./bin/campus-evacuation | grep "id=1,"
```

Your output should look similar to this
```
time,model_id,model_name,port_name,data
0,30,P3,exit,Vehicle{src=P3,id=1,dest=}
0,21,P3 & Raven Rd,out2,Vehicle{src=P3,id=1,dest=P3 & Raven Rd to Bronson Ave & Raven Rd}
9,8,P3 & Raven Rd to Bronson Ave & Raven Rd,exit,Vehicle{src=P3,id=1,dest=P3 & Raven Rd to Bronson Ave & Raven Rd}
```

To reference the simulation options, run 
```
./bin/campus-evacuation --help
```

To execute all scenarios as a batch, run
```
source run_scenarios.sh
```

To render an animation of simulation, run
```
python analysis/create_heatmap_animation.py
```

## Install
Instructions for installing this project's dependencies.

* Cadmium: Install Cadmium by following the instructions in the [Cadmium Installation Manual](https://devssim.carleton.ca/manuals/installation/).
* External C libraries: Run, `install.sh`
* External Python libraries: Create a Python virtual environment and run, `pip install -r requirements.txt`
* [FFmpeg](https://ffmpeg.org/): Install according to your OS

## Further Reading
- [Cadmium and the DEVSsim Server](https://devssim.carleton.ca/): Two tools for discrete-event modeling and simulation.
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [hackergrrl/art-of-readme](https://github.com/hackergrrl/art-of-readme)
- [The Turing Way: Naming files, folders and other things](https://book.the-turing-way.org/project-design/info-management/filenaming/)

## Acknowledgements
- [Gabriel Wainer](https://www.sce.carleton.ca/faculty/wainer/doku.php) and the [ARSLab](https://arslab.sce.carleton.ca/) 

## License
[MIT](https://choosealicense.com/licenses/mit/)
