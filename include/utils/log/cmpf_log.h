#ifndef CMPF_INCLUDE_UTILS_LOG_CMPF_LOG_H_
#define CMPF_INCLUDE_UTILS_LOG_CMPF_LOG_H_

#include <cstdio>

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

// 这里可以添加未来的日志模块声明

#endif // CMPF_INCLUDE_UTILS_LOG_CMPF_LOG_H_