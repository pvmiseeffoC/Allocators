#include "gtest/gtest.h"
#include <chrono>
#include <thread>
#include <vector>
#include <future>

template <class Func>
auto bench( size_t iterations, Func&& f )
{
    std::chrono::nanoseconds duration { 0l };
    for ( auto i = 0; i < iterations; ++i )
    {
        const auto start = std::chrono::high_resolution_clock::now();
        f();
        const auto end = std::chrono::high_resolution_clock::now();
        duration += std::chrono::duration_cast<std::chrono::nanoseconds>( end - start );
    };
    const auto avg_duration = ( duration / iterations ).count();
    return avg_duration;
}

template <class Func>
auto bench_multithreaded(
    size_t iterations,
    Func&& f,
    size_t threads = std::thread::hardware_concurrency() )
{
    std::vector<std::future<unsigned long long>> futures;
    for ( auto i = 0; i < threads; ++i )
        futures.push_back(
            std::async( std::launch::async, [&]() { return bench( iterations / threads, f ); } ) );

    unsigned long long duration { 0 };
    for ( auto i = 0; i < threads; ++i )
        duration += futures[i].get();
    return duration / threads;
}

#define REGISTER_BM_TEST( name, func, size, base, ... )                                 \
    TEST( Benchmark, name##size )                                                       \
    {                                                                                   \
        auto time = bench( /*iterations*/ 10000, func<base<size>, __VA_ARGS__> );       \
        std::cerr << "Test: " << #name #size << " finished for: " << time << std::endl; \
    }

#define REGISTER_BM_MULTI_TEST( name, func, size, base, ... )                                      \
    TEST( BenchmarkMulti, name##size )                                                             \
    {                                                                                              \
        auto time = bench_multithreaded( /*iterations*/ 10000, func<base<size>, __VA_ARGS__>, 2 ); \
        std::cerr << "Test multithread alloc with 2 threads: " << #name #size                      \
                  << " finished for: " << time << std::endl;                                       \
        time = bench_multithreaded( /*iterations*/ 10000, func<base<size>, __VA_ARGS__>, 4 );      \
        std::cerr << "Test multithread alloc with 4 threads: " << #name #size                      \
                  << " finished for: " << time << std::endl;                                       \
        time = bench_multithreaded( /*iterations*/ 10000, func<base<size>, __VA_ARGS__>, 6 );      \
        std::cerr << "Test multithread alloc with 6 threads: " << #name #size                      \
                  << " finished for: " << time << std::endl;                                       \
        time = bench_multithreaded( /*iterations*/ 10000, func<base<size>, __VA_ARGS__>, 8 );      \
        std::cerr << "Test multithread alloc with 9 threads: " << #name #size                      \
                  << " finished for: " << time << std::endl;                                       \
    }
