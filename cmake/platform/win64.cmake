# Windows (MinGW cross-build) specific settings and library dependencies.
#   Included by CMakeLists.txt, after the targets are defined.
#   Targets: ${LIB_NAME} (library), ${TEST_APP_NAME} (test app)

# See help: cmake/platform/help.win64.txt
#--------------------------------------------------

# boost (header-only), from 'script/debian/mingw_cross/fetch_boost.sh'
#   the apt package's headers in '/usr/include' are only for the native compiler
set( DIR_BOOST ${PROJECT_SOURCE_DIR}/lib/win64_mingw/boost/include )
if( NOT EXISTS ${DIR_BOOST}/boost/stacktrace.hpp )
    message( FATAL_ERROR "boost is missing from '${DIR_BOOST}', run 'script/debian/mingw_cross/fetch_boost.sh'" )
endif()
target_include_directories( ${LIB_NAME} SYSTEM PRIVATE
    ${DIR_BOOST}
)

# boost.stacktrace: function names (and file:line with debug info) via libbacktrace
#   from 'script/debian/mingw_cross/fetch_and_build_libbacktrace.sh'
#   PUBLIC: anything linking the library needs it too
set( DIR_LIBBACKTRACE ${PROJECT_SOURCE_DIR}/lib/win64_mingw/libbacktrace )
if( NOT EXISTS ${DIR_LIBBACKTRACE}/lib/libbacktrace.a )
    message( FATAL_ERROR "libbacktrace is missing from '${DIR_LIBBACKTRACE}', run 'script/debian/mingw_cross/fetch_and_build_libbacktrace.sh'" )
endif()
add_library( backtrace STATIC IMPORTED )
set_target_properties( backtrace PROPERTIES
    IMPORTED_LOCATION               ${DIR_LIBBACKTRACE}/lib/libbacktrace.a
    INTERFACE_INCLUDE_DIRECTORIES   ${DIR_LIBBACKTRACE}/include
)

target_compile_definitions( ${LIB_NAME} PRIVATE
    BOOST_STACKTRACE_USE_BACKTRACE
)

target_link_libraries( ${LIB_NAME} PUBLIC
    backtrace
)

# link the GCC and C++ runtimes statically,
#   so the .exe runs without shipping MinGW DLLs next to it
#   note: this links 'winpthreads' for posix compat
target_link_options( ${TEST_APP_NAME} PRIVATE
    -static
    -static-libgcc
    -static-libstdc++
)

# TODO:
# MinGW - no console, look into this
#   -Wl,--subsystem,console => Build for console subsystem.
#   -Wl,--subsystem,windows => Build for window subsystem.
#   https://caiorss.github.io/C-Cpp-Notes/compiler-flags-options.html

# startup script:  bin/win64[_<build type>]/start_<executable>.bat
#   sets up - env vars to find libraries
#           - the working directory
#   to not require installation
#   CRLF: cmd.exe misparses some .bat files with LF line endings
configure_file( ${PROJECT_SOURCE_DIR}/cmake/start.bat.in
                ${DIR_BIN}/start_${TEST_APP_NAME}.bat
                @ONLY
                NEWLINE_STYLE CRLF )
