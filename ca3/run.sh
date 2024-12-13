#!/bin/bash

mkdir result

rm -rf ./shared/out
rm -rf ./serial/out
rm -rf ./parallel/out

make -C shared
make -C serial
make -C parallel

./serial/out/VoiceFilter.out
./parallel/out/VoiceFilter.out
