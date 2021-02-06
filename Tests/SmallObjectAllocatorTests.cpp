#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include <memory>
#include "Benchmark.h"
#include "SmallObjectAllocator/SmallObject.h"
#include "SmallObjectAllocator/SmallValueObject.h"

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

    using MultiThreadSmallObjBase = SmallObject<
        MultiThreaded,
        DEFAULT_CHUNK_SIZE,
        MAX_SMALL_OBJECT_SIZE,
        DEFAULT_ALIGN_SIZE,
        DEFAULT_MUTEX>;

    template <std::size_t N>
    using MySmallObj = TestObject<SingleThreadSmallObjBase, N>;

    template <std::size_t N>
    using MySmallObjMulti = TestObject<MultiThreadSmallObjBase, N>;

    template <std::size_t N>
    using EmptyBaseTestObj = TestObject<EmptyBase, N>;

    template <class Base>
    struct Node : public Base
    {
        Node* next = nullptr;
        Node() {}
    };

    template <typename Base, std::size_t size>
    static void create_linked_list()
    {
        using MyNode = Node<Base>;

        MyNode* start = new MyNode();
        MyNode* current = start;

        for (auto i = 0; i < size; ++i)
        {
            auto nextNode = new MyNode();
            current->next = nextNode;
            current = nextNode;
        }

        current = start;
        while (current)
        {
            auto prev = current;
            current = prev->next;
            delete prev;
        }
    }

    const std::size_t smallAllocSize = 100;
    const std::size_t mediumAllocSize = 10000;
    const std::size_t hugeAllocSize = 100000;


    namespace  // linked list small alloc tests
    {
        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_SMALL_ALLOC,
            create_linked_list,
            2,
            MySmallObj,
            smallAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_SMALL_ALLOC,
            create_linked_list,
            2,
            EmptyBaseTestObj,
            smallAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_SMALL_ALLOC,
            create_linked_list,
            3,
            MySmallObj,
            smallAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_SMALL_ALLOC,
            create_linked_list,
            3,
            EmptyBaseTestObj,
            smallAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_SMALL_ALLOC,
            create_linked_list,
            5,
            MySmallObj,
            smallAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_SMALL_ALLOC,
            create_linked_list,
            5,
            EmptyBaseTestObj,
            smallAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_SMALL_ALLOC,
            create_linked_list,
            17,
            MySmallObj,
            smallAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_SMALL_ALLOC,
            create_linked_list,
            17,
            EmptyBaseTestObj,
            smallAllocSize);
    }  // namespace

    namespace  // linked list medium alloc tests
    {
        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            2,
            MySmallObj,
            mediumAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            2,
            EmptyBaseTestObj,
            mediumAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            3,
            MySmallObj,
            mediumAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            3,
            EmptyBaseTestObj,
            mediumAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            5,
            MySmallObj,
            mediumAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            5,
            EmptyBaseTestObj,
            mediumAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            17,
            MySmallObj,
            mediumAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            17,
            EmptyBaseTestObj,
            mediumAllocSize);
    }  // namespace

    namespace  // linked list huge alloc tests
    {
        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_HUGE_ALLOC,
            create_linked_list,
            2,
            MySmallObj,
            hugeAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_HUGE_ALLOC,
            create_linked_list,
            2,
            EmptyBaseTestObj,
            hugeAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_HUGE_ALLOC,
            create_linked_list,
            3,
            MySmallObj,
            hugeAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_HUGE_ALLOC,
            create_linked_list,
            3,
            EmptyBaseTestObj,
            hugeAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_HUGE_ALLOC,
            create_linked_list,
            5,
            MySmallObj,
            hugeAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_HUGE_ALLOC,
            create_linked_list,
            5,
            EmptyBaseTestObj,
            hugeAllocSize);

        REGISTER_BM_TEST(
            SMALL_LINKED_LIST_HUGE_ALLOC,
            create_linked_list,
            17,
            MySmallObj,
            hugeAllocSize);
        REGISTER_BM_TEST(
            EMPTYBASE_LINKED_LIST_HUGE_ALLOC,
            create_linked_list,
            17,
            EmptyBaseTestObj,
            hugeAllocSize);
    }  // namespace

    namespace  // linked list medium alloc tests
    {
        REGISTER_BM_MULTI_TEST(
            SMALL_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            2,
            MySmallObjMulti,
            mediumAllocSize);
        REGISTER_BM_MULTI_TEST(
            EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            2,
            EmptyBaseTestObj,
            mediumAllocSize);

        REGISTER_BM_MULTI_TEST(
            SMALL_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            3,
            MySmallObjMulti,
            mediumAllocSize);
        REGISTER_BM_MULTI_TEST(
            EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            3,
            EmptyBaseTestObj,
            mediumAllocSize);

        REGISTER_BM_MULTI_TEST(
            SMALL_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            5,
            MySmallObjMulti,
            mediumAllocSize);
        REGISTER_BM_MULTI_TEST(
            EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            5,
            EmptyBaseTestObj,
            mediumAllocSize);

        REGISTER_BM_MULTI_TEST(
            SMALL_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            17,
            MySmallObjMulti,
            mediumAllocSize);
        REGISTER_BM_MULTI_TEST(
            EMPTYBASE_LINKED_LIST_MEDIUM_ALLOC,
            create_linked_list,
            17,
            EmptyBaseTestObj,
            mediumAllocSize);
    }  // namespace
}  // namespace benchmark_tests