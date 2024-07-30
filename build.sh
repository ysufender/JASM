#!/bin/bash

refresh=false
generate=false

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

if [ $generate == true ]; then
    echo "[BUILD_SCRIPT] Generate-only mode"
    exit
fi

mkdir -p "build/Debug"
cmake -S . -B build
cmake --build --preset DebugBuild
