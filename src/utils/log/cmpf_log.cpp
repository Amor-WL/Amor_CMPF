#include "utils/log/cmpf_log.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctime>

namespace cmpf {

// 实现Logger类
Logger::Logger() : initialized_(false), file_(nullptr) {
}

Logger::~Logger() {
    if (initialized_ && file_ != nullptr) {
        fclose(file_);
        file_ = nullptr;
        initialized_ = false;
    }
}

void Logger::Init() {
    Init("log");
}

void Logger::Init(const char* log_dir) {
    if (initialized_) {
        return;
    }

    // 检查并创建日志目录
    struct stat st;
    if (stat(log_dir, &st) == -1) {
        if (mkdir(log_dir, 0755) == -1) {
            char error_msg[kCmpfLogBufferMiddle];
            snprintf(error_msg, sizeof(error_msg),
                     "Failed to create log directory: %s\n", log_dir);
            Write(error_msg);
            return;
        }
    }

    // 获取pid
    pid_t pid = getpid();

    // 获取进程名
    char process_name[kCmpfLogBufferLow];
    snprintf(process_name, sizeof(process_name), "/proc/%d/comm", pid);

    FILE* comm_file = fopen(process_name, "r");
    char name[kCmpfLogBufferLow] = "unknown";
    if (comm_file != nullptr) {
        if (fscanf(comm_file, "%s", name) != 1) {
            strncpy(name, "unknown", sizeof(name));
        }
        fclose(comm_file);
    }

    // 创建日志文件名
    char log_filename[kCmpfLogBufferMiddle];
    snprintf(log_filename, sizeof(log_filename),
             "%s/%d_%s.log", log_dir, pid, name);

    // 打开日志文件
    file_ = fopen(log_filename, "a");
    if (file_ != nullptr) {
        // 写入初始日志
        time_t now = time(0);
        struct tm tstruct = *localtime(&now);
        char time_str[80];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tstruct);
        fprintf(file_,
                "[%s] Log initialized for process %d (%s) in directory %s\n",
                time_str, pid, name, log_dir);
        fflush(file_);
        initialized_ = true;
    } else {
        char error_msg[kCmpfLogBufferHigh];
        snprintf(error_msg, sizeof(error_msg),
                 "Failed to open log file: %s\n", log_filename);
        Write(error_msg);
    }
}

void Logger::Write(const char* message) {
    if (!initialized_ || file_ == nullptr) {
        char error_msg[256];
        snprintf(error_msg, sizeof(error_msg),
                 "Logger not initialized, cannot write log: %s\n", message);
        fprintf(stderr, "%s", error_msg);
        return;
    }

    // 写入日志
    time_t now = time(0);
    struct tm tstruct = *localtime(&now);
    char time_str[80];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &tstruct);
    fprintf(file_, "[%s] %s\n", time_str, message);
    fflush(file_);
}

void Logger::Writef(const char* format, ...) {
    if (!initialized_ || file_ == nullptr) {
        va_list args;
        va_start(args, format);
        fprintf(stderr, "Logger not initialized, cannot write log: ");
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");
        va_end(args);
        return;
    }

    // 创建缓冲区来存储格式化后的字符串
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // 调用Write方法来输出日志
    Write(buffer);
}

// 获取日志对象
Logger& GetLogger() {
    static Logger* logger = new Logger();
    return *logger;
}

void LogStub() {
    GetLogger().Write("cmpf_log stub");
    return;
}

} // namespace cmpf
