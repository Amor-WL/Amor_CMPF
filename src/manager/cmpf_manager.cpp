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

#include <sys/epoll.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <systemd/sd-daemon.h>

#include "core/cmpf_core.h"
#include "utils/config/cmpf_config.h"
#include "utils/log/cmpf_log.h"

namespace cmpf {

// worker最大失败次数（全局配置）
static int g_worker_max_fail_count = 15;

// worker连续失败重置时间（分钟，全局配置）
// 两次退出间隔超过此时间，则不视为连续失败，失败次数重置
static int g_worker_reset_fail_interval_minutes = 30;

// 定义worker状态结构
struct WorkerInfo {
    int id;
    pid_t pid;
    int fail_count;
    time_t last_exit_time;  // 上次退出时间，用于判断是否为连续失败
    bool running;
};

// 处理worker进程退出，判断是否为连续失败
void HandleWorkerExit(WorkerInfo& info, pid_t pid, int status);

// 初始化配置和日志
void InitSystem(int instance_id) {
    // 加载配置文件
    bool config_result = GetConfig().Init("/opt/cmpf/config/cmpf.conf");
    if (!config_result) {
        GetLogger().Write("Failed to load config file, using default settings");
    }

    // 读取日志目录配置
    std::string log_dir_str = GetConfig().GetString("manager.log.dir", "log");
    const char* log_dir = log_dir_str.c_str();

    // 初始化日志
    GetLogger().Init(log_dir);
    GetLogger().Writef("Manager process started with instance_id: %d",
                       instance_id);
}

// 初始化worker信息
std::map<int, WorkerInfo> InitWorkers() {
    // 获取worker数量
    int worker_count = GetConfig().GetInt("worker.count", 2);
    GetLogger().Writef("Worker count configured: %d", worker_count);

    // 获取最大失败次数配置
    g_worker_max_fail_count = GetConfig().GetInt("worker.max_fail_count", 15);
    GetLogger().Writef("Worker max fail count configured: %d",
                       g_worker_max_fail_count);

    // 获取连续失败重置时间配置
    g_worker_reset_fail_interval_minutes =
        GetConfig().GetInt("worker.reset_fail_interval_minutes", 30);
    GetLogger().Writef("Worker reset fail interval configured: %d minutes",
                       g_worker_reset_fail_interval_minutes);

    // 存储worker信息
    std::map<int, WorkerInfo> workers;

    // 初始化worker进程信息
    for (int i = 1; i <= worker_count; i++) {
        WorkerInfo info;
        info.id = i;
        info.pid = 0;
        info.fail_count = 0;
        info.last_exit_time = 0;
        info.running = false;
        workers[i] = info;
    }

    return workers;
}

// 处理worker进程退出，判断是否为连续失败
void HandleWorkerExit(WorkerInfo& info, pid_t pid, int status) {
    info.running = false;
    // 判断是否为连续失败
    time_t now = time(nullptr);
    if (info.last_exit_time != 0) {
        // 计算两次退出的时间差（秒）
        int diff_seconds = static_cast<int>(now - info.last_exit_time);
        int threshold_seconds = g_worker_reset_fail_interval_minutes * 60;

        if (diff_seconds > threshold_seconds) {
            // 间隔太长，重置失败次数
            info.fail_count = 1;
            GetLogger().Writef(
                "Worker %d (PID: %d) exited status: %d, "
                "after %d min (%d min threshold), reset fail count: %d/%d",
                info.id, pid, status, diff_seconds / 60,
                g_worker_reset_fail_interval_minutes,
                info.fail_count, g_worker_max_fail_count);
        } else {
            // 连续失败，计数加1
            info.fail_count++;
            GetLogger().Writef(
                "Worker %d (PID: %d) exited status: %d, "
                "after %d sec (consecutive), fail count: %d/%d",
                info.id, pid, status, diff_seconds,
                info.fail_count, g_worker_max_fail_count);
        }
    } else {
        // 第一次退出
        info.fail_count = 1;
        GetLogger().Writef(
            "Worker %d (PID: %d) exited status: %d, "
            "first exit, fail count: %d/%d",
            info.id, pid, status,
            info.fail_count, g_worker_max_fail_count);
    }

    // 更新上次退出时间
    info.last_exit_time = now;

    if (info.fail_count >= g_worker_max_fail_count) {
        GetLogger().Writef(
            "Worker %d has failed %d times (max: %d), stopping attempts",
            info.id, info.fail_count, g_worker_max_fail_count);
    }
}

// 启动或重启worker进程
void StartWorker(WorkerInfo& info) {
    if (!info.running && info.fail_count < g_worker_max_fail_count) {
        pid_t pid = fork();
        if (pid == 0) {
            // 子进程 - 启动worker
            std::string worker_id_str = std::to_string(info.id);
            execlp("/opt/cmpf/bin/cmpf_main", "cmpf_main",
                   "worker", worker_id_str.c_str(), nullptr);
            // 如果execlp返回，说明出错了
            GetLogger().Writef("Failed to start worker %d", info.id);
            exit(1);
        } else if (pid > 0) {
            // 父进程 - 记录worker PID
            info.pid = pid;
            info.running = true;
            GetLogger().Writef("Started worker %d with PID: %d",
                               info.id, pid);
        } else {
            // fork失败
            GetLogger().Writef("Failed to fork worker %d", info.id);
            info.fail_count++;
            GetLogger().Writef("Worker %d failed to start, fail count: %d",
                               info.id, info.fail_count);
        }
    }
}

// 检查worker进程状态
bool CheckWorkerStatus(std::map<int, WorkerInfo>& workers) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0) {
        // 找到对应的worker
        for (auto& pair : workers) {
            WorkerInfo& info = pair.second;
            if (info.pid == pid) {
                HandleWorkerExit(info, pid, status);
                break;
            }
        }
        return true;
    }
    return false;
}

// manager主循环
void ManagerMainLoop(int instance_id) {
    // 初始化系统
    InitSystem(instance_id);

    // 初始化worker信息
    std::map<int, WorkerInfo> workers = InitWorkers();

    // 通知systemd服务已启动
    sd_notify(0, "READY=1");

    // 进入主循环
    GetLogger().Writef("Manager entering main loop");
    while (true) {
        // 检查并启动/重启worker
        for (auto& pair : workers) {
            WorkerInfo& info = pair.second;
            StartWorker(info);
        }

        // 检查worker进程状态
        bool has_event = CheckWorkerStatus(workers);

        // 如果有进程退出事件，不sleep，直接继续下一轮检查
        // 避免大量进程同时退出时处理不过来
        if (!has_event) {
            usleep(100000); // 100ms
        }
    }
}

} // namespace cmpf

// 模块初始化函数
extern "C" void InitModule() {
    std::cout << "Initializing manager module" << std::endl;

    // 注册manager主线程
    cmpf::RegisterThread("manager", "manager_main", 1, []() {
        // 传递默认实例ID 0
        cmpf::ManagerMainLoop(0);
    });
}
