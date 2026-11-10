## Add a class
Use `script/create_class.sh` (interactive, or see `script/create_class.sh --help`).

It creates the header and source files, then makes CMake reload the project:
`CMakeLists.txt` is touched (IDEs like QtCreator re-run CMake) and existing `build/*` directories are reconfigured.
