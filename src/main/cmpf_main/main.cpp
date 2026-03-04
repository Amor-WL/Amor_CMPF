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
#include <iostream>
#include <cstdlib>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // 解析命令行参数
    std::string instance_type = "manager"; // 默认类型
    int instance_id = 0; // 默认ID
    
    if (argc > 1) {
        instance_type = argv[1];
    }
    
    if (argc > 2) {
        instance_id = std::atoi(argv[2]);
    }
    
    std::cout << "Starting CMPF process with type: " << instance_type 
              << ", id: " << instance_id << std::endl;
    
    // 初始化并启动核心流程
    if (!cmpf::InitAndStartCore(instance_type, instance_id)) {
        std::cout << "Failed to initialize and start core process" << std::endl;
        return 1;
    }
    
    // 进入主循环，保持进程运行
    std::cout << "CMPF process " << instance_type << " (id: " << instance_id 
              << ") started successfully, entering main loop" << std::endl;
    
    while (true) {
        // 无限循环，保持进程运行
        // 实际业务逻辑由加载的模块中的线程处理
        sleep(1);
    }
    
    return 0;
}
