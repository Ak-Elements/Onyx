#!/bin/bash

# Define variables
sourceDir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
buildDir="$sourceDir/build"

# Check if cmake is installed
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed or not in the system PATH."
    exit 1
fi

# Prompt user for generator
echo "Select the CMake generator to use:"
echo "Common options:"
echo "1 - Unix Makefiles (default)"
echo "2 - Ninja"
echo "3 - Ninja Multi-Config"
echo "4 - Xcode (macOS only)"
echo "5 - Visual Studio 17 2022 (Windows only, cross-compiling)"
echo "Custom (enter a different generator name) "
read -p "Enter CMake generator [default: Unix Makefiles]: " userGenerator

# Map user input to corresponding generator, only if userGenerator is defined (not empty)
if [ -n "$userGenerator" ]; then
    case "$userGenerator" in
        1)
            generator="Unix Makefiles"
            ;;
        2)
            generator="Ninja"
            ;;
        3)
            generator="Ninja Multi-Config"
            ;;
        4)
            generator="Xcode"
            ;;
        5)
            generator="Visual Studio 17 2022"
            ;;
        *)
            # If the input is not a number between 1-5, use it as a custom generator name
            generator="$userGenerator"
            ;;
    esac
else
    # Default to Unix Makefiles if no input is given
    generator="Unix Makefiles"
fi

# Create build directory if it doesn't exist
mkdir -p "$buildDir"

# Run CMake
echo "Running CMake with generator: $generator"
cmake -S "$sourceDir" -B "$buildDir" -G "$generator" -D "ONYX_BUILD_EDITOR=ON"

# Check if CMake succeeded
if [ $? -eq 0 ]; then
    echo "CMake completed successfully. Build files are in '$buildDir'."
else
    echo "CMake encountered an error!"
    read -p "Press any key to resume ..."
    exit $?
fi