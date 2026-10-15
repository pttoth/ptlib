/** -----------------------------------------------------------------------------
  * FILE:    list.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Linked list class.
  *           Can only hold handles, no templates.
  * -----------------------------------------------------------------------------
  */


#pragma once

#include "def.h"
#include "stdutil.h"
#include <cassert>
#include <vector>

//--------------------------------------------------
namespace pt{

struct Handle
{
    u32     mAge;
    u32     mIndex;

    bool operator==( const Handle& rhs ) const{
        return ( mAge == rhs.mAge ) && ( mIndex && rhs.mIndex );
    }
};


//-----
namespace list{

// TODO: use this
struct HNode: public Handle
{};

// TODO: drop this
using NodeIndex = s32;


struct Node{
    static const NodeIndex  stInvalidIndex  = INT32_MIN;

    Handle    mHandle;
    NodeIndex mNextIndex      = stInvalidIndex;
    NodeIndex mPreviousIndex  = stInvalidIndex;
};

} // end of namespace 'list'
//-----


struct List
{
    using Node      = list::Node;
    using NodeIndex = list::NodeIndex;

    static const NodeIndex  stHeadIndex         = -1;
    static const NodeIndex  stTailIndex         = -2;
    static const NodeIndex  stHeadFreeIndex     = -3;
    static const NodeIndex  stTailFreeIndex     = -4;

    std::vector<Node>   mNodes;

    Node    mHead;
    Node    mHeadFree;
    Node    mTail;
    Node    mTailFree;
    Node    mStub;
};

} // end of namespace 'pt'


//--------------------------------------------------
//  Functions
//--------------------------------------------------
namespace pt{
namespace list{

// Node
bool            IsStub              ( const Node& node );

// List
void            PushBack            ( List& list, Handle handle );
void            PushFront           ( List& list, Handle handle );


namespace priv{

void            AddEmptyNode        (       List& list );
bool            ConnectAfterNode    (       List& list, s32 indexTarget, s32 indexNewNode );
bool            ConnectBeforeNode   (       List& list, s32 indexTarget, s32 indexNewNode );
void            DisconnectNode      (       List& list, NodeIndex nodeIndex );
bool            HasFreeNode         ( const List& list );
const Node&     ResolveNodeIndex    ( const List& list, s32 nodeIndex );
Node&           ResolveNodeIndex    (       List& list, s32 nodeIndex );
NodeIndex       Search              ( const List& list, Handle h );
NodeIndex       TakeFreeNode        (       List& list );
NodeIndex       TakeFreeNodeForced  (       List& list );
bool            ValidNodeIndex      ( const List& list, s32 index );

} // end of namespace 'priv'
} // end of namespace 'list'
} // end of namespace 'pt'

//-------------------------
// Node
bool pt::list::
IsStub( const Node& node )
{
    return ( INT32_MIN == node.mPreviousIndex )
        && ( INT32_MIN == node.mNextIndex );
}


//-------------------------
// List
void pt::list::priv::
AddEmptyNode( List& list )
{
    s32 newNodeIndex = Size( list.mNodes );
    EmplaceBack( list.mNodes );
    ConnectAfterNode( list, list.stHeadFreeIndex, newNodeIndex );
}


bool pt::list::priv::
HasFreeNode( const List& list )
{
    return list.stTailFreeIndex == list.mHeadFree.mNextIndex;
}


bool pt::list::priv::
ConnectAfterNode( List& list, s32 indexTarget, s32 indexNewNode )
{
    bool paramsValid = ValidNodeIndex( list, indexTarget )
                    && ValidNodeIndex( list, indexNewNode );
    assert( paramsValid );
    if( !paramsValid ) return false;

    Node& targetNode    = ResolveNodeIndex( list, indexTarget );
    Node& newNode       = ResolveNodeIndex( list, indexNewNode );
    Node& nextNode      = ResolveNodeIndex( list, targetNode.mNextIndex );

    newNode.mPreviousIndex  = indexTarget;
    newNode.mNextIndex      = targetNode.mNextIndex;

    nextNode.mPreviousIndex = indexNewNode;
    targetNode.mNextIndex   = indexNewNode;

    return true;
}


bool pt::list::priv::
ConnectBeforeNode( List& list, s32 indexTarget, s32 indexNewNode )
{
    bool paramsValid = ValidNodeIndex( list, indexTarget )
                    && ValidNodeIndex( list, indexNewNode );
    assert( paramsValid );
    if( !paramsValid ) return false;


    Node& node = ResolveNodeIndex( list, indexTarget );
    return ConnectAfterNode( list, node.mPreviousIndex, indexNewNode );
}


