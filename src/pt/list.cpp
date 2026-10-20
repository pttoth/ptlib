#include "pt/list.h"

//--------------------------------------------------
// List

void pt::list::
Init( List& list ) noexcept
{
    if( priv::Initialized( list ) ) return;

    priv::GetSentinelHead( list ) =
        Node{ .mHandle      = Handle{0},
              .mNextIndex   = List::stTailHandle.Index(),
              .mPrevIndex   = List::stInvalidIndex,
              .mAge         = 0 };

    priv::GetSentinelTail( list ) =
        Node{ .mHandle      = Handle{0},
              .mNextIndex   = List::stInvalidIndex,
              .mPrevIndex   = List::stHeadHandle.Index(),
              .mAge         = 0 };

    priv::GetSentinelHeadFree( list ) =
        Node{ .mHandle      = Handle{0},
              .mNextIndex   = List::stTailFreeHandle.Index(),
              .mPrevIndex   = List::stInvalidIndex,
              .mAge         = 0 };

    priv::GetSentinelTailFree( list ) =
        Node{ .mHandle      = Handle{0},
              .mNextIndex   = List::stInvalidIndex,
              .mPrevIndex   = List::stHeadFreeHandle.Index(),
              .mAge         = 0 };

    list.mStubRO = Node{ .mHandle       = Handle{0},
                         .mNextIndex    = List::stInvalidIndex,
                         .mPrevIndex    = List::stInvalidIndex,
                         .mAge          = 0 };

    list.mStubRW = Node{ .mHandle       = Handle{0},
                         .mNextIndex    = List::stInvalidIndex,
                         .mPrevIndex    = List::stInvalidIndex,
                         .mAge          = 0 };

}


void pt::list::
PushBack( List& list, Handle h ) noexcept
{
    IF_UNLIKELY( !priv::Initialized( list ) ){
        Init( list );
    }

    handle::index   iFreeNode   = priv::PrepareFreeNode( list );
    IF_UNLIKELY( List::stInvalidIndex == iFreeNode ){
        // TODO: log error
        return;
    }

    bool suc;
    Node&   node    = priv::GetNode( list, iFreeNode, suc );
    node.mHandle = h;

    priv::ConnectBeforeNode( list, list.stTailHandle.Index(), iFreeNode );
}


void pt::list::
PushFront( List& list, Handle h ) noexcept
{
    IF_UNLIKELY( !priv::Initialized( list ) ){
        Init( list );
    }

    handle::index   iFreeNode   = priv::PrepareFreeNode( list );
    IF_UNLIKELY( List::stInvalidIndex == iFreeNode ){
        // TODO: log error
        return;
    }

    bool suc;
    Node&   node    = priv::GetNode( list, iFreeNode, suc );
    node.mHandle = h;

    priv::ConnectAfterNode( list, list.stHeadHandle.Index(), iFreeNode );
}


void pt::list::
Remove( List& list, HNode hNode )  noexcept
{
    IF_UNLIKELY( !priv::Initialized( list ) ){
        Init( list );
    }

    bool badHandle = ( !priv::ValidHNode( hNode ) )
                  || ( List::stHeadHandle == hNode )
                  || ( List::stTailHandle == hNode )
                  || ( List::stHeadFreeHandle == hNode )
                  || ( List::stTailFreeHandle == hNode );
    IF_UNLIKELY( badHandle ) return;

    Node& node = GetRef( list, hNode );
    IF_UNLIKELY( IsStub( node ) ) return;

    priv::DisconnectNode( list, hNode.Index() );
    IncrementAge( node );
    priv::ConnectBeforeNode( list, list.stTailFreeHandle.Index(), hNode.Index() );
}


pt::list::HNode pt::list::
Search( const List& list, Handle h ) noexcept
{
    IF_UNLIKELY( !priv::Initialized( list ) ){
        return HNode{0};
    }

    handle::index iCurrentNode = priv::GetSentinelHead( list ).mNextIndex;
    u64 failsafeCount = 0;
    u64 failsafeMax   = Size( list.mNodes ) + 1;

    // traverse list
    while( true ){
        // error in chain, or over-iteration, exit
        bool error = (List::stInvalidIndex == iCurrentNode);
        assert( !error );
        IF_UNLIKELY( error ) return HNode{0};

        // reached tail
        if( List::stTailHandle.Index() == iCurrentNode ) return HNode{0};

        // found item
        bool suc;
        const Node& node = priv::GetNode( list, iCurrentNode, suc );
        if( node.mHandle == h ){
            return CreateHandle( 1, node.mAge, iCurrentNode );
        }

        // not found item
        iCurrentNode = node.mNextIndex;
        ++failsafeCount;

        // failsafe: iteration got lost (wrong chain, or some error too weird)
        bool failsafeTriggered = ( failsafeCount >= failsafeMax );
        assert( !failsafeTriggered );
        IF_UNLIKELY( failsafeTriggered ) return HNode{0};
    }

    return HNode{0};
}


//--------------------------------------------------
// list::priv::
pt::handle::index pt::list::priv::
PrepareFreeNode( List& list ) noexcept
{
    handle::index   currentIndex    = List::stInvalidIndex;
    Node*           currentNode     = &GetSentinelHeadFree( list );

    while( true ){
        bool suc;
        currentIndex = currentNode->mNextIndex;
        currentNode = &GetNode( list, currentIndex, suc );

        // no empty nodes available
        if( currentIndex == List::stTailFreeHandle.Index() ){
            // add empty node
            handle::index index = Size( list.mNodes );
            bool outOfIndexSpace = ( index >= List::stMinSentinelIndex );
            IF_UNLIKELY( outOfIndexSpace ){
                // TODO: log error message "pt::List has ran out of index space"
                //  debug stacktrace
                return List::stInvalidIndex;
            }

            EmplaceBack( list.mNodes );
            ConnectAfterNode( list, list.stHeadFreeHandle.Index(), index );

        // found node, but exhausted
        }else if( Exhausted( *currentNode ) ){
            // Fully disconnect the exhausted Node
            DisconnectNode( list, currentIndex );
            // Disconnect() broke the chain here
            //  all previous elements have been removed here so far,
            //  so headFree is definitely the previous node, restore that
            currentIndex = List::stHeadFreeHandle.Index();
            currentNode  = &GetSentinelHeadFree( list );

        // found usable node
        }else{
            DisconnectNode( list, currentIndex );
            currentNode->mHandle = Handle{0};
            return currentIndex;
        }
    }

    assert( false );    // unknown error, should not be reached
    return List::stInvalidIndex;
}
