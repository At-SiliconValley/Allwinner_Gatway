#include "pthread.h"
#include "log.h"
#include "unistd.h"

bool thread1_running = true,thread2_running = true;

/**
 * @brief 线程的执行函数
 * 
 * @param args 
 * @return void* 
 */
void * func1(void * args){

    //log_info("thread1 running...");

    while(1){
        log_info("thread1 running...");
        if( thread1_running == false ){
            pthread_exit((void*)11);
        }
        sleep(2);
    }

    return (void*)10;
}

void * func2(void * args){

    //log_info("thread2 running...");

    while(1){
        log_info("thread2 running...");
        if( thread2_running == false ){
            pthread_exit((void*)22);
        }
        sleep(2);
    }
    return (void*)20;
}
int main(void){

    //进程: 一个程序就是一个进程[ 有单独的内存空间 ]
    //线程: 一个执行单元[一个进程可以有多个线程, 多个线程共用所在进程的内存空间,每个线程栈内存是独立的]

    //pthread_create: 创建线程
    //pthread_join: 等待线程运行完成
    //pthread_cancel : 在线程外面调用,用于结束线程
    //pthread_exit: 在线程里面调用,线程主动退出

    pthread_t t1,t2;
    //TaskCreate( 任务名字, 任务堆栈大小,任务优先级, 执行函数, NULL, 句柄 )
    int res = pthread_create(&t1,NULL,func1, NULL );

    if(res != 0){
        log_info("线程1创建失败");
    }

    res = pthread_create(&t2,NULL,func2, NULL );
    if(res != 0){
        log_info("线程2创建失败");
    }

    char sw = 0;
    bool is_running = true;
    while(is_running){

        scanf("%c",&sw);
        log_info("sw=%c",sw);
        switch (sw)
        {
        case 'A':
            //结束线程1
            pthread_cancel(t1);
            break;
        case 'B':
            //结束线程2
            pthread_cancel(t2);
            break;
        case 'D':
            //让线程1主动退出
            thread1_running = false;
            break;
        case 'E':
            //让线程2主动退出
            thread2_running = false;
            break;
        case 'C':
            //退出while
            is_running = false;
            break;
        default:
            break;
        }
        scanf("%c",&sw);
    }

    //等待线程执行结束
    int a = 0, b = 0;
    pthread_join(t1, (void**)&a);
    pthread_join(t2, (void**)&b);
    log_info("a=%d b=%d",a,b);

    //while(1);
}