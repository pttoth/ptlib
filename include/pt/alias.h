/** -----------------------------------------------------------------------------
  * FILE:    alias.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Alias definitions
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <memory>
#include <mutex>
#include <string>

namespace pt{
    using MutexLock = std::unique_lock< std::mutex >;
    using MutexLockGuard = std::lock_guard< std::mutex >;
    // TODO: add C++17 detector macro here
    //using MutexScopedLock = std::scoped_lock< std::mutex >;

}

// shared pointer aliases for 'std::string'
using StdStringPtr        = std::shared_ptr< std::string >;
using ConstStdStringPtr   = std::shared_ptr< const std::string >;
using StdStringWPtr       = std::weak_ptr< std::string >;
using ConstStdStringWPtr  = std::weak_ptr< const std::string >;
using StdStringUPtr       = std::unique_ptr< std::string >;
using ConstStdStringUPtr  = std::unique_ptr< const std::string >;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

// Verify we're not on exotic platforms
static_assert(sizeof(f32) == 4, "float is not 32-bit");
static_assert(sizeof(f64) == 8, "double is not 64-bit");
static_assert(std::numeric_limits<f32>::is_iec559,  "float is not IEEE 754 binary32");
static_assert(std::numeric_limits<f32>::digits10 == 6,  "unexpected float precision");
static_assert(std::numeric_limits<f64>::is_iec559, "double is not IEEE 754 binary64");
static_assert(std::numeric_limits<f64>::digits10 == 15, "unexpected double precision");
