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

#include <exception>
#include <memory>


#define PT_UNIMPLEMENTED_FUNCTION \
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
