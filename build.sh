conan install . --output-folder=build --build=missing
cmake --preset conan-release
mkdir build
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake  -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release
cmake --build .
chmod +x ip_filter
cpack --config CPackConfig.cmake
make package_source