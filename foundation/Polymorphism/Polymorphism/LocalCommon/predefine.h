#ifndef AV4_ALARM_PREDEFINE_H
#define AV4_ALARM_PREDEFINE_H

#define UNNORMAL_STATE_TIME        (10)    //超出正常时间持续时间
#define VIDEO_SAVE_TIME (50)
#define MIN(x,y)	((x)<(y)?(x):(y))

#define SUBSCRIBE_ALARM_SERVER  (10204)
#define SUBSCRIBE_TRAINITEM_CLIENT  (20258)
#define SUBSCRIBE_TRAINITEM_SERVER  (10158)
#define SUBSCRIBE_KEYCLNT_CLIENT    (20206)
#define SUBSCRIBE_KEYCLNT_SERVER    (10102)
#define SUBSCRIBE_LKJFILE_CLIENT    (20259)
#define SUBSCRIBE_LKJFILE_SERVER    (10159)

#include "Include/alarmType.h"
#include "Include/mydebug.h"
#include <QString>
#include <QDateTime>


typedef struct TrainItemDBInfo
{
    QString     id;             //编号，guid
    QDateTime   recordDataTime; //项点发生时的日期，时间
    int         itemNum;        //项点编号
    QString     deviceNum;      //设备号
    int         trainNum;       //机车号
    QString     trainCode;      //车次
    int         trainType;      //机车型号。
    int         driverNum;      //完整的司机号
    int         assitantDriverNum;//副司机号
    QString     m_sLatitude;    //纬度
    QString     m_sLongitude;   //经度
    QByteArray  m_originTaxData;//原始TAX数据
    int         isUpLoad;       //上传状态
}TTrainItemDBInfo;

enum EMTrainItem
{
    None=0,
    ARRIVING                = 11,   //进站
    LEAVING                 = 12,   //出站
    WAYSIDE_STOP            = 13,   //区间停车
    STATION_STOP            = 14,   //站内停车
    PIPE_PRESSURE_ZERO      = 15,   //管压为零
    SIGNAL_DEGRADE          = 16,   //信号降级
    SHUNTING_STARTING       = 17,   //调车启动
    TRAIN_STARTING          = 18,   //列车启动
    SIGNAL_HULT             = 19,   //信号故障
    ABNORMAL_STATION_STOP   = 20,   //站内非正常停车
    IN_STORAGE              = 21,   //入库
    OUT_STORAGE             = 22,   //出库
    SPEED_LIMIT_REVELATION  = 23,   //限速揭示
    DETACH_NO_PRESSURE_HOLD = 24,   //摘解未保压
    SPECIFIC_GUIDE_BULLETIN = 25,   //特定引导揭示
    STOP_NO_PRESSURE_HOLDING    = 26,   //停车后未保压
    HAND_SIGNAL_GUIDANCE    = 27,   //手信号引导
    TRIGGERED_UNLOADING     = 28,   //卸载
    EMERGENCY_BRAKING       = 29,   //紧急制动？
    SERVICE_BRAKING         = 30,   //常用制动
    CONTRACTING_BRAKE_RUN   = 31,   //抱闸运行
    ILLEGAL_REVERSING       = 32,   //违规换向
    SINGLE_MACHINE_NOT_FORWARD,     //单机未前移
    PIPE_PRESSURE_ZERO_RUN,         //运行中管压为零
    NON_ZERO_CROSSING_OF_HANDLE,    //手柄非零位过分相
    STOP_OUT_OF_ARRIVING,           //机外停车(非红黄灯)
    NO_EMU_AFTER_RELIEF,            //缓解后未动车
    SIGNAL_MUTATION,                //信号突变
    LOW_SPEED_FREIGHT_MITIGATION,   //货车低速缓解
    LACK_OF_AIR_ENOUGH_TO_DRIVE,    //充风不足开车
    SINGLE_VALVE_BRAKE_STOP,        //单阀制动停车
    ENGINE_PARKING_ON_THE_WAY       //途中柴油机停车
};

#endif //AV4_ALARM_PREDEFINE_H
