#ifndef CMPF_CORE_MODULE_H_
#define CMPF_CORE_MODULE_H_

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace cmpf {

// 线程信息结构体
typedef struct {
    std::string name;           // 线程名称
    int priority;               // 线程优先级，数值越小优先级越高
    std::function<void()> func; // 线程函数
} ThreadInfo;

// 模块信息结构体（保持向后兼容）
typedef struct {
    std::string name;           // 模块名称
    void* handle;               // 模块句柄
    std::vector<ThreadInfo> threads; // 模块注册的线程
} ModuleInfo;

// 模块基类
class Module {
public:
    virtual ~Module();
    
    // 初始化模块
    virtual bool init();
    
    // 销毁模块
    virtual void destroy();
    
    // 获取模块名称
    const std::string& name() const;
    
    // 获取模块句柄
    void* handle() const;
    
    // 添加线程
    void addThread(const ThreadInfo& thread);
    
    // 获取线程列表
    const std::vector<ThreadInfo>& threads() const;

public:
    Module(const std::string& name, void* handle = nullptr);

private:
    std::string name_;
    void* handle_;
    std::vector<ThreadInfo> threads_;
};

// 模块管理器
class ModuleManager {
public:
    static ModuleManager& instance();
    
    // 加载单个模块
    bool loadModule(const std::string& so_path);
    
    // 从目录加载模块
    bool loadModulesFromDirectory(const std::string& directory);
    
    // 获取所有模块
    const std::vector<std::shared_ptr<Module>>& modules() const;

    // 注册线程
    void RegisterThread(const std::string& module_name, const std::string& thread_name, int priority, std::function<void()> func);

private:
    ModuleManager();
    ~ModuleManager();
    
    std::vector<std::shared_ptr<Module>> modules_;
};

// 线程管理器
class ThreadManager {
public:
    static ThreadManager& instance();
    
    // 启动所有注册的业务线程
    bool startBusinessThreads();

private:
    ThreadManager();
};

// 模块初始化函数类型
typedef void (*InitFunc)();

} // namespace cmpf

#endif // CMPF_CORE_MODULE_H_