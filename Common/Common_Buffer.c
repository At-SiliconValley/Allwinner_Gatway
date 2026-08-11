#include "Common_Buffer.h"
#include "Common_Config.h"
#include "cJSON.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define RETURN_IF_ERROR(expr)                                                  \
  do {                                                                         \
    ComStatus _st = (expr);                                                    \
    if (_st != COM_OK) {                                                       \
      return _st                                                               \
    }                                                                          \
  } while (0)

/**
 * @brief 创建双缓冲
 */
ComStatus Common_Buffer_CreateDoubleBuffer(DoubleBuffer** buffer, uint16_t size)
{
    // ❌ if (size > 0) { return 0; }                    // 逻辑反了！size>0是合法值
    //    return 0 是int不是ComStatus，应该用 COM_FAIL
    // ✅
    if (size == 0) {
        return COM_FAIL;
    }

    // ❌ SubBuffer* subbuffer = (SubBuffer *)malloc(sizeof(SubBuffer));  // 这个变量多余
    // ✅ 直接创建读缓冲和写缓冲
    SubBuffer* readbuffer  = (SubBuffer *)malloc(sizeof(SubBuffer));
    SubBuffer* writebuffer = (SubBuffer *)malloc(sizeof(SubBuffer));
    if (readbuffer == NULL || writebuffer == NULL) {
        if (readbuffer != NULL) free(readbuffer);
        if (writebuffer != NULL) free(writebuffer);
        return COM_FAIL;
    }
    memset(readbuffer, 0, sizeof(SubBuffer));
    memset(writebuffer, 0, sizeof(SubBuffer));

    // ❌ malloc(sizeof(SubBuffer));                     // 返回值没接收，内存泄漏
    // ❌ malloc(sizeof(subbuffer.buf));                 // sizeof(指针)=8字节，不是size
    // ✅ 为读/写缓冲分配内部buf
    readbuffer->buf = (char *)malloc(size);
    writebuffer->buf = (char *)malloc(size);
    if (readbuffer->buf == NULL || writebuffer->buf == NULL) {
        if (readbuffer->buf != NULL) free(readbuffer->buf);
        if (writebuffer->buf != NULL) free(writebuffer->buf);
        free(readbuffer);
        free(writebuffer);
        return COM_FAIL;
    }

    // ❌ memset(subbuffer.buf, 0, sizeof(subbuffer.buf)); // buf是野指针！sizeof(buf)=8
    // ✅ 第三个参数是实际分配的size
    memset(readbuffer->buf, 0, size);
    memset(writebuffer->buf, 0, size);

    // ❌ subbuffer->size = size;                        // subbuffer是多余变量
    readbuffer->size = size;
    writebuffer->size = size;

    // ❌ DoubleBuffer doublebuffer;                    // 栈变量，函数返回就没了
    // ❌ malloc(sizeof(doublebuffer));                 // 返回值没接收
    // ✅ 堆分配
    DoubleBuffer* doublebuffer = (DoubleBuffer *)malloc(sizeof(DoubleBuffer));
    if (doublebuffer == NULL) {
        free(readbuffer->buf); free(writebuffer->buf);
        free(readbuffer); free(writebuffer);
        return COM_FAIL;
    }
    memset(doublebuffer, 0, sizeof(DoubleBuffer));

    // ❌ doublebuffer.buf_arr[0] = readBuffer;         // readBuffer未声明
    // ❌ doublebuffer.buf_arr[1] = writeBuffer;        // writeBuffer未声明
    doublebuffer->buf_arr[0] = readbuffer;
    doublebuffer->buf_arr[1] = writebuffer;
    doublebuffer->read_index = 0;
    doublebuffer->write_index = 1;

    // ❌ 缺少互斥锁初始化
    pthread_mutex_init(&doublebuffer->readLock, NULL);
    pthread_mutex_init(&doublebuffer->writeLock, NULL);

    *buffer = doublebuffer;
    return COM_OK;
}

/**
 * @brief 从指定缓冲中读取数据
 */
