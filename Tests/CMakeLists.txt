# CMakeList.txt : CMake project for Tests, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("Tests")

# Add source to this project's executable.
add_executable (Tests "Tests.cpp" )

add_subdirectory(benchmark)

include_directories(${CMAKE_CURRENT_LIST_DIR}/..)


target_link_libraries(Tests LINK_PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../Allocators)
target_link_libraries(Tests LINK_PRIVATE benchmark::benchmark)

target_include_directories(Tests PUBLIC ${CMAKE_CURRENT_LIST_DIR}/.. ${CMAKE_CURRENT_LIST_DIR}/benchmark/include)