#include "stdint.h"
#include "Common_Config.h"
int main(void){

    float x = 1230.45f;

    Float2U16 f216;
    f216.data = x;

    log_info("%d   %d",f216.arr[0], f216.arr[1]);

    Float2U16 fx;
    fx.arr[0] = f216.arr[0];
    fx.arr[1] = f216.arr[1];

    log_info("%.2f",fx.data);
}