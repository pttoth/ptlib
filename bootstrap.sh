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

# autoconfig results
cfg_system=""
cfg_arch=""


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
    cfg_arch=$(dpkg --print-architecture 2>/dev/null)

    # amd64
    if [ "$cfg_arch" = "amd64" ]; then
        echo "Executing '${scriptdir}/script/debian_amd64/bootstrap_debian.sh'"
        "${scriptdir}"/script/debian_amd64/bootstrap_debian.sh

    # arm64
    elif [ "$cfg_arch" = "arm64" ]; then
        echo "This architecture is not supported: '$cfg_arch'"
        exit 1

    # other
    else
        echo "This architecture is not supported: '$cfg_arch'"
        exit 1
    fi

# other
else
    echo "Unknown"
    echo "  Aborting due to unknown system."
    exit 1
fi

popd > /dev/null # from 'scriptdir'
