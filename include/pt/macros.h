/** -----------------------------------------------------------------------------
  * FILE:    macros.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Various generic macro definitions.
  *            Macros with dependencies (eg.: includes) should come here.
  * -----------------------------------------------------------------------------
  */

#pragma once

#include "def.h"

#include <stdexcept>
#include <memory>

namespace pt{
namespace helper{

void PrintStackTrace( const char* message = "" );

} // end of namespace 'helper'
} // end of namespace 'pt'

// compatibility helper macro for defining removable macro functions
#if defined __cplusplus && __GNUC_PREREQ (2,95)
# define __PT_VOID_CAST static_cast<void>
#else
# define __PT_VOID_CAST (void)
#endif


#define PT_UNIMPLEMENTED_FUNCTION \
    pt::helper::PrintStackTrace( "Called function is not implemented" ); \
    throw std::logic_error( "Called function is not implemented" );


#define PT_FORWARD_DECLARE_CLASS( CLASSNAME ) \
class CLASSNAME; \
using CLASSNAME##Ptr          = std::shared_ptr< CLASSNAME >; \
using Const##CLASSNAME##Ptr   = std::shared_ptr< const CLASSNAME >; \
using CLASSNAME##WPtr         = std::weak_ptr< CLASSNAME >; \
using Const##CLASSNAME##WPtr  = std::weak_ptr< const CLASSNAME >; \
using CLASSNAME##UPtr         = std::unique_ptr< CLASSNAME >; \
using Const##CLASSNAME##UPtr  = std::unique_ptr< const CLASSNAME >;


#define PT_FORWARD_DECLARE_STRUCT( STRUCTNAME ) \
struct STRUCTNAME; \
using STRUCTNAME##Ptr          = std::shared_ptr< STRUCTNAME >; \
using Const##STRUCTNAME##Ptr   = std::shared_ptr< const STRUCTNAME >; \
using STRUCTNAME##WPtr         = std::weak_ptr< STRUCTNAME >; \
using Const##STRUCTNAME##WPtr  = std::weak_ptr< const STRUCTNAME >; \
using STRUCTNAME##UPtr         = std::unique_ptr< STRUCTNAME >; \
using Const##STRUCTNAME##UPtr  = std::unique_ptr< const STRUCTNAME >;


//return the enum name in parameter as string
#define EnumToString(ENUM_NAME) #ENUM_NAME
