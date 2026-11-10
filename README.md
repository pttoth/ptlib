# ptlib

#### A library for some generic purpose tools.

### Alias

Class alias definitions to allow easier/friendlier usage of STL features.

### Config

Writes/reads key-value pairs into/from a file where the key is converted from enum values to strings and vice versa.

### Event

Class-independent event object, that can register and call functions sequentially in the order of registration.

### Logger

A 'tee'-like logger that writes simultaneously to the standard output and into a specified log file. The output file is UTF-8 encoded.

Currently it blocks the calling thread.
In later versions this will be moved out into a separate logging process.

### Utilities

Various generic-purpose functions, that simplify the usage of STL solutions to problems.

## How to build

#### WARNING

Although the project supports `Linux` and `Win64`, the development only supports Debian and WSL2 ( MinGW cross-compile to Win64).

On other Linux platforms, install the required dependencies, then use `script/build/sh`.

MSVC build on Windows is not supported, but should work, if done manually.

`cmake -G "Visual Studio 17 2022" -A x64 -S . -B build/msvc` .. or something like that, glhf.

---

Call `bootstrap.sh` (Debian, native or WSL2). It checks the required apt packages and builds for the current system: `win64` in WSL, `linux` on native Linux.

To build a specific platform: `script/build.sh [-u] [-d] [-p <platform>]` (-u: unity build, -d debug build)
- platforms: `linux` (native), `win64` (cross-compiled with MinGW-w64)
- output: `bin/<platform>/` (debug: `bin/<platform>_debug/`): `libptlib.a` and the test app `ptlib_test`
- headers: `include/`
