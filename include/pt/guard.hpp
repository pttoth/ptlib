/** -----------------------------------------------------------------------------
  * FILE:    guard.hpp
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Convenience class to temporarily guard resources or defer execution.
  *          It calls a lambda (taken as parameter) at the end of its lifecycle.
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <utility>

namespace pt{

template<class T>
class Guard
{
public:
    Guard( T&& pLambda ):
        mEnabled( true ), mLambda( pLambda )
    {}
    virtual ~Guard(){
        Destroy();
    }

    Guard( const Guard& other )             = delete;
    Guard& operator=( const Guard& other )  = delete;
    Guard( Guard&& source ):
        mLambda( std::move( source.mLambda ) ),
        mEnabled( source.mEnabled )
    {
        source.mLambda = nullptr;
        source.mEnabled = false;
    }
    Guard& operator=( Guard&& source )
    {
        Destroy();
        mLambda = std::move( source.mLambda );
        mEnabled = source.mEnabled;
        source.mLambda = nullptr;
        source.mEnabled = false;
    }
    bool operator==( const Guard& other ) const = delete;

    bool Disable(){
        mEnabled = false;
    }

protected:
private:
    void Destroy(){
        if( mEnabled && (nullptr != mLambda) ){
            mLambda();
        }
    }

    T&&  mLambda;
    bool mEnabled = true;
};

template<class T>
Guard<T> CreateGuard( T&& pLambda ){
    return Guard<T>( pLambda );
}

} // end of namespace 'pt'
