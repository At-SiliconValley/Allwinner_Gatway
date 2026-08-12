PARAM:=Common/cJSON.c
PARAM+=Common/log.c
PARAM+=driver/Driver_MQTT.c
PARAM+=Common/Common_Pool.c
PARAM+=Common/Common_Buffer.c
PARAM+=driver/Driver_Modbus.c
PARAM+=App/App_Application.c

CFLAGS += -Wall -Wextra

CFLAGS += -I.
CFLAGS += -IApp
CFLAGS += -ICommon
CFLAGS += -Idriver
# CFLAGS += -Iota

ifdef SYSROOT
	CFLAGS += --sysroot=$(SYSROOT)
endif

LDLIBS += -lpaho-mqtt3c
LDLIBS += -lcurl
LDLIBS += -lcrypto
LDLIBS += -lmodbus
#modbus文件路径,toolchain前面的路径需要改为自己的工程路径
# LDLIBS += -L/home/shtos/桌面/01_Projects/485_gateway/toolchain/arm-linux-gnueabihf/lib

SRC += $(shell find App -name "*.c" -type f)
SRC += $(shell find Common -name "*.c" -type f)
SRC += $(shell find driver -name "*.c" -type f)
# SRC += $(shell find thirdparty -name "*.c" -type f)

OBJ := $(SRC:.c=.o)

TARGET := gateway

.PHONY: all, clean

all: $(TARGET)

clean:
	@-rm -f $(TARGET) $(OBJ) main.o
$(TARGET): main.o $(OBJ)
	@-$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

main: $(PARAM) main.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -lmodbus -o $@
	-@./$@
	-@rm -rf $@