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

# 代码规范
本项目严格遵守Google C++代码规范，以确保代码的一致性、可读性和可维护性。

## 代码规范文件
- **cpp_style_guide.md**：简化版的代码规范文档，提供了核心规则的概述，方便快速参考。
- **cpp_style_guide.txt**：完整的代码规范文档，包含了详细的规则说明、背景原因和最佳实践。

## 提交代码前的检查
在提交代码前，请确保：
1. 代码符合Google C++代码规范的要求
2. 所有修改都已通过编译和测试
3. 注释清晰、准确，能够帮助其他开发者理解代码

通过严格遵守代码规范，可以提高代码质量，减少bug，使团队协作更加高效。
