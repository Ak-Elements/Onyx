@echo off
setlocal

REM Define variables
set "sourceDir=%~dp0"
set "buildDir=%sourceDir%build"
set "defaultGenerator=Visual Studio 17 2022"

REM Check if cmake is installed
where cmake >nul 2>&1
if errorlevel 1 (
    echo Error: CMake is not installed or not in the system PATH.
    exit /b 1
)

REM Display generator options
echo Select the CMake generator to use (default: "%defaultGenerator%"):
echo 1 - Visual Studio 17 2022 (default)
echo 2 - Ninja
echo Custom (enter a different generator name)

REM Prompt user for generator
set /p "userChoice=Enter the number corresponding to the CMake generator: "

REM Handle the user's choice
if "%userChoice%"=="1" (
    set "generator=Visual Studio 17 2022"
) else if "%userChoice%"=="2" (
    set "generator=Ninja"
) else if defined userChoice (
    REM If the user doesn't input a valid number, treat their input as the generator name
    set "generator=%userChoice%"
)

REM If no generator was selected, use the default one
if not defined generator (
    set "generator=%defaultGenerator%"
)

REM Run CMake
echo Running CMake to generate Visual Studio 2022 solution...
cmake -S %sourceDir% -B %buildDir% -G "%generator%" -D "ONYX_BUILD_EDITOR=ON"

REM Check if CMake succeeded
if %errorlevel% equ 0 (
    echo CMake completed successfully. Solution is in "%buildDir%".
) else (
    echo CMake encountered an error!
    exit /b %errorlevel%
)

endlocal