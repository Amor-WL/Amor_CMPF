// Copyright 2026 CMPF Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "core/cmpf_core.h"
#include "log/cmpf_log.h"
#include "memory/cmpf_memory.h"
#include "process/cmpf_process.h"
#include "shared_memory/cmpf_shared_memory.h"
#include "utils/cmpf_utils.h"
#include "config/cmpf_config.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/wait.h>
#include <map>

namespace cmpf {

// 定义worker状态结构
struct WorkerInfo {
    int id;
    pid_t pid;
    int fail_count;
    bool running;
};

void LogStub();
void MemoryStub();
void ProcessStub();
void SharedMemoryStub();
void UtilsStub();

// 初始化配置和日志
void InitSystem(int instance_id) {
    // 加载配置文件
    bool config_result = GetConfig().init("/opt/cmpf/config/cmpf.conf");
    if (!config_result) {
        GetLogger().write("Failed to load config file, using default settings");
    }
    
    // 读取日志目录配置
    std::string log_dir_str = GetConfig().get_string("manager.log.dir", "log");
    const char* log_dir = log_dir_str.c_str();
    
    // 初始化日志
    GetLogger().init(log_dir);
    GetLogger().writef("Manager process started with instance_id: %d", instance_id);
    GetLogger().writef("Manager process started");
    if (!config_result) {
        GetLogger().writef("Failed to load config file, using default settings");
    }
}

// 初始化worker信息
std::map<int, WorkerInfo> InitWorkers() {
    // 获取worker数量，默认值为2
    int worker_count = GetConfig().get_int("worker.count", 2);
    GetLogger().writef("Worker count configured: %d", worker_count);
    
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
void StartWorker(WorkerInfo& info) {
    if (!info.running && info.fail_count < 15) {
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程 - 启动worker
            std::string worker_id_str = std::to_string(info.id);
            execlp("/opt/cmpf/bin/worker", "worker", worker_id_str.c_str(), nullptr);
            // 如果execlp返回，说明出错了
            GetLogger().writef("Failed to start worker %d", info.id);
            exit(1);
        } else if (pid > 0) {
            // 父进程 - 记录worker PID
            info.pid = pid;
            info.running = true;
            GetLogger().writef("Started worker %d with PID: %d", info.id, pid);
        } else {
            // fork失败
            GetLogger().writef("Failed to fork worker %d", info.id);
            info.fail_count++;
            GetLogger().writef("Worker %d failed to start, fail count: %d", info.id, info.fail_count);
        }
    }
}

// 检查worker进程状态
void CheckWorkerStatus(std::map<int, WorkerInfo>& workers) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0) {
        // 找到对应的worker
        for (auto& pair : workers) {
            WorkerInfo& info = pair.second;
            if (info.pid == pid) {
                info.running = false;
                info.fail_count++;
                GetLogger().writef("Worker %d (PID: %d) exited with status: %d, fail count: %d", 
                              info.id, pid, status, info.fail_count);
                
                if (info.fail_count >= 15) {
                    GetLogger().writef("Worker %d has failed %d times, stopping attempts", 
                                  info.id, info.fail_count);
                }
                break;
            }
        }
    }
}

} // namespace cmpf

int main(int argc, char* argv[]) {
    // 获取实例id，默认为1
    int instance_id = 1;
    if (argc > 1) {
        instance_id = std::atoi(argv[1]);
    }
    
    // 初始化系统
    cmpf::InitSystem(instance_id);
    
    // 初始化worker信息
    std::map<int, cmpf::WorkerInfo> workers = cmpf::InitWorkers();
    
    // 通知systemd服务已启动
    sd_notify(0, "READY=1");
    
    // 进入主循环
    cmpf::GetLogger().writef("Manager entering main loop");
    while (true) {
        // 检查并启动/重启worker
        for (auto& pair : workers) {
            cmpf::WorkerInfo& info = pair.second;
            cmpf::StartWorker(info);
        }
        
        // 检查worker进程状态
        cmpf::CheckWorkerStatus(workers);
        
        // 短暂睡眠，避免CPU占用过高
        usleep(100000); // 100ms
    }
    
    cmpf::GetLogger().writef("Manager process exiting");
    return 0;
}