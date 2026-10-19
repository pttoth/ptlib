//-----------------------------------------------------------------------------
//  FILE:    list.h
//  AUTHOR:  ptoth
//  EMAIL:   peter.t.toth92@gmail.com
//  PURPOSE: Linked list class.
//            Can only hold handles, no templates.
//-----------------------------------------------------------------------------

#pragma once

#include "def.h"
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


struct Node{
    Handle      mHandle;
    HNode       mNextH;
    HNode       mPrevH;
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
                   "'handle::bitcount_index' is maxxed, need to adjsut List::stMaxIndex!" );
    static constexpr handle::index stMaxIndex = (handle::index{1} << handle::bitcount_index) - 1;
    static constexpr handle::index stMinSentinelIndex = stMaxIndex - 3;

    static constexpr HNode  stHeadHandle     = CreateHandle( 1, 0, stMinSentinelIndex + 0 );
    static constexpr HNode  stTailHandle     = CreateHandle( 1, 0, stMinSentinelIndex + 1 );
    static constexpr HNode  stHeadFreeHandle = CreateHandle( 1, 0, stMinSentinelIndex + 2 );
    static constexpr HNode  stTailFreeHandle = CreateHandle( 1, 0, stMinSentinelIndex + 3 );

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

bool    Expired     ( const Node& node ) noexcept;
void    IncrementAge( Node& node ) noexcept;
bool    IsStub      ( const Node& node )    noexcept;
//  SetNext/Prev: Ignores 'h' if it is a stub.
bool    SetNext     ( Node& node, HNode h ) noexcept;
bool    SetPrev     ( Node& node, HNode h ) noexcept;
void    Reset       ( Node& node )          noexcept;

} // end of namespace 'list'
} // end of namespace 'pt'



//-------------------------
// List
namespace pt{
namespace list{

const Node&     GetRef      ( const List& list, HNode  hNode )  noexcept;
Node&           GetRef      (       List& list, HNode  hNode )  noexcept;
void            Init        (       List& list               )  noexcept;
void            PushBack    (       List& list, Handle h     )  noexcept;
void            PushFront   (       List& list, Handle h     )  noexcept;
void            Remove      (       List& list, HNode  hNode )  noexcept;
HNode           Search      ( const List& list, Handle h     )  noexcept;


namespace priv{ // These expect already-sanitized inputs!

bool            ConnectAfterNode    (       List& list, HNode hTargetNode, HNode hNewNode ) noexcept;
bool            ConnectBeforeNode   (       List& list, HNode hTargetNode, HNode hNewNode ) noexcept;
void            DisconnectNode      (       List& list, HNode hNode ) noexcept;
bool            IndexValid          ( List& list, handle::index index ) noexcept;

// GetNode:
//  Returns the node at 'index'. Only returns stub, if index is invalid.
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
void            ReturnNode          (       List& list, HNode hNode ) noexcept;


// Returns a free node, creates one, if needed.
HNode           TakeFreeNode        (       List& list ) noexcept;

} // end of namespace 'priv'

} // end of namespace 'list'
} // end of namespace 'pt'


//-------------------------
// Node
PT_FORCE_INLINE bool pt::list::
Expired( const Node& node ) noexcept
{
    return node.mAge == handle::ageMax;
}


PT_FORCE_INLINE void pt::list::
IncrementAge( Node& node ) noexcept
{
    bool underLimit = ( node.mAge < handle::ageMax );
    node.mAge += underLimit ? 1 : 0 ;
}


PT_FORCE_INLINE bool pt::list::
IsStub( const Node& node ) noexcept
{
    return IsStub( node.mPrevH ) && IsStub( node.mNextH ) && IsStub( node.mHandle );
}


PT_FORCE_INLINE bool pt::list::
SetNext( Node& node, HNode hNode ) noexcept
{
    bool h_valid = !IsStub( hNode );
    node.mNextH = h_valid ? hNode : node.mNextH;
    return h_valid;
}


PT_FORCE_INLINE bool pt::list::
SetPrev( Node& node, HNode hNode ) noexcept
{
    bool h_valid = !IsStub( hNode );
    node.mPrevH = h_valid ? hNode : node.mPrevH;
    return h_valid;
}


