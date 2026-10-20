#!/bin/bash

# --dry-run
#   Don't run the install command, just output it at the end

# --no-check
#   Supply all packages in the install command,
#     regardless of them being already installed.

#--------------------------------------------------
# config (manual)
packages=""
packages="$packages git"
packages="$packages git-lfs"
packages="$packages cmake"
packages="$packages build-essential"

#--------------------------------------------------
# config (auto-set)

params_forwarded=""

#--------------------------------------------------
#find out script directory
pushd $(dirname "${BASH_SOURCE[0]}") > /dev/null
scriptdir=$(pwd)
popd > /dev/null


# read params
while [[ $# -gt 0 ]]; do
    case "$1" in
        --dry-run)
            params_forwarded="$params_forwarded $1"
            ;;
        --no-check)
            params_forwarded="$params_forwarded $1"
            ;;
        *)
            ;;
    esac
    shift
done


"${scriptdir}/apt_installer.sh" $params_forwarded --packages $packages
errorcode=$?

exit $errorcode
