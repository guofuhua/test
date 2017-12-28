#ifndef COMMENT_H
#define COMMENT_H


#include <QString>
#include <QDateTime>

#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//解析STM32数据
typedef struct  STM32Data
{
    enum TRAIN_SIGNAL{TNoMak=0,TGreen = 1,TYello = 2,TDoubleYello = 3,TRedYello = 4,TRed=5,TWhite = 6,TGreenYello = 7,TYelloTwo = 8};
    enum SIGNAL_TYPE{NoType = 0,ChuZhan=2,JinZhan = 3,TongGuo = 4,YuGao = 5,RongXu = 6};
    enum TRAIN_TYPE{BENWU = 0,BUJI = 1};//本务/补机
    enum CHE_WEI{DZero = 0,DBack =1, DForword=2};
    STM32Data()
    {
        m_id = "";
        m_realSpeed = 0;
        m_limitSpeed = 0;
        m_signal = TNoMak;
        m_handlePosition = 0;
        m_cheWei = DZero;
        m_bDrag = 0;
        m_bBreak = 0;
        m_signalNum = 0;
        m_signalType = NoType;
        m_kmMark = 0;
        m_trainNum = 0;
        m_trainPipePress = 0;

        m_strTrainCode = "";
        m_stationNum = 0;
        m_segmentNum = 0; //qu duan hao
        m_trainModel = 0; //机车型号
        m_driverNum = 0;
        m_iRoomUsed = 0;

        m_accelerationDirection = 'N';
        m_trainType = BENWU;
        m_bPassengerTrain = true;
        m_currentState = 0;
        m_bShuntingFlag = false;
        m_bDownLevel = false;
        m_bReConnect = 0;
        m_iCountLong = 0;
        m_iCountNumer = 0;
        m_assitantDriverNum = 0;
        m_bUpLoad = false;
        m_strDeviceNumber = "0";

    };
    QString m_id;  //存储在数据库中记录的索引

    QDateTime m_time; //tax返回的时间
    int   m_kmMark;   //公里标 3字节
    unsigned short m_realSpeed;    //实速 2字节
    QString m_strTrainCode;         //车次
    unsigned int   m_trainNum;  //机车号 2字节

    //========机车工况
    unsigned char m_handlePosition; //手柄位置（0非零,1零位)；
    CHE_WEI m_cheWei; //车位00,01：向后,10向前）；
    bool  m_bDrag;   //牵引
    bool  m_bBreak; //制动
    // ======机车工况

    unsigned char m_iRoomUsed;   //司机室占用
    bool m_bDownLevel;  // 真： 降级 / 假：监控
    bool m_bShuntingFlag; //调车标志
    unsigned char m_bReConnect;  //重联信息
    TRAIN_TYPE  m_trainType;  //列车类型  本务/补机。
    bool  m_bPassengerTrain;     //是否客车
    unsigned short m_iCountLong; //计长（单位：0.1米）
    short   m_iCountNumer;        //辆数
    int m_stationNum;   //完整的车站号
    int m_driverNum;    //完整的司机号
    unsigned char m_segmentNum; //(交路号)区段号
    unsigned short m_iAllWeight;//总重
    unsigned short m_iHandleLevel;//手柄级位
    //unsigned char m_iMonitorControldata;//监控状态
//-----------------------
    short m_limitSpeed;   //限速
    TRAIN_SIGNAL  m_signal;  //机车信号
    unsigned short   m_signalNum;//信号机编号
    SIGNAL_TYPE m_signalType; //信号机种类
    unsigned short   m_trainPipePress; //列车管压力

    unsigned short m_BalancePreass; //均衡缸压力

    unsigned short m_averagePreass; //平均管压力

    unsigned short   m_breakPreass1;//制动缸1压力
    unsigned short   m_breakPreass2;//制动缸2压力


     unsigned short m_allWindpreass;//总风管压力
     unsigned short m_trainWindPreass; //列车管充风流量

     unsigned char m_bigSwitchCmd;//大闸指令
     unsigned char m_smallSwitchCmd;//小闸指令
     unsigned char m_atherBreakCmd;//其他制动指令
     unsigned char m_subAtherBareakCmd;  //“其他制动指令”屏蔽字节
    unsigned char m_trainModel; //机车型号
    int m_assitantDriverNum;//副司机号
    unsigned short m_iMotorLevel;//柴速
//----------------------------

    char m_accelerationDirection; //加速度方向,A=A端方向，B=B端方向
    bool m_bUpLoad;//是否上传，为数据库使用添加
    bool m_bUpValid;

    unsigned short m_breakPreassCode;//制动机故障代码
    QString m_strDeviceNumber;//设备编号
    bool operator==(STM32Data _data)
    {
        if(m_realSpeed == _data.m_realSpeed &&
            //m_limitSpeed == _data.m_limitSpeed &&
            m_signal == _data.m_signal &&
            m_handlePosition == _data.m_handlePosition &&
            m_cheWei == _data.m_cheWei &&
            m_bDrag == _data.m_bDrag &&
            m_bBreak == _data.m_bBreak &&
            m_iRoomUsed == _data.m_iRoomUsed &&
            m_signalNum == _data.m_signalNum &&
            m_signalType == _data.m_signalType &&//m_kmMark == _data.m_kmMark &&            abs(m_kmMark - _data.m_kmMark) < 80 &&
            m_trainNum ==  _data.m_trainNum &&
            m_trainPipePress == _data.m_trainPipePress &&
            m_breakPreass1 == _data.m_breakPreass1 &&
            m_strTrainCode == _data.m_strTrainCode &&
            m_stationNum == _data.m_stationNum &&
            m_driverNum == _data.m_driverNum &&
            m_segmentNum == _data.m_segmentNum && //qu duan hao
            m_trainModel == _data.m_trainModel && //机车型号
            m_assitantDriverNum == _data.m_assitantDriverNum &&
            m_accelerationDirection == _data.m_accelerationDirection &&
            m_trainType == _data.m_trainType &&
            m_bPassengerTrain == _data.m_bPassengerTrain)
        {
            return true;
        }
        return false;
    }

    unsigned char m_currentState ;//最新系统当前状态

    std::string m_sLongitude;//经度
    std::string m_sLatitude;//纬度

    QByteArray m_byteTAXData;//ＴＡＸ原数据
}STM32Data;

//版本信息
typedef struct TVersionInfo{

    unsigned char m_uSoftwaveVersion;
    unsigned char m_uHardwaveVersion;
}TVersionInfo;

//视频图片报文
typedef struct TImageInfo
{
    bool m_bHardDiskFault;//硬盘故障
    bool m_bCard1Fault;//采集卡1故障
    bool m_bCard2Fault;//采集卡2故障
    unsigned short m_uVideoCheck;//摄像头自检 -16通道 每位一通道
    unsigned char m_uImageChannel;//图像通道
    unsigned char m_uImageType;//图像格式
    QByteArray m_bytePictureData;//图像数据 数据
}TImageInfo;

struct TFireCamera
{
    uchar camera[4];
};

//视频图片报文
typedef struct TFireInfo
{
    quint16 sync;//硬盘故障
    quint16 length;//采集卡1故障
    uchar type;//采集卡2故障
    uchar count;//摄像头自检 -16通道 每位一通道
    quint16 none;//图像通道
    struct TFireCamera fireProbe[32];//图像格式
    char check_bit;//图像数据 数据
}TFireInfo;
#endif // COMMENT_H
