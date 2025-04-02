## 1. Obtain the Source Code

### Option 1: Download the ZIP File
You can download the source code directly from the GitHub repository as a ZIP file:

1. Go to the repository's GitHub page.
2. Click the **Code** button, then select **Download ZIP**.
3. Extract the downloaded ZIP file to a directory of your choice.

### Option 2: Clone the Repository with Git
If you prefer to use Git, clone the repository to your local system with the following command:

> - [**GitHub Desktop**](https://desktop.github.com/): A beginner-friendly Git client with a clean interface.
> - [**Sourcetree**](https://www.sourcetreeapp.com/): A powerful Git GUI client with advanced features.
> - [**TortoiseGit**](https://tortoisegit.org/): An easy-to-use Git client that integrates into Windows Explorer.
> - [**Tower**](https://www.git-tower.com/): A professional Git client with advanced features.

## 2. Build Instructions
Follow these steps to obtain configure, and build the Onyx engine.

### Generate Projects
For simple and quick generation of solution files, use the provided PowerShell or batch script located in the root of the repository:

-   PowerShell: generate_projects.ps1
-   Batch: generate_projects.bat
> **_NOTE:_** These scripts automatically configure and generate the required solution files.

### Configure the Build with CMake
Configure the Build with CMake
If you prefer manual configuration, use CMake commands to generate project files for Visual Studio 2022:

Example: Basic Configuration
Run the following command:

```bash
cmake -S . -B tmp/out/ -G "Visual Studio 17 2022" -A x64
```
Example: Editor Build
To build the Onyx Editor and include ImGui support, add the following flags to your CMake command:

```bash
cmake -DONYX_BUILD_EDITOR=ON -DONYX_USE_IMGUI=ON -S . -B tmp/out/ -G "Visual Studio 17 2022" -A x64
```

If you want to use GameInput add the following flag to enable it. (please ensure that you installed the required [Windows SDK version](#optional-for-gameinput-support)):

```bash
cmake -DONYX_BUILD_EDITOR=ON -DONYX_USE_IMGUI=ON -DONYX_USE_GAMEINPUT=ON -S . -B tmp/out/ -G "Visual Studio 17 2022" -A x64 
```

### 3. Build the Project

Navigate to the `build` directory and compile the project. Using Visual Studio:

1.  Open the generated solution file (`tmp/out/Onyx.sln`) in Visual Studio.
2.  Set the build configuration (Debug/Release).
3.  Build the solution by pressing `Ctrl+Shift+B` or selecting **Build > Build Solution** from the menu.

Alternatively, use CMake's command-line build tool:

```bash
cmake --build tmp/out/ --config Release
```

#### CMake
The compiled binaries can be found in the `tmp/out` directory. Navigate to the appropriate subdirectory (e.g., `tmp/out/editor/Release` or `tmp/out/editor/Debug`) and run the executable to start the Onyx engine.

### 4. Run Onyx

After building the project, the location of the compiled binaries depends on the method you used to generate and build the project:

#### Project Generation Scripts
When using the provided project generation scripts (`generate_projects.ps1` or `generate_projects.bat`), the build output is located in `/tmp/out/{project}/{configuration}/`.  
For example:
- Editor Debug build: `/tmp/out/editor/Debug`
- Engine Release build: `/tmp/out/editor/Release`

#### Running Onyx from Visual Studio
If you are using Visual Studio, you can run Onyx directly by pressing **F5**:
1. Open the generated solution file in Visual Studio.
2. Ensure that the correct project (e.g., `onyx-editor`) is set as the startup project.
3. Press **F5** to build and run the selected project.  
   The working directory is already configured correctly by CMake, so no additional setup is required.

#### Running Onyx Directly from the Build Folder
If you choose to run the Onyx executable directly from the build output directory (e.g., `tmp/out/editor/Debug`), you must set the working directory correctly for the engine to locate necessary resources and assets.

The working directory must be set to the **project root directory**, which is the same level as the `tmp` folder. Here’s how to ensure the working directory is set properly:

1. **Command Line**:
   - Navigate to the project root directory:
     ```bash
     cd `/path/to/Onyx`
     ```
   - Run the executable using its relative path:
     ```bash
     tmp/out/editor/Debug/onyx-editor.exe
     ```

2. **Shortcut or Script**:
   - Create a shortcut to the executable.
   - Right-click the shortcut and select **Properties**.
   - In the **Start in** field, specify the path to the project root directory (e.g., `C:\path\to\Onyx`).
   - Use the shortcut to launch the executable.

3. **File Explorer**:
   - Open a terminal (Command Prompt or PowerShell) from the project root directory.
   - Run the executable using its relative path, for example:
     ```bash
     .\tmp\out\editor\Debug\editor.exe
     ```

> **_NOTE:_**  The working directory is critical for the engine to function correctly as it resolves relative paths for assets and configurations.
> If you encounter errors due to missing assets, verify that the working directory is set to the project root and the required files exist in the `tmp` folder. 

----------

## Optional for GameInput support  
   - **Windows SDK Version**: `10.0.26100`  
   - **Installation**: Use the Visual Studio Installer (instructions below).  
   - [Download Windows SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/)

#### Installing Windows SDK 10.0.26100 Using Visual Studio Installer

To install the Windows SDK version `10.0.26100` for GameInput support, follow these steps:

1. **Launch the Visual Studio Installer**  
   - Open the **Start Menu** and search for `Visual Studio Installer`.  
   - Click on the Visual Studio Installer to open it.

2. **Modify Your Visual Studio Installation**  
   - In the Visual Studio Installer, locate your installed Visual Studio version (e.g., Visual Studio 2022).  
   - Click on the **Modify** button.

3. **Select the Required Workloads**  
   - In the **Workloads** tab, ensure you have selected the following workload:  
     - **Desktop development with C++**

4. **Add the Windows SDK Version 10.0.26100**  
   - Switch to the **Individual components** tab.  
   - Scroll down to the **SDKs, libraries, and frameworks** section.  
   - Look for **Windows 11 SDK (10.0.26100)** and check the box.

5. **Install the Selected Components**  
   - Click the **Modify** button to install the selected components.  
   - Wait for the installation to complete.

----------

## Common Issues

-   **Missing Vulkan SDK or shaderc libraries**:  
    Ensure the Vulkan SDK installation includes both 32-bit and 64-bit tool libraries. If the problem persists, reinstall the Vulkan SDK.
    
-   **CMake version mismatch**:  
    Confirm that your CMake version is 3.2 or higher by running:
    
    ```bash
    cmake --version
    ``` 
    
-   **Compiler compatibility**:  
    Ensure your compiler supports C++20 or higher. Update your compiler if needed.
    
-   **Verify GameInput / Windows SDK Installation**  
    - Open Visual Studio.  
    - Navigate to **Tools > Options > Projects and Solutions > VC++ Directories**.  
    - Confirm that the SDK version `10.0.26100` is available.