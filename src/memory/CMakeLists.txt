# memory模块的CMakeLists.txt

# 收集memory模块的源文件
file(GLOB MEMORY_SOURCES "*.cpp")

# 创建memory模块的动态库
add_library(cmpf_memory SHARED ${MEMORY_SOURCES})

# memory模块依赖core和log模块
target_link_libraries(cmpf_memory
    PUBLIC
        cmpf_core
        cmpf_log
)

# 安装规则
install(TARGETS cmpf_memory
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)