/** -----------------------------------------------------------------------------
  * FILE:    threadsafe.hpp
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE:
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <mutex>

#include "pt/logging.h"

namespace pt{
namespace thread{

template<typename T, typename... Signature>
class Guarded
{
public:
    //---------------------------------------------------------------------------
    //  CLASS:   Guarded::Token
    //  PURPOSE: Locks and holds the wrapped data for the thread
    //             while this instance exists (move is possible).
    //---------------------------------------------------------------------------
    class Token
    {
        friend class Guarded;

    public:
        //move ctor
        Token(Token&& source):
            mData(source.mData), mLock( std::move( source.mLock ) )
        {
            source.mData = nullptr;
        }

        virtual ~Token()
        {
            mLock.unlock();
        }

        Token& operator=(const Token& other) = delete;
        Token& operator=(Token&& source) = delete;
        bool operator==(const Token& other) = delete;

        T& operator*()
        {
            return *mData;
        }

        T* operator->()
        {
            return mData;
        }

    private:
        T*                             mData = nullptr;
        std::unique_lock<std::mutex>   mLock;

        //private default ctor (prevents inheritance + overrides)
        Token( T& data, std::mutex& mutex ):
            mData(&data), mLock(mutex)
        {}
    };
    //--------------------------------------------------

    Guarded(T&& data):
        mData(data)
    {}

    Guarded(Signature... args):
        mData(args...)
    {}

    virtual ~Guarded()
    {
        //wait for mMutex to be unlocked
        std::lock_guard<std::mutex> lock(mMutex);
    }

    Token Acquire()
    {
        return Token(mData, mMutex);
    }


private:
    T           mData;
    std::mutex  mMutex;
};

} //end of namespace thread
} //end of namespace pt
