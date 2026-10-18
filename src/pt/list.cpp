#include "pt/list.h"

//--------------------------------------------------
// List

void pt::list::
Init( List& list ) noexcept
{
    priv::GetSentinelHead( list ) =
        Node{ .mHandle  = 0,
              .mNextH   = List::stTailHandle,
              .mPrevH   = 0,
              .mAge     = 0 };

    priv::GetSentinelTail( list ) =
        Node{ .mHandle  = 0,
              .mNextH   = 0,
              .mPrevH   = List::stHeadHandle,
              .mAge     = 0 };

    priv::GetSentinelHeadFree( list ) =
        Node{ .mHandle  = 0,
              .mNextH   = List::stTailFreeHandle,
              .mPrevH   = 0,
              .mAge     = 0 };

    priv::GetSentinelTailFree( list ) =
        Node{ .mHandle  = 0,
              .mNextH   = 0,
              .mPrevH   = List::stHeadFreeHandle,
              .mAge     = 0 };

    list.mStubRO = Node{ .mHandle   = 0,
                         .mNextH    = 0,
                         .mPrevH    = 0,
                         .mAge      = 0 };

    list.mStubRW = Node{ .mHandle   = 0,
                         .mNextH    = 0,
                         .mPrevH    = 0,
                         .mAge      = 0 };

}


void pt::list::
PushBack( List& list, Handle h ) noexcept
{
    HNode   hNode   = priv::TakeFreeNode( list );
    Node&   node    = GetRef( list, hNode );

    if( IsStub( node ) ) return;

    node.mHandle = h;

    priv::DisconnectNode( list, hNode );
    priv::ConnectBeforeNode( list, list.stTailHandle, hNode );
}


void pt::list::
PushFront( List& list, Handle h ) noexcept
{
    HNode   hNode   = priv::TakeFreeNode( list );
    Node&   node    = GetRef( list, hNode );

    if( IsStub( node ) ) return;

    node.mHandle = h;

    priv::DisconnectNode( list, hNode );
    priv::ConnectAfterNode( list, list.stHeadHandle, hNode );
}


void pt::list::
Remove( List& list, HNode hNode )  noexcept
{
    Node& node = GetRef( list, hNode );
    if( IsStub( node ) ) return;

    priv::DisconnectNode( list, hNode );
    priv::ConnectAfterNode( list, list.stHeadFreeHandle, hNode );
    // TODO: need generation increase here
    // TODO: need some guarantee, that node is part of the active chain

}


pt::list::HNode pt::list::
Search( const List& list, Handle hNode ) noexcept
{
    if( IsStub( hNode ) ) return HNode{0};

    HNode hCurrentNode = priv::GetSentinelHead( list ).mNextH;
    u64 failsafeCount = 0;

    // traverse list
    while( hCurrentNode != list.stTailHandle ){
        // failsafe to prevent infinite loops if errors occur
        // TODO: need to count for sentinels too
        bool error = ( Size( list.mNodes ) < failsafeCount );
        assert( !error );
        if( error ) return HNode{0};


        const Node* currentNode = &GetRef( list, hCurrentNode );
        if( hNode == currentNode->mHandle ){
            return hCurrentNode;
        }

        hCurrentNode = currentNode->mNextH;
        ++failsafeCount;
    }

    return HNode{0};
}


pt::list::HNode pt::list::priv::
TakeFreeNode( List& list ) noexcept
{
    // check for available free nodes
    const Node& headNode = GetRef_NoChecks( list, list.stHeadFreeHandle );
    if( headNode.mNextH == list.stTailFreeHandle ){
        // add empty node
        handle::index index = Size( list.mNodes );
        bool err_OutOfIndexSpace = ( List::stMinSentinelIndex <= index );
        assert( !err_OutOfIndexSpace );
        if( err_OutOfIndexSpace ) return HNode{0};

        EmplaceBack( list.mNodes );
        HNode newNode = CreateHandle( handle::type{1}, handle::age{0}, index );
        ConnectAfterNode( list, list.stHeadFreeHandle, newNode );
    }

    // TODO: need head.next == tail validation here

    HNode hNode = list.mSentinels.name.mHeadFree.mNextH;
    DisconnectNode( list, hNode );
    return hNode;
}
