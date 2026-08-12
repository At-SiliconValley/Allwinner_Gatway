#ifndef __APP_OTA_H__
#define __APP_OTA_H__
#include "Driver_Http.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdio.h"
#include "cJSON.h"
#include "string.h"
#include "openssl/sha.h"
#include "sys/reboot.h"
#include "time.h"
#include "signal.h"

void App_OTA_Run(void);

#endif
