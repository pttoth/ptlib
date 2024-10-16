#include "pt/logging.h"

#include "pt/def.h"
#include "pt/utility.hpp"

#ifdef PT_PLATFORM_LINUX
#include <sys/types.h>
#include <sys/stat.h>

#elif defined PT_PLATFORM_WINDOWS
#include <windows.h>
#include <tchar.h>
#include <windef.h>
#include <winuser.h>
#include <winerror.h>
#include <handleapi.h>
#include <errhandlingapi.h>
#include <processthreadsapi.h>
#include <synchapi.h>
//#include "win/logger_win_msgdata.h"
//#include "win/common.h"
#endif



#include <locale>
#include <codecvt>


#include <stdio.h>
//#include <strsafe.h>

#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

#include <string>

#include <cstdint>

#include <sstream>

#include <assert.h>



#include <chrono>
#include <iomanip>
#include <algorithm>




//initializing externs
pt::log::logstream pt::log::debug{ "Debug" };
pt::log::logstream pt::log::out{ "Log" };
pt::log::logstream pt::log::warn{ "Warning" };
pt::log::logstream pt::log::err{ "ERROR" };


std::string gRootDirectory;
std::string gFilePath;


std::string pt::log::
AutoGenerateLogFileName()
{
    std::time_t now_c = std::chrono::system_clock::to_time_t(
                        std::chrono::system_clock::now() );

    std::stringstream ss;
    ss << "log-";
    ss << std::put_time( std::localtime( &now_c ), "%Y-%m-%d_%H-%M-%S" );

    std::string filename( ss.str() );
    filename.append(".txt");

    return filename;
}

using namespace pt;
using namespace pt::log;



//TODO: this may be combined with the windows path validation code
inline bool
ValidatePathLinux(const std::string& path,
                  const std::string& rootdir)
{
    if(path.length() == 0 || rootdir.length() == 0){
        return false;
    }

    if ((rootdir.find("..") != std::string::npos)
       ||(path.find("..") != std::string::npos))
    {
        return false;
    }

    //std::string fullpath = rootdir + path;
    //TODO: do full path validation here too

    return true;
}


#ifdef PT_PLATFORM_LINUX
bool
InitializeLinux(const std::string& root_directory,
                const std::string& filename,
                uint32_t timeout)
{
    //bool success = false;

    if(!ValidatePathLinux(filename, root_directory)){
        return false;
    }


    //TODO: initialize multiprocess structure
    EnsureExistingDirectory(filename);

    LoadSettings();
    return true;
}




#elif defined PT_PLATFORM_WINDOWS


//need valid filename check? can be a security risk?
//currently:
//  ./<rel_path>/filename.ext
//invalid:
//  absolute paths
//  anything containing '..'
//TODO: think through, what needs to be checked!
//TODO: switch params to UTF-8 encoding instead of wchar/wstring
inline bool
ValidatePathWin(const std::string& path,
                const std::string& rootdir)
{
    if(path.length() == 0 || rootdir.length() == 0){
        return false;
    }

    if ((path.find("..") != std::string::npos))
    {
        return false;
    }

    //std::string fullpath = rootdir + path;
    //TODO: do full path validation here too

    return true;
}


//ptoth note: doesn't handle spaces in filenames!
bool
InitializeWin(const std::string& root_directory,
              const std::string& filename,
              uint32_t timeout)
{
    //bool success = false;

    if(!ValidatePathWin(filename, root_directory)){
        return false;
    }

    EnsureExistingDirectory(filename);

    //TODO: initialize multiprocess structure

    LoadSettings();
    return true;
}


#endif


bool pt::log::
Initialize(const std::string &root_directory,
           const std::string &filename,
           uint32_t timeout)
{
    bool success = false;

    //TODO: uniformize path (switch '\'-s with '/', etc.)

    //TODO: if uniform root doesn't end in '/', then add it between 'root' and 'filename'
    std::string fullpath = root_directory + filename;

    #ifdef PT_PLATFORM_LINUX
    success = InitializeLinux(root_directory, filename, timeout);
    #elif defined PT_PLATFORM_WINDOWS
    success = InitializeWin(root_directory, filename, timeout);
    #endif

    if(!success){
        return false;
    }

    gRootDirectory = root_directory;
    gFilePath = fullpath;

    return true;
}


void pt::log::
Destroy()
{
    //TODO: implement
}

void pt::log::
LoadSettings()
{
    //TODO: temp solution, replace
    //noexcept?
    pt::log::debug.setEnabled(true);
    pt::log::out.setEnabled(true);
    pt::log::warn.setEnabled(true);
    pt::log::err.setEnabled(true);
}


//--------------------------------------------------


const std::string& pt::log::
GetFileName()
{
    return gFilePath;
}

const std::string& pt::log::
GetRootDirectory()
{
    return gRootDirectory;
}
