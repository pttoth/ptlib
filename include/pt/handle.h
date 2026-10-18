//-----------------------------------------------------------------------------
//  FILE:    handle.h
//  AUTHOR:  ptoth
//  EMAIL:   peter.t.toth92@gmail.com
//  PURPOSE: References to objects. A lot more practical than storing (smart/classic) pointers.
//    Requires a system, that owns instance storage and manages handle resolutions per handle type.
//    A handle consists of 64 bits, split by the logic below:

//
//       free(6) type(14)       age(20)              index(24)
//        ------|--------------|--------------------|------------------------
//       63   58 57          44 43                24 23                     0

//    Index:    Identifies storage index.
//    Age:      Generation counter. Prevents use-after-delete (when Valid(handle) is used).
//                (Dangling handles could otherwise access instances that are stored in reused indices.)
//                Age counter is increased, when object instance is logically deleted.
//                If Age hits max, the storage slot (index) is marked exhausted.
//                The system should have a well-defined point, where age counters are reset.
//                  eg.: level transition in a videogame, etc. (entirely use-case-dependent).
//    Type:     Although type tracking is not strictly necessary with proper use,
//                it can reduce the surface for errors during use.
//                With a user-defined enum, handle type can be stored in these bits.
//                  0 is reserved for identifying invalid (default) handle.
//                  1 is reserved for unknown/don't care, but valid type.
//                If types are not strictly tracked, just set this to 1, when in use.
//    Free:     Reserved for custom use.
//                Possible practical use-case: handle versioning during serialization.

// Usage recommendations:
//    Additional type safety is available, by using empty, inheritor structs per type.
//      eg.:
//        struct HEntity: public Handle{
//            using Handle::Handle;
//        };
//      The child handles should have identical memory print with Handle, and no vtable!
//      They can however implement reference getters that provide convenient access to
//        the instance's data through the handle.
//        eg.:
//          struct HEntity: public Handle{
//              using Handle::Handle;
//              const vec3& Pos() const noexcept;
//                    vec3& Pos()       noexcept;
//          };


//-----------------------------------------------------------------------------
#pragma once

#include "pt/def.h"
#include <cassert>

namespace pt{
    namespace handle{
        // NOTE: handles are expected to be unsigned
        using value     = u64;
        using age       = u32;
        using index     = u32;
        using type      = u16;  // create a custom enum

        constexpr   u8 bitcount_index   = 24;
        constexpr   u8 bitcount_age     = 20;
        constexpr   u8 bitcount_type    = 14;
        constexpr   u8 bitcount_free    = 6;

        constexpr   u8 shift_index      = 0;
        constexpr   u8 shift_age        = bitcount_index;
        constexpr   u8 shift_type       = bitcount_index + bitcount_age;
        constexpr   u8 shift_free       = bitcount_index + bitcount_age + bitcount_type;

        constexpr   value mask_index    = (value{1} << bitcount_index) - 1;
        constexpr   value mask_age      = (value{1} << bitcount_age)   - 1;
        constexpr   value mask_type     = (value{1} << bitcount_type)  - 1;

        // validation
        static_assert( bitcount_index <= sizeof(index) * 8,
                       "assigned bitcount does not fit type 'pt::handle::index'!" );
        static_assert( bitcount_age <= sizeof(age) * 8,
                       "assigned bitcount does not fit type 'pt::handle::age'!" );
        static_assert( bitcount_type <= sizeof(type) * 8,
                       "assigned bitcount does not fit type 'pt::handle::type'!" );
        static_assert( sizeof(value) * 8 == bitcount_age + bitcount_free + bitcount_index + bitcount_type,
                       "'pt::Handle' bit distribution does not add up!" );

        // helper class
        //  Classes can inherit this to ensure age member.
        //  experimental: Might cause alignment issues and unnecessary padding.
        struct StorageEntry{
            age mAge = 0;   // note that age only uses 'handle::bitcount_age' bits
        };

