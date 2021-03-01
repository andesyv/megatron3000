# Megatron 3000
## Development setup
The project uses two build systems that you can choose from when building: QMake and CMake.

### QMake
Definitively the easiest setup. Using either a Qt extension or Qt creator itself, configure the project using the `megatron3000.pro` file.

### CMake
The project is additionaly setup using CMake as an optional approach. The CMake project setup has some dependencies in terms of externally linked libraries, which you will have to manually provide the environment paths to yourself. With CMake, this boils down to supplying the path to the CMake project of the dependency to the *CMAKE_PREFIX_PATH* variable. You can either set this as an environment variable in your working environment or you can manually set the CMake variable for the project. Here is how to do this for some different environment setups:

#### Visual Studio
There are two ways of setting up a development environment in Visual Studio: using the CMake CLI or using the build-in CMake support in Visual Studio, the latter being the easiest.

**CLI**:

Using the CLI you can very simply generate a solution file using something like this:
```
cmake -G "Visual Studio 16 2019" -DCMAKE_PREFIX_PATH="D:\Qt\5.15.2\msvc2019_64"
```
Replace `Visual Studio 16 2019` with whatever version of Visual Studio you're using, and replace `D:\Qt\5.15.2\msvc2019_64` with the path to your Qt installation (for a specific Qt version and compiler combo). Building the solution as normal in Visual Studio will then build the project, but you will have to manually set up a debugging target and deployment on your own.

**CMake Extension**:

Visual Studio 2017 and later has build-in support for CMake. To use this you open up the editor, go to File -> Open -> CMake and then navigate to and select the root `CMakeLists.txt` in this project.

A sample `CMakeSettings.json` file is included in the project. The CMake extension uses this to correctly configure the CMake project for Visual Studio. In this file you will need to specify additional paths to any dependencies required by the project, like Qt. Just change the `cmakeCommandArgs` key to `"-DCMAKE_PREFIX_PATH=\"qt-installation-path"` where *qt-installation-path* is the full path to your Qt installation.

#### Visual Studio Code
If you're using the [CMake extension for visual studio code](https://marketplace.visualstudio.com/items?itemName=twxs.cmake) you can add custom settings to cmake for a particular workspace by adding it to the `.vscode/settings.json` file. Simply add the key `cmake.configureEnvironment` with the property `CMAKE_PREFIX_PATH` followed by the path to your Qt installation to the `.vscode/settings.json` file (create it if it doesn't exist). Example `.vscode/settings.json` file:
```json
{
    "cmake.configureEnvironment": {
        "CMAKE_PREFIX_PATH": "D:\\Qt\\5.15.2\\msvc2019_64"
    }
}
```