#!/bin/bash

make -C shared
make -C serial
make -C parallel

./serial/out/serial.out
./parallel/out/parallel.out
