#!/bin/bash

rm -f ./main
rm -f ./out.txt

gcc -c ./include/Config.c -o ./include/Config.o
gcc ./src/Simulation.c -o ./test -I ./include/ ./include/Config.o -lGL -lglut -lm -lGLU -lpthread 

./test
