# CMakeLists.txt for worker module

# 设置库名称
set(LIBRARY_NAME cmpf_worker)

# 添加源文件
set(SOURCE_FILES
    cmpf_worker.cpp
)

# 创建共享库
add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES})

# 设置包含目录
target_include_directories(${LIBRARY_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 链接依赖库
target_link_libraries(${LIBRARY_NAME} PRIVATE
    cmpf_core
    cmpf_process
    cmpf_utils
    dl
    systemd
)

# 设置输出路径
set_target_properties(${LIBRARY_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib
    PREFIX "lib"
    SUFFIX ".so"
)
