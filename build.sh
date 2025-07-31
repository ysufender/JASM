#!/bin/bash

set -e

refresh=false
generate=false
windows=false
release=false

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
        -R|--release)
            release=true
            shift
            ;;
        *)
            shift
            break
            ;;
    esac
done


if [[ $refresh == true && -d build ]]; then
    echo "[BUILD_SCRIPT] Cleaning the build directory..."
    rm -rf build
fi

preset="Debug"
if [ $windows == true ]; then
    preset="Debug-MinGW"
elif [ $release == true ]; then
    preset="Release"
fi

echo "[BUILD_SCRIPT] Generating build files."
cmake --preset $preset 

if [ $generate == true ]; then
    echo "[BUILD_SCRIPT] Generate-only mode"
    exit
fi

cmake --build build --preset $preset "$@"
