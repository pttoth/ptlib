//-----------------------------------------------------------------------------
//  FILE:    list.h
//  AUTHOR:  ptoth
//  EMAIL:   peter.t.toth92@gmail.com
//  PURPOSE: Linked list class.
//            Can only hold handles, no templates.
//-----------------------------------------------------------------------------

#pragma once

#include "macros.h"
#include "handle.h"
#include "stdutil.h"
#include <cassert>
#include <vector>

//--------------------------------------------------
namespace pt{

//-----
namespace list{

struct HNode: public Handle
{
    using Handle::Handle;
};

// TODO: add enum NodeState
//  Free
//  Active
// This is currently not needed, but, if additional features get implemented,
//  it would be nice to be able to easily track node state.


struct Node{
    Handle          mHandle     = Handle{0};
    handle::index   mNextIndex  = handle::index_max;
    handle::index   mPrevIndex  = handle::index_max;
    // Can add an mActive member to ensure the node is in use
    //  or some random generated u32/u64 val to ensure Node is actually part of List

    // from Handle
    handle::age mAge = 0;
};

} // end of namespace 'list'
//-----


struct List
{
    using Node      = list::Node;
    using HNode     = list::HNode;

    // The few highest value indices are taken for sentinel nodes (head and tail);
    static_assert( 8*sizeof(handle::index) != handle::bitcount_index,
                   "'handle::bitcount_index' is maxxed, need to adjust List::stMaxIndex!" );

    // Highest value in index interval means invalid index.
    static constexpr handle::index stInvalidIndex   = handle::index_max;
    // The highest ones before that are control node indices.
    static constexpr handle::index stMaxIndex           = stInvalidIndex - 1;
    static constexpr handle::index stMinSentinelIndex   = stMaxIndex - 3;
    static constexpr HNode  stHeadHandle        = CreateHandle( 1, 0, stMinSentinelIndex + 0 );
    static constexpr HNode  stTailHandle        = CreateHandle( 1, 0, stMinSentinelIndex + 1 );
    static constexpr HNode  stHeadFreeHandle    = CreateHandle( 1, 0, stMinSentinelIndex + 2 );
    static constexpr HNode  stTailFreeHandle    = CreateHandle( 1, 0, stMinSentinelIndex + 3 );

    //-----

    std::vector<Node>   mNodes;
    Node                mSentinels[4];
    Node                mStubRW;
    Node                mStubRO;

};

} // end of namespace 'pt'


//--------------------------------------------------
//  Functions
//--------------------------------------------------
//-------------------------
// Node
namespace pt{
namespace list{

bool    Exhausted   ( const Node& node ) noexcept;
void    IncrementAge(       Node& node ) noexcept;
bool    IsStub      ( const Node& node ) noexcept;
void    Reset       (       Node& node ) noexcept;

} // end of namespace 'list'
} // end of namespace 'pt'



//-------------------------
// List
namespace pt{
namespace list{
// These functions can be called from outside.
//  Therefore all incoming data and logic sanitization need to happen here.
// NOTE: Passing to these functions a 'hNode', that is not part of 'list' is Undefined Behavior!
const Node& GetRef      ( const List& list, const HNode&  hNode )  noexcept;
Node&       GetRef      (       List& list, HNode  hNode )  noexcept;
void        Init        (       List& list               )  noexcept;
void        PushBack    (       List& list, Handle h     )  noexcept;
void        PushFront   (       List& list, Handle h     )  noexcept;
void        Remove      (       List& list, HNode  hNode )  noexcept;
// Search: O(n)
HNode       Search      ( const List& list, Handle h     )  noexcept;


namespace priv{
// These functions are considered private and not to be called from outside.
//  Therefore, they expect correct inputs, logic and container state.

void            ConnectAfterNode    ( List& list, handle::index iTargetNode, handle::index iNewNode ) noexcept;
void            ConnectBeforeNode   ( List& list, handle::index iTargetNode, handle::index iNewNode ) noexcept;
void            DisconnectNode      ( List& list, handle::index iNode ) noexcept;
const Node&     GetNode             ( const List& list, handle::index index, bool& o_success ) noexcept;
Node&           GetNode             (       List& list, handle::index index, bool& o_success ) noexcept;
const Node&     GetSentinelHead     ( const List& list ) noexcept;
Node&           GetSentinelHead     (       List& list ) noexcept;
const Node&     GetSentinelTail     ( const List& list ) noexcept;
Node&           GetSentinelTail     (       List& list ) noexcept;
const Node&     GetSentinelHeadFree ( const List& list ) noexcept;
Node&           GetSentinelHeadFree (       List& list ) noexcept;
const Node&     GetSentinelTailFree ( const List& list ) noexcept;
Node&           GetSentinelTailFree (       List& list ) noexcept;
const Node&     GetStub             ( const List& list ) noexcept;
Node&           GetStub             (       List& list ) noexcept;
bool            Initialized         ( const List& list ) noexcept;
handle::index   PrepareFreeNode     (       List& list ) noexcept;
bool            ValidHNode          ( const HNode& hNode ) noexcept;

} // end of namespace 'priv'

} // end of namespace 'list'
} // end of namespace 'pt'


