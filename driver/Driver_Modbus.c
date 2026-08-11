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
 */
ComStatus Driver_Modbus_Init(void) {
  // ❌ modbus_new_rtu("/dev/tty/ttyS0", 115200, 'N', 8, 1);
  //    问题1: 返回值modbus_t*没赋值给ctx，后面ctx还是NULL
  //    问题2: 路径"/dev/tty/ttyS0"多余了/tty，应该是"/dev/ttyS0"
  // ✅ 正确写法：
  ctx = modbus_new_rtu("/dev/ttyS0", 115200, 'N', 8, 1);
  if (ctx == NULL) {
    perror("modbus_new_rtu failed");
    return COM_FAIL;
  }

  // ❌ modbus_set_debug(ctx, true);  ← ctx是NULL时会崩溃；true应该用TRUE(项目规范)
  // ✅ 正确写法：
  modbus_set_debug(ctx, TRUE);

  // ❌ 缺少：modbus_connect连接从机
  // ✅ 补充：
  if (modbus_connect(ctx) == -1) {
    perror("modbus_connect failed");
    modbus_free(ctx);
    ctx = NULL;
    return COM_FAIL;
  }

  return COM_OK;
}

/**
 * @brief 写单个线圈
 */
void Driver_Modbus_WriteSingleCoil(uint8_t id, uint16_t index, uint8_t data) {
  // ❌ if (!(id > 247 && (data != 0 | data != 1) && ctx == NULL))
  //    问题1: 取反!让逻辑全反了——NOT(非法条件)永远为假，永远不会return
  //    问题2: | 是位或，data!=0|data!=1恒为真
  //    问题3: 应该用 || 和 && 组合
  // ✅ 正确写法：参数非法时return，合法时执行
  if (id > 247 || (data != 0 && data != 1) || ctx == NULL) {
    return;
  }

  // ❌ modbus_set_slave(ctx, 1);  ← 第二个参数写死了1，应该用id（从机地址）
  // ✅ 正确写法：
  modbus_set_slave(ctx, id);
  modbus_write_bit(ctx, index, data);
}

/**
 * @brief 读单个离散寄存器
 */
void Driver_Modbus_ReadDiscRegister(uint8_t id, uint16_t index, uint8_t *data) {
  // ❌ if (id > 247 || ctx == NULL) { ... }  ← 逻辑反了！参数非法时进去了，反而执行操作
  //    缺少 data == NULL 检查
  // ✅ 正确写法：参数非法时return
  if (id > 247 || data == NULL || ctx == NULL) {
    return;
  }

  // ❌ modbus_set_slave(ctx, 1);  ← 硬编码1，应该用id
  modbus_set_slave(ctx, id);

  // ❌ modbus_read_input_registers(ctx, 1, index, data);
  //    问题1: 读离散量应该用read_input_bits，不是read_input_registers
  //    问题2: 第二个参数1写错了，应该是index
  // ✅ 正确写法：
  uint8_t dest[1];
  modbus_read_input_bits(ctx, index, 1, dest);
  *data = dest[0];
}

/**
 * @brief 写多个保持寄存器
 */
void Driver_Modbus_WriteHoldRegisters(uint8_t id, uint16_t index, uint16_t size,
                                      uint16_t *datas) {
  if (id > 247 || datas == NULL || size == 0 || ctx == NULL) {
    return;
  }

  // ❌ modbus_set_slave(ctx, 1);  ← 硬编码1，应该用id
  // ✅ 正确写法：
  modbus_set_slave(ctx, id);
  modbus_write_registers(ctx, index, size, datas);
}

/**
 * @brief 读取多个输入寄存器
 */
void Driver_Modbus_ReadInputRegisters(uint8_t id, uint16_t index, uint16_t size,
                                      uint16_t *datas) {
  if (id > 247 || datas == NULL || size == 0 || ctx == NULL) {
    return;
  }

  // ❌ modbus_set_slave(ctx, 1);  ← 硬编码1，应该用id
  modbus_set_slave(ctx, id);

  // ❌ modbus_read_input_registers(1, index, size, datas);
  //    ← 第一个参数传了1(int)，应该是ctx(modbus_t*)
  // ✅ 正确写法：
  modbus_read_input_registers(ctx, index, size, datas);
}

/**
 * @brief 回收 Modbus 资源
 */
void Driver_Modbus_Destory(void) {
  if (ctx != NULL) {
    modbus_close(ctx);
    modbus_free(ctx);
    ctx = NULL;
  }
}
