#include "test/TestStacktrace.hpp"

#include <boost/stacktrace.hpp>

#include <iostream>
#include <string>
#include <thread>

#if defined _WIN32
    #include <windows.h>
#endif

// Verification of boost.stacktrace + libbacktrace, mainly for real Windows.
//
// Why: libbacktrace can't find the exe's own file name on Windows by itself
//   (GCC bug 82543: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=82543).
//   boost works around it: it passes the path from 'GetModuleFileNameA()' to libbacktrace
//   ('boost/stacktrace/detail/location_from_symbol.hpp', 'program_location').
//   That path is ANSI and limited to 260 chars (MAX_PATH),
//   so libbacktrace can't open the exe if the path
//     - has characters missing from the system's ANSI code page (shown as '?')
//     - is longer than 259 chars
//   -> no function names, addresses only.
//
// Expected results per build type (from CMakeLists.txt):
//   Release:         stripped        -> addresses only
//   RelWithDebInfo:  symbols + -g    -> function names + file:line
//   Debug:           symbols + -g    -> function names + file:line
//
// How to run on Windows:
//   build:   script/build.sh -p win64 -rd     (and/or -d, and the default Release)
//   copy:    bin/win64_relwithdebinfo/ptlib_test.exe to the Windows machine
//   run it from these locations (cmd.exe or PowerShell), compare the results:
//     1. a plain ASCII path:       C:\pt_test\ptlib_test.exe
//     2. a path with spaces:       C:\pt test\ptlib_test.exe
//     3. non-ASCII, your code page:  C:\pt_áéő\ptlib_test.exe
//     4. non-ASCII, other script:  C:\pt_日本\ptlib_test.exe
//     5. a path over 260 chars (needs long paths enabled in Windows)
//   cases 3-5 may fail: that's the 'GetModuleFileNameA' limit above, not a build error


namespace {

struct TraceResult
{
    bool        found_function  = false;   // probe function's name found in the trace
    bool        found_source    = false;   // ...with file:line
    std::string trace;
};


// noinline: must stay a separate frame, even in optimized builds
__attribute__(( noinline )) TraceResult
StacktraceProbeFunction()
{
    const boost::stacktrace::stacktrace st;

    TraceResult result;
    result.trace = boost::stacktrace::to_string( st );

    for( const boost::stacktrace::frame& frame : st ){
        if( frame.name().find( "StacktraceProbeFunction" ) != std::string::npos ){
            result.found_function = true;
            result.found_source = !frame.source_file().empty() && 0 < frame.source_line();
        }
    }

    // keeps this frame on the stack (no tail call optimization)
    asm volatile( "" ::: "memory" );
    return result;
}


bool
CheckResult( const char* context, const TraceResult& result )
{
#if defined PT_TEST_EXPECT_FUNCTION_NAMES
    const bool expect_function = true;
#else
    const bool expect_function = false;
#endif
#if defined PT_TEST_EXPECT_SOURCE_LINES
    const bool expect_source = true;
#else
    const bool expect_source = false;
#endif

    const bool ok = ( result.found_function == expect_function )
                 && ( result.found_source   == expect_source );

    std::cout << "  " << context << ":\n";
    std::cout << result.trace;
    std::cout << "    function name: " << ( result.found_function ? "yes" : "no" )
              << "  (expected: "       << ( expect_function       ? "yes" : "no" ) << ")\n";
    std::cout << "    file:line:     " << ( result.found_source   ? "yes" : "no" )
              << "  (expected: "       << ( expect_source         ? "yes" : "no" ) << ")\n";
    std::cout << "    -> " << ( ok ? "OK" : "MISMATCH" ) << "\n\n";
    return ok;
}


// prints what boost passes to libbacktrace as the exe's path
void
PrintExePath()
{
#if defined _WIN32
    char        path[MAX_PATH];
    const DWORD length = GetModuleFileNameA( nullptr, path, MAX_PATH );
    const bool  truncated = ( MAX_PATH == length );
    const bool  lossy = ( std::string( path, length ).find( '?' ) != std::string::npos );

    std::cout << "  GetModuleFileNameA: '" << std::string( path, length ) << "'\n";
    std::cout << "    length: " << length << " / " << MAX_PATH
              << ( truncated ? "  !! TRUNCATED: path too long" : "" ) << "\n";
    if( lossy ){
        std::cout << "    !! '?' in path: characters missing from the ANSI code page\n";
    }
    std::cout << "\n";
#else
    std::cout << "  (not Windows: libbacktrace finds the exe itself)\n\n";
#endif
}

} // namespace


bool TestStacktrace::
run()
{
    std::cout << "--------------------------------------------------\n";
    std::cout << "  Testing Stacktrace (boost.stacktrace + libbacktrace)\n";
    std::cout << "--------------------------------------------------\n";

    PrintExePath();

    bool success = CheckResult( "main thread", StacktraceProbeFunction() );

    // boost keeps the libbacktrace state per thread ('thread_local'): check a new thread too
    TraceResult thread_result;
    std::thread thread( [&thread_result](){ thread_result = StacktraceProbeFunction(); } );
    thread.join();
    success = CheckResult( "second thread", thread_result ) && success;

    return success;
}
