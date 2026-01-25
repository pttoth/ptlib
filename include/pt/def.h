/** -----------------------------------------------------------------------------
  * FILE:    def.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Standalone macros and definitions for project-wide use.
  *          (They have no include dependencies)
  * -----------------------------------------------------------------------------
  */

#pragma once

//---------------------------------------------------
//  platform definitions:
//---------------------------------------------------

//-----Linux-----------------------------------------
#if defined (__linux__) || defined (__linux) || defined (linux)
    #define PT_PLATFORM_LINUX

#endif

//-----WINDOWS---------------------------------------
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64__) \
    || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) \
    || defined(__TOS_WIN__) || defined(__WINDOWS__) \
    || defined(__CYGWIN__)

    #define PT_PLATFORM_WINDOWS
#endif

//-----Mac-------------------------------------------
#if defined (__APPLE__) || defined (__MACH__)
    #define PT_PLATFORM_MAC

#endif
//---------------------------------------------------

#if !defined (PT_PLATFORM_LINUX) && !defined (PT_PLATFORM_WINDOWS) && !defined (PT_PLATFORM_MAC)
    #error "Unknown platform!"
#endif

#if defined PT_PLATFORM_LINUX   && defined PT_PLATFORM_WINDOWS \
 || defined PT_PLATFORM_LINUX   && defined PT_PLATFORM_MAC \
 || defined PT_PLATFORM_WINDOWS && defined PT_PLATFORM_MAC \

    #error "Multiple platforms are detected at the same time!"

#endif

// Check, if build target is 32 or 64 bit
//  Intended supported platforms: Windows/Linux - x86/x86_64/arm32/AArch64 - GCC/Clang/MSVC
#if defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__) || defined(__LP64__) || defined(_LP64)
    #define PT_PLATFORM_64_BIT
    static_assert( (sizeof(void*) == 8), "Unknown, exotic platform!" );
#elif defined(_WIN32) || defined(__ILP32__) || defined(__i386__) || defined(__arm__) || defined(__thumb__)
    #define PT_PLATFORM_32_BIT
    static_assert( (sizeof(void*) == 4), "Unknown, exotic platform!" );
#else
    #error "Unknown platform!"
#endif


//---------------------------------------------------
//  generic definitions:
//---------------------------------------------------

#ifndef PT_DEBUG_NOAUTO
    #ifndef NDEBUG
        #define PT_DEBUG_ENABLED
    #endif
#endif
