#!/bin/bash

# Fetches the dependencies of the 'win64' (MinGW cross-compile) build into 'lib/win64_mingw/'.
#   boost (header-only, used: stacktrace)   ->  lib/win64_mingw/boost/include/boost/
#
# The archives are pinned by version and SHA256:
#   Downloaded archives are kept in 'dl_cache/'.
#   Re-runs reuse a downloaded archive if its hash matches (re-download otherwise),
#     and always re-extract it.

set -euo pipefail

source "$(dirname -- "${BASH_SOURCE[0]}")/shared.sh"
# inherited vars / functions:
#   dir_root
#   dir_dl_cache
#   timestamp
#   fnDownloadFile_SHA256

#--------------------------------------------------
# boost
#   checksum source: <url>.json  (official, published with the release)
#--------------------------------------------------
boost_version=1.83.0
boost_sha256=c0685b68dd44cc46574cce86c4e17c0f611b15e195be9848dfd0769a0a207628
boost_name="boost_${boost_version//./_}"
boost_url="https://archives.boost.io/release/${boost_version}/source/${boost_name}.tar.gz"
dir_boost="${dir_root}/lib/win64_mingw/boost"

#--------------------------------------------------
# boost: headers only
#--------------------------------------------------
archive="${dir_dl_cache}/${boost_name}.tar.gz"
fnDownloadFile_SHA256 "$boost_url" "$boost_sha256" "$archive"

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
