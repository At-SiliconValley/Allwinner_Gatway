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
  // ⚠️ 【错误1】modbus_new_rtu 返回 modbus_t* 指针，必须赋值给 ctx 才能用
  //          正确写法：ctx = modbus_new_rtu("/dev/ttyS0", 115200, 'N', 8, 1);
  //          注意：串口路径 "/dev/tty/ttyS0" 有多余的 /tty 层级，应该是 "/dev/ttyS0"
  modbus_new_rtu("/dev/tty/ttyS0", 115200, 'N', 8, 1);
  // ⚠️ 【错误2】ctx 还是 NULL，直接传给 modbus_set_debug 会空指针崩溃
  //          正确：先判断 if (ctx == NULL) { perror("modbus_new_rtu"); return COM_FAIL; }
  // ⚠️ 【错误3】true → TRUE（项目统一用 TRUE/FALSE 大写）
  // ⚠️ 【错误4】缺少关键步骤：modbus_connect(ctx) 连接从机，失败要 modbus_free(ctx) 并置 NULL
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
  // ⚠️ 【错误5】参数校验逻辑完全反了！
  //   NOT( id>247 AND data不是0/1 AND ctx==NULL ) → 这几乎永远为 FALSE，永远不会 return
  //   正确写法：if (id > 247 || (data != 0 && data != 1) || ctx == NULL) { return; }
  // ⚠️ 【错误6】data != 0 | data != 1 用了按位或 | 而不是逻辑或 ||，且 data!=0||data!=1 恒为真
  //          任何数要么 ≠0 要么 ≠1，应该用 &&：data != 0 && data != 1
  if (!(id > 247 && (data != 0 | data != 1) && ctx == NULL)) {
    return;
  } else {
    // ⚠️ 【错误7】modbus_set_slave 第二个参数写死了 1，应该用参数 id（从机地址）
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
  // ⚠️ 【错误8】参数校验逻辑反了！当参数非法时应该 return，而不是继续执行
  //          正确写法：if (id > 247 || data == NULL || ctx == NULL) { return; }
  // ⚠️ 【错误9】缺少 data == NULL 的检查
  if (id > 247 || ctx == NULL) {
    // ⚠️ 【错误10】modbus_set_slave 第二个参数写死了 1，应该用参数 id
    modbus_set_slave(ctx, 1);
    // ⚠️ 【错误11】读离散寄存器应该用 modbus_read_input_bits，不是 modbus_read_input_registers
    //          modbus_read_input_bits(ctx, index, 1, data)
    //          第二个参数 1 写错了，应该是 index（寄存器地址）
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
    // ⚠️ 【错误12】modbus_set_slave 第二个参数写死了 1，应该用参数 id（从机地址）
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
    // ⚠️ 【错误13】modbus_set_slave 第二个参数写死了 1，应该用参数 id（从机地址）
    modbus_set_slave(ctx, 1);
    // ⚠️ 【错误14】modbus_read_input_registers 第一个参数应该是 ctx（modbus_t*），不是 1
    //          正确：modbus_read_input_registers(ctx, index, size, datas)
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