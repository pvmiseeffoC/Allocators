#include <bitset>
#include <cassert>
#include <limits>
#include "Chunk.h"

namespace allocators
{
    template <class AllocationPolicy>
    bool Chunk<AllocationPolicy>::init( SizeType blockSize, DataType blocks )
    {
        assert( blockSize > 0 );
        assert( blocks > 0 );
        const auto sizeToAllocate = blockSize * blocks;
        assert( sizeToAllocate / blockSize == blocks );

        _dataPtr = AllocationPolicy::alloc( sizeToAllocate );
        if ( !_dataPtr )
        {
            LOG(
                "Allocation failed! with params blockSize %zu and numBlocks %d", blockSize, blocks );
            return false;
        }

        reset( blockSize, blocks );
        return true;
    }
    template <class AllocationPolicy>
    void* Chunk<AllocationPolicy>::allocate( SizeType blockSize )
    {
        if ( isFull() )
        {
            LOG( "Allocation called on full chunk!" );
            return nullptr;
        }

        auto offset = _firstAvailableBlock * blockSize;
        assert( offset / blockSize == _firstAvailableBlock );
        decltype( _dataPtr ) resultPtr = _dataPtr + offset;
        _firstAvailableBlock = *resultPtr;
        --_blocksAvailable;
        return resultPtr;
    }
    template <class AllocationPolicy>
    void Chunk<AllocationPolicy>::deallocate( void* p, SizeType blockSize )
    {
        assert( p >= _dataPtr );
        auto releasePtr = static_cast<DataType*>( p );

        assert( ( releasePtr - _dataPtr ) % blockSize == 0 );

        auto idx = static_cast<DataType>( ( releasePtr - _dataPtr ) / blockSize );

        // check if block was already deleted
        if ( _blocksAvailable < 0 )
        {
            assert( _firstAvailableBlock != idx );
        }

        *releasePtr = _firstAvailableBlock;
        _firstAvailableBlock = idx;

        assert( _firstAvailableBlock == ( releasePtr - _dataPtr ) / blockSize );
    }
    template <class AllocationPolicy>
    void Chunk<AllocationPolicy>::reset( SizeType blockSize, DataType blocks )
    {
        assert( blockSize > 0 );
        assert( blocks > 0 );

        assert( ( ( blockSize * blocks ) / blockSize ) == blocks );
        _blocksAvailable = blocks;
        _firstAvailableBlock = 0;

        // init each data block to store at its begining the number of the next
        // free data block as per Modern CPP Design
        DataType i = 0;
        for ( DataType* p = _dataPtr; i != blocks; p += blockSize )
        {
            *p = ++i;
        }
    }
    template <class AllocationPolicy>
    void Chunk<AllocationPolicy>::release()
    {
        assert( _dataPtr != nullptr );
        AllocationPolicy::free( _dataPtr );
    }
    template <class AllocationPolicy>
    bool Chunk<AllocationPolicy>::isCorrupted(
        SizeType blockSize,
        DataType blocks,
        bool checkIndexes ) const
    {
        if ( blocks < _blocksAvailable )
        {
            assert( false );
            return true;
        }

        if ( isFull() )
            return false;

        DataType index = _firstAvailableBlock;

        if ( blocks <= index )
        {
            assert( false );
            return true;
        }

        if ( !checkIndexes )
            return false;

        std::bitset<std::numeric_limits<DataType>::max()> found;

        DataType* next;

        for ( DataType i = 0;; )
        {
            next = _dataPtr + ( index * blockSize );
            found.set( index, true );
            ++i;
            if ( i >= _blocksAvailable )
                break;  // success

            index = *next;
            if ( blocks <= index )
            {  // block index bigger than block count
                assert( false );
                return true;
            }

            if ( found.test( index ) )
            {  // repeated index, loop in linked list
                assert( false );
                return true;
            }
        }

        if ( found.count() != _blocksAvailable )
        {
            // maybe we called free twice on the same block
            // and the loop didn't catch it
            assert( false );
            return true;
        }

        return false;
    }
    template <class AllocationPolicy>
    bool Chunk<AllocationPolicy>::isBlockAvailable( void* ptr, SizeType blockSize, DataType blocks)
    {
        if ( isFull() )
            return false;
        (void)blocks;
        DataType* location = static_cast<DataType*>( ptr );
        auto offset = location - _dataPtr;
        assert( offset % blockSize == 0 );
        auto blockIdx = static_cast<DataType>( offset / blockSize );

        auto index = _firstAvailableBlock;
        assert( blocks > index );

        if ( index == blockIdx )
            return true;

        std::bitset<std::numeric_limits<DataType>::max()> found;
        DataType* next = nullptr;
        for ( DataType i = 0;; )
        {
            next = _dataPtr + ( index * blockSize );
            found.set( index, true );

            ++i;
            if ( i >= _blocksAvailable )
                break;

            index = *next;

            assert( blocks > index );        // corruption checks
            assert( !found.test( index ) );  // loop in linked list

            if ( index == blockIdx )
                return true;
        }

        return false;
    }

    template class Chunk<NewAllocationPolicy>;
    template class Chunk<MallocAllocationPolicy>;
}  // namespace allocators
