/** -----------------------------------------------------------------------------
  * FILE:    block.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: A piece of heap memory.
  *     Makes no assumptions on usage whatsoever.
  * -----------------------------------------------------------------------------
  */

#pragma once

#include "pt/alias.h"

namespace pt{
namespace mem{

struct Block
{
    uintptr_t   mData       = 0;
    u64         mCapacity   = 0;
};

/**
 * @brief AllocateBlock
 *  Allocates 'capacity_' of bytes and returns a Block pointing to it.
 */
bool    AllocateBlock( Block& outBlock_, size_t capacity_ ) noexcept;

/**
 * @brief DestroyBlock
 *  Frees the memory pointed to by 'Block::mData'.
 *  Then makes the Block instance a stub.
 */
void    DestroyBlock( Block& ) noexcept; // frees the memory block

/**
 * @brief IsStub
 *  Determines whether Block is a stub (pointer or capacity is 0).
 */
bool    IsStub( Block& ) noexcept;


} // end of namespace 'mem'
} // end of namespace 'pt'
