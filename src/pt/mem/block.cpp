#include "pt/mem/block.h"

#include "pt/mem/Memory.h"
#include <cassert>


bool pt::mem::
AllocateBlock( Block& block_, size_t capacity_ ) noexcept
{
    constexpr size_t minalign   = PT_MEM_ALIGNMENT_MINIMUM;

    // sanitize input
    assert( IsStub( block_ ) );
    if( !IsStub( block_ ) ) return false;

    // round up real capacity to nearest min alignment
    size_t realcap   = capacity_ + (minalign - (capacity_ % minalign)) % minalign;
    block_.mCapacity = capacity_;
    block_.mData     = (uintptr_t) malloc( realcap );    // drop this
    //block_.mData     = (uintptr_t) aligned_alloc( minalign, realcap ); // TODO: use this (set project to c++17)

    return true;
}


void pt::mem::
DestroyBlock( Block& block_ ) noexcept
{
    assert( !IsStub( block_ ) );

    free( (void*) block_.mData );

    block_.mData     = 0;
    block_.mCapacity = 0;
}


bool pt::mem::
IsStub( Block& block_ ) noexcept
{
    return (0 == block_.mData) || (0 == block_.mCapacity);
}
