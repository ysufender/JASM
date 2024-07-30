param(
    [switch]$refresh,
    [switch]$generate
)

if ($refresh) {
    rm -r -Force build
}

cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug

if (-not $generate) {
    cmake --build build
}
