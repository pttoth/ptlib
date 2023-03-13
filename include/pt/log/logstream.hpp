/*
 * -------------------------------------------------------------------------
 * FILE:    logstream.hpp
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE: Multiprocess logger, that writes simultaneously to a log window and a log file.
 * -------------------------------------------------------------------------
 */

#pragma once

#include "pt/utility.hpp"

#include <iostream>
#include <fstream>

//#include <experimental/filesystem>
//#include <filesystem>


// TODO: make it actually multiprocess !

namespace pt{

namespace cpp{
    //namespace fs = std::experimental::filesystem;
    //namespace fs = std::filesystem;
}

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

    #ifdef __linux__
    template<typename T>
    void LogMessageLinux(T data) const{
        //temporary solution
        //TODO: enqueue the message in the logger process instead
        //note: take care of race conditions between debug/out/warn/err

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
    #elif _WIN32 || _WIN64
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
    void LogMessage(T data) const{
        #ifdef __linux__
        LogMessageLinux<T>(data);
        #elif _WIN32 || _WIN64
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

    DEFINE_LOGSTREAM_OUT_FUNC_OPERATOR(std::ostream&, (std::ostream&))
    DEFINE_LOGSTREAM_OUT_FUNC_OPERATOR(std::ios&, (std::ios&))
    DEFINE_LOGSTREAM_OUT_FUNC_OPERATOR(std::ios_base&, (std::ios_base&))
};

} //end of namespace 'log'
} //end of namespace 'pt'

