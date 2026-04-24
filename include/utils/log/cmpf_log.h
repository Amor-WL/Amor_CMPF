#ifndef CMPF_INCLUDE_UTILS_LOG_CMPF_LOG_H_
#define CMPF_INCLUDE_UTILS_LOG_CMPF_LOG_H_

#include <cstdio>

// 日志缓冲区分级大小定义
#define CMPF_LOG_BUFFER_LOW     256
#define CMPF_LOG_BUFFER_MIDDLE  512
#define CMPF_LOG_BUFFER_HIGH    1024
#define CMPF_LOG_BUFFER_HUGE    4096

namespace cmpf {

class Logger {
public:
    Logger();
    ~Logger();
    
    void Init();
    void Init(const char* log_dir);
    void Write(const char* message);
    void Writef(const char* format, ...);
    
private:
    bool initialized_;
    FILE* file_;
};

// 获取日志对象
Logger& GetLogger();

void LogStub();

} // namespace cmpf

#endif // CMPF_INCLUDE_UTILS_LOG_CMPF_LOG_H_