// Tests.cpp : Defines the entry point for the application.
//
#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <memory>
#include "SmallObjectAllocator/SmallObject.h"

using namespace allocators;
class MySmallObject
    : public SmallObject<
          DEFAULT_THREADING_MODEL,
          DEFAULT_CHUNK_SIZE,
          MAX_SMALL_OBJECT_SIZE,
          DEFAULT_ALIGN_SIZE,
          DEFAULT_MUTEX>
{
    int _a;
    int _b;
    int _c;

public:
    MySmallObject() = default;
    MySmallObject( int a, int b, int c ) : _a( a ), _b( b ), _c( c ) {}
};

class NoSmallObj
{
private:
    int _a;
    int _b;
    int _c;

public:
    NoSmallObj() = default;
    NoSmallObj( int a, int b, int c ) : _a( a ), _b( b ), _c( c ) {}
};

template <class Tp>
inline void DoNotOptimize( Tp const& value )
{
#if defined( __clang__ )
    asm volatile( "" : "+r,m"( value ) : : "memory" );
#else
    asm volatile( "" : "+m,r"( value ) : : "memory" );
#endif
}

class PerformanceTest
{
public:
    template <typename Func>
    static auto run( Func&& func, size_t const num_executes )
    {
        auto start = std::chrono::system_clock::now();

        for ( auto i = 0; i < num_executes; ++i )
        {
            func();
        }

        auto end = std::chrono::system_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count();

        auto average_duration = duration / num_executes;

        return average_duration;
    }
};

TEST( ComparePerformance, ComparePerformance )
{
    auto normal = PerformanceTest::run(
        []() { DoNotOptimize( std::make_unique<NoSmallObj>( 10, 10, 10 ) ); }, /*num_execs*/ 100 );

    auto smallObj = PerformanceTest::run(
        []() { DoNotOptimize( std::make_unique<MySmallObject>( 10, 10, 10 ) ); },
        /*num_execs*/ 100 );

    EXPECT_GE( normal, smallObj );
}
