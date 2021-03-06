#include "SmallObjectAllocator.h"

namespace allocators
{
    template < template<class> class THREADING_POLICY, class MUTEX_POLICY>
    void* SmallObjectAllocator<THREADING_POLICY, MUTEX_POLICY>::allocate( SizeType size )
    {
        if ( size > getMaxObjectSize() )
            return DefaultAllocator::alloc( size );

        size = size == 0 ? 1 : size;
        LOG( " Allocate called from %p ", this );

        const auto idx = getOffset( size, getAlignmentSize() ) - 1;
        const auto allocatorsSize = getOffset( getMaxObjectSize(), getAlignmentSize() );

        assert( idx < allocatorsSize );
        std::lock_guard<Lock> guard(_locks[idx]);
        auto& allocator = _allocators[idx];
        LOG( " Allocating from %p ", &allocator );

        assert( allocator.blockSize() >= size );
        assert( allocator.blockSize() < size + getAlignmentSize() );

        void* allocated = allocator.allocate();

        if ( !allocated && tryToFreeUpSomeMemory() )
        {
            allocated = allocator.allocate();
        }

        if ( !allocated )
        {
            LOG( " Allocation failed " );
            assert( false );
            throw std::bad_alloc();
        }
        return allocated;
    }

    template <template<class> class THREADING_POLICY, class MUTEX_POLICY>
    void SmallObjectAllocator<THREADING_POLICY, MUTEX_POLICY>::deallocate( void* ptr, SizeType size ) noexcept
    {
        if ( !ptr )
        {
            LOG( "Deallocate called with nullptr" );
            assert( false );
            return;
        }

        if ( size > getMaxObjectSize() )
        {
            DefaultAllocator::free( static_cast<DataType*>( ptr ) );
            return;
        }

        assert( _allocators );

        const auto idx = getOffset( size, getAlignmentSize() ) - 1;
        const auto allocatorsSize = getOffset( getMaxObjectSize(), getAlignmentSize() );
        assert( idx < allocatorsSize );

        std::lock_guard<Lock> guard(_locks[idx]);
        auto& allocator = _allocators[idx];
        LOG( " Deallocating from : %p ", &allocator );

        assert( allocator.blockSize() >= size );
        assert( allocator.blockSize() < size + getAlignmentSize() );

        const auto didDeallocate = allocator.deallocate( ptr, /*ChunkHint*/ nullptr );
        assert( didDeallocate );
        LOG( " Did Deallocate: %s ", booleanStr( didDeallocate ) );
    }

    template <template<class> class THREADING_POLICY, class MUTEX_POLICY>
    void SmallObjectAllocator<THREADING_POLICY, MUTEX_POLICY>::deallocate( void* ptr )
    {
        if ( !ptr )
        {
            LOG( "Deallocate called with nullptr" );
            assert( false );
            return;
        }

        assert( _allocators );

        const auto allocatorsSize = getOffset( getMaxObjectSize(), getAlignmentSize() );

        FixedAllocator* foundAlloc = nullptr;
        ChunkType const* foundChunk = nullptr;

        for ( SizeType i = 0; i < allocatorsSize; ++i )
        {
            foundChunk = _allocators[i].hasBlock( ptr );
            if ( foundChunk )
            {
                foundAlloc = &_allocators[i];
                break;
            }
        }

        if ( !foundAlloc )
        {
            DefaultAllocator::free( static_cast<DataType*>( ptr ) );
            return;
        }

        assert( foundChunk );
        const auto didDeallocate =
            foundAlloc->deallocate( ptr, const_cast<ChunkType*>( foundChunk ) );

        assert( didDeallocate );
        LOG( " DidDeallocate : %s ", booleanStr( didDeallocate ) );
    }

    template <template<class> class THREADING_POLICY, class MUTEX_POLICY>
    bool SmallObjectAllocator<THREADING_POLICY, MUTEX_POLICY>::tryToFreeUpSomeMemory()
    {
        bool didFreeMemory = false;

        const auto allocatorsSize = getOffset( getMaxObjectSize(), getAlignmentSize() );

        for ( SizeType i = 0; i < allocatorsSize; ++i )
        {
            std::lock_guard<Lock> guard(_locks[i]);
            didFreeMemory |= _allocators[i].freeEmptyChunk();
        }

        for ( SizeType i = 0; i < allocatorsSize; ++i )
        {
            std::lock_guard<Lock> guard(_locks[i]);
            didFreeMemory |= _allocators[i].tryToFreeUpSomeMemory();
        }

        return didFreeMemory;
    }

    template <template<class> class THREADING_POLICY, class MUTEX_POLICY>
    bool SmallObjectAllocator<THREADING_POLICY, MUTEX_POLICY>::corrupt() const
    {
        if ( !_allocators || getAlignmentSize() == 0 || getMaxObjectSize() == 0 )
        {
            LOG( " Corrupt SmallObjectAllocator %p", this );

            assert( false );
            return true;
        }

        const auto allocatorsSize = getOffset( getMaxObjectSize(), getAlignmentSize() );
        for ( SizeType i = 0; i < allocatorsSize; ++i )
        {
            std::lock_guard<Lock> guard(_locks[i]);
            if ( _allocators[i].isCorrupt() )
                return true;
        }

        return false;
    }

    template <template<class> class THREADING_POLICY, class MUTEX_POLICY>
    SizeType SmallObjectAllocator<THREADING_POLICY, MUTEX_POLICY>::getOffset( SizeType numBytes, SizeType alignment )
    {
        const auto extra = alignment - 1;
        return ( numBytes + extra ) / alignment;
    }

    template <template<class> class THREADING_POLICY, class MUTEX_POLICY>
    SmallObjectAllocator<THREADING_POLICY, MUTEX_POLICY>::SmallObjectAllocator(
        SizeType pageSize,
        SizeType maxObjectSize,
        SizeType objectAlignSize )
        : _maxObjectSize( maxObjectSize )
        , _objectAlignSize( objectAlignSize )
        , _allocators( nullptr )
    {
        LOG( " SmallObjectAllocator %p", this );
        const auto allocCount = getOffset( maxObjectSize, objectAlignSize );
        _allocators = new FixedAllocator[allocCount];
        _locks = new Lock[allocCount];

        for ( SizeType i = 0; i < allocCount; ++i )
            _allocators[i].init( ( i + 1 ) * objectAlignSize, pageSize );
    }
    template<template<class>class THREADING_POLICY, class MUTEX_POLICY>
    inline SmallObjectAllocator<THREADING_POLICY, MUTEX_POLICY>::~SmallObjectAllocator()
    {
        delete[] _allocators;
        delete[] _locks;
    }
}  // namespace allocators
