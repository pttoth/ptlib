#include "pt/name.h"

#include "pt/alias.h"
#include "pt/utility.hpp"

#include <algorithm>
#include <assert.h>
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
    mData( std::move(source.mData) ), mId( source.mId )
{
    source.mData = nullptr;
    source.mId   = 0;
}


pt::Name::
Name( char* const cstr )
{
    CreatePreInitState_( cstr );
}


pt::Name::
Name( const char* const cstr )
{
    CreatePreInitState_( cstr );
}


pt::Name::
Name( const std::string& str )
{
    CreatePreInitState_( str.c_str() );
}


pt::Name::
~Name()
{}


pt::Name& pt::Name::
operator=( const Name& other )
{
    mData = other.mData;
    mId   = other.mId;
    return *this;
}


pt::Name& pt::Name::
operator=( Name&& source )
{
    if( this != &source ){
        mData = std::move(source.mData);
        mId = source.mId;
    }
    source.mData = nullptr;
    source.mId = 0;
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
    // sync instances globally, if needed
    this->Init_();
    other.Init_();

    if( (0 == mId) || (0 == other.mId) ){
        return false;
    }
    return ( mId == other.mId );
}


bool pt::Name::
operator!=( const Name& other ) const
{
    bool retval = this->operator==( other );
    return !retval;
}


const std::string& pt::Name::
GetStdString() const
{
    Init_();
    if( 0 == mId ){
        static const std::string DummyEmptyString;
        return DummyEmptyString;
    }
    assert( nullptr != mData );
    return mData->str;
}


bool pt::Name::
IsEmpty() const
{
    // check, that mId is not out of sync with mData
    assert( (0 == mId) && (nullptr == mData) || (0 != mId) && (nullptr != mData) );
    return (0 == mId) || ( IsInPreInitState_() && 0 == mData->str.length() );
}


void pt::Name::
Init() const
{
    Init_();
}


void pt::Name::
CreateSyncedState_( const char* const cstr, size_t length ) const
{
    assert( nullptr != cstr );
    size_t    strlength = ( 0 < length ) ? length : strlen( cstr );
    uint32_t  hash = pt::MurmurHash2( cstr, strlength, smHashSeed );
    ConstNameDataPtr data;

    //data should already be sanitized here
    // if new data is empty string
    if( 0 == strlength ){
        mData = nullptr;
        mId   = 0;
        return;
    }

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


bool pt::Name::
IsInPreInitState_() const
{
    return ( UINT64_MAX == mId );
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


void pt::Name::
CreatePreInitState_( const char* const cstr )
{
    if( nullptr == cstr ){
        mData = nullptr;
        mId = 0;
        return;
    }
    if( 0 == strlen( cstr ) ){
        mData = nullptr;
        mId = 0;
        return;
    }

    ConstNameDataPtr pNameData = std::make_shared< const NameData >( std::string(cstr), UINT64_MAX );
    mData = pNameData;
    mId = pNameData->id;
}


void pt::Name::
Init_() const
{
    if( IsInPreInitState_() ){
        ConstNameDataPtr tempPtr = mData; // save a local pointer to prevent destruction
        CreateSyncedState_( tempPtr->str.c_str(), tempPtr->str.length() );
    }
    assert( UINT64_MAX != mId );
}


std::ostream&
operator<<( std::ostream& os, const pt::Name& obj )
{
    os << obj.GetStdString();
    return os;
}
