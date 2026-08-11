#include "Driver_MQTT.h"
#include <MQTTClient.h>
#include <MQTTClientPersistence.h>
#include <MQTTReasonCodes.h>
#include <unistd.h>

// MQTT 客户端全局变量
static MQTTClient client;
static MQTTClient_connectOptions conn_opts =
    MQTTClient_connectOptions_initializer;

// 收到消息时的回调函数指针
static MqttReceiveCallback receiveHandle;

/**
 * @brief 连接断开时的回调函数（自动重连）
 *
 * @param context MQTT 上下文
 * @param cause   断开原因
 *
 * 实现步骤：
 *   1、定义 int res = 0, int time = 0
 *   2、while(1) 死循环重连：
 *      a. 调用 MQTTClient_connect(client, &conn_opts) 尝试连接
 *      b. 如果连接失败（res != MQTTCLIENT_SUCCESS）：
 *         - 如果 time < 60，time++
 *         - sleep(time) 等待后重试
 *         - continue
 *      c. 连接成功后，重新订阅主题：
 *         - 调用 MQTTClient_subscribe(client, PULL_TOPIC, MQTTREASONCODE_GRANTED_QOS_0)
 *         - 如果订阅失败，同上递增等待时间后重试
 *      d. 连接和订阅都成功，break 跳出循环
 *
 * 注意：PULL_TOPIC 在 Common_Config.h 中定义
 */
void Driver_MQTT_ConnectionLost(void *context, char *cause) {
  int res = 0, time = 0;

  while (1) {
    // ❌ MQTTClient_connect 返回值没检查
    res = MQTTClient_connect(client, &conn_opts);

    // ❌ do { sleep(60); } while (cnt_res != MQTTCLIENT_SUCCESS);
    //    ← 问题：即使连接成功也会sleep(60)才退出！应该连接失败才sleep递增
    // ✅ 正确写法：连接失败才递增等待
    if (res != MQTTCLIENT_SUCCESS) {
      if (time < 60) time++;
      sleep(time);
      continue;
    }

    // ❌ int sub_res = MQTTClient_subscribe(...)  ← 返回值没检查
    res = MQTTClient_subscribe(client, PULL_TOPIC, MQTTREASONCODE_GRANTED_QOS_0);

    // ❌ if (cnt_res == sub_res == MQTTCLIENT_SUCCESS)  ← C语言不能链式比较！
    //    (a==b==c) 会被解析为 ((a==b)==c)，即 1==c 或 0==c，逻辑完全错误
    // ✅ 正确写法：用 && 分开比较
    if (res == MQTTCLIENT_SUCCESS) {
      break;
    }

    if (time < 60) time++;
    sleep(time);
  }
}

/**
 * @brief 收到 MQTT 消息的回调函数
 *
 * @param context   MQTT 上下文
 * @param topicName 主题名
 * @param topicLen  主题名长度
 * @param message   消息内容
 * @return int 返回 1 表示处理成功
 */
int Driver_MQTT_MessageArrived(void *context, char *topicName, int topicLen,
                               MQTTClient_message *message) {
  if (receiveHandle != NULL) {
    // ❌ receiveHandle(message->payload, (char *) message->payload);
    //    ← 第一个参数传错了！message->payload是void*指针，不是int len
    // ✅ 正确写法：第一个参数是消息长度 message->payloadlen
    receiveHandle(message->payloadlen, (char *)message->payload);
  }
  // ❌ MQTTClient_freeMessage 和 MQTTClient_free 写在if里面，receiveHandle为NULL时不会执行→内存泄漏
  // ✅ 这两行应该放在if外面，不管receiveHandle是否为NULL都要释放
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

/**
 * @brief 消息发送完成回调
 */
void Driver_MQTT_DeliveryComplete(void *context, MQTTClient_deliveryToken dt) {
  // ❌ log_info("Message send Success!\n");  ← log_info自带换行，不需要\n；建议和老师一致
  // ✅ 正确写法：
  log_info("消息发送完成");
}

/**
 * @brief 初始化 MQTT 客户端
 *
 * @param rcb 收到消息时的回调函数
 * @return ComStatus
 */
ComStatus Driver_MQTT_Init(MqttReceiveCallback rcb) {
  receiveHandle = rcb;

  // ❌ MQTTClient_create(...);  ← 返回值没检查！失败时照样往下走
  // ✅ 正确写法：
  int res = MQTTClient_create(&client, MQTT_SERVER_URL, "app_mqtt",
                              MQTTCLIENT_PERSISTENCE_NONE, NULL);
  if (res != MQTTCLIENT_SUCCESS) {
    log_info("MQTTClient_create failed");
    return COM_FAIL;
  }

  // ❌ 缺少：conn_opts 设置
  // ✅ 补充：
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  // ❌ 缺少：注册回调函数
  // ✅ 补充：
  MQTTClient_setCallbacks(client, NULL,
                          Driver_MQTT_ConnectionLost,
                          Driver_MQTT_MessageArrived,
                          Driver_MQTT_DeliveryComplete);

  // ❌ 缺少：连接服务器
  // ✅ 补充：
  res = MQTTClient_connect(client, &conn_opts);
  if (res != MQTTCLIENT_SUCCESS) {
    log_info("MQTTClient_connect failed");
    return COM_FAIL;
  }

  // ❌ 缺少：订阅主题
  // ✅ 补充：
  res = MQTTClient_subscribe(client, PULL_TOPIC, MQTTREASONCODE_GRANTED_QOS_0);
  if (res != MQTTCLIENT_SUCCESS) {
    log_info("MQTTClient_subscribe failed");
    return COM_FAIL;
  }

  return COM_OK;
}

/**
 * @brief 向指定 topic 发送数据
 */
void Driver_MQTT_Send(char *topicName, char *datas, int len) {
  // ❌ if (!(topicName == NULL | datas == NULL | len < 0 | client == NULL))
  //    问题1: | 是位或运算符，应该用 || 逻辑或
  //    问题2: 取反 ! 让逻辑反了——参数非法时反而执行发送
  //    问题3: len < 0 应为 len <= 0（len==0也没意义）
  // ✅ 正确写法：参数非法时直接return，合法时才发送
  if (topicName == NULL || datas == NULL || len <= 0 || client == NULL) {
    return;
  }
  MQTTClient_publish(client, topicName, len, datas,
                     MQTTREASONCODE_GRANTED_QOS_0, 0, NULL);
}

/**
 * @brief 回收 MQTT 资源
 */
void Driver_MQTT_Deinit(void) {
  if (client != NULL) {
    MQTTClient_disconnect(client, 2000);
    MQTTClient_destroy(&client);
  }
}
