#include "Driver_Modbus.h"
#include "log.h"

int main(void){

    Driver_Modbus_Init();


    uint16_t data = 22245;
    Driver_Modbus_WriteHoldRegisters(5,0,1,&data);

    uint16_t datas[2] = {0};
    Driver_Modbus_ReadInputRegisters(5, 1, 2,datas);

    // log_info("读取保持寄存器的数据是:%d",res);

}