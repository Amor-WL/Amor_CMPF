cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(Amor VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 配置编译选项
if(MSVC)
    # Windows平台编译选项
    add_compile_options(/W4 /WX /source-charset:utf-8 /execution-charset:utf-8)
    # 禁用特定警告
    add_compile_options(/wd4819)
else()
    # Linux平台编译选项
    add_compile_options(-Wall -Wextra -Werror -std=c++11)
endif()

# 定义宏，用于导出API和平台检测
add_definitions(-DAMOR_BUILD)
if(MSVC)
    add_definitions(-D_WIN32 -DAMOR_PLATFORM_WINDOWS)
endif()

# 包含头文件目录
include_directories(include)

# 收集所有源文件
file(GLOB_RECURSE SOURCES "src/**/*.cpp")

# 创建动态库
if(MSVC)
    # Windows平台创建DLL
    add_library(amor SHARED ${SOURCES})
    target_compile_definitions(amor PRIVATE AMOR_BUILD)
else()
    # Linux平台创建SO
    add_library(amor SHARED ${SOURCES})
    set_target_properties(amor PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# 安装规则
install(TARGETS amor
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/ DESTINATION include/amor
    FILES_MATCHING PATTERN "*.h"
)

# 可选：创建示例程序
# add_executable(amor_example example/main.c)
# target_link_libraries(amor_example amor)