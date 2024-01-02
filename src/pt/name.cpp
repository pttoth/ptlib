#include "pt/name.h"

#include "pt/alias.h"
#include "pt/utility.hpp"

#include <algorithm>
#include <cstring>

uint64_t    pt::Name::smNextFreeID = 1;
std::mutex  pt::Name::smMutex;
std::unordered_map<      uint64_t, pt::Name::ConstNameDataPtr > pt::Name::smMapById   = std::unordered_map<      uint64_t, Name::ConstNameDataPtr >();
std::unordered_multimap< uint32_t, pt::Name::ConstNameDataPtr > pt::Name::smMapByHash = std::unordered_multimap< uint32_t, Name::ConstNameDataPtr >();


pt::Name::
Name()
{}


pt::Name::
Name( const Name& other ):
    mData( other.mData ), mId( other.mId )
{}


pt::Name::
Name( Name&& source ):
    mData( source.mData ), mId( source.mId )
{}


pt::Name::
Name( char* const cstr )
{
    Construct( cstr );
}


pt::Name::
Name( const char* const cstr )
{
    Construct( cstr );
}


pt::Name::
Name( const std::string& str )
{
    Construct( str.c_str(), str.length() );
}


pt::Name::
~Name()
{}


pt::Name& pt::Name::
operator=( const Name& other )
{
    mData = other.mData;
    mId = other.mId;
    return *this;
}


pt::Name& pt::Name::
operator=(  Name&& source  )
{
    if( this != &source ){
        mData = std::move(source.mData);
        mId = source.mId;
    }
    return *this;
}


pt::Name& pt::Name::
operator=( const std::string& str )
{
    *this = Name( str );
    return *this;
}


bool pt::Name::
operator==( const Name& other ) const
{
    return ( mId == other.mId );
}


bool pt::Name::
operator!=( const Name& other ) const
{
    return ( mId != other.mId );
}


const std::string& pt::Name::
GetStdString() const
{
    return mData->str;
}


bool pt::Name::
IsEmpty() const
{
    return ( 0 == mId );
}


void pt::Name::
Construct( const char* const cstr, size_t length )
{
    size_t    strlength = ( 0 < length ) ? length : strlen( cstr );
    uint32_t  hash = pt::MurmurHash2( cstr, strlength, smHashSeed );
    ConstNameDataPtr data;

    pt::MutexLockGuard lock( smMutex );
    auto range = smMapByHash.equal_range( hash );
    for( auto iter = range.first; iter!= range.second; ++iter ){
        ConstNameDataPtr pNameData = iter->second;
        // if string values match
        if( 0 == strcmp( cstr, pNameData->str.c_str() ) ){
            mData = pNameData;
            mId = pNameData->id;
            return;
        }
    }

    // if 'cstr' was not found, create a new element
    uint64_t newid = Name::TakeNextFreeId_NoLock();
    mData = std::make_shared< const NameData >( std::string( cstr ), newid );
    mId = newid;
    smMapByHash.insert( std::make_pair( hash, mData ) );
    smMapById.insert( std::make_pair( newid, mData ) );
}


uint64_t pt::Name::
TakeNextFreeId()
{
    pt::MutexLockGuard lock( smMutex );
    return TakeNextFreeId_NoLock();
}


uint64_t pt::Name::
TakeNextFreeId_NoLock()
{
    uint64_t retval = smNextFreeID;
    ++smNextFreeID;
    return retval;
}


std::ostream&
operator<<( std::ostream& os, const pt::Name& obj )
{
    return os << obj.GetStdString();
}
