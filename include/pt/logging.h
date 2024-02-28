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
//  the compiler extensions that make this available before C++17 aren't always, but mostly available
//aliases for a more convenient usage syntax
inline log::logstream& debug = log::debug;
inline log::logstream& out = log::out;
inline log::logstream& warn = log::warn;
inline log::logstream& err = log::err;


// Macro versions of loggers
#define PT_LOG_OUT(expr)  pt::log::out << expr << std::endl;
#define PT_LOG_WARN(expr) pt::log::warn << expr << std::endl;
#define PT_LOG_ERR(expr)  pt::log::err << expr << std::endl;

//Like assertions, PT_LOG_DEBUG can be macro-disabled
//  to eliminate unnecessary performance footprint in release builds
#ifdef PT_DEBUG_ENABLED
//TODO: closing element should be something like 'pt::log::send'
//  until receiving that, the operator<< calls fill up a local buffer
//  the closing element will initiate inter-process message transmission
#define PT_LOG_DEBUG(expr) pt::log::debug << expr << std::endl;
#else
#define PT_LOG_DEBUG(expr) (__PT_VOID_CAST (0))
#endif

} //end of namespace 'pt'
