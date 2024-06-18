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
