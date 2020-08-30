// Tests.cpp : Defines the entry point for the application.
//
#include <iostream>
#include "SmallObjectAllocator/SmallObject.h"

using namespace allocators;
template <typename... Args>
class my_tuple
    : public SmallObject<
          DEFAULT_THREADING_MODEL,
          DEFAULT_CHUNK_SIZE,
          MAX_SMALL_OBJECT_SIZE,
          DEFAULT_ALIGN_SIZE,
          DEFAULT_MUTEX>
{
public:

};

using namespace std;

int main()
{
    cout << "Hello CMake." << endl;
    return 0;
}
