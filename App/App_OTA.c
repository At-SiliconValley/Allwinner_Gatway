#include "App_OTA.h"
#define MAJOR 3
#define MINOR 1
#define PATCH 0

static bool ota_is_running = true;

/**
 * @brief 检测是否存在新版本固件
 *
 * @param json
 * @return true
 * @return false
 */
static bool App_OTA_IsNewVersion(char *json, size_t size, char **shaStr)
{

    //{"major": 3, "minor": 1 , "patch": 1 , "sha1": ""}
    cJSON *root = cJSON_ParseWithLength(json, size);
    if (root == NULL)
    {
        log_info("版本信息json字符串异常");
        return false;
    }

    cJSON *major = cJSON_GetObjectItemCaseSensitive(root, "major");
    cJSON *minor = cJSON_GetObjectItemCaseSensitive(root, "minor");
    cJSON *patch = cJSON_GetObjectItemCaseSensitive(root, "patch");
    cJSON *sha1 = cJSON_GetObjectItemCaseSensitive(root, "sha1");

    if (major == NULL || !cJSON_IsNumber(major) || minor == NULL || !cJSON_IsNumber(minor) || patch == NULL || !cJSON_IsNumber(patch) || sha1 == NULL || !cJSON_IsString(sha1))
    {
        log_info("major/minor/patch/sha1字段缺失或者类型错误");
        cJSON_Delete(root);
        return false;
    }

    // 校验是否存在新版本
    if (major->valueint > MAJOR ||
        (major->valueint == MAJOR && minor->valueint > MINOR) ||
        (major->valueint == MAJOR && minor->valueint == MINOR && patch->valueint > PATCH))
    {
        log_info("检测到有新版本,当前版本[%d.%d.%d] 新版本[%d.%d.%d]",MAJOR,MINOR,PATCH,major->valueint,minor->valueint,patch->valueint);
        *shaStr = (char*)malloc( strlen(sha1->valuestring) );
        memcpy(*shaStr,sha1->valuestring, strlen(sha1->valuestring));
        cJSON_Delete(root);
        return true;
    }
    cJSON_Delete(root);
    return false;
}
static void App_OTA_CheckVersion(void)
{

    // 1、下载官网版本信息json文件
    MemoryBuffer mem = {0};
    log_info("开始下载版本json文件");
    if (Driver_Http_GetJson(VERSION_PATH, &mem) == COM_FAIL)
    {
        log_info("版本文件获取失败");
        return;
    }
    log_info("%s",mem.response);
    // 2、解析json
    char* shaStr = NULL;
    if( App_OTA_IsNewVersion(mem.response,mem.size, &shaStr) == false ){
        log_info("当前版本就是最新版本");
        free(shaStr);
        return;
    }
    // 3、如果存在,下载固件
    log_info("存在新固件,开始下载...");
    if( Driver_Http_DownloadFile(FIRMWARE_PATH,FIRMWARE_SAVE_PATH ) == COM_FAIL){
        log_info("新固件下载失败");
        free(shaStr);
        return;
    }
    log_info("新固件下载完成,准备校验");
    // 4、校验固件是否完整
    FILE* f1 = fopen(FIRMWARE_SAVE_PATH,"r");
    if(f1 == NULL){
        log_info("固件程序打开失败,无法校验");
        free(shaStr);
        return;
    }

    //初始化SHA上下文
    SHA_CTX ctx;
    SHA1_Init(&ctx);

    char datas[128] = {0};
    size_t len = 0;
    while( ( len = fread(datas, 1 , 128, f1 ) ) > 0 ){
        //更新SHA上下文数据
        SHA1_Update( &ctx, (void*)datas, len);
    }
    //根据上下文数据生成hash码
    uint8_t hashCode[ SHA_DIGEST_LENGTH ] = {0};
    //[0x01,0x05,0xA1]
    SHA1_Final( hashCode, &ctx );

    //将hash码转成字符串
    char hashStr[ 2 * SHA_DIGEST_LENGTH + 1 ] = {0};
    //"0105A1"
    for( uint8_t i=0; i<SHA_DIGEST_LENGTH; i ++ ){
        sprintf(hashStr + i * 2,  "%02x", hashCode[i]);
    }
    log_info("计算出的hashcode:%s  版本文件中的hashcode:%s",hashStr,shaStr);

    if( strcmp(hashStr,shaStr) == 0 ){
        log_info("固件完整...");
        // 5、重启[后续在linux中会有一个自启脚本,通过脚本切换新版本固件运行]
        free(shaStr);
        reboot(RB_AUTOBOOT);

    }else{
        free(shaStr);
        log_info("固件下载不完整");
    }
    
}


void App_OTA_Exit (int code){
    ota_is_running = false;
}
void App_OTA_Run(void)
{
    //先开机自检
    App_OTA_CheckVersion();

    //捕获信号
    signal( SIGINT, App_OTA_Exit );
    signal( SIGTERM, App_OTA_Exit );
    
    while(ota_is_running){

        //获取当前时间
        time_t t = time(NULL);
        //将时间戳转成日期时间
        struct tm* dt = localtime(&t);

        //后续每天凌晨2点自检
        if( dt->tm_hour == 2 ){
            App_OTA_CheckVersion();
        }

        sleep( 3600 );
    }
}