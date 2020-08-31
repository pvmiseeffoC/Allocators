// Tests.cpp : Defines the entry point for the application.
//
#include <benchmark/benchmark.h>
#include <chrono>
#include <iostream>
#include <memory>
#include "SmallObjectAllocator/SmallObject.h"

using namespace allocators;

namespace benchmark_tests
{
    template <typename Base, std::size_t N>
    class TestObject : public Base
    {
        char data[N];

    public:
        TestObject() = default;
    };

    class EmptyBase
    {
    };

    using SingleThreadSmallObjBase = SmallObject<
        SingleThreaded,
        DEFAULT_CHUNK_SIZE,
        MAX_SMALL_OBJECT_SIZE,
        DEFAULT_ALIGN_SIZE,
        DEFAULT_MUTEX>;

    template<std::size_t N>
    using MySmallObj = TestObject<SingleThreadSmallObjBase, N>;

    template<std::size_t N>
    using EmptyBaseTestObj = TestObject<EmptyBase, N>;

    template <typename T, std::size_t size>
    static void create_array_and_delete()
    {
        auto arr = new T[size];
        benchmark::DoNotOptimize( arr );
        delete[] arr;
    }

    template <typename T, std::size_t size, std::size_t numArrays>
    static void create_multiple_arrays_and_delete()
    {
        T** arr = new T*[numArrays];
        benchmark::DoNotOptimize(arr);

        for(auto i =0; i < numArrays; ++i)
            arr[i] = new T[size];

        for(auto i= 0; i < numArrays; ++i)
            delete[] arr[i];

        delete[] arr;
    }

    template<class Base>
    struct Node : public Base
    {
        Node* next;
        Node()
        {
        }
    };

    template <typename Base, std::size_t size>
    static void create_linked_list()
    {
        using MyNode = Node<Base>;

        MyNode* start = new MyNode();
        benchmark::DoNotOptimize(start);
        MyNode* current = start;
        std::vector<MyNode*> ptrs;
        ptrs.emplace_back(start);
        
        for(auto i = 0; i < size; ++i)
        {
            auto nextNode = new MyNode();
            current->next = nextNode;
            current = nextNode;
            ptrs.emplace_back(current);
        }

        current = start;
        for(auto ptr : ptrs)
        {
            delete ptr;
        }
    }

