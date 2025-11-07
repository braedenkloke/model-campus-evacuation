# Campus Evacuation DEVS Model and Simulation
[DEVS](https://en.wikipedia.org/wiki/DEVS) model and simulation of a university campus evacuation using [Cadmium](https://devssim.carleton.ca/). 

# Usage
To execute the simulation, run
```
source run.sh
```

Your output should look like this
```
time,model_id,model_name,port_name,data
0,1,road,,0
0,2,parking lot,,3
0,1,road,,1
0,2,parking lot,exit,1
0,2,parking lot,,2
1,1,road,,2
...
14,1,road,exit,1
14,1,road,,0
14,1,road,,0
14,2,parking lot,,0
Done.
```

For how to supply your own inputs, 
refer to the README file in the subdirectores of [input_data/](input_data/).

You can configure the simulation by modifying [main/main.cpp](main/main.cpp).

## Remote Simulation
Alternatively, you can run the simulation remotely on the [DEVSim servers](https://devssim.carleton.ca/). 

# Install
Install Cadmium by running a [bootstrap script](https://github.com/braedenkloke/scripts-devssim),
or by following the instructions in the [Cadmium Installation Manual](https://devssim.carleton.ca/manuals/installation/).

Move this project folder to your Cadmium projects folder as such
```
cadmium-projects
|--cadmium_v2
|--model-campus-evacuation
```

Lastly, ensure you have your `CADMIUM` environment variable is set to your `cadmium_v2/include` directory
```
printenv | grep CADMIUM
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
