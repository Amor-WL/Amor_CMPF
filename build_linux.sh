#!/bin/bash

# 显示帮助信息
show_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  -h    Show this help message"
    echo "  -t    Compile test cases (currently not implemented)"
    echo "  -c    Clean the output directory"
    echo "  -C    Clean the build directory"
    echo "  -d    Deploy to specified directory after compilation"
    echo ""
    echo "Without options, the script will compile all subprojects by default."
}

# 解析命令行参数
clean_output=false
clean_build=false
deploy_dir=""

while getopts "htcCd:" opt; do
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
        d) 
            deploy_dir="$OPTARG"
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

# 部署逻辑
if [ -n "$deploy_dir" ]; then
    echo "Deploying to $deploy_dir..."
    
    # 检查目录是否存在
    if [ ! -d "$deploy_dir" ]; then
        echo "Error: Deployment directory $deploy_dir does not exist."
        exit 1
    fi
    
    # 创建必要的目录结构
    mkdir -p "$deploy_dir/bin"
    mkdir -p "$deploy_dir/lib"
    mkdir -p "$deploy_dir/service"
    
    # 复制二进制文件（保留bin与lib目录结构）
    if [ -d "build/output/bin" ]; then
        cp -r "build/output/bin/"* "$deploy_dir/bin/"
    fi
    
    if [ -d "build/output/lib" ]; then
        cp -r "build/output/lib/"* "$deploy_dir/lib/"
    fi
    
    # 复制scripts目录下的service后缀文件与target后缀文件到service目录
    if [ -d "scripts" ]; then
        cp -f "scripts/"*.service "$deploy_dir/service/" 2>/dev/null || true
        cp -f "scripts/"*.target "$deploy_dir/service/" 2>/dev/null || true
    fi
    
    # 复制scripts目录下的sh后缀文件到目标路径
    if [ -d "scripts" ]; then
        cp -f "scripts/"*.sh "$deploy_dir/" 2>/dev/null || true
    fi
    
    # 复制配置文件到目标路径
    if [ -f "scripts/cmpf.conf" ]; then
        cp -f "scripts/cmpf.conf" "$deploy_dir/" 2>/dev/null || true
    fi
    
    echo "Deployment completed successfully."
fi

echo "Build completed."