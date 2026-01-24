/** -----------------------------------------------------------------------------
  * FILE:    block.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: A piece of heap memory. Makes no assumptions on usage whatsoever.
  *     Avoids RAII -> Blocks can point to subsections of other Blocks.
  *     Useful in managing lifecycles of nicely sized chunks to avoid mem fragmentation.
  * -----------------------------------------------------------------------------
  */

#pragma once

#include "pt/alias.h"

namespace pt{
namespace mem{

//--------------------------------------------------
//  Macros and static asserts
//--------------------------------------------------
namespace helper{
    constexpr bool IsPowerOfTwo( uint64_t n ) noexcept{
       return (n > 0) && (n & (n - 1)) == 0;
    }
} // end of namespace 'helper'
//------------------------------

// PT_MEM_ALIGNMENT_MINIMUM
//  Defines minimum memory alignment for allocations with 'AllocateBlock()'
//  Must be power of two.
#ifndef PT_MEM_ALIGNMENT_MINIMUM
    #define PT_MEM_ALIGNMENT_MINIMUM 4*1024     // 4 kbyte
#endif

// 'MinimumAlignment'
//  Type-safe copy of 'PT_MEM_ALIGNMENT_MINIMUM'
constexpr u64 MinimumAlignment = PT_MEM_ALIGNMENT_MINIMUM;

//------------------------------
// Ensure power-of-two requirement of 'PT_MEM_ALIGNMENT_MINIMUM'
static_assert( pt::mem::helper::IsPowerOfTwo( MinimumAlignment ),
              "'PT_MEM_ALIGNMENT_MINIMUM' must be a power of two");

//--------------------------------------------------
//  Block
//--------------------------------------------------
struct Block
{
    uintptr_t   mData       = 0;
    u64         mCapacity   = 0;
};

// AllocateBlock
//  Allocates 'capacity_' of bytes and returns a Block pointing to it.
bool    AllocateBlock( Block& outBlock_, size_t capacity_ ) noexcept;

// DestroyBlock
//  Frees the memory pointed to by 'Block::mData'.
//  Then makes the Block instance a stub.
void    DestroyBlock( Block& ) noexcept; // frees the memory block

// IsStub
//  Determines whether Block is a stub (pointer or capacity is 0).
bool    IsStub( const Block& ) noexcept;


} // end of namespace 'mem'
} // end of namespace 'pt'
