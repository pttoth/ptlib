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

struct Arena
{
    heap::Block     mBlock;
    uintptr_t       mPtr     = 0;
    bool            mManaged = false;

    static const u64    mCleanupsArraySize  = 64;
    u64                 mCleanupsCount      = 0;

    // cleanup lambdas calling destructors
    std::array<std::function<void()>,mCleanupsArraySize>    mCleanups;
    std::vector<std::function<void()>>                      mCleanupsOverflow; // only used if ran out of array

//--------------------------------------------------

    Arena() noexcept = default;

    virtual ~Arena() noexcept;


    template<typename T = u8>
    T* AllocRaw( u64 amount_ ) noexcept
    {
        u64 freeAmount = this->RemainingSize<T>();
        if( freeAmount < amount_ ) return nullptr;

        uintptr_t alignedPtr = pt::AlignAs<T>( mPtr );
        mPtr = alignedPtr + amount_;

        return (T*) alignedPtr;
    }


    template<typename T, typename... Args>
    T*  Alloc( u64 amount_, Args&&... args ) noexcept
    {
        T* buffer = AllocRaw<T>( amount_ );
        if( nullptr == buffer ) return nullptr;

        // construct T objects
        u64 ctor_successes = 0; // count successes, in case exceptions occur
        try{
            for( u64 i=0; i<amount_; ++i ){
                new (buffer+i) T( std::forward<Args>(args)... );
                ++ctor_successes;
            }
        }catch(...){
            // some construction threw an exception
            //  clean up the ones that already succeeded
            for( u64 i=0; i<ctor_successes; ++i ){
                try{
                    (buffer+ctor_successes-1-i)->~T();
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
            mCleanups[mCleanupsCount] = [buffer, amount_]() noexcept{
                for( u64 i=0; i<amount_; ++i ){
                    try{
                        (buffer+amount_-1-i)->~T();   // reverse destruction order
                    }catch(...){
                        // TODO: log error
                    }
                }
            };
        }else{
            // ... or store in overflow vector (on heap)
            mCleanupsOverflow.reserve(32);
            mCleanupsOverflow.push_back( [buffer, amount_]() noexcept{
                for( u64 i=0; i<amount_; ++i ){
                    try{
                        (buffer+amount_-1-i)->~T();   // reverse destruction order
                    }catch(...){
                        // TODO: log error
                    }
                }
            } );
        }
        ++mCleanupsCount;

        return buffer;
    }


    void ReleaseResources() noexcept
    {
        Reset();
        if( mManaged ){
            heap::DestroyBlock( mBlock );
        }else{
            heap::ReturnBlock( mBlock );
        }

        mBlock          = heap::Block();
        mPtr            = 0;
        mManaged        = false;
        mCleanupsCount  = 0;
        mCleanups.fill( nullptr );
        mCleanupsOverflow.clear();
    }


    template<typename T>
    u64 RemainingBytesWithAlignment() const noexcept
    {
        if( 0 == mPtr ) return 0;

        // align 'mPtr' upward
        uintptr_t alignedPtr = pt::AlignAs<T>( mPtr );
        uintptr_t endPtr = mBlock.mData + mBlock.mCapacity;

        // verification, that Arena ptr is inside Block range
        assert( mBlock.mData <= mPtr );
        assert( mPtr <= endPtr );
        if( endPtr <= alignedPtr ) return 0;

        return (endPtr - alignedPtr);
    }


    template<typename T>
    u64 RemainingSize() const noexcept
    {
        return RemainingBytesWithAlignment<T>() / sizeof(T);
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
