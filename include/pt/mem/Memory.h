#pragma once

#include "pt/alias.h"
#include "pt/macros.h"
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

struct Arena
{
    heap::Block     mBlock;
    uintptr_t       mPtr     = mBlock.mData;
    bool            mManaged = false;

    static const u64    mCleanupsArraySize  = 64;
    u64                 mCleanupsCount      = 0;

    // cleanup lambdas calling destructors
    std::array<std::function<void()>,mCleanupsArraySize>    mCleanups;
    std::vector<std::function<void()>>                      mCleanupsOverflow; // only used if ran out of array


    Arena() noexcept = default;


    virtual ~Arena() noexcept
    {
        Reset();
        if( mManaged ){
            heap::DestroyBlock( mBlock );
        }else{
            heap::ReturnBlock( mBlock );
        }
    }


    void* AllocRaw( u64 bytes_, size_t typeAlignment_ = alignof(u8) ) noexcept
    {
        u64 freeAmount = this->RemainingSizeWithAlignment( 1, 1 );
        if( freeAmount < bytes_ ) return nullptr;

        // Past this point, it is expected, that all alignment and size params are correct.
        //  No further bounds checking needed.

        // align 'mPtr' upward
        uintptr_t alignedPtr = (mPtr + typeAlignment_ - 1) & ~(typeAlignment_ - 1);
        uintptr_t retval = alignedPtr;

        mPtr = alignedPtr + bytes_;

        return (void*) retval;
    }


    template<typename T, typename... Args>
    T*  Alloc( u64 amount_, Args&&... args ) noexcept
    {
        void* buffer = (void*) AllocRaw( amount_ * sizeof(T), alignof(T) );
        if( nullptr == buffer ) return nullptr;

        // construct T objects
        u64 ctor_successes = 0; // count successes, in case exceptions occur
        T* p = (T*) buffer;
        try{
            for( u64 i=0; i<amount_; ++i ){
                new (p+i) T( std::forward<Args>(args)... );
                ++ctor_successes;
            }
        }catch(...){
            // some construction threw an exception
            //  clean up the ones that already succeeded
            for( u64 i=0; i<ctor_successes; ++i ){
                try{
                    (p+ctor_successes-1-i)->~T();
                }catch(...){
                    // TODO: log error
                    assert(false);
                }
            }
            assert(false);
            // TODO: log error
            return nullptr;
        }

        // store cleanup function for this allocation
        if( mCleanupsCount < mCleanupsArraySize ){
            // store in array (on stack)
            mCleanups[mCleanupsCount] = [p, amount_]() noexcept{
                for( u64 i=0; i<amount_; ++i ){
                    try{
                        (p+amount_-1-i)->~T();   // reverse destruction order
                    }catch(...){
                        // TODO: log error
                    }
                }
            };
        }else{
            // ... or store in overflow vector (on heap)
            mCleanupsOverflow.reserve(32);
            mCleanupsOverflow.push_back( [p, amount_]() noexcept{
                for( u64 i=0; i<amount_; ++i ){
                    try{
                        (p+amount_-1-i)->~T();   // reverse destruction order
                    }catch(...){
                        // TODO: log error
                    }
                }
            } );
        }
        ++mCleanupsCount;

        return p;
    }

    template<typename T>
    u64 RemainingSize() const noexcept
    {
        return RemainingSizeWithAlignment( sizeof(T), alignof(T) );
    }


    u64 RemainingSizeWithAlignment( size_t typeSize_, size_t typeAlignment_ ) const noexcept
    {
        // align 'mPtr' upward
        uintptr_t alignedPtr = (mPtr + typeAlignment_ - 1) & ~(typeAlignment_ - 1);
        uintptr_t endPtr = mBlock.mData + mBlock.mCapacity;

        // verification, that Arena ptr is inside Block range
        assert( mBlock.mData <= mPtr );
        assert( mPtr <= endPtr );
        if( endPtr <= alignedPtr ) return 0;

        return (endPtr - alignedPtr) / typeSize_;
    }


    void Reset() noexcept
    {
        // call cleanups in reverse order
        //  overflow vector
        auto iStart = mCleanupsOverflow.rbegin();
        auto iEnd   = mCleanupsOverflow.rend();
        for( auto it=iStart; it!=iEnd; ++it){
            try{
                auto& f = *it;
                assert( nullptr != f );
                if( nullptr != f ){
                    f();
                }
                f = nullptr;
            }catch(...){
                // this should be impossible, but just in case...
                //  absolutely and utterly FUBAR case
                //  just print an error and a debug stacktrace, then ignore
                assert( false );
                // TODO: log error
            }
        }
        //  starting array
        u64 maxIdx = std::min( mCleanupsArraySize, mCleanupsCount );
        for( u64 i=0; i<maxIdx; ++i){
            try{
                auto& f = mCleanups[maxIdx-i-1];
                assert( nullptr != f );
                if( nullptr != f ){
                    f();
                }
                f = nullptr;
            }catch(...){
                // this should be impossible, but just in case...
                //  absolutely and utterly FUBAR case
                //  just print an error and a debug stacktrace, then ignore
                assert( false );
                // TODO: log error
            }
        }
        mCleanupsOverflow.clear();
        mCleanups.fill( nullptr );
        mCleanupsCount = 0;

        // reset bump ptr
        mPtr = mBlock.mData;
    }
};




//--------------------------------------------------

Arena   CreateArena( u64 bytes );
template<typename T>
Arena   CreateArena( u64 amount )
{
    return CreateArena( amount * sizeof(T) );
}

bool    Initialize();
void    Quit();

} // end of namespace 'mem'
} // end of namespace 'pt'
