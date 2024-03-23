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


// Macro versions of loggers
#define PT_LOG_INFO(expr) pt::log::out << expr << pt::log::send
#define PT_LOG_OUT(expr)  pt::log::out << expr << pt::log::send   //deprecated, will be removed in a later version
#define PT_LOG_WARN(expr) pt::log::warn << "WARNING: " << expr << pt::log::send
#define PT_LOG_ERR(expr)  pt::log::err << "ERROR: " << expr << pt::log::send

#define __PT_LOG_ONCE( __LOGSTREAM, expr ) \
{ \
    static bool firsttime = true; \
    if( firsttime ){ \
        __LOGSTREAM << expr << pt::log::send; \
        firsttime = false; \
    } \
}

#define PT_LOG_ONCE_INFO(expr) __PT_LOG_ONCE( pt::log::out, expr )
#define PT_LOG_ONCE_WARN(expr) __PT_LOG_ONCE( pt::log::warn << "WARNING: " , expr )
#define PT_LOG_ONCE_ERR(expr) __PT_LOG_ONCE( pt::log::err << "ERROR: " , expr )


#define __PT_LOG_LIMITED( __LOGSTREAM, log_limit, expr ) \
{ \
    static int count = 1; \
    size_t limit = log_limit; \
    if( count < limit ){ \
        __LOGSTREAM << expr << pt::log::send; \
        ++count; \
    }else if( count == limit ){ \
        __LOGSTREAM << expr << pt::log::send; \
        __LOGSTREAM << "  Limit(" << limit << ") reached. Suppressing further logging of this message." << pt::log::send; \
        ++count; \
    } \
}

#define PT_LOG_LIMITED_INFO(log_limit, expr) __PT_LOG_LIMITED( pt::log::out, log_limit, expr )
#define PT_LOG_LIMITED_WARN(log_limit, expr) __PT_LOG_LIMITED( pt::log::warn << "WARNING: " , log_limit, expr )
#define PT_LOG_LIMITED_ERR(log_limit, expr) __PT_LOG_LIMITED( pt::log::err << "ERROR: " , log_limit, expr )

//Like assertions, PT_LOG_DEBUG can be macro-disabled
//  to eliminate unnecessary performance footprint in release builds
#ifdef PT_DEBUG_ENABLED
//TODO: implement 'pt::log::send'
//  until receiving that value, the operator<< calls only fill up a local buffer
//  upon receiving that value, logging will initiate inter-process message transmission
#define PT_LOG_DEBUG(expr) pt::log::debug << expr << pt::log::send
#define PT_LOG_ONCE_DEBUG(expr) __PT_LOG_ONCE( pt::log::debug, expr )
#define PT_LOG_LIMITED_DEBUG(log_limit, expr) __PT_LOG_LIMITED( pt::log::debug, log_limit, expr )
#else
#define PT_LOG_DEBUG(expr) (__PT_VOID_CAST (0))
#define PT_LOG_ONCE_DEBUG(expr) (__PT_VOID_CAST (0))
#define PT_LOG_LIMITED_DEBUG(log_limit, expr) (__PT_VOID_CAST (0))
#endif

} //end of namespace 'pt'
