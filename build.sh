#!/bin/bash

set -e

refresh=false
generate=false
windows=false

while test $# -gt 0; do
    case "$1" in
        -r|--refresh)
            refresh=true
            shift
            ;;
        -g|--generate)
            generate=true
            shift
            ;;
        -w|--windows)
            windows=true
            shift
            ;;
        *)
            shift
            break
            ;;
    esac
done

preset="Debug"

if [[ $refresh == true && -d build ]]; then
    echo "[BUILD_SCRIPT] Cleaning the build directory..."
    rm -rf build
fi

if [ $windows == true ]; then
    preset="Debug-MinGW"
fi

if [ -d build/Debug ]; then
    echo "[BUILD_SCRIPT] Already generated."
else
    echo "[BUILD_SCRIPT] Generating build files."
    cmake --preset $preset 
fi

if [ $generate == true ]; then
    echo "[BUILD_SCRIPT] Generate-only mode"
    exit
fi

cmake --build build --preset $preset "$@"
