#include "App_Application.h"
#include <stdint.h>
#include <stdlib.h>

// 全局双缓冲：上行缓冲（modbus→MQTT）、下行缓冲（MQTT→modbus）
static DoubleBuffer *upBuffer;
static DoubleBuffer *downBuffer;

// 前向声明
static void App_Application_MQTTReceiveHandle(int len, char *datas);
static void App_Application_UpTaskFunc(void *args);
static void App_Application_DownTaskFunc(void *args);

/**
 * @brief 应用初始化（按顺序初始化各模块，任一步失败则回滚已初始化的资源）
 *
 * 实现步骤：
 *

 *
 *   2、初始化 Modbus：
 *      - 调用 Driver_Modbus_Init()
 *      - 如果返回 COM_FAIL：
 *        log_info 打印 "modbus 初始化失败"
 *        调用 Driver_MQTT_Deinit() 回收 MQTT
 *        return
 *      - log_info 打印 "modbus准备就绪"
 *
 *   3、创建双缓冲：
 *      a. 创建上行缓冲：
 *         - 调用 Common_Buffer_CreateDoubleBuffer(&upBuffer, 1024)
 *         - 如果返回 COM_FAIL：
 *           log_info 打印 "上行双缓冲构建失败"
 *           调用 Driver_Modbus_Destory()、Driver_MQTT_Deinit()
 *           return
 *         - log_info 打印 "上行缓冲准备就绪"
 *      b. 创建下行缓冲：
 *         - 调用 Common_Buffer_CreateDoubleBuffer(&downBuffer, 1024)
 *         - 如果返回 COM_FAIL：
 *           log_info 打印 "下行双缓冲构建失败"
 *           调用 Driver_Modbus_Destory()、Driver_MQTT_Deinit()
 *           调用 Common_Buffer_Destory(upBuffer) 回收上行缓冲
 *           return
 *         - log_info 打印 "下行缓冲准备就绪"
 *
 *   4、创建线程池：
 *      - 调用 Common_Poll_Create(2) 创建2个线程的线程池
 *      - 如果返回 COM_FAIL：
 *        log_info 打印 "线程池构建失败"
 *        依次回收：Driver_Modbus_Destory()、Driver_MQTT_Deinit()
 * Common_Buffer_Destory(upBuffer)、Common_Buffer_Destory(downBuffer)
 *        return
 *      - log_info 打印 "线程池准备就绪"
 *
 *   5、添加任务到线程池：
 *      a. 添加上行任务：
 *         - 定义 Task upTask = { .args = NULL, .Func =
 App_Application_UpTaskFunc }
 *         - 调用 Common_Poll_AddTask(&upTask)
 *         - 如果失败，回收所有资源后 return
 *         - log_info 打印 "上行任务添加就绪"
 *      b. 添加下行任务（同上，Func 为 App_Application_DownTaskFunc）
 */
static void App_Application_Init(void) {
  // TODO: 按照上述步骤实现应用初始化
  /*
   *   1、初始化 MQTT：
   *      - 调用 Driver_MQTT_Init(App_Application_MQTTReceiveHandle)
   *      - 将 MQTT 收到消息的回调注册为 App_Application_MQTTReceiveHandle
   *      - 如果返回 COM_FAIL，log_info 打印 "mqtt 初始化失败"，return
   *      - log_info 打印 "MQTT Client 准备就绪"
   */
  // MqttReceiveCallback mqttrckb;
  Driver_MQTT_Init(App_Application_MQTTReceiveHandle);
}

/**
 * @brief 应用运行入口
 *
 * 调用 App_Application_Init() 初始化，然后 while(1) 死循环保持程序运行
 */
void App_Application_Run(void) {
  // TODO: 先调用 App_Application_Init()，然后 while(1);
}

/**
 * @brief MQTT 收到数据的回调函数
 *
 * @param len   数据长度
 * @param datas 数据内容
 *
 * 实现步骤：
 *   1、log_info 打印 "接收到MQTT数据:%s", datas
 *   2、调用 Common_Buffer_Write(downBuffer, datas, len) 将数据写入下行缓冲
 *      （下行缓冲中的数据会被下行线程取出，解析JSON后通过Modbus发给设备）
 */
static void App_Application_MQTTReceiveHandle(int len, char *datas) {
  // TODO: 按照上述步骤实现 MQTT 接收回调
  LOG_INFO("MQTT datas:%s", datas);
  Common_Buffer_Write(downBuffer, datas, len);
}

/**
 * @brief 上行线程执行函数（从上行缓冲读数据，通过 MQTT 发送到云端）
 *
 * @param args 线程参数（未使用）
 *
 * 实现步骤：
 *   1、while(1) 死循环：
 *      a. 定义 char* datas = NULL; uint16_t size = 0;
 *      b. 调用 Common_Buffer_Read(upBuffer, &datas, &size) 从上行缓冲读数据
 *      c. 如果 size > 0：
 *         - log_info 打印 "从上行缓冲区获取到数据:%s", datas
 *         - 调用 Driver_MQTT_Send(PUSH_TOPIC, datas, size) 发送到云端
 *         - free(datas) 释放读取到的数据内存
 *
 * 注意：PUSH_TOPIC 在 Common_Config.h 中定义
 */
