@echo off

REM A simple build script
REM Check if CMake is available
cmake --version > nul 2>&1
if errorlevel 1 (
    echo CMake is not found in PATH.
    echo Please install CMake and add it to your system PATH.
    exit /b 1
)

REM Create build directory
if exist build (
    echo Deleting existing build directory...
    rd /s /q build
    if errorlevel 1 (
        echo Failed to delete build directory.
        exit /b 1
    )
)

md build
if errorlevel 1 (
    echo Failed to create build directory.
    exit /b 1
)

cd build
if errorlevel 1 (
    echo Failed to change directory to build.
    exit /b 1
)

REM Set environment variables for UTF-8 encoding
 chcp 65001 > nul
 set CMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE%
 set CC=cl.exe
 set CXX=cl.exe

 REM Run CMake to generate Visual Studio project with platform definition
 echo Running CMake to generate Visual Studio project...
 cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_CXX_FLAGS="/D _WIN32 /D AMOR_PLATFORM_WINDOWS /D WIN32_LEAN_AND_MEAN" -DCMAKE_VERBOSE_MAKEFILE=ON
if errorlevel 1 (
    echo CMake failed to generate project.
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed.
    exit /b 1
)

cd ..
echo Build completed successfully.