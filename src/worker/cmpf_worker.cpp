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
#include "process/cmpf_process.h"
#include "utils/config/cmpf_config.h"
#include "utils/log/cmpf_log.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <systemd/sd-daemon.h>
#include <iostream>

namespace cmpf {

// worker主循环
void WorkerMainLoop(int instance_id) {
    // 加载配置文件
    bool config_result = GetConfig().init("/opt/cmpf/config/cmpf.conf");
    if (!config_result) {
        GetLogger().write("Failed to load config file, using default settings");
    }
    
    // 读取日志目录配置
    std::string log_dir_str = GetConfig().get_string("worker.log.dir", "log");
    const char* log_dir = log_dir_str.c_str();
    
    // 初始化日志
    GetLogger().init(log_dir);
    GetLogger().writef("Worker process started with instance_id: %d", instance_id);
    GetLogger().writef("Worker process started");
    if (!config_result) {
        GetLogger().writef("Failed to load config file, using default settings");
    }
    
    // 通知systemd服务已启动
    sd_notify(0, "READY=1");
    
    // 创建epoll实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        GetLogger().writef("Failed to create epoll instance");
        return;
    }
    
    // 注册信号处理（可选）
    struct epoll_event events[10];
    
    // 进入epoll循环，阻塞等待事件
    GetLogger().writef("Worker entering epoll loop");
    while (true) {
        int nfds = epoll_wait(epoll_fd, events, 10, -1);
        if (nfds == -1) {
            GetLogger().writef("Epoll wait error");
            break;
        }
        
        // 处理事件（目前为空，仅用于阻塞）
    }
    
    // 关闭epoll
    close(epoll_fd);
    
    GetLogger().writef("Worker process exiting");
}

} // namespace cmpf

// 模块初始化函数
extern "C" void InitModule() {
    std::cout << "Initializing worker module" << std::endl;
    
    // 注册worker主线程
    cmpf::RegisterThread("worker", "worker_main", 1, []() {
        // 传递默认实例ID 1
        cmpf::WorkerMainLoop(1);
    });
}
