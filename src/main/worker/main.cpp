#include <iostream>
#include "core/cmpf_core.h"
#include "log/cmpf_log.h"
#include "memory/cmpf_memory.h"
#include "process/cmpf_process.h"
#include "shared_memory/cmpf_shared_memory.h"
#include "utils/cmpf_utils.h"

// 声明外部函数
extern "C" void log_stub();
extern "C" void memory_stub();
extern "C" void process_stub();
extern "C" void shared_memory_stub();
extern "C" void utils_stub();

int main() {
    std::cout << "Worker started" << std::endl;
    
    // 调用各个模块的函数以验证链接
    log_stub();
    memory_stub();
    process_stub();
    shared_memory_stub();
    utils_stub();
    
    std::cout << "Worker finished" << std::endl;
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();
    return 0;
}