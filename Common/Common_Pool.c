#include "Common_Pool.h"

#define POOL_QUEUE_NAME "/pool_queue"

// 线程池全局变量
static pthread_t* threadPool = NULL;  // 线程数组
static mqd_t mqid = -1;              // 消息队列描述符
static int pool_size = 0;            // 线程池大小

/**
 * @brief 线程的执行逻辑（每个线程从队列中取任务并执行）
 * 
 * @param args 线程参数（这里未使用）
 * @return void* 
 * 
 * 实现步骤：
 *   1、定义 Task task 变量
 *   2、while(1) 死循环：
 *      a. 调用 mq_receive(mqid, (char*)&task, sizeof(Task), NULL) 从队列取任务
 *      b. 如果收到的字节数 == sizeof(Task)：
 *         - 如果 task.Func 不为 NULL，执行 task.Func(task.args)
 */
void* execTask(void* args)
{
    // TODO: 按照上述步骤实现线程执行逻辑

    return NULL;
}

/**
 * @brief 创建消息队列，用于存储待执行的任务
 * 
 * @param size 队列最大消息数
 * @return ComStatus 
 * 
 * 实现步骤：
 *   1、mq_unlink(POOL_QUEUE_NAME) 先销毁可能存在的旧队列
 *   2、定义 struct mq_attr attr：
 *      .mq_curmsgs = 0          // 当前消息数
 *      .mq_flags = 0            // 阻塞模式
 *      .mq_maxmsg = size        // 最大消息数
 *      .mq_msgsize = sizeof(Task)  // 每条消息大小
 *   3、mq_open(POOL_QUEUE_NAME, O_RDWR | O_CREAT, 0666, &attr) 创建队列
 *   4、如果返回 -1，perror 打印错误，返回 COM_FAIL
 *   5、将返回值赋给全局变量 mqid，返回 COM_OK
 */
static ComStatus Common_Poll_CreateQueue(int size)
{
    // TODO: 按照上述步骤实现消息队列创建

    return COM_FAIL; // 临时返回值
}

/**
 * @brief 创建线程池
 * 
 * @param size 线程池的线程数量
 * @return ComStatus 
 * 
 * 实现步骤：
 *   1、参数校验：size 必须 > 0
 *   2、保存 pool_size = size
 *   3、调用 Common_Poll_CreateQueue(size) 创建任务队列
 *   4、malloc 申请 threadPool = size * sizeof(pthread_t)，memset 清零
 *   5、循环 size 次，pthread_create 创建线程，执行 execTask 函数
 *   6、每一步失败都要清理已申请资源（关闭队列、释放线程池内存）
 *   7、返回 COM_OK
 */
ComStatus Common_Poll_Create(int size)
{
    // TODO: 按照上述步骤实现线程池创建

    return COM_FAIL; // 临时返回值
}

/**
 * @brief 向线程池添加任务
 * 
 * @param task 待添加的任务
 * @return ComStatus 
 * 
 * 实现步骤：
 *   1、参数校验：task 不能为 NULL
 *   2、调用 mq_send(mqid, (char*)task, sizeof(Task), 0) 发送任务到队列
 *   3、如果返回 0 表示成功，返回 COM_OK
 *   4、否则 perror 打印错误，返回 COM_FAIL
 */
ComStatus Common_Poll_AddTask(Task* task)
{
    // TODO: 按照上述步骤实现任务添加

    return COM_FAIL; // 临时返回值
}

/**
 * @brief 销毁线程池，回收所有资源
 * 
 * 实现步骤：
 *   1、如果 mqid != -1：
 *      a. mq_close(mqid) 关闭队列
 *      b. mq_unlink(POOL_QUEUE_NAME) 删除队列
 *      c. mqid = -1
 *   2、如果 threadPool 不为 NULL：
 *      a. 循环 pool_size 次：
 *         - pthread_cancel(threadPool[i]) 取消线程
 *         - pthread_join(threadPool[i], NULL) 等待线程结束
 *      b. free(threadPool) 释放线程数组内存
 *      c. threadPool = NULL
 */
void Common_Poll_Destory(void)
{
    // TODO: 按照上述步骤实现资源回收
}