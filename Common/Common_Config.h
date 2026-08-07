#ifndef __COMMON_CONFIG_H__
#define __COMMON_CONFIG_H__
#include "log.h"
#include "unistd.h"

typedef enum{
    COM_OK,
    COM_FAIL
}ComStatus;

#define HOST_NAME "192.168.50.48"
//ws://host:port
#define MQTT_SERVER_URL ("ws://" HOST_NAME ":8083")

#define PULL_TOPIC "atguigu"

#endif
