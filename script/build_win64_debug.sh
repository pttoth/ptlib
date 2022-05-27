#!/bin/bash

#find out script directory
pushd $(dirname "${BASH_SOURCE[0]}") > /dev/null
scriptdir=$(pwd)
popd > /dev/null

#move to project root directory
pushd $scriptdir/..

cmake ./projects/win64 -Bbuild -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug

cd build

#get available core count
cores=$(nproc)

make -j $cores

cd ..

popd
