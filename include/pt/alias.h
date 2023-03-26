/** -----------------------------------------------------------------------------
  * FILE:    alias.h
  * AUTHOR:  pttoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Alias definitions
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <mutex>

namespace pt{
    using MutexLock = std::unique_lock< std::mutex >;
    using MutexLockGuard = std::lock_guard< std::mutex >;
    //C++17
    //using MutexScopedLock = std::scoped_lock< std::mutex >;
}
