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
    echo "[TEST_SCRIPT] Cleaning the build directory..."
    rm -rf build
fi

cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug -DJASM_TEST_MODE:BOOL=TRUE

if [ $generate == true ]; then
    echo "[BUILD_SCRIPT] Generate-only mode"
    exit
fi

cmake --build build

if [[ -f "build/bin/jasm" && $generate == false ]]; then
    if [ ! -d test ]; then
        mkdir test
    fi

    cp -rf "build/bin/jasm" "test/jasm"
    echo "[BUILD_SCRIPT] test/jasm has been copied"
fi
