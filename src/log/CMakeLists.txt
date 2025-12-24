# log模块的CMakeLists.txt

# 收集log模块的源文件
file(GLOB LOG_SOURCES "*.cpp")

# 创建log模块的动态库
add_library(cmpf_log SHARED ${LOG_SOURCES})



# log模块依赖core模块
target_link_libraries(cmpf_log
    PUBLIC
        cmpf_core
)

# 安装规则
install(TARGETS cmpf_log
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)