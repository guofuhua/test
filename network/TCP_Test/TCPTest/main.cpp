#include <QCoreApplication>
//#include "Log/QLog.h"
//#include "qtcpclienttest.h"
//#include "qtcpservertest.h"
//#include "qfiledeleteline.h"
//#include "qupdateapp.h"
//#include "QSendUpdateAlarm.h"
//#include "QTcpTransport.h"
//#include "QTcpDataTransmit.h"
//#include "/project/ProjectCommon/TCPAlarm/QTcpAlarmClient.h"
#include "/project/ProjectCommon/TcpDataTransmit/QTcpDataTransmit.h"
#include "QTestSlot.h"
#define TCP_PROTOCOL_DEVICE_TYPE    (1)
#define TCP_PROTOCOL_VERSION        (200)

#define TCP_SEND_UPGRADE_FLAG   (0xFFA2)
#define BUFFER_VERSION_INDEX    (TCP_PROTOCOL_DEVICE_TYPE)
#define TCP_PROTOCOL_BUFF_LEN   (BUFFER_VERSION_INDEX + TCP_PROTOCOL_VERSION)
#define MIN(x,y)	((x)<(y)?(x):(y))
#define UPDATE_FILE ("../AV4Pack_2.3.06.0808.tar.gz")
//标志位
#define BEAT_HEAND_FLAG (0XFFF2)
//客户端类型
#define CLIENT_TYPE     (9)


typedef struct
{
    unsigned int    t_trainType;//车型
    unsigned int    t_trainNumber;//车号
    int             t_iFlashSpace;//Flash　乘余空间
    int             t_iSdSpace;//ＳＤ　乘余空间
    unsigned char   t_bSdState;//ＳＤ卡状态
    unsigned char   t_bAV4NetState;//AV4网络状态

    unsigned char   t_bWifiState;//Wifi状态
    unsigned char   t_i3GSignal;//3G
    unsigned char   t_bLocationState;//定位状态
    char            t_sLongitude[20];//经度
    char            t_sLatitude[20];//纬度
    unsigned char 	t_bTaxPortState;//ＴＡＸ接口状态
    unsigned char	t_iPragmeModelCunt;//运行模块数量
    char            t_cSerialNumber[10];//车次
}THeartBeat_State;

#if (0)
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //创建日志实例
    QLog::getInstance()->setDirLog("test_server");
    QLog::getInstance()->logToSDCard("pro to run !!===============================\n");
    QTcpServerTest test;
    return a.exec();
}
#else
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //创建日志实例
//    QLog::getInstance()->setDirLog("test_client");
//    QLog::getInstance()->logToSDCard("pro to run !!===============================\n");
//    QTcpClientTest test;
//    QUpdateApp app;
//    if (app.checkUpgradeFile(UPDATE_FILE))
//    {
//        exit(0);
//    }
//    QFileDeleteLine test;
//    test.addOneERR(1007, QDateTime::currentDateTime());
//    int alarm = 0;
//    QDateTime time;
//    if (test.isFileExists())
//    {
//        if (test.getUpdateAlarm(alarm, time))
//        {
//            printf("success---alarm type:%d, time:%s----------\n", alarm, time.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str());
//        }else{
//            printf("failed-------------\n");
//        }
//    }
//    else
//    {
//        printf("no alarm-------------\n");
//    }
//    QSendUpdateAlarm test;
//    test.start();
//    exit(0);
//    QTcpTransport test;
//    QString test_str = QString("test");
//    QString data = QString("test");
//    test.send(0xff01, data);
//    char test[10] = {0x1,0x1,0x0, 0x2,0x3};
//    QByteArray arr = QByteArray(test);
//    for (int len = 0; len < arr.size(); len++)
//    {
//        printf("0x%x  ", arr.at(len));
//    }
//    printf("\n");
//    QTestSlot test_slot;
//    QTcpDataTransmit test;
//    test.Passive = true;
//    test.DeviceNum = 1234;
//    test.ClientType = 1;
//    test.host = "192.168.1.93";
//    test.port = 9876;
//    QTcpDataTransmit test;
//    test.DeviceNum = 1234;
//    test.ClientType = 'A';
//    test.Passive = true;
//    test.host = "192.168.1.93";
//    test.port = 8080;

