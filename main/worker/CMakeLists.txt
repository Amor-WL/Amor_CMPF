# worker目录的CMakeLists.txt

# 收集worker的源文件
file(GLOB WORKER_SOURCES "*.cpp")

# 创建worker可执行文件
add_executable(worker ${WORKER_SOURCES})

# 设置输出目录
set_target_properties(worker PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/bin/Release"
)

# worker可执行文件依赖所有模块
target_link_libraries(worker
    PRIVATE
        cmpf_core
        cmpf_log
        cmpf_memory
        cmpf_process
        cmpf_shared_memory
        cmpf_utils
)

# 安装规则
install(TARGETS worker
    RUNTIME DESTINATION bin
)