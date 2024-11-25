#!/bin/bash

make -C shared
make -C food
make -C warehouse
make -C main

./main/out/main.out