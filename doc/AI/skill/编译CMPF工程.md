---
name: 编译 CMPF 工程
description: 指导如何在 WSL Ubuntu 环境中编译和测试 CMPF（Common Multi-Process Framework）工程
---

# 编译 CMPF 工程

## 描述

本技能用于指导如何在 WSL Ubuntu 环境中编译和测试 CMPF（Common Multi-Process Framework，通用多进程框架）工程。通过遵循本技能中的步骤，您可以快速、正确地完成编译过程，生成可执行文件并进行测试。

## 使用场景

- 需要编译 CMPF 工程以进行测试或部署
- 项目代码更新后需要重新编译
- 首次在新环境中搭建编译环境
- 需要清理构建目录并重新编译

## 指令

### 前置条件

1. **WSL Ubuntu 环境**：确保已安装并配置好 WSL Ubuntu 22.04
2. **用户权限**：确保当前用户为 amor
3. **依赖项**：确保安装了必要的编译工具（CMake 3.22.1、g++ 11.3.0、make 等）
4. **项目目录**：确保当前工作目录在 `Amor_CMPF` 目录下

### 编译步骤

#### 最常见场景：使用编译脚本进行编译与部署

使用项目提供的 `build_linux.sh` 脚本进行编译和部署：

```bash
# 编译并部署到指定目录
./build_linux.sh -d ./run_space
```

#### 场景 2：仅编译不部署

如果只需要编译而不需要部署：

```bash
# 仅编译
./build_linux.sh
```

#### 场景 3：清理构建目录并重新编译

如果需要清理构建目录并重新编译：

```bash
# 清理构建目录并重新编译
./build_linux.sh -C

# 编译并部署
./build_linux.sh -d ./run_space
```

#### 场景 4：清理输出目录并重新编译

如果只需要清理输出目录而保留构建文件：

```bash
# 清理输出目录并重新编译
./build_linux.sh -c -d ./run_space
```

### 运行服务

编译和部署完成后，可以启动 CMPF 服务：

```bash
# 进入部署目录
cd run_space

# 启动服务
sudo ./start_service.sh
```

### 服务管理命令

以下是常用的服务管理命令：

#### 启动服务

```bash
# 启动服务
sudo systemctl start cmpf_manager.service

# 检查服务状态（非交互式）
sudo systemctl is-active cmpf_manager.service
```

#### 停止服务

```bash
# 停止服务
sudo systemctl stop cmpf_manager.service

# 检查服务状态（非交互式）
sudo systemctl is-active cmpf_manager.service
```

#### 重启服务

```bash
# 重启服务
sudo systemctl restart cmpf_manager.service

# 检查服务状态（非交互式）
sudo systemctl is-active cmpf_manager.service
```

#### 禁用服务（开机不自动启动）

```bash
sudo systemctl disable cmpf_manager.service
```

#### 启用服务（开机自动启动）

```bash
sudo systemctl enable cmpf_manager.service
```

### 使用场景

1. **首次部署**：
   - 执行编译和部署命令
   - 运行 `sudo ./start_service.sh` 启动服务

2. **更新代码后重新部署**：
   - 停止当前服务：`sudo systemctl stop cmpf_manager.service`
   - 执行编译和部署命令
   - 启动服务：`sudo systemctl start cmpf_manager.service`
   或直接使用重启命令：`sudo systemctl restart cmpf_manager.service`

3. **服务异常需要重启**：
   - 使用重启命令：`sudo systemctl restart cmpf_manager.service`

4. **查看服务状态**：
   - 使用非交互式命令：`sudo systemctl is-active cmpf_manager.service`
   - 避免使用交互式命令：`systemctl status cmpf_manager.service`

### 重要提示

> **注意**：尽量不要使用交互式命令（如 `systemctl status cmpf_manager.service`），因为这些命令可能会在终端中卡住，需要手动退出。建议使用非交互式命令（如 `systemctl is-active cmpf_manager.service`）来检查服务状态。

### 常见问题与解决方案

#### 问题 1：部署目录不存在

**症状**：执行部署命令时出现 "Error: Deployment directory $deploy_dir does not exist." 错误

**解决方案**：
- 确保指定的部署目录存在
- 可以手动创建部署目录：`mkdir -p run_space`
- 或者在执行编译命令前先创建目录：`mkdir -p run_space && ./build_linux.sh -d ./run_space`

