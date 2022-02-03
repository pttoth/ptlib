/** -----------------------------------------------------------------------------
  * FILE:    std_extension.hpp
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Contains some utility functions for which
  *          the standard STL methods are more or less messy
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <cstddef>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <regex>

namespace pt{

template<typename T>
inline int
IndexOfInVector(std::vector<T>& vec, T& element)
{
    for(int i=0; i<vec.size(); ++i){
        if(vec[i] == element){
            return i;
        }
    }
    return -1;
}

template<typename T>
inline bool
ContainedInVector(std::vector<T>& vec, T& element)
{
    return ( -1 < indexOfInVector(vec, element) );
}

template<typename T>
inline void
RemoveElementInVector(std::vector<T>& vec, size_t pos)
{
    auto iter = vec.begin()+pos;
    vec.erase(iter);
}

inline bool
IsEmptyOrWhitespaceLine(const std::string &str)
{
    for(char c : str){
        if( !isspace(c) ){ return false; }
    }
    return true;
}

inline std::string
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

inline bool
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

inline bool
SplitString(std::string* retval,
            const std::string& str,
            const std::string& sequence)
{
    return SplitString(retval, str, sequence.c_str());
}

inline std::string
StringToLower(const std::string& str)
{
    std::string result;
    result.reserve(str.length());
    std::transform(str.begin(), str.end(), result.begin(), ::tolower);
    return std::move(result);
}

inline std::string
StringToUpper(const std::string& str)
{
    std::string result;
    result.reserve(str.length());
    std::transform(str.begin(), str.end(), result.begin(), ::toupper);
    return std::move(result);
}


inline bool
MatchRegex(const char* const str, const char* const regex_str)
{
    std::regex reg(regex_str);
    return std::regex_match(str, reg);
}

inline bool
MatchRegex(const char* str, const std::string& regex_str)
{
    MatchRegex(str, regex_str.c_str());
}

inline bool
MatchRegex(const std::string& str, const char* regex_str)
{
    MatchRegex(str.c_str(), regex_str);
}

inline bool
MatchRegex(const std::string& str, const std::string& regex_str)
{
    MatchRegex(str.c_str(), regex_str.c_str());
}


} //end of namespace


//-------------------------------------------------------------------------
#include <string>
#include <assert.h>

#include <locale>
#include <codecvt>

#include <cstring>




void CStrToWCStr(wchar_t* output, const char* const text, size_t size);
std::string WStringToUTF8(const std::wstring& wstring_to_convert);
std::wstring StringToWString(const std::string& string_to_convert);

void CreateDirectory(const std::string& path);
void EnsureExistingDirectory(const std::string& path);


