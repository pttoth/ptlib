
#include "pt/name.h"

#include <functional>

#include <vector>

#include <cstring>

using namespace pt;


const std::string Name::mDefaultString = std::string();
uint64_t Name::mNextFreeID = 1;
std::unordered_multimap< uint64_t, const Name::StringData > Name::mStringData;

uint64_t Name::
GetNextFreeID()
{
    uint64_t retval = mNextFreeID;
    ++mNextFreeID;
    return retval;
}


Name::InstanceData Name::InstanceData::
ConstructInstanceData( const std::string& str )
{
    //switch this to std::size_t
    std::size_t hash        = std::hash<std::string>{}( str );
    StringData  comparator  = StringData( str, 0 ); //'id' will not be used in comparation, so default 0

    Name::InstanceData retval;

    const StringData*   valptr = nullptr;

    auto iter = mStringData.find( hash ); //jump 'iter' to first occurence of key


    while( iter != mStringData.end()
        && hash == (*iter).first )      //while iterating over entries of the same key
    {
        auto entry = *iter;
        const StringData&   val = entry.second;

        if( val == comparator ){
            valptr = &val;
            break;
        }
        ++iter;
    }

    //if the string was not contained
    if( nullptr == valptr ){
        uint64_t ID = GetNextFreeID();
        StringData newEntry( str, ID );
        mStringData.insert( {hash, newEntry} );

        retval.mID   = ID;
        retval.mHash = hash;
        retval.mPStr = newEntry.mPStr;

        return retval;
    }else{  //if the string was already contained
        const StringData& refVal = (*valptr);
        retval.mID   = refVal.mID;
        retval.mPStr = refVal.mPStr;
        retval.mHash = hash;
    }

    return retval;
}


Name::
Name():
    mData()
{}


Name::
Name( const std::string& str ):
    mData( InstanceData::ConstructInstanceData( str ) )
{}


Name::
Name( const char* const str ):
    mData( InstanceData::ConstructInstanceData( str ) )
{}


bool Name::
operator==( const std::string& rhs ) const{
    if( IsEmpty() ){
        return ( 0 == rhs.length() );
    }
    return (*mData.mPStr) == rhs;
}


bool Name::
operator==( const char* const rhs ) const{
    if( IsEmpty() ){
        return ( 0 == strlen(rhs) );
    }
    return (0 == strcmp( (*mData.mPStr).c_str(), rhs ) );
}


bool Name::
operator==( char* const rhs ) const{
    const char* const rhs_ = rhs;
    return Name::operator==( rhs_ );
}


bool Name::
operator!=( const std::string& rhs ) const{
    if( IsEmpty() ){
        return ( 0 != rhs.length() );
    }
    return (*mData.mPStr) != rhs;
}


bool Name::
operator!=( const char* const rhs ) const{
    if( IsEmpty() ){
        return ( 0 != strlen(rhs) );
    }
    return (0 != strcmp( (*mData.mPStr).c_str(), rhs ) );
}


bool Name::
operator!=( char* const rhs ) const{
    const char* const rhs_ = rhs;
    return Name::operator!=( rhs_ );
}


const std::string& Name::
ToString() const
{
    if( 0 == mData.mID ){
        return mDefaultString;
    }
    return *(mData.mPStr);
}


const bool Name::
IsEmpty() const
{
    return (0 == mData.mID);
}
