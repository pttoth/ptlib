#!/bin/bash

# Sets up the project on Debian (native or WSL2) and builds it for the system it runs on.
#   - checks the architecture
#   - checks the required apt packages (prints the install command if any is missing)
#   - builds the release targets: 'win64' (cross-compiled) in WSL, 'linux' on native Linux

set -euo pipefail

project_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

if [ "$EUID" -eq 0 ]; then
    echo "Don't run this as root."
    echo "  The script will clearly state what to run as sudo, if needed."
    exit 1
fi

if [ ! -f /etc/debian_version ]; then
    echo "Unsupported system: only Debian (native or WSL2) is supported."
    echo "You're on your own."
    echo "  Find and install the dependencies on your system"
    echo "  Use 'script/debian/install_dependencies.sh' as reference"
    echo "  Use 'script/build.sh'"
    echo "  Take care!"
    exit 1
fi

if ! "${project_root}/script/debian/validate_arch.sh" "$( "${project_root}/script/debian/detect_arch.sh" )"; then
    echo "Unsupported architecture: '$( "${project_root}/script/debian/detect_arch.sh" --force )'"
    exit 1
fi

# --dry-run: only check, print the 'sudo apt install' command for missing packages
if ! "${project_root}/script/debian/install_dependencies.sh" --dry-run; then
    echo "Install the missing packages, then run this script again."
    exit 1
fi

# WSL: the kernel version contains 'microsoft'
if grep -qi microsoft /proc/sys/kernel/osrelease; then
    "${project_root}/script/debian/mingw_cross/fetch_and_build_boost.sh"
    "${project_root}/script/build.sh" -p win64
else
    "${project_root}/script/build.sh" -p linux
fi
