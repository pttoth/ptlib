/** -----------------------------------------------------------------------------
  * FILE:    def.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE:
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
