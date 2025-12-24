# process模块的CMakeLists.txt

# 收集process模块的源文件
file(GLOB PROCESS_SOURCES "*.cpp")

# 创建process模块的动态库
add_library(cmpf_process SHARED ${PROCESS_SOURCES})



# process模块依赖core、log和memory模块
target_link_libraries(cmpf_process
    PUBLIC
        cmpf_core
        cmpf_log
        cmpf_memory
)

# 安装规则
install(TARGETS cmpf_process
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)