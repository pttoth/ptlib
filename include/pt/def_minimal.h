//-----------------------------------------------------------------------------
//  FILE:    def_minimal.h
//  AUTHOR:  ptoth
//  EMAIL:   peter.t.toth92@gmail.com
//  PURPOSE: Macros and definitions, with minimal dependencies.
//-----------------------------------------------------------------------------
#pragma once

#include <cstdint>

//----------------------------------------
//  primitive aliases
using s8    = int8_t;
using s16   = int16_t;
using s32   = int32_t;
using s64   = int64_t;

using u8    = uint8_t;
using u16   = uint16_t;
using u32   = uint32_t;
using u64   = uint64_t;

using f32   = float;
using f64   = double;

// Paranoid guard against exotic platforms
static_assert( sizeof(f32) == 4, "float is not 32-bit" );
static_assert( sizeof(f64) == 8, "double is not 64-bit" );

//----------------------------------------
//  function declarations
namespace pt{
namespace helper{

[[noreturn]]
void FatalError( const char* message = "" ) noexcept;

void WarnUnimplementedFunction() noexcept;
void WarnDeprecatedFunction() noexcept;

} // end of namespace 'helper'
} // end of namespace 'pt'


//---------------------------------------------------
//  Environment detection
//---------------------------------------------------
//------ Linux ------
#if defined (__linux__) || defined (__linux) || defined (linux)
    #define PT_PLATFORM_LINUX

#endif

//----- WINDOWS -----
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__) \
    || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) \
    || defined(__TOS_WIN__) || defined(__WINDOWS__) \
    || defined(__CYGWIN__)

    #define PT_PLATFORM_WINDOWS
#endif

//------- Mac -------
#if defined (__APPLE__) || defined (__MACH__)
    #define PT_PLATFORM_MAC

#endif

//-------------------
#if !defined (PT_PLATFORM_LINUX) && !defined (PT_PLATFORM_WINDOWS) && !defined (PT_PLATFORM_MAC)
    #error "Unknown platform!"
#endif

#if defined PT_PLATFORM_LINUX   && defined PT_PLATFORM_WINDOWS \
 || defined PT_PLATFORM_LINUX   && defined PT_PLATFORM_MAC \
 || defined PT_PLATFORM_WINDOWS && defined PT_PLATFORM_MAC \

    #error "Multiple platforms are detected at the same time!"

#endif

//-----------------------------------
//  Compiler detection
#if defined(__clang__)
    #define PT_COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #define PT_COMPILER_MSVC 1
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define PT_COMPILER_MINGW 1
#elif defined(__GNUC__)
    #define PT_COMPILER_GCC 1
#endif


//---------------------------------------------------
//  generic definitions:
//---------------------------------------------------
// macro: PT_DEBUG:
//  TODO: PT_DEBUG_ENABLED is deprecated, remove!
#ifndef PT_DEBUG_NOAUTO
    #ifndef NDEBUG
        #define PT_DEBUG
        #define PT_DEBUG_ENABLED
    #endif
#endif

//-----------------------------------
// PT_FORCE_INLINE:
#define PT_FORCE_INLINE inline
//  TODO: add correct definition

//-----------------------------------
// PT_TO_C_STRING():
//   Return CONTENT param as cstring: "CONTENT".
#define PT_TO_C_STRING( CONTENT ) #CONTENT
// TODO: deprecated, remove!
#define EnumToString(ENUM_NAME) #ENUM_NAME

//-----------------------------------
// PT_UNIMPLEMENTED_FUNCTION / PT_WARN_UNIMPLEMENTED_FUNCTION:
//  Quick-use placeholder to mark unimplemented functions.
//  In debug, it crashes the program with a stacktrace.
//  In release, it produces a warning.
#define PT_UNIMPLEMENTED_FUNCTION \
    pt::helper::FatalError( "Called function is not implemented" );

#define PT_WARN_UNIMPLEMENTED_FUNCTION \
    pt::helper::WarnUnimplementedFunction();

//-----------------------------------
// PT_WARN_DEPRECATED_FUNCTION:
//  Placeholder to mark deprecated functions.
//  In debug, it produces a warning.
//  In release, it does nothing.
#ifdef PT_DEBUG
    #define PT_WARN_DEPRECATED_FUNCTION \
        pt::helper::WarnDeprecatedFunction();
#else
    #define PT_WARN_DEPRECATED_FUNCTION static_cast<void>
#endif


//-----------------------------------
// IF_LIKELY / IF_UNLIKELY:
//   Hints for branch predictor.
#if defined(_MSVC_LANG)
    #define CPP_LANG_VERSION _MSVC_LANG
#else
    #define CPP_LANG_VERSION __cplusplus
#endif

#if CPP_LANG_VERSION >= 202002L
// use native cpp20 feature
    #define IF_LIKELY(condition)   if (condition) [[likely]]
    #define IF_UNLIKELY(condition) if (condition) [[unlikely]]

#elif defined(__clang__) && __has_cpp_attribute(clang::likely)
// use clang's extension
    #define IF_LIKELY(condition)   if (condition) [[clang::likely]]
    #define IF_UNLIKELY(condition) if (condition) [[clang::unlikely]]

#elif defined(__GNUC__)
// use gcc's extension
    #define IF_LIKELY(condition)   if (__builtin_expect(!!(condition), 1))
    #define IF_UNLIKELY(condition) if (__builtin_expect(!!(condition), 0))

#else
// branch predictor hints are not supported
    #define IF_LIKELY(condition)   if (condition)
    #define IF_UNLIKELY(condition) if (condition)

#endif

//--------------------------------------------------
