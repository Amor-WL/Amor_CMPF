# utils模块的CMakeLists.txt

# 收集utils模块的源文件
file(GLOB UTILS_SOURCES "*.cpp")

# 创建utils模块的动态库
add_library(cmpf_utils SHARED ${UTILS_SOURCES})

# utils模块依赖core模块
target_link_libraries(cmpf_utils
    PUBLIC
        cmpf_core
)

# 安装规则
install(TARGETS cmpf_utils
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)