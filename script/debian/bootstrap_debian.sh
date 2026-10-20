#!/bin/bash

#find out script directory
pushd $(dirname "${BASH_SOURCE[0]}") > /dev/null
scriptdir=$(pwd)
popd > /dev/null


if [ "$EUID" -eq 0 ]; then
    echo "Don't run this as root."
    echo "  The script will clearly state what to run as sudo, if needed."
    exit 1
fi

pushd "${scriptdir}" > /dev/null


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
../generate_project.sh debian_amd64
../build_debian_amd64.sh
../copy_startup_script.sh debian_amd64


popd > /dev/null # from 'scriptdir'
