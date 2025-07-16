# Building JASM

## Using Scripts

You can use the [build.sh](../build.sh) (or [build.ps1](../build.ps1)) script if you don't want to manually tinker with cmake.
Be aware that these scripts build the project in debug mode. If you want to build in release mode
You'll have to deal with cmake directly.

### build.sh

build.sh <-r|--refresh> <-g|--generate> <-w|--windows>

-r|--refresh : Clean the build directory for a fresh start.
-g|--generate: Only generate build files. Do not build the project.
-w|--windows: If this flag is set, the script will invoke cmake with windows presets.

### build.ps1

build.ps1 <-refresh> <-generate>

The flags work the same as the shell script.

Be aware that the script will also use `CMakePresets.json` since it only invokes cmake.
Go to [CMakePresets](#CMakePresets) section for more info.

## Building Manually

I advise you to use this file structure while building:

```
./
|_build
 |_ <cmake build files> 
 |_ bin
  |_ Debug
  |_ Release
```

As for now the `CMakePresets.json` only contains presets for debug build but I'll add presets for release builds too. So
you'll be able to use the presets.

## CMakePresets

The default presets are given below:

```json
"configurePresets": [
    {
        "name": "default",
        "hidden": true,
        "generator": "Ninja",
        "binaryDir": "build",
        "cacheVariables": {
            "CMAKE_CXX_COMPILER": "g++",
            "CMAKE_EXPORT_COMPILE_COMMANDS": true,
            "CMAKE_CXX_STANDART": "20",
            "CMAKE_CXX_STANDART_REQUIRED": true,

            "CXX_COMPILER_NAME":"",
            "TOOLCHAIN_MODE": "OFF",
            "USE_PIPELINES_OPT": "OFF",
            "OUTPUT_PATH": ""
        }
    },
    {
        "name": "Debug",
        "inherits": "default",
        "cacheVariables": {
            "CXX_COMPILER_NAME":"g++",
            "CMAKE_BUILD_TYPE": "Debug",
            "OUTPUT_PATH": "Debug"
        }
    },
    {
        "name": "Debug-MinGW",
        "inherits": "default",
        "cacheVariables": {
            "CXX_COMPILER_NAME":"x86_64-w64-mingw32-g++",
            "CMAKE_CXX_COMPILER": "x86_64-w64-mingw32-g++",
            "OUTPUT_PATH": "Debug-MinGW"
        }
    }
],
"buildPresets": [
    {
        "name": "Debug",
        "configurePreset": "Debug"
    },
    {
        "name": "Debug-MinGW",
        "configurePreset": "Debug-MinGW"
    }
]
```

I don't advise changing the preset names, because you'll have to change the build scripts too
to match the new names. But feel free to change the variables I'll explain below (default
values are given between paranthesis).

```
default:
    generator (Ninja): As the name suggests, it's the Makefile generator you're using for cmake.
    binaryDir (build): Where the build files will be stored. Note that the resulting files will be under build/<name>/ and not build/
    CMAKE_CXX_COMPILER (g++): Pretty clear I suppose
    CMAKE_EXPORT_COMPILE_COMMANDS (true): For lsps (clangd) to work properly.
    TOOLCHAIN_MODE (OFF): Instead of building an executable, build a library to be linked. NOTE: This is not yet ready.
    USE_PIPELINES_OPT (OFF): Enables the pipelines option. It's still optional if you enable it but will be present. NOTE: This is not yet ready.

Debug:
    CXX_COMPILER_NAME (g++): To differ from Debug-MinGW 
    OUTPUT_PATH (Debug): Place resulting bins to build/Debug/

Debug-MinGW:
    CXX_COMPILER_NAME (x86_64-w64-mingw32-g++): I hate building for Windows. For some reason I wasn't able to build for windows on Linux.
    OUTPUT_PATH (Debug-MinGW):
```
