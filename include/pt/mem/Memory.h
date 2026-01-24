#pragma once

#include "pt/alias.h"
#include <deque>

#ifndef PT_MEM_BLOCKSIZE_DEFAULT
    #define PT_MEM_BLOCKSIZE_DEFAULT 2*1024*1024 // 2 MB
    // TODO: add compile-time zero-check
#endif

#ifndef PT_MEM_ALIGNMENT_MINIMUM
    #define PT_MEM_ALIGNMENT_MINIMUM 4*1024     // 4 kbyte
    // TODO: add compile-time zero-check
#endif

namespace pt{
// TODO: delete 'mem' namespace
namespace mem{

//--------------------------------------------------
namespace heap{


/**
 * @brief CreateBlocksFromMemory
 *  Create 'numBlocks_' amount of Blocks that sequentially point to memory at 'startPtr_'.
 *  Does not allocate memory!
 * @param outBlocks_:   C-array of Blocks (at least 'numBlocks_' instances), the output is written here. Pre-allocated by caller.
 * @param numBlocks_:   Amount of Blocks to create.
 * @param blockCapacity_: Block capacity in bytes. All blocks match in capacity.
 * @param startPtr_:    Pointer to memory the blocks point to. Pre-allocated by caller.
 * @param bytes_:       Amount of bytes available at 'startPtr_'. Minimum 'numBlocks_' * 'blockCapacity_' size.
 * @return Returns 'true' on total success, 'false' in any other case. Partial or even full results can be thrown away safely (no allocations).
 */
bool    CreateBlocksFromMemory( Block* outBlocks_, u64 numBlocks_, size_t blockCapacity_, uintptr_t startPtr_, size_t bytes_ );

// TODO: need borrower first
/**
 * @brief ReturnBlock
 *  Returns Block to the Memory Manager, if the block is valid.
 *  It DOES NOT check, if Block actually came from Manager!
 *  Accepts Block if:
 *      - not a stub (ptr and capacity not zero)
 *      - capacity is power of two
 *      - capacity is smaller than maximum supported
 */
bool    ReturnBlock( Block& ) noexcept;

// TODO: finish doc
/**
 * @brief RequestBlock
 * @param capacity_
 * @return
 */
Block   RequestBlock( size_t capacity_ ) noexcept;

std::deque<Block>& GetFreeBlocksOfSize( size_t ) noexcept;
u64 GetMaximumAvailableBlockCapacity() noexcept;

template<typename T>
std::deque<Block>& GetFreeBlocks() noexcept
{
    return GetFreeBlocksOfSize( sizeof(T) );
}


} // end of namespace 'heap'


//--------------------------------------------------

bool    Initialize();
void    Quit();

} // end of namespace 'mem'
} // end of namespace 'pt'
