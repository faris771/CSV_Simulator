#!/bin/bash

# Clean previous builds

rm -r home

make clean

# Build all components
make all

# Build the drawer process separately
gcc -g drawer.c -o drawer -lglut -lGLU -lGL -lm -lpthread -lrt

# Run the main program
./main



