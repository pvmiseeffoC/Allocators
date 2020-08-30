#pragma once
#include "SmallObjectBase.h"

namespace allocators
{
    template <
        template <class, class> class ThreadingPolicy = DEFAULT_THREADING_MODEL,
        SizeType chunkSize = DEFAULT_CHUNK_SIZE,
        SizeType maxSmallObjectSize = MAX_SMALL_OBJECT_SIZE,
        SizeType objectAlignSize = DEFAULT_ALIGN_SIZE,
        class MutexPolicy = DEFAULT_MUTEX>
    class SmallValueObject
        : public SmallObjectBase<
              ThredingPolicy,
              chunkSize,
              maxSmallObjectSize,
              objectAlignSize,
              MutexPolicy>
    {
    protected:
        inline SmallValueObject() = default;
        inline SmallValueObject(SmallValueObject const&) = default;
        inline SmallValueObject& operator=(SmallValueObject const&) = default;
        inline ~SmallValueObject() = default;
    };
}  // namespace allocators