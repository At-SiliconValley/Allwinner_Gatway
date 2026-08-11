#include "Driver_Http.h"

/**
 * @brief http收到一个数据包的回调
 *
 * @param ptr
 * @param size
 * @param nmemb
 * @param stream
 * @return size_t
 */
static size_t write_json_cb(char *ptr, size_t size, size_t nmemb, void *stream)
{

    MemoryBuffer *mem = (MemoryBuffer *)stream;

    // 根据本次接收到的数据大小,调整缓冲内存大小
    char *data = realloc(mem->response, mem->size + nmemb + 1);

    if (!data)
    {
        perror("内存调整失败");
        return 0;
    }

    mem->response = data;
    // 复制数据到缓冲
    memcpy(mem->response + mem->size, ptr, nmemb);
    mem->size += nmemb;
    mem->response[mem->size] = 0;

    return nmemb;
}
/**
 * @brief 获取版本文件
 *
 * @param url 请求的url
 * @param mem 存储json的缓冲
 * @return ComStatus
 */
ComStatus Driver_Http_GetJson(char *url, MemoryBuffer *mem)
{

    // 1、curl 公共初始化
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK)
    {
        perror("curl 全局初始化失败");
        return COM_FAIL;
    }

    // 2、请求初始化
    CURL *curl = curl_easy_init();
    if (url)
    {

        // 设置请求参数
        // 设置请求url
        curl_easy_setopt(curl, CURLOPT_URL, url);
        // 设置获取到数据之后写入函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_json_cb);
        // 设置数据写入的位置
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, mem);

        // 发起请求
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            perror("请求json文件失败");
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return COM_FAIL;
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return COM_OK;
    }
    else
    {
        perror("请求初始化失败");
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return COM_FAIL;
    }
}

/**
 * @brief 下载固件
 *
 * @param url
 * @param fileName
 * @return ComStatus
 */
ComStatus Driver_Http_DownloadFile(char *url, char *fileName)
{

    // 1、curl 公共初始化
    CURLcode res = curl_global_init(CURL_GLOBAL_ALL);
    if (res != CURLE_OK)
    {
        perror("curl 全局初始化失败");
        return COM_FAIL;
    }

    // 2、请求初始化
    CURL *curl = curl_easy_init();

    if (curl)
    {

        FILE *f1 = fopen(fileName, "wb");
        if (f1 == NULL)
        {
            perror("文件打开失败");
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return COM_FAIL;
        }
        // 3、设置请求的url
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // 4、设置写入函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);

        // 5、指定写入位置
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, f1);

        // 6、发起请求
        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            perror("请求下载固件失败");
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            fclose(f1);
            return COM_FAIL;
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        fclose(f1);
        return COM_OK;
    }

    curl_global_cleanup();
    return COM_FAIL;
}