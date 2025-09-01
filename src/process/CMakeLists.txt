# process模块的CMakeLists.txt

# 收集process模块的源文件
file(GLOB PROCESS_SOURCES "*.cpp")

# 创建process模块的动态库
if(MSVC)
    # Windows平台创建DLL
    add_library(cmpf_process SHARED ${PROCESS_SOURCES})
    target_compile_definitions(cmpf_process PRIVATE CMPF_BUILD)
else()
    # Linux平台创建SO
    add_library(cmpf_process SHARED ${PROCESS_SOURCES})
    set_target_properties(cmpf_process PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
endif()

# 设置输出目录
set_target_properties(cmpf_process PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/lib/Release"
    LIBRARY_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/lib/Release"
    RUNTIME_OUTPUT_DIRECTORY_Release "${CMAKE_BINARY_DIR}/bin/Release"
)

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