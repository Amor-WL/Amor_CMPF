#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include "core/cmpf_core.h"
#include "log/cmpf_log.h"
#include "memory/cmpf_memory.h"
#include "process/cmpf_process.h"
#include "shared_memory/cmpf_shared_memory.h"
#include "utils/cmpf_utils.h"

// 声明外部函数
void log_stub();
void memory_stub();
void process_stub();
void shared_memory_stub();
void utils_stub();

int main(int argc, char* argv[]) {
    // 获取实例id，默认为1
    int instance_id = 1;
    if (argc > 1) {
        instance_id = std::atoi(argv[1]);
    }
    
    // 打印日志
    g_logger.init();
    g_logger.writef("Worker process started with instance_id: %d", instance_id);
    // 初始化日志
    g_logger.init();
    g_logger.writef("Worker process started");
    
    // 通知systemd服务已启动
    sd_notify(0, "READY=1");
    
    // 创建epoll实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        g_logger.writef("Failed to create epoll instance");
        return 1;
    }
    
    // 注册信号处理（可选）
    struct epoll_event events[10];
    
    // 进入epoll循环，阻塞等待事件
    g_logger.writef("Worker entering epoll loop");
    while (true) {
        int nfds = epoll_wait(epoll_fd, events, 10, -1);
        if (nfds == -1) {
            g_logger.writef("Epoll wait error");
            break;
        }
        
        // 处理事件（目前为空，仅用于阻塞）
    }
    
    // 关闭epoll
    close(epoll_fd);
    
    g_logger.writef("Worker process exiting");
    return 0;
}