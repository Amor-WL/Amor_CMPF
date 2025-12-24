# shared_memory模块的CMakeLists.txt

# 收集shared_memory模块的源文件
file(GLOB SHARED_MEMORY_SOURCES "*.cpp")

# 创建shared_memory模块的动态库
add_library(cmpf_shared_memory SHARED ${SHARED_MEMORY_SOURCES})

# shared_memory模块依赖core、log和memory模块
target_link_libraries(cmpf_shared_memory
    PUBLIC
        cmpf_core
        cmpf_log
        cmpf_memory
)

# 安装规则
install(TARGETS cmpf_shared_memory
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)