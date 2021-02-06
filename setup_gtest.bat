pushd googletest        # Main directory of the cloned repository.
mkdir build          # Create a directory to hold the build output.
pushd build
cmake ..             # Generate native build scripts for GoogleTest.

popd #build

popd #googletest