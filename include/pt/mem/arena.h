#pragma once

#include "pt/mem/Memory.h"
#include "pt/utility.hpp"
#include <array>
#include <cassert>
#include <functional>
#include <vector>

namespace pt{
namespace mem{

struct Arena
{
//-------------------------
//  Members
//-------------------------
    heap::Block     mBlock;
    uintptr_t       mPtr     = 0;
    bool            mManaged = false;

    static const u64    mCleanupsArraySize  = 64;
    u64                 mCleanupsCount      = 0;

    // cleanup lambdas calling destructors
    std::array<std::function<void()>,mCleanupsArraySize>    mCleanups;
    std::vector<std::function<void()>>                      mCleanupsOverflow; // only used if ran out of array

//-------------------------
//  Functions
//-------------------------

    Arena() noexcept = default;
    virtual ~Arena() noexcept;

    Arena( Arena&& ) noexcept = default;

    static Arena CreateArena( u64 bytes ) noexcept;

//    template<typename T>
//    static Arena CreateArena( u64 amount ) noexcept;

    static void DestroyArena( Arena& ) noexcept;

//    template<typename T = u8>
//    T* AllocRaw( u64 amount_ ) noexcept;

//    template<typename T, typename... Args>
//    T*  Alloc( u64 amount_, Args&&... args ) noexcept;

//    template<typename T>
//    u64 RemainingBytesWithAlignment() const noexcept;

//    template<typename T>
//    u64 RemainingSize() const noexcept;

    void Reset() noexcept;

//-------------------------
//  Template defs
//-------------------------
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


    template<typename T>
    static Arena CreateArena( u64 amount ) noexcept
    {
        return CreateArena( amount * sizeof(T) );
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

};


} // end of namespace 'mem'
} // end of namespace 'pt'