//-------------------------
// Node
PT_FORCE_INLINE bool pt::list::
Exhausted( const Node& node ) noexcept
{
    return ( node.mAge == handle::age_max );
}


PT_FORCE_INLINE void pt::list::
IncrementAge( Node& node ) noexcept
{
    node.mAge += Exhausted( node ) ? 0 : 1;
}


PT_FORCE_INLINE bool pt::list::
IsStub( const Node& node ) noexcept
{
    return ( List::stInvalidIndex == node.mPrevIndex )
        && ( List::stInvalidIndex == node.mNextIndex )
        && IsStub( node.mHandle );
}


PT_FORCE_INLINE void pt::list::
Reset( Node& node ) noexcept
{
    node = Node{};
}


//-------------------------
// list::
PT_FORCE_INLINE const pt::list::Node& pt::list::
GetRef( const List& list, const HNode& hNode ) noexcept
{
    bool initialized = priv::Initialized( list );
    bool validHNode  = priv::ValidHNode( hNode );
    assert( initialized );
    assert( validHNode );
    IF_UNLIKELY( !initialized || !validHNode )  return priv::GetStub( list );

    bool suc;
    const Node& node = priv::GetNode( list, hNode.Index(), suc );
    bool valid = suc && ( hNode.Age() == node.mAge );
    assert( hNode.Type() == 1 );
    assert( !IsStub( node ) );
    return valid ? node : priv::GetStub( list );
}


PT_FORCE_INLINE pt::list::Node& pt::list::
GetRef( List& list, HNode hNode ) noexcept
{
    bool initialized = priv::Initialized( list );
    bool validHNode  = priv::ValidHNode( hNode );
    assert( validHNode );
    IF_UNLIKELY( !validHNode ) return priv::GetStub( list );
    IF_UNLIKELY( !initialized ){
        Init( list );
    }

    bool suc;
    Node& node = priv::GetNode( list, hNode.Index(), suc );
    bool valid = suc && ( hNode.Age() == node.mAge );
    assert( hNode.Type() == 1 );
    assert( !IsStub( node ) );
    return valid ? node : priv::GetStub( list );
}


//-------------------------
// list::priv::


PT_FORCE_INLINE void pt::list::priv::
ConnectAfterNode( List& list, handle::index iTargetNode, handle::index iNewNode ) noexcept
{
    bool    sucTarget, sucNew, sucNext;
    Node&   targetNode  = GetNode( list, iTargetNode, sucTarget );
    Node&   newNode     = GetNode( list, iNewNode, sucNew );
    Node&   nextNode    = GetNode( list, targetNode.mNextIndex, sucNext );

    // update connections
    newNode.mPrevIndex      = iTargetNode;
    newNode.mNextIndex      = targetNode.mNextIndex;
    targetNode.mNextIndex   = iNewNode;
    nextNode.mPrevIndex     = iNewNode;
}


