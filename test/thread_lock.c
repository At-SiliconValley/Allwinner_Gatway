#include "pthread.h"
#include "unistd.h"
#include "log.h"
pthread_t t1 , t2;
int sum = 0;

pthread_mutex_t lock;
void *func1(void * args){

    for(int i = 1; i<=20000;i++){
        //上锁
        pthread_mutex_lock(&lock);
        sum++;
        //释放锁
        pthread_mutex_unlock(&lock);
    }
}


int main(void){

    //创建线程锁
    pthread_mutex_init(&lock,NULL);
    //存在多线程的时候,如果多个线程同时对某一个共享资源做写操作会出现线程不安全问题,所以需要再每次使用共享资源的时候上锁,使用完之后释放锁,避免线程安全问题
    pthread_create(&t1,NULL, func1, NULL);
    pthread_create(&t2,NULL, func1, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    log_info("sum=%d",sum);
}

