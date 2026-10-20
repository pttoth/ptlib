#!/bin/bash

#find out script directory
pushd $(dirname "${BASH_SOURCE[0]}") > /dev/null
scriptdir=$(pwd)
popd > /dev/null

rootdir="$scriptdir/.."

platform=$1


echo "Creating startup script for platform '$platform'"


pushd "$rootdir" > /dev/null


# check platform parameter
if [ -z $platform ] ; then
    echo "No platform defined!"
    echo "  Choosing default: 'debian_amd64'"
    platform=debian_amd64
    #exit 1
fi


# verify that project is built for target platform
if [ ! -d "./bin/$platform" ] ; then
    echo "Directory does not exist: './bin/$platform/'"
    echo "  Build project first!"
    exit 1
fi


fname=$(cat "./script/data/common/__CFG_TARGET_EXECUTABLE_NAME__")



cat "./script/data/start.sh.template" \
    | tr '\n' '\r' \
    | sed "s/__CFG_TARGET_EXECUTABLE_NAME__/$fname/g" \
    | tr '\r' '\n' \
    > "./bin/$platform/start_$fname.sh"

chmod -v +x "./bin/$platform/start_$fname.sh"

popd > /dev/null
