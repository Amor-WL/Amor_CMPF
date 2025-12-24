#pragma once
#include <cstdio>

class Logger {
public:
    Logger();
    ~Logger();
    
    void init();
    void write(const char* message);
    void writef(const char* format, ...);
    
private:
    bool m_initialized;
    FILE* m_file;
};

// 全局日志对象
extern Logger g_logger;

void log_stub();

// 这里可以添加未来的日志模块声明