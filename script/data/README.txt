Helper data is stored here to be used by scripts managing the project.
    eg.: generating cmake projects or managing debug/release builds per platform

For any file present in the following dirs
    `common/`
    `debian_amd64/`
    `win64/`
    ... the file name gets looked up in `CMakeLists.txt.template`.
    The matched substring gets replaced by the file's contents during project generation.
        `common/`           concerns all platforms
        `<platform>/`     concerns only the set current platform

Target executable name is set in `common/__CFG_TARGET_EXECUTABLE_NAME__`
    This one is used by builder scripts too to identify the generated binary name.
