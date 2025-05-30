/** -----------------------------------------------------------------------------
  * FILE:    guard.hpp
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Convenience class to temporarily guard resources or defer execution.
  *          It calls a lambda (taken as parameter) at the end of its lifecycle.
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <functional>

namespace pt{

template<class T>
class Guard
{
public:
    Guard( const T& pLambda ):
        mLambda( pLambda ), mEnabled( true )
    {}
    virtual ~Guard(){
        DestroyGuard();
    }

    Guard( const Guard& other )             = delete;
    Guard& operator=( const Guard& other )  = delete;
    Guard( Guard&& source ):
        mLambda( std::move( source.mLambda ) ),
        mEnabled( source.mEnabled )
    {
        source.mLambda  = nullptr;
        source.mEnabled = false;
    }
    Guard& operator=( Guard&& source )
    {
        DestroyGuard();
        mLambda = std::move( source.mLambda );
        mEnabled = source.mEnabled;
        source.mLambda  = nullptr;
        source.mEnabled = false;
    }
    bool operator==( const Guard& other ) const = delete;

    void Disable(){
        mEnabled = false;
    }

protected:
private:
    void DestroyGuard(){
        if( mEnabled && mLambda ){
            mLambda();
        }
    }

    std::function<void()> mLambda = nullptr;
    bool mEnabled = true;
};

template<class T>
Guard<T> CreateGuard( const T& pLambda ){
    return Guard<T>( pLambda );
}

} // end of namespace 'pt'
