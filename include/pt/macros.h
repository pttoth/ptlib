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

//return the enum name in parameter as string
#define EnumToString(ENUM_NAME) #ENUM_NAME
