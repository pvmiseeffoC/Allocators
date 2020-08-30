#include <cassert>
#include <memory>

namespace allocators
{
    template <long long MAX_SIZE>
    class LinearAllocator
    {
        char* _memory;
        long long _offset;

    public:
        LinearAllocator()
        {
            _memory = static_cast<char*>( malloc( MAX_SIZE ) );
            _offset = 0;
        }

        ~LinearAllocator() { free( _memory ); }

        void* allocate( std::size_t size )
        {
            assert( MAX_SIZE - _offset > size )

                char* address = _memory + _offset;

            _offset += size;

            return address;
        }

        void deallocate( void* ptr, size_t size ) { _offset -= size; }
    };
}  // namespace allocators
