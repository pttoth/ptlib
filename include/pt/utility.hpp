/** -----------------------------------------------------------------------------
  * FILE:    utility.hpp
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

namespace pt{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
template<typename T>
inline int
IndexOfInVector(const std::vector<T>& vec, const T& element)
{
    for(int i=0; i<vec.size(); ++i){
        if(vec[i] == element){
            return i;
        }
    }
    return -1;
}

#pragma GCC diagnostic pop

template<typename T>
inline bool
ContainedInVector(const std::vector<T>& vec, const T& element)
{
    return ( -1 < IndexOfInVector(vec, element) );
}

template<typename T>
inline void
RemoveElementInVector(std::vector<T>& vec, size_t pos)
{
    auto iter = vec.begin()+pos;
    vec.erase(iter);
}

bool IsEmptyOrWhitespaceLine(const std::string &str);

std::string TrimWhitespaces(const std::string& str);

bool SplitString(std::string* retval, const std::string& str, const char *sequence);
bool SplitString(std::string* retval, const std::string& str, const std::string& sequence);

std::string StringToLower(const std::string& str);
std::string StringToUpper(const std::string& str);

bool MatchRegex(const char* const str, const char* const regex_str);
bool MatchRegex(const char* str, const std::string& regex_str);
bool MatchRegex(const std::string& str, const char* regex_str);
bool MatchRegex(const std::string& str, const std::string& regex_str);

void CStrToWCStr(wchar_t* output, const char* const text, size_t size);
std::string WStringToUTF8(const std::wstring& wstring_to_convert);
std::wstring StringToWString(const std::string& string_to_convert);

void CreateDirectory(const std::string& path);
void EnsureExistingDirectory(const std::string& path);


} //end of namespace


//-------------------------------------------------------------------------




