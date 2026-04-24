#include "core/cmpf_core.h"
#include "cmpf_core_module.h"
#include <iostream>

namespace cmpf {

// Core类的单例实现
Core& Core::instance() {
    static Core instance;
    return instance;
}

// 构造函数
Core::Core() {
}

// 析构函数
Core::~Core() {
}

// 初始化core模块
bool Core::InitCore() {
    std::cout << "Core module initialized" << std::endl;
    return true;
}

// 加载基础模块
bool Core::LoadBaseModules() {
    std::string base_dir = "/opt/cmpf/cmpf_core";
    std::cout << "Loading base modules from: " << base_dir << std::endl;
    return ModuleManager::instance().loadModulesFromDirectory(base_dir);
}

// 加载业务模块
bool Core::LoadBusinessModules(const std::string& process_name) {
    // 构建业务模块目录路径
    std::string business_dir = "/opt/cmpf/" + process_name;
    std::cout << "Loading business modules from: " << business_dir << std::endl;
    return ModuleManager::instance().loadModulesFromDirectory(business_dir);
}

// 启动所有注册的业务线程
bool Core::StartBusinessThreads() {
    return ThreadManager::instance().startBusinessThreads();
}

// 注册业务线程
void Core::RegisterThread(const std::string& module_name, const std::string& thread_name, int priority, std::function<void()> func) {
    ModuleManager::instance().RegisterThread(module_name, thread_name, priority, func);
}

// 初始化并启动核心流程（包含初始化core、加载基础模块、加载业务模块、启动业务线程）
bool Core::InitAndStartCore(const std::string& process_name) {
    return InitAndStartCore(process_name, 0);
}

// 初始化并启动核心流程（包含初始化core、加载基础模块、加载业务模块、启动业务线程），支持实例ID
bool Core::InitAndStartCore(const std::string& process_name, int instance_id) {
    // 标记instance_id参数为已使用
    (void)instance_id;
    
    // 初始化core模块
    if (!InitCore()) {
        std::cout << "Failed to initialize core module" << std::endl;
        return false;
    }
    
    // 加载基础模块
    if (!LoadBaseModules()) {
        std::cout << "Failed to load base modules" << std::endl;
        return false;
    }
    
    // 加载业务模块
    if (!LoadBusinessModules(process_name)) {
        std::cout << "Failed to load business modules" << std::endl;
        return false;
    }
    
    // 启动业务线程
    if (!StartBusinessThreads()) {
        std::cout << "Failed to start business threads" << std::endl;
        return false;
    }
    
    return true;
}

// 全局函数实现，调用Core类的方法
bool InitAndStartCore(const std::string& process_name) {
    return Core::instance().InitAndStartCore(process_name);
}

bool InitAndStartCore(const std::string& process_name, int instance_id) {
    return Core::instance().InitAndStartCore(process_name, instance_id);
}

void RegisterThread(const std::string& module_name, const std::string& thread_name, int priority, std::function<void()> func) {
    Core::instance().RegisterThread(module_name, thread_name, priority, func);
}

} // namespace cmpf