/*
 * -------------------------------------------------------------------------
 * FILE:    name.h
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE:
 *   A reference-counted string class optimized for frequent comparisons.
 *    Goal is to achieve features similar to UnrealScript's 'name' class.
 *      Each unique string has a globally unique uint64_t id.
 *      Name-Name comparison is just an integer (id) comparison.
 *      Construction from string data is 1 alloc slower. (shared_ptr)
 *      Copies are cheap (id + shared_ptr).
 *      One-time global state-sync is slow (hashing, global race condition, map-search, alloc).
 *      Each unique Name data is kept once, globally, even after instances are destroyed.
 *    Recommended usage:
 *      Avoid one-time use. (prefer 'static const')
 *      Frequent comparisons will pay back initial performance cost.
 *      { ...
 *        static const pt::Name my_name1( "AssetName" );
 *        static const pt::Name my_name2( "AssetName" );
 *        ...
 *        my_name1 == my_name2; // comparison is O(1): int == int
 *                              // (first-time comparison may involve late-init)
 *        ...
 *      }
 *    In order to avoid static init-order issues,
 *      global index is only acquired during late-init at first usage.
 *    Non-synced instances can be copied.
 *      This can hurt performance as later, each instance will sync separately.
 *      Calling '.Init()' will ensure synced-state, before copying instance.
 *    The following 'const' functions on uninitialized instances will initiate late-init.
 *      - comparison operators (== and !=)
 *          They init the right-side instance too!
 *      - GetStdString()
 * -------------------------------------------------------------------------
 */

#pragma once

#include <memory>
#include <mutex>
#include <ostream>
#include <string>
#include <unordered_map>

namespace pt{

class Name
{
public:
    Name();
    Name( const Name& other );  // can copy pending late-init state
    Name( Name&& source );      // can move pending late-init state
    Name( char* const cstr );
    Name( const char* const cstr );
    Name( const std::string& str );
    virtual ~Name();

    Name& operator=( const Name& other );
    Name& operator=( Name&& source );
    Name& operator=( const std::string& str );

    bool operator==( const Name& other ) const;
    bool operator!=( const Name& other ) const;

    size_t length() const;

    const std::string& GetStdString() const;
    bool IsEmpty() const;
    void Init() const;

private:
    //-------- definitions --------
    struct NameData{
        std::string     str;
        uint64_t        id = 0;

        NameData() = default;
        NameData( const std::string& str_, uint64_t id_ ):
            str( str_ ), id( id_ )
        {}
    };
    using NameDataPtr       = std::shared_ptr<       NameData >;
    using ConstNameDataPtr  = std::shared_ptr< const NameData >;

    //------ static contents ------
    static uint64_t TakeNextFreeId();
    static uint64_t TakeNextFreeId_NoLock();

    static const uint32_t   smHashSeed = 0x5f3759df; // note: the seed chosen here had no considerations of any kind
    static uint64_t         smNextFreeID;            // 0: non-existent | 1+: valid ID
    static std::mutex       smMutex;
    static std::unordered_map< uint64_t, ConstNameDataPtr >      smMapById;
    static std::unordered_multimap< uint32_t, ConstNameDataPtr > smMapByHash;

    //----- private functions -----
    void CreateSyncedState_( const char* const cstr, size_t length = 0 ) const;
    void CreatePreInitState_( const char* const cstr );
    void Init_() const;
    bool IsInPreInitState_() const;

    //------ private members ------

    // pending late-init: The instance holds the string data by itself,
    //                    but is not sync-ed globally yet (has no global id or proper ref-count).

    mutable ConstNameDataPtr mData;   // While pending late-init, this holds the non-unique string data.
                                      // While initialized, it provides direct access to the unique string data.
    mutable uint64_t         mId = 0; // redundant storage, prevents cache-miss and race-conditions
                                      // 0:          empty
                                      // UINT64_MAX: pending late-init
                                      // <other>:    globally unique string id
};

} // end of namespace 'pt'

std::ostream& operator<<( std::ostream& os, const pt::Name& obj );
