#include <iostream>
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

int main() {
    std::cout << "Manager started" << std::endl;
    
    // 调用各个模块的函数以验证链接
    log_stub();
    memory_stub();
    process_stub();
    shared_memory_stub();
    utils_stub();
    
    std::cout << "Manager finished" << std::endl;
    std::cout << "Press any key to exit..." << std::endl;
    std::cin.get();
    return 0;
}