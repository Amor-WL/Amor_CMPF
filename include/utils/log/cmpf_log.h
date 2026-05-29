#ifndef CMPF_INCLUDE_UTILS_LOG_CMPF_LOG_H_
#define CMPF_INCLUDE_UTILS_LOG_CMPF_LOG_H_

#include <cstdio>

namespace cmpf {

// 日志缓冲区分级大小定义
constexpr int kCmpfLogBufferLow = 256;
constexpr int kCmpfLogBufferMiddle = 512;
constexpr int kCmpfLogBufferHigh = 1024;
constexpr int kCmpfLogBufferHuge = 4096;

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
