
PARAM:=Common/cJSON.c
PARAM+=Common/log.c
PARAM+=driver/Driver_MQTT.c
PARAM+=Common/Common_Pool.c
PARAM+=Common/Common_Buffer.c
PARAM+=driver/Driver_Modbus.c
PARAM+=App/App_Application.c
PARAM+=driver/Driver_Http.c

INCLUDE_PATH:=-ICommon
INCLUDE_PATH+=-Idriver
INCLUDE_PATH+=-IApp
#json模块测试
json_test: $(PARAM) test/Cjson_Test.c
	-@gcc $^ -ICommon -o $@
	-@./$@
	-@rm -rf $@

#-I: 设置include目录
#日志模块测试
log_test: $(PARAM) test/log_test.c
	-@gcc $^ -ICommon -o $@
	-@./$@
	-@rm -rf $@

#-l: 连接指定库
mqtt_test: $(PARAM) test/mqtt_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -o $@
	-@./$@
	-@rm -rf $@

thread_test: $(PARAM) test/thread_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -o $@
	-@./$@
	-@rm -rf $@

thread_lock: $(PARAM) test/thread_lock.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -o $@
	-@./$@
	-@rm -rf $@

thread_queue: $(PARAM) test/thread_queue.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -o $@
	-@./$@
	-@rm -rf $@

pool_test: $(PARAM) test/pool_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -o $@
	-@sudo ./$@
	-@rm -rf $@

buffer_test: $(PARAM) test/buffer_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -o $@
	-@./$@
	-@rm -rf $@

modbus_slave: $(PARAM) test/modbus_slave_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -lmodbus -o $@
	-@./$@
	-@rm -rf $@
modbus_master: $(PARAM) test/modbus_master_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -lmodbus -o $@
	-@./$@
	-@rm -rf $@

float2U16: $(PARAM) test/Float2U16.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -lmodbus -o $@
	-@./$@
	-@rm -rf $@

main: $(PARAM) main.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -lmodbus -o $@
	-@./$@
	-@rm -rf $@

io_test: $(PARAM) test/io_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -lmodbus -lcurl -o $@
	-@./$@
	-@rm -rf $@

http_test: $(PARAM) test/http_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -lmodbus -lcurl -o $@
	-@./$@
	-@rm -rf $@

process_test: $(PARAM) test/process_test.c
	-@gcc $^ $(INCLUDE_PATH) -lpaho-mqtt3c -lmodbus -lcurl -o $@
	-@./$@
	-@rm -rf $@