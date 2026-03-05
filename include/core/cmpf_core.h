#ifndef CMPF_CORE_H_
#define CMPF_CORE_H_

#include <string>
#include <vector>
#include <functional>

namespace cmpf {

// Core类 - 核心模块类
class Core {
public:
    // 获取单例实例
    static Core& instance();
    
    // 初始化并启动核心流程（包含初始化core、加载基础模块、加载业务模块、启动业务线程）
    bool InitAndStartCore(const std::string& process_name);
    
    // 初始化并启动核心流程（包含初始化core、加载基础模块、加载业务模块、启动业务线程），支持实例ID
    bool InitAndStartCore(const std::string& process_name, int instance_id);
    
    // 注册业务线程
    void RegisterThread(const std::string& module_name, const std::string& thread_name, int priority, std::function<void()> func);
    
private:
    // 构造函数
    Core();
    
    // 析构函数
    ~Core();
    
    // 初始化core模块
    bool InitCore();
    
    // 加载基础模块
    bool LoadBaseModules();
    
    // 加载业务模块
    bool LoadBusinessModules(const std::string& process_name);
    
    // 启动所有注册的业务线程
    bool StartBusinessThreads();
};

// 全局函数，方便调用
bool InitAndStartCore(const std::string& process_name);
bool InitAndStartCore(const std::string& process_name, int instance_id);
void RegisterThread(const std::string& module_name, const std::string& thread_name, int priority, std::function<void()> func);

} // namespace cmpf

#endif // CMPF_CORE_H_