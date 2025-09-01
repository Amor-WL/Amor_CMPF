# log模块的CMakeLists.txt

# 收集log模块的源文件
file(GLOB LOG_SOURCES "*.cpp")

# 创建log模块的动态库
if(MSVC)
    # Windows平台创建DLL
    add_library(cmpf_log SHARED ${LOG_SOURCES})
    target_compile_definitions(cmpf_log PRIVATE CMPF_BUILD)
else()
    # Linux平台创建SO
    add_library(cmpf_log SHARED ${LOG_SOURCES})
    set_target_properties(cmpf_log PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# 设置输出目录
set_target_properties(cmpf_log PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/lib/Release"
    LIBRARY_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/lib/Release"
    RUNTIME_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/bin/Release"
)

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