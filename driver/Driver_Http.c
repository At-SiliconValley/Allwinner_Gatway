#include "Driver_Http.h"

/**
 * @brief curl收到数据包时的回调函数
 *        每收到一块数据，curl就会调用这个函数
 *
 * @param ptr    本次收到的数据指针
 * @param size   每个数据单元的大小（始终为1）
 * @param nmemb  本次收到的数据单元数量（即字节数）
 * @param stream 用户自定义指针，这里传的是 MemoryBuffer*
 * @return size_t 返回实际处理的字节数，返回0会让curl中止传输
 */
static size_t write_json_cb(char *ptr, size_t size, size_t nmemb, void *stream)
{
    // 1. 把 stream 转成 MemoryBuffer* 类型
    (MemoryBuffer*)stream;
    // 2. 用 realloc 扩容 response 缓冲区，新大小 = 原大小 + nmemb + 1（+1给'\0'）

    // 3. 检查 realloc 是否成功，失败则 perror 打印错误并 return 0

    // 4. 把扩容后的指针赋值回 mem->response

    // 5. 用 memcpy 把 ptr 指向的数据拷贝到 mem->response + mem->size 的位置

    // 6. mem->size 累加 nmemb

    // 7. 在 mem->response[mem->size] 处写入 '\0' 作为字符串结尾

    // 8. return nmemb
}

/**
 * @brief 通过HTTP GET请求获取JSON数据
 *
 * @param url 请求的URL地址
 * @param mem 用于存储响应数据的 MemoryBuffer 指针
 * @return ComStatus COM_OK成功 / COM_FAIL失败
 */
ComStatus Driver_Http_GetJson(char *url, MemoryBuffer *mem)
{
    // 1. 调用 curl_global_init(CURL_GLOBAL_ALL) 全局初始化libcurl
    //    检查返回值是否为 CURLE_OK，失败则 perror 并 return COM_FAIL
    
    // 2. 调用 curl_easy_init() 初始化一个CURL句柄
    //    检查返回值是否为 NULL

    // 3. 如果句柄有效：
    //    a. curl_easy_setopt 设置 CURLOPT_URL 为传入的 url
    //    b. curl_easy_setopt 设置 CURLOPT_WRITEFUNCTION 为 write_json_cb
    //    c. curl_easy_setopt 设置 CURLOPT_WRITEDATA 为 mem
    //    d. 调用 curl_easy_perform 发起请求
    //    e. 检查返回值是否为 CURLE_OK，失败则 perror → curl_easy_cleanup → curl_global_cleanup → return COM_FAIL
    //    f. curl_easy_cleanup(curl) 清理句柄
    //    g. curl_global_cleanup() 全局清理
    //    h. return COM_OK

    // 4. 如果句柄无效：
    //    a. perror 打印错误
    //    b. curl_easy_cleanup(curl) 清理句柄
    //    c. curl_global_cleanup() 全局清理
    //    d. return COM_FAIL
}

/**
 * @brief 通过HTTP下载文件到本地
 *
 * @param url      下载链接
 * @param fileName 本地保存路径
 * @return ComStatus COM_OK成功 / COM_FAIL失败
 */
ComStatus Driver_Http_DownloadFile(char *url, char *fileName)
{
    // 1. 调用 curl_global_init(CURL_GLOBAL_ALL) 全局初始化
    //    失败则 perror 并 return COM_FAIL

    // 2. 调用 curl_easy_init() 初始化CURL句柄

    // 3. 如果句柄有效：
    //    a. 用 fopen(fileName, "wb") 打开文件（二进制写模式）
    //    b. 如果文件打开失败：perror → curl_easy_cleanup → curl_global_cleanup → return COM_FAIL
    //    c. curl_easy_setopt 设置 CURLOPT_URL 为 url
    //    d. curl_easy_setopt 设置 CURLOPT_WRITEFUNCTION 为 fwrite（libcurl标准写入函数）
    //    e. curl_easy_setopt 设置 CURLOPT_WRITEDATA 为文件指针 f1
    //    f. 调用 curl_easy_perform 发起下载请求
    //    g. 检查返回值，失败则 perror → curl_easy_cleanup → curl_global_cleanup → fclose(f1) → return COM_FAIL
    //    h. curl_easy_cleanup(curl)
    //    i. curl_global_cleanup()
    //    j. fclose(f1) 关闭文件
    //    k. return COM_OK

    // 4. 如果句柄无效：
    //    a. curl_global_cleanup()
    //    b. return COM_FAIL
}