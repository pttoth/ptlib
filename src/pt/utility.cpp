#include "pt/utility.hpp"

#include <locale>
#include <codecvt>

#include <sstream>

#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>



inline void
CreateDirectoryLinux(const std::string& path)
{
    int result = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
                                    //  r/w/search for usr + group
                                    //  r/search for other
    //if not created and doesn't exist already, it's a relevant error
    if( !(0 == result)
        && !((-1 == result) && (EEXIST == errno)) )
    {
        std::stringstream ss;
        ss << "Error creating directory: " << path << "\n"
              << "    result(" << result << "), errno(" << errno << ")";
        throw std::invalid_argument(ss.str());
    }
}


#elif _WIN32 || _WIN64


inline void
CreateDirectoryWin(const std::string& path)
{
    //TODO: implement
    throw std::runtime_error("CreateDirectory has not yet been implemented for Windows!\n    File logging won't be available, unless directories already exist!");
}


#endif


void
CreateDirectory(const std::string& path)
{
    #ifdef __linux__
    return CreateDirectoryLinux(path);
    #elif _WIN32 || _WIN64
    return CreateDirectoryWin(path);
    #endif
}


void
EnsureExistingDirectory(const std::string& path)
{
    std::string dir;
    dir.reserve(path.size());
    size_t prev = 0;
    size_t curr = 0;

    //TODO: transform path to uniform format
    // eg: '\' to '/'
    //std::transform(...

    //TODO: catch std::invalid_arguments !
    //create directories one by one
    while( (curr = path.find('/', prev+1)) != std::string::npos ){
        dir += path.substr(prev, curr-prev);

        CreateDirectory(dir); //throws std::invalid_arguments!
        prev = curr;
    }
}


void
CStrToWCStr(wchar_t* output, const char* const text, size_t size)
{
    mbstowcs(output, text, size);
    output[size-1] = 0; //enforce closing '0' in all cases
                        //shortens output if user forgot to allocate for the nullterminator
}


std::string
WStringToUTF8(const std::wstring& wstring_to_convert)
{
    //setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.to_bytes( wstring_to_convert );
}


std::wstring
StringToWString(const std::string& string_to_convert)
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    //std::string narrow = converter.to_bytes(wide_utf16_source_string);
    std::wstring wide = converter.from_bytes(string_to_convert);
    return wide;
    /*
    //setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.from_bytes( string_to_convert );
    */
}



