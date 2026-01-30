//-----------------------------------------------------------------------------
// FILE:    block.h
// AUTHOR:  ptoth
// EMAIL:   peter.t.toth92@gmail.com
// PURPOSE: A piece of heap memory. Makes no assumptions on usage whatsoever.
//  Avoids RAII -> Blocks can point to subsections of other Blocks.
//  Useful in managing lifecycles of nicely sized chunks to avoid mem fragmentation.
//-----------------------------------------------------------------------------

#pragma once

#include "pt/alias.h"

namespace pt{
namespace mem{
//--------------------------------------------------
//  Block
//--------------------------------------------------
struct Block
{
    uintptr_t   mData       = 0;
    u64         mCapacity   = 0;

    Block() noexcept                        = default;
    ~Block() noexcept                       = default; // no virtual!
    Block(const Block&) noexcept            = default;
    Block(Block&&) noexcept                 = default;
    Block& operator=(const Block&) noexcept = default;
    Block& operator=(Block&&) noexcept      = default;
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
