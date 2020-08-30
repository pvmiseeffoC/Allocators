// Tests.cpp : Defines the entry point for the application.
//
#include <benchmark/benchmark.h>
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
    MySmallObject(int a, int b, int c) : _a(a), _b(b), _c(c) {}
};

class NoSmallObj
{
private:
    int _a;
    int _b;
    int _c;

public:
    NoSmallObj() = default;
    NoSmallObj(int a, int b, int c) : _a(a), _b(b), _c(c) {}
};

static void BM_MY_SMALL_OBJECT_ALLOCATION(benchmark::State& state)
{
    for (auto _ : state)
        std::make_unique<MySmallObject>(100, 10, 10);
}
// Register the function as a benchmark
BENCHMARK(BM_MY_SMALL_OBJECT_ALLOCATION);

// Define another benchmark
static void BM_NO_SMALL_OBJECT_ALLOCATION(benchmark::State& state)
{
    for (auto _ : state)
        std::make_unique<NoSmallObj>(100, 10, 10);
}
BENCHMARK(BM_NO_SMALL_OBJECT_ALLOCATION);

BENCHMARK_MAIN();
