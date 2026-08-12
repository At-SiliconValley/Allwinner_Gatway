#include "App_OTA.h"

// 当前固件版本号：主版本.次版本.补丁版本
#define MAJOR 3
#define MINOR 1
#define PATCH 0

// OTA线程运行标志
static bool ota_is_running = true;

/**
 * @brief 解析版本JSON，判断是否有新版本
 *
 *        JSON格式示例：{"major":3, "minor":1, "patch":1, "sha1":"abc123..."}
 *
 * @param json   版本JSON字符串
 * @param size   JSON字符串长度
 * @param shaStr 输出参数，用于返回新版本的SHA1校验值
 * @return true  有新版本
 * @return false 无新版本或解析失败
 */
static bool App_OTA_IsNewVersion(char *json, size_t size, char **shaStr)
{
    // 1. 调用 cJSON_ParseWithLength(json, size) 解析JSON字符串
    //    如果返回 NULL，log_info 打印"版本信息json字符串异常"，return false

    // 2. 用 cJSON_GetObjectItemCaseSensitive 从 root 中取出 "major" 字段
    //    用 cJSON_GetObjectItemCaseSensitive 从 root 中取出 "minor" 字段
    //    用 cJSON_GetObjectItemCaseSensitive 从 root 中取出 "patch" 字段
    //    用 cJSON_GetObjectItemCaseSensitive 从 root 中取出 "sha1" 字段

    // 3. 校验四个字段是否都存在且类型正确：
    //    major/minor/patch 必须是 cJSON_IsNumber
    //    sha1 必须是 cJSON_IsString
    //    任何一个不满足 → log_info 打印错误 → cJSON_Delete(root) → return false

    // 4. 比较版本号，判断是否为新版本（新版本的判断逻辑：major更大 或 major相等但minor更大 或 major和minor都相等但patch更大）
    //    检查逻辑：
    //    major->valueint  > MAJOR  ||
    //    (major->valueint == MAJOR && minor->valueint  > MINOR) ||
    //    (major->valueint == MAJOR && minor->valueint == MINOR && patch->valueint > PATCH)

    // 5. 如果满足新版本条件：
    //    a. log_info 打印当前版本和新版本号
    //       提示：用 MAJOR, MINOR, PATCH 打印当前版本，用 major->valueint, minor->valueint, patch->valueint 打印新版本
    //    b. 用 malloc 分配空间给 *shaStr，大小为 strlen(sha1->valuestring)
    //    c. 用 memcpy 把 sha1->valuestring 拷贝到 *shaStr
    //    d. cJSON_Delete(root) 释放JSON
    //    e. return true

    // 6. 不满足新版本条件：
    //    a. cJSON_Delete(root)
    //    b. return false
}

/**
 * @brief 检查并执行OTA升级流程
 *
 *        流程：下载版本JSON → 解析比较版本 → 下载新固件 → SHA1校验 → 重启
 */
static void App_OTA_CheckVersion(void)
{
    // 1. 声明 MemoryBuffer mem = {0}，用于存储下载的版本JSON数据

    // 2. log_info 打印"开始下载版本json文件"

    // 3. 调用 Driver_Http_GetJson(VERSION_PATH, &mem) 下载版本JSON
    //    如果返回 COM_FAIL → log_info 打印"版本文件获取失败" → return

    // 4. log_info 打印 mem.response（即下载到的JSON内容）

    // 5. 声明 char* shaStr = NULL
    //    调用 App_OTA_IsNewVersion(mem.response, mem.size, &shaStr) 检查是否有新版本
    //    如果返回 false → log_info 打印"当前版本就是最新版本" → free(shaStr) → return

    // 6. 走到这里说明有新版本
    //    log_info 打印"存在新固件,开始下载..."

    // 7. 调用 Driver_Http_DownloadFile(FIRMWARE_PATH, FIRMWARE_SAVE_PATH) 下载新固件
    //    如果返回 COM_FAIL → log_info 打印"新固件下载失败" → free(shaStr) → return

    // 8. log_info 打印"新固件下载完成,准备校验"

    // 9. 用 fopen(FIRMWARE_SAVE_PATH, "r") 打开刚下载的固件文件
    //    如果打开失败 → log_info 打印"固件程序打开失败,无法校验" → free(shaStr) → return

    // 10. 计算固件文件的SHA1值：
    //     a. 声明 SHA_CTX ctx，调用 SHA1_Init(&ctx) 初始化SHA上下文
    //     b. 声明 char datas[128] = {0} 作为读取缓冲区
    //     c. 循环调用 fread(datas, 1, 128, f1) 分块读取文件
    //        每次读取后调用 SHA1_Update(&ctx, (void*)datas, len) 更新SHA上下文
    //        当 fread 返回 ≤0 时退出循环
    //     d. 声明 uint8_t hashCode[SHA_DIGEST_LENGTH] = {0}
    //     e. 调用 SHA1_Final(hashCode, &ctx) 生成最终哈希值

    // 11. 将二进制哈希值转成十六进制字符串：
    //     a. 声明 char hashStr[2 * SHA_DIGEST_LENGTH + 1] = {0}
    //     b. 循环 i 从 0 到 SHA_DIGEST_LENGTH-1
    //        用 sprintf(hashStr + i * 2, "%02x", hashCode[i]) 逐字节转换

    // 12. log_info 打印计算出的 hashStr 和版本文件中的 shaStr

    // 13. 用 strcmp(hashStr, shaStr) 比较两个哈希值
    //     如果相等 → log_info 打印"固件完整..."
    //              → free(shaStr)
    //              → 调用 reboot(RB_AUTOBOOT) 重启系统（暂时注释掉，后续启用）
    //     如果不相等 → log_info 打印"固件下载不完整"
    //                → free(shaStr)
}

/**
 * @brief 信号处理函数，用于优雅退出OTA线程
 *
 * @param code 信号编号
 */
void App_OTA_Exit(int code)
{
    // 把 ota_is_running 设为 false，让主循环退出
}

/**
 * @brief OTA升级主线程入口
 *
 *        启动时检查一次版本，之后每天凌晨2点再检查
 */
void App_OTA_Run(void)
{
    // 1. 开机时立即检查一次版本更新
    //    调用 App_OTA_CheckVersion()

    // 2. 注册信号处理函数，用于优雅退出
    //    signal(SIGINT, App_OTA_Exit)  捕获 Ctrl+C
    //    signal(SIGTERM, App_OTA_Exit) 捕获 kill 命令

    // 3. 主循环：while(ota_is_running)
    //    a. 用 time(NULL) 获取当前时间戳
    //    b. 用 localtime(&t) 转换成 struct tm 结构体
    //    c. 检查 dt->tm_hour 是否等于 2（凌晨2点）
    //       如果是 → 调用 App_OTA_CheckVersion() 检查更新
    //    d. sleep(3600) 休眠1小时，避免CPU空转
}