void pt::list::
PushBack( List& list, Handle handle )
{
    NodeIndex   nodeIndex       = priv::TakeFreeNodeForced( list );
    Node&       node            = priv::ResolveNodeIndex( list, nodeIndex );

    node.mHandle = handle;

    priv::DisconnectNode( list, nodeIndex );
    priv::ConnectBeforeNode( list, list.stTailIndex, nodeIndex );
}


void pt::list::
PushFront( List& list, Handle handle )
{
    NodeIndex   nodeIndex   = priv::TakeFreeNodeForced( list );
    Node&       node        = priv::ResolveNodeIndex( list, nodeIndex );

    node.mHandle = handle;

    priv::DisconnectNode( list, nodeIndex );
    priv::ConnectAfterNode( list, list.stHeadIndex, nodeIndex );
}


void pt::list::priv::
DisconnectNode( List& list, NodeIndex nodeIndex )
{
    Node&       node        = ResolveNodeIndex( list, nodeIndex );
    Node&       nextNode    = ResolveNodeIndex( list, node.mNextIndex );
    Node&       prevNode    = ResolveNodeIndex( list, node.mPreviousIndex );

    // remove node from chain
    prevNode.mNextIndex         = node.mNextIndex;
    nextNode.mPreviousIndex     = node.mPreviousIndex;
    // reset node indices
    node.mNextIndex             = Node::stInvalidIndex;
    node.mPreviousIndex         = Node::stInvalidIndex;
}


const pt::list::Node& pt::list::priv::
ResolveNodeIndex( const List& list, s32 nodeIndex )
{
    if( -1 < nodeIndex && nodeIndex < (s32) Size( list.mNodes ) ){
        return list.mNodes[nodeIndex];
    }

    if( list.stHeadIndex      == nodeIndex )  return list.mHead;
    if( list.stTailIndex      == nodeIndex )  return list.mTail;
    if( list.stHeadFreeIndex  == nodeIndex )  return list.mHeadFree;
    if( list.stTailFreeIndex  == nodeIndex )  return list.mTailFree;

    return list.mStub;
}


pt::list::Node& pt::list::priv::
ResolveNodeIndex( List& list, s32 nodeIndex )
{
    if( -1 < nodeIndex && nodeIndex < (s32) Size( list.mNodes ) ){
        return list.mNodes[nodeIndex];
    }

    if( list.stHeadIndex      == nodeIndex )  return list.mHead;
    if( list.stTailIndex      == nodeIndex )  return list.mTail;
    if( list.stHeadFreeIndex  == nodeIndex )  return list.mHeadFree;
    if( list.stTailFreeIndex  == nodeIndex )  return list.mTailFree;

    return list.mStub;
}


pt::list::NodeIndex pt::list::priv::
Search( const List& list, Handle h )
{
    const Node* prevNode            = &list.mHead;
    NodeIndex   currentNodeIndex    = prevNode->mNextIndex;

    u64 failsafeCount = 0;

    // traverse list
    while( currentNodeIndex != list.stTailIndex ){
        const Node* currentNode = &ResolveNodeIndex( list, currentNodeIndex );
        if( h == currentNode->mHandle ){
            return currentNodeIndex;
        }

        prevNode = currentNode;

        // failsafe to prevent infinite loops if errors occur
        bool infiniteLoop = ( Size( list.mNodes ) < failsafeCount );
        assert( !infiniteLoop );
        if( infiniteLoop ) return Node::stInvalidIndex;

        ++failsafeCount;
    }

    return Node::stInvalidIndex;
}


pt::list::NodeIndex pt::list::priv::
TakeFreeNode( List& list )
{
    if( !HasFreeNode( list ) ) return Node::stInvalidIndex;

    DisconnectNode( list, list.mHeadFree.mNextIndex );
    return list.mHeadFree.mNextIndex;
}


pt::list::NodeIndex pt::list::priv::
TakeFreeNodeForced( List& list )
{
    if( !HasFreeNode( list ) ){
        AddEmptyNode( list );
    }

    return TakeFreeNode( list );
}


bool pt::list::priv::
ValidNodeIndex( const List& list, s32 index )
{
    return ( -1 < index ) && ( index < (s32) Size( list.mNodes ) );
}
