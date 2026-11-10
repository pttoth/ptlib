# Setup Debian build environment

Works on native Debian and on Debian in WSL2.

## Dependencies

This tutorial will guide you through installing the following tools.

### Tools

#### Apt package dependencies
- git
- git-lfs
- cmake
- build-essential
- libboost-stacktrace-dev
- g++-mingw-w64-x86-64-posix (win64 cross-compiler)

# Setup steps

- install system dependencies
  * (sudo) run `script/debian/install_dependencies.sh`

- build project
  * run `script/build.sh -p linux` or `script/build.sh -p win64` ( `-u` unity build, `-d` debug build )

- the built library (`libptlib.a`) and test app can be found in `bin/<platform>/` or `bin/<platform>_debug/`
- recommend using the startup script `bin/linux/start_<executable>.sh` next to the binary for launching (has workarounds so that install is not required)