ComStatus Common_Buffer_Read(DoubleBuffer *buffer, char **datas,
                             uint16_t *size) {
  // ❌ 缺少参数校验
  if (buffer == NULL || datas == NULL || size == NULL) {
    return COM_FAIL;
  }

  *size = 0;
  *datas = NULL;

  pthread_mutex_lock(&buffer->readLock);

  // ❌ readbuffer = buffer->buf_arr[...];              // 没声明类型
  SubBuffer* readbuffer = buffer->buf_arr[buffer->read_index];   //
  //**
  // why buf_arr[buffer->read_index] instead of buf_arr[0] ?
  // when zhe read_index = 1,means a process is reading this arr,so :
  //        think about: 
  //            there is two buf_arr :    
  //          buf_arr[0]
  //          buf_arr[1]
  //      and:  
  //          when the read_index is 1, the buf_arr[0]     
  //  */

  // ❌ if () {                                        // 条件不完整
  if (readbuffer->used_len == 0) {
    pthread_mutex_lock(&buffer->writeLock);
    // ❌ 缺少交换读写索引
    buffer->read_index = !buffer->read_index;
    buffer->write_index = !buffer->write_index;
    readbuffer = buffer->buf_arr[buffer->read_index];

    if (readbuffer->used_len == 0) {
      pthread_mutex_unlock(&buffer->writeLock);
      pthread_mutex_unlock(&buffer->readLock);
      return COM_FAIL;
    }
    pthread_mutex_unlock(&buffer->writeLock);
  }

  *size = (readbuffer->buf[0] << 8) | readbuffer->buf[1];
  readbuffer->used_len -= 2;

  // ❌ 缺少 malloc datas 和 memcpy 数据
  *datas = (char *)malloc(*size + 1);
  if (*datas == NULL) {
    pthread_mutex_unlock(&buffer->readLock);
    return COM_FAIL;
  }
  memset(*datas, 0, *size + 1);
  memcpy(*datas, &readbuffer->buf[2], *size);

  memmove(readbuffer->buf, &readbuffer->buf[2 + *size], readbuffer->used_len);

  pthread_mutex_unlock(&buffer->readLock);
  return COM_OK;
}

/**
 * @brief 将数据写入指定缓冲
 */
ComStatus Common_Buffer_Write(DoubleBuffer *buffer, char *datas,
                              uint16_t size) {
  // ❌ 缺少参数校验
  if (buffer == NULL || datas == NULL || size == 0) {
    return COM_FAIL;
  }

  // ❌ pthread_mutex_lock(&buffer->buf_arr[buffer->write_index]);
  //    锁的是数组元素不是互斥锁！
  pthread_mutex_lock(&buffer->writeLock);

  // ❌ if (writeBuffer.size) {                        // writeBuffer未声明
  // ❌ buf                                            // 不完整语句
  SubBuffer* writeBuffer = buffer->buf_arr[buffer->write_index];

  if (writeBuffer->size - writeBuffer->used_len < size + 2) {
    pthread_mutex_unlock(&buffer->writeLock);
    return COM_FAIL;
  }

  writeBuffer->buf[writeBuffer->used_len]     = (size >> 8) & 0xFF;
  writeBuffer->buf[writeBuffer->used_len + 1] = size & 0xFF;
  writeBuffer->used_len += 2;
  memcpy(&writeBuffer->buf[writeBuffer->used_len], datas, size);
  writeBuffer->used_len += size;

  pthread_mutex_unlock(&buffer->writeLock);
  return COM_OK;
}

/**
 * @brief 回收缓冲资源
 */
void Common_Buffer_Destory(DoubleBuffer *buffer) {
  if (buffer == NULL) {
    return;
  }

  // ❌ free(buf_arr[0]->buf);                        // 不能直接访问buf_arr
  // ✅ 通过buffer指针访问
  free(buffer->buf_arr[0]->buf);
  free(buffer->buf_arr[0]);

  // ❌ free(buf_arr[1]->buf);
  free(buffer->buf_arr[1]->buf);
  free(buffer->buf_arr[1]);

  // ❌ pthread_mutex_destory();                       // 拼写错误destory→destroy，缺参数
  // ✅
  pthread_mutex_destroy(&buffer->writeLock);

  // ❌ pthread_mutex_destory(readLock);               // 拼写错误，readLock未声明
  // ✅
  pthread_mutex_destroy(&buffer->readLock);

  free(buffer);
}
