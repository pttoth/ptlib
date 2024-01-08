cmake_minimum_required(VERSION 3.5)

project(ptlib_debian LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(MY_PROJ_ROOT ${PROJECT_SOURCE_DIR}/../..)
set(MY_BUILD_DIR ${MY_PROJ_ROOT}/build)
set(MY_OUTPUT_DIR_WIN32 ${MY_PROJ_ROOT}/bin/win32)
set(MY_OUTPUT_DIR_WIN64 ${MY_PROJ_ROOT}/bin/win64)
set(MY_OUTPUT_DIR_DEBIAN ${MY_PROJ_ROOT}/bin/debian)

#set up actual output directory
set(MY_OUTPUT_DIR ${MY_OUTPUT_DIR_DEBIAN})
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

set(MY_COMPILE_FLAGS "-Wall -Wextra -DPT_MEASURE_PERFORMANCE")
set(CMAKE_C_FLAGS ${MY_COMPILE_FLAGS})
set(CMAKE_CXX_FLAGS ${MY_COMPILE_FLAGS})

#build an archive (.a), for static linking
add_library(ptlib
    ${MY_PROJ_ROOT}/src/pt/config.cpp
    ${MY_PROJ_ROOT}/src/pt/name.cpp
    ${MY_PROJ_ROOT}/src/pt/profiler.cpp
    ${MY_PROJ_ROOT}/src/pt/utility.cpp
    ${MY_PROJ_ROOT}/src/pt/log/logstream.cpp
    ${MY_PROJ_ROOT}/src/pt/logging.cpp
    ${MY_PROJ_ROOT}/include/pt/alias.h
    ${MY_PROJ_ROOT}/include/pt/config.h
    ${MY_PROJ_ROOT}/include/pt/def.h
    ${MY_PROJ_ROOT}/include/pt/name.h
    ${MY_PROJ_ROOT}/include/pt/profiler.h
    ${MY_PROJ_ROOT}/include/pt/utility.hpp
    ${MY_PROJ_ROOT}/include/pt/event.hpp
    ${MY_PROJ_ROOT}/include/pt/logging.h
    ${MY_PROJ_ROOT}/include/pt/log/logstream.hpp
)

target_include_directories(ptlib PRIVATE
    ${MY_PROJ_ROOT}/include
)

set_target_properties(ptlib PROPERTIES COMPILE_FLAGS
    "${MY_COMPILE_FLAGS} -fPIC"
)

#build Test app
add_executable(ptlib_test
    ${MY_PROJ_ROOT}/src/main.cpp
    ${MY_PROJ_ROOT}/src/TestEvent.cpp
    ${MY_PROJ_ROOT}/src/TestLogger.cpp
    ${MY_PROJ_ROOT}/src/TestUtility.cpp
    ${MY_PROJ_ROOT}/include/Test.hpp
    ${MY_PROJ_ROOT}/include/TestConfig.hpp
    ${MY_PROJ_ROOT}/include/TestEvent.hpp
    ${MY_PROJ_ROOT}/include/TestLogger.hpp
    ${MY_PROJ_ROOT}/include/TestUtility.hpp
)

target_include_directories(ptlib_test PRIVATE
    ${MY_PROJ_ROOT}/include
)

target_link_libraries(ptlib_test
    -L"${MY_OUTPUT_DIR}"
    -L"${MY_OUTPUT_DIR_DEBUG}"
    -lptlib
)

add_dependencies(ptlib_test ptlib)

