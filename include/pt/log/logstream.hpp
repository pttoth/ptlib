/*
 * -------------------------------------------------------------------------
 * FILE:    logstream.hpp
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE: Multiprocess logger, that writes simultaneously to a log window and a log file.
 * -------------------------------------------------------------------------
 */

//  TODO: make it actually multiprocess !

#pragma once

#include "pt/def.h"
#include "pt/utility.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

//#include <experimental/filesystem>
//#include <filesystem>


namespace pt{

namespace cpp{
    //namespace fs = std::experimental::filesystem;
    //namespace fs = std::filesystem;
}


class Name;


namespace log{


class logstream;

#define DEFINE_LOGSTREAM_OUT_OPERATOR(STREAM_OUT_VAR_1)	\
    logstream& operator<<(STREAM_OUT_VAR_1 data){	\
        if( mEnabled ){ \
            LogMessage<STREAM_OUT_VAR_1>(data); \
        } \
        return *this; \
    }

#define DEFINE_LOGSTREAM_OUT_FUNC_OPERATOR(STREAM_OUT_VAR_1, FUNC_PARAMS)	\
    logstream& operator<<(STREAM_OUT_VAR_1 (*data) FUNC_PARAMS){	\
        if( mEnabled ){ \
            LogMessage<STREAM_OUT_VAR_1 FUNC_PARAMS>(data); \
        } \
        return *this; \
    }


class logstream{
    bool            mEnabled;
    std::string     mMessagePrefix;

    //TODO: make 'LogMessage' compatible with 'ostringstream::rdbuf()' as parameter
    //  eg.: std::ostringstream oss;
    //       pt::log::out << oss.rdbuf();

    #ifdef PT_PLATFORM_LINUX
    template<typename T>
    void LogMessageLinux( const T data ) const{
        //temporary solution
        //TODO: enqueue the message in the logger process instead

        //TODO: take care of race conditions between debug/out/warn/err

        //TODO: by default, fill a buffer only
        //      when 'pt::log::send' is received, send the message to the logger process
        std::string fname = this->getFileName();

        if( mEnabled ){
            std::cout << data;
        }

        if( (mEnabled) && (fname.length() != 0) ){
            std::fstream fs;
            fs.open( fname.c_str(), std::fstream::out
                                  | std::fstream::app);

            if(!fs.is_open()){
                std::cout << "fs is NOT open!\n";
            }

            if( 0 < mMessagePrefix.length() ){
                fs << mMessagePrefix << ": " << data;
            }else{
                fs << data;
            }

            fs.close();
        }
    }
    #elif PT_PLATFORM_WINDOWS
    template<typename T>
    void LogMessageWin(T data) const{
//test code for handling Unicode paths
//  MinGW 8.1.0's buggy with c++17 'std::filesystem', need new compiler for this
/*
            //std::filesystem::path p = fnameW;
            pt::cpp::fs::path p = L"árvíztűrő tükörfúrógép/asd.txt";
            //pt::cpp::fs::path p = fnameW.c_str();

            std::fstream fs;
            fs.open(p, std::fstream::out
                        | std::fstream::app);

            if(!fs.is_open()){
                std::cout << "fs wchar is NOT open!\n";
            }

            fs << data;

            fs.close();
*/

            std::cout << data;

            std::string fname = this->getFileName();
            std::string path = fname;
            std::fstream fs;
            fs.open(path, std::fstream::out
                        | std::fstream::app);

            if(!fs.is_open()){
                std::cout << "fs char is NOT open!\n";
            }

            fs << data;

            fs.close();
    }
    #endif


    template<typename T>
    void LogMessage( const T data ) const{
        #ifdef PT_PLATFORM_LINUX
        LogMessageLinux<T>(data);
        #elif PT_PLATFORM_WINDOWS
        LogMessageWin<T>(data);
        #endif
    }

    const std::string& getFileName() const;

public:
    logstream();
    logstream( const std::string& prefix );
    virtual ~logstream(){}
    logstream(const logstream& other)=delete;
    logstream(logstream&& source)=delete;

    logstream& operator=(const logstream& other)=delete;
    logstream& operator=(logstream&& source)=delete;

    void setEnabled(bool val){
        mEnabled = val;
    }

    //void setFile(const char *filename);
    //void setFile(const std::string &filename);

    //TODO:
    // missing functionality:
    //  << std::hex, std::dec, etc.

    DEFINE_LOGSTREAM_OUT_OPERATOR(const bool)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const short)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const unsigned short)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const int)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const unsigned int)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const long)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const unsigned long)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const float)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const double)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const long double)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const void*)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const std::streambuf*)

    DEFINE_LOGSTREAM_OUT_OPERATOR(const char)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const char*)
    DEFINE_LOGSTREAM_OUT_OPERATOR(const std::string&)

    logstream& operator<<( const Name& data);

    DEFINE_LOGSTREAM_OUT_FUNC_OPERATOR(std::ostream&, (std::ostream&))
    DEFINE_LOGSTREAM_OUT_FUNC_OPERATOR(std::ios&, (std::ios&))
    DEFINE_LOGSTREAM_OUT_FUNC_OPERATOR(std::ios_base&, (std::ios_base&))

    template<class T>
    logstream& operator<<( const std::vector<T> list ){
        if( mEnabled ){
            std::ostringstream oss;
            oss << "[";
            size_t maxsize = list.size();
            for( size_t i=0; i<maxsize; ++i ){
                oss << list[i];
                if( i+1 < maxsize ){
                    oss << ", ";
                }
            }
            oss << "]";
            LogMessage<std::string>( oss.str() ); //TODO: change to 'oss.rdbuf()' after it's compatible
        }
        return *this;
    }

};

} //end of namespace 'log'
} //end of namespace 'pt'

