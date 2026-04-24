#include "cmpf_core_module.h"
#include <dlfcn.h>
#include <dirent.h>
#include <algorithm>
#include <thread>
#include <iostream>

namespace cmpf {

// Module类实现
Module::Module(const std::string& name, void* handle) 
    : name_(name), handle_(handle) {
}

Module::~Module() {
    if (handle_) {
        dlclose(handle_);
    }
}

bool Module::init() {
    return true;
}

void Module::destroy() {
    if (handle_) {
        dlclose(handle_);
        handle_ = nullptr;
    }
}

const std::string& Module::name() const {
    return name_;
}

void* Module::handle() const {
    return handle_;
}

void Module::addThread(const ThreadInfo& thread) {
    threads_.push_back(thread);
}

const std::vector<ThreadInfo>& Module::threads() const {
    return threads_;
}

// ModuleManager类实现
ModuleManager::ModuleManager() {
}

ModuleManager::~ModuleManager() {
    for (auto& module : modules_) {
        module->destroy();
    }
}

ModuleManager& ModuleManager::instance() {
    static ModuleManager instance;
    return instance;
}

bool ModuleManager::loadModule(const std::string& so_path) {
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
    
    auto module = std::make_shared<Module>(module_name, handle);
    modules_.push_back(module);
    std::cout << "Loaded module: " << module_name << " from: " << filename << std::endl;
    return true;
}

bool ModuleManager::loadModulesFromDirectory(const std::string& directory) {
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
            loadModule(so_path);
        }
    }

    closedir(dir);
    return true;
}

const std::vector<std::shared_ptr<Module>>& ModuleManager::modules() const {
    return modules_;
}

void ModuleManager::RegisterThread(const std::string& module_name, const std::string& thread_name, int priority, std::function<void()> func) {
    // 查找模块
    for (auto& module : modules_) {
        if (module->name() == module_name) {
            ThreadInfo thread;
            thread.name = thread_name;
            thread.priority = priority;
            thread.func = func;
            module->addThread(thread);
            std::cout << "Registered thread: " << thread_name << " for module: " << module_name << " with priority: " << priority << std::endl;
            return;
        }
    }

    // 如果模块不存在，创建新模块
    auto module = std::make_shared<Module>(module_name, nullptr);
    ThreadInfo thread;
    thread.name = thread_name;
    thread.priority = priority;
    thread.func = func;
    module->addThread(thread);
    modules_.push_back(module);
    std::cout << "Registered thread: " << thread_name << " for new module: " << module_name << " with priority: " << priority << std::endl;
}

// ThreadManager类实现
ThreadManager::ThreadManager() {
}

ThreadManager& ThreadManager::instance() {
    static ThreadManager instance;
    return instance;
}

bool ThreadManager::startBusinessThreads() {
    // 按优先级排序线程
    std::vector<ThreadInfo> all_threads;
    for (const auto& module : ModuleManager::instance().modules()) {
        for (const auto& thread : module->threads()) {
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

} // namespace cmpf
