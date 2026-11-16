#!/bin/bash

# Shared settings and helpers of the 'script/debian/' scripts.

dir_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../.." && pwd)"

# downloads and git clones
dir_dl_cache="${dir_root}/dl_cache"

# temporary build directories (kept between runs, re-runs build incrementally)
dir_tmp_build="${dir_dl_cache}/build"
#dir_tmp_build=/tmp/pt_install/mingw

# win64 (MinGW cross-compile): '-posix' compiler, same as 'cmake/toolchain/win64-mingw.cmake'
toolchain_prefix=x86_64-w64-mingw32

timestamp=$(date '+%y%m%d_%H%M%S')

#--------------------------------------------------
# helpers
#--------------------------------------------------

# fnPrintHeader <title>   -> prints a visible header, separates the output of the scripts
fnPrintHeader() {
    printf '\n\e[1;96m'      # bold bright cyan
    echo "=================================================="
    echo "  $1"
    echo "=================================================="
    printf '\e[0m\n'       # reset color
}

# fnDetectArch   -> prints the system's architecture (dpkg), e.g. amd64
fnDetectArch() {
    dpkg --print-architecture 2>/dev/null || true
}

# fnValidateArch <arch>   -> succeeds if the project can be built on <arch>
#   amd64
fnValidateArch() {
    [[ "${1:-}" == "amd64" ]]
}

# fnDownloadFile_SHA256 <url> <sha256> <file>   -> downloads <url> into <file>, unless it's already there with matching checksum
fnDownloadFile_SHA256() {
    local url="$1"
    local sha256="$2"
    local file="$3"

    if [[ -f "$file" ]] && echo "${sha256}  ${file}" | sha256sum --check --status; then
        echo "Already downloaded: '${file#"$dir_root"/}'"
        return 0
    fi

    echo "Downloading '$url'"
    mkdir -p "$(dirname "$file")"
    curl -fL -o "${file}.part" "$url"

    if ! echo "${sha256}  ${file}.part" | sha256sum --check --status; then
        echo "ERROR: checksum mismatch for '$url'"
        echo "  expected: $sha256"
        echo "  got:      $(sha256sum "${file}.part" | cut -d ' ' -f 1)"
        rm -vf "${file}.part"
        exit 1
    fi

    mv -v "${file}.part" "$file"
}
