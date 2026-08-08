#include "Common_Buffer.h"


/**
 * @brief 创建双缓冲
 * 
 * @param buffer 创建的缓冲的指针
 * @return ComStatus 
 */
ComStatus Common_Buffer_CreateDoubleBuffer(DoubleBuffer** buffer,  uint16_t size   ){

    //1、校验参数
    if( size ==0 ){
        return COM_FAIL;
    }
    //2、创建读缓冲
    //2.1、申请读缓冲内存
    SubBuffer* readBuffer = (SubBuffer* )malloc( sizeof(SubBuffer) );
    if( readBuffer == NULL ){
        perror("读缓冲内存申请失败");
        return COM_FAIL;
    }
    memset(readBuffer, 0, sizeof(SubBuffer));
    //2.2、申请读缓冲中缓冲内存
    readBuffer->buf =  (char*)malloc(size);
    if( readBuffer->buf == NULL ){

        perror("读缓冲中存储数据内存申请失败");
        free(readBuffer);
        return COM_FAIL;
    }
    //2.2、读缓冲属性初始化
    memset(readBuffer->buf, 0, size);
    readBuffer->size = size;
    log_info("读缓冲内存申请成功");
    //3、申请写缓冲内存
    SubBuffer* writeBuffer = (SubBuffer* )malloc( sizeof(SubBuffer) );
    if( writeBuffer == NULL ){
        perror("写缓冲内存申请失败");
        free(readBuffer->buf);
        free(readBuffer);
        return COM_FAIL;
    }
    memset(writeBuffer, 0, sizeof(SubBuffer));
    //2.2、申请写缓冲中存储数据内存
    writeBuffer->buf =  (char*)malloc(size);
    if( writeBuffer->buf == NULL ){

        perror("写缓冲中存储数据内存申请失败");
        free(writeBuffer);
        free(readBuffer->buf);
        free(readBuffer);
        return COM_FAIL;
    }
    //2.2、读缓冲属性初始化
    memset(writeBuffer->buf, 0, size);
    writeBuffer->size = size;
    log_info("写缓冲内存申请成功");
    //4、创建双缓冲
    DoubleBuffer* doubleBuffer = (DoubleBuffer*)malloc(sizeof(DoubleBuffer));
    if(doubleBuffer == NULL){
        perror("双缓冲内存申请失败");
        free(writeBuffer->buf);
        free(writeBuffer);
        free(readBuffer->buf);
        free(readBuffer);
        return COM_FAIL;
    }
    log_info("双缓冲内存申请成功");
    memset(doubleBuffer,0,sizeof(DoubleBuffer));
    doubleBuffer->buf_arr[0] = readBuffer;
    doubleBuffer->buf_arr[1] = writeBuffer;
    doubleBuffer->read_index = 0;
    doubleBuffer->write_index = 1;
    //5、创建读写锁
    if( pthread_mutex_init(&doubleBuffer->readLock, NULL ) != 0){
        perror("读锁创建失败");
        free(writeBuffer->buf);
        free(writeBuffer);
        free(readBuffer->buf);
        free(readBuffer);
        free(doubleBuffer);
        return COM_FAIL;
    }
    log_info("读锁创建成功");
    if( pthread_mutex_init(&doubleBuffer->writeLock, NULL ) != 0){
        perror("写锁创建失败");
        free(writeBuffer->buf);
        free(writeBuffer);
        free(readBuffer->buf);
        free(readBuffer);
        pthread_mutex_destroy(&doubleBuffer->readLock);
        free(doubleBuffer);
        return COM_FAIL;
    }
    log_info("写锁创建成功");
    *buffer = doubleBuffer;
    log_info("双缓冲创建成功");
    return COM_OK;
}

/**
 * @brief 从指定缓冲中读取数据
 * 
 * @param buffer 待读取数据的缓冲
 * @param datas 读取到的数据
 * @param size 读取的数据的大小
 * @return ComStatus 
 */
ComStatus Common_Buffer_Read( DoubleBuffer* buffer, char* datas, uint16_t* size  ){

}
/**
 * @brief 将数据写入指定缓冲
 * 
 * @param buffer 待写入数据的缓冲
 * @param datas 待写入数据
 * @param size 数据大小
 * @return ComStatus 
 */
ComStatus Common_Buffer_Write( DoubleBuffer* buffer,char* datas, uint16_t size  ){

}

/**
 * @brief 回收缓冲资源
 * 
 * @param buffer 
 */
void Common_Buffer_Destory(DoubleBuffer* buffer ){

}