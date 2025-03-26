/*
 * -------------------------------------------------------------------------
 * FILE:    profiler.h
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE: Performance profiler class.
 *            Constructor starts the measurement.
 *            Destructor stops and records the measurement.
 *          If 'PT_MEASURE_PERFORMANCE' is not defined, the profiling macros have zero overhead.
 * USAGE:   PERF_WATCH( "foo_measurement", foo( param1, param2, ... ) )
 * -------------------------------------------------------------------------
 */

#include "pt/name.h"

#include <chrono>
#include <string>


namespace pt{

class PerfProfiler
{
public:
    using Timestamp = std::chrono::time_point< std::chrono::steady_clock >;

    struct Measurement{
        std::string name;
        Timestamp startTime;
        Timestamp endTime;

        Measurement()
        {}
        Measurement( const std::string& name_, Timestamp startTime_, Timestamp endTime_ ):
            name( name_ ), startTime( startTime_ ), endTime( endTime_ )
        {}
    };

    PerfProfiler( char* id );
    PerfProfiler( const char* const id );
    PerfProfiler( const std::string& id );
    PerfProfiler( const PerfProfiler& other ) = delete;
    PerfProfiler( PerfProfiler&& other ) = delete;
    virtual ~PerfProfiler();
    PerfProfiler& operator=( const PerfProfiler& other ) = delete;
    PerfProfiler& operator=( PerfProfiler&& other ) = delete;

private:
    std::string mName;
    Timestamp   mStartTime;

};

} // end of namespace 'pt'


#ifdef PT_MEASURE_PERFORMANCE
    #define PERF_WATCH( PARAM_PERF_ID, PARAM_FUNCTION_CALL ) \
    { \
        pt::PerfProfiler perf_watcher( PARAM_PERF_ID ); \
        PARAM_FUNCTION_CALL; \
    }
#else
    #define PERF_WATCH( PARAM_PERF_ID, PARAM_FUNCTION_CALL ) PARAM_FUNCTION_CALL
#endif

