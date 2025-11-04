#!/bin/env bash
set -x

mkdir build

rm -r build/*

cp -r Assets build/Assets

if [[ "$1" == "imgui" ]]; then
    clang++ -o build/main *.cpp -lSDL3 -lSDL3_image -lm -DENABLEIMGUI
else
    clang++ -o build/main main.cpp -lSDL3 -lSDL3_image -lm
fi

./build/main