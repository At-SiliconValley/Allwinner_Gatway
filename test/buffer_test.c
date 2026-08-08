#include "Common_Buffer.h"

int main(void){
    DoubleBuffer* buffer = NULL;
    ComStatus status = Common_Buffer_CreateDoubleBuffer(&buffer, 50);

    if(status == COM_FAIL){
        log_info("缓冲创建失败");
        return 1;
    }

    Common_Buffer_Write(buffer, "hello",5);
    Common_Buffer_Write(buffer, "hadoop",6);
    Common_Buffer_Write(buffer, "java",4);

    char* datas = NULL;
    uint16_t size = 0;
    Common_Buffer_Read(buffer, &datas,&size);

    if(size != 0){

        log_info("1读取数据长度:%d 数据:%s",size,datas);

        free(datas);
        
    }

    Common_Buffer_Read(buffer, &datas,&size);

    if(size != 0){

        log_info("2读取数据长度:%d 数据:%s",size,datas);

        free(datas);
        
    }

    Common_Buffer_Read(buffer, &datas,&size);

    if(size != 0){

        log_info("3读取数据长度:%d 数据:%s",size,datas);

        free(datas);
        
    }

    Common_Buffer_Destory(buffer);
}