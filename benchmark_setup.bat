pushd benchmark

# Make a build directory to place the build output.
call cmake -E make_directory "build"
# Generate build system files with cmake.
call cmake -E chdir "build" cmake -DCMAKE_BUILD_TYPE=Release ../

call cmake -E chdir "build" cmake -DCMAKE_BUILD_TYPE=Debug ../
# or, starting with CMake 3.13, use a simpler form:
# cmake -DCMAKE_BUILD_TYPE=Release -S . -B "build"
# Build the library.
call cmake --build "build" --config Release

call cmake --build "build" --config Debug

popd