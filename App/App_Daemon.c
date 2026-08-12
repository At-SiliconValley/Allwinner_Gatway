#define _DEFAULT_SOURCE
#include "App_Daemon.h"

static bool is_running = true;
/**
 * @brief 进程检测
 *
 * @param pid 待检测的进程id
 * @param path 进程执行的程序路径
 * @param args 执行程序需要的参数
 */
static void App_Daemon_CheckProcess(pid_t *pid, char *path, char *args)
{

    log_info("开始检测 %s %s 进程", path, args);
    if (*pid != -1)
    {

        // 1、检测进程的状态
        int status = 0;
        pid_t pd = waitpid(*pid, &status, WNOHANG);

        if (pd == 0)
        {
            log_info("%s %s 进程正在运行中", path, args);
            return;
        }
        else if (pd == *pid)
        {
            log_info("%s %s 进程结束", path, args);
            *pid = -1;
        }
        else
        {
            log_info("%s %s 进程出错", path, args);
            *pid = -1;
        }
    }

    log_info("%s %s 进程不存在,准备启动进程", path, args);
    if (*pid == -1)
    {

        // 进程不存在了,重新启动进程
        pid_t p = fork();

        if (p == 0)
        {

            // 子进程,执行程序
            execl(path, path, args, NULL);
            //执行错误退出
            _exit( 1 );
        }
        else if (p > 0)
        {
            *pid = p;
        }
    }
}

void App_Daemon_Exit(int code)
{
    is_running = false;
}
void App_Daemon_Run(void)
{

    // 1、让当前进程变成守护进程
    // 第一个参数: 是否切换目录为根目录 [0-切换为根目录 1-不切换,是程序运行所在目录]
    // 第二参数:  是否将标准输入输出错误输出重定向到/dev/null
    daemon(0, 1);
    // 2、重定向标准输出和错误输出
    FILE *f3 = freopen("/dev/null", "r", stdin);
    FILE *f1 = freopen("/root/log.txt", "a", stdout);
    FILE *f2 = freopen("/root/log.txt", "a", stderr);

    // 3、信号捕获
    signal(SIGINT, App_Daemon_Exit);
    signal(SIGTERM, App_Daemon_Exit);
    // 4、进程守护
    pid_t otaPid = -1;
    pid_t appPid = -1;
    while (is_running)
    {

        App_Daemon_CheckProcess(&otaPid, "/usr/bin/gateway", "ota");
        App_Daemon_CheckProcess(&appPid, "/usr/bin/gateway", "app");
        sleep(2);
    }

    fclose(f1);
    fclose(f2);
    fclose(f3);

    if (otaPid != -1)
    {

        kill(otaPid, SIGTERM);
    }

    if (appPid != -1)
    {
        kill(appPid, SIGTERM);
    }
}