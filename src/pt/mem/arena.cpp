#include "pt/mem/arena.h"

#include "pt/logging.h"
#include "pt/utility.hpp"

#include <cassert>

using namespace pt::mem;


Arena::
~Arena() noexcept
{
    if( 0 < mPtr ){
        pt::PrintStackTrace( "WARNING: Destructor running on non-empty Arena!" );
    }
    assert( 0 == mPtr );
}


Arena Arena::
CreateArena( u64 bytes ) noexcept
{
    Arena a;
    PT_UNIMPLEMENTED_FUNCTION
    //auto blocks = heap::GetBlocks();

    //a.mBlock = heap::GetBlocks


    return a;
}


void Arena::
DestroyArena( Arena& arena ) noexcept
{
    arena.Reset();
    if( arena.mManaged ){
        heap::DestroyBlock( arena.mBlock );
    }else{
        heap::ReturnBlock( arena.mBlock );
    }

    arena.mBlock          = heap::Block();
    arena.mPtr            = 0;
    arena.mManaged        = false;
    arena.mCleanupsCount  = 0;
    arena.mCleanups.fill( nullptr );
    arena.mCleanupsOverflow.clear();
}


void Arena::
Reset() noexcept
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
            pt::PrintStackTrace( "Unexpected exception while resetting Arena" );
            assert( false );
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
            pt::PrintStackTrace( "Unexpected exception while resetting Arena" );
            assert( false );
        }
    }
    mCleanupsOverflow.clear();
    mCleanups.fill( nullptr );
    mCleanupsCount = 0;

    // reset bump ptr
    mPtr = mBlock.mData;
}
