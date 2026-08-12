#include "Driver_MQTT.h"

void func(int len,char* datas){
    log_info("收到消息[%d]:%.*s", len, len, datas);
}
int main(void){

    Driver_MQTT_Init(func);

    while(1){

        Driver_MQTT_Send("XXX","HELLO",5);

        sleep(5);
    }
}