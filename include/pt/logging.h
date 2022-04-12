/*
 * -------------------------------------------------------------------------
 * FILE:    logging.h
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE: Multiprocess logger, that writes simultaneously to std::cout and a log file.
 * -------------------------------------------------------------------------
 */

#pragma once

#include <string>
#include <cstring>

#include "pt/log/logstream.hpp"

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


//aliases for a more convenient usage syntax
inline log::logstream& debug = log::debug;
inline log::logstream& out = log::out;
inline log::logstream& warn = log::warn;
inline log::logstream& err = log::err;

} //end of namespace 'pt'








