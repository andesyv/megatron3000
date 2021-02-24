# Megatron 3000
## Development setup
### CMake
Using the CMake approach you can setup your project using the CMake configuration provided. The CMake project setup has some dependencies in terms of externally linked libraries, which you will have to manually provide the environment paths to yourself. With CMake, this boils down to supplying the path to the CMake project of the dependency to the *CMAKE_PREFIX_PATH*. Here is how to do this for different environment setups:

#### Visual Studio Code
If you're using the [CMake extension for visual studio code](https://marketplace.visualstudio.com/items?itemName=twxs.cmake) then you only has to set the CMake prefix path in that workspace. Simply add to *.vscode/settings.json* (create it if it doesn't exist) the key `cmake.configureEnvironment` with the property `CMAKE_PREFIX_PATH` followed by the path to your Qt installation. Example *.vscode/settings.json* file:
```json
{
    "cmake.configureEnvironment": {
        "CMAKE_PREFIX_PATH": "D:\\Qt\\6.0.1\\msvc2019_64"
    }
}
```