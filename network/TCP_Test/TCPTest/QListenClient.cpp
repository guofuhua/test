#include "QListenClient.h"
#include <QHostAddress>
#include "mydebug.h"
#include <QThread>

QListenClient::QListenClient(int nSocketDescriptor, QObject *parent) :
    QTcpConnect(parent)
{
    if(m_tcpSocket.setSocketDescriptor(nSocketDescriptor))
    {
        connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
        connect(&m_tcpSocket,SIGNAL(disconnected()),this,SLOT(slotLostConnection()));
    }
    else
    {
        ERR("================================ setSocketDescriptor error ================================\n");
        slotLostConnection();
    }
}

//
//FUCTION: 发送回复
//PARAMETER： startFlag, 回复的标志位
//RETURN：No
bool QListenClient::sendReply(quint16 startFlag)
{
    if (!m_tcpSocket.isValid())
    {
        ERR("socket invalid!!!!\n");
        return false;
    }
    if(!m_SendDataMutex.tryLock())
    {
        ERR("get write mutex error \n");
        return false;
    }

    QByteArray reply;
    bool bIsSuccess;
    QDataStream sendStream(&reply,QIODevice::WriteOnly);
    sendStream.setVersion(QDataStream::Qt_4_6);
    sendStream << startFlag;
    m_tcpSocket.write(reply);
    bIsSuccess = m_tcpSocket.waitForBytesWritten(1000);
    m_SendDataMutex.unlock();
    return bIsSuccess;
}

void QListenClient::processReadyRead(QByteArray data)
{
    int size = data.size();
    char check;
    if (size < 3)
    {
        return;
    }
    else
    {
        check = data.at(size - 1);
    }
    QDataStream _in(&data,QIODevice::ReadOnly);
    quint16 startFlag;

    _in >> startFlag;

    INFO("QListenClient::processReadyRead, startFlag:0x%x\n", startFlag);
    emit signalReadData(startFlag, data);
    sendReply(startFlag);
}

QListenClient::~QListenClient()
{
    FUNC_IN;
}

//
//FUCTION: TCP失去连接
//PARAMETER：No
//RETURN：No
void QListenClient::slotLostConnection()
{
    INFO("========================= this connect delete later ===================== \n");
    QString ipAndPort = getIPAndPort();
    emit sigCallManageDeleteClient(ipAndPort);
}

//
//FUCTION: 获取TCP连接信息字符串
//PARAMETER：No
//RETURN：当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"
QString QListenClient::getIPAndPort()
{
    QString strIPAndPort = m_tcpSocket.localAddress().toString() + tr(":%1-").arg(m_tcpSocket.localPort()) + m_tcpSocket.peerAddress().toString() + tr(":%1").arg(m_tcpSocket.peerPort());
    return strIPAndPort;
}
