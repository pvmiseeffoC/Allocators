# CMakeList.txt : CMake project for Allocators, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_C_COMPILER, "clang")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)

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
	            "BuddyAllocator/BuddyAllocator.hpp"
	            "BuddyAllocator/BuddyAllocator.h"
	            "BuddyAllocator/ThreadingPolicies.h"
	            "LinearAllocator/LinearAllocator.h"
				"SmallObjectAllocator/Chunk.h" 
				"SmallObjectAllocator/Chunk.cpp" 
				"SmallObjectAllocator/FixedAllocator.h" 
				"SmallObjectAllocator/FixedAllocator.cpp" 
				"SmallObjectAllocator/SmallObjectAllocator.h"
				"SmallObjectAllocator/SmallObjectAllocator.hpp"
				"SmallObjectAllocator/SmallObjectAllocatorSingleton.h"
				"SmallObjectAllocator/PolicyClasses.h"
				"SmallObjectAllocator/SmallObjectBase.h"
				"SmallObjectAllocator/SmallObject.h"
				"SmallObjectAllocator/SmallValueObject.h"
				"Tests/BuddyAllocatorTests.cpp"
				"Tests/Benchmark.h"
				"Tests/SmallObjectAllocatorTests.cpp"
				"main.cpp")

##TODO: Add tests and install targets if needed.

include_directories(${CMAKE_CURRENT_LIST_DIR})

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()

# Now simply link against gtest or gtest_main as needed. Eg
target_link_libraries(Allocators gtest_main)

# Create groups on your project
# createProjectFilters(FILES <files> [PREFIX <prefix>] [ROOT <rootFolder>])
FUNCTION(createProjectFilters)
	cmake_parse_arguments(ARG "" "PREFIX;ROOT" "FILES" ${ARGN})
	
	STRING(REPLACE "/" "\\" _prefix "${ARG_PREFIX}")
	
	foreach(_source IN ITEMS ${ARG_FILES})
		get_filename_component(_source_path "${_source}" PATH)
		if (ARG_ROOT)
			string(REPLACE "${ARG_ROOT}" "" _relative_source_path "${_source_path}")
		else()
			set(_relative_source_path "${_source_path}")
		endif()
		
		string(REPLACE "/" "\\" _group_path "${_relative_source_path}")
		source_group("${_prefix}\\${_group_path}" FILES "${_source}")
	endforeach()
ENDFUNCTION()

# Generate project folders
get_target_property(MySources Allocators SOURCES)
createProjectFilters(FILES ${MySources} ROOT "${CMAKE_CURRENT_LIST_DIR}")

get_target_property(MyInterfaceSources Allocators INTERFACE_SOURCES)
createProjectFilters(FILES ${MyInterfaceSources} ROOT "${CMAKE_CURRENT_LIST_DIR}")