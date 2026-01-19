# Campus Evacuation DEVS Model and Simulation
[DEVS](https://en.wikipedia.org/wiki/DEVS) model and simulation of a university campus evacuation using [Cadmium](https://devssim.carleton.ca/). 

# Usage
To execute the simulation, run
```
source build_sim.sh
./bin/scenario_01 
cat output_data/scenario_01_log.csv | grep "id=1,"
```

Your output should look similar to this
```
time,model_id,model_name,port_name,data
0,30,P3,exit,Vehicle{src=P3,id=1,dest=}
0,21,P3 & Raven Rd,out2,Vehicle{src=P3,id=1,dest=P3 & Raven Rd to Bronson Ave & Raven Rd}
9,8,P3 & Raven Rd to Bronson Ave & Raven Rd,exit,Vehicle{src=P3,id=1,dest=P3 & Raven Rd to Bronson Ave & Raven Rd}
```

For how to supply your own inputs, 
refer to the README file in the subdirectores of [input_data/](input_data/).

## Remote Simulation
Alternatively, you can run the simulation remotely on the [DEVSim servers](https://devssim.carleton.ca/). 

# Install
Instructions for installing this project's dependencies.

## Cadmium
Install Cadmium by running a [bootstrap script](https://github.com/braedenkloke/scripts-devssim),
or by following the instructions in the [Cadmium Installation Manual](https://devssim.carleton.ca/manuals/installation/).

Next, move this project folder to your Cadmium projects folder as such
```
cadmium-projects
|--cadmium_v2
|--model-campus-evacuation
```

Lastly, ensure you have your `CADMIUM` environment variable is set to your `cadmium_v2/include` directory
```
printenv | grep CADMIUM
```

## External libraries
Install external libraries by running
```
source install.sh
```

# Further Reading
- [Cadmium and the DEVSsim Server](https://devssim.carleton.ca/): Two tools for discrete-event modeling and simulation.
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [hackergrrl/art-of-readme](https://github.com/hackergrrl/art-of-readme)
- [The Turing Way: Naming files, folders and other things](https://book.the-turing-way.org/project-design/info-management/filenaming/)

# Acknowledgements
- [Gabriel Wainer](https://www.sce.carleton.ca/faculty/wainer/doku.php) and the [ARSLab](https://arslab.sce.carleton.ca/) 

# License
[MIT](https://choosealicense.com/licenses/mit/)
