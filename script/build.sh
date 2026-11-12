#!/bin/bash

# NOTE: 'fnPrintUsage' prints this header, so altering this requires updating 'fnPrintUsage' line references
# Configures and builds the project for a target platform.
#
# Usage:
#   build.sh [-h|--help] [-u|-un] [-d|-rd] [-p platform]
#
#   -u,  --unity            unity build: source files are compiled in batches (faster full builds)
#   -un, --no-unity         no unity build (default), takes precedence over -u
#   -d,  --debug            debug build (default: release)
#   -rd, --relwithdebinfo   release build with debug info (-O2 -g), the last of -d / -rd wins
#   -p,  --platform <name>  linux | win64  (default: linux)
#   -h,  --help             print this help
#
# Build directory:  build/<platform>[_debug|_relwithdebinfo]/
# Output:           bin/<platform>[_debug|_relwithdebinfo]/  (libptlib.a, test app)

set -euo pipefail

dir_script="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
dir_root="$(cd -- "$dir_script/.." && pwd)"

fnPrintUsage() {
    # print the defined lines of this script
    sed -n '4,17p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
}

platform=linux
build_type=Release
build_suffix=""
unity_param_on=false
unity_param_off=false

#-------------------------
# read params
#-------------------------
while [[ $# -gt 0 ]]; do
    case "$1" in
        -p|--platform)
            if [[ $# -lt 2 ]]; then
                echo "Missing platform after '$1'"
                fnPrintUsage
                exit 1
            fi
            platform="$2"
            shift
            ;;
        -u|--unity)
            unity_param_on=true
            ;;
        -un|--no-unity)
            unity_param_off=true
            ;;
        -d|--debug)
            build_type=Debug
            build_suffix=_debug
            ;;
        -rd|--relwithdebinfo)
            build_type=RelWithDebInfo
            build_suffix=_relwithdebinfo
            ;;
        -h|--help)
            fnPrintUsage
            exit 0
            ;;
        *)
            echo "Invalid parameter: '$1'"
            fnPrintUsage
            exit 1
            ;;
    esac
    shift
done

# '-un' wins over '-u' (in any order), default: off
unity_build=OFF
if [[ "$unity_param_on" == true && "$unity_param_off" == false ]]; then
    unity_build=ON
fi

cmake_args=()

case "$platform" in
    linux)
        ;;
    win64)
        cmake_args+=( "-DCMAKE_TOOLCHAIN_FILE=${dir_root}/cmake/toolchain/win64-mingw.cmake" )
        ;;
    *)
        echo "Unknown platform: '$platform'"
        fnPrintUsage
        exit 1
        ;;
esac

#-------------------------


dir_build="${dir_root}/build/${platform}${build_suffix}"

echo "Building '${platform}' (${build_type}, unity: ${unity_build}) in '${dir_build}'"

# always pass CMAKE_UNITY_BUILD (ON or OFF): CMake caches it,
#   omitting it would keep the value of the previous configure
cmake -S "$dir_root" -B "$dir_build" -G "Unix Makefiles" \
      -DCMAKE_BUILD_TYPE="$build_type" -DCMAKE_UNITY_BUILD="$unity_build" "${cmake_args[@]}"

cmake --build "$dir_build" -j "$(nproc)"
