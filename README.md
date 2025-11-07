# Campus Evacuation DEVS Model and Simulation
[DEVS](https://en.wikipedia.org/wiki/DEVS) model and simulation of a university campus evacuation using [Cadmium](https://devssim.carleton.ca/). 

# Usage
To execute the simulation, run
```
source run.sh
```

You output will be in the directory [output_data/cleaned/](output_data/cleaned).

Your event log will look like this
```
timestamp;order_id;resource;activity
0;1;mes;enter
5;1;cell;end
5;2;mes;enter
10;2;cell;end
10;3;mes;enter
15;3;cell;end
15;4;mes;enter
20;4;cell;end
```

Your state log will look like this
```
timestamp;resource;state
0;shopFloor;idle
0;shopFloor;busy
5;shopFloor;idle
5;shopFloor;busy
10;shopFloor;idle
10;shopFloor;busy
15;shopFloor;idle
15;shopFloor;busy
20;shopFloor;idle
```

For how to supply your own order inputs, 
refer to the README file in the directory [input_data/](input_data/).

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
