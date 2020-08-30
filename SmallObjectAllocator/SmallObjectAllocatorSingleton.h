#pragma once
#include "PolicyClasses.h"
#include "SmallObjectAllocator.h"

namespace allocators
{
    template <
        template <class, class> class ThreadingPolicy = DEFAULT_THREADING_MODEL,
        SizeType chunkSize = DEFAULT_CHUNK_SIZE,
        SizeType maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE,
        SizeType objectAlignSize = DEFAULT_ALIGN_SIZE,
        class MutexPolicy = DEFAULT_MUTEX>
    class SmallObjectAllocatorSingleton : public SmallObjectAllocator
    {
    public:
        using MyAllocator = SmallObjectAllocatorSingleton<
            ThreadingPolicy,
            chunkSize,
            maxSmallObjectSize,
            objectAlignSize,
            MutexPolicy>;

        using MyThreadingPolicy = ThreadingPolicy<MyAllocator, MutexPolicy>;

        using MyLock = typename MyThreadingPolicy::Lock;

    public:
        inline static SmallObjectAllocatorSingleton& instance()
        {
            static MyAllocator smallObjAllocator;

            return smallObjAllocator;
        }

        inline SmallObjectAllocatorSingleton()
            : SmallObjectAllocator( chunkSize, maxSmallObjectSize, objectAlignSize )
        {
        }

        ~SmallObjectAllocatorSingleton() = default;

        SmallObjectAllocatorSingleton(SmallObjectAllocatorSingleton const& ) = delete;
        SmallObjectAllocatorSingleton& operator=(SmallObjectAllocatorSingleton const& ) = delete;

    public:
        static void removeExtraMemory()
        {
            MyLock lock;
            (void)lock;
            instance().tryToFreeUpSomeMemory();
        }

        static bool isCorrupt()
        {
            MyLock lock;
            (void)lock;
            return instance().corrupt();
        }
    };
}  // namespace allocators
