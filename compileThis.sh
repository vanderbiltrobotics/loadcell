#!/bin/bash

# Run this file when you want to compile this app.
# The output binary is 'weightsensor'. Run that to test it.
# - Josh P

g++ -o weightsensor -std=c++11 main.cpp HX711.cpp -lm -lpthread -lrt -lroboticscape
