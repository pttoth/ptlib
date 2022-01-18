cmake_minimum_required(VERSION 3.5)

project(ptlib_debian LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(MY_PROJ_ROOT ${PROJECT_SOURCE_DIR}/../..)

#set(MY_BUILD_DIR ${MY_PROJ_ROOT}/build)

set(MY_OUTPUT_DIR_WIN32 ${MY_PROJ_ROOT}/bin/win32)
set(MY_OUTPUT_DIR_WIN64 ${MY_PROJ_ROOT}/bin/win64)
set(MY_OUTPUT_DIR_DEBIAN ${MY_PROJ_ROOT}/bin/debian)

#set up actual output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR_DEBIAN})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MY_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${MY_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${MY_OUTPUT_DIR})


#build an archive (.a), for static linking
add_library(ptlib
    ${MY_PROJ_ROOT}/src/main.cpp
    )

target_include_directories(ptlib PRIVATE
    /usr/include
    ${MY_PROJ_ROOT}/include
    )


#build Test app
add_executable(ptlib_test
    ${MY_PROJ_ROOT}/src/main.cpp
    )

target_include_directories(ptlib_test PRIVATE
    /usr/include
    ${MY_PROJ_ROOT}/include
    )

target_link_libraries(ptlib_test
	ptlib
	)

