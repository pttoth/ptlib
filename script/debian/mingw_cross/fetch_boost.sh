#!/bin/bash

# Fetches the dependencies of the 'win64' (MinGW cross-compile) build into 'lib/win64_mingw/'.
#   boost (header-only, used: stacktrace)   ->  lib/win64_mingw/boost/include/boost/
#
# The archives are pinned by version and SHA256:
#   Downloaded archives are kept in 'dl_cache/'.
#   Re-runs reuse a downloaded archive if its hash matches (re-download otherwise),
#     and always re-extract it.

set -euo pipefail

dir_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/../../.." && pwd)"
dir_dl_cache="${dir_root}/dl_cache"

#--------------------------------------------------
# boost
#   checksum source: <url>.json  (official, published with the release)
#--------------------------------------------------
boost_version=1.83.0
boost_sha256=c0685b68dd44cc46574cce86c4e17c0f611b15e195be9848dfd0769a0a207628
boost_name="boost_${boost_version//./_}"
boost_url="https://archives.boost.io/release/${boost_version}/source/${boost_name}.tar.gz"
dir_boost="${dir_root}/lib/win64_mingw/boost"
timestamp=$(date '+%y%m%d_%H%M%S')

#--------------------------------------------------
# helpers
#--------------------------------------------------

# fnFetch <url> <sha256> <file>   -> downloads <url> into <file>, unless it's already there with matching checksum
fnFetch() {
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

#--------------------------------------------------
# boost: headers only
#--------------------------------------------------
archive="${dir_dl_cache}/${boost_name}.tar.gz"
fnFetch "$boost_url" "$boost_sha256" "$archive"

# always re-extracted: the result matches the archive, even if the dir was modified or is from an older version
# keep the previous one as backup: <dir>.bak_YYMMDD_HHMMSS
if [[ -e "$dir_boost" ]]; then
    echo "Backing up existing boost dir to 'boost.bak_${timestamp}'"
    echo
    printf '\e[93m'     # bright yellow
    echo "NOTE: Boost backups can be around 140-320 MB per backup!"
    echo "      Recommend manually deleting them in"
    echo "        '$(dirname "$dir_boost")'"
    echo
    printf '\e[0m'      # reset color
    mv -v "$dir_boost" "${dir_boost}.bak_${timestamp}"
fi

echo "Extracting boost headers into '${dir_boost#"$dir_root"/}'"
mkdir -p "${dir_boost}/include"
tar -xzf "$archive" -C "${dir_boost}/include" --strip-components=1 "${boost_name}/boost"
