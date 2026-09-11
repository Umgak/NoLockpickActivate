cmake -B deps/common/build -S deps/common -DCMAKE_INSTALL_PREFIX="$PWD" deps/common
cmake --build deps/common/build --config Release --target install
cmake -B deps/skse64/build -S deps/skse64 -DCMAKE_INSTALL_PREFIX="$PWD" deps/skse64