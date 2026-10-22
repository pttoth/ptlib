#!/bin/bash

#find out script directory
pushd $(dirname "${BASH_SOURCE[0]}") > /dev/null
scriptdir=$(pwd)
popd > /dev/null

project_root="$scriptdir/../.."

if [ "$EUID" -eq 0 ]; then
    echo "Don't run this as root."
    echo "  The script will clearly state what to run as sudo, if needed."
    exit 1
fi

#-------------------------
# read and validate architeture param
cfg_arch=$1
arch=""
if [ -z "$cfg_arch" ]; then
    cfg_arch=$( "${project_root}"/script/debian/detect_arch.sh )
    echo "No architecture defined, '$cfg_arch' detected"
fi

if ! "${project_root}"/script/debian/validate_arch.sh $cfg_arch; then
    echo "Unknown architecture: '$cfg_arch'"
    exit 1
fi

arch=$cfg_arch

#-------------------------


pushd "${project_root}/script/debian" > /dev/null


#--------------------------------------------------
# setups requiring root access

if [ "$EUID" -eq 0 ]; then
    # Running as root, can install

    if ! ./install_dependencies.sh ; then
        exit 1
    fi

else
    # running in userspace
    # --dry-run to avoid requiring admin rights
    if ! ./install_dependencies.sh --dry-run ; then
        exit 1
    fi

fi

#--------------------------------------------------
# setup in userspace
./obtain_ptlib.sh


# build project
../generate_project.sh debian_$arch
../build_debian_$arch.sh
../copy_startup_script.sh debian_$arch


popd > /dev/null # from 'scriptdir'
