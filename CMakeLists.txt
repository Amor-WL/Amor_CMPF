cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(cmpf VERSION 1.0.0 LANGUAGES CXX)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 配置编译选项
add_compile_options(-Wall -Wextra -Werror -std=c++17)

# 包含头文件目录
link_directories(${CMAKE_BINARY_DIR}/lib/Release)
include_directories(include)

# 添加各模块子目录
add_subdirectory(src/core)
add_subdirectory(src/log)
add_subdirectory(src/memory)
add_subdirectory(src/process)
add_subdirectory(src/shared_memory)
add_subdirectory(src/utils)

# 添加可执行文件子目录
add_subdirectory(src/main/manager)
add_subdirectory(src/main/worker)

# 安装规则
install(TARGETS cmpf_core cmpf_log cmpf_memory cmpf_process cmpf_shared_memory cmpf_utils manager worker
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include/cmpf
    FILES_MATCHING PATTERN "*.h"
)