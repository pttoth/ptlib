//-----------------------------------------------------------------------------
// FILE:    utility.hpp
// AUTHOR:  ptoth
// EMAIL:   peter.t.toth92@gmail.com
// PURPOSE: Contains some utility functions for which
//          the standard STL methods are more or less messy
//-----------------------------------------------------------------------------


// TODO: clean up ordering, formatting, overall readability!

// TODO: add constexpr to functions where applicable!

#pragma once

#include "pt/alias.h"
#include "pt/def.h"
#include "pt/util/countl_zero.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <string>
#include <sstream>
#include <vector>


// alias for shortening 'std::make_shared' calls
template<typename T, typename... Signature>
std::shared_ptr<T> NewPtr( Signature... params ){
    return std::make_shared<T>( params... );
}


namespace pt{

void PrintStackTrace( const std::string& additional_message );
void PrintStackTrace( const char* additional_message = "" );


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

template<typename T>
inline std::string
GetVectorAsString( const std::vector<T>& vec )
{
    std::stringstream ss;
    ss << "(";
    for( auto& elem : vec ){
        ss << "'" << elem << "', ";
    }
    ss << ")";
    return ss.str();
}


bool IsCharDigit(char c);
bool IsEmptyOrWhitespaceLine(const std::string& str);

constexpr bool
IsPowerOfTwo( uint64_t n ) noexcept
{
   return (n > 0) && (n & (n - 1)) == 0;
}


/**
 * @brief IsStringNumeric:  Determines, whether the string is a number in text format.
 *                           Supports negative numbers. Doesn't support whitespaces.
 */
bool IsStringNumeric(const std::string& str);

std::string TrimWhitespaces(const std::string& str);

bool SplitString(std::string* retval, const std::string& str, const char *sequence);
bool SplitString(std::string* retval, const std::string& str, const std::string& sequence);


// Doesn't transform multi-byte (non-english) characters
std::string StringToLower(const std::string& str);
std::string StringToUpper(const std::string& str);

std::string StringPrefix( const std::string& str, size_t count );
std::string StringPostfix( const std::string& str, size_t count );

bool MatchRegex(const char* const str, const char* const regex_str);
bool MatchRegex(const char* str, const std::string& regex_str);
bool MatchRegex(const std::string& str, const char* regex_str);
bool MatchRegex(const std::string& str, const std::string& regex_str);

void CStrToWCStr(wchar_t* output, const char* const text, size_t size);
std::string WStringToUTF8(const std::wstring& wstring_to_convert);
std::wstring StringToWString(const std::string& string_to_convert);

void MakeDirectory(const std::string& path);
void EnsureExistingDirectory(const std::string& path);

template<typename T>
const T& Clamp( const T& v, const T& lo, const T& hi ){
    return std::min( std::max( lo, v ), hi );
}


/**
 * @brief Sleep for 'time_ms' milliseconds.
 */
void SleepMS( size_t time_ms );


/**
 * @brief Reads all text contents of file 'path'
 *          and returns it as std::string.
 */
std::string ReadTextFile( const std::string& path );

/**
 * @brief Reads all bytes of file 'path'
 *          and returns it as an std::vector <uint8_t>.
 */
std::vector<uint8_t> ReadBinaryFile( const std::string& path );


// MurmurHash2
//   source: https://github.com/aappleby/smhasher
uint32_t MurmurHash2( const void* key, int len, uint32_t seed );


// RoundToNextPowerOfTwo
//  Returns the smallest, power of 2 number, that is greater than or equal to 'n'.
//  Edge case: n == 0 -> out: 1
//  return - guaranteed nonzero uint
constexpr u32
RoundToNextPowerOfTwo( u64 n ) noexcept
{
#undef PT_COUNTL_ZERO_64_HARDWARE_SUPPORTED
#ifdef PT_COUNTL_ZERO_64_HARDWARE_SUPPORTED
    if( n <= 1 ) return 1;
    return u32(1) << (64 - pt::util::countl_zero64( n-1 ));
#else
    // No compiler or platform-specific code.
    //  Significantly slower than 'countl_zero()' with hardware support,
    //  but faster than the software implementation of 'countl_zero()'.
    if (n == 0) return 1;
    assert( n <= (1ULL << 63) );
    if (n > (1ULL << 63)) return 0; // overflow

    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    return n + 1;
#endif
}


// ExponentOfPowerOfTwo64_NoCheck
//  Optimal log2 function for known power-of-two input.
//  'n' must be a power of two, otherwise, undefined behavior.
//  Caller must sanitize input! Use 'ExponentOfPowerOfTwo64()' if needed.
constexpr u32 ExponentOfPowerOfTwo64_NoCheck( u64 n ) noexcept
{
    assert( 0 < n );
    assert( pt::IsPowerOfTwo( n ) );
    // TODO: can be more optimal, if pt::util::countr_zero64() is implemented.
    return 63 - pt::util::countl_zero64( n );
}


// ExponentOfPowerOfTwo64
//  Optimal log2 function for known power-of-two input.
//  'n' must be a power of two, otherwise, the function returns 0.
constexpr u32 ExponentOfPowerOfTwo64( u64 n ) noexcept
{
    assert( 0 < n );
    if( 0 == n ) return 64;

    bool isPowTwo = pt::IsPowerOfTwo( n );
    assert( isPowTwo );
    if( !isPowTwo ) return 0;

    return ExponentOfPowerOfTwo64_NoCheck( n );
}


template<typename T>
inline uintptr_t
AlignAs( uintptr_t p_ )
{
    return (p_ + alignof(T) - 1) & ~(alignof(T) - 1);
}

} //end of namespace 'pt'


//-------------------------------------------------------------------------




