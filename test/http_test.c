#include "Driver_Http.h"

int main(void){

    // MemoryBuffer buffer = {0};
    // Driver_Http_GetJson( "https://www.baidu.com/", &buffer);

    // log_info("%s",buffer.response);
    // free(buffer.response);
    log_info("==================");
    Driver_Http_DownloadFile("http://192.168.10.110:8000/server/http_test","/home/atguigu/main");

}