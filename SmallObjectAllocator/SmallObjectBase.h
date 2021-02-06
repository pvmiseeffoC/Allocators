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
        static MyLock& getLock()
        {
            static MyLock lock;
            return lock;
        }

        static void* operator new( SizeType size ) noexcept
        {
            auto& lock = getLock();
            std::lock_guard<decltype(lock)> guard{ lock };
            return SmallObjAllocSingleton::instance().allocate( size );
        }

        //placement new
        inline static void* operator new( std::size_t size, void* place )
        {
            return ::operator new( size, place );
        }

        static void operator delete( void* ptr, SizeType size ) noexcept
        {
            auto& lock = getLock();
            std::lock_guard<decltype(lock)> guard{ lock };
            SmallObjAllocSingleton::instance().deallocate( ptr, size );
        }

        /// Placement delete
        inline static void operator delete( void* p, void* place )
        {
            auto& lock = getLock();
            std::lock_guard<decltype(lock)> guard{ lock };
            ::operator delete( p, place );
        }

        static void* operator new[]( std::size_t size ) noexcept
        {
            auto& lock = getLock();
            std::lock_guard<decltype(lock)> guard{ lock };
            return SmallObjAllocSingleton::instance().allocate( size );
        }

        static void* operator new[]( std::size_t size, const std::nothrow_t& ) noexcept
        {
            auto& lock = getLock();
            std::lock_guard<decltype(lock)> guard{ lock };
            return SmallObjAllocSingleton::instance().allocate( size );
        }
        /// Array-object delete.
        static void operator delete[]( void* p, std::size_t size ) noexcept
        {
            auto& lock = getLock();
            std::lock_guard<decltype(lock)> guard{ lock };
            SmallObjAllocSingleton::instance().deallocate( p, size );
        }

    protected:
        inline SmallObjectBase() = default;
        inline SmallObjectBase( SmallObjectBase const& ) = default;
        inline SmallObjectBase& operator=( SmallObjectBase const& ) { return *this; }
        inline ~SmallObjectBase() = default;
    };

}  // namespace allocators
