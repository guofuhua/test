#include "QPrePublicPctl.h"
#include "comment.h"
#include <string.h>
#include <windows.h>
#include <QRegExp>


STM32Data g_taxData;
uchar verifyData(unsigned char* _data,int _lgth);
QPrePublicPctl::QPrePublicPctl(QObject *parent) : QObject(parent)
{

}


//
//FUCTION:模拟产生	公共信息报文
//PARAMETER：
//RETURN：No
void QPrePublicPctl::pdtPublicPtl()
{
    TPublicByte tPublic;

    uchar* m_TaxBuffer = tPublic.m_uMainFrameByte;

        memset(m_TaxBuffer,0,sizeof(TPublicByte));
        //bzero((void*)(m_TaxBuffer),sizeof(TPublicByte));
        m_TaxBuffer[0] = m_TaxBuffer[1] = 0xAA;
        ushort iUShortTemp = 0x40;
        memcpy(&(m_TaxBuffer[2]),&iUShortTemp,sizeof(unsigned short));
        m_TaxBuffer[4] = 0x03;


        uchar uTemp;

           //时间 日期
            unsigned int  iDataTime,iTemp;

            QString _time;// = QString("%1-%2-%3 %4:%5:%6").arg(2000 + PARSE_BCD(_data[3]))  //year
    {
            _time = g_taxData.m_time.toString("yyyy-MM-dd hh:mm:ss");
            iDataTime = _time.right(2).toInt();//秒

            iTemp =_time.mid(14,2).toInt();
            iDataTime |= (iTemp<<6);        //分
            iTemp =_time.mid(11,2).toInt();
            iDataTime |= (iTemp<<12);        //时
            iTemp =_time.mid(8,2).toInt();
            iDataTime |= (iTemp<<17);        //日
            iTemp =_time.mid(5,2).toInt();
            iDataTime |= (iTemp<<22);//月
            iTemp =_time.mid(2,2).toInt();
            iDataTime |= (iTemp<<26);//年
            memcpy(&(m_TaxBuffer[5]),&iDataTime,sizeof(unsigned int));
        }

        //公里标
        memcpy(&(m_TaxBuffer[9]),&g_taxData.m_kmMark,3);

            //实速、
         //   taxData.m_realSpeed &= 0x3FF;
            //（无限速）
        memcpy(&(m_TaxBuffer[12]),&g_taxData.m_realSpeed,sizeof(unsigned short));

        //车次
       _time = g_taxData.m_strTrainCode;
       //数字部分
       _time.remove(QRegExp("[A-Za-z]"));
       iTemp = _time.toInt();
       memcpy(&(m_TaxBuffer[18]),&iTemp,3);
       //字符部分
       _time = g_taxData.m_strTrainCode;
       _time = _time.remove(QRegExp("[0-9]")).right(4);
        uchar *pChar= (uchar*) _time.toStdString().c_str();
        memset((char*)&m_TaxBuffer[14],'\x20',4);
        memcpy(&m_TaxBuffer[14-_time.size()],pChar,_time.size());

        //机车号
        memcpy(&(m_TaxBuffer[21]),&g_taxData.m_trainNum,sizeof(unsigned short));

        //机车工况
        //手柄位置（0非零,1零位)；
        uTemp = (g_taxData.m_handlePosition & 0x01);
        //车位00,01：向后,10向前）；

        if(g_taxData.m_cheWei == STM32Data::DForword)
        {

            uTemp |= 0x02;
        }
        if(STM32Data::DBack ==g_taxData.m_cheWei)
        {
             uTemp |= (0x01<<2) ;
        }

        //制动 //牵引
        if(g_taxData.m_bBreak)  uTemp |= 0x08;
        if(g_taxData.m_bDrag)   uTemp |= 0x10;

        m_TaxBuffer[23] =uTemp;



        //司机室占用
         if(g_taxData.m_iRoomUsed == 1){
             uTemp =1;
         }else if(g_taxData.m_iRoomUsed == 2){
             uTemp =2;
         }else{
            uTemp = g_taxData.m_iRoomUsed;
         }
          m_TaxBuffer[24] =uTemp;

        //装置状态
          // 真： 降级 / 假：监控
          uTemp = g_taxData.m_bDownLevel;

          //调车标志
          if(g_taxData.m_bShuntingFlag)
          {
              uTemp |= 0x40;
          }
          m_TaxBuffer[25] = uTemp;

        //重联信息
        m_TaxBuffer[26] = (uchar)g_taxData.m_bReConnect;


       // 本/补、客/货
         uTemp = g_taxData.m_bPassengerTrain; //是否客车
         uTemp |= (g_taxData.m_trainType<<1)&0x02;   //列车类型  本务/补机。

         m_TaxBuffer[27] = uTemp;

        // 最大计长
         memcpy(&(m_TaxBuffer[28]),&g_taxData.m_iCountLong,sizeof(unsigned short));

         //
         //辆数
         m_TaxBuffer[30]= (uchar)g_taxData.m_iCountNumer;

         //制动机故障代码
        memcpy(&(m_TaxBuffer[31]),&g_taxData.m_breakPreassCode,sizeof(unsigned short));



         //列车管压力
        memcpy(&(m_TaxBuffer[33]),&g_taxData.m_trainPipePress,sizeof(unsigned short));

        //均衡缸压力
        memcpy(&(m_TaxBuffer[35]),&g_taxData.m_BalancePreass,sizeof(unsigned short));

        //平均管压力
        memcpy(&(m_TaxBuffer[37]),&g_taxData.m_averagePreass,sizeof(unsigned short));

        //制动缸1压力
        memcpy(&(m_TaxBuffer[39]),&g_taxData.m_breakPreass1,sizeof(unsigned short));
        //制动缸2压力
        memcpy(&(m_TaxBuffer[41]),&g_taxData.m_breakPreass2,sizeof(unsigned short));


        //总风管压力
        memcpy(&(m_TaxBuffer[43]),&g_taxData.m_allWindpreass,sizeof(unsigned short));

        //列车管充风流量
        memcpy(&(m_TaxBuffer[45]),&g_taxData.m_trainWindPreass,sizeof(unsigned short));

    //大闸指令
        m_TaxBuffer[47] = (unsigned char) g_taxData.m_bigSwitchCmd;
     //小闸指令
        m_TaxBuffer[48] = (unsigned char) g_taxData.m_smallSwitchCmd;
    //其他制动指令
        m_TaxBuffer[49] = (unsigned char) g_taxData.m_atherBreakCmd;
     //“其他制动指令”屏蔽字节
        m_TaxBuffer[50] = (unsigned char) g_taxData.m_subAtherBareakCmd;

//        车站号
         m_TaxBuffer[51] = (uchar)(g_taxData.m_stationNum&0xFF);
         m_TaxBuffer[52] = (uchar)((g_taxData.m_stationNum>>8)&0xFF);
//        司机号
         m_TaxBuffer[53] = (uchar)((g_taxData.m_driverNum)&0xFF);
         m_TaxBuffer[54] = (uchar)((g_taxData.m_driverNum>>8)&0xFF);
         m_TaxBuffer[55] = (uchar)((g_taxData.m_driverNum>>16)&0xFF);

 //区段号（交路号）
         m_TaxBuffer[56] = (uchar)g_taxData.m_segmentNum;

     // 最大总重
         memcpy(&(m_TaxBuffer[59]),&g_taxData.m_iAllWeight,sizeof(unsigned short));
//手柄级位
         memcpy(&(m_TaxBuffer[61]),&g_taxData.m_iHandleLevel,sizeof(unsigned short));


            //(没有机车信号） m_TaxBuffer[10]
//SC

   //没有 副司机号
            //机车号
      m_TaxBuffer[63] =(uchar) verifyData((unsigned char*) (m_TaxBuffer),sizeof(TPublicByte)-1);

      QByteArray tempArray((const char*)m_TaxBuffer, 64);

      emit signalSendUdpData(tempArray);

}

