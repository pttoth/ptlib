/** -----------------------------------------------------------------------------
  * FILE:    guard.hpp
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Convenience class to temporarily guard resources or defer execution.
  *          It calls a lambda (taken as parameter) at the end of its lifecycle.
  * -----------------------------------------------------------------------------
  */

#pragma once

namespace pt{

template<class T>
class Guard
{
public:
    Guard( T&& pLambda ):
        mEnabled( true ), mLambda( pLambda )
    {}
    virtual ~Guard(){
        if( mEnabled && (nullptr != mLambda) ){
            mLambda();
        }
    }

    Guard( const Guard& other )             = delete;
    Guard& operator=( const Guard& other )  = delete;
    Guard( Guard&& source )                 = default;
    Guard& operator=( Guard&& source )      = default;

    bool operator==( const Guard& other ) const = delete;

    bool Disable(){
        mEnabled = false;
    }

protected:
private:
    T&&  mLambda;
    bool mEnabled = true;
};

template<class T>
Guard<T> CreateGuard( T&& pLambda ){
    return Guard<T>( pLambda );
}

} // end of namespace 'pt'
