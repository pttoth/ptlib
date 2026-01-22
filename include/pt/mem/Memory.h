#pragma once

#include "pt/alias.h"
#include "pt/macros.h"
#include "pt/utility.hpp"
#include <array>
#include <cassert>
#include <deque>
#include <functional>
#include <new>
#include <vector>


#ifndef PT_MEM_BLOCKSIZE_DEFAULT
    #define PT_MEM_BLOCKSIZE_DEFAULT 2*1024*1024 // 2 MB
    // TODO: add compile-time zero-check
#endif

#ifndef PT_MEM_ALIGNMENT_MINIMUM
    #define PT_MEM_ALIGNMENT_MINIMUM 4*1024     // 4 kbyte
    // TODO: add compile-time zero-check
#endif

namespace pt{
namespace mem{

//--------------------------------------------------
namespace heap{

// heap::Block
//  A piece of heap memory.
//  Makes no assumptions on usage whatsoever.
struct Block
{
    uintptr_t   mData       = 0;
    u64         mCapacity   = 0;
};

void    DestroyBlock( Block& ); // frees the memory block
void    ReturnBlock( Block& );  // returns the memory into ready pool

} // end of namespace 'heap'


//--------------------------------------------------

bool    Initialize();
void    Quit();

} // end of namespace 'mem'
} // end of namespace 'pt'
