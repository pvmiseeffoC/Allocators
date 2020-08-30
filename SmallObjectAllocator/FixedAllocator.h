#pragma once
#include <limits>
#include <vector>
#include "Chunk.h"

namespace allocators
{
    class FixedAllocator
    {
    public:
        using ChunkType = DefaultChunk;
        using Chunks = std::vector<ChunkType>;

    private:

        void deallocate( void* p );

        bool makeChunk();

        ChunkType* findChunk( void* p );

        DataType _minObjectsPerChunk = 8;
        DataType _maxObjectsPerChunk = std::numeric_limits<DataType>::max();

        SizeType _blockSize;
        DataType _numBlocks;

        Chunks _chunks;

        ChunkType* _allocChunk;
        ChunkType* _deallocChunk;
        ChunkType* _emptyChunk;

    public:
        FixedAllocator();

        /// disable copying
        FixedAllocator( FixedAllocator const& ) = delete;
        FixedAllocator& operator=( FixedAllocator const& ) = delete;

        /// Allow move sematincs
        FixedAllocator( FixedAllocator && ) = default;
        FixedAllocator& operator=( FixedAllocator && ) = default;

        ~FixedAllocator();

        void init( SizeType blockSize, SizeType pageSize );

        void* allocate();

        bool deallocate( void* p, ChunkType* hint );

        inline SizeType blockSize() const { return _blockSize; }

        bool freeEmptyChunk();

        bool tryToFreeUpSomeMemory();

        SizeType emptyChunks( bool fast = true ) const;

        bool isCorrupt() const;

        const ChunkType* hasBlock( void* p ) const;
    };
}  // namespace allocators
