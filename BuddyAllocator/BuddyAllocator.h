#pragma once
#include <vector>
#include <memory>
#include <list>

// size is in power of 2s, aka size = 10 is 2^10 bytes
template <std::size_t MAX_SIZE, std::size_t MIN_SIZE, class THREADING_POLICY>
class BuddyAllocator
{
private:
    struct Node
    {
        std::size_t idx;
        Node( std::size_t i ) : idx( i ) {}

        friend bool operator<(Node const& a, Node const& b){return a.idx < b.idx;}
        friend bool operator==(Node const& a, Node const& b){return a.idx == b.idx;}
    };

    using LockType = typename THREADING_POLICY::LockType;
    using Lock = typename THREADING_POLICY::Lock;

private:
    std::vector<std::list<Node>> _freeList;

    std::unique_ptr<uint8_t[]> _memoryPtr;

    LockType _lock;
private:
    bool splitToLevel( std::size_t level );
    void freeNode(std::size_t nodeIdx, std::size_t level);
    void mergeFreeNodes(std::size_t parent, std::size_t level);

    std::size_t fixSize(std::size_t allocSize) const;
public:
    BuddyAllocator();

    void* allocate( std::size_t size );
    void deallocate( void* ptr, std::size_t size );

    std::vector<std::size_t> freeNodesPerLevel() const;

    ~BuddyAllocator();
};

#include "BuddyAllocator.hpp"