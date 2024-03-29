/*
 * -------------------------------------------------------------------------
 * FILE:    logging.h
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE: Multiprocess logger, that writes simultaneously to std::cout and a log file.
 * -------------------------------------------------------------------------
 */

#pragma once

#include "pt/log/logstream.hpp"

#include <string>
#include <cstring>

#include <iostream>

namespace pt{
namespace log{

const uint32_t default_timeout = 5000;

std::string AutoGenerateLogFileName();

const std::string& GetRootDirectory();
const std::string& GetFileName();


//'rootdir' has to exist already
//directories in 'filepath' are created if missing
//'default_timeout' is the time to wait for the logging subprocess to start responding (not yet relevant)
bool Initialize(const std::string& rootdir,
                const std::string& filepath,
                uint32_t timeout = default_timeout);


void Destroy();
void LoadSettings();


extern logstream debug;
extern logstream out;
extern logstream warn;
extern logstream err;

} //end of namespace 'log'


// TODO: might need to add a C++17 checker macro here, or similar (compiler extension check?)
//      ...or move to C++17 :)
//  the compiler extensions that make this available before C++17 aren't always available
//  NOTE: doesn't compile on Windows in C++14, but it does on Debian

//aliases for a more convenient usage syntax
//inline log::logstream& debug = log::debug;
//inline log::logstream& out = log::out;
//inline log::logstream& warn = log::warn;
//inline log::logstream& err = log::err;


// Macro version of debug logger
//  like assertions, these can be defined to NULL to eliminate unnecessary performance footprint in release builds
//  #define PT_DISABLE_DEBUG_OUTPUT
//  note: defining 'NDEBUG' also defines 'PT_DISABLE_DEBUG_OUTPUT'

#ifdef __linux__

#ifdef NDEBUG
#define PT_DISABLE_DEBUG_OUTPUT
#endif

#if defined __cplusplus && __GNUC_PREREQ (2,95)
# define __PT_VOID_CAST static_cast<void>
#else
# define __PT_VOID_CAST (void)
#endif

#ifdef PT_DISABLE_DEBUG_OUTPUT
#define PT_LOG_DEBUG(expr) (__PT_VOID_CAST (0))
#else
#define PT_LOG_DEBUG(expr) pt::log::debug << expr << std::endl;
#endif

#elif _WIN32 || _WIN64
// TODO: implement for windows too, needs some work
//   this macro is trickier on windows, possibly need to implement for MinGW, VC++ and clang(?) too

#define PT_LOG_DEBUG(expr) pt::log::debug << expr << std::endl;

#endif

} //end of namespace 'pt'
