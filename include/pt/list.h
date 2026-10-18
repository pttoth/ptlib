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

    union Sentinels{
        struct S{
            Node   mHead;
            Node   mTail;
            Node   mHeadFree;
            Node   mTailFree;
            Node   mStubRW;
            Node   mStubRO;
        } name;
        Node v[6];
    };

    // The few highest value indices are taken for sentinel nodes (head and tail);
    static_assert( 8*sizeof(handle::index) != handle::bitcount_index,
                   "'handle::bitcount_index' is maxxed, need to adjsut List::stMaxIndex!" );
    static constexpr handle::index stMaxIndex = (handle::index{1} << handle::bitcount_index) - 1;
    static constexpr handle::index stMinSentinelIndex = stMaxIndex - 6;

    static constexpr HNode  stHeadHandle     = CreateHandle( 1, 0, stMinSentinelIndex + 0 );
    static constexpr HNode  stTailHandle     = CreateHandle( 1, 0, stMinSentinelIndex + 1 );
    static constexpr HNode  stHeadFreeHandle = CreateHandle( 1, 0, stMinSentinelIndex + 2 );
    static constexpr HNode  stTailFreeHandle = CreateHandle( 1, 0, stMinSentinelIndex + 3 );
    static constexpr HNode  stStubRWHandle   = CreateHandle( 1, 0, stMinSentinelIndex + 4 );
    static constexpr HNode  stStubROHandle   = CreateHandle( 1, 0, stMinSentinelIndex + 5 );

    //-----

    std::vector<Node>   mNodes;


    Sentinels mSentinels;


};

} // end of namespace 'pt'


//--------------------------------------------------
//  Functions
//--------------------------------------------------
//-------------------------
// Node
namespace pt{
namespace list{

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
void            PushBack    (       List& list, Handle h     )  noexcept;
void            PushFront   (       List& list, Handle h     )  noexcept;
void            Remove      (       List& list, HNode  hNode )  noexcept;
HNode           Search      ( const List& list, Handle h     )  noexcept;


namespace priv{
// These expect already-sanitized inputs!

bool            ConnectAfterNode    (       List& list, HNode hTargetNode, HNode hNewNode ) noexcept;
bool            ConnectBeforeNode   (       List& list, HNode hTargetNode, HNode hNewNode ) noexcept;
void            DisconnectNode      (       List& list, HNode hNode ) noexcept;
const Node&     GetRef_NoChecks     ( const List& list, HNode hNode ) noexcept;
Node&           GetRef_NoChecks     (       List& list, HNode hNode ) noexcept;

// Returns a free node, creates one, if needed.
HNode           TakeFreeNode        (       List& list ) noexcept;

} // end of namespace 'priv'

} // end of namespace 'list'
} // end of namespace 'pt'


//-------------------------
// Node
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


PT_FORCE_INLINE bool pt::list::
IsStub( const Node& node ) noexcept
{
    return IsStub( node.mPrevH ) && IsStub( node.mNextH ) && IsStub( node.mHandle );
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
    const Node& node = priv::GetRef_NoChecks( list, hNode );
    bool valid = IsStub( node )
              || ( hNode.Type() != 1 )
              || ( hNode.Age()  != node.mAge );
    return valid ? node : list.mSentinels.name.mStubRO;
}


PT_FORCE_INLINE pt::list::Node& pt::list::
GetRef( List& list, HNode hNode ) noexcept
{
    Node& node = priv::GetRef_NoChecks( list, hNode );
    bool valid = IsStub( node )
              || ( hNode.Type() != 1 )
              || ( hNode.Age()  != node.mAge );
    return valid ? node : list.mSentinels.name.mStubRW;
}


PT_FORCE_INLINE bool pt::list::priv::
ConnectAfterNode( List& list, HNode hTargetNode, HNode hNewNode ) noexcept
{
    Node& targetNode    = GetRef_NoChecks( list, hTargetNode );
    Node& newNode       = GetRef_NoChecks( list, hNewNode );
    Node& nextNode      = GetRef_NoChecks( list, targetNode.mNextH );

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
    Node&   node        = GetRef_NoChecks( list, hNode );
    Node&   nextNode    = GetRef_NoChecks( list, node.mNextH );
    Node&   prevNode    = GetRef_NoChecks( list, node.mPrevH );

    // remove node from chain
    prevNode.mNextH  = node.mNextH;
    nextNode.mPrevH  = node.mPrevH;
    Reset( node.mNextH );
    Reset( node.mPrevH );
}


PT_FORCE_INLINE const pt::list::Node& pt::list::priv::
GetRef_NoChecks( const List& list, HNode hNode ) noexcept
{
    handle::index i = hNode.Index();
    if( (list.stMinSentinelIndex <= i) && ( i <= list.stMaxIndex ) ){
        handle::index iSentinel = i - list.stMinSentinelIndex;
        return list.mSentinels.v[ iSentinel ];
    }

    if( i < Size( list.mNodes ) ){
        return list.mNodes[i];
    }

    return list.mSentinels.name.mStubRO;
}


PT_FORCE_INLINE pt::list::Node& pt::list::priv::
GetRef_NoChecks( List& list, HNode hNode ) noexcept
{
    handle::index i = hNode.Index();
    if( (list.stMinSentinelIndex <= i) && ( i <= list.stMaxIndex ) ){
        handle::index iSentinel = i - list.stMinSentinelIndex;
        return list.mSentinels.v[ iSentinel ];
    }

    if( i < Size( list.mNodes ) ){
        return list.mNodes[i];
    }

    return list.mSentinels.name.mStubRW;
}


