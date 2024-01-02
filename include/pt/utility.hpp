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
#include <cstdint>

//return the enum name in parameter as string
#define EnumToString(ENUM_NAME) #ENUM_NAME

namespace pt{

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
template<typename T>
inline int64_t
IndexOfInVector(const std::vector<T>& vec, const T& element)
{
    for(int64_t i=0; i<vec.size(); ++i){
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
inline bool
PushBackIfNotInVector( std::vector<T>& vec, T& element )
{
    if( not pt::ContainedInVector( vec, element ) ){
        vec.push_back( element );
        return true;
    }
    return false;
}


template<typename T>
inline void
RemoveElementInVector(std::vector<T>& vec, size_t pos)
{
    auto iter = vec.begin()+pos;
    vec.erase(iter);
}

bool IsCharDigit(char c);
bool IsEmptyOrWhitespaceLine(const std::string& str);

/**
 * @brief IsStringNumeric:  Determines, whether the string is a number in text format.
 *                           Supports negative numbers. Doesn't support whitespaces.
 */
bool IsStringNumeric(const std::string& str);

std::string TrimWhitespaces(const std::string& str);

bool SplitString(std::string* retval, const std::string& str, const char *sequence);
bool SplitString(std::string* retval, const std::string& str, const std::string& sequence);


// doesn't work with non-ASCII chars, skips transforming multi-byte characters
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

template<typename T>
const T& Clamp( const T& v, const T& lo, const T& hi ){
    return std::min( std::max( lo, v ), hi );
}

// MurmurHash2
//   source: https://github.com/aappleby/smhasher
uint32_t MurmurHash2( const void* key, int len, uint32_t seed );


} //end of namespace 'pt'


//-------------------------------------------------------------------------




