# Onyx Engine Installation Guide

## Introduction
Onyx is a Vulkan-based C++ engine designed for high-performance graphics rendering. This guide outlines the requirements and step-by-step instructions for building Onyx on Windows.

---

## Requirements & Dependencies
To successfully build and run Onyx, ensure the following software and tools are installed:

### Required Tools
1. **CMake**  
   - **Version**: `3.28` or higher  
   - [Download CMake](https://cmake.org/download/)

2. **Vulkan SDK**  
   - **Version**: `1.3` or higher  
   - **Additional Requirements**:  
     During Vulkan SDK installation, ensure the following components are selected:  
     - **SDK 32-bit Core Components**  
     - **Shader Toolchain Debug Symbols 64-bit**  
     - **Shader Toolchain Debug Symbols 32-bit**  
   - [Download Vulkan SDK](https://vulkan.lunarg.com/sdk/home)

3. **C++ Compiler**  
   - **Standard**: `C++20` or higher  
   - **Examples**:  
     - Microsoft Visual Studio
         - The free [Community Edition](https://visualstudio.microsoft.com/vs/community/) includes the MSVC compiler with C++20 support.
      - GCC (GNU Compiler Collection)
         - Install GCC 11+ from the [GCC Installation Guide](https://gcc.gnu.org/install/).
         - Windows users can install GCC via [WSL](https://code.visualstudio.com/docs/cpp/config-wsl) for a Linux-like environment.

---

## Obtain the Source Code

### Option 1: Download the ZIP File
You can download the source code directly from the GitHub repository as a ZIP file:

1. Go to the repository's GitHub page.
2. Click the **Code** button, then select **Download ZIP**.
3. Extract the downloaded ZIP file to a directory of your choice.

### Option 2: Clone the Repository with Git
If you prefer to use Git, clone the repository to your local system with the following command:

```bash
git clone <repository-url> Onyx
cd Onyx
```
> **_TIP:_** If you are unfamiliar with the command line, you can use one of the Git clients for your platform.
> Refer to the platform-specific build instructions for further details on those.

## Build Instructions
Follow the steps to configure, and build the Onyx engine depending on your platform.

- [Windows Build Instructions](docs/windows_build.md)
- [Linux Build Instructions](docs/linux_build.md)
- [macOS Build Instructions](docs/macos-build.md)

## Additional Notes

-   A GPU with Vulkan support is required.
-   Contributions and issues can be tracked on the repository's GitHub page.

## License

Required Notice: Copyright AkElements
This repository is licensed under the PolyForm Noncommercial License 1.0.0
https://polyformproject.org/licenses/noncommercial/1.0.0/