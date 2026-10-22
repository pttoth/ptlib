# Setup Debian build environment

## Dependencies

This tutorial will guide you through installing the following tools and compiling the following libraries.

### Tools

#### Apt package dependencies
- git
- cmake
- build-essential

### Additional libraries
- ptlib (https://github.com/pttoth/ptlib)

# Setup steps

- install system dependencies
  * (sudo) run `script/debian/install_dependencies.sh`

- download and build project dependencies
  * run `script/debian/obtain_ptlib.sh`

- build project
  * run `script/build_debian.sh` or `script/build_debian_debug.sh`

- the built binaries can be found in `bin/<platform>/` or `bin/<platform>_debug/`
- recommend using the startup script `bin/<platform>/start_<projectname>.sh` next to the binary for launching (has workarounds so that install is not required)
