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
void WarnUnimplementedFunction();
void WarnDeprecatedFunction();

} // end of namespace 'helper'
} // end of namespace 'pt'

// @TODO: move things without dependencies to 'pt/def.h'

// COMPATIBILITY
//   this is a copy of the convenience-macro '__GNUC_PREREQ' found in '/usr/include/features.h'
//   it's not part of the GNU standard and is not available on Windows...
#if defined __GNUC__ && defined __GNUC_MINOR__
# define PT__GNUC_PREREQ(maj, min) \
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define PT__GNUC_PREREQ(maj, min) 0
#endif

// @TODO: PT__GNUC_PREREQ -> __PT_GNUC_PREREQ

// compatibility helper macro for defining removable macro functions
#if defined __cplusplus && PT__GNUC_PREREQ (2,95)
# define __PT_VOID_CAST static_cast<void>
#else
# define __PT_VOID_CAST (void)
#endif


#define PT_UNIMPLEMENTED_FUNCTION \
    pt::helper::PrintStackTrace( "Called function is not implemented" ); \
    throw std::logic_error( "Called function is not implemented" );

#define PT_WARN_UNIMPLEMENTED_FUNCTION \
    pt::helper::WarnUnimplementedFunction();



#ifdef PT_DEBUG_ENABLED
    #define PT_WARN_DEPRECATED_FUNCTION \
        pt::helper::WarnDeprecatedFunction();
#else
    #define PT_WARN_DEPRECATED_FUNCTION __PT_VOID_CAST (0);
#endif



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
