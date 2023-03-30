#include "pt/log/logstream.hpp"

#include "pt/logging.h"

#include <chrono>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <cstring>




const std::string& pt::log::logstream::
getFileName() const
{
    return pt::log::GetFileName();
}


pt::log::logstream::
logstream(): mEnabled(true)
{}


pt::log::logstream::
logstream(const std::string &prefix):
    mEnabled(true), mMessagePrefix( prefix )
{}
