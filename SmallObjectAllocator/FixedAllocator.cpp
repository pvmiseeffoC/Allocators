#include "FixedAllocator.h"

namespace allocators
{
    bool FixedAllocator::makeChunk()
    {
        bool didAlloc = false;
        try
        {
            auto size = _chunks.size();

            if ( _chunks.capacity() == size )
            {
                size = std::max( size, static_cast<decltype( size )>( 8 ) );
                _chunks.reserve( size * 2 );
            }
            ChunkType newChunk;
            didAlloc = newChunk.init( _blockSize, _numBlocks );
            if ( didAlloc )
                _chunks.emplace_back( newChunk );
        }
        catch ( ... )
        {
            LOG("makeChunk Threw Exception");
            didAlloc = false;
        }
        if ( !didAlloc )
        {
            LOG("makeChunk did not succeed!");
            return false;
        }
        _allocChunk = &_chunks.back();
        _deallocChunk = &_chunks.front();
        return true;
    }

    FixedAllocator::ChunkType* FixedAllocator::findChunk( void* p )
    {
        if ( _chunks.empty() )
            return nullptr;

        // bi directional sliding window
        assert( _deallocChunk );
        const SizeType chunkSize = _blockSize * _numBlocks;
        ChunkType* low = _deallocChunk;
        ChunkType* high = _deallocChunk + 1;
        const ChunkType* lowBound = &_chunks.front();
        const ChunkType* highBound = &_chunks.back();

        if ( high == highBound )
            high = nullptr;

        while ( true )
        {
            if ( high )
            {
                if ( high->hasBlock( p, chunkSize ) )
                    return high;

                if ( ++high == highBound )
                {
                    high = nullptr;
                    if ( !low )
                        break;
                }
            }

            if ( low )
            {
                if ( low->hasBlock( p, chunkSize ) )
                    return low;

                if ( low-- == lowBound )
                {
                    low = nullptr;
                    if ( !high )
                        break;
                }
            }
        }

        return nullptr;
    }

    FixedAllocator::FixedAllocator()
        : _blockSize( 0 )
        , _numBlocks( 0 )
        , _chunks( 0 )
        , _allocChunk( nullptr )
        , _deallocChunk( nullptr )
        , _emptyChunk( nullptr )
    {
    }

    FixedAllocator::~FixedAllocator()
    {
        for ( auto i = std::begin( _chunks ); i != std::end( _chunks ); ++i )
            i->release();
    }

    void FixedAllocator::init( SizeType blockSize, SizeType pageSize )
    {
        LOG( "blockSize:  %zu | pageSize:  %zu", blockSize, pageSize );
        assert( blockSize > 0 );
        assert( pageSize >= blockSize );
        _blockSize = blockSize;

        auto blockCount = static_cast<DataType>( pageSize / blockSize );

        assert( static_cast<SizeType>( blockCount ) == pageSize / blockSize );

        _numBlocks = std::max( _numBlocks, _minObjectsPerChunk );
        _numBlocks = std::min( _numBlocks, _maxObjectsPerChunk );
    }

    void* FixedAllocator::allocate()
    {
        assert( !_emptyChunk || _emptyChunk->_blocksAvailable == _numBlocks );

        if ( !_allocChunk || _allocChunk->isFull() )
        {
            if ( !_emptyChunk )
            {
                _allocChunk = _emptyChunk;
                _emptyChunk = nullptr;
            }
            else
            {
                for ( auto i = std::begin( _chunks );; ++i )
                {
                    if ( i == std::end( _chunks ) )
                    {
                        if ( !makeChunk() )
                            return nullptr;
                        break;
                    }
                    if ( !i->isFull() )
                    {
                        _allocChunk = &( *i );
                        break;
                    }
                }
            }
        }
        else if ( _allocChunk == _emptyChunk )
        {
            _emptyChunk = nullptr;
        }

        assert( _allocChunk != nullptr );
        assert( !_allocChunk->isFull() );

        auto ptr = _allocChunk->allocate( _blockSize );
        assert( !_emptyChunk || _emptyChunk->_blocksAvailable == _numBlocks );

        LOG( " %p ", ptr );

#if defined( _DEBUG ) || defined( DEBUG )
        if ( _allocChunk->isCorrupted( _blockSize, _numBlocks, true ) )
        {
            assert( false );
            return nullptr;
        }
#endif

        return ptr;
    }

    bool FixedAllocator::deallocate( void* p, ChunkType* hint )
    {
        LOG( " PTR :  %p | ChunkHint :  %p ", p, hint );
        assert( !_chunks.empty() );
        assert( inRange( _chunks, _deallocChunk ) );
        assert( inRange( _chunks, _allocChunk ) );

        ChunkType* found = hint ? hint : findChunk( p );
        assert( found->hasBlock( p, _numBlocks * _blockSize ) );

#if defined( DEBUG_INFO )
        if ( found->isCorrupted( _blockSize, _numBlocks, /*checkIndexes*/ true ) )
        {
            assert( false );
            return false;
        }
        else if ( found->isBlockAvailable( p, _blockSize, _numBlocks ) )
        {  // block is free already
            // deallocate called twice for same block?
            assert( false );
            return false;
        }
#endif
        _deallocChunk = found;
        deallocate( p );

        return true;
    }

