#include "pt/log/logstream.hpp"

#include "pt/logging.h"
#include "pt/name.h"

#include <chrono>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <cstring>

using namespace pt::log;


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


logstream& logstream::
operator<<( const pt::Name& data ){
    if( mEnabled ){
        LogMessage<std::string>( data.GetStdString() );
    }
    return *this;
}
