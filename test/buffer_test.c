#include "Common_Buffer.h"

int main(void){
    DoubleBuffer* buffer = NULL;
    ComStatus status = Common_Buffer_CreateDoubleBuffer(&buffer, 10);

    if(status == COM_FAIL){
        log_info("缓冲创建失败");
        return 1;
    }

    SubBuffer* writeBuffer = buffer->buf_arr[ buffer->write_index ];
    writeBuffer->buf[writeBuffer->used_len] = 'A';

    log_info("-=========>%c",writeBuffer->buf[writeBuffer->used_len]);

    SubBuffer* readBuffer = buffer->buf_arr[ buffer->read_index ];
    readBuffer->buf[readBuffer->used_len] = 'B';

    log_info("-=========>%c",readBuffer->buf[readBuffer->used_len]);
}