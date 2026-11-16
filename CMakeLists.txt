cmake_minimum_required( VERSION 3.20 )

project( ptlib LANGUAGES C CXX )

# names of the built targets
set( LIB_NAME            ptlib )        # static library: lib<name>.a
set( TEST_APP_NAME       ptlib_test )   # test app

set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )

if( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE Release )
endif()


#--------------------------------------------------
# platform
#   decided by the compiler's target system:
#     native build                        -> linux
#     cmake/toolchain/win64-mingw.cmake   -> win64
#--------------------------------------------------
if( WIN32 )
    set( PLATFORM win64 )
elseif( CMAKE_SYSTEM_NAME STREQUAL "Linux" )
    set( PLATFORM linux )
else()
    message( FATAL_ERROR "Unsupported target system: '${CMAKE_SYSTEM_NAME}'" )
endif()

message( STATUS "Platform:   ${PLATFORM}" )
message( STATUS "Build type: ${CMAKE_BUILD_TYPE}" )


#--------------------------------------------------
# output directory:  bin/<platform>[_<build type>]/
#   Release: no suffix
#   others:  lowercase build type, e.g. _debug, _relwithdebinfo
#--------------------------------------------------
set( DIR_BIN ${PROJECT_SOURCE_DIR}/bin/${PLATFORM} )
if( NOT CMAKE_BUILD_TYPE STREQUAL "Release" )
    string( TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER )
    set( DIR_BIN ${DIR_BIN}_${BUILD_TYPE_LOWER} )
endif()

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIR_BIN} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIR_BIN} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIR_BIN} )


#--------------------------------------------------
# source files
#   library:            src/pt/**   include/pt/**
#                       src/platform/<platform>/**   include/platform/<platform>/**
#   test app:           src/test/**   include/test/**
#
#   CONFIGURE_DEPENDS: added/removed files are picked up on the next build,
#     no need to list them here.
#--------------------------------------------------
set( SOURCE_PATTERNS *.c *.cpp *.h *.hpp )

set( LIB_GLOBS )
set( PLATFORM_GLOBS )
set( TEST_GLOBS )
foreach( pattern ${SOURCE_PATTERNS} )
    list( APPEND LIB_GLOBS
        src/pt/${pattern}
        include/pt/${pattern}
    )
    list( APPEND PLATFORM_GLOBS
        src/platform/${PLATFORM}/${pattern}
        include/platform/${PLATFORM}/${pattern}
    )
    list( APPEND TEST_GLOBS
        src/test/${pattern}
        include/test/${pattern}
    )
endforeach()


# Project source files are determined by these vars.
# If there's overlap between these paths, filtering will likely also be needed!
# path in {include,src}/.
#   ./pt
file( GLOB_RECURSE LIB_FILES CONFIGURE_DEPENDS
      RELATIVE ${PROJECT_SOURCE_DIR} ${LIB_GLOBS} )
#   ./platform
file( GLOB_RECURSE PLATFORM_FILES CONFIGURE_DEPENDS
      RELATIVE ${PROJECT_SOURCE_DIR} ${PLATFORM_GLOBS} )
#   ./test
file( GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS
      RELATIVE ${PROJECT_SOURCE_DIR} ${TEST_GLOBS} )

# filters: exclude files from the lists above (none needed now)
#   e.g.: list( FILTER LIB_FILES EXCLUDE REGEX "^src/pt/experimental/" )


#--------------------------------------------------
# targets
#--------------------------------------------------
# shared library
#   replace 'STATIC' with 'SHARED':   lib<name>.so | lib<name>.dll + lib<name>.dll.a
#     - win64: the C++ runtime is linked into the .dll ('cmake/platform/win64.cmake'),
#         so memory and exceptions must not cross the .dll boundary
#         (free memory in the module it was allocated, catch exceptions inside the .dll)
add_library( ${LIB_NAME} STATIC
    ${LIB_FILES}
    ${PLATFORM_FILES}
)

add_executable( ${TEST_APP_NAME}
    ${TEST_FILES}
)

# PUBLIC: targets linking the library get its include directories
target_include_directories( ${LIB_NAME}
    PUBLIC  ${PROJECT_SOURCE_DIR}/include
            ${PROJECT_SOURCE_DIR}/include/platform/${PLATFORM}
    PRIVATE ${PROJECT_SOURCE_DIR}/src/pch
)
target_include_directories( ${TEST_APP_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src/pch
)

# -fPIC: the library can be linked into shared libraries too
set_target_properties( ${LIB_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# also makes the library build before the test app
target_link_libraries( ${TEST_APP_NAME} PRIVATE
    ${LIB_NAME}
)

# custom macros:
#   PT_DEBUG_ENABLED
#     enables additional debug logic and verbose debug logging
#     !! not having 'NDEBUG' defined will enable 'PT_DEBUG_ENABLED' !!
#   PT_DEBUG_NOAUTO
#     prevents auto-defining 'PT_DEBUG_ENABLED' based on 'NDEBUG'
#   PT_MEASURE_PERFORMANCE
foreach( target ${LIB_NAME} ${TEST_APP_NAME} )
    target_compile_options( ${target} PRIVATE
        -Wall -Wextra -Wno-unused-parameter
    )
    target_compile_definitions( ${target} PRIVATE
        PT_MEASURE_PERFORMANCE
    )
    # precompiled header: 'src/pch/pch.h' (see the file for what belongs in it)
    #   C++ files only: the C++ headers in it would break compiling .c files
    target_precompile_headers( ${target} PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:${PROJECT_SOURCE_DIR}/src/pch/pch.h>
    )
endforeach()

# strip release binaries
target_link_options( ${TEST_APP_NAME} PRIVATE
    $<$<CONFIG:Release>:-s>
)


#--------------------------------------------------
# platform specific dependencies
#--------------------------------------------------
include( ${PROJECT_SOURCE_DIR}/cmake/platform/${PLATFORM}.cmake )
