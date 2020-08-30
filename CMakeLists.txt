﻿# CMakeList.txt : CMake project for Allocators, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

SET(CMAKE_CXX_STANDARD 17)

project ("Allocators")

if(MSVC)
    add_definitions(/FI"utils.h" /W4 /WX)
else()
    # GCC or Clang
    add_definitions(-include utils.h -Wall -Wextra -pedantic -Werror)
endif()
include_directories(${CMAKE_CURRENT_LIST_DIR})

# Add source to this project's executable.
add_executable (Allocators 
				"main.cpp" 
				"utils.h"
	            "LinearAllocator/LinearAllocator.h"
				"SmallObjectAllocator/Chunk.h" 
				"SmallObjectAllocator/Chunk.cpp" 
				"SmallObjectAllocator/FixedAllocator.h" 
				"SmallObjectAllocator/FixedAllocator.cpp" 
				"SmallObjectAllocator/SmallObjectAllocator.h"
				"SmallObjectAllocator/SmallObjectAllocator.cpp"
				"SmallObjectAllocator/SmallObjectAllocatorSingleton.h"
				"SmallObjectAllocator/PolicyClasses.h" "SmallObjectAllocator/SmallObjectBase.h" "SmallObjectAllocator/SmallObject.h" "SmallObjectAllocator/SmallValueObject.h")

# TODO: Add tests and install targets if needed.