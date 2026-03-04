#ifndef CMPF_CORE_H_
#define CMPF_CORE_H_

#include <string>
#include <vector>
#include <functional>

namespace cmpf {

// 业务线程信息结构体
typedef struct {
    std::string name;           // 线程名称
    int priority;               // 线程优先级，数值越小优先级越高
    std::function<void()> func; // 线程函数
} ThreadInfo;

// 模块信息结构体
typedef struct {
    std::string name;           // 模块名称
    void* handle;               // 模块句柄
    std::vector<ThreadInfo> threads; // 模块注册的线程
} ModuleInfo;

// 初始化core模块
bool InitCore();

// 加载基础模块
bool LoadBaseModules();

// 加载业务模块
bool LoadBusinessModules(const std::string& process_name);

// 启动所有注册的业务线程
bool StartBusinessThreads();

// 初始化并启动核心流程（包含初始化core、加载基础模块、加载业务模块、启动业务线程）
bool InitAndStartCore(const std::string& process_name);

// 初始化并启动核心流程（包含初始化core、加载基础模块、加载业务模块、启动业务线程），支持实例ID
bool InitAndStartCore(const std::string& process_name, int instance_id);

// 模块初始化函数类型
typedef void (*InitFunc)();

// 注册业务线程
void RegisterThread(const std::string& module_name, const std::string& thread_name, int priority, std::function<void()> func);

// 获取模块信息
std::vector<ModuleInfo> GetModules();

void CoreStub();

} // namespace cmpf

#endif // CMPF_CORE_H_