static void App_Application_UpTaskFunc(void *args) {
  // TODO: 按照上述步骤实现上行任务
  char *datas = NULL;
  uint16_t size = 0;
  Common_Buffer_Read(upBuffer, &datas, &size);
  if (size > 0) {
    log_info("data from upbuffer: %s", datas);
    Driver_MQTT_Send(PUSH_TOPIC, datas, size);
    free(datas);
  }
}

/**
 * @brief 下行线程执行函数（从下行缓冲读数据，解析JSON，通过Modbus控制设备）
 *
 * @param args 线程参数（未使用）
 *
 * JSON 协议格式示例：
 * {
 *   "id" : 5,
 *   "type" : "set/get",
 *   "is_start" : 1,
 *   "targetAngle" : -3300,
 *   "targetSpeed" : 1600
 * }
 *
 * 实现步骤：
 *   1、while(1) 死循环：
 *      a. 定义 char* datas = NULL; uint16_t size = 0;
 *      b. 调用 Common_Buffer_Read(downBuffer, &datas, &size) 从下行缓冲读数据
 *      c. 如果 size > 0：
 *
 *         // 2、解析 JSON
 *         - 调用 cJSON_ParseWithLength(datas, size) 解析 JSON
 *         - 如果返回 NULL，log_info 打印 "json解析失败"，continue
 *
 *         // 3、提取公共字段 type 和 id
 *         - 调用 cJSON_GetObjectItemCaseSensitive 获取 "type" 字段
 *         - 调用 cJSON_GetObjectItemCaseSensitive 获取 "id" 字段
 *         - 校验 type 必须是 String 类型，id 必须是 Number 类型
 *         - 如果校验失败，cJSON_Delete(root) 释放 JSON，continue
 *
 *         // 4、根据 type 判断是 set（设置）还是 get（查询）
 *         - 使用 strcmp(type->valuestring, "set") 比较
 *
 *         // 4a、如果是 "set"（设置模式）：
 *            - 提取 "is_start" 字段（Number 类型）
 *            - 提取 "targetAngle" 字段（Number 类型）
 *            - 提取 "targetSpeed" 字段（Number 类型）
 *            - 校验以上三个字段都存在且类型正确
 *            - 定义 Float2U16 fx（联合体：float 和 uint16_t[2] 共享内存）
 *            - 发送目标速度：
 *              fx.data = (float)targetSpeed->valuedouble
 *              调用 Driver_Modbus_WriteHoldRegisters(id->valueint,
 * TARGET_SPEED_ADDR, 2, fx.arr)
 *            - 发送目标角度：
 *              fx.data = (float)targetAngle->valuedouble
 *              调用 Driver_Modbus_WriteHoldRegisters(id->valueint,
 * TARGET_ANGLE_ADDR, 2, fx.arr)
 *            - 启动/停止电机：
 *              调用 Driver_Modbus_WriteSingleCoil(id->valueint,
 * START_MOTOR_ADDR, isStart->valueint)
 *
 *         // 4b、如果是 "get"（查询模式）：
 *            - 调用 cJSON_CreateObject() 创建返回 JSON 对象
 *            - 调用 cJSON_AddNumberToObject 添加 "id" 字段
 *            - 读取当前速度：
 *              调用 Driver_Modbus_ReadInputRegisters(id->valueint,
 * CURRENT_SPPED_ADDR, 2, fx.arr) 调用 cJSON_AddNumberToObject 添加
 * "currentSpeed" 字段
 *            - 读取当前角度：
 *              调用 Driver_Modbus_ReadInputRegisters(id->valueint,
 * CURRENT_ANGLE_ADDR, 2, fx.arr) 调用 cJSON_AddNumberToObject 添加
 * "currentAngle" 字段
 *            - 读取电机转向：
 *              定义 uint8_t dir = 0
 *              调用 Driver_Modbus_ReadDiscRegister(id->valueint,
 * MOTOR_DIR_ADDR, &dir) 调用 cJSON_AddStringToObject 添加 "dir" 字段（dir==1 ?
 * "正转" : "反转"）
 *            - 调用 cJSON_PrintUnformatted(obj) 将 JSON 转为字符串
 *            - 调用 Common_Buffer_Write(upBuffer, json, strlen(json))
 * 写入上行缓冲
 *            - free(json) 释放 JSON 字符串
 *            - cJSON_Delete(obj) 释放 JSON 对象
 *
 *         // 5、清理
 *         - cJSON_Delete(root) 释放根 JSON
 *         - free(datas) 释放读取到的原始数据
 *
 * 注意：TARGET_ANGLE_ADDR、TARGET_SPEED_ADDR 等宏在 Common_Config.h 中定义
 */
static void App_Application_DownTaskFunc(void *args) {
  // TODO: 按照上述步骤实现下行任务
}