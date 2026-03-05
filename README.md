CMPF（Common Multi-Process Framework, 通用多进程框架）是一个基于C++的多进程框架，它提供了一种简单而强大的方式来创建和管理多个进程。
对于业务模块来说，只需要通过so部署，即可在CMPF框架中运行，让代码聚焦于业务逻辑，而不需要关注进程管理等底层细节。

# 项目背景
现代应用对多进程框架的需求呈现出多核并行、故障隔离、高性能三大核心特征。但目前市面上并没有一个基于C++的通用多进程框架。
本项目是个实验性的通用多进程框架，灵感来自于Nginx以及防火墙开源项目WAF的设计。旨在探讨多进程框架的通用性支持的可行性。

# 开发环境
- 操作系统：WSL2 Ubuntu 22.04
- 编译器：g++ 11.3.0
- 构建工具：cmake 3.22.1

# 编译工程
编译的统一入口为build_linux.sh
一般使用如下命令执行编译与部署：

```bash
# 编译并部署到指定目录
./build_linux.sh -d ./run_space
cd run_space
sudo ./start_service.sh
```

# 执行
在执行了上述的部署命令后，在部署路径下会生成以下文件：

```plaintext
run_space/
├── bin/
│   ├── cmpf_main
├── service/
│   ├── cmpf_manager.service
├── lib/
│   ├── libcmpf_core.so
│   ├── libcmpf_process.so
│   ├── libcmpf_utils.so
│   ├── libcmpf_manager.so
│   ├── libcmpf_worker.so
├── start_service.sh
├── cmpf.conf
```

随后用户执行sudo ./start_service.sh脚本即可启动CMPF框架。
该脚本会往systemd服务部署目录下拷贝service文件，随后将二进制文件和库文件部署到系统目录下。
随后执行systemctl start命令启动cmpf_manager服务，该服务使用cmpf_main二进制。

# 框架整体部署架构
当前的项目是个多进程框架，worker模型。使用systemd进行部署管理。
1. 二进制文件：只有cmpf_main一个统一入口。libcmpf_xxx.so等模块动态链接文件若干。
2. 进程：manager一个，worker可能有多个（通过cmpf_main启动，指定不同的实例ID），worker数量由配置文件决定。
3. manager进程被systemd管理，worker进程被manager进程管理。
4. worker负责具体的业务功能。manager只负责进程管理。
5. 在manager中，负责管理worker进程的生命周期，包括启动、监控和重启。当worker进程异常退出时，会自动重启；当worker重复重启超过15次时，会停止尝试重启。

## 统一二进制入口
本项目使用cmpf_main作为统一的二进制入口，通过命令行参数来指定实例类型和ID：
- 启动manager实例：`cmpf_main manager`
- 启动worker实例：`cmpf_main worker <worker_id>`

这种设计简化了部署和管理，只需要维护一个二进制文件，而不是多个不同的二进制文件。

## Core模块功能
Core模块（libcmpf_core.so）是所有进程都需要加载的核心模块，定义了所有CMPF进程都应该执行的基本操作：

1. **基础模块加载与初始化**：
   - 在系统的/opt/cmpf/cmpf_core目录下，遍历所有的so文件，进行统一加载
   - 该目录存放了所有需要加载的基础so的软链接，如libcmpf_utils.so

2. **业务模块加载与初始化**：
   - 在/opt/cmpf/"进程名"目录下，遍历所有的so文件，进行统一加载
   - 该目录存放了所有需要加载的业务so的软链接，如libcmpf_manager.so和libcmpf_worker.so

3. **业务线程管理**：
   - 初始化、启动线程
   - 在so加载步骤中，模块的InitModule函数会注册业务线程信息
   - 按优先级进行线程的启动

4. **实例管理**：
   - 支持通过命令行参数指定实例类型和ID
   - 根据实例类型加载对应的业务模块

Core模块通过dlopen动态加载其他模块，实现了模块的热插拔和灵活配置。

# 代码结构
本项目采用清晰的代码文件与头文件分离架构，便于模块管理和代码维护。

