/*
 * -------------------------------------------------------------------------
 * FILE:    name.h
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE: Immutable string class optimized for frequent comparisons.
 *          - Statically stores different string occurences once.
 *          - Maps an integer ID for each string occurence.
 *          - Comparison is done with the single integer ID instead of string operations.
 *          - Never deletes string occurences.
 *          - Performance tradeoff is that construction is more expensive.
 *   The purpose is similar to UnrealScript's 'name' class.
 *   The Name instances are best used as static consts.
 * -------------------------------------------------------------------------
 */

//TODO: unused String deletion
//      Name instances' shared ptr-s may allow tracking usage and detect unused strings
//      take a look some time...


//TODO: add some character filtering
//          [a-zA-Z0-9] + [-_.]

#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include <unordered_map>

namespace pt{
class Name
{
    using SharedStringPtr = std::shared_ptr< const std::string >;

    struct StringData
    {
        uint64_t        mID;
        SharedStringPtr mPStr;

        StringData():
            mID( 0 )
        {}
        StringData( const StringData& other ) = default;
        StringData( StringData&& source ) = default;

        StringData( const std::string& str_, uint64_t id_ ):
            mID( id_ ),
            mPStr( std::make_shared< const std::string >( str_ ) )
        {}

        virtual ~StringData() = default;

        StringData& operator=( const StringData& other ) = default;
        StringData& operator=( StringData&& source ) = default;

        //comparator only searches by string
        bool operator==( const StringData& rhs ) const{
            return ( *mPStr == *(rhs.mPStr) );
        }

        static StringData ConstructStringData( const std::string& str );
    };

public:
    Name();

    Name( const std::string& str );
    Name( const char* const str );

    Name( const Name& other ) = default;
    Name( Name&& source ) = default;

    Name& operator=( const Name& other ) = default;
    Name& operator=( Name&& source ) = default;

    virtual ~Name()
    {}

    bool operator==( const Name& rhs ) const{
        return ( mData.mID == rhs.mData.mID );
    }
    bool operator==( const std::string& rhs ) const;
    bool operator==( const char* const rhs ) const;
    bool operator==(       char* const rhs ) const;


    bool operator!=( const Name& rhs ) const{
        return ( mData.mID != rhs.mData.mID );
    }
    bool operator!=( const std::string& rhs ) const;
    bool operator!=( const char* const rhs ) const;
    bool operator!=(       char* const rhs ) const;

    const std::string& ToString() const;

    bool IsEmpty() const;


private:
    //-------------------------
    //  stack contents
    //-------------------------
    StringData mData;   //wrapped in struct to better support const-initialization (TODO: think some, might have been a bad direction)

    //-------------------------
    //  static contents
    //-------------------------
    //static std::unordered_multimap< uint64_t, const StringData > mStringData; //TODO: rename
    static std::unordered_multimap< std::size_t, const StringData > mStringData; //TODO: rename
    static uint64_t mNextFreeID;
    static const std::string mDefaultString; // default empty string to return with ToString if Name is empty | TODO: find a cleaner solution ... :(

    static uint64_t GetNextFreeID();
    static Name     ConstructInstanceData( const std::string& str );

};

}
