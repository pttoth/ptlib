#include "pt/mem/Memory.h"
#include "pt/utility.hpp"
#include <cstdlib>
#include <cassert>

using HeapBlock = pt::mem::heap::Block;

//--------------------------------------------------
//  Private interface of Memory
//--------------------------------------------------
namespace pt{
namespace mem{
namespace heap{

bool    AllocateBlock( HeapBlock& outBlock_, size_t capacity_ );
void    CreateBlocksFromMemory( HeapBlock* outBlocks_, u64 numBlocks_, size_t blockCapacity_, uintptr_t startPtr_ );
std::deque<heap::Block>     GetBlocks();


//-----
std::vector<HeapBlock>  gOwnedBlocks;        // memory here needs to be freed on 'Quit()'
i32                     gCurrentBlockIdx;    // round-robin current idx

std::vector<std::deque<HeapBlock>>  gBlocksPool;

//std::deque<HeapBlock>   gBlocksPool;         // memory chunks, that can be assigned to Arenas

} // end of namespace 'heap'



u64 GetRemainingBytes( Arena& );    // TODO: find out, if needed


bool                    gInitialized = false;


} } // end of namespace 'pt::mem'

//--------------------------------------------------

bool pt::mem::heap::
AllocateBlock( Block& outBlock_, size_t capacity_ )
{
    const size_t minalign   = PT_MEM_ALIGNMENT_MINIMUM;
    const size_t cap        = capacity_;

    // sanitize input
    assert( 0 < cap );
    if( 0 == cap ) return false;

    // round up real capacity to nearest min alignment
    size_t realcap      = cap + (minalign - (cap % minalign)) % minalign;
    outBlock_.mCapacity = cap;
    //TODO: set project to c++17
    outBlock_.mData     = (uintptr_t) malloc( realcap );    // drop this
    //outBlock_.mData     = (uintptr_t) aligned_alloc( minalign, realcap ); // use this

    return true;
}


void pt::mem::heap::
CreateBlocksFromMemory( Block* outBlocks_, u64 numBlocks_, size_t blockCapacity_, uintptr_t startPtr_ )
{
    assert( nullptr != outBlocks_ );
    assert( 0 < numBlocks_ );
    assert( 0 < blockCapacity_ );
    assert( 0 != startPtr_ );

    if( nullptr == outBlocks_ ) return;
    if( 0 == startPtr_ ) return;
    if( 0 == blockCapacity_ ) return;

    for( u64 i=0; i<numBlocks_; ++i ){
        Block& current = outBlocks_[i];
        current.mCapacity   = blockCapacity_;
        current.mData       = startPtr_ + i*blockCapacity_;
    }
}


void pt::mem::heap::
DestroyBlock( Block& block_ )
{
    assert( 0 != block_.mData );
    assert( 0 < block_.mCapacity );

    free( (void*) block_.mData );

    block_.mData     = 0;
    block_.mCapacity = 0;
}


void pt::mem::heap::
ReturnBlock( Block& b )
{
    PT_UNIMPLEMENTED_FUNCTION
    auto pool = GetBlocks();



}


std::deque<HeapBlock> pt::mem::heap::
GetBlocks()
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


pt::mem::Arena pt::mem::
CreateArena( u64 bytes )
{
    Arena a;
    auto blocks = heap::GetBlocks();

    //a.mBlock = heap::GetBlocks


    return a;
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
