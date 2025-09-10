# Building JASM

## Using Scripts

You can use the [build.sh](../build.sh) (or [build.ps1](../build.ps1)) script if you don't want to manually tinker with cmake.
Be aware that these scripts build the project in debug mode. If you want to build in release mode
You'll have to deal with cmake directly.

### build.sh

`build.sh <-r|--refresh> <-g|--generate> <-R|--release>`

`-r|--refresh` : Clean the build directory for a fresh start.
`-g|--generate`: Only generate build files. Do not build the project.
`-R|--release` : Build in release mode, with all optimizations.

### build.ps1

`build.ps1 <-refresh> <-generate>`

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
            "CMAKE_CXX_STANDARD": "20",
            "CMAKE_CXX_STANDARD_REQUIRED": true,

            "CXX_COMPILER_NAME":"",
            "TOOLCHAIN_MODE": "OFF",
            "USE_PIPELINES_OPT": "OFF",
            "OUTPUT_PATH": ""
        }
    },
    {
        "name": "Release",
        "inherits": "default",
        "cacheVariables": {
            "CXX_COMPILER_NAME":"g++",
            "CMAKE_BUILD_TYPE": "Release",
            "OUTPUT_PATH": "Release",
            "CMAKE_CXX_FLAGS": "-O3 -flto -DNDEBUG",
        }
    },
    {
        "name": "Debug",
        "inherits": "default",
        "cacheVariables": {
            "CXX_COMPILER_NAME":"g++",
            "CMAKE_BUILD_TYPE": "Debug",
            "OUTPUT_PATH": "Debug"
            "CMAKE_CXX_FLAGS": "-O0 -g"
        }
    }
],
"buildPresets": [
    {
        "name": "Debug",
        "configurePreset": "Debug"
    },
    {
        "name": "Release",
        "configurePreset": "Release"
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
    CMAKE_BUILD_TYPE: Debug
    CMAKE_CXX_FLAGS: -O0 -g
    OUTPUT_PATH (Debug): Place resulting bins to build/Debug/

Release:
    CMAKE_BUILD_TYPE: Release
    CMAKE_CXX_FLAGS: -O3 -flto -DNDEBUG
    OUTPUT_PATH: Release
```
