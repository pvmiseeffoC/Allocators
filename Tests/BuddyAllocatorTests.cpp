#include "gtest/gtest.h"

#include "Benchmark.h"
#include "BuddyAllocator/BuddyAllocator.h"
#include "BuddyAllocator/ThreadingPolicies.h"

using namespace buddy;

class MallocAllocator
{
public:
    void* allocate( size_t size ) { return malloc( size ); }
    void deallocate( void* ptr, size_t size ) { free( ptr ); }
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
    BuddyAllocator<20, 8, SingleThreaded> buddy;
    auto buddyTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                buddy,
                /*allocations*/ 1 << 11,
                /*allocSize*/ 270 );
        } );

    MallocAllocator mallocAlloc;
    auto mallocTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                mallocAlloc,
                /*allocations*/ 1 << 11,
                /*allocSize*/ 270 );
        });

    EXPECT_LE( buddyTime, mallocTime);
}

TEST( BuddyAllocatorTests, TestPerformanceFor1024byteObjects )
{
    BuddyAllocator<24, 10, SingleThreaded> buddy;
    auto buddyTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                buddy,
                /*allocations*/ 1 << 14,
                /*allocSize*/ 1024 );
        } );

    MallocAllocator mallocAlloc;
    auto mallocTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                mallocAlloc,
                /*allocations*/ 1 << 14,
                /*allocSize*/ 1024 );
        });

    EXPECT_LE( buddyTime, mallocTime);
}

TEST( BuddyAllocatorTests, TestPerformanceFor69byteObjects )
{
    BuddyAllocator<32, 10, SingleThreaded> buddy;
    auto buddyTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                buddy,
                /*allocations*/ 1 << 22,
                /*allocSize*/ 69 );
        } );

    MallocAllocator mallocAlloc;
    auto mallocTime = bench(
        /*iterations*/ 1000,
        [&]() {
            linearAllocDealloc(
                mallocAlloc,
                /*allocations*/ 1 << 22,
                /*allocSize*/ 69 );
        });

    EXPECT_LE( buddyTime, mallocTime);
}
