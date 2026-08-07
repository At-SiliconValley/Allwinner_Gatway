#include "pthread.h"
#include "mqueue.h"
#include "unistd.h"
#include "log.h"
pthread_t t1,t2;

mqd_t res;
/**
 * @brief 线程1执行函数[用于向队列中写入数据]
 * 
 * @param args 
 * @return void* 
 */
void *func1(void * args){

    while(1){
        log_info("准备发送数据...");
        mq_send(res,"hello",5,0);
        perror("发送完成...");
        sleep(4);
    }
}
/**
 * @brief 线程2执行函数[用于从队列中读取数据]
 * 
 * @param args 
 * @return void* 
 */
void *func2(void * args){

    //因为队列中指定了每条消息大小是1024,所以存储数据的数组大小应该>=1024[最少能存一条数据]
    char datas[1024] = {0};
    while(1){
        log_info("准备读取队列数据");
        ssize_t size = mq_receive(res, datas, 1024, NULL);
        perror("接收完成");
        if(size > 0){
            log_info("%s",datas);
        }
    }
}
int main(void){

    //创建队列
    //第一个参数: 队列的名称[必须以/开头],队列存储在/dev/mqueue目录
    //第二个参数: 指定操作模式[O_RDWR: 可以对队列做读写操作  O_CREATE:队列不存在则创建]
    //第三个参数: 当第二参数包含O_CREAT, 此时第三个参数代表权限[第一个0代表8进制]
    //第四个参数: 当第二参数包含O_CREAT,此时第四个参数代表队列的属性
    struct mq_attr attr = {
        .mq_curmsgs = 0, //当前队列中消息的个数
        .mq_maxmsg = 10, //队列中最大可以存储的消息个数[大小如果>/proc/sys/fs/mqueue/msg_max中的大小,此时要么使用sudo执行,要么改写/proc/sys/fs/mqueue/msg_max的值]
        .mq_msgsize = 1024, //队列中可以存储的单个消息最大大小[大小如果>/proc/sys/fs/mqueue/msgsize_max中的大小,此时要么使用sudo执行,要么改写/proc/sys/fs/mqueue/msgsize_max的值]
        .mq_flags = 0 //0-代表阻塞 1-代表付阻塞
    };
    res = mq_open("/my_queue",O_RDWR | O_CREAT, 0666,&attr);
    if(res == -1){
        perror("队列打开失败");
        return 1;
    }

    //创建两个线程
    pthread_create( &t1, NULL, func1, NULL );
    pthread_create( &t2, NULL, func2, NULL );

    //等待线程执行结束
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

    //关闭队列
    mq_unlink("/my_queue");
}
