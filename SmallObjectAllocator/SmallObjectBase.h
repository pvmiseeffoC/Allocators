#pragma once
#include "SmallObjectAllocatorSingleton.h"

namespace allocators
{
    template <
        template <class, class>
        class ThreadingPolicy,
        SizeType chunkSize,
        SizeType maxSmallObjectSize,
        SizeType objectAlignSize,
        class MutexPolicy>
    class SmallObjectBase
    {
    public:
        using SmallObjAllocSingleton = SmallObjectAllocatorSingleton<
            ThreadingPolicy,
            chunkSize,
            maxSmallObjectSize,
            objectAlignSize,
            MutexPolicy>;

    private:
        using MyThreadingModel = ThreadingPolicy<SmallObjAllocSingleton, MutexPolicy>;

        using MyLock = typename SmallObjAllocSingleton::MyLock;

    public:
        static void* operator new( SizeType size ) noexcept
        {
            MyLock lock;
            (void)lock;
            return SmallObjAllocSingleton::instance().allocate( size );
        }

        //placement new
        inline static void* operator new( std::size_t size, void* place )
        {
            return ::operator new( size, place );
        }

        static void operator delete( void* ptr, SizeType size ) noexcept
        {
            MyLock lock;
            (void)lock;
            SmallObjAllocSingleton::instance().deallocate( ptr, size );
        }

        static void operator delete( void* ptr, const std::nothrow_t& ) noexcept
        {
            MyLock lock;
            (void)lock;
            SmallObjAllocSingleton::instance().deallocate( ptr );
        }

        /// Placement delete
        inline static void operator delete( void* p, void* place )
        {
            ::operator delete( p, place );
        }


        static void* operator new [](std::size_t size) noexcept
        {
            MyLock lock;
            (void)lock; // get rid of warning
            return SmallObjAllocSingleton::instance().allocate(size);
        }

            /// Non-throwing array-object new returns NULL if allocation fails.
            static void* operator new [](std::size_t size,
                const std::nothrow_t&) noexcept
        {
            MyLock lock;
            (void)lock; // get rid of warning
            return SmallObjAllocSingleton::instance().allocate(size);
        }

            /// Placement array-object new merely calls global placement new.
            inline static void* operator new [](std::size_t size, void* place)
        {
            return ::operator new(size, place);
        }

            /// Array-object delete.
            static void operator delete [](void* p, std::size_t size) noexcept
        {
            MyLock lock;
            (void)lock; // get rid of warning
            SmallObjAllocSingleton::instance().deallocate(p, size);
        }

            /** Non-throwing array-object delete is only called when nothrow
             new operator is used, and the constructor throws an exception.
             */
            static void operator delete [](void* p,
                const std::nothrow_t&) noexcept
        {
            MyLock lock;
            (void)lock; // get rid of warning
            SmallObjAllocSingleton::instance().deallocate(p);
        }

            /// Placement array-object delete merely calls global placement delete.
            inline static void operator delete [](void* p, void* place)
        {
            ::operator delete (p, place);
        }

    protected:
        inline SmallObjectBase() = default;
        inline SmallObjectBase( SmallObjectBase const& ) = default;
        inline SmallObjectBase& operator=( SmallObjectBase const& ) { return *this; }
        inline ~SmallObjectBase() = default;
    };

}  // namespace allocators
