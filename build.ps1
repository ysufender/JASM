param(
    [switch]$refresh
)

if ($refresh) {
    rm -r -Force build
}

cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++
cmake --build build
