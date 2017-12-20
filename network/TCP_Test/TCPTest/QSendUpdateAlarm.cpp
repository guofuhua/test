#include "QSendUpdateAlarm.h"
#include "qfiledeleteline.h"
#include "TCPAlarm/QTcpAlarmClient.h"
#include "Log/QLog.h"
#include "predefine.h"

QSendUpdateAlarm::QSendUpdateAlarm(QObject *parent) :
    QThread(parent)
{
}

void QSendUpdateAlarm::run()
{
    int alarm = 0;
    QDateTime time;
    QFileDeleteLine errFile;
    QTcpAlarmClient tcpClient;
    //如果存在升级错误文件，向报警模块发送升级异常
    while (errFile.isFileExists())
    {
        sleep(1);
        //从文件中读取第一行，并解析出其中的参数
        if (!errFile.readOneLine(alarm, time))
        {
            ERR("QSendUpdateAlarm::QFileDeleteLine::readOneLine failed!!\n");
            QLog::getInstance()->logToSDCard("SendAlarm.log", "QSendUpdateAlarm::QFileDeleteLine::readOneLine failed!!\n");
            break;
        }
        //向报警TCP服务器端发送报警信息
        INFO("success---alarm type:%d, time:%s----------\n", alarm, time.toString("yyyy-MM-dd hh:mm:ss").toStdString().c_str());
        if (!tcpClient.SendAlarmNotify(alarm, ALARM_NO_VIDEO, time))
        {
            ERR("QSendUpdateAlarm::QTcpAlarmClient::SendAlarmNotify failed!!\n");
            QLog::getInstance()->logToSDCard("SendAlarm.log", "QSendUpdateAlarm::QTcpAlarmClient::SendAlarmNotify failed!!\n");
            continue;
        }
        //成功发送报警,删除已发送的报警信息
        if (!errFile.deleteOneLine())
        {
            ERR("QSendUpdateAlarm::QFileDeleteLine::deleteOneLine failed!!\n");
            QLog::getInstance()->logToSDCard("SendAlarm.log", "QSendUpdateAlarm::QFileDeleteLine::deleteOneLine failed!!\n");
        }
    }
    //退出线程
    quit();
}
