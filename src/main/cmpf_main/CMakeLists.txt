# CMakeLists.txt for cmpf_main

# 设置可执行文件名称
set(EXECUTABLE_NAME cmpf_main)

# 添加源文件
set(SOURCE_FILES
    main.cpp
)

# 创建可执行文件
add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

# 设置包含目录
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# 链接依赖库
target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    cmpf_core
    dl
)

# 设置输出路径
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin
)
