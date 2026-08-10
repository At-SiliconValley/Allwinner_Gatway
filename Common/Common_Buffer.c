#include "Common_Buffer.h"

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
    // TODO: 按照上述步骤实现双缓冲创建

    return COM_FAIL; // 临时返回值，实现后改成 COM_OK
}

/**
 * @brief 从指定缓冲中读取数据
 * 
 * @param buffer 待读取数据的缓冲
 * @param datas  读取到的数据（输出参数，内部 malloc，调用者需 free）
 * @param size   读取到的数据大小（输出参数）
 * @return ComStatus COM_OK 成功 / COM_FAIL 失败
 * 
 * 实现步骤：
 *   1、参数校验：buffer / datas / size 不能为 NULL
 *   2、初始化 *size = 0, *datas = NULL
 *   3、上读锁 pthread_mutex_lock(&buffer->readLock)
 *   4、获取当前读缓冲 readBuffer = buffer->buf_arr[buffer->read_index]
 *   5、判断 readBuffer->used_len 是否为 0：
 *      5.1、如果为 0，说明当前读缓冲没有数据，需要交换读写缓冲：
 *           a. 上写锁 pthread_mutex_lock(&buffer->writeLock)
 *           b. 交换索引：read_index = !read_index; write_index = !write_index
 *           c. 重新获取 readBuffer = buffer->buf_arr[buffer->read_index]
 *           d. 如果交换后 readBuffer->used_len 仍为 0：
 *              - 释放写锁、释放读锁，返回 COM_FAIL
 *           e. 释放写锁
 *   6、读取数据（协议格式：前2字节为长度，后面为数据）：
 *      6.1、*size = (buf[0] << 8) | buf[1] （读取长度）
 *      6.2、readBuffer->used_len -= 2
 *      6.3、malloc 申请 *datas = (*size + 1) 字节，memset 清零
 *      6.4、memcpy(*datas, &buf[2], *size) 拷贝数据
 *   7、将剩余数据向前移动（memmove），更新 used_len
 *   8、释放读锁，返回 COM_OK
 */
ComStatus Common_Buffer_Read(DoubleBuffer* buffer, char** datas, uint16_t* size)
{
    // TODO: 按照上述步骤实现数据读取

    return COM_FAIL; // 临时返回值
}

/**
 * @brief 将数据写入指定缓冲
 * 
 * @param buffer 待写入数据的缓冲
 * @param datas  待写入数据
 * @param size   数据大小
 * @return ComStatus COM_OK 成功 / COM_FAIL 失败
 * 
 * 实现步骤：
 *   1、参数校验：buffer / datas 不能为 NULL，size 不能为 0
 *   2、上写锁 pthread_mutex_lock(&buffer->writeLock)
 *   3、获取当前写缓冲 writeBuffer = buffer->buf_arr[buffer->write_index]
 *   4、检查写缓冲剩余空间是否足够（需要 size + 2 字节，2字节存长度）：
 *      如果 writeBuffer->size - writeBuffer->used_len < size + 2：
 *        释放写锁，返回 COM_FAIL
 *   5、写入数据（协议格式：先写2字节长度，再写数据）：
 *      5.1、buf[used_len] = (size >> 8) & 0xFF      （长度高字节）
 *      5.2、buf[used_len + 1] = size & 0xFF          （长度低字节）
 *      5.3、used_len += 2
 *      5.4、memcpy(&buf[used_len], datas, size)       （数据）
 *      5.5、used_len += size
 *   6、释放写锁，返回 COM_OK
 */
ComStatus Common_Buffer_Write(DoubleBuffer* buffer, char* datas, uint16_t size)
{
    // TODO: 按照上述步骤实现数据写入

    return COM_FAIL; // 临时返回值
}

/**
 * @brief 回收缓冲资源
 * 
 * @param buffer 待销毁的双缓冲
 * 
 * 实现步骤：
 *   1、如果 buffer == NULL，直接 return
 *   2、释放 buf_arr[0]->buf，释放 buf_arr[0]
 *   3、释放 buf_arr[1]->buf，释放 buf_arr[1]
 *   4、pthread_mutex_destroy 销毁 writeLock
 *   5、pthread_mutex_destroy 销毁 readLock
 *   6、free(buffer)
 */
void Common_Buffer_Destory(DoubleBuffer* buffer)
{
    // TODO: 按照上述步骤实现资源回收
}