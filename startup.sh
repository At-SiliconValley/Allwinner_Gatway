#! /bin/sh
# 网关自启/OTA切换脚本
# 用法: xxx.sh start|stop|restart
# 功能: 启动时自动检测新固件并切换，停止时kill进程，重启时先停后启

# 定义路径常量
# NEW_PROCESS=/usr/bin/gateway.update   — OTA下载的新固件路径
# OLD_PROCESS=/usr/bin/gateway          — 当前运行中的固件路径
# TMP_PROCESS=/usr/bin/gateway.bak     — 旧固件备份路径

# 1. 参数校验：判断 $# 是否小于1，小于1则 echo 提示并 exit

# 2. 定义 start_process 函数：
#    a. 用 [ -e $NEW_PROCESS ] 判断新固件文件是否存在
#    b. 如果存在：mv $OLD_PROCESS $TMP_PROCESS  — 备份旧固件
#    c. mv $NEW_PROCESS $OLD_PROCESS             — 新固件替换旧固件
#    d. chmod +x $OLD_PROCESS                    — 添加执行权限
#    e. 用 ps -ef | grep "$OLD_PROCESS daemon" | grep -v grep | awk '{print $1}' | xargs kill 杀死旧守护进程
#    f. 执行 $OLD_PROCESS daemon 启动守护进程
#    g. 用 $? 检查上一步执行结果，如果 != 0 说明启动失败：
#       - mv $OLD_PROCESS $NEW_PROCESS  — 恢复新固件
#       - mv $TMP_PROCESS $OLD_PROCESS  — 恢复旧固件
#       如果成功：rm -rf $TMP_PROCESS   — 删除备份

# 3. 定义 stop_process 函数：
#    用 ps -ef | grep "$OLD_PROCESS daemon" | grep -v grep | awk '{print $1}' | xargs kill 杀死守护进程

# 4. 定义 restart_process 函数：
#    先调用 stop_process 再调用 start_process

# 5. 用 case $1 in 根据参数分发：
#    "start")   → start_process ;;
#    "stop")    → stop_process ;;
#    "restart") → restart_process ;;
#    *)         → echo "参数传入错误" ;;