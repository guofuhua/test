#include "QUploadRunRecord.h"
#include "SystemSetting/QSystemSetting.h"
#include "Log/QLog.h"
#include "Include/mydebug.h"

void log_callback(const char* file, const char* message);

QUploadRunRecord::QUploadRunRecord(QObject *parent) :
    QObject(parent)
{
    m_tcp = NULL;
    connect(this, SIGNAL(signalStartRun()), this, SLOT(slotStartRun()));
}

void QUploadRunRecord::StartRun()
{
    emit signalStartRun();
}

void QUploadRunRecord::slotNewFile(QByteArray data)
{
    TRACE("---m_tcp:%p--\n", m_tcp);
    if (NULL == m_tcp) {
        slotStartRun();
    }
    char FileNo = data.size() / 16;
    data.insert(0, FileNo);
#if (DEBUG)
    DEB("%s\n", data.toHex().data());
#endif
    bool ret = m_tcp->SendData(0xffa3, data.data(), data.size());
    if (ret) {
        INFO("Send SOC File Info to Server!\n");
    } else {
        ERR("Send Error!\n");
    }
}

void QUploadRunRecord::slotStartRun()
{
    TRACE("---m_tcp:%p--\n", m_tcp);
    if (NULL != m_tcp)
        return;
    m_tcp = new QTcpDataTransmit(this);
    m_tcp->DeviceNum = QSystemSetting::getInstance()->getDeviceNumbet().toInt();      //设备号
    m_tcp->ClientType = 105;    //客户端类型
    m_tcp->Passive = false;       //True就表示为被动模式，为False表示为主动模式，默认为False
    m_tcp->host = QSystemSetting::getInstance()->getTCPServerIp();       //远端主机IP
    m_tcp->port = QSystemSetting::getInstance()->getTCPServerPort();           //远端主机端口
    m_tcp->callback_Log = log_callback;
}

void log_callback(const char* file, const char* message)
{
    QLog::getInstance()->logToSDCard(file, message);
}
