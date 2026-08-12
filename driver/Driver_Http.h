#ifndef __DRIVER_HTTP_H__
#define __DRIVER_HTTP_H__
#include "curl/curl.h"
#include "Common_Config.h"
#include "stdlib.h"
#include "string.h"

typedef struct{
    char* response; //存储http数据的缓冲
    size_t size; //缓冲中数据的大小
}MemoryBuffer;

/**
 * @brief 获取版本文件
 * 
 * @param url 请求的url
 * @param mem 存储json的缓冲
 * @return ComStatus 
 */
ComStatus Driver_Http_GetJson( char* url,MemoryBuffer* mem );

/**
 * @brief 下载固件
 * 
 * @param url 
 * @param fileName 
 * @return ComStatus 
 */
ComStatus Driver_Http_DownloadFile( char* url, char* fileName );
#endif
