#!/bin/bash

# Run this file when you want to compile this app.
# The output binary is 'weightsensor'. Run that to test it.
# - Josh P

g++ HX711.cpp -o weightsensor -std=c++11 -lm -lpthread -lrt -lroboticscape
