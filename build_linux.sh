#!/bin/bash

# 创建构建目录
mkdir -p build
cd build

# 运行CMake生成Makefile
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
make

# 如果编译成功，运行测试
if [ -f test_amor ]; then
    echo "Running test..."
    ./test_amor
fi

cd ..
echo "Build completed."