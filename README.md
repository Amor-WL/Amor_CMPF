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
./build_linux.sh -d ./run_space
cd run_space
sudo ./start_service.sh

# 执行
在执行了上述的部署命令后，在部署路径下会生成以下文件：
run_space/
├── bin/
│   ├── manager
│   ├── worker
├── service/
│   ├── cmpf_manager.service
├── lib/
│   ├── libxxxxx.so
├── start_service.sh

随后用户执行sudo ./start_service.sh脚本即可启动CMPF框架。
该脚本会往systemd服务部署目录下拷贝service与target文件，随后将二进制文件部署到系统目录下。
随后执行systemctl start命令启动manager服务，并期望通过target文件来启动worker服务。

# 框架整体部署架构
当前的项目是个多进程框架，worker模型。使用systemd进行部署管理。
1. 二进制文件：manager、worker各一个。libcmpf_xxx.so等模块动态链接文件若干。
2. 进程：manager一个，agent一个，worker可能有多个（进程名worker_1, worker_2），worker数量由配置文件决定。
3. 依赖关系：worker依赖manager
4. 所有进程都被systemd管理。
5. worker负责具体的业务功能。manager只负责进程管理。
6. 在manager中，负责订阅worker的进程信息，在内部有自己的状态机。比如，当发现同一个worker号存在两个实例时，会杀死旧的。或者当worker重复重启超过15次则结束所有服务。负责类似的功能。（当前还未实现）

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