#### 问题 2：编译失败

**症状**：编译过程中出现语法错误或其他编译错误

**解决方案**：
- 检查代码是否有语法错误
- 查看编译日志中的具体错误信息
- 确保 WSL 环境有足够的权限
- 确保安装了所有必要的依赖库

#### 问题 3：测试执行失败

**症状**：编译成功但测试执行失败

**解决方案**：
- 检查测试文件是否存在：`build/output/bin/test_cmpf`
- 确保 LD_LIBRARY_PATH 环境变量设置正确
- 查看测试日志中的具体错误信息

#### 问题 4：服务启动失败 - 权限问题

**症状**：执行 `start_service.sh` 时出现 "command not found" 或 "需要root权限执行此脚本" 错误

**解决方案**：
- 确保脚本有执行权限：`chmod +x start_service.sh`
- 使用 sudo 执行脚本：`sudo ./start_service.sh`

#### 问题 5：服务启动失败 - 服务文件路径问题

**症状**：执行 `start_service.sh` 时出现 "cannot stat './cmpf_manager.service': No such file or directory" 错误

**解决方案**：
- 确保服务文件存在于 `run_space/service/` 目录中
- 检查 `start_service.sh` 脚本中的 `SERVICE_DIR` 变量是否正确设置为 `$SCRIPT_DIR/service`

#### 问题 6：服务启动失败 - systemd 配置错误

**症状**：执行 `start_service.sh` 后出现 "Unit cmpf_manager.service has a bad unit file setting" 错误

**解决方案**：
- 检查 `cmpf_manager.service` 文件中的 `ExecStart` 行格式是否正确
- 环境变量应该使用 `Environment` 指令设置，而不是直接在 `ExecStart` 行中设置
- 正确的配置示例：
  ```
  Environment=LD_LIBRARY_PATH=/opt/cmpf/lib
  ExecStart=/opt/cmpf/bin/manager
  ```

#### 问题 7：服务启动失败 - 其他原因

**症状**：执行 `start_service.sh` 后服务无法正常启动

**解决方案**：
- 检查 systemd 服务状态：`systemctl status cmpf_manager.service`
- 查看服务日志：`journalctl -u cmpf_manager.service`
- 确保当前用户有足够的权限执行 sudo 命令

### 清理构建目录

> **注意**：请谨慎使用此操作！只有在项目结构或依赖发生重大变化，导致增量编译失败时，才需要执行完全重新构建。

如果需要完全重新构建，可以执行：

```bash
# 进入项目目录
cd /home/amor/code/cmpf_dev_2/Amor_CMPF

# 清理构建目录
./build_linux.sh -C

# 重新编译并部署
./build_linux.sh -d ./run_space
```

## 示例

### 示例 1：常规编译与部署流程

```bash
# 编译并部署
./build_linux.sh -d ./run_space

# 启动服务
cd run_space
sudo ./start_service.sh
```

### 示例 2：清理构建目录并重新编译

```bash
# 清理构建目录
./build_linux.sh -C

# 重新编译并部署
./build_linux.sh -d ./run_space

# 启动服务
cd run_space
sudo ./start_service.sh
```

### 示例 3：仅编译不部署

```bash
# 仅编译
./build_linux.sh

# 查看编译结果
ls -la build/output/
```

## 输出结果

编译成功后，可执行文件和库文件会生成在 `build/output` 目录中，具体结构如下：

```
build/output/
├── bin/
│   ├── manager
│   ├── worker
│   ├── test_cmpf (如果编译了测试)
├── lib/
│   ├── libcmpf_xxx.so (各种模块动态链接文件)
```

部署后，文件会复制到指定的部署目录，结构如下：

```
run_space/
├── bin/
│   ├── manager
│   ├── worker
├── service/
│   ├── cmpf_manager.service
├── lib/
│   ├── libcmpf_xxx.so
├── start_service.sh
├── cmpf.conf (配置文件)
```

> **重要提示**：`run_space` 只是临时运行目录，任何修改都应该先修改源代码（如 `scripts/` 目录下的文件），然后执行编译部署脚本进行应用修改。直接修改 `run_space` 目录中的文件不会持久化，下次编译部署时会被覆盖。

您可以通过以下命令验证编译结果：

```bash
# 检查可执行文件是否存在
ls -la build/output/bin/

# 查看可执行文件信息
file build/output/bin/manager
file build/output/bin/worker
```