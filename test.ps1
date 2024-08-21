param(
    [switch]$refresh,
    [switch]$generate
)

$ErrorActionPreference='Stop'
$pStart = 0

if ($refresh) {
    echo "[TEST_SCRIPT] Cleaning the build directory..."
    rm -r -Force build
}

if (Test-Path "build/Test") {
    echo "[TEST_SCRIPT] Already generated."
} else {
    echo "[TEST_SCRIPT] Generating build files..."
    cmake --preset Test
}

if ($generate) {
    echo "[TEST_SCRIPT] Generate-only mode."
    exit
}

cmake --build build --preset Test

if ($refresh) {
    $script:pStart = 2
}

cls
build/bin/Test/tests  $args[$pStart..-1]
