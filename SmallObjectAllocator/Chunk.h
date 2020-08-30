#pragma once
#include <cstdio>
#include <cstdlib>

namespace allocators
{
    struct NewAllocationPolicy;
    struct MallocAllocationPolicy;

    using DataType = std::uint8_t;
    using SizeType = std::size_t;

    template <class AllocationPolicy = NewAllocationPolicy>
    class Chunk
    {
    public:
        using AllocatorPolicy = AllocationPolicy;
    private:
        friend class FixedAllocator;

        bool init( SizeType blockSize, DataType blocks );

        void* allocate( SizeType blockSize );

        void deallocate( void* p, SizeType blockSize );

        void reset( SizeType blockSize, DataType blocks );

        void release();

        bool isCorrupted( SizeType blockSize, DataType blocks, bool checkIndexes ) const;

        bool isBlockAvailable( void* p, SizeType blockSize, DataType blocks );

        inline bool hasBlock( void* p, SizeType chunkSize ) const
        {
            auto ucPtr = static_cast<DataType*>( p );
            return ( ucPtr >= _dataPtr ) && ( _dataPtr + chunkSize > ucPtr );
        }

        inline bool isFull() const { return ( 0 == _blocksAvailable ); }

        DataType* _dataPtr;

        DataType _firstAvailableBlock;

        DataType _blocksAvailable;
    };

    using DefaultChunk = Chunk<NewAllocationPolicy>;

    struct NewAllocationPolicy
    {
        static DataType* alloc( std::size_t s )
        {
            return static_cast<DataType*>( ::operator new( s ) );
        }
        static void free( DataType* ptr ) { ::operator delete( ptr ); }
    };

    struct MallocAllocationPolicy
    {
        static DataType* alloc( std::size_t s )
        {
            return static_cast<DataType*>( ::std::malloc( s ) );
        }
        static void free( DataType* ptr ) { ::std::free( static_cast<void*>( ptr ) ); }
    };
}  // namespace allocators
