#include "Common_Pool.h"
#include "Common_Config.h"
#include <stddef.h>
#include <fcntl.h>
#include <mqueue.h>
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define POOL_QUEUE_NAME "/pool_queue"

// 线程池全局变量
static pthread_t *threadPool = NULL; // 线程数组
static mqd_t mqid = -1;              // 消息队列描述符
static int pool_size = 0;            // 线程池大小

/**
 * @brief 线程的执行逻辑（每个线程从队列中取任务并执行）
 *
 * @param args 线程参数（这里未使用）
 * @return void*
 */
void *execTask(void *args) {
  Task task;
  while (1) {
    // ❌ size_t sz = mq_receive(mqid, (char *)&task, sizeof(Task), NULL);
    //    ← 问题：mq_receive返回ssize_t（有符号），用size_t（无符号）接收时，
    //      错误返回值-1会变成SIZE_MAX(18446744073709551615)，永远不等于sizeof(Task)
    // ✅ 正确写法：用 ssize_t
    ssize_t sz = mq_receive(mqid, (char *)&task, sizeof(Task), NULL);
    if (sz == sizeof(Task)) {
      if (task.Func != NULL) {
        task.Func(task.args);
      }
    }
  }
  return NULL;
}

/**
 * @brief 创建消息队列，用于存储待执行的任务
 *
 * @param size 队列最大消息数
 * @return ComStatus
 */
static ComStatus Common_Poll_CreateQueue(int size) {
  mq_unlink(POOL_QUEUE_NAME);

  // ❌ mq_attr attr  ← 类型名是 struct mq_attr，不是 mq_attr
  // ✅ 正确写法：
  struct mq_attr attr = {
      .mq_curmsgs = 0,
      .mq_flags = 0,
      .mq_maxmsg = size,
      .mq_msgsize = sizeof(Task),
  };

  // ❌ int mq_res = mq_open(...); if (mq_res == -1) { perror(...); }  ← 失败后没return COM_FAIL，继续往下走
  // ✅ 正确写法：
  mqd_t mq_res = mq_open(POOL_QUEUE_NAME, O_RDWR | O_CREAT, 0666, &attr);
  if (mq_res == -1) {
    perror("mq_open failed");
    return COM_FAIL;
  }

  // ❌ 缺少：把mq_res赋值给全局变量mqid
  // ✅ 补充：
  mqid = mq_res;
  return COM_OK;
}

/**
 * @brief 创建线程池
 *
 * @param size 线程池的线程数量
 * @return ComStatus
 */
ComStatus Common_Poll_Create(int size) {
  // ❌ if (size > 0) {  ← 逻辑反了，size>0是合法值才进入，应该是size<=0时报错
  // ✅ 正确写法：
  if (size <= 0) {
    return COM_FAIL;
  }

  pool_size = size;

  // ❌ Common_Poll_CreateQueue(size);  ← 返回值没检查
  // ✅ 正确写法：
  if (Common_Poll_CreateQueue(size) != COM_OK) {
    return COM_FAIL;
  }

  // ❌ malloc(sizeof(&threadPool));  ← sizeof(&threadPool)取的是指针的指针大小(8字节)，不是size * sizeof(pthread_t)
  //    threadPool是全局变量，malloc结果没赋值给它
  // ✅ 正确写法：
  threadPool = (pthread_t *)malloc(size * sizeof(pthread_t));
  if (threadPool == NULL) {
    return COM_FAIL;
  }

  // ❌ memset(threadPool, 0, sizeof(threadPool));  ← sizeof(threadPool)是8字节（指针大小），不是size * sizeof(pthread_t)
  // ✅ 正确写法：
  memset(threadPool, 0, size * sizeof(pthread_t));

  // ❌ 缺少：循环创建线程
  // ✅ 补充：
  for (int i = 0; i < size; i++) {
    if (pthread_create(&threadPool[i], NULL, execTask, NULL) != 0) {
      // ❌ 缺少：失败时清理已创建的资源
      // ✅ 补充：这里简化处理，返回失败
      return COM_FAIL;
    }
  }

  return COM_OK;
}

/**
 * @brief 向线程池添加任务
 *
 * @param task 待添加的任务
 * @return ComStatus
 */
ComStatus Common_Poll_AddTask(Task *task) {
  // ❌ 缺少：task为NULL的校验
  // ✅ 补充：
  if (task == NULL) {
    return COM_FAIL;
  }

  // ❌ if (mq_send(mqid, ...) == 0) { return COM_FAIL; } else { return COM_OK; }
  //    ← 逻辑反了！mq_send返回0表示成功，应该返回COM_OK；返回-1才是失败
  // ✅ 正确写法：
  if (mq_send(mqid, (char *)task, sizeof(Task), 0) == 0) {
    return COM_OK;
  } else {
    perror("mq_send failed");
    return COM_FAIL;
  }
}

/**
 * @brief 销毁线程池，回收所有资源
 */
void Common_Poll_Destory(void) {
  if (mqid != -1) {
    mq_close(mqid);
    mq_unlink(POOL_QUEUE_NAME);
    mqid = -1;
  }

  if (threadPool != NULL) {
    // ❌ for (size_t i; i < pool_size; i++)  ← i没初始化（未定义行为），且free和置NULL写在循环里会重复执行
    // ✅ 正确写法：
    for (int i = 0; i < pool_size; i++) {
      pthread_cancel(threadPool[i]);
      pthread_join(threadPool[i], NULL);
    }
    // ❌ free(threadPool); threadPool = NULL;  ← 应该放在循环外面，只执行一次
    // ✅ 正确写法：
    free(threadPool);
    threadPool = NULL;
  }
}
