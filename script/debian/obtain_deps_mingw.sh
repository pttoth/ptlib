#!/bin/bash

# Sets up all required dependencies of the 'win64' (MinGW cross-compile) build.
#   runs the required fetch / build scripts

set -euo pipefail

dir_script="$(dirname -- "${BASH_SOURCE[0]}")"

pushd "$dir_script"/dep > /dev/null

echo "No required dependencies are defined."

popd > /dev/null
