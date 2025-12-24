# manager目录的CMakeLists.txt

# 收集manager的源文件
file(GLOB MANAGER_SOURCES "*.cpp")

# 创建manager可执行文件
add_executable(manager ${MANAGER_SOURCES})

# manager可执行文件依赖所有模块
target_link_libraries(manager
    PRIVATE
        cmpf_core
        cmpf_log
        cmpf_memory
        cmpf_process
        cmpf_shared_memory
        cmpf_utils
)

# 安装规则
install(TARGETS manager
    RUNTIME DESTINATION bin
)