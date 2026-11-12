# Linux (native) specific settings and library dependencies.
#   Included by CMakeLists.txt, after the targets are defined.
#   Targets: ${LIB_NAME} (library), ${TEST_APP_NAME} (test app)

# See help: cmake/platform/help.linux.txt
#--------------------------------------------------

# boost.stacktrace: function names (and file:line with debug info) via libbacktrace
#   GCC ships it ('backtrace.h' and 'libbacktrace.a' in GCC's own directories)
#   PUBLIC: anything linking the library needs it too
target_compile_definitions( ${LIB_NAME} PRIVATE
    BOOST_STACKTRACE_USE_BACKTRACE
)

target_link_libraries( ${LIB_NAME} PUBLIC
    backtrace
)

# startup script:  bin/linux[_debug]/start_<executable>.sh
#   sets up - env vars to find libraries
#           - the working directory
#   to not require installation
configure_file( ${PROJECT_SOURCE_DIR}/cmake/start.sh.in
                ${DIR_BIN}/start_${TEST_APP_NAME}.sh
                @ONLY
                FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                                 GROUP_READ GROUP_EXECUTE
                                 WORLD_READ WORLD_EXECUTE )
