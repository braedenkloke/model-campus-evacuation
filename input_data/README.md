# Order Input Files
Input files of orders to be placed to system.

# Usage
Each line of an order input file corresponds to the integer time in seconds that an order should be placed to the system.

Examining the `default` order input file,
we observe that an order should be placed to the system at 0 seconds, 1 second, 2 seconds and 3 seconds.

The `default` order input file is arranged as such,

```
0
1
2
3
```

You can pass different order input files to the system as arguments.
For example,

```
./bin/smart_manufacturing_system no_orders.txt
```
