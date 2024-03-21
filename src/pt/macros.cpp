#include "pt/macros.h"

#include "pt/logging.h"
#include "pt/utility.hpp"

void pt::helper::
PrintStackTrace( const char* message ){
    pt::PrintStackTrace( message );
}


void pt::helper::
WarnUnimplementedFunction()
{
    pt::PrintStackTrace( "Unimplemented/unfinished function called" );
    //TODO: use logging instead
    //TODO: get filename and line number instead of stacktrace
}