        constexpr bool    AgeBitsCorrect   ( age   a )    noexcept;
        constexpr bool    IndexBitsCorrect ( index i )    noexcept;
        constexpr bool    TypeBitsCorrect  ( type  t )    noexcept;

    } // end of namespace 'handle'


    struct Handle{
        handle::value   mVal = 0;

        constexpr handle::age     Age()   const noexcept;
        constexpr handle::index   Index() const noexcept;
        constexpr handle::type    Type()  const noexcept;

        // TODO: implement
        void SetAge     ( handle::age   age     ) noexcept;
        void SetIndex   ( handle::index index   ) noexcept;
        void SetType    ( handle::type  type    ) noexcept;

        constexpr Handle() noexcept = default;
        constexpr Handle( handle::value rhs ) noexcept:
            mVal( rhs )
        {}

        PT_FORCE_INLINE constexpr bool
        operator==( const Handle& rhs ) const noexcept{
            return mVal == rhs.mVal;
        }

        PT_FORCE_INLINE constexpr bool
        operator!=( const Handle& rhs ) const noexcept{
            return !operator==( rhs );
        }
    };


    constexpr handle::value   CreateHandle( handle::type type, handle::age age, handle::index index ) noexcept;
    bool            IsStub( const Handle& h ) noexcept;
    void            Reset( Handle& h ) noexcept;

    //-------------------------
    // Note: These have to be implemented after Handle is hooked up to a system!
    //  experimental: these might move into the system interface in the future
    //                need testing...

    // Checks, whether resolving the handle to a pointer will point to a valid instance.
    //  Valid if:
    //  - Type not null
    //  - Index is within bounds
    //  - Age matches instance Age
    extern bool     Valid( Handle h ) noexcept;

    // Resolves the handle to a pointer.
    //  Type can be inferred from Type bits or Handle child type.
    extern void*    GetPtr( Handle h ) noexcept;
    //-------------------------

} // end of namespace 'pt'


PT_FORCE_INLINE constexpr bool pt::handle::
AgeBitsCorrect( age a ) noexcept
{
    const value aval = a;
    return aval == ( aval & mask_age );
}


PT_FORCE_INLINE constexpr bool pt::handle::
IndexBitsCorrect( index i ) noexcept
{
    const value ival = i;
    return ival == ( ival & mask_index );
}


PT_FORCE_INLINE constexpr bool pt::handle::
TypeBitsCorrect( type t ) noexcept
{
    const value tval = t;
    return tval == ( tval & mask_type );
}


PT_FORCE_INLINE constexpr pt::handle::age pt::Handle::
Age() const noexcept
{
    return static_cast<handle::age>( ( mVal >> handle::shift_age ) & handle::mask_age );
}


PT_FORCE_INLINE constexpr pt::handle::index pt::Handle::
Index() const noexcept
{
    return static_cast<handle::index>( ( mVal >> handle::shift_index ) & handle::mask_index );
}


PT_FORCE_INLINE constexpr pt::handle::type pt::Handle::
Type() const noexcept
{
    return static_cast<handle::type>( ( mVal >> handle::shift_type ) & handle::mask_type );
}


PT_FORCE_INLINE constexpr pt::handle::value pt::
CreateHandle( handle::type type, handle::age age, handle::index index ) noexcept
{
    using namespace handle;

    bool paramsValid = TypeBitsCorrect( type ) && AgeBitsCorrect( age ) && IndexBitsCorrect( index );
    assert( paramsValid );

    value retval = ( value{ type  & handle::mask_type  } << handle::shift_type  )
                 | ( value{ age   & handle::mask_age   } << handle::shift_age   )
                 | ( value{ index & handle::mask_index } << handle::shift_index );

    return paramsValid ? retval : 0 ;
}


PT_FORCE_INLINE bool pt::
IsStub( const Handle& h ) noexcept
{
    return 0 == h.Type();
}


PT_FORCE_INLINE void pt::
Reset( Handle& h ) noexcept
{
    h = Handle{};
}
