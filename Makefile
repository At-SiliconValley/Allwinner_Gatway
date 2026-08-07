
PARAM:=Common/cJSON.c
PARAM+=Common/log.c
PARAM+=driver/Driver_MQTT.c

INCLUDE_PATH:=-ICommon
INCLUDE_PATH+=-Idriver
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