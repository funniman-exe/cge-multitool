# The cge7-193 Multitool
The cge7-193 Multitool is a tool created by funniman.exe to attempt to provide a user friendly interface for accessing information related to servers running on the Source Engine.
<br>
This tool was originally designed for use with the INTERLOPER ARG's "cge7-193" TF2 server (hence the multitool's name), but has since been expanded to support adding custom profiles with your own servers.

## Features
### Current features
- Custom Profiles
- User friendly FastDL access
- Scraping known FastDL files for changes
- Server information
- Server status
- Configuration via `prefs.json`

### Upcoming features
- Scraping for new/unknown Fastdl files
- Scraping Filters

## Building
### Prerequisites
- [MinGW](https://www.mingw-w64.org/downloads/) (The builds published on the releases page are compiled using [WinLibs MSVCRT Runtime](https://winlibs.com/#download-release)) (Ensure the `MinGW` `bin` folder is accessable via the `PATH`)
- The Windows operating system (reccomended: Windows 10+)
- [The PicoSHA2 Header-only SHA256 Library](https://github.com/okdshin/PicoSHA2/blob/master/picosha2.h) (place into the `thirdparty` folder)
- [The nlohmann JSON Parser](https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp) (place into the `thirdparty` folder)
- [BinaryAlien's libssq](https://github.com/BinaryAlien/libssq) (clone repo to the `thirdparty` folder)
- [CMake](https://cmake.org) (Ensure that `CMake` has been added to the `PATH`)

### Fixing libssq
The `libssq` library has a typo that must be fixed before it can be compiled.
Open the file `src/error.c`, and on line 36, remove the `,` from the end of the line.

~~You must also add the line `target_link_libraries(ssq ws2_32.lib)` after the `add_library(ssq)` line in `CMakeLists.txt`.~~ Unecessary? Requires more testing...

### Special libssq building instructions
You must Build libssq as follows (as opposed to as instructed in it's github repository):
```
cmake -B build -G "MinGW Makefiles"
cmake --build build
```

### Building
To build, run the following commands in your shell prompt:
```
cmake -B build -G "MinGW Makefiles"
cmake --build build
```
The `cge-multitool.exe` executable will be built in the directory previously specified.

## Isssues -- libssq
- ### Cannot open include file: 'stdbool.h'
Ensure that you are building using the specified build commands, and ensure that the `PATH` entry for the `MinGW` `bin` folder is placed above any Microsoft compilers (ex: `MSVC420`)

## Issues -- cge-multitool
- ### Crash when running info command
The cause for this error is not fully understood, as it seems to occur seemingly at random. This crash also seems to happen in two places: while loading the server info and while loading how many players are in the server.