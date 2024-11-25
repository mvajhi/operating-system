#!/bin/bash

make -C shared
make -C food
make -C warehouse
make -C main

rm -rf tmp 
mkdir tmp

./main/out/main.out