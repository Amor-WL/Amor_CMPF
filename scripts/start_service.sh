#!/bin/bash

# 定义变量
SCRIPT_DIR="$(dirname "$0")"
SERVICE_DIR="$SCRIPT_DIR"
LIB_DIR="$(pwd)/lib"
BIN_DIR="$(pwd)/bin"
SYSTEMD_DIR="/usr/lib/systemd/system"
WORKING_DIR="/opt/cmpf"
WORKING_LIB_DIR="$WORKING_DIR/lib"
WORKING_BIN_DIR="$WORKING_DIR/bin"
WORKING_CONFIG_DIR="$WORKING_DIR/config"
CONFIG_FILE="$WORKING_CONFIG_DIR/cmpf.conf"
USER="cmpf"
GROUP="cmpf"

# 检查是否以root用户运行
check_root() {
    if [ "$(id -u)" -ne 0 ]; then
        echo "错误：需要root权限执行此脚本"
        exit 1
    fi
}

# 创建cmpf用户和组
create_user_group() {
    echo "正在检查/创建cmpf用户和组..."
    # 检查组是否存在，如果不存在则创建
    if ! getent group "$GROUP" > /dev/null; then
        groupadd -r "$GROUP"
        echo "已创建组: $GROUP"
    else
        echo "组: $GROUP 已存在"
    fi
    
    # 检查用户是否存在，如果不存在则创建
    if ! getent passwd "$USER" > /dev/null; then
        useradd -r -g "$GROUP" -d /var/lib/cmpf -s /sbin/nologin "$USER"
        echo "已创建用户: $USER"
    else
        echo "用户: $USER 已存在"
    fi
}

# 创建工作目录
create_working_dir() {
    echo "正在创建工作目录..."
    mkdir -p "$WORKING_DIR"
    mkdir -p "$WORKING_LIB_DIR"
    mkdir -p "$WORKING_BIN_DIR"
    mkdir -p "$WORKING_CONFIG_DIR"
    
    # 设置目录权限
    chown -R "$USER:$GROUP" "$WORKING_DIR"
    chmod -R 755 "$WORKING_DIR"
}

# 部署库文件
deploy_libs() {
    echo "正在部署库文件..."
    cp -f "$LIB_DIR"/* "$WORKING_LIB_DIR/"
    # 设置权限
    chown -R "$USER:$GROUP" "$WORKING_LIB_DIR"
    chmod -R 755 "$WORKING_LIB_DIR"
}

# 部署二进制文件
deploy_binaries() {
    echo "正在部署二进制文件..."
    cp -f "$BIN_DIR"/* "$WORKING_BIN_DIR/"
    # 设置权限
    chown -R "$USER:$GROUP" "$WORKING_BIN_DIR"
    chmod -R 755 "$WORKING_BIN_DIR"
}

# 部署服务文件
deploy_services() {
    echo "正在部署服务文件..."
    
    # 部署配置文件
    echo "正在部署配置文件..."
    mkdir -p "$WORKING_CONFIG_DIR"
    cp -f "$SCRIPT_DIR/cmpf.conf" "$CONFIG_FILE"
    chown -R "$USER:$GROUP" "$WORKING_CONFIG_DIR"
    chmod -R 644 "$CONFIG_FILE"
    
    # 部署服务文件
    cp -f "$SERVICE_DIR/cmpf_manager.service" "$SYSTEMD_DIR/"
    systemctl daemon-reload
}

# 启动manager服务
start_manager() {
    echo "正在启动cmpf_manager服务..."
    systemctl start cmpf_manager.service
    systemctl enable cmpf_manager.service
}

# 停止manager服务
stop_manager() {
    echo "正在停止cmpf_manager服务..."
    systemctl stop cmpf_manager.service
}

# 重启manager服务
restart_manager() {
    echo "正在重启cmpf_manager服务..."
    systemctl restart cmpf_manager.service
}

# 卸载服务
uninstall_services() {
    echo "正在卸载服务..."
    systemctl disable cmpf_manager.service
    rm -f "$SYSTEMD_DIR/cmpf_manager.service"
    systemctl daemon-reload
    
    # 卸载工作目录
    echo "正在卸载工作目录..."
    rm -rf "$WORKING_DIR"
}

# 删除库文件
remove_libs() {
    # 库文件现在在工作目录中，由uninstall_services函数统一删除
    echo "库文件将随工作目录一起删除..."
}

# 主函数
main() {
    check_root
    
    case "$1" in
        --stop)
            stop_manager
            ;;
        --restart)
            restart_manager
            ;;
        --uninstall)
            stop_manager
            uninstall_services
            remove_libs
            ;;
        *)
            create_user_group
            create_working_dir
            deploy_libs
            deploy_binaries
            deploy_services
            stop_manager
            start_manager
            ;;
    esac
    
    echo "操作完成"
}

# 执行主函数
main "$@"
