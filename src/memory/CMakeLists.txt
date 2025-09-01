# memory模块的CMakeLists.txt

# 收集memory模块的源文件
file(GLOB MEMORY_SOURCES "*.cpp")

# 创建memory模块的动态库
if(MSVC)
    # Windows平台创建DLL
    add_library(cmpf_memory SHARED ${MEMORY_SOURCES})
    target_compile_definitions(cmpf_memory PRIVATE CMPF_BUILD)
else()
    # Linux平台创建SO
    add_library(cmpf_memory SHARED ${MEMORY_SOURCES})
    set_target_properties(cmpf_memory PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# 设置输出目录
set_target_properties(cmpf_memory PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/lib/Release"
    LIBRARY_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/lib/Release"
    RUNTIME_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/bin/Release"
)

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