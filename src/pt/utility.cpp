#include "pt/utility.hpp"

//-----linux-----
#ifdef PT_PLATFORM_LINUX
#include <unistd.h>

//----windows----
#elif defined PT_PLATFORM_WINDOWS
#include<windows.h>

//------mac------
#elif defined PT_PLATFORM_MAC
//...
//---------------
#endif

#include <locale>
#include <codecvt>

#include <assert.h>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <regex>


bool pt::
IsCharDigit(char c)
{
    return !((c < '0') || ('9' < c));
}


bool pt::
IsEmptyOrWhitespaceLine(const std::string &str)
{
    for(char c : str){
        if( !isspace(c) ){ return false; }
    }
    return true;
}


bool pt::
IsStringNumeric(const std::string &str)
{
    //not empty
    if( 0 == str.length() ){
        return false;
    }

    std::string::const_iterator it = str.begin();

    //may start with '-'
    if( '-' == *it ){
        ++it;
    }

    //contains at least 1 number
    if( !IsCharDigit(*it) ){
        return false;
    }
    ++it;

    //rest may contain only numbers
    while( it != str.end() ){
        if( !IsCharDigit(*it) ){
            return false;
        }
        ++it;
    }
    return true;
}


std::string pt::
TrimWhitespaces(const std::string& str)
{
    if( 0 < str.length() ){
        size_t low = 0;
        size_t high = str.length()-1;
        bool done = false;
        while(!done){
            if( (low < str.length()) && isspace(str[low]) ){
                ++low;
            }else{
                done = true;
            }
        }
        done = false;
        while(!done){
            if( (low <= high) && isspace(str[high]) ){
                --high;
            }else{
                done = true;
            }
        }
        if(low <= high){
            return str.substr(low, high-low+1);
        }
    }
    return std::string();
}


bool pt::
SplitString(std::string* retval,
            const std::string& str,
            const char *sequence)
{
    size_t idx      = str.find(sequence);
    size_t seq_len  = strlen(sequence);
    if(std::string::npos != idx){
        retval[0] = str.substr(0, idx);
        retval[1] = str.substr(idx+seq_len, str.length()-idx-1);
        return true;
    }
    return false;
}


bool pt::
SplitString(std::string* retval,
            const std::string& str,
            const std::string& sequence)
{
    return SplitString(retval, str, sequence.c_str());
}


std::string pt::
StringToLower(const std::string& str)
{
    std::string result;
    const size_t len = str.length();
    result.resize( len );

    //a note on UTF-8 support:
    //  [a-zA-Z] falls between 0-127
    //  therefore, this implementation never breaks UTF-8 encoding
    //    but skips transforming special, non-ASCII characters
    //  see: https://en.wikipedia.org/wiki/UTF-8#Encoding

    // branchless (except the cycle condition, which is prediction-friendly)
    const char diff = 'a' - 'A'; // 97 - 65
    for( size_t i=0; i<len; ++i ){
        const char& c = str[i];
        result[i] = c + ( ('A'<=c) && (c<='Z') ) * diff;
    }
    return result;
}


std::string pt::
StringToUpper(const std::string& str)
{
    std::string result;
    const size_t len = str.length();
    result.resize( len );

    //a note on UTF-8 support:
    //  [a-zA-Z] falls between 0-127
    //  therefore, this implementation never breaks UTF-8 encoding
    //    but skips transforming special, non-ASCII characters
    //  see: https://en.wikipedia.org/wiki/UTF-8#Encoding

    // branchless (except the cycle condition, which is prediction-friendly)
    const char diff = 'a' - 'A'; // 97 - 65
    for( size_t i=0; i<len; ++i ){
        const char& c = str[i];
        result[i] = c - ( ('a'<=c) && (c<='z') ) * diff;
    }
    return result;
}


bool pt::
MatchRegex(const char* const str, const char* const regex_str)
{
    std::regex reg(regex_str);
    return std::regex_match(str, reg);
}


bool pt::
MatchRegex(const char* str, const std::string& regex_str)
{
    return MatchRegex(str, regex_str.c_str());
}


bool pt::
MatchRegex(const std::string& str, const char* regex_str)
{
    return MatchRegex(str.c_str(), regex_str);
}


bool pt::
MatchRegex(const std::string& str, const std::string& regex_str)
{
    return MatchRegex(str.c_str(), regex_str.c_str());
}


#ifdef PT_PLATFORM_LINUX
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

#elif defined PT_PLATFORM_WINDOWS

inline void
CreateDirectoryWin(const std::string& path)
{
    //TODO: implement
    throw std::runtime_error("CreateDirectory has not yet been implemented for Windows!\n    File logging won't be available, unless directories already exist!");
}

#elif defined PT_PLATFORM_MAC
#error "pt::CreateDirectory() is not defined for Mac platform!"
#endif


void pt::
CreateDirectory(const std::string& path)
{
    #ifdef PT_PLATFORM_LINUX
    return CreateDirectoryLinux(path);
    #elif defined PT_PLATFORM_WINDOWS
    return CreateDirectoryWin(path);
    #elif defined PT_PLATFORM_MAC
    return CreateDirectoryMac(path);
    #endif
}


void pt::
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


void pt::
CStrToWCStr(wchar_t* output, const char* const text, size_t size)
{
    mbstowcs(output, text, size);
    output[size-1] = 0; //enforce closing '0' in all cases
                        //shortens output if user forgot to allocate for the nullterminator
}


std::string pt::
WStringToUTF8(const std::wstring& wstring_to_convert)
{
    //setup converter
    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
    return converter.to_bytes( wstring_to_convert );
}


std::wstring pt::
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


void pt::
Sleep( size_t time_ms )
{
    #ifdef PT_PLATFORM_LINUX
    usleep( time_ms * 1000 );
    #elif defined PT_PLATFORM_WINDOWS
    Sleep( time_ms );
    #elif defined PT_PLATFORM_MAC
    #error "pt::Sleep( size_t ) is not defined for Mac platform!"
    #endif
}


uint32_t pt::
MurmurHash2( const void* key, int len, uint32_t seed )
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  // Initialize the hash to a 'random' value

  uint32_t h = seed ^ len;

  // Mix 4 bytes at a time into the hash

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
      h *= m;
  };

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}


