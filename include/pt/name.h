/*
 * -------------------------------------------------------------------------
 * FILE:    name.h
 * AUTHOR:  Peter Toth
 * E-MAIL:  peter.t.toth92@gmail.com
 * PURPOSE: A reference-counted string class optimized for comparisons.
 *          Goal is to achieve features similar to UnrealScript's 'name' class.
 *            Each unique string has a globally unique uint64_t id.
 *            Name-Name comparison is just an integer (id) comparison.
 *            Construction from string data is slower.
 *            Copies are cheap.
 *            Each unique Name data is kept once, globally, even after use.
 *          Recommended (optimized) usage:
 *          { ...
 *            static const Name my_name_obj( "AssetName" );
 *            ...
 *          }
 * -------------------------------------------------------------------------
 */

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
    Name( const Name& other );
    Name( Name&& source );
    Name( char* const cstr );
    Name( const char* const cstr );
    Name( const std::string& str );
    virtual ~Name();

    Name& operator=( const Name& other );
    Name& operator=( Name&& source );
    Name& operator=( const std::string& str );

    bool operator==( const Name& other ) const;
    bool operator!=( const Name& other ) const;

    const std::string& GetStdString() const;
    bool IsEmpty() const;
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
    void Construct( const char* const cstr, size_t length = 0 );

    //------ private members ------

    ConstNameDataPtr mData;
    uint64_t         mId = 0; // redundant storage, prevents cache-miss

};

} // end of namespace 'pt'

std::ostream& operator<<( std::ostream& os, const pt::Name& obj );