## 目录结构
```plaintext
Amor_CMPF/
├── include/           # 对外发布的头文件
│   ├── core/          # 核心模块头文件
│   ├── process/       # 进程模块头文件
│   └── utils/         # 工具模块头文件
├── src/               # 源代码文件
│   ├── core/          # 核心模块实现
│   ├── process/       # 进程模块实现
│   ├── utils/         # 工具模块实现
│   ├── log/           # 日志模块
│   ├── main/          # 主程序入口
│   ├── manager/       # 管理器模块
│   └── worker/        # 工作进程模块
├── build/             # 构建目录
├── run_space/         # 运行空间
└── scripts/           # 脚本文件
```

## 头文件分离设计
本项目采用头文件分离的设计理念，将头文件分为两类：

### 1. 对外发布的头文件（放在 `include` 目录）
- 包含公共接口和数据结构
- 被其他模块引用的头文件
- 定义模块间通信的接口
- 会被安装到系统目录中

### 2. 模块内部使用的头文件（放在 `src/模块名` 目录）
- 只在本模块内部使用的头文件
- 不被其他模块直接引用的头文件
- 包含实现细节的头文件
- 不会被安装到系统目录中

## 模块对外头文件管理规范
- 模块对外头文件仅能暴露需要公开发布的接口
- 对于同时包含对外发布接口和内部接口的C++模块：
  * 将需要发布的头文件放置于include目录中
  * 通过类的不同访问域（如public、private）来区分公共接口和内部接口
  * 不再使用单独的"inner"后缀头文件
- 注意事项：
  * 若模块为纯对外发布或纯内部依赖，则仅需一份头文件，并放置于相应的合适目录
  * 对于需要在多个文件中共享的内部接口，可使用"inner"后缀头文件（如cmpf_core_inner.h），放置于模块目录中

## 单文件内非成员函数设计经验
对于仅在单个文件中使用的内部结构体和函数，推荐使用以下设计方案：

### 1. 使用未命名命名空间隔离内部实现
- 在实现文件（.cpp）中创建未命名命名空间（`namespace {}`）
- 将内部结构体和函数放置在未命名命名空间中
- 未命名命名空间的作用域仅限于当前文件，外部无法访问

### 2. 内部结构体和函数的定义
- 在未命名命名空间中定义内部结构体
- 在未命名命名空间中定义内部函数，可引用内部结构体作为参数或返回值
- 内部函数可以被同一文件中的类成员函数或其他内部函数调用

### 3. 示例代码
```cpp
// 未命名命名空间，用于隔离内部实现
namespace {

// 内部结构体，不对外发布
struct InternalConfig {
    int max_modules;       // 最大模块数
    int thread_priority;   // 默认线程优先级
    std::string base_path; // 基础路径
};

// 内部函数，不对外发布
// 入参引用内部结构体，获取其中的一个字段
int getMaxModules(const InternalConfig& config) {
    return config.max_modules;
}

} // 未命名命名空间

// 在类成员函数中使用内部结构体和函数
bool Core::InitCore() {
    // 使用内部结构体
    InternalConfig config;
    config.max_modules = 10;
    
    // 调用内部函数
    int maxModules = getMaxModules(config);
    
    return true;
}
```

这种设计的优势：
- **封装性**：内部实现通过未命名命名空间隔离，外部无法访问
- **简洁性**：对外头文件中不需要包含这些内部定义，保持公共接口的简洁性
- **安全性**：避免了命名冲突和外部误用
- **可维护性**：内部实现集中在一个文件中，便于后续的修改和维护

## 设计优势总结
- **接口清晰度提升**：对外发布的头文件明确了模块的公共接口
- **编译依赖优化**：内部头文件修改不会触发其他模块的重新编译
- **封装性增强**：模块内部实现细节对外部不可见
- **代码维护性提高**：便于模块内部重构而不影响外部接口

# 代码规范
本项目严格遵守Google C++代码规范，以确保代码的一致性、可读性和可维护性。

## 代码规范文件
- **cpp_style_guide.md**：简化版的代码规范文档，提供了核心规则的概述，方便快速参考。
- **cpp_style_guide.txt**：完整的代码规范文档，包含了详细的规则说明、背景原因和最佳实践。

