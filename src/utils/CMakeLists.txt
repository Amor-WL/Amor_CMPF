# utils模块的CMakeLists.txt

# 收集utils模块的源文件
file(GLOB UTILS_SOURCES "*.cpp" "config/*.cpp" "shm/*.cpp" "log/*.cpp")

# 创建utils模块的动态库
add_library(cmpf_utils SHARED ${UTILS_SOURCES})

# utils模块不直接依赖core模块，而是通过基础模块加载机制加载

# 安装规则
install(TARGETS cmpf_utils
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)