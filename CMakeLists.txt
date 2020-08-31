# CMakeList.txt : CMake project for Allocators, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

SET(CMAKE_CXX_STANDARD 17)

if(MSVC)
add_definitions(/O2)
##else()
##    # GCC or Clang
##    add_definitions(-Wall -Wextra -pedantic -Werror)
endif()

project ("Allocators")
# Add source to this project's executable.
add_executable (Allocators
				"utils.h"
	            "LinearAllocator/LinearAllocator.h"
				"SmallObjectAllocator/Chunk.h" 
				"SmallObjectAllocator/Chunk.cpp" 
				"SmallObjectAllocator/FixedAllocator.h" 
				"SmallObjectAllocator/FixedAllocator.cpp" 
				"SmallObjectAllocator/SmallObjectAllocator.h"
				"SmallObjectAllocator/SmallObjectAllocator.cpp"
				"SmallObjectAllocator/SmallObjectAllocatorSingleton.h"
				"SmallObjectAllocator/PolicyClasses.h"
				"SmallObjectAllocator/SmallObjectBase.h"
				"SmallObjectAllocator/SmallObject.h"
				"SmallObjectAllocator/SmallValueObject.h"
				"main.cpp")


add_subdirectory(benchmark)		
target_link_libraries(Allocators LINK_PRIVATE benchmark::benchmark)
target_include_directories(Allocators PUBLIC ${CMAKE_CURRENT_LIST_DIR}/benchmark/include)

# TODO: Add tests and install targets if needed.

include_directories(${CMAKE_CURRENT_LIST_DIR})
