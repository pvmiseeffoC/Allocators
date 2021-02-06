#pragma once
#include <mutex>

namespace allocators
{
    using DEFAULT_MUTEX = std::mutex;
    using DEFAULT_RECURSIVE_MUTEX = std::recursive_mutex;

    // Threading Policies
    template <class Owner, class MutexPolicy = DEFAULT_MUTEX>
    class SingleThreaded
    {
    public:
        struct EmptyLock
        {
            EmptyLock() = default;

            void lock() const {}
            void unlock() const {}
        };
        using Lock = EmptyLock;
        using MyOwner = Owner;
    };

    template <class Owner, class MutexPolicy = DEFAULT_MUTEX>
    class MultiThreaded
    {
    public:
        struct Lock
        {
            MutexPolicy _mx;

        public:
            void lock() { _mx.lock(); }
            void unlock() { _mx.unlock(); }
        };
    };

    template <class T, class U>
    using DEFAULT_THREADING_MODEL = ::allocators::SingleThreaded<T, U>;

    template <class T, class U>
    using SINGLE_THREADING_MODEL = ::allocators::SingleThreaded<T, U>;
    template <class T, class U>
    using MULTI_THREADING_MODEL = ::allocators::MultiThreaded<T, U>;

#define DEFAULT_CHUNK_SIZE 4096
#define MAX_SMALL_OBJECT_SIZE 256
#define DEFAULT_ALIGN_SIZE 4

}  // namespace allocators
