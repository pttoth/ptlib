//-----------------------------------------------------------------------------
//  FILE:    def.h
//  AUTHOR:  ptoth
//  EMAIL:   peter.t.toth92@gmail.com
//  PURPOSE: Macros, definitions and aliases.
//-----------------------------------------------------------------------------
#pragma once

#include "def_minimal.h"
#include <memory>
#include <mutex>

//-----------------------------------
// Paranoid guard against exotic platforms
static_assert( std::numeric_limits<f32>::is_iec559,  "float is not IEEE 754 binary32" );
static_assert( std::numeric_limits<f32>::digits10 == 6,  "unexpected float precision" );
static_assert( std::numeric_limits<f64>::is_iec559, "double is not IEEE 754 binary64" );
static_assert( std::numeric_limits<f64>::digits10 == 15, "unexpected double precision" );

//-----------------------------------
//  MutexLock / MutexLockGuard
namespace pt{
    using MutexLock         = std::unique_lock< std::mutex >;
    using MutexLockGuard    = std::lock_guard< std::mutex >;
    // TODO: add C++17 detector macro here
    //using MutexScopedLock = std::scoped_lock< std::mutex >;
}

//-----------------------------------
// PT_FORWARD_DECLARE_CLASS / _STRUCT

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
