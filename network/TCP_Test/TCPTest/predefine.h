#ifndef AV4_ALARM_PREDEFINE_H
#define AV4_ALARM_PREDEFINE_H

#define UNNORMAL_STATE_TIME        (10)    //超出正常时间持续时间
#define VIDEO_SAVE_TIME (50)

#define ALARM_RAILWAY_OBSTACLE (10)//异物报警
#define ALARM_TRAINITEM_3 (13)//区间停车
#define ALARM_TRAINITEM_5 (15)//管压为零
#define ALARM_TRAINITEM_9 (19)//信号故障
#define ALARM_TRAINITEM_10 (20)//站内非正常停车

#define ALARM_TAX_READ_FAIL	(1000)	//TAX读取失败
#define ALARM_TAX_DATA_ERR	(1001)	//TAX数据错误
#define ALARM_TAX_NO_DATA	(1002)	//TAX长时间无数据接收
#define ALARM_GPS_READ_FAIL	(1003)	//北斗读取失败
#define ALARM_GPS_DATA_ERR	(1004)	//北斗数据错误
#define ALARM_GPS_NO_DATA	(1005)	//北斗长时间无数据接收
#define ALARM_CAMERA_FAULT	(1006)	//图像采集器故障
#define ALARM_AV4_SD_FAULT	(1007)	//AV4 SD卡故障
#define ALARM_EXP_SD_FAULT	(1008)	//AV4EXP SD卡故障
#define ALARM_PACK_DOWN_FAIL	(1009)	//升级包下载失败
#define ALARM_PACK_CHECK_FAIL	(1010)	//升级包校验失败
#define ALARM_UPGRADE_FAIL	(1011)	//升级收尾失败
#define ALARM_DATABASE_ABNORMAL	(1012)	//报警模块数据库异常
#define ALARM_UPGRADE_BACK_FAIL	(1013)	//升级备份失败

//test---begin
#define DEBUG (1)

#if defined(DEBUG)
#define FUNC_IN //do {printf("[%s][%s(in )][%d]\n", __FILE__, __FUNCTION__, __LINE__);}while(0);
#define FUNC_OUT //do {printf("[%s][%s(out)][%d]\n", __FILE__, __FUNCTION__, __LINE__);}while(0);
#define TRACE(format,...) do {printf("FILE: "__FILE__", LINE: %d: "format"\n", __LINE__, ##__VA_ARGS__);}while(0);
#define DEB(format,...) //do {printf("[DEBUG]FILE: "__FILE__", LINE: %d: "format"\n", __LINE__, ##__VA_ARGS__);}while(0);
#define INFO(format,...) do {printf("[INFO]"format, ##__VA_ARGS__);}while(0);
#define WARN(format,...) do {printf("[WARNING]FILE: "__FILE__", LINE: %d: "format, __LINE__, ##__VA_ARGS__);}while(0);
#define ERR(format,...) do {printf("[ERROR]FILE: "__FILE__", LINE: %d: "format, __LINE__, ##__VA_ARGS__);}while(0);
#else
#define FUNC_IN
#define FUNC_OUT
#define TRACE(format,...)
#define WARN(format,...)
#define DEB(format,...)
#define ERR(format,...)
#define INFO(format,...)
#endif
//test---end

#endif //AV4_ALARM_PREDEFINE_H
