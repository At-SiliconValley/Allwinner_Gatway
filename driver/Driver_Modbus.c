#include "Driver_Modbus.h"
#include <modbus/modbus-rtu.h>
#include <modbus/modbus.h>
#include <stdbool.h>
#include <sys/param.h>

// Modbus RTU 上下文全局变量
static modbus_t *ctx;

/**
 * @brief 初始化 Modbus RTU 通信
 *
 * @return ComStatus
 *
 * 实现步骤：
 *   1、调用 modbus_new_rtu("/dev/pts/4", 115200, 'N', 8, 1) 创建 RTU 上下文
 *      参数说明：串口设备路径、波特率115200、无校验、8数据位、1停止位
 *      如果返回 NULL，perror 打印错误，返回 COM_FAIL
 *   2、调用 modbus_set_debug(ctx, true) 开启调试模式
 *   3、调用 modbus_connect(ctx) 连接从机
 *      如果返回 -1：
 *         perror 打印错误
 *         modbus_free(ctx) 释放资源
 *         ctx = NULL
 *         返回 COM_FAIL
 *   4、返回 COM_OK
 */
ComStatus Driver_Modbus_Init(void) {
  // TODO: 按照上述步骤实现 Modbus 初始化
  modbus_new_rtu("/dev/tty/ttyS0", 115200, 'N', 8, 1);
  modbus_set_debug(ctx, true);
  return COM_FAIL; // 临时返回值
}

/**
 * @brief 写单个线圈
 *
 * @param id    从机地址（1-247）
 * @param index 线圈地址
 * @param data  线圈值（只能为 0 或 1）
 *
 * 实现步骤：
 *   1、参数校验：id > 247 或 data 不是 0/1 或 ctx == NULL，则返回
 *   2、调用 modbus_set_slave(ctx, id) 设置从机地址
 *   3、调用 modbus_write_bit(ctx, index, data) 写线圈
 */
void Driver_Modbus_WriteSingleCoil(uint8_t id, uint16_t index, uint8_t data) {
  // TODO: 按照上述步骤实现写单个线圈
  if (!(id > 247 && (data != 0 | data != 1) && ctx == NULL)) {
    return;
  } else {
    modbus_set_slave(ctx, 1);
    modbus_write_bit(ctx, index, data);
  }
}

/**
 * @brief 读单个离散寄存器
 *
 * @param id    从机地址（1-247）
 * @param index 寄存器地址
 * @param data  读取到的值（输出参数）
 *
 * 实现步骤：
 *   1、参数校验：id > 247 或 data == NULL 或 ctx == NULL，则返回
 *   2、调用 modbus_set_slave(ctx, id) 设置从机地址
 *   3、调用 modbus_read_input_bits(ctx, index, 1, data) 读取1个离散量
 */
void Driver_Modbus_ReadDiscRegister(uint8_t id, uint16_t index, uint8_t *data) {
  // TODO: 按照上述步骤实现读离散寄存器

  if (id > 247 || ctx == NULL) {
    modbus_set_slave(ctx, 1);
    modbus_read_input_registers(ctx, 1, index, data);
  }
}

/**
 * @brief 写多个保持寄存器
 *
 * @param id    从机地址（1-247）
 * @param index 起始寄存器地址
 * @param size  寄存器数量
 * @param datas 待写入的数据数组
 *
 * 实现步骤：
 *   1、参数校验：id > 247 或 datas == NULL 或 size == 0 或 ctx == NULL，则返回
 *   2、调用 modbus_set_slave(ctx, id) 设置从机地址
 *   3、调用 modbus_write_registers(ctx, index, size, datas) 写寄存器
 */
void Driver_Modbus_WriteHoldRegisters(uint8_t id, uint16_t index, uint16_t size,
                                      uint16_t *datas) {
  // TODO: 按照上述步骤实现写保持寄存器

  if (id > 247 || datas == NULL || size == 0 || ctx == NULL) {
    return;
  } else {
    modbus_set_slave(ctx, 1);
    modbus_write_registers(ctx, index, size, datas);
  }
}

/**
 * @brief 读取多个输入寄存器
 *
 * @param id    从机地址（1-247）
 * @param index 起始寄存器地址
 * @param size  寄存器数量
 * @param datas 读取到的数据（输出参数）
 *
 * 实现步骤：
 *   1、参数校验：id > 247 或 datas == NULL 或 size == 0 或 ctx == NULL，则返回
 *   2、调用 modbus_set_slave(ctx, id) 设置从机地址
 *   3、调用 modbus_read_input_registers(ctx, index, size, datas) 读取寄存器
 */
void Driver_Modbus_ReadInputRegisters(uint8_t id, uint16_t index, uint16_t size,
                                      uint16_t *datas) {
  if (id > 247 || datas == NULL || size == 0 || ctx == NULL) {
    return;
  } else {
    modbus_set_slave(ctx, 1);
    modbus_read_input_registers(1, index, size, datas);
  }
}

/**
 * @brief 回收 Modbus 资源
 *
 * 实现步骤：
 *   1、如果 ctx 不为 NULL：
 *      a. 调用 modbus_close(ctx) 关闭连接
 *      b. 调用 modbus_free(ctx) 释放资源
 *      c. ctx = NULL
 */
void Driver_Modbus_Destory(void) {
  if (ctx != NULL) {
    modbus_close(ctx);
    modbus_free(ctx);
    ctx = NULL;
  }
}