#!/usr/bin/env bash

set -euo pipefail

pushd -- "$(dirname -- "${BASH_SOURCE[0]}")" > /dev/null
SCRIPT_DIR="$PWD"
popd > /dev/null

PATH_TEMPLATE="$SCRIPT_DIR/data/CMakeLists.txt.template"
PATH_OUTPUT="$SCRIPT_DIR/../CMakeLists.txt"



PLATFORM="${1:-debian_amd64}"
#PLATFORM="${1:-}"




if [[ -n "$PLATFORM" ]]; then
    echo "using default platform '$PLATFORM'"
fi


#--------------------------------------------------
# check target platform support
SUPPORTED_PLATFORMS=(
    "debian_amd64"
    "win64"
)

fnPrintSupportedPlatforms() {
    echo "supported platforms:"
    for platform in "${SUPPORTED_PLATFORMS[@]}"; do
        echo "  $platform"
    done
}

platform_supported=false


for current_platform in "${SUPPORTED_PLATFORMS[@]}"; do
    if [[ "$PLATFORM" == "$current_platform" ]]; then
        platform_supported=true
        break
    fi
done



if [[ "$platform_supported" == false ]]; then
    echo "Error: unknown platform '$PLATFORM'"
    fnPrintSupportedPlatforms
    exit 1
fi
#--------------------------------------------------



out_filecontents="$(cat -- "$PATH_TEMPLATE")"


fnExpandMacro() {
    local macro_path="$1"
    local macro_name
    local macro_value

    macro_name="$(basename "$macro_path")"

    # Read the entire macro file, stripping trailing newlines.
    macro_value="$(cat -- "$macro_path")"

    # To preserve trailing newlines instead:
    # IFS= read -r -d '' macro_value < "$macro_path" || true

    out_filecontents="${out_filecontents//"$macro_name"/"$macro_value"}"
}


echo "Generating project files for platform '$PLATFORM'..."

while IFS= read -r -d '' file; do
    fnExpandMacro "$file"
done < <(find "$SCRIPT_DIR/data/common" -type f -print0)


while IFS= read -r -d '' file; do
    fnExpandMacro "$file"
done < <(find "$SCRIPT_DIR/data/$PLATFORM" -type f -print0)


tmp="$PATH_OUTPUT.tmp"

printf '%s' "$out_filecontents" > "$tmp"
mv -- "$tmp" "$PATH_OUTPUT"

echo "done"

# TODO: delete
#echo "Generated: $PATH_OUTPUT"
