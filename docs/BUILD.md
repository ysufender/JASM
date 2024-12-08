# Building JASM

## Using Scripts

You can use the [build.sh](../build.sh) (or [build.ps1](../build.ps1)) script if you don't want to manually tinker with cmake.
Be aware that these scripts build the project in debug mode. If you want to build in release mode
You'll have to deal with cmake directly.

### build.sh

build.sh <-r|--refresh> <-g|--generate>

-r|--refresh : Clean the build directory for a fresh start.
-g|--generate: Only generate build files. Do not build the project.

### build.ps1

build.ps1 <-refresh> <-generate>

The flags work the same as the shell script.

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