    void FixedAllocator::deallocate( void* p )
    {
        LOG( " %p ", p );

        assert( _deallocChunk->hasBlock( p, _numBlocks * _blockSize ) );
        assert( _emptyChunk != _deallocChunk );
        assert( _deallocChunk->_blocksAvailable != _numBlocks );
        assert( !_emptyChunk || _emptyChunk->_blocksAvailable == _numBlocks );

        _deallocChunk->deallocate( p, _blockSize );
        if ( _deallocChunk->_blocksAvailable == _numBlocks )
        {
            assert( _emptyChunk != _deallocChunk );

            if ( _emptyChunk )
            {
                auto lastChunk = &_chunks.back();
                if ( lastChunk == _deallocChunk )
                {
                    _deallocChunk = _emptyChunk;
                }
                else if ( lastChunk != _emptyChunk )
                {
                    std::swap( *_emptyChunk, *lastChunk );
                }
                assert( lastChunk->_blocksAvailable == _numBlocks );
                lastChunk->release();

                _chunks.pop_back();
                if ( _allocChunk == lastChunk || _allocChunk->isFull() )
                    _allocChunk = _deallocChunk;
            }
            _emptyChunk = _deallocChunk;
        }
        assert( !_emptyChunk || _emptyChunk->_blocksAvailable == _numBlocks );
    }

    bool FixedAllocator::freeEmptyChunk()
    {
        assert( _emptyChunk == nullptr || _emptyChunk->_blocksAvailable == _numBlocks );
        if ( _emptyChunk == nullptr )
            return false;

        assert( !_chunks.empty() );
        assert( emptyChunks() == 1 );

        auto last = &_chunks.back();
        if ( last != _emptyChunk )             // this is basically what the erase-remove idiom does
            std::swap( *_emptyChunk, *last );  // it swaps elements to be removed to the end
                                               // and then erases them all at once
        // without the need to move any elements of the vector
        assert( last->_blocksAvailable == _numBlocks );
        last->release();
        _chunks.pop_back();

        if ( _chunks.empty() )
        {
            _allocChunk = nullptr;
            _deallocChunk = nullptr;
        }
        else
        {
            if ( _deallocChunk == _emptyChunk )
            {
                _deallocChunk = &_chunks.front();
                assert( _deallocChunk->_blocksAvailable < _numBlocks );
            }
            if ( _allocChunk == _emptyChunk )
            {
                _allocChunk = &_chunks.back();
                assert( _allocChunk->_blocksAvailable < _numBlocks );
            }
        }

        _emptyChunk = nullptr;
        assert( emptyChunks() == 0 );
        return true;
    }

    bool FixedAllocator::tryToFreeUpSomeMemory()
    {
        if(_chunks.empty())
            assert(!_allocChunk && !_deallocChunk);

        if(_chunks.size() == _chunks.capacity())
            return false;

        _chunks.resize(_chunks.size());
        
        return true;
    }

    SizeType FixedAllocator::emptyChunks( bool fast ) const
    {
        if ( fast )
            return static_cast<SizeType>( _emptyChunk != nullptr );

        SizeType count = 0;
        for ( auto const& chunk : _chunks )
            if ( chunk._blocksAvailable == _numBlocks )
                ++count;

        return count;
    }

    bool FixedAllocator::isCorrupt() const
    {
        const bool isEmpty = _chunks.empty();
        auto first = std::begin( _chunks );
        auto last = std::end( _chunks );

        auto numEmpty = emptyChunks();
        if ( isEmpty )
        {
            if ( first != last )
            {
                assert( false );
                return true;
            }
            if ( 0 < numEmpty )
            {
                assert( false );
                return true;
            }
            if ( _deallocChunk != nullptr || _allocChunk != nullptr || _emptyChunk != nullptr )
            {
                assert( false );
                return true;
            }
        }
        else
        {
            if ( first >= last )  // means it's empty
            {
                assert( false );
                return true;
            }
            if ( !inRange( _chunks, _allocChunk ) || !inRange( _chunks, _deallocChunk ) )
            {
                assert( false );
                return true;
            }

            if ( numEmpty == 0 && _emptyChunk != nullptr )
            {
                assert( false );
                return true;
            }
            else if ( ( numEmpty == 1 && _emptyChunk == nullptr ) )
            {
                assert( false );
                return true;
            }
            else if ( !inRange( _chunks, _emptyChunk ) )
            {
                assert( false );
                return true;
            }
            else if ( _emptyChunk->_blocksAvailable != _numBlocks )
            {
                assert( false );
                return true;
            }
            else if ( numEmpty != 0 && numEmpty != 1 )
            {
                assert( false );
                return true;
            }

            for ( auto const& chunk : _chunks )
                if ( chunk.isCorrupted( _blockSize, _numBlocks, /*checkIndexes*/ true ) )
                    return true;
        }

        return false;
    }

    const FixedAllocator::ChunkType* FixedAllocator::hasBlock( void* p ) const
    {
        LOG( " %p ", p );
        const auto length = _numBlocks * _blockSize;

        for ( auto const& chunk : _chunks )
            if ( chunk.hasBlock( p, length ) )
                return &chunk;
        return nullptr;
    }
}  // namespace allocators
