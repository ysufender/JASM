param(
    [switch]$refresh,
    [switch]$gen
)

if ($refresh) {
    rm -r -Force build
}

cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Debug

if (-not $gen) {
    cmake --build build
}

if (Test-Path "build/bin/jasm.exe"){
    cp  "build/bin/jasm.exe" "test/jasm.exe"
}