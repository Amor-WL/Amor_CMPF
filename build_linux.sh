#!/bin/bash

# 显示帮助信息
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -h    Show this help message"
    echo "  -t    Compile test cases (currently not implemented)"
    echo "  -c    Clean the output directory"
    echo "  -C    Clean the build directory"
    echo ""
    echo "Without options, the script will compile all subprojects by default."
}

# 解析命令行参数
clean_output=false
clean_build=false

while getopts "htcC" opt; do
    case $opt in
        h) 
            show_help
            exit 0
            ;;
        t) 
            echo "Compiling test cases..."
            echo "ok"
            exit 0
            ;;
        c) 
            clean_output=true
            ;;
        C) 
            clean_build=true
            ;;
        *) 
            show_help
            exit 1
            ;;
    esac
done

# 执行清理操作
if [ "$clean_output" = true ]; then
    echo "Cleaning output directory..."
    rm -rf build/output
fi

if [ "$clean_build" = true ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# 创建构建目录
mkdir -p build
cd build

# 运行CMake生成Makefile
cmake .. -DCMAKE_BUILD_TYPE=Release

# 编译项目
make

# 如果编译成功，运行测试
if [ -f output/bin/test_cmpf ]; then
    echo "Running test..."
    LD_LIBRARY_PATH=./output/lib ./output/bin/test_cmpf
fi

cd ..
echo "Build completed."