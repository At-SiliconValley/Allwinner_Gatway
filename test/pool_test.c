#include "Common_Pool.h"

void func1(void* args){
    while(1){
        log_info("task1 running");
        sleep(2);
    }
}

void func2(void* args){
    while(1){
        log_info("task2 running");
        sleep(2);
    }
}
int main(void){

    //创建线程池
    if( Common_Poll_Create(2) == COM_FAIL){
        return 1;
    }

    Task task = {
        .Func = func1,
        .args = NULL
    };
    Common_Poll_AddTask(&task );
    Task task2 = {
        .Func = func2,
        .args = NULL
    };
    Common_Poll_AddTask(&task2 );

    while(1);
}