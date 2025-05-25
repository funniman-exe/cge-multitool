# The cge7-193 Multitool
The cge7-193 Multitool is a tool created by funniman.exe to attempt to provide a user friendly interface for accessing information related to the cge7-193 Team Fortress 2 server.

## Features
### Current features
- User friendly Fastdl access
- Scraping known Fastdl files for changes
- TF2 Server information
- TF2 Server status

### Upcoming features
- Scraping for new/unknown Fastdl files
- Scraping Filters

## Building
### Prerequisites
- GNU Compilation Tools (gcc, g++)
- The Windows operating system (reccomended: Windows 10+)
- [The PicoSHA2 Header-only SHA256 Library](https://github.com/okdshin/PicoSHA2/blob/master/picosha2.h) (place into the `thirdparty` folder)
- [The nlohmann JSON Parser](https://github.com/nlohmann/json/releases/download/v3.12.0/json.hpp) (place into the `thirdparty` folder)
- [BinaryAlien's libssq](https://github.com/BinaryAlien/libssq/tree/main) (clone repo to the `thirdparty` folder, build dynamic library)
- [CMake](https://cmake.org) (Ensure that `CMake` has been added to the PATH)

### Fixing libssq
The `libssq` library has a typo that must be fixed before it can be compiled.
Open the file `src/error.c`, and on line 36, remove the `,` from the end of the line.

You must also add the line `target_link_libraries(ssq ws2_32.lib)` after the `add_library(ssq)` line in `CMakeLists.txt`.

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