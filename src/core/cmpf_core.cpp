#include "core/cmpf_core.h"
#include <dlfcn.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <iostream>

namespace cmpf {

// 全局模块信息
std::vector<ModuleInfo> g_modules;

// 初始化core模块
bool InitCore() {
    std::cout << "Core module initialized" << std::endl;
    return true;
}

// 加载单个so文件
bool LoadModule(const std::string& so_path) {
    void* handle = dlopen(so_path.c_str(), RTLD_NOW);
    if (!handle) {
        std::cout << "Failed to load module: " << so_path << ", error: " << dlerror() << std::endl;
        return false;
    }

    // 尝试获取模块初始化函数
    InitFunc init_func = (InitFunc)dlsym(handle, "InitModule");
    if (init_func) {
        init_func();
    }

    std::string filename = so_path.substr(so_path.find_last_of("/") + 1);
    // 提取模块名，去掉lib前缀和.so后缀
    std::string module_name = filename;
    if (module_name.substr(0, 3) == "lib") {
        module_name = module_name.substr(3);
    }
    if (module_name.size() > 3 && module_name.substr(module_name.size() - 3) == ".so") {
        module_name = module_name.substr(0, module_name.size() - 3);
    }
    ModuleInfo module;
    module.name = module_name;
    module.handle = handle;
    g_modules.push_back(module);
    std::cout << "Loaded module: " << module_name << " from: " << filename << std::endl;
    return true;
}

// 加载指定目录下的所有so文件
bool LoadModulesFromDirectory(const std::string& directory) {
    DIR* dir = opendir(directory.c_str());
    if (!dir) {
        std::cout << "Failed to open directory: " << directory << std::endl;
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        if (filename == "." || filename == "..") {
            continue;
        }

        if (filename.size() > 3 && filename.substr(filename.size() - 3) == ".so") {
            std::string so_path = directory + "/" + filename;
            LoadModule(so_path);
        }
    }

    closedir(dir);
    return true;
}

// 加载基础模块
bool LoadBaseModules() {
    std::string base_dir = "/opt/cmpf/cmpf_core";
    std::cout << "Loading base modules from: " << base_dir << std::endl;
    return LoadModulesFromDirectory(base_dir);
}

// 加载业务模块
bool LoadBusinessModules(const std::string& process_name) {
    // 构建业务模块目录路径
    std::string business_dir = "/opt/cmpf/" + process_name;
    std::cout << "Loading business modules from: " << business_dir << std::endl;
    return LoadModulesFromDirectory(business_dir);
}

// 启动所有注册的业务线程
bool StartBusinessThreads() {
    // 按优先级排序线程
    std::vector<ThreadInfo> all_threads;
    for (const auto& module : g_modules) {
        for (const auto& thread : module.threads) {
            all_threads.push_back(thread);
        }
    }

    // 按优先级排序
    std::sort(all_threads.begin(), all_threads.end(), [](const ThreadInfo& a, const ThreadInfo& b) {
        return a.priority < b.priority;
    });

    // 启动线程
    for (const auto& thread : all_threads) {
        std::cout << "Starting thread: " << thread.name << " with priority: " << thread.priority << std::endl;
        std::thread(thread.func).detach();
    }

    return true;
}

// 注册业务线程
void RegisterThread(const std::string& module_name, const std::string& thread_name, int priority, std::function<void()> func) {
    // 查找模块
    for (auto& module : g_modules) {
        if (module.name == module_name) {
            ThreadInfo thread;
            thread.name = thread_name;
            thread.priority = priority;
            thread.func = func;
            module.threads.push_back(thread);
            std::cout << "Registered thread: " << thread_name << " for module: " << module_name << " with priority: " << priority << std::endl;
            return;
        }
    }

    // 如果模块不存在，创建新模块
    ModuleInfo module;
    module.name = module_name;
    module.handle = nullptr;
    
    ThreadInfo thread;
    thread.name = thread_name;
    thread.priority = priority;
    thread.func = func;
    module.threads.push_back(thread);
    
    g_modules.push_back(module);
    std::cout << "Registered thread: " << thread_name << " for new module: " << module_name << " with priority: " << priority << std::endl;
}

// 获取模块信息
std::vector<ModuleInfo> GetModules() {
    return g_modules;
}

// 初始化并启动核心流程（包含初始化core、加载基础模块、加载业务模块、启动业务线程）
bool InitAndStartCore(const std::string& process_name) {
    return InitAndStartCore(process_name, 0);
}

// 初始化并启动核心流程（包含初始化core、加载基础模块、加载业务模块、启动业务线程），支持实例ID
bool InitAndStartCore(const std::string& process_name, int instance_id) {
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

void CoreStub()
{
    return;
}

} // namespace cmpf