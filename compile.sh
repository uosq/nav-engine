#!/bin/env bash
set -x

if [[ "$1" == "imgui" ]]; then
    clang++ -o main *.cpp -lSDL3 -lm -DENABLEIMGUI && ./main
else
    clang++ -o main main.cpp -lSDL3 -lm && ./main
fi