# Manufacturing System DEVS Model and Simulation
[DEVS](https://en.wikipedia.org/wiki/DEVS) model and simulation of a manufacturing system using [Cadmium](https://devssim.carleton.ca/).

# Background

## Manufacturing System
The modeled system is based off the smart manufacturing system studied by [Friederich and Lazarova-Molnar (2024)](https://journals.sagepub.com/doi/full/10.1177/00375497241302866?casa_token=TZrSVjFZ1-YAAAAA%3AI2L_IkiDduYKh8NvE_x07_OGoQNQjjB3_x00TFNuzlrwdu8NNnzc1HG7uaIE3aYi7RN6gynVvqpXbQ).

The sytem consists of a Customer, 
a [manufacturing execution system (MES)](https://en.wikipedia.org/wiki/Manufacturing_execution_system),
and a ShopFloor.
The Customer places orders to the system.
The MES handles new orders and directs them to a line on the ShopFloor.
The ShopFloor consists of a single line with an assembly cell which assembles the orders.
Both the MES and Customer are considered as stateless.

Future work would consist of expanding the ShopFloor to have multiple production lines, 
autonomous ground vehicles (AGVs),
and components with failure states.

# Usage
With [Cadmium](https://devssim.carleton.ca/) installed, run
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

# Acknowledgements
- [Sanja Lazarova-Molnar](https://lazarova-molnar.net/) and the [SYDSEN Research Group](https://sydsen.aifb.kit.edu/) 
- [Gabriel Wainer](https://www.sce.carleton.ca/faculty/wainer/doku.php) and the [ARSLab](https://arslab.sce.carleton.ca/) 
- [Mitacs](https://www.mitacs.ca/our-programs/globalink-research-award/)

# See Also
- [DEVS-miner](https://github.com/braedenkloke/devs-miner)
- [Friederich, J. (2023). *Data-Driven Assessment of Reliability for Cyber-Physical Production Systems*.](https://portal.findresearcher.sdu.dk/en/publications/data-driven-assessment-of-reliability-for-cyber-physical-producti)
- [Cadmium Project Template](https://github.com/Sasisekhar/blank_project_rt)
- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [hackergrrl/art-of-readme](https://github.com/hackergrrl/art-of-readme)

# License
[MIT](https://choosealicense.com/licenses/mit/)
