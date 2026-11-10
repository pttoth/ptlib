# Cross-compile for 64-bit Windows with MinGW-w64 from Debian / WSL.
#   apt package: g++-mingw-w64-x86-64-posix
#   usage: cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain/win64-mingw.cmake ...
#   (script/build.sh -p win64 does this for you)

set( CMAKE_SYSTEM_NAME      Windows )
set( CMAKE_SYSTEM_PROCESSOR x86_64 )

set( TOOLCHAIN_PREFIX x86_64-w64-mingw32 )

# '-posix' variant: needed for std::thread, std::mutex, etc.
set( CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}-gcc-posix )
set( CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++-posix )
set( CMAKE_RC_COMPILER  ${TOOLCHAIN_PREFIX}-windres )

# look for libraries and headers only in the MinGW sysroot, programs on the host
set( CMAKE_FIND_ROOT_PATH /usr/${TOOLCHAIN_PREFIX} )
set( CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER )
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY )
