#!/bin/bash

# NOTE: 'fnPrintUsage' prints this header, so altering this requires updating 'fnPrintUsage' line references
# Creates the header and source files of a new class from
#   'script/data/newclass.h.model' and 'script/data/newclass.cpp.model',
#   then makes CMake reload the project.
#
# Usage:
#   script/create_class.sh [options] <ClassName>
#
#   Runs with the defaults below, without questions. Interactive mode (missing class name or -i)
#     asks for every value (Enter keeps the given parameter or default), then for confirmation.
#
#   <ClassName>                     class name (also used as file name)
#   -P, --platform <platform>       common | linux | win64  (default: common)
#   -p, --path <subdir>             sub directory inside 'include/' and 'src/' (default: pt)
#       --hpp                       '.hpp' header extension (default: '.h')
#   -d, --explicit-default          declare the special member functions as '= default'
#   -i, --interactive               interactive mode
#   -h, --help                      print this help
#
# Files are created in
#   common:     include/<subdir>/                       src/<subdir>/
#   platform:   include/platform/<platform>/<subdir>/   src/platform/<platform>/<subdir>/

set -euo pipefail

scriptdir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
rootdir="$(cd -- "$scriptdir/.." && pwd)"

PLATFORMS=( common linux win64 )

# defaults
classname=""
platform=common
subdir=pt
hpp=false
explicit_default=false
interactive=false

#--------------------------------------------------
# helpers
#--------------------------------------------------
fnPrintUsage() {
    # print the defined lines of this script
    sed -n '4,24p' "${BASH_SOURCE[0]}" | sed 's/^# \{0,1\}//'
}

# fnRead <prompt> <variable name>   -> exits, if there is no more input
fnRead() {
    if ! read -r -p "$1" "$2"; then
        echo
        echo "ERROR: no input"
        exit 1
    fi
}

# fnAskYesNo <question> <default: y|n>   -> returns 0 for yes
fnAskYesNo() {
    local question="$1"
    local default="$2"
    local hint="(y/N)"
    local answer

    [[ "$default" == "y" ]] && hint="(Y/n)"

    while true; do
        fnRead "$question $hint " answer
        answer="${answer:-$default}"
        case "$answer" in
            y|Y) return 0 ;;
            n|N) return 1 ;;
            *)   echo "invalid answer!" ;;
        esac
    done
}

# fnAsk <prompt> <variable name>   -> Enter keeps the variable's current value (shown as [default])
fnAsk() {
    local answer
    fnRead "$1 [${!2}]: " answer
    [[ -n "$answer" ]] && printf -v "$2" '%s' "$answer"
    return 0
}

# fnAskValid <prompt> <variable name> <check function>
#   -> asks until the check passes, an invalid answer does not become the default
fnAskValid() {
    local default="${!2}"
    fnAsk "$1" "$2"
    while ! "$3" "${!2}"; do
        echo "invalid value: '${!2}'"
        printf -v "$2" '%s' "$default"
        fnAsk "$1" "$2"
    done
}

fnIsValidName() {
    [[ "$1" =~ ^[A-Za-z_][A-Za-z0-9_]*$ ]]
}

fnIsValidPlatform() {
    local p
    for p in "${PLATFORMS[@]}"; do
        [[ "$1" == "$p" ]] && return 0
    done
    return 1
}

#--------------------------------------------------
# read params
#--------------------------------------------------
while [[ $# -gt 0 ]]; do
    case "$1" in
        -P|--platform)
            platform="${2:-}"
            shift
            ;;
        -p|--path)
            subdir="${2:-}"
            shift
            ;;
        --hpp)
            hpp=true
            ;;
        -d|--explicit-default)
            explicit_default=true
            ;;
        -i|--interactive)
            interactive=true
            ;;
        -h|--help)
            fnPrintUsage
            exit 0
            ;;
        -*)
            echo "Invalid parameter: '$1'"
            fnPrintUsage
            exit 1
            ;;
        *)
            classname="$1"
            ;;
    esac
    shift
done

#--------------------------------------------------
# interactive mode: ask for every value, the current one (parameter or default) is the default answer
#--------------------------------------------------
[[ -z "$classname" ]] && interactive=true

