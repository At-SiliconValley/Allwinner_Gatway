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

static pthread_t *threadPool = NULL;
static mqd_t mqid = -1;
static int pool_size = 0;

/**
 * @brief 线程的执行逻辑
 */
void *execTask(void *args) {
  Task task;
  while (1) {
    // ❌ size_t sz = mq_receive(mqid, (char *)&task, sizeof(Task), NULL);
    //    mq_receive返回ssize_t(有符号)，用size_t(无符号)接收：
    //    错误返回值-1变成SIZE_MAX，永远不等于sizeof(Task)
    // ✅ 用 ssize_t
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
 * @brief 创建消息队列
 */
static ComStatus Common_Poll_CreateQueue(int size) {
  mq_unlink(POOL_QUEUE_NAME);

  // ❌ mq_attr attr = { ... };                        // 类型名是 struct mq_attr，不是 mq_attr
  // ✅
  struct mq_attr attr = {
      .mq_curmsgs = 0,
      .mq_flags = 0,
      .mq_maxmsg = size,
      .mq_msgsize = sizeof(Task),
  };

  // ❌ int mq_res = mq_open(...); if (mq_res == -1) { perror(...); }
  //    失败后没return COM_FAIL，继续往下走了
  // ✅
  mqd_t mq_res = mq_open(POOL_QUEUE_NAME, O_RDWR | O_CREAT, 0666, &attr);
  if (mq_res == -1) {
    perror("mq_open failed");
    return COM_FAIL;
  }

  // ❌ 缺少：把mq_res赋值给全局变量mqid
  mqid = mq_res;
  return COM_OK;
}

/**
 * @brief 创建线程池
 */
ComStatus Common_Poll_Create(int size) {
  // ❌ if (size > 0) {                                // 逻辑反了，size<=0才应该报错
  // ✅
  if (size <= 0) {
    return COM_FAIL;
  }

  pool_size = size;

  // ❌ Common_Poll_CreateQueue(size);                 // 返回值没检查
  if (Common_Poll_CreateQueue(size) != COM_OK) {
    return COM_FAIL;
  }

  // ❌ malloc(sizeof(&threadPool));                   // sizeof(&threadPool)=8(指针的指针大小)
  //    malloc结果没赋值给threadPool
  // ✅
  threadPool = (pthread_t *)malloc(size * sizeof(pthread_t));
  if (threadPool == NULL) {
    return COM_FAIL;
  }

  // ❌ memset(threadPool, 0, sizeof(threadPool));     // sizeof(threadPool)=8不是size*sizeof(pthread_t)
  // ✅
  memset(threadPool, 0, size * sizeof(pthread_t));

  // ❌ 缺少：循环 pthread_create 创建线程
  for (int i = 0; i < size; i++) {
    if (pthread_create(&threadPool[i], NULL, execTask, NULL) != 0) {
      return COM_FAIL;
    }
  }

  return COM_OK;
}

/**
 * @brief 向线程池添加任务
 */
ComStatus Common_Poll_AddTask(Task *task) {
  // ❌ 缺少 task == NULL 校验
  if (task == NULL) {
    return COM_FAIL;
  }

  // ❌ if (mq_send(...) == 0) { return COM_FAIL; } else { return COM_OK; }
  //    逻辑反了！mq_send返回0=成功，返回-1=失败
  // ✅
  if (mq_send(mqid, (char *)task, sizeof(Task), 0) == 0) {
    return COM_OK;
  } else {
    perror("mq_send failed");
    return COM_FAIL;
  }
}

/**
 * @brief 销毁线程池
 */
void Common_Poll_Destory(void) {
  if (mqid != -1) {
    mq_close(mqid);
    mq_unlink(POOL_QUEUE_NAME);
    mqid = -1;
  }

  if (threadPool != NULL) {
    // ❌ for (size_t i; i < pool_size; i++)            // i没初始化（未定义行为）
    // ❌     free(threadPool); threadPool = NULL;      // 写在循环里会重复执行
    // ✅
    for (int i = 0; i < pool_size; i++) {
      pthread_cancel(threadPool[i]);
      pthread_join(threadPool[i], NULL);
    }
    free(threadPool);
    threadPool = NULL;
  }
}
