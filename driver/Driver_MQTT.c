#include "Driver_MQTT.h"
#include <MQTTClient.h>
#include <MQTTClientPersistence.h>
#include <MQTTReasonCodes.h>
#include <unistd.h>

static MQTTClient client;
static MQTTClient_connectOptions conn_opts =
    MQTTClient_connectOptions_initializer;
static MqttReceiveCallback receiveHandle;

/**
 * @brief 连接断开时的回调函数（自动重连）
 */
void Driver_MQTT_ConnectionLost(void *context, char *cause) {
  int res = 0, time = 0;

  while (1) {
    // ❌ int cnt_res = MQTTClient_connect(client, &conn_opts);  // 没检查返回值
    res = MQTTClient_connect(client, &conn_opts);

    // ❌ do { sleep(60); } while (cnt_res != MQTTCLIENT_SUCCESS);
    //    即使连接成功也会sleep(60)！应该连接失败才sleep递增
    // ✅ 连接失败才递增等待
    if (res != MQTTCLIENT_SUCCESS) {
      if (time < 60) time++;
      sleep(time);
      continue;
    }

    // ❌ int sub_res = MQTTClient_subscribe(...);       // 没检查返回值
    res = MQTTClient_subscribe(client, PULL_TOPIC, MQTTREASONCODE_GRANTED_QOS_0);

    // ❌ if (cnt_res == sub_res == MQTTCLIENT_SUCCESS)  // C不能链式比较!
    //    (a==b==c) → ((a==b)==c) → 1==c或0==c，逻辑完全错
    // ✅ 用 && 分开
    if (res == MQTTCLIENT_SUCCESS) {
      break;
    }

    if (time < 60) time++;
    sleep(time);
  }
}

/**
 * @brief 收到 MQTT 消息的回调函数
 */
int Driver_MQTT_MessageArrived(void *context, char *topicName, int topicLen,
                               MQTTClient_message *message) {
  if (receiveHandle != NULL) {
    // ❌ receiveHandle(message->payload, (char *)message->payload);
    //    第一个参数传错了！message->payload是void*不是int，应该传message->payloadlen
    // ✅
    receiveHandle(message->payloadlen, (char *)message->payload);
  }
  // ❌ MQTTClient_freeMessage 和 MQTTClient_free 写在if里面
  //    receiveHandle为NULL时不会执行 → 内存泄漏
  // ✅ 放外面，无论如何都要释放
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}

/**
 * @brief 消息发送完成回调
 */
void Driver_MQTT_DeliveryComplete(void *context, MQTTClient_deliveryToken dt) {
  // ❌ log_info("Message send Success!\n");            // \n多余，log_info自带换行
  // ✅
  log_info("消息发送完成");
}

/**
 * @brief 初始化 MQTT 客户端
 */
ComStatus Driver_MQTT_Init(MqttReceiveCallback rcb) {
  receiveHandle = rcb;

  // ❌ MQTTClient_create(&client, ...);                // 返回值没检查！
  // ✅
  int res = MQTTClient_create(&client, MQTT_SERVER_URL, "app_mqtt",
                              MQTTCLIENT_PERSISTENCE_NONE, NULL);
  if (res != MQTTCLIENT_SUCCESS) {
    log_info("MQTTClient_create failed");
    return COM_FAIL;
  }

  // ❌ 缺少 conn_opts 设置
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;

  // ❌ 缺少 setCallbacks 注册回调
  MQTTClient_setCallbacks(client, NULL,
                          Driver_MQTT_ConnectionLost,
                          Driver_MQTT_MessageArrived,
                          Driver_MQTT_DeliveryComplete);

  // ❌ 缺少 connect 连接服务器
  res = MQTTClient_connect(client, &conn_opts);
  if (res != MQTTCLIENT_SUCCESS) {
    log_info("MQTTClient_connect failed");
    return COM_FAIL;
  }

  // ❌ 缺少 subscribe 订阅主题
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
  //    问题1: | 是位或，不是逻辑或 ||
  //    问题2: 取反!让逻辑反了——参数非法时反而执行发送
  //    问题3: len < 0 应为 len <= 0
  //    问题4: 条件成立时没有return，继续执行了MQTTClient_publish
  // ✅ 参数非法时直接return
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