if [[ "$interactive" == true ]]; then
    fnAskValid "class name"                 classname   fnIsValidName
    fnAskValid "platform (${PLATFORMS[*]})" platform    fnIsValidPlatform

    hpp_answer=n
    [[ "$hpp" == true ]] && hpp_answer=y
    if fnAskYesNo "Use '.hpp' header extension?" "$hpp_answer"; then
        hpp=true
    else
        hpp=false
    fi

    # '/' stands for no path (shown as default instead of an empty []), normalized to "" below
    subdir="${subdir:-/}"
    fnAsk "path inside 'include/' and 'src/' ('/' for none)" subdir

    explicit_default_answer=n
    [[ "$explicit_default" == true ]] && explicit_default_answer=y
    if fnAskYesNo "Make functions explicit default?" "$explicit_default_answer"; then
        explicit_default=true
    else
        explicit_default=false
    fi
fi

#--------------------------------------------------
# validation
#--------------------------------------------------
header_ext=".h"
[[ "$hpp" == true ]] && header_ext=".hpp"

if ! fnIsValidName "$classname"; then
    echo "ERROR: invalid class name: '$classname'"
    exit 1
fi

if ! fnIsValidPlatform "$platform"; then
    echo "ERROR: invalid platform: '$platform' (${PLATFORMS[*]})"
    exit 1
fi

#--------------------------------------------------
# paths
#--------------------------------------------------
# normalize sub directory: no leading/trailing '/', no '..'
subdir="${subdir#/}"
subdir="${subdir%/}"
if [[ "/$subdir/" == *"/../"* ]]; then
    echo "ERROR: '..' is not allowed in path: '$subdir'"
    exit 1
fi

platform_dir=""
[[ "$platform" != "common" ]] && platform_dir="platform/$platform/"

# header path relative to the include directories
include_path="${subdir:+$subdir/}${classname}${header_ext}"

path_h="include/${platform_dir}${include_path}"
path_s="src/${platform_dir}${subdir:+$subdir/}${classname}.cpp"

for f in "$path_h" "$path_s"; do
    if [[ -e "$rootdir/$f" ]]; then
        echo "ERROR: '$f' already exists!"
        exit 1
    fi
done

echo "--------------------------------------------------"
echo "Class name:                  $classname"
echo "Platform:                    $platform"
echo "Header:                      $path_h"
echo "Source:                      $path_s"
echo "Explicit default functions:  $explicit_default"
echo "--------------------------------------------------"

if [[ "$interactive" == true ]] && ! fnAskYesNo "OK?" y; then
    echo "Aborted."
    exit 1
fi

#--------------------------------------------------
# create files

# with '= default', the special member functions get no definition in the .cpp
#   ('operator==' can't be defaulted before C++20, it is always defined)
def_default=""
[[ "$explicit_default" == true ]] && def_default=" = default"

mkdir -p "$(dirname "$rootdir/$path_h")" "$(dirname "$rootdir/$path_s")"

sed -e "s|__className__|$classname|g" \
    -e "s|__defDefault__|$def_default|g" \
    "$scriptdir/data/newclass.h.model" > "$rootdir/$path_h"

if [[ "$explicit_default" == true ]]; then
    # keep only the include and 'operator==' from the source model
    sed -n -e '1,/^__usingNamespaceName__/p' -e '/^__usingNamespaceName__/{s/.*//;p;p}' \
           -e '/^bool __className__::/,$p' \
        "$scriptdir/data/newclass.cpp.model"
else
    cat "$scriptdir/data/newclass.cpp.model"
fi \
    | sed -e "s|__headerInclude__|#include \"$include_path\"|g" \
          -e "s|__usingNamespaceName__|//using namespace ClassNameSpace;|g" \
          -e "s|__className__|$classname|g" \
    > "$rootdir/$path_s"

echo "Created '$path_h'"
echo "Created '$path_s'"

#--------------------------------------------------
# make CMake reload the project
#   CMakeLists.txt is a configure dependency: touching it makes the next build
#   (and IDEs watching it, e.g. QtCreator) re-run CMake, which re-globs the sources.
touch "$rootdir/CMakeLists.txt"

# reconfigure existing build directories right away
for cache in "$rootdir"/build/*/CMakeCache.txt; do
    [[ -e "$cache" ]] || continue
    builddir="$(dirname "$cache")"
    echo "Reloading CMake project in '${builddir#"$rootdir"/}'"
    cmake -S "$rootdir" -B "$builddir" > /dev/null
done
