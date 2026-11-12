#!/bin/bash

# Sets up all dependencies of the 'win64' (MinGW cross-compile) build into 'lib/win64_mingw/'.
#   runs every fetch / build script of this directory

set -euo pipefail

dir_script="$(dirname -- "${BASH_SOURCE[0]}")"

"${dir_script}/fetch_boost.sh"
"${dir_script}/fetch_and_build_libbacktrace.sh"
