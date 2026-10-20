#!/bin/bash

#find out script directory
pushd $(dirname "${BASH_SOURCE[0]}") > /dev/null
scriptdir=$(pwd)
popd > /dev/null

# config
buildtarget=release
platform=debian_amd64


if [ "$1" = "debug" ]; then
    buildtarget=debug
    platform=${platform}_debug
fi

fname_executable=$(cat "$scriptdir/data/common/__CFG_TARGET_EXECUTABLE_NAME__")
path_target_binary="${scriptdir}/../bin/${platform}/${fname_executable}"
builddir="./build/${platform}"


#get available core count
cores=$(nproc)


#move to project root directory
pushd "$scriptdir/.."


if [ "$buildtarget" = "debug" ]; then
    echo "Building debug target"
    cmake . -B$builddir -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug
    pushd "${builddir}"
    make -j $cores
    popd #from $builddir

else
    echo "Building release target"
    cmake . -B$builddir -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
    pushd "${builddir}"
    make -j $cores
    popd #from $builddir

    echo "Stripping binary '${path_target_binary}'"
    strip "${path_target_binary}"
fi

popd #from $scriptdir
