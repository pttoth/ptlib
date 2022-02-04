/*
 * -------------------------------------------------------------------------
 * FILE:    logging.h
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE: Multiprocess logger, that writes simultaneously to std::cout and a log file.
 * NOTE:    The multiprocess part is missing for now...
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
} //end of namespace 'pt'








