#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <vector>
#include <string>
#include <cstdlib>
#include "core/cmpf_core.h"
#include "log/cmpf_log.h"
#include "memory/cmpf_memory.h"
#include "process/cmpf_process.h"
#include "shared_memory/cmpf_shared_memory.h"
#include "utils/cmpf_utils.h"
#include "config/cmpf_config.h"

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
    
    // 初始化日志
    g_logger.init();
    g_logger.writef("Manager process started with instance_id: %d", instance_id);
    g_logger.writef("Manager process started");
    
    // 加载配置文件
    bool config_result = g_config.init("/opt/cmpf/config/cmpf.conf");
    if (!config_result) {
        g_logger.writef("Failed to load config file, using default settings");
    }
    
    // 获取worker数量，默认值为2
    int worker_count = g_config.get_int("worker.count", 2);
    g_logger.writef("Worker count configured: %d", worker_count);
    
    // 启动worker进程
    std::vector<pid_t> worker_pids;
    for (int i = 1; i <= worker_count; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程 - 启动worker
            std::string worker_id = std::to_string(i);
            execlp("/usr/bin/cmpf_worker", "cmpf_worker", worker_id.c_str(), nullptr);
            // 如果execlp返回，说明出错了
            g_logger.writef("Failed to start worker %d", i);
            exit(1);
        } else if (pid > 0) {
            // 父进程 - 记录worker PID
            worker_pids.push_back(pid);
            g_logger.writef("Started worker %d with PID: %d", i, pid);
        } else {
            // fork失败
            g_logger.writef("Failed to fork worker %d", i);
        }
    }
    
    // 通知systemd服务已启动
    sd_notify(0, "READY=1");
    
    // 创建epoll实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        g_logger.writef("Failed to create epoll instance");
        return 1;
    }
    
    // 注册信号处理（可选）
    // struct epoll_event event;
    struct epoll_event events[10];
    
    // 进入epoll循环，阻塞等待事件
    g_logger.writef("Manager entering epoll loop");
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
    
    g_logger.writef("Manager process exiting");
    return 0;
}