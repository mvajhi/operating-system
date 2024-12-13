#!/bin/bash

rm -rf result
mkdir result

make -C shared
make -C serial
make -C parallel

./serial/out/VoiceFilter.out
./parallel/out/VoiceFilter.out