//
//FUCTION:模拟产生	机车号报文
//PARAMETER：
//RETURN：No
void QPrePublicPctl::pdtTrainNumberPlt()
{

//    taxData.m_trainNum = 301;
//    taxData.m_trainModel = 103;

    TTrainNumberByte tTrainNumber;
    uchar* uBuffer =(uchar*) tTrainNumber.m_uTrainNumberByte;

    memset(uBuffer,0,sizeof(TTrainNumberByte));

//报文长度
    uBuffer[0] = uBuffer[1] = 0xAA;
    ushort iUShortTemp = 0x20;
    memcpy(&(uBuffer[2]),&iUShortTemp,sizeof(unsigned short));

    // 报文类型
    uBuffer[4] =0x06;

   // 机车类型
     uBuffer[5] = (uchar) g_taxData.m_trainModel;

     memcpy(&(uBuffer[6]),&g_taxData.m_trainNum,sizeof(g_taxData.m_trainNum));

     uBuffer[31] =(uchar) verifyData((unsigned char*) (uBuffer),sizeof(TTrainNumberByte)-1);

     QByteArray tempArray((const char*)uBuffer, 32);

     emit signalSendUdpData(tempArray);

}
//
//FUCTION:模拟产生	时间报文
//PARAMETER：
//RETURN：No
void QPrePublicPctl::pdtTimePlt()
{

    //时间报文
    TTimeByte tLocalTimePlt;

    SYSTEMTIME myTime;

    uchar* uBuffer =(uchar*) tLocalTimePlt.m_uLocalTimeByte;


    memset(uBuffer,0,sizeof(TTimeByte));
//报文长度
    uBuffer[0] = uBuffer[1] = 0xAA;
    ushort iUShortTemp = 0x16;
    memcpy(&(uBuffer[2]),&iUShortTemp,sizeof(unsigned short));
   // 报文类型

    uBuffer[4] =0x04;

    GetLocalTime(&myTime);

    memcpy(&(uBuffer[5]),&myTime,sizeof(SYSTEMTIME));

    uBuffer[21] =(uchar) verifyData((unsigned char*) (uBuffer),sizeof(TTimeByte)-1);

    QByteArray tempArray((const char*)uBuffer, 22);

    emit signalSendUdpData(tempArray);
}
void QPrePublicPctl::sendUdpData(QByteArray &_bArrayData)
{


}

//
//FUCTION: 校验指定长度的数据
//PARAMETER：_data:串口缓冲数据  _lgth:长度的数据
//RETURN：校验结果
uchar verifyData(unsigned char* _data,int _lgth)
{
    unsigned char _num = 0;

    for(int i = 0; i < _lgth-1; i++) _num += _data[i];

    return _num;

}
