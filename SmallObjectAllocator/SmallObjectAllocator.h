#pragma once
#include "FixedAllocator.h"

namespace allocators
{
    class SmallObjectAllocator
    {
    private:
        std::unique_ptr<FixedAllocator[]> _allocators;

        const SizeType _maxObjectSize;
        const SizeType _objectAlignSize;

    private:
        using ChunkType = typename FixedAllocator::ChunkType;
        using DefaultAllocator = typename ChunkType::AllocatorPolicy;

    public:
        SmallObjectAllocator() = delete;
        SmallObjectAllocator( SmallObjectAllocator const& ) = delete;
        SmallObjectAllocator& operator=( SmallObjectAllocator const& ) = delete;

    public:
        SmallObjectAllocator( SmallObjectAllocator&& ) = default;
        SmallObjectAllocator& operator=( SmallObjectAllocator&& ) = default;

    public:
        void* allocate( SizeType size );

        void deallocate( void* ptr, SizeType size ) noexcept;

        void deallocate( void* ptr );

        bool tryToFreeUpSomeMemory();

        bool corrupt() const;

        inline SizeType getMaxObjectSize() const { return _maxObjectSize; }

        inline SizeType getAlignmentSize() const { return _objectAlignSize; }

        static SizeType getOffset( SizeType numBytes, SizeType alignment );

    protected:
        SmallObjectAllocator( SizeType pageSize, SizeType maxObjectSize, SizeType objectAlignSize );

        ~SmallObjectAllocator() = default;
    };
}  // namespace allocators
