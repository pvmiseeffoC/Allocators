#pragma once
#include <vector>
#include <memory>
#include <list>
#include <bitset>

// size is in power of 2s, aka size = 10 is 2^10 bytes
template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
class BuddyAllocator
{
private:

    using LockType = typename THREADING_POLICY::LockType;
    using ScopedLock = typename THREADING_POLICY::ScopedLock;

private:
    std::vector<std::list<std::size_t>> _freeList;
    std::bitset< (1 << (MAX_SIZE - MIN_SIZE + 1)) / 2 > _freeNodes;

    std::unique_ptr<uint8_t[]> _memoryPtr;

    LockType _lock;
private:
    bool splitToLevel( std::size_t level ) noexcept;
    void freeNode(std::size_t nodeIdx, std::size_t level) noexcept;
    void freeNode(std::size_t nodeIdx, std::size_t level, ScopedLock& l) noexcept;

    std::size_t fixSize(std::size_t allocSize) const noexcept;
public:
    BuddyAllocator();

    void* allocate( std::size_t size ) noexcept;
    void deallocate( void* ptr, std::size_t size ) noexcept;

    std::vector<std::size_t> freeNodesPerLevel() const noexcept;

    ~BuddyAllocator();
};

#include "BuddyAllocator.hpp"
