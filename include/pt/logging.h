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

//TODO: refactor (placeholder)
//  enables early references to 'pt::log::send'
const char send = '\n';

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
//  the compiler extensions that make this available before C++17 aren't always, but mostly available
//aliases for a more convenient usage syntax
/*
inline log::logstream& debug = log::debug;
inline log::logstream& out = log::out;
inline log::logstream& warn = log::warn;
inline log::logstream& err = log::err;
*/


// Macro versions of loggers
#define PT_LOG_INFO(expr) pt::log::out << expr << pt::log::send
#define PT_LOG_OUT(expr)  pt::log::out << expr << pt::log::send   //deprecated, will be removed in a later version
#define PT_LOG_WARN(expr) pt::log::warn << expr << pt::log::send
#define PT_LOG_ERR(expr)  pt::log::err << expr << pt::log::send

#define __PT_LOG_ONCE( __LOGSTREAM, expr ) \
{ \
    static bool firsttime = true; \
    if( firsttime ){ \
        __LOGSTREAM << expr << pt::log::send; \
        firsttime = false; \
    } \
}


#define PT_LOG_ONCE_INFO(expr) __PT_LOG_ONCE( pt::log::out, expr )
#define PT_LOG_ONCE_WARN(expr) __PT_LOG_ONCE( pt::log::warn, expr )
#define PT_LOG_ONCE_ERR(expr) __PT_LOG_ONCE( pt::log::err, expr )


//Like assertions, PT_LOG_DEBUG can be macro-disabled
//  to eliminate unnecessary performance footprint in release builds
#ifdef PT_DEBUG_ENABLED
//TODO: implement 'pt::log::send'
//  until receiving that value, the operator<< calls only fill up a local buffer
//  upon receiving that value, logging will initiate inter-process message transmission
#define PT_LOG_DEBUG(expr) pt::log::debug << expr << pt::log::send
#define PT_LOG_ONCE_DEBUG(expr) __PT_LOG_ONCE( pt::log::debug, expr )
#else
#define PT_LOG_DEBUG(expr) (__PT_VOID_CAST (0))
#define PT_LOG_ONCE_DEBUG(expr) (__PT_VOID_CAST (0))
#endif

} //end of namespace 'pt'
