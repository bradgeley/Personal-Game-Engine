# build-wasm.ps1
# Bradley Christensen - Automate Emscripten build

# Paths
$projectRoot = "C:\dev\Personal-Game-Engine"
$buildDir = "$projectRoot\build-wasm"

# Clean build directory
Write-Host "Cleaning build directory..."
if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir\*
} else {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Navigate to build directory
Set-Location $buildDir

# Configure the project with Emscripten
Write-Host "Running CMake configure..."
emcmake cmake -S $projectRoot -B . -G Ninja

# Build the project
Write-Host "Building project..."
cmake --build .
Write-Host "Build complete!"