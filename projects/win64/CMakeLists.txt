cmake_minimum_required(VERSION 3.5)

project(ptlib_win64 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(MY_PROJ_ROOT ${PROJECT_SOURCE_DIR}/../..)
set(MY_BUILD_DIR ${MY_PROJ_ROOT}/build)
set(MY_OUTPUT_DIR_WIN32 ${MY_PROJ_ROOT}/bin/win32)
set(MY_OUTPUT_DIR_WIN64 ${MY_PROJ_ROOT}/bin/win64)
set(MY_OUTPUT_DIR_DEBIAN ${MY_PROJ_ROOT}/bin/debian)

#set up actual output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR_WIN64})
set(MY_OUTPUT_DIR_DEBUG ${MY_OUTPUT_DIR}_debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MY_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MY_OUTPUT_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${MY_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${MY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${MY_OUTPUT_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${MY_OUTPUT_DIR_DEBUG})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${MY_OUTPUT_DIR_DEBUG})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${MY_OUTPUT_DIR_DEBUG})


#build an archive (.a), for static linking
add_library(ptlib
    ${MY_PROJ_ROOT}/src/pt/config.cpp
    ${MY_PROJ_ROOT}/src/pt/utility.cpp
    ${MY_PROJ_ROOT}/include/pt/array.h
    ${MY_PROJ_ROOT}/include/pt/config.h
    ${MY_PROJ_ROOT}/include/pt/utility.hpp
    ${MY_PROJ_ROOT}/include/pt/event.hpp
)

target_include_directories(ptlib PRIVATE
    ${MY_PROJ_ROOT}/include
)


#build Test app
add_executable(ptlib_test
    ${MY_PROJ_ROOT}/src/main.cpp
    ${MY_PROJ_ROOT}/test/test_config.hpp
)

target_include_directories(ptlib_test PRIVATE
    ${MY_PROJ_ROOT}/include
)

target_link_libraries(ptlib_test
    -L"${MY_OUTPUT_DIR}"
    -L"${MY_OUTPUT_DIR_DEBUG}"
    -lptlib
)
