# core模块的CMakeLists.txt

# 收集core模块的源文件
file(GLOB CORE_SOURCES "*.cpp")

# 创建core模块的动态库
if(MSVC)
    # Windows平台创建DLL
    add_library(cmpf_core SHARED ${CORE_SOURCES})
    target_compile_definitions(cmpf_core PRIVATE CMPF_BUILD)
else()
    # Linux平台创建SO
    add_library(cmpf_core SHARED ${CORE_SOURCES})
    set_target_properties(cmpf_core PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# 设置输出目录
set_target_properties(cmpf_core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/lib/Release"
    LIBRARY_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/lib/Release"
    RUNTIME_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/bin/Release"
)

# core模块不需要依赖其他模块，因为它是基础模块

# 安装规则
install(TARGETS cmpf_core
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)