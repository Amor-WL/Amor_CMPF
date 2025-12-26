# 设置编译选项
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义源文件和头文件
set(SRC_FILES cmpf_config.cpp)
set(INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/include/config)

# 创建库
add_library(cmpf_config SHARED ${SRC_FILES})

# 设置包含目录
target_include_directories(cmpf_config PUBLIC ${INCLUDE_DIRS})

# 设置输出路径
set_target_properties(cmpf_config PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib
)