PT_FORCE_INLINE void pt::list::
Reset( Node& node ) noexcept
{
    node = Node{};
}


//-------------------------
// List
PT_FORCE_INLINE const pt::list::Node& pt::list::
GetRef( const List& list, HNode hNode ) noexcept
{
    assert( priv::Initialized( list ) );
    bool suc;
    const Node& node = priv::GetNode( list, hNode.Index(), suc );
    bool valid = suc && ( hNode.Age()  == node.mAge );
    assert( hNode.Type() == 1 );
    assert( !IsStub( node ) );
    return valid ? node : priv::GetStub( list );
}


PT_FORCE_INLINE pt::list::Node& pt::list::
GetRef( List& list, HNode hNode ) noexcept
{
    assert( priv::Initialized( list ) );
    bool suc;
    Node& node = priv::GetNode( list, hNode.Index(), suc );
    bool valid = suc && ( hNode.Type() == 1 )
                     && ( hNode.Age()  == node.mAge );
    return valid ? node : priv::GetStub( list );
}


PT_FORCE_INLINE bool pt::list::priv::
ConnectAfterNode( List& list, HNode hTargetNode, HNode hNewNode ) noexcept
{
    Node& targetNode    = GetNode( list, hTargetNode.Index() );
    Node& newNode       = GetNode( list, hNewNode.Index() );
    Node& nextNode      = GetNode( list, targetNode.mNextH.Index() );

    // update connections
    newNode.mPrevH      = hTargetNode;
    newNode.mNextH      = targetNode.mNextH;
    targetNode.mNextH   = hNewNode;
    nextNode.mPrevH     = hNewNode;

    return true;
}


PT_FORCE_INLINE bool pt::list::priv::
ConnectBeforeNode( List& list, HNode hTargetNode, HNode hNewNode ) noexcept
{
    Node& targetNode = GetRef( list, hTargetNode );
    return ConnectAfterNode( list, targetNode.mPrevH, hNewNode );
}


PT_FORCE_INLINE void pt::list::priv::
DisconnectNode( List& list, HNode hNode ) noexcept
{
    Node&   node        = GetNode( list, hNode.Index() );
    Node&   prevNode    = GetNode( list, node.mPrevH.Index() );
    Node&   nextNode    = GetNode( list, node.mNextH.Index() );
    assert( !IsStub( nextNode ) );
    assert( !IsStub( prevNode ) );

    bool    prevStub    = IsStub( prevNode );
    bool    nextStub    = IsStub( nextNode );
    bool    valid       = ! ( prevStub || nextStub );

    // remove node from chain
    prevNode.mNextH  = valid ? node.mNextH : HNode{0};
    nextNode.mPrevH  = valid ? node.mPrevH : HNode{0};


    Reset( node.mNextH );
    Reset( node.mPrevH );

    ++node.mAge;

    bool underAgeLimit = ( handle::ageMax == node.mAge );
    HNode newHandle;
    newHandle.SetIndex( hNode.Index() + 1 );

    return underAgeLimit ? newHandle : HNode{0} ;
}


PT_FORCE_INLINE bool pt::list::priv::
IndexValid( List& list, handle::index i ) noexcept
{

}


PT_FORCE_INLINE const pt::list::Node& pt::list::priv::
GetNode( const List& list, handle::index i, bool& o_success ) noexcept
{
    assert( priv::Initialized( list ) );
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
    assert( priv::Initialized( list ) );
    o_success = true;

    if( (list.stMinSentinelIndex <= i) && ( i <= list.stMaxIndex ) ){
        handle::index iSentinel = i - list.stMinSentinelIndex;
        return list.mSentinels[ iSentinel ];
    }

    if( i < Size( list.mNodes ) ){
        return list.mNodes[i];
    }

    o_success = true;
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


PT_FORCE_INLINE void pt::list::priv::
ReturnNode( List& list, HNode hNode ) noexcept
{
    priv::DisconnectNode( list, hFreeNode );
    priv::ConnectAfterNode( list, list.stHeadHandle, hFreeNode );
}
