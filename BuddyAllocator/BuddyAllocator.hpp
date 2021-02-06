#pragma once

#include "BuddyAllocator.h"
#include <cmath>
#include <array>
#include <algorithm>

namespace buddylib
{
    inline std::size_t pow2( std::size_t t ) { return 0x1 << t; }

    inline std::size_t nextPow2( std::size_t t )  // from bittwiddling hacks
    {
        --t;
        t |= t >> 1;
        t |= t >> 2;
        t |= t >> 4;
        t |= t >> 8;
        t |= t >> 16;
        ++t;
        return t;
    }

    inline bool isPow2( std::size_t t ) { return ( t & ( t - 1 ) ) == 0; }

    inline std::size_t leftChildIdx( std::size_t idx ) { return ( idx << 1 ) + 1; }

    inline std::size_t rightChildIdx( std::size_t idx ) { return ( idx << 1 ) + 2; }

    inline std::size_t parentIdx( std::size_t childIdx ) { return ( childIdx - 1 ) >> 1; }
}  // namespace buddylib

template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline bool BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::splitToLevel( std::size_t level )
{
    auto beginSplitLvl = level;
    while ( beginSplitLvl > 0 && _freeList.at( --beginSplitLvl ).empty() )
        ;

    if ( _freeList[beginSplitLvl].empty() )
        return false;

    for ( std::size_t currentLvl = beginSplitLvl; currentLvl < level; ++currentLvl )
    {
        auto& freeNodes = _freeList[currentLvl];
        auto& nextLvlNodes = _freeList[currentLvl + 1];
        auto nodeToSplitIdx = freeNodes.front().idx;
        freeNodes.pop_front();
        std::array<std::size_t, 2> children {
            buddylib::leftChildIdx( nodeToSplitIdx ), buddylib::rightChildIdx( nodeToSplitIdx ) };
        auto insertPos =
            std::lower_bound( std::begin( nextLvlNodes ), std::end( nextLvlNodes ), children[0] );
        nextLvlNodes.insert( insertPos, std::begin( children ), std::end( children ) );
    }
    return true;
}

template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline void BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::freeNode(
    std::size_t nodeIdx,
    std::size_t level )
{
    Lock guard{ _lock };
    auto insertPos =
        std::lower_bound( std::begin( _freeList[level] ), std::end( _freeList[level] ), nodeIdx );
    _freeList[level].insert( insertPos, Node( nodeIdx ) );

    mergeFreeNodes( buddylib::parentIdx( nodeIdx ), level );
}

template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline void BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::mergeFreeNodes(
    std::size_t parent,
    std::size_t level )
{
    for ( ;; )
    {
        auto left = buddylib::leftChildIdx( parent );
        auto right = buddylib::rightChildIdx( parent );
        auto pos =
            std::lower_bound( std::begin( _freeList[level] ), std::end( _freeList[level] ), left );

        if ( *pos == left && ++pos != std::end( _freeList[level] ) && *pos == right )
        {
            _freeList[level].erase(std::prev(pos), std::next(pos));
            auto insertPos =
                std::lower_bound(std::begin(_freeList[level - 1]), std::end(_freeList[level - 1]), parent);
            _freeList[level - 1].insert(insertPos, parent);
        }
        else
            return;

        if(level - 1 == 0)
            return;

        parent = buddylib::parentIdx(parent);
        --level;
    }
}

template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline std::size_t BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::fixSize(
    std::size_t allocSize ) const
{
    if ( !buddylib::isPow2( allocSize ) )
        allocSize = buddylib::nextPow2( allocSize );
    return std::max( allocSize, buddylib::pow2( MIN_SIZE ) );
}

template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::BuddyAllocator()
    : _memoryPtr( new uint8_t[buddylib::pow2( MAX_SIZE )] )
{
    _freeList.resize( MAX_SIZE - MIN_SIZE + 1 );
    _freeList[0].emplace_back( 0 );
}

template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline void* BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::allocate( std::size_t size )
{
    size = fixSize( size );

    auto level = MAX_SIZE - static_cast<std::size_t>( std::log2( size ) + 0.1 );
    Lock guard{ _lock };
    if ( _freeList.at( level ).empty() && !splitToLevel( level ) )
        return nullptr;

    auto node = _freeList.at( level ).front();
    auto levelBeginIdx = buddylib::pow2( level );
    auto levelSize = buddylib::pow2( MAX_SIZE ) / levelBeginIdx;
    auto offset = levelSize * ( node.idx - ( levelBeginIdx - 1 ) );
    _freeList.at( level ).pop_front();
    return _memoryPtr.get() + offset;
}

template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline void BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::deallocate(
    void* ptr,
    std::size_t size )
{
    size = fixSize( size );
    auto level = MAX_SIZE - static_cast<std::size_t>( std::log2( size ) + 0.1 );
    auto levelBeginIdx = buddylib::pow2( level );
    auto levelSize = buddylib::pow2( MAX_SIZE ) / levelBeginIdx;
    auto offset = ( static_cast<uint8_t*>(ptr) - _memoryPtr.get() );
    auto nodeIdx = offset / levelSize + levelBeginIdx - 1;

    freeNode( nodeIdx, level );
}

template<std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline std::vector<std::size_t> BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::freeNodesPerLevel() const
{
    std::vector<std::size_t> result;
    for(auto const& freeNodes : _freeList)
        result.emplace_back(freeNodes.size());
    return result;
}

template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
inline BuddyAllocator<MAX_SIZE, MIN_SIZE, THREADING_POLICY>::~BuddyAllocator()
{
}
