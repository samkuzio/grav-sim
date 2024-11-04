#!/bin/bash

make clean
make
valgrind --leak-check=full \
    ./grav-sim \
        -input tests/data/simulations/sun-and-earth-365-days.json \
        -output tests/data/outputs/test.sim \
        -timestep 86400 \
        -steps 365