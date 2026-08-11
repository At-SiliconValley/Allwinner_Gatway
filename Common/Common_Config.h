#ifndef __COMMON_CONFIG_H__
#define __COMMON_CONFIG_H__
#include "log.h"
#include "unistd.h"
#include "stdint.h"
typedef enum{
    COM_OK,
    COM_FAIL
}ComStatus;

typedef union 
{
    float data;
    uint16_t arr[2];
}Float2U16 ;


#define HOST_NAME "192.168.50.48"

#define VERSION_PATH ("http://" HOST_NAME ":8000/server/version.json")
#define FIRMWARE_PATH ("http://" HOST_NAME ":8000/server/gateway")
#define FIRMWARE_SAVE_PATH "/usr/bin/gateway.update"
//ws://host:port
#define MQTT_SERVER_URL ("ws://" HOST_NAME ":8083")

#define PULL_TOPIC "atguigu"
#define PUSH_TOPIC "response"

//目标角度存储角标
#define TARGET_ANGLE_ADDR 0
//目标速度存储角标
#define TARGET_SPEED_ADDR 2
//启动电机存储角标
#define START_MOTOR_ADDR 0

//电机转向存储角标
#define MOTOR_DIR_ADDR 0
//当前速度存储角标
#define CURRENT_SPPED_ADDR 0
//当前角度存储角标
#define CURRENT_ANGLE_ADDR 2

#endif
