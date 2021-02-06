#include "gtest/gtest.h"

#include "Benchmark.h"
#include "BuddyAllocator/BuddyAllocator.h"
#include "BuddyAllocator/ThreadingPolicies.h"

using namespace buddy;

class NewAllocator
{
public:
    void* allocate( size_t size ) { return ::operator new( size ); }
    void deallocate( void* ptr, size_t size ) { return ::operator delete( ptr, size ); }
};

template <typename T>
bool operator==( std::vector<T> const& a, std::vector<T> const& b )
{
    bool equal = a.size() = b.size();
    if ( !equal )
        return false;

    for ( auto i = 0; i < a.size(); ++i )
        equal &= a[i] == b[i];
    return equal;
}

template <class Allocator>
void linearAllocDealloc( Allocator& alloc, std::size_t allocations, std::size_t allocSize )
{
    std::vector<void*> ptrs;
    for ( auto i = 0; i < allocations; ++i )
    {
        ptrs.emplace_back( alloc.allocate( allocSize ) );
    }

    for ( auto i = 0; i < allocations; ++i )
    {
        alloc.deallocate( ptrs[i], allocSize );
    }
}

template <class Allocator>
void linearAllocReverseOrderDealloc(
    Allocator& alloc,
    std::size_t allocations,
    std::size_t allocSize )
{
    std::vector<void*> ptrs;
    for ( auto i = 0; i < allocations; ++i )
    {
        ptrs.emplace_back( alloc.allocate( allocSize ) );
    }

    for ( auto i = allocations - 1; i < allocations; ++i )
    {
        alloc.deallocate( ptrs[i], allocSize );
    }
}

TEST( BuddyAllocatorTests, TestPerformanceFor270byteObjects )
{
    BuddyAllocator<24, 8, SingleThreaded> buddy;
    auto buddyTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                buddy,
                /*allocations*/ 1 << 12,
                /*allocSize*/ 270 );
        } );

    NewAllocator newAlloc;
    auto newTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                newAlloc,
                /*allocations*/ 1 << 12,
                /*allocSize*/ 270 );
        });

    EXPECT_LE( buddyTime, newTime );
}

TEST( BuddyAllocatorTests, TestPerformanceFor1024byteObjects )
{
    BuddyAllocator<28, 10, SingleThreaded> buddy;
    auto buddyTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                buddy,
                /*allocations*/ 1 << 16,
                /*allocSize*/ 1024 );
        } );

    NewAllocator newAlloc;
    auto newTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                newAlloc,
                /*allocations*/ 1 << 16,
                /*allocSize*/ 1024 );
        });

    EXPECT_LE( buddyTime, newTime );
}
