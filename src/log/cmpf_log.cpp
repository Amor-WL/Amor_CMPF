#include "log/cmpf_log.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

// 实现Logger类
Logger::Logger() : m_initialized(false), m_file(nullptr) {
}

Logger::~Logger() {
    if (m_initialized && m_file != nullptr) {
        fclose(m_file);
        m_file = nullptr;
        m_initialized = false;
    }
}

void Logger::init() {
    if (m_initialized) {
        return;
    }

    // 创建log文件夹
    mkdir("log", 0755);
    
    // 获取pid
    pid_t pid = getpid();
    
    // 获取进程名
    char process_name[256];
    snprintf(process_name, sizeof(process_name), "/proc/%d/comm", pid);
    
    FILE* comm_file = fopen(process_name, "r");
    char name[256] = "unknown";
    if (comm_file != nullptr) {
        if (fscanf(comm_file, "%s", name) != 1) {
            strncpy(name, "unknown", sizeof(name));
        }
        fclose(comm_file);
    }
    
    // 创建日志文件名
    char log_filename[256];
    snprintf(log_filename, sizeof(log_filename), "log/%d_%s.log", pid, name);
    
    // 打开日志文件
    m_file = fopen(log_filename, "a");
    if (m_file != nullptr) {
        // 写入初始日志
        time_t now = time(0);
        struct tm tstruct = *localtime(&now);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tstruct);
        fprintf(m_file, "[%s] Log initialized for process %d (%s)\n", time_str, pid, name);
        fflush(m_file);
        m_initialized = true;
    } else {
        std::cerr << "Failed to open log file: " << log_filename << std::endl;
    }
}

void Logger::write(const char* message) {
    if (!m_initialized || m_file == nullptr) {
        std::cerr << "Logger not initialized, cannot write log: " << message << std::endl;
        return;
    }

    // 写入日志
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    char time_str[80];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tstruct);
    fprintf(m_file, "[%s] %s\n", time_str, message);
    fflush(m_file);
}

void Logger::writef(const char* format, ...) {
    if (!m_initialized || m_file == nullptr) {
        va_list args;
        va_start(args, format);
        std::cerr << "Logger not initialized, cannot write log: ";
        vfprintf(stderr, format, args);
        std::cerr << std::endl;
        va_end(args);
        return;
    }

    // 创建缓冲区来存储格式化后的字符串
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 调用write方法来输出日志
    write(buffer);
}

// 定义全局日志对象
Logger g_logger;

void log_stub() {
    std::cout << "cmpf_log stub" << std::endl;
    return;
}