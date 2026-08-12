#include "unistd.h"
#include "log.h"
#include "sys/wait.h"
int main(void){


    log_info("当前进程是:%d",getpid());

    //复制进程
    //pid==0: 代表当前是子进程
    //pid>0: 代表当前是父进程
    //pid<0: 出错
    int pid = fork();

    if(pid == 0){
        log_info("当前进程是:%d 父进程是:%d" ,getpid(),getppid());
        //exec: 执行 [注意: 第一个参数必须是文件名 参数必须以NULL结尾]
        //  l: list 列表[参数以列表的形式一个一个传递]
        //  v: vector 数组[所有参数放在数组中一起传递]
        //  e: Environment 创建一个环境变量数组传递给程序使用
        //  p: PATH 会获取环境变量PATH
        // execl("/home/atguigu/my_shell/hello_word.sh", NULL );
        //execl: 执行指定程序,并传入参数,参数以列表形式传入
        //execl("/bin/ls", "/bin/ls", "-l", "-a", NULL );

        //执行程序,参数以列表的形式传入,可以指定环境变量供执行程序使用
        //env环境变量也必须以NULL结尾
        // char* env[] = {"PATH=/bin:/home/atguigu/my_shell",NULL}; 
        // execle( "/bin/ls", "/bin/ls", "-l", "-a" , NULL , env);

        //执行指定程序,参数以列表形式传入,可以读取系统环境变量PATH
        // execlp("ls","ls","-l","-a",NULL);

        //执行指定程序,参数以字符串数组的形式传入
        // char* args[] = { "ls","-l","-a",NULL };
        // execv("/bin/ls",args);

        //执行指定程序,参数以字符串数组的形式传入,传入环境变量供执行程序使用
        // char* args[] = { "ls","-l","-a",NULL };
        // char* env[] = {"PATH=/bin:/home/atguigu/my_shell",NULL}; 
        // execve( "/bin/ls", args, env );

        char* args[] = { "mkdir","/xx/yy/zz",NULL };
        execvp("mkdir1", args);

        perror("不会被执行,只有出错才会执行");
    }else if(pid > 0 ){
        log_info("当前进程是:%d",getpid() );

        int status = 0;
        //等待子进程结束
        pid_t pt = waitpid( pid, &status, WUNTRACED);
        log_info("====================================================");
        log_info("status=%d  pt=%d",status,pt);
    }else{
        log_info("进程fork错误");
    }
}