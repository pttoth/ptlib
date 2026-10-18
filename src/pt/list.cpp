#include "pt/list.h"

//--------------------------------------------------
// List

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
}


pt::list::HNode pt::list::
Search( const List& list, Handle hNode ) noexcept
{
    if( IsStub( hNode ) ) return HNode{0};

    HNode hPrevNode      = list.stHeadHandle;
    HNode hCurrentNode;

    const Node* prevNode = &GetRef( list, hPrevNode );
    hCurrentNode         = prevNode->mNextH;

    u64 failsafeCount = 0;

    // traverse list
    while( hCurrentNode != list.stTailHandle ){
        const Node* currentNode = &GetRef( list, hCurrentNode );
        if( hNode == currentNode->mHandle ){
            return hCurrentNode;
        }

        prevNode = currentNode;

        // failsafe to prevent infinite loops if errors occur
        bool infiniteLoop = ( Size( list.mNodes ) < failsafeCount );
        assert( !infiniteLoop );
        if( infiniteLoop ) return HNode{0};

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

    HNode hNode = list.mHeadFree.mNextH;
    DisconnectNode( list, hNode );
    return hNode;
}
