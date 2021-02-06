#pragma once

#include <mutex>

namespace buddy
{
    struct SingleThreaded
    {
        struct Empty
        {
            inline void lock() const{};
            inline void unlock()const {};
        };
        template <class T>
        struct LockGuard
        {
            LockGuard(T t){}
        };
        using LockType = Empty;
        using ScopedLock = LockGuard<LockType>;
    };

    struct MultiThreaded
    {
        using LockType = std::mutex;
        using ScopedLock = std::lock_guard<LockType>;
    };
}