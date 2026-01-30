//-----------------------------------------------------------------------------
// FILE:    countl_zero.h
// AUTHOR:  ptoth
// EMAIL:   peter.t.toth92@gmail.com
// PURPOSE: Multiplatform, C++14 compatible replacements of (C++20)'std::countl_zero'
//  Supports GCC/Clang/MSVC | x86/x86_64/ARM/ARM64 | Linux/Windows
//-----------------------------------------------------------------------------

#pragma once

#include <cstdint>

#if defined(_MSC_VER)
    #include <intrin.h>
#endif

namespace pt{
namespace util{
namespace platformspecific{

//-------------------------
// 32-bit
//-------------------------
// fail on ARM to mandate review here
#if defined(_M_ARM) || defined(_M_ARM64) || defined(__arm__) || defined(__aarch64__)
#error "Need to verify ARM support!"
//  see the implemented MSVC version!
#endif

// GCC, Clang
#if defined(__GNUC__) || defined(__clang__)
#define PT_COUNTL_ZERO_32_GCC_CLANG_SUPPORTED
constexpr int
countl_zero32_gcc_clang( uint32_t x ) noexcept
{
    // '__builtin_clz()' measures unsigned int, not strictly 32 bits!
    static_assert( sizeof(unsigned int) == sizeof(uint32_t), "'unsigned int' is not 32 bits!" );
    if ( x == 0 ) return 32;
    return __builtin_clz( x );
}
#endif


// MSVC on x86 / ARM
//  (ARM supported since VS 2017+)
//  TODO: ARM might break!
#if defined(_MSC_VER)
#define PT_COUNTL_ZERO_32_MSVC_SUPPORTED
constexpr int
countl_zero32_msvc( uint32_t x ) noexcept
{
    if ( x == 0 ) return 32;
    unsigned long index;
    _BitScanReverse( &index, x );
    return 31 - static_cast<int>( index );
}
#endif

// Fallback for compilers without the above builtins
// (unlikely case on GCC/Clang/MSVC + x64/AArch64)
// Fully portable, but slow!
constexpr int
countl_zero32_compat( uint32_t x ) noexcept
{
    if ( x == 0 ) return 32;

    int count = 32;
    uint32_t bit = 1u << 31;
    while ((x & bit) == 0) {
        bit >>= 1;
        --count;
        //if (count == 0) break;  // already checked at function start
    }
    return count;
}

//-------------------------
// 64-bit
//-------------------------
// fail on ARM to mandate review here
#if defined(_M_ARM) || defined(_M_ARM64) || defined(__arm__) || defined(__aarch64__)
#error "Need to verify ARM support!"
//  see the implemented MSVC version!
#endif

// GCC, Clang, everywhere that supports 64-bit
// - clz instruction on x86-64 / AArch64 / ARM64
#if defined(__GNUC__) || defined(__clang__)
#define PT_COUNTL_ZERO_64_GCC_CLANG_SUPPORTED
constexpr int
countl_zero64_gcc_clang( uint64_t x ) noexcept
{
    // '__builtin_clzll()' measures ull, not strictly 64 bits!
    static_assert( sizeof(unsigned long long) == sizeof(uint64_t), "'unsigned long long' is not 64 bits!" );

    if ( x == 0 ) return 64;
    return __builtin_clzll( x );
}
#endif


// MSVC on x64 / ARM64
//  (ARM supported since VS 2017+)
//  TODO: ARM64 might break!
#if defined(_MSC_VER)
#define PT_COUNTL_ZERO_64_MSVC_SUPPORTED
constexpr int
countl_zero64_msvc( uint64_t x ) noexcept
{
    if ( x == 0 ) return 64;
    unsigned long index;
    _BitScanReverse64( &index, x );
    return 63 - static_cast<int>( index );
}
#endif


// Fallback for compilers without the above builtins
// (unlikely case on GCC/Clang/MSVC + x64/AArch64)
// Fully portable, but slow!
constexpr int
countl_zero64_compat( uint64_t x ) noexcept
{
    if ( x == 0 ) return 64;

    int count = 64;
    uint64_t bit = 1ULL << 63;
    while ((x & bit) == 0) {
        bit >>= 1;
        --count;
        //if (count == 0) break;  // already checked at function start
    }
    return count;
}


} // end of namespace 'platformspecific'


#if defined (PT_COUNTL_ZERO_32_GCC_CLANG_SUPPORTED) || defined (PT_COUNTL_ZERO_32_MSVC_SUPPORTED)
    #define PT_COUNTL_ZERO_32_HARDWARE_SUPPORTED
#endif


#if defined (PT_COUNTL_ZERO_64_GCC_CLANG_SUPPORTED) || defined (PT_COUNTL_ZERO_64_MSVC_SUPPORTED)
    #define PT_COUNTL_ZERO_64_HARDWARE_SUPPORTED
#endif


constexpr int
countl_zero32( uint32_t x ) noexcept
{
#if defined(PT_COUNTL_ZERO_32_GCC_CLANG_SUPPORTED)
    return platformspecific::countl_zero32_gcc_clang( x );
#elif defined(PT_COUNTL_ZERO_32_MSVC_SUPPORTED)
    return platformspecific::countl_zero32_msvc( x );
#else
    #if defined(_MSC_VER)
        #pragma message("countl_zero32() falls back to slow implementation!")
    #else
        #warning "countl_zero32() falls back to slow implementation!"
    #endif
    return platformspecific::countl_zero32_compat( x );
#endif
}


constexpr int
countl_zero64( uint64_t x ) noexcept
{
#if defined(PT_COUNTL_ZERO_64_GCC_CLANG_SUPPORTED)
    return platformspecific::countl_zero64_gcc_clang( x );
#elif defined(PT_COUNTL_ZERO_64_MSVC_SUPPORTED)
    return platformspecific::countl_zero64_msvc( x );
#else
    #if defined(_MSC_VER)
        #pragma message("countl_zero64() falls back to slow implementation!")
    #else
        #warning "countl_zero64() falls back to slow implementation!"
    #endif
    return platformspecific::countl_zero64_compat( x );
#endif
}


} // end of namespace 'util'
} // end of namespace 'pt'
