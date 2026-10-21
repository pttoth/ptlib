#include "pt/def.h"

#include "pt/logging.h"
#include "pt/utility.hpp"

#include <boost/stacktrace.hpp>


void pt::helper::
FatalError( const char* message ) noexcept
{
    using namespace boost;
    const std::string trace = stacktrace::to_string( stacktrace::stacktrace() );
    PT_LOG_ERR( "Fatal Error: " << message << "\n" << trace.c_str() );
    exit( EXIT_FAILURE );
}


void pt::helper::
WarnUnimplementedFunction() noexcept
{
    pt::PrintStackTrace( "Unimplemented/unfinished function called" );
    //TODO: use logging instead
    //TODO: get filename and line number instead of stacktrace
}


void pt::helper::
WarnDeprecatedFunction() noexcept
{
    pt::PrintStackTrace( "Called function is marked deprecated" );
    //TODO: use logging instead
    //TODO: get filename and line number instead of stacktrace
}