    const std::size_t smallAllocSize = 100;
    const std::size_t mediumAllocSize = 10000;
    const std::size_t hugeAllocSize = 1000000;

#define REGISTER_BM_TEST( name, func, size, base, ... )          \
    static void BM_##name##size( benchmark::State& state ) \
    {                                                \
        for ( auto _ : state )                       \
        {                                            \
            func<base<size>,__VA_ARGS__>();                     \
        };                                           \
    }                                                \
    BENCHMARK( BM_##name##size )

    namespace { // small alloc tests
        REGISTER_BM_TEST( SMALL_OBJ_SMALL_ALLOC, create_array_and_delete, 2, MySmallObj, smallAllocSize );
        REGISTER_BM_TEST( EMPTYBASE_OBJ_SMALL_ALLOC, create_array_and_delete, 2, EmptyBaseTestObj, smallAllocSize );
        
        REGISTER_BM_TEST( SMALL_OBJ_SMALL_ALLOC, create_array_and_delete, 3, MySmallObj, smallAllocSize );
        REGISTER_BM_TEST( EMPTYBASE_OBJ_SMALL_ALLOC, create_array_and_delete, 3, EmptyBaseTestObj, smallAllocSize );
        
        REGISTER_BM_TEST( SMALL_OBJ_SMALL_ALLOC, create_array_and_delete, 4, MySmallObj, smallAllocSize );
        REGISTER_BM_TEST( EMPTYBASE_OBJ_SMALL_ALLOC, create_array_and_delete, 4, EmptyBaseTestObj, smallAllocSize );
        
        REGISTER_BM_TEST( SMALL_OBJ_SMALL_ALLOC, create_array_and_delete, 8, MySmallObj, smallAllocSize );
        REGISTER_BM_TEST( EMPTYBASE_OBJ_SMALL_ALLOC, create_array_and_delete, 8, EmptyBaseTestObj, smallAllocSize );
        
        REGISTER_BM_TEST( SMALL_OBJ_SMALL_ALLOC, create_array_and_delete, 9, MySmallObj, smallAllocSize );
        REGISTER_BM_TEST( EMPTYBASE_OBJ_SMALL_ALLOC, create_array_and_delete, 9, EmptyBaseTestObj, smallAllocSize );
        
        REGISTER_BM_TEST( SMALL_OBJ_SMALL_ALLOC, create_array_and_delete, 17, MySmallObj, smallAllocSize );
        REGISTER_BM_TEST( EMPTYBASE_OBJ_SMALL_ALLOC, create_array_and_delete, 17, EmptyBaseTestObj, smallAllocSize );
    }

    namespace // medium alloc tests
    {

        REGISTER_BM_TEST(SMALL_OBJ_MEDIUM_ALLOC, create_array_and_delete, 2, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_MEDIUM_ALLOC, create_array_and_delete, 2, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_MEDIUM_ALLOC, create_array_and_delete, 3, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_MEDIUM_ALLOC, create_array_and_delete, 3, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_MEDIUM_ALLOC, create_array_and_delete, 4, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_MEDIUM_ALLOC, create_array_and_delete, 4, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_MEDIUM_ALLOC, create_array_and_delete, 8, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_MEDIUM_ALLOC, create_array_and_delete, 8, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_MEDIUM_ALLOC, create_array_and_delete, 9, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_MEDIUM_ALLOC, create_array_and_delete, 9, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_MEDIUM_ALLOC, create_array_and_delete, 17, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_MEDIUM_ALLOC, create_array_and_delete, 17, EmptyBaseTestObj, mediumAllocSize);
    }

    namespace // huge alloc tests
    {
        REGISTER_BM_TEST(SMALL_OBJ_HUGE_ALLOC, create_array_and_delete, 2, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_HUGE_ALLOC, create_array_and_delete, 2, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_HUGE_ALLOC, create_array_and_delete, 3, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_HUGE_ALLOC, create_array_and_delete, 3, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_HUGE_ALLOC, create_array_and_delete, 4, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_HUGE_ALLOC, create_array_and_delete, 4, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_HUGE_ALLOC, create_array_and_delete, 8, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_HUGE_ALLOC, create_array_and_delete, 8, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_HUGE_ALLOC, create_array_and_delete, 9, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_HUGE_ALLOC, create_array_and_delete, 9, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_OBJ_HUGE_ALLOC, create_array_and_delete, 17, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_OBJ_HUGE_ALLOC, create_array_and_delete, 17, EmptyBaseTestObj, hugeAllocSize);
    }

    namespace // linked list small alloc tests
    {
        REGISTER_BM_TEST(SMALL_LINKED_LIST_SMALL_ALLOC, create_linked_list, 2, MySmallObj, smallAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_SMALL_ALLOC, create_linked_list, 2, EmptyBaseTestObj, smallAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_SMALL_ALLOC, create_linked_list, 3, MySmallObj, smallAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_SMALL_ALLOC, create_linked_list, 3, EmptyBaseTestObj, smallAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_SMALL_ALLOC, create_linked_list, 4, MySmallObj, smallAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_SMALL_ALLOC, create_linked_list, 4, EmptyBaseTestObj, smallAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_SMALL_ALLOC, create_linked_list, 8, MySmallObj, smallAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_SMALL_ALLOC, create_linked_list, 8, EmptyBaseTestObj, smallAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_SMALL_ALLOC, create_linked_list, 9, MySmallObj, smallAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_SMALL_ALLOC, create_linked_list, 9, EmptyBaseTestObj, smallAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_SMALL_ALLOC, create_linked_list, 17, MySmallObj, smallAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_SMALL_ALLOC, create_linked_list, 17, EmptyBaseTestObj, smallAllocSize);
    }

    namespace // linked list medium alloc tests
    {
        REGISTER_BM_TEST(SMALL_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 2, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 2, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 3, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 3, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 4, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 4, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 8, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 8, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 9, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 9, EmptyBaseTestObj, mediumAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 17, MySmallObj, mediumAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC, create_linked_list, 17, EmptyBaseTestObj, mediumAllocSize);
    }

    namespace // linked list huge alloc tests
    {
        REGISTER_BM_TEST(SMALL_LINKED_LIST_HUGE_ALLOC, create_linked_list, 2, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_HUGE_ALLOC, create_linked_list, 2, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_HUGE_ALLOC, create_linked_list, 3, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_HUGE_ALLOC, create_linked_list, 3, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_HUGE_ALLOC, create_linked_list, 4, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_HUGE_ALLOC, create_linked_list, 4, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_HUGE_ALLOC, create_linked_list, 8, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_HUGE_ALLOC, create_linked_list, 8, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_HUGE_ALLOC, create_linked_list, 9, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_HUGE_ALLOC, create_linked_list, 9, EmptyBaseTestObj, hugeAllocSize);

        REGISTER_BM_TEST(SMALL_LINKED_LIST_HUGE_ALLOC, create_linked_list, 17, MySmallObj, hugeAllocSize);
        REGISTER_BM_TEST(EMPTYBASE_LINKED_LIST_HUGE_ALLOC, create_linked_list, 17, EmptyBaseTestObj, hugeAllocSize);
    }
}  // namespace benchmark_tests

    //// Run the benchmark
BENCHMARK_MAIN();
