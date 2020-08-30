#pragma once
#include <mutex>

namespace allocators
{
    template <class Owner, class MutexPolicy = DEFAULT_MUTEX>
    class SingleThreaded
    {
    public:
        struct EmptyLock
        {
            EmptyLock() = default;
            explicit Lock( SingleThreaded& ) {}
            explicit Lock( SingleThreaded* ) {}
        };
        using Lock = EmptyLock;
        using MyOwner = Owner;
    };

    using DEFAULT_MUTEX = std::mutex;
    using DEFAULT_RECURSIVE_MUTEX = std::recursive_mutex;

    template <class T, class U>
    using DEFAULT_THREADING_MODEL = ::allocators::SingleThreaded<T, U>;

#define DEFAULT_CHUNK_SIZE 4096
#define MAX_SMALL_OBJECT_SIZE 256
#define DEFAULT_ALIGN_SIZE 4

}  // namespace allocators
