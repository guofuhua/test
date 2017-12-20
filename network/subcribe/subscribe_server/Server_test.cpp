#include "Server_test.h"
#include <QDateTime>
#include "../../../../Common/Log/QLog.h"

void toSDCard(const std::string& file, const char *str, bool bIsSuccess);

Server_test::Server_test(QObject *parent) :
    QObject(parent)
{
    QLog::getInstance()->setDirLog("test");
    m_pSubAlmSvr = new SubScribe::SubscribeServer();
//    m_pSubAlmSvr->moveToThread(&m_thread);
#if (0)
    m_pSubAlmSvr->init(10883,"Server_test");
#else
    m_pSubAlmSvr->init(10885,"Server_test_10885");
#endif
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotSubscribe()));
    connect(this, SIGNAL(signalBroadCast(QByteArray)), m_pSubAlmSvr, SLOT(slotBroadCast(QByteArray)));
    connect(this, SIGNAL(initSubscribe(int, QString)), m_pSubAlmSvr, SLOT(slotInit(int,QString)));
//    m_thread.start();
//    emit initSubscribe(10883, "Server_test");
    m_timer.start(10000);
}

//
//FUCTION:向订阅服务器发送报警信息
//PARAMETER：alarmInfo, 报警记录信息
//RETURN：No
void Server_test::AlarmToSubscriber()
{
    printf("AlarmToSubscriber\n");
    if (NULL == m_pSubAlmSvr) return;
    char cData[100];
    int alarmType = 1;
    QString test = "m_videoName";
    sprintf(cData,"send:%d,%s\n",alarmType,test.toStdString().c_str());
    QLog::getInstance()->logToSDCard("Alarm.log", cData);
    QByteArray array;
    QDataStream _data( &array,QIODevice::ReadWrite);
    _data.setVersion(QDataStream::Qt_4_6);
    _data<<uchar(0xFE);
    //报警类型
    _data<<alarmType;
    _data<<QDateTime::currentDateTime();//报警发生时的日期，时间
    _data<<test;//报警时录像的名称，默认情况下用报警发生时的时间当做文件名
//    bool ret = m_pSubAlmSvr->BroadCastData(array);
    emit signalBroadCast(array);

//    printf("broadCastData return:%d, %s", ret, cData);
}

void Server_test::slotSubscribe()
{
    static int i = 0;
    if (i++ == 10)
    {
        exit(0);
    }
    AlarmToSubscriber();
}
