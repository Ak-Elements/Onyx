# Define variables
$sourceDir = $PSScriptRoot  # The directory where the script is located (assuming it's the source root)
$buildDir = Join-Path $sourceDir "build"  # The output directory for the VS solution
$defaultGenerator = "Visual Studio 17 2022"  # Default generator

# Check if cmake is installed
if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
    Write-Host "Error: CMake is not installed or not in the system PATH." -ForegroundColor Red
    exit 1
}

# Prompt user for generator
Write-Host "Select the CMake generator to use (default: $defaultGenerator):"
Write-Host "Common options:"
Write-Host "1 - Visual Studio 17 2022 (default)"
Write-Host "2 - Ninja"
Write-Host "Custom (enter a different generator name)"
$generatorChoice = Read-Host "Enter CMake generator or a custom generator name."
if ($generatorChoice)
{
     # Map number choices to generator names
    switch ($generatorChoice)
    {
        "1" { $generator = "Visual Studio 17 2022" }
        "2" { $generator = "Ninja" }
        default
        { 
            $generator = $generatorChoice  # If input is not a valid number, treat it as a generator name
        }
    }
}
else
{
    $generator = $defaultGenerator
}

if (-not (Get-Command cmake -ErrorAction SilentlyContinue))
{
    Write-Host "Error: CMake is not installed or not in the system PATH." -ForegroundColor Red
    exit 1
}

# Ensure build directory exists
if (!(Test-Path $buildDir)) {
    New-Item -ItemType Directory -Path $buildDir | Out-Null
}

# Run CMake
Write-Host "Running CMake to generate Visual Studio 2022 solution..."
cmake -S "`"$sourceDir`"" -B "`"$buildDir`"" -G "`"$generator`"" -D "ONYX_BUILD_EDITOR=ON"

# Check if CMake succeeded
if ($LASTEXITCODE -eq 0) {
    Write-Host "CMake completed successfully. Solution is in '$buildDir'."
} else {
    Write-Host "CMake encountered an error!" -ForegroundColor Red
    exit $LASTEXITCODE
}