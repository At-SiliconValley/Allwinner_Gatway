#define _DEFAULT_SOURCE
#include "App_Daemon.h"

// 守护进程运行标志
static bool is_running = true;

/**
 * @brief 检测并守护一个子进程
 *        如果进程不存在或已退出，则重新fork启动
 *
 * @param pid  子进程ID的指针（-1表示进程不存在）
 * @param path 可执行程序的路径
 * @param args 传递给程序的参数
 */
static void App_Daemon_CheckProcess(pid_t *pid, char *path, char *args)
{
    // 1. log_info 打印"开始检测 %s %s 进程"，传入 path 和 args

    // 2. 如果 *pid != -1（说明之前启动过这个进程，需要检测它是否还活着）：
    //    a. 声明 int status = 0
    //    b. 调用 waitpid(*pid, &status, WNOHANG) 非阻塞检测进程状态
    //       WNOHANG 表示不等待，立即返回
    //    c. 判断返回值：
    //       - 如果 pd == 0：进程还在运行
    //         → log_info 打印"进程正在运行中" → return
    //       - 如果 pd == *pid：进程已正常结束
    //         → log_info 打印"进程结束" → *pid = -1
    //       - 其他情况：进程异常
    //         → log_info 打印"进程出错" → *pid = -1

    // 3. 走到这里说明进程不存在（*pid == -1）
    //    log_info 打印"进程不存在,准备启动进程"

    // 4. 调用 fork() 创建子进程
    //    a. 如果返回值 == 0：当前是子进程
    //       - 调用 execl(path, path, args, NULL) 执行目标程序
    //         注意：execl 第一个参数是路径，第二个起是 argv[0], argv[1]...
    //         NULL 表示参数结束
    //       - 如果 execl 返回说明执行失败，调用 _exit(1) 退出子进程
    //    b. 如果返回值 > 0：当前是父进程
    //       - *pid = 返回值（保存子进程ID）
}

/**
 * @brief 信号处理函数，优雅退出守护进程
 *
 * @param code 信号编号
 */
void App_Daemon_Exit(int code)
{
    // 把 is_running 设为 false，主循环检测到后退出
}

/**
 * @brief 守护进程主入口
 *
 *        1. 将当前进程转为守护进程（后台运行）
 *        2. 重定向标准输入输出到日志文件
 *        3. 循环监控 ota 和 app 两个子进程，挂了就拉起来
 */
void App_Daemon_Run(void)
{
    // 1. 调用 daemon(0, 1) 将当前进程变为守护进程
    //    第一个参数 0：切换工作目录到根目录 /
    //    第二个参数 1：将标准输入输出错误重定向到 /dev/null

    // 2. 重定向标准输入输出，把日志写到文件：
    //    a. freopen("/dev/null", "r", stdin)   — 标准输入重定向到空设备
    //    b. freopen("/root/log.txt", "a", stdout) — 标准输出追加到日志文件
    //    c. freopen("/root/log.txt", "a", stderr) — 标准错误追加到日志文件

    // 3. 注册信号处理函数：
    //    signal(SIGINT, App_Daemon_Exit)   — 捕获 Ctrl+C
    //    signal(SIGTERM, App_Daemon_Exit)  — 捕获 kill 命令

    // 4. 初始化两个子进程ID为 -1（表示尚未启动）：
    //    pid_t otaPid = -1  — OTA升级进程
    //    pid_t appPid = -1  — 应用主进程

    // 5. 主循环 while(is_running)：
    //    a. 调用 App_Daemon_CheckProcess(&otaPid, "/usr/bin/gateway", "ota") 守护OTA进程
    //    b. 调用 App_Daemon_CheckProcess(&appPid, "/usr/bin/gateway", "app") 守护APP进程
    //    c. sleep(2) 休眠2秒后再检查

    // 6. 退出循环后（收到信号），清理资源：
    //    a. fclose(f1) / fclose(f2) / fclose(f3) 关闭日志文件
    //    b. 如果 otaPid != -1 → kill(otaPid, SIGTERM) 终止OTA子进程
    //    c. 如果 appPid != -1 → kill(appPid, SIGTERM) 终止APP子进程
}