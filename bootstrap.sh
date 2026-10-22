#!/bin/bash

#find out script directory
pushd $(dirname "${BASH_SOURCE[0]}") > /dev/null
scriptdir=$(pwd)
popd > /dev/null

project_root="$scriptdir"

if [ "$EUID" -eq 0 ]; then
    echo "Don't run this as root."
    echo "  The script will clearly state what to run as sudo, if needed."
    exit 1
fi

pushd "$project_root" > /dev/null

# autoconfig results
cfg_system=""
arch=""
arch_forced=""

echo -n "Identifying system... "

# Windows + WSL
if grep -qiE '(microsoft|wsl)' /proc/sys/kernel/osrelease 2>/dev/null ||
   grep -qiE '(microsoft|wsl)' /proc/version 2>/dev/null; then
    echo "WSL"
    echo "  ERROR: WSL is not yet supported!"

# Debian
elif [ -f /etc/debian_version ]; then
    echo "Debian"
    cfg_system=debian
    cfg_arch=$( "${project_root}"/script/debian/detect_arch.sh )
    arch_forced=$( "${project_root}"/script/debian/detect_arch.sh force )

    if ! "${project_root}"/script/debian/validate_arch.sh $cfg_arch; then
        echo "Unknown architecture: '$arch_forced'"
        exit 1
    fi

    arch=$cfg_arch

    echo "Executing '${project_root}/script/debian_amd64/bootstrap_debian.sh'"
    "${project_root}"/script/debian_amd64/bootstrap_debian.sh $arch

# other
else
    echo "Unknown"
    echo "  Aborting due to unknown system."
    exit 1
fi

popd > /dev/null # from 'project_root'