//    QObject::connect(&test_slot, SIGNAL(sigSendData(quint16,char*,int)), &test, SLOT(slotAbort()));
////    QObject::connect(&test, SIGNAL(sigReadData(quint16,char*,int, bool &)), &test_slot, SLOT(slotReadData(quint16,char*,int, bool &)));
//    if (test.Connect())
//    {
//        printf("connect server successful\n");
//    }
//    test_slot.sendStart(30);

////    QTcpAlarmClient test;
////    test.SendAlarmNotify(1017);
////    return 0;

    QTcpDataTransmit *m_tcp = new QTcpDataTransmit();
    m_tcp->DeviceNum = 1234;      //设备号
    m_tcp->ClientType = CLIENT_TYPE;//(17);    //客户端类型

    m_tcp->Passive = false;       //True就表示为被动模式，为False表示为主动模式，默认为False
    m_tcp->host = "101.201.145.136";       //远端主机IP
    m_tcp->port = 9801;           //远端主机端口

    QString version;
    char buff[TCP_PROTOCOL_BUFF_LEN];

    memset(buff, 0, TCP_PROTOCOL_BUFF_LEN);

    buff[0] = 0;    //设备类型
    version = "2.5.0.0922";
    memcpy(&buff[BUFFER_VERSION_INDEX], version.toStdString().c_str(), MIN(TCP_PROTOCOL_VERSION,version.size()));
#if (DEBUG)
    QString test = QString(QByteArray::fromRawData(&buff[BUFFER_VERSION_INDEX], TCP_PROTOCOL_VERSION));
    DEB("device type:%d, version:%s, client type:%d\n", buff[0], test.toStdString().c_str(), m_tcp->ClientType);
#endif
    sleep(3);
//    if (!m_tcp->Connect())
//    {
//        sleep(5);
//        qDebug() << m_tcp->Connect();
//    }
//    sleep(5);
//    qDebug() << m_tcp->SendData(TCP_SEND_UPGRADE_FLAG, buff, TCP_PROTOCOL_BUFF_LEN);
//    sleep(5);
//    qDebug() << m_tcp->SendData(TCP_SEND_UPGRADE_FLAG, buff, TCP_PROTOCOL_BUFF_LEN);

    THeartBeat_State heartState;
  //  return true;
     //机车型号
    heartState.t_trainType = 207;
      //机车号
    heartState.t_trainNumber = 3017;
    //经度
    memcpy(heartState.t_sLongitude,buff,20); //0
    //纬度;
    memcpy(heartState.t_sLatitude,buff,20); //0


    heartState.t_bLocationState = (uchar)1;

     //3G网络信号强度
    heartState.t_i3GSignal = 20;

    heartState.t_bWifiState = 1;
    heartState.t_bAV4NetState = 1;

    heartState.t_iPragmeModelCunt =10;
    int iSDSpace = 8888888;
    heartState.t_bSdState = (bool)iSDSpace;
    heartState.t_iSdSpace = (int) iSDSpace;
     //检查TAX数据状态
    heartState.t_bTaxPortState =  (uchar) 1;

    // 磁盘剩余空间 d
    heartState.t_iFlashSpace = 6666666;

    memset(heartState.t_cSerialNumber,0,sizeof(heartState.t_cSerialNumber));
    memcpy(heartState.t_cSerialNumber ,"K801",4); //0

    QByteArray _sendData;

    _sendData.insert(0,(char*)(&heartState.t_trainType), 73);

    sleep(5);
    m_tcp->SendData(BEAT_HEAND_FLAG, buff, 109);//发送标志位与数据，Size是Data的长度
    sleep(6);
    m_tcp->SendData(BEAT_HEAND_FLAG, buff, 109);//发送标志位与数据，Size是Data的长度
 //   return true;
    return a.exec();
}
#endif
