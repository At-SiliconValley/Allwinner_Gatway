#include "App_Application.h"
#include "App_OTA.h"
#include "App_Daemon.h"
int main( int num, char* args[] ){
    if(num <= 1){
        log_info("必须传入app/ota/deamon参数");
        return 0;
    }

    if( strcmp(args[1] , "app") == 0 ){
        App_Application_Run();
    }else if( strcmp(args[1] , "ota") == 0 ){
        App_OTA_Run();
    }else if( strcmp(args[1], "daemon") == 0 ){
        App_Daemon_Run();
    }else{
        log_info("参数传入错误");
    }

    return 0;
}