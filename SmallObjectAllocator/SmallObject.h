#pragma once
#include "SmallObjectBase.h"

namespace allocators
{
    template <
        template <class, class>
        class ThreadingPolicy,
        SizeType chunkSize = DEFAULT_CHUNK_SIZE,
        SizeType maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE,
        SizeType objectAlignSize = DEFAULT_ALIGN_SIZE,
        class MutexPolicy = DEFAULT_MUTEX>
    class SmallObject
        : public SmallObjectBase<
              ThreadingPolicy,
              chunkSize,
              maxSmallObjectSize,
              objectAlignSize,
              MutexPolicy>
    {
    public:
        virtual ~SmallObject() = default;

    protected:
        inline SmallObject() = default;

        SmallObject( SmallObject const& ) = delete;           // non-copyable
        SmallObject& operator=( SmallObject const& ) = delete;// non-copyable
    };
}  // namespace allocators
