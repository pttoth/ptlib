#include "pt/string.h"

#include "pt/logging.h"

#include <assert.h>

using namespace pt;

string::
string()
{}


string::
string( const string& other )
{}


string::
string( string&& source )
{}


string::
~string()
{}


string& string::
operator=( const string& other )
{}


string& string::
operator=( string&& source )
{}


bool string::
operator==( const string& other ) const
{}

//--------------------------------------------------

//--------------------------------------------------

//--------------------------------------------------

size_t  mNumPartitions      = 1; // max: 512 (8-byte string blocks)
size_t  mNumEmptyPartitions = 1;
size_t  mNextEmptyPartition = 0;
char    mData[4096];


string::MemoryBlock::
MemoryBlock( size_t partition_count ):
    mNumPartitions( partition_count )
{
    assert( mNumPartitions <= 512 );    //  minimum partition size: 8
    assert( 0 == mNumPartitions % 8 );  //  partition size have to be multiple of 2
}

string::MemoryBlock::
~MemoryBlock()
{

}

//--------------------------------------------------

void string::MemoryBlock::
DetectNextEmptyPartition()
{
    if( IsFull() ){
        mNextEmptyPartition = 4096 % mNumPartitions; // might be unnecessary, as 'mNumEmptyPartitions' signals the block being full
        return;
    }

    for( size_t i = 0; i < mNumPartitions; ++i ){
        // if partition is empty (cannot contain empty string)
        size_t offset = ( (i + mNextEmptyPartition) * GetPartitionSize() ) % mNumPartitions ;
        if( 0 == mData[offset] ){
            mNextEmptyPartition = i;
            return;
        }
    }

    // should not be able to reach this point
    PT_LOG_ERR( "Memory management in 'pt::string' encountered a memory block in an invalid state. Block says it's not full, but couldn't find empty partition!" );
#ifdef PT_DEBUG_ENABLED
    pt::PrintStackTrace();
#endif
    exit(1);
}

//--------------------------------------------------

string::TempMemoryBlock::
TempMemoryBlock( size_t size ):
    mSize( size )
{
    size_t actual_size = 4096 * (int( size/4096 ) + bool(size % 4096));
    mData = new char[actual_size];
}


string::TempMemoryBlock::
~TempMemoryBlock()
{
    delete[] mData;
}


