#include <QCoreApplication>
#include "CItemWaysideStop.h"
#include "CItemStop.h"
#include "Entity/QAlarmTructInfo.h"

void toTXTDataStm32(unsigned char* _data,STM32Data* _TXTdata)
{

    unsigned int  iDataTime;
    memcpy(&iDataTime,&(_data[3]),sizeof(unsigned int));

    QString _time;// = QString("%1-%2-%3 %4:%5:%6").arg(2000 + PARSE_BCD(_data[3]))  //year

    _time =  QString("%1-").arg( 2000 + (iDataTime>>26));
    _time = _time + QString("%1-").arg( (iDataTime>>22)&0x0F );
    _time = _time + QString("%1 ").arg( (iDataTime>>17)&0x1F );

    _time = _time + QString("%1:").arg( (iDataTime>>12)&0x1F );
    _time = _time + QString("%1:").arg( (iDataTime>>6)&0x3F );
    _time = _time + QString("%1").arg(  (iDataTime )  &0x3F );

    _TXTdata->m_time = QDateTime::fromString(_time,"yyyy-M-d h:m:s");



    _TXTdata->m_realSpeed = _data[7] +  (0x03 & _data[8])*256;
    _TXTdata->m_limitSpeed = (_data[8] >> 4) + _data[9]*16;

    _TXTdata->m_signal = (STM32Data::TRAIN_SIGNAL)(0x0F & _data[10]);//此处枚举值和协议中定义的信号灯 颜色值 一致  ///test
    //手柄位置（0非零,1零位)；
    _TXTdata->m_handlePosition = 0x01 & _data[11];
    //车位00,01：向后,10向前）；
    _TXTdata->m_cheWei = (STM32Data::CHE_WEI)((0x06 & _data[11])>>1);
    //制动
    _TXTdata->m_bDrag = (((0x18 & _data[11]) >> 3) == 2)?true:false;
     //牵引
    _TXTdata->m_bBreak = (((0x18 & _data[11]) >> 3) == 1)?true:false;

    _TXTdata->m_signalNum = _data[12] + _data[13]*256;
    _TXTdata->m_signalType = (STM32Data::SIGNAL_TYPE)_data[14];
    //公里标
    _TXTdata->m_kmMark = _data[15] + _data[16]*256 + (0x3F & _data[17])*256*256;

    _TXTdata->m_iAllWeight = _data[18] +  (_data[19])*0x100; //总重
    _TXTdata->m_iCountLong = _data[20] +  (_data[21])*0x100; //计长（单位：0.1米）
   //  qDebug() <<" _TXTdata->m_iCountLong used "<<_TXTdata->m_iCountLong;
    _TXTdata->m_iCountNumer  = _data[22] ;        //辆数
    _TXTdata->m_iHandleLevel=0;

    //本/补、客/货
    _TXTdata->m_trainType = (STM32Data::TRAIN_TYPE)((0x02 & _data[23]) >> 1);
    _TXTdata->m_bPassengerTrain  = 0x01& _data[23]; //是否客车

   //车次
     //区段号（交路号）
    _TXTdata->m_segmentNum = _data[26] & 0x01f;
    qDebug() << _TXTdata->m_segmentNum << _data[26];
    //车站号
    _TXTdata->m_stationNum = _data[27] + _data[43]*256;
    //司机号
    _TXTdata->m_driverNum = _data[28] + _data[29]*256 + _data[48]*256*256;
    //副司机号
    _TXTdata->m_assitantDriverNum = _data[30] + _data[31]*256 + _data[49]*256*256;
    //机车号
    _TXTdata->m_trainNum = _data[32] + _data[33]*256;
    //机车型号
    _TXTdata->m_trainModel = _data[34]+ _data[52]*256;
    //列车管压力
    _TXTdata->m_trainPipePress = _data[35] + _data[36]*256;
    //实际交路号  _data[53];

    //制动缸压力
    _TXTdata->m_breakPreass = _data[54] + _data[55]*256;

     //制动输出    _data[56];
     // 柴速
    _TXTdata->m_iMotorLevel = _data[57] + _data[58]*256;

    //车次
    _TXTdata->m_strTrainCode =QString("%1%2%3%4%5").arg((QChar)_data[44]).arg((QChar)_data[45]).arg((QChar)_data[46]).arg((QChar)_data[47]).arg(_data[66] + _data[67]*256 + _data[68]*256*256);
    _TXTdata->m_strTrainCode =  _TXTdata->m_strTrainCode.trimmed();

     if(_TXTdata->m_cheWei == 1)
         _TXTdata->m_accelerationDirection = 'B';
     else if (_TXTdata->m_cheWei  == 2)
        _TXTdata->m_accelerationDirection = 'A';
     else
        _TXTdata->m_accelerationDirection = 'N';


    // b2:1/0-调车/非调车
    //调车标志
    if(_data[37]& 0x04)
    {        //信号机不再刷新，记录为上一次的信号机编号
        _TXTdata->m_bShuntingFlag = true;

    }else{
        _TXTdata->m_bShuntingFlag = false;

    }

    if(_data[37]& 0x01){// 真： 降级 / 假：监控
        _TXTdata->m_bDownLevel = true;
    }else{
        _TXTdata->m_bDownLevel = false;
    }

}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    unsigned char buf[70] = {0x02, 0xC0, 0x05, 0x48, 0x29, 0xF9, 0x46, 0x04, 0xC0, 0x03, 0x21, 0x14, 0xB2, 0x02, 0x05, 0x1B, 0x12, 0x40, 0x88, 0x13, 0xC5, 0x03, 0x51, 0x00, 0x02, 0x00, 0x21, 0x3C, 0x29, 0x46, 0x2A, 0x46, 0x66, 0x01, 0xE1, 0x58, 0x02, 0x01, 0x00, 0x00, 0x67, 0x10, 0x10, 0x01, 0x20, 0x20, 0x46, 0x43, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0xC1, 0x00, 0x00, 0x19, 0x12, 0x00, 0x00, 0x80, 0x00, 0x00, 0x02, 0x00, 0x00};
    STM32Data stm32;
    toTXTDataStm32(buf, &stm32);

//    CTrainItem *test1 = new CTrainItem();
    QString m_relativePath = QString("/record-") + QString("yyyy-MM-dd-hh-mm-ss") + "/";
    qDebug() << m_relativePath.remove('/');
    CItemStop *test2 = new CItemStop();;
    qDebug() << "---------------";
    CItemWaysideStop *test3 = new CItemWaysideStop(ARRIVING);
    qDebug() << "---------------";
//    test1->isAlarmType();
//    test2->isAlarmType();
//    test3->isAlarmType();
//    CTrainItem *test = test3;
    CItemStop *testB = test2;
//    test->isAlarmType();
//    testB->isAlarmType();
    testB->initdata();
    testB = test3;
    qDebug() << "---------------";
    testB->initdata();
    return a.exec();
}
