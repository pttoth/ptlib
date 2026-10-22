#!/bin/bash

# syntax:
#   apt_installer.sh [--dry-run] [--no-check] --packages <package> [<package> ...]

# --dry-run
#   Don't run the install command, just output it at the end

# --no-check
#   Supply all packages in the install command,
#     regardless of them being already installed.

#--------------------------------------------------
# config (manual)
packages=""

#--------------------------------------------------
# config (auto-set)

step_Install=true
step_Check=true

all_installed=true

packages_missing=""

errorcode=0

#--------------------------------------------------
# read params
if [[ $# -eq 0 ]] ; then
    echo "Usage:"
    echo "  apt_installer.sh [--dry-run] [--no-check] --packages <package> [<package> ...]"
    echo
    echo "  --dry-run"
    echo "      Don't run the install command, just output it at the end."
    echo
    echo "  --no-check"
    echo "      Install all packages, regardless of them being already installed."
    exit 0
fi


while [[ $# -gt 0 ]]; do
    case "$1" in
        --dry-run)
            step_Install=false
            ;;
        --no-check)
            step_Check=false
            ;;
        --packages)
            shift
            packages="$*"
            break
            ;;
        *)
            echo "Invalid parameter: $1"
            exit 1
            ;;
    esac
    shift
done

# TODO: disable
# debug
#$step_Install || echo "--dry-run"
#$step_Check   || echo "--no-check"

#--------------------------------------------------
# check whether packages are installed
if $step_Check == "true"; then
    if [ -z "$packages" ] ; then
        echo "Nothing to install."
        exit 0
    fi
    
    echo "Checking installed packages..."
    for package in $packages; do
        if dpkg-query -W -f='${Status}' "$package" 2>/dev/null | grep -q "install ok installed"; then
            echo "[ OK ] $package"
        else
            echo "[    ] $package"
            packages_missing="$packages_missing $package"
            all_installed=false
        fi
    done

    if $all_installed == "true"; then
        # success, all installed
        exit 0
    else
        # packages missing
        errorcode=1
    fi

else
    # check was skipped
    packages_missing=$packages
fi


#--------------------------------------------------
# install packages
command="apt install $packages_missing -y"

if $step_Install == "true"; then

    echo "installing packages:"
    echo "  $packages_missing"
    echo
    echo "executing command:"
    echo "  $command"

    $command
    errorcode=$?
else
    echo "install packages with:"
    echo "  sudo $command"
fi

exit $errorcode
