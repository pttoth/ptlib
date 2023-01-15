#pragma once

#include <mutex>
#include <shared_mutex>


#include <assert.h>

template<class T>
class Threadsafe
{

public:
    //--------------------------------------------------
    //  Lock subclasses
    //--------------------------------------------------
    class WriteLock{
        friend class Threadsafe; //only Threadsafe can call ctor

    public:
        virtual ~WriteLock() = default;

        WriteLock( WriteLock&& source ) = default;
        WriteLock& operator=( WriteLock&& source ) = default;

        //no copy allowed
        WriteLock( const WriteLock& other ) = delete;
        WriteLock& operator=( const WriteLock& other ) = delete;
        bool operator==( const WriteLock& other ) const = delete;

        T* operator->(){
            return &mData;
        }

        const T* operator->() const{
            return &mData;
        }

    private:
        T& mData;
        std::shared_timed_mutex& mMutex;
        std::unique_lock< std::shared_timed_mutex > mLockExclusive;

        WriteLock( T& data_, std::shared_timed_mutex& mutex_ ):
            mData( data_ ), mMutex( mutex_ ), mLockExclusive( std::unique_lock< std::shared_timed_mutex >( mMutex ) )
        {}

    };
    //--------------------------------------------------
    class ReadLock{
        friend class Threadsafe; //only Threadsafe can call ctor

    public:
        virtual ~ReadLock() = default;

        ReadLock( ReadLock&& source ) = default;
        ReadLock& operator=( ReadLock&& source ) = default;

        //no copy allowed
        ReadLock( const ReadLock& other ) = delete;
        ReadLock& operator=( const ReadLock& other ) = delete;
        bool operator==( const ReadLock& other ) const = delete;

        const T* operator->() const{
            return &mData;
        }

    private:
        const T& mData;
        std::shared_timed_mutex& mMutex;
        std::shared_lock< std::shared_timed_mutex > mLockShared;

        ReadLock( const T& data_, std::shared_timed_mutex& mutex_ ):
            mData( data_ ), mMutex( mutex_ ), mLockShared( std::shared_lock< std::shared_timed_mutex >( mMutex ) )
        {}

    };
    //--------------------------------------------------


    Threadsafe();
    Threadsafe( const Threadsafe& other ){
        std::shared_lock< std::shared_timed_mutex > lock( other.mMutex );
        mData = other.mData;
    }
    Threadsafe( Threadsafe&& source )
    {
        std::shared_lock< std::shared_timed_mutex > lock( source.mMutex );
        mData = std::move( source.mData );
    }

    virtual ~Threadsafe(){
        //wait for any existing read or write operation to finish
        std::lock_guard< std::shared_timed_mutex > lock( mMutex );
    }


    Threadsafe& operator=( const Threadsafe& other ) = delete;
    Threadsafe& operator=( Threadsafe&& source ) = delete;
    bool operator==( const Threadsafe<T>& other ) = delete;

    ReadLock LockRead(){
        return ReadLock( mData, mMutex );
    }
    WriteLock LockWrite(){
        return WriteLock( mData, mMutex );
    }

protected:
private:
    T mData;
    std::shared_timed_mutex mMutex;

};
