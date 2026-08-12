# 待实现清单

## 1. App/App_Application.c — 应用层（5个函数）

- [ ] `App_Application_Init()` — 应用初始化：依次初始化MQTT→Modbus→双缓冲→线程池→添加任务，任一步失败回滚
- [ ] `App_Application_Run()` — 应用入口：调用Init后死循环
- [ ] `App_Application_MQTTReceiveHandle()` — MQTT回调：收到数据写入下行缓冲
- [ ] `App_Application_UpTaskFunc()` — 上行线程：从上行缓冲读数据→MQTT发送到云端
- [ ] `App_Application_DownTaskFunc()` — 下行线程：从下行缓冲读数据→解析JSON→Modbus控制设备

## 2. App/App_OTA.c — OTA升级（4个函数）

- [ ] `App_OTA_IsNewVersion()` — 解析版本JSON，比较major/minor/patch判断是否有新版本
- [ ] `App_OTA_CheckVersion()` — 完整OTA流程：下载JSON→比较版本→下载固件→SHA1校验→重启
- [ ] `App_OTA_Exit()` — 信号处理：设置退出标志
- [ ] `App_OTA_Run()` — OTA主循环：开机检查一次+每天凌晨2点检查

## 3. App/App_Daemon.c — 守护进程（3个函数）

- [ ] `App_Daemon_CheckProcess()` — 检测子进程状态，挂了就fork重新拉起
- [ ] `App_Daemon_Exit()` — 信号处理：设置退出标志
- [ ] `App_Daemon_Run()` — 守护主循环：daemon化→重定向IO→循环监控ota/app两个子进程

## 4. driver/Driver_Http.c — HTTP驱动（3个函数）

- [ ] `write_json_cb()` — curl数据回调：realloc扩容→memcpy拷贝→累加size
- [ ] `Driver_Http_GetJson()` — HTTP GET获取JSON：curl初始化→设置URL/回调→发起请求
- [ ] `Driver_Http_DownloadFile()` — HTTP下载文件：curl初始化→fopen→fwrite写入

## 5. 自启脚本 — Shell脚本（1个）

- [ ] `自启脚本` — 网关启停脚本：start(新固件切换→启动) / stop(杀进程) / restart