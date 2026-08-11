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
 *
 * @param buffer 创建的缓冲的指针（输出参数）
 * @param size   每个子缓冲的大小
 * @return ComStatus COM_OK 成功 / COM_FAIL 失败
 *
 * 实现步骤：
 *   1、校验参数：size 必须 > 0
 *   2、创建读缓冲 SubBuffer：
 *     2.1、malloc 申请 SubBuffer 内存，memset 清零
 *     2.2、malloc 申请读缓冲内部 buf（size 字节），memset 清零
 *     2.3、设置 readBuffer->size = size
 *   3、创建写缓冲 SubBuffer（同上步骤）
 *   4、创建 DoubleBuffer：
 *     4.1、malloc 申请 DoubleBuffer 内存，memset 清零
 *     4.2、buf_arr[0] = readBuffer, buf_arr[1] = writeBuffer
 *     4.3、read_index = 0, write_index = 1
 *   5、创建互斥锁：
 *     5.1、pthread_mutex_init 创建 readLock
 *     5.2、pthread_mutex_init 创建 writeLock
 *   6、*buffer = doubleBuffer，返回 COM_OK
 *
 * 注意：每一步失败都要释放已申请的资源，防止内存泄漏
 */
ComStatus Common_Buffer_CreateDoubleBuffer(DoubleBuffer** buffer, uint16_t size)
{
    // ❌ if (size > 0) { return 0; }   ← 逻辑反了！size>0是合法值，不应该return；return 0是int不是ComStatus
    // ✅ 正确写法：
    if (size == 0) {
        return COM_FAIL;
    }

    // ❌ SubBuffer* subbuffer = (SubBuffer *)malloc(sizeof(SubBuffer));  ← 这个变量多余，后面没用
    // ✅ 直接创建读缓冲和写缓冲两个指针：
    SubBuffer* readbuffer  = (SubBuffer *)malloc(sizeof(SubBuffer));
    SubBuffer* writebuffer = (SubBuffer *)malloc(sizeof(SubBuffer));
    if (readbuffer == NULL || writebuffer == NULL) {
        // ❌ 缺少：malloc失败要释放已成功的那个
        // ✅ 补充：
        if (readbuffer != NULL) free(readbuffer);
        if (writebuffer != NULL) free(writebuffer);
        return COM_FAIL;
    }
    memset(readbuffer, 0, sizeof(SubBuffer));
    memset(writebuffer, 0, sizeof(SubBuffer));

    // ❌ subbuffer->buf = (char *)malloc(size);  ← subbuffer是多余的变量，应该用readbuffer/writebuffer
    readbuffer->buf = (char *)malloc(size);
    writebuffer->buf = (char *)malloc(size);
    if (readbuffer->buf == NULL || writebuffer->buf == NULL) {
        // ❌ 缺少：buf分配失败要释放已分配的资源
        if (readbuffer->buf != NULL) free(readbuffer->buf);
        if (writebuffer->buf != NULL) free(writebuffer->buf);
        free(readbuffer);
        free(writebuffer);
        return COM_FAIL;
    }

    // ❌ memset(subbuffer.buf, 0, sizeof(subbuffer.buf));  ← subbuffer不是指针，.buf访问错误；sizeof(subbuffer.buf)是8字节（指针大小）不是size
    // ✅ memset的第三个参数应该是size（实际分配的buf大小）：
    memset(readbuffer->buf, 0, size);
    memset(writebuffer->buf, 0, size);

    // ❌ subbuffer->size = size;  ← subbuffer是多余变量
    readbuffer->size = size;
    writebuffer->size = size;

    // ❌ DoubleBuffer doublebuffer;  ← 栈变量，函数返回就没了；应该用malloc分配堆内存
    // ✅ 正确写法：
    DoubleBuffer* doublebuffer = (DoubleBuffer *)malloc(sizeof(DoubleBuffer));
    if (doublebuffer == NULL) {
        free(readbuffer->buf);
        free(writebuffer->buf);
        free(readbuffer);
        free(writebuffer);
        return COM_FAIL;
    }
    memset(doublebuffer, 0, sizeof(DoubleBuffer));

    // ❌ doublebuffer.buf_arr[0] = readbuffer;  ← doublebuffer是栈变量时用.访问，但前面应改为指针
    // ✅ 指针用->访问：
    doublebuffer->buf_arr[0] = readbuffer;
    doublebuffer->buf_arr[1] = writebuffer;
    doublebuffer->read_index = 0;
    doublebuffer->write_index = 1;

    // ❌ 缺少：互斥锁初始化
    // ✅ 补充：
    pthread_mutex_init(&doublebuffer->readLock, NULL);
    pthread_mutex_init(&doublebuffer->writeLock, NULL);

    // ❌ 缺少：把创建好的doublebuffer赋值给输出参数
    // ✅ 补充：
    *buffer = doublebuffer;

    return COM_OK;
}

/**
 * @brief 从指定缓冲中读取数据
 *
 * @param buffer 待读取数据的缓冲
 * @param datas  读取到的数据（输出参数，内部 malloc，调用者需 free）
 * @param size   读取到的数据大小（输出参数）
 * @return ComStatus COM_OK 成功 / COM_FAIL 失败
 */
ComStatus Common_Buffer_Read(DoubleBuffer *buffer, char **datas,
                             uint16_t *size) {
  // ❌ 缺少：参数校验
  // ✅ 补充：
  if (buffer == NULL || datas == NULL || size == NULL) {
    return COM_FAIL;
  }

  *size = 0;
  *datas = NULL;

  pthread_mutex_lock(&buffer->readLock);

  // ❌ readbuffer = buffer->buf_arr[buffer->read_index];  ← readbuffer没声明类型
  // ✅ 正确写法：
  SubBuffer* readbuffer = buffer->buf_arr[buffer->read_index];

  // ❌ if () {  ← 条件不完整
  // ✅ 判断当前读缓冲是否有数据：
  if (readbuffer->used_len == 0) {
    // 当前读缓冲空了，交换读写缓冲
    pthread_mutex_lock(&buffer->writeLock);
    // ❌ 缺少：交换读写索引
    // ✅ 补充：交换read_index和write_index
    buffer->read_index = !buffer->read_index;
    buffer->write_index = !buffer->write_index;
    readbuffer = buffer->buf_arr[buffer->read_index];

    if (readbuffer->used_len == 0) {
      // ❌ 缺少：交换后还是空的，释放锁返回
      pthread_mutex_unlock(&buffer->writeLock);
      pthread_mutex_unlock(&buffer->readLock);
      return COM_FAIL;
    }
    pthread_mutex_unlock(&buffer->writeLock);
  }

  // 读取协议头2字节（长度）
  *size = (readbuffer->buf[0] << 8) | readbuffer->buf[1];
  readbuffer->used_len -= 2;

  // ❌ 缺少：malloc datas并拷贝数据
  // ✅ 补充：
  *datas = (char *)malloc(*size + 1);
  if (*datas == NULL) {
    pthread_mutex_unlock(&buffer->readLock);
    return COM_FAIL;
  }
  memset(*datas, 0, *size + 1);
  memcpy(*datas, &readbuffer->buf[2], *size);

  // ❌ 缺少：把剩余数据前移（memmove）
  // ✅ 补充：
  memmove(readbuffer->buf, &readbuffer->buf[2 + *size], readbuffer->used_len);

  pthread_mutex_unlock(&buffer->readLock);
  return COM_OK;
}

/**
 * @brief 将数据写入指定缓冲
 *
 * @param buffer 待写入数据的缓冲
 * @param datas  待写入数据
 * @param size   数据大小
 * @return ComStatus COM_OK 成功 / COM_FAIL 失败
 */
ComStatus Common_Buffer_Write(DoubleBuffer *buffer, char *datas,
                              uint16_t size) {
  // ❌ 缺少参数校验：buffer / datas / size
  // ✅ 补充：
  if (buffer == NULL || datas == NULL || size == 0) {
    return COM_FAIL;
  }

  // ❌ pthread_mutex_lock(&buffer->buf_arr[buffer->write_index]);  ← 锁的是buf_arr元素，不是互斥锁！
  // ✅ 应该锁 writeLock 互斥锁：
  pthread_mutex_lock(&buffer->writeLock);

  // ❌ if (writeBuffer.size) {  ← writeBuffer没声明，且.size是SubBuffer的字段，应该用->
  // ✅ 正确写法：
  SubBuffer* writeBuffer = buffer->buf_arr[buffer->write_index];

  // ❌ 缺少：检查剩余空间
  // ✅ 补充：需要 size+2 字节（2字节存长度头）
  if (writeBuffer->size - writeBuffer->used_len < size + 2) {
    pthread_mutex_unlock(&buffer->writeLock);
    return COM_FAIL;
  }

  // ❌ buf  ← 不完整的语句，缺少写入协议头和数据
  // ✅ 补充：写2字节长度头 + 数据
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
 *
 * @param buffer 待销毁的双缓冲
 */
void Common_Buffer_Destory(DoubleBuffer *buffer) {
  if (buffer == NULL) {
    return;
  }

  // ❌ free(buf_arr[0]->buf);  ← buf_arr不能直接访问，应该通过buffer->buf_arr[0]
  // ✅ 正确写法：
  free(buffer->buf_arr[0]->buf);
  free(buffer->buf_arr[0]);

  // ❌ free(buf_arr[1]->buf);  ← 同上
  // ✅ 正确写法：
  free(buffer->buf_arr[1]->buf);
  free(buffer->buf_arr[1]);

  // ❌ pthread_mutex_destory();  ← 拼写错误：destory → destroy；缺少参数
  // ✅ 正确写法：
  pthread_mutex_destroy(&buffer->writeLock);

  // ❌ pthread_mutex_destory(readLock);  ← 拼写错误；readLock未声明，应该用buffer->readLock
  // ✅ 正确写法：
  pthread_mutex_destroy(&buffer->readLock);

  free(buffer);
}
