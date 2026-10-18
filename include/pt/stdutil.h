#pragma once

#include "def.h"
#include <vector>


template<typename T>
PT_FORCE_INLINE u64
Capacity( const std::vector<T>& v )
{
    return v.capacity();
}


template<typename T, class... Args >
PT_FORCE_INLINE void
EmplaceBack( std::vector<T>& v, Args... args )
{
    v.emplace_back( args... );
}


template<typename T>
PT_FORCE_INLINE void
PushBack( std::vector<T>& v, T& element )
{
    v.push_back( element );
}


template<typename T>
PT_FORCE_INLINE void
Reserve( std::vector<T>& v, u64 size )
{
    v.reserve( size );
}


template<typename T>
PT_FORCE_INLINE void
Resize( std::vector<T>& v, u64 size )
{
    v.resize( size );
}


template<typename T>
PT_FORCE_INLINE u64
Size( const std::vector<T>& v )
{
    return v.size();
}
