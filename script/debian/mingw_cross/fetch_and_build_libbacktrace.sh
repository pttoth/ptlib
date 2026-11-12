#!/bin/bash

# Fetches and cross-compiles libbacktrace for the 'win64' (MinGW cross-compile) build.
#   boost.stacktrace uses it for function names (and file:line with debug info) in stacktraces
#   ->  lib/win64_mingw/libbacktrace/{include,lib}/

set -euo pipefail


source "$(dirname -- "${BASH_SOURCE[0]}")/shared.sh"
# inherited vars / functions:
#   dir_root
#   dir_dl_cache
#   dir_tmp_build
#   timestamp
#   toolchain_prefix

#--------------------------------------------------
# libbacktrace
#   no official releases: pinned to a commit of the repository (GCC's libbacktrace, mirrored)
#   version: commit hash, branch or tag
#     commit hash: fixed content
#     branch/tag:  re-runs pick up its updates
#--------------------------------------------------
version=0b9b49cf4a2c9229fc052d6716e1528b2f23e91a    # 2026-09-03
repo_url="https://github.com/ianlancetaylor/libbacktrace"
dir_lbt_repo="${dir_dl_cache}/libbacktrace"
dir_lbt_build="${dir_tmp_build}/libbacktrace"
dir_lbt="${dir_root}/lib/win64_mingw/libbacktrace"

#--------------------------------------------------
# libbacktrace: clone / update, check out 'version'
#--------------------------------------------------
if [[ -d "${dir_lbt_repo}/.git" ]]; then
    echo "Already cloned: '${dir_lbt_repo#"$dir_root"/}'"
else
    git clone "$repo_url" "$dir_lbt_repo"
fi

pushd "$dir_lbt_repo" > /dev/null
    # Since 'version' can be a hash, a tag or a branch
    #   AND the defined version could change, while the repo was cached
    #   git behavior could fail multiple ways here, depending on the 'version' value
    # 'git pull' fails when not on a branch (checked out commit hash or tag),
    #   so: fetch all updates (--force: moved tags too), check out, then pull if on a branch
    # Local, uncommitted changes fail update, which is good.
    # All other failure cases should be remote-related repo management issues.
    git fetch --tags --force origin
    git checkout "$version"
    if git symbolic-ref --quiet HEAD > /dev/null; then
        git pull --ff-only
    fi
popd > /dev/null

#--------------------------------------------------
# libbacktrace: build and install
#   out-of-tree build in 'dir_lbt_build', the clone stays untouched
#--------------------------------------------------
mkdir -p "$dir_lbt_build"

# keep the previous one as backup: <dir>.bak_YYMMDD_HHMMSS
if [[ -e "$dir_lbt" ]]; then
    echo "Backing up existing libbacktrace dir to 'libbacktrace.bak_${timestamp}'"
    mv -v "$dir_lbt" "${dir_lbt}.bak_${timestamp}"
fi

echo "Building libbacktrace into '${dir_lbt#"$dir_root"/}'"
pushd "$dir_lbt_build" > /dev/null
    "${dir_lbt_repo}/configure" --host="$toolchain_prefix" CC="${toolchain_prefix}-gcc-posix" \
                 --prefix="$dir_lbt" --disable-shared > configure.log 2>&1 \
        || { cat configure.log; exit 1; }
    make -j "$(nproc)" > make.log 2>&1 || { cat make.log; exit 1; }
    make install > install.log 2>&1 || { cat install.log; exit 1; }
popd > /dev/null

# libtool leftover, not needed
rm -vf "${dir_lbt}/lib/libbacktrace.la"

echo "Done: '${dir_lbt#"$dir_root"/}'"