PT_FORCE_INLINE void pt::list::priv::
ConnectBeforeNode( List& list, handle::index iTargetNode, handle::index iNewNode ) noexcept
{
    bool suc;
    Node& targetNode = GetNode( list, iTargetNode, suc );
    return ConnectAfterNode( list, targetNode.mPrevIndex, iNewNode );
}


PT_FORCE_INLINE void pt::list::priv::
DisconnectNode( List& list, handle::index iNode ) noexcept
{
    bool    sucNode, sucPrev, sucNext;
    Node&   node        = GetNode( list, iNode, sucNode );
    Node&   prevNode    = GetNode( list, node.mPrevIndex, sucPrev );
    Node&   nextNode    = GetNode( list, node.mNextIndex, sucNext );

    // update connections
    prevNode.mNextIndex  = node.mNextIndex;
    nextNode.mPrevIndex  = node.mPrevIndex;
    node.mNextIndex = List::stInvalidIndex;
    node.mPrevIndex = List::stInvalidIndex;
}


PT_FORCE_INLINE const pt::list::Node& pt::list::priv::
GetNode( const List& list, handle::index i, bool& o_success ) noexcept
{
    o_success = true;

    if( (list.stMinSentinelIndex <= i) && ( i <= list.stMaxIndex ) ){
        handle::index iSentinel = i - list.stMinSentinelIndex;
        return list.mSentinels[ iSentinel ];
    }

    if( i < Size( list.mNodes ) ){
        return list.mNodes[i];
    }

    o_success = false;
    return priv::GetStub( list );
}


PT_FORCE_INLINE pt::list::Node& pt::list::priv::
GetNode( List& list, handle::index i, bool& o_success ) noexcept
{
    o_success = true;

    if( (list.stMinSentinelIndex <= i) && ( i <= list.stMaxIndex ) ){
        handle::index iSentinel = i - list.stMinSentinelIndex;
        return list.mSentinels[ iSentinel ];
    }

    if( i < Size( list.mNodes ) ){
        return list.mNodes[i];
    }

    o_success = false;
    return priv::GetStub( list );
}


PT_FORCE_INLINE const pt::list::Node& pt::list::priv::
GetSentinelHead( const List& list ) noexcept
{
    return list.mSentinels[0];
}


PT_FORCE_INLINE pt::list::Node& pt::list::priv::
GetSentinelHead( List& list ) noexcept
{
    return list.mSentinels[0];
}


PT_FORCE_INLINE const pt::list::Node& pt::list::priv::
GetSentinelTail( const List& list ) noexcept
{
    return list.mSentinels[1];
}


PT_FORCE_INLINE pt::list::Node& pt::list::priv::
GetSentinelTail( List& list ) noexcept
{
    return list.mSentinels[1];
}


PT_FORCE_INLINE const pt::list::Node& pt::list::priv::
GetSentinelHeadFree( const List& list ) noexcept
{
    return list.mSentinels[2];
}


PT_FORCE_INLINE pt::list::Node& pt::list::priv::
GetSentinelHeadFree( List& list ) noexcept
{
    return list.mSentinels[2];
}


PT_FORCE_INLINE const pt::list::Node& pt::list::priv::
GetSentinelTailFree( const List& list ) noexcept
{
    return list.mSentinels[3];
}


PT_FORCE_INLINE pt::list::Node& pt::list::priv::
GetSentinelTailFree( List& list ) noexcept
{
    return list.mSentinels[3];
}


PT_FORCE_INLINE const pt::list::Node& pt::list::priv::
GetStub( const List& list ) noexcept
{
    return list.mStubRO;
}


PT_FORCE_INLINE pt::list::Node& pt::list::priv::
GetStub( List& list ) noexcept
{
    Reset( list.mStubRW );
    return list.mStubRW;
}


PT_FORCE_INLINE bool pt::list::priv::
Initialized( const List& list ) noexcept
{
    return !IsStub( list.mSentinels[0] );
}


PT_FORCE_INLINE bool pt::list::priv::
ValidHNode( const HNode& hNode ) noexcept
{
    // This approach ensures that the unused bytes match too
    HNode expected = CreateHandle( 1, hNode.Age(), hNode.Index() );
    bool success = ( hNode == expected );
    assert( success );
    return success;
}
