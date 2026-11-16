#include "pt/def.h"

#include "pt/logging.h"
#include "pt/utility.hpp"

#include <cstdlib>

void pt::helper::
FatalError( const char* message ) noexcept
{
    PT_LOG_ERR( "Fatal Error: " << message );
    pt::PrintStackTrace();
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
