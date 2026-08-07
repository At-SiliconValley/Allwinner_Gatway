#include "log.h"

int main(void){

    //
    log_set_level(LOG_TRACE);
    log_trace("hello word");
    log_debug("hello word");
    log_info("hello word");
    log_warn("hello word");
    log_error("hello word");
    log_fatal("hello word");

}