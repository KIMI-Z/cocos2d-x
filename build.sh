#run in build directory,new build can use rm command to clean the build directory, second build can remove the rm command to save time
#rm -rf *
cmake ..     -DCMAKE_CXX_STANDARD=17     -DCMAKE_CXX_FLAGS="-D_LIBCPP_ENABLE_CXX17_REMOVED_UNARY_BINARY_FUNCTION"     -DCMAKE_EXE_LINKER_FLAGS="-L/media/kimi/Data/Document/Cmake/cocos2d-x/external -lglibc_compat -lm -Wl,--undefined-version"     -DUSE_FMOD=OFF

make -j$(nproc)