#!/bin/bash

set -e

refresh=false
generate=false
args=("$@")
pStart=0

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
    echo "[TEST_SCRIPT] Cleaning the build directory..."
    rm -rf build
fi

if [ -d build/Test ]; then
    echo "[TEST_SCRIPT] Already generated."
else
    echo "[TEST_SCRIPT] Generating build files."
    cmake --preset Test
fi

if [ $generate == true ]; then
    echo "[TEST_SCRIPT] Generate-only mode"
    exit
fi

cmake --build --preset Test 

if [ $refresh == true ]; then
    pStart=$(pStart + 1)
fi

clear
build/bin/Test/tests "${args[@]:pStart}"
