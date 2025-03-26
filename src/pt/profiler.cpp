#include "pt/profiler.h"

#include "pt/alias.h"

#include <assert.h>
#include <vector>

//TODO: remove this
#include <iostream>

//TODO: make the vector double-buffered (fast recording, slow file writing)
//TODO: empty the vector to some output (can be a graph view widget or a file)

std::mutex gmMeasurementsMutex;
std::vector< pt::PerfProfiler::Measurement > gmMeasurements = std::vector< pt::PerfProfiler::Measurement >();



pt::PerfProfiler::
PerfProfiler( char* id ):
    mName( id ), mStartTime( std::chrono::steady_clock::now() )
{}

pt::PerfProfiler::
PerfProfiler( const char* const id ):
    mName( id ), mStartTime( std::chrono::steady_clock::now() )
{}

pt::PerfProfiler::
PerfProfiler( const std::string& id ):
    mName( id ), mStartTime( std::chrono::steady_clock::now() )
{}


pt::PerfProfiler::
~PerfProfiler()
{
    Timestamp endTime = std::chrono::steady_clock::now();
    // TODO: use this
    //pt::MutexLockGuard lock( gmMeasurementsMutex );
    //gmMeasurements.push_back( Measurement( std::move( mName ), mStartTime, endTime ) );
    //  do not use mId from here on (it was moved)
    //-----
    // TODO: remove this
    //temporary reroute to console log until multithreaded execution is implemented
    auto duration_us = std::chrono::duration_cast< std::chrono::microseconds >( endTime - mStartTime );
    const std::chrono::duration<long, std::micro> usec = duration_us;
    std::cout << "Perf measurement: '" << mName << "': " << usec.count() << "us\n";
}
