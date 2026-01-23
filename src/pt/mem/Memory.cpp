#include "pt/mem/Memory.h"

#include "pt/logging.h"
#include "pt/macros.h"
#include "pt/utility.hpp"

#include <array>
#include <cassert>
#include <vector>

//--------------------------------------------------
//  Private interface of Memory
//--------------------------------------------------
namespace pt{
namespace mem{
namespace heap{


//-----
std::vector<Block>  gOwnedBlocks;        // memory here needs to be freed on 'Quit()'
i32                 gCurrentBlockIdx;    // round-robin current idx

std::vector<std::deque<Block>>  gBlocksPool;

// TODO: reorder in .cpp correctly
u64 GetMaximumAvailableBlockCapacity() noexcept
{
    PT_UNIMPLEMENTED_FUNCTION
}

// TODO: reorder in .cpp correctly

Block RequestBlock(size_t capacity_) noexcept
{

}



} // end of namespace 'heap'


bool                gInitialized = false;



} } // end of namespace 'pt::mem'

//--------------------------------------------------

bool pt::mem::heap::
AllocateBlock( Block& block_, size_t capacity_ ) noexcept
{
    constexpr size_t minalign   = PT_MEM_ALIGNMENT_MINIMUM;

    // sanitize input
    assert( IsStub( block_ ) );
    if( !IsStub( block_ ) ) return false;

    // round up real capacity to nearest min alignment
    size_t realcap      = capacity_ + (minalign - (capacity_ % minalign)) % minalign;
    block_.mCapacity = capacity_;
    block_.mData     = (uintptr_t) malloc( realcap );    // drop this
    //block_.mData     = (uintptr_t) aligned_alloc( minalign, realcap ); // TODO: use this (set project to c++17)

    return true;
}


bool pt::mem::heap::
CreateBlocksFromMemory( Block* outBlocks_, u64 numBlocks_, size_t blockCapacity_, uintptr_t startPtr_, size_t bytes_ )
{
    assert( nullptr != outBlocks_ );
    assert( 0 < numBlocks_ );
    assert( 0 < blockCapacity_ );
    assert( 0 != startPtr_ );
    assert( numBlocks_*blockCapacity_ <= bytes_ );

    if( nullptr == outBlocks_ ) return false;
    if( 0 == numBlocks_ ) return false;
    if( 0 == blockCapacity_ ) return false;
    if( 0 == startPtr_ ) return false;
    if( bytes_ < numBlocks_*blockCapacity_ ) return false;

    for( u64 i=0; i<numBlocks_; ++i ){
        outBlocks_[i].mCapacity = blockCapacity_;
        outBlocks_[i].mData     = startPtr_ + i*blockCapacity_;
    }

    return true;
}


bool pt::mem::heap::
IsStub( Block& block_ ) noexcept
{
    return (0 == block_.mData) || (0 == block_.mCapacity);
}


// TODO: move up to match ordering in header
void pt::mem::heap::
DestroyBlock( Block& block_ ) noexcept
{
    assert( !IsStub( block_ ) );

    free( (void*) block_.mData );

    block_.mData     = 0;
    block_.mCapacity = 0;
}


bool pt::mem::heap::
ReturnBlock( Block& block_ ) noexcept
{
    const bool  capacityValid   = (0 < block_.mCapacity) && !IsPowerOfTwo( block_.mCapacity );
    const u64   capacityMax     = GetMaximumAvailableBlockCapacity();
    assert( !IsStub( block_ ) );
    assert( capacityValid );
    assert( block_.mCapacity <= capacityMax );

    // if Block data is invalid, don't give back to mem pool
    //  as it cannot have originated from the mem pool
    // - Block is empty or invalid
    if( IsStub( block_ )
        || !capacityValid
        || (capacityMax < block_.mCapacity)
    ){
        const char* msg = "MemManager: Tried to return invalid Block! Skipping.";
        PT_LOG_ERR( msg );
        PrintStackTrace( msg );
        return false;
    }

    auto pool = GetFreeBlocksOfSize( block_.mCapacity );
    pool.push_back( block_ );
    return true;
}


std::deque<HeapBlock>& pt::mem::heap::
GetBlocks() noexcept
{
    PT_UNIMPLEMENTED_FUNCTION
    //return heap::gBlocksPool;
    return std::deque<HeapBlock>();
}


u64 pt::mem::
GetRemainingBytes( Arena& a )
{
    // verify, that Arena ptr is inside Block range
    assert( a.mBlock.mData <= a.mPtr );
    assert( a.mPtr <+ a.mBlock.mData + a.mBlock.mCapacity );

    return a.mPtr - a.mBlock.mCapacity;
}



bool pt::mem::
Initialize()
{
    if( gInitialized ) return true;

    constexpr u64 countOwnedBlocksDefault = 64 / sizeof( heap::Block );
    heap::gOwnedBlocks.reserve( countOwnedBlocksDefault );
    heap::AllocateBlock( heap::gOwnedBlocks[0], PT_MEM_BLOCKSIZE_DEFAULT );

    gInitialized = true;
    return true;
}


void pt::mem::
Quit()
{
    if( !gInitialized ) return;

    for( auto& b : heap::gOwnedBlocks ){
        heap::DestroyBlock( b );
    }

    heap::gOwnedBlocks.clear();
    heap::gOwnedBlocks.shrink_to_fit();
    heap::gBlocksPool.clear();
    heap::gBlocksPool.shrink_to_fit();
    heap::gCurrentBlockIdx = 0;

    gInitialized = false;
}
