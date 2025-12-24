# core模块的CMakeLists.txt

# 收集core模块的源文件
file(GLOB CORE_SOURCES "*.cpp")

# 创建core模块的动态库
add_library(cmpf_core SHARED ${CORE_SOURCES})



# core模块不需要依赖其他模块，因为它是基础模块

# 安装规则
install(TARGETS cmpf_core
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)