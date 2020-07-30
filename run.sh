#!/bin/bash

mkdir build

rm -f ./Simulation

gcc -c ./src/Config.c -I ./include/ -o ./build/Config.o 
gcc -c ./src/Simulation.c -I ./include/  -o ./build/Simulation.o
gcc ./build/Config.o ./build/Simulation.o -o ./build/Simulation -lGL -lglut -lm -lGLU -lpthread

./build/Simulation
