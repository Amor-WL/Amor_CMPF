#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/wait.h>
#include <map>
#include "core/cmpf_core.h"
#include "log/cmpf_log.h"
#include "memory/cmpf_memory.h"
#include "process/cmpf_process.h"
#include "shared_memory/cmpf_shared_memory.h"
#include "utils/cmpf_utils.h"
#include "config/cmpf_config.h"

// 定义worker状态结构
struct WorkerInfo {
    int id;
    pid_t pid;
    int fail_count;
    bool running;
};

void log_stub();
void memory_stub();
void process_stub();
void shared_memory_stub();
void utils_stub();

// 初始化配置和日志
void init_system(int instance_id) {
    // 加载配置文件
    bool config_result = g_config.init("/opt/cmpf/config/cmpf.conf");
    if (!config_result) {
        std::cerr << "Failed to load config file, using default settings" << std::endl;
    }
    
    // 读取日志目录配置
    std::string log_dir_str = g_config.get_string("manager.log.dir", "log");
    const char* log_dir = log_dir_str.c_str();
    
    // 初始化日志
    g_logger.init(log_dir);
    g_logger.writef("Manager process started with instance_id: %d", instance_id);
    g_logger.writef("Manager process started");
    if (!config_result) {
        g_logger.writef("Failed to load config file, using default settings");
    }
}

// 初始化worker信息
std::map<int, WorkerInfo> init_workers() {
    // 获取worker数量，默认值为2
    int worker_count = g_config.get_int("worker.count", 2);
    g_logger.writef("Worker count configured: %d", worker_count);
    
    // 存储worker信息
    std::map<int, WorkerInfo> workers;
    
    // 初始化worker进程信息
    for (int i = 1; i <= worker_count; i++) {
        WorkerInfo info;
        info.id = i;
        info.pid = 0;
        info.fail_count = 0;
        info.running = false;
        workers[i] = info;
    }
    
    return workers;
}

// 启动或重启worker进程
void start_worker(WorkerInfo& info) {
    if (!info.running && info.fail_count < 15) {
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程 - 启动worker
            std::string worker_id_str = std::to_string(info.id);
            execlp("/opt/cmpf/bin/worker", "worker", worker_id_str.c_str(), nullptr);
            // 如果execlp返回，说明出错了
            g_logger.writef("Failed to start worker %d", info.id);
            exit(1);
        } else if (pid > 0) {
            // 父进程 - 记录worker PID
            info.pid = pid;
            info.running = true;
            g_logger.writef("Started worker %d with PID: %d", info.id, pid);
        } else {
            // fork失败
            g_logger.writef("Failed to fork worker %d", info.id);
            info.fail_count++;
            g_logger.writef("Worker %d failed to start, fail count: %d", info.id, info.fail_count);
        }
    }
}

// 检查worker进程状态
void check_worker_status(std::map<int, WorkerInfo>& workers) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0) {
        // 找到对应的worker
        for (auto& pair : workers) {
            WorkerInfo& info = pair.second;
            if (info.pid == pid) {
                info.running = false;
                info.fail_count++;
                g_logger.writef("Worker %d (PID: %d) exited with status: %d, fail count: %d", 
                              info.id, pid, status, info.fail_count);
                
                if (info.fail_count >= 15) {
                    g_logger.writef("Worker %d has failed %d times, stopping attempts", 
                                  info.id, info.fail_count);
                }
                break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    // 获取实例id，默认为1
    int instance_id = 1;
    if (argc > 1) {
        instance_id = std::atoi(argv[1]);
    }
    
    // 初始化系统
    init_system(instance_id);
    
    // 初始化worker信息
    std::map<int, WorkerInfo> workers = init_workers();
    
    // 通知systemd服务已启动
    sd_notify(0, "READY=1");
    
    // 进入主循环
    g_logger.writef("Manager entering main loop");
    while (true) {
        // 检查并启动/重启worker
        for (auto& pair : workers) {
            WorkerInfo& info = pair.second;
            start_worker(info);
        }
        
        // 检查worker进程状态
        check_worker_status(workers);
        
        // 短暂睡眠，避免CPU占用过高
        usleep(100000); // 100ms
    }
    
    g_logger.writef("Manager process exiting");
    return 0;
}