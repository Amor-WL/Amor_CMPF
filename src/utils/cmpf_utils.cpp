#include "utils/cmpf_utils.h"
#include "core/cmpf_core.h"
#include <unistd.h>

namespace cmpf {

void UtilsStub()
{
    return;
}

// 模块初始化函数
extern "C" void InitModule() {
    // 注册一个示例线程
    RegisterThread("libcmpf_utils.so", "utils_thread", 5, []() {
        // 线程函数逻辑
        while (true) {
            // 这里可以添加具体的线程逻辑
            sleep(1);
        }
    });
}

} // namespace cmpf