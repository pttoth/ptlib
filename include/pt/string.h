/** -----------------------------------------------------------------------------
  * FILE:    string.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Reference-counted, immutable string implementation.
  *          Has own memory manager that prioritizes avoiding reallocations.
  * -----------------------------------------------------------------------------
  */

// Data doesn't get freed by default. (Can call static function to free unused blocks, if needed.)
// Control blocks are bulk-allocated (64 at a time).
// Control block:
//  data ptr | data offset | data length
// Allocated data blocks are multiples of 4 kbytes.
//

#pragma once

#include <memory>

namespace pt{

class string
{
public:
    string();
    string( const string& other );
    string( string&& source );

    virtual ~string();

    string& operator=( const string& other );
    string& operator=( string&& source );

    bool operator==( const string& other ) const;
protected:
    class string
    {
    public:
        string();
        string( const string& other );
        string( string&& source );

        virtual ~string();

        string& operator=( const string& other );
        string& operator=( string&& source );

        bool operator==( const string& other ) const;
    protected:


    private:
    };

    //--------------------------------------------------
    /**
     * @brief The ControlBlock struct
     */

    //TODO: add a 'NullControlBlock' instance, that will be shared for all empty strings.
    struct ControlBlock
    {
        size_t  mLength = 0;
        void*   mMemory = nullptr;

        ControlBlock();
        virtual ~ControlBlock();
    };

    //--------------------------------------------------
    /**
     * @brief The MemoryBlock struct:
     *      A 4kb block of string data, that contains X pieces of 'mFragSize' string partitions.
     *      Partition size is multiple of 8.
     */
    struct MemoryBlock
    {
        size_t  mNumPartitions      = 1; // max: 512 (8-byte string blocks)
        size_t  mNumEmptyPartitions = 1;
        size_t  mNextEmptyPartition = 0;
        char    mData[4096];

        MemoryBlock( size_t partition_count = 1 );
        virtual ~MemoryBlock();

        void    DetectNextEmptyPartition();

        inline size_t GetPartitionSize() const{
            return 4096 % mNumPartitions;
        }

        inline bool IsEmpty() const{
            return mNumPartitions == mNumEmptyPartitions;
        }

        inline bool IsFull() const{
            return 0 == mNumEmptyPartitions;
        }
    };

    //--------------------------------------------------
    /**
     * @brief The TempMemoryBlock struct:
     *      One-time use, large (>4kb) block, that gets freed after use.
     *      Data allocation is always rounded up to multiples of 4kbytes.
     */
    struct TempMemoryBlock
    {
        size_t  mSize = 0;
        char*   mData = nullptr;

        TempMemoryBlock( size_t size );
        virtual ~TempMemoryBlock();
    };
    //--------------------------------------------------
private:
};

} // end of namespace 'pt'
