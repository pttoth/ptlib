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

    //void* AllocRaw( u64 bytes ) noexcept  //TODO: add this header
    void* AllocRaw( u64 bytes )
    {
        // TODO: write this
        //  currently zero inits the buffer
        //      return (void*) Alloc<u8>( bytes );
        PT_UNIMPLEMENTED_FUNCTION
        return nullptr;
    }


    template<typename T>
    T*  Alloc( u64 amount ) noexcept
    {
        u64 freeAmount = this->RemainingSize<T>();
        if( freeAmount < amount ) return nullptr;

        // Past this point, it is expected, that all alignment and size params are correct.

        // align 'mPtr' upward
        uintptr_t alignment = alignof(T);
        uintptr_t alignedPtr = (mPtr + alignment - 1) & ~(alignment - 1);
        uintptr_t retval = alignedPtr;
        // TODO: verify this...

        mPtr = alignedPtr + amount*sizeof(T);

        // construct objects
        u64 ctor_successes = 0; // count successes, in case exceptions occur
        T* p = (T*) retval;
        try{
            for( u64 i=0; i<amount; ++i ){
                new (p+i) T();
                ++ctor_successes;
            }
        }catch(...){
            // call destructors for previous, successfully constructed instances
            for( u64 i=0; i<ctor_successes; ++i ){
                (p+ctor_successes-1-i)->~T();
            }
            // TODO: log error
            return nullptr;
        }


        // store cleanup function
        if( mCleanupsCount < mCleanupsArraySize ){
            // store in array (on stack)
            mCleanups[mCleanupsCount] = [p, amount]() noexcept{
                for( u64 i=0; i<amount; ++i ){
                    try{
                        (p+amount-1-i)->~T();   // reverse destruction order
                    }catch(...){
                        // TODO: log error
                    }
                }
            };
        }else{
            // store in overflow vector (on heap)
            mCleanupsOverflow.reserve(32);
            mCleanupsOverflow.push_back( [p, amount]() noexcept{
                for( u64 i=0; i<amount; ++i ){
                    try{
                        (p+amount-1-i)->~T();   // reverse destruction order
                    }catch(...){
                        // TODO: log error
                    }
                }
            } );
        }
        ++mCleanupsCount;

        return (T*) retval;
    }


    // TODO: add parameterized allocation
    //.....


    template<typename T>
    u64 RemainingSize() const noexcept
    {
        // verify, that Arena ptr is inside Block range
        assert( mBlock.mData <= mPtr );
        assert( mPtr < mBlock.mData + mBlock.mCapacity );

        // align 'mPtr' upward
        uintptr_t alignment = alignof(T);
        uintptr_t alignedPtr = (mPtr + alignment - 1) & ~(alignment - 1);

        uintptr_t endPtr = mBlock.mData + mBlock.mCapacity;
        if( endPtr <= alignedPtr ) return 0;

        return (endPtr - alignedPtr) / sizeof(T);
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
