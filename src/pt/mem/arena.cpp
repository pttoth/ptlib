#include "pt/mem/arena.h"

using namespace pt::mem;


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
ReleaseResources() noexcept
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


Arena Arena::
DestroyArena( Arena& ) noexcept
{
    PT_UNIMPLEMENTED_FUNCTION
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
