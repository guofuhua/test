#include "QClientInfo.h"
#include "/project/ProjectCommon/Include/mydebug.h"

namespace SubScribe{
QClientInfo::QClientInfo(QString ServerIP, int ServerPort, bool keepAlive, qint8 type, QObject *parent) :
    QObject(parent)
{
    serverIP = ServerIP;
    serverPort = ServerPort;
    m_keepAlive = keepAlive;
    m_type = type;//订阅类型,2为临时,其他为永久
    registerTcp = NULL;
    registerDone = false;
}

//
//FUCTION: 向服务端发送订阅请求
//PARAMETER：info,连接类型，是否保持连接，是否是临时连接
//RETURN：No
void QClientInfo::sendRegisterQuery(int port) const
{
    if (NULL != registerTcp)
        registerTcp->sendScribeRequest(port, m_keepAlive, m_type);
}

void QClientInfo::receiveReadData(QByteArray data, quint16 port, QString ip)
{
    DEB("ip:%s, port:%d\ndata:%s\n",ip.toStdString().c_str(), port, QString(data.toHex()).toStdString().c_str());
    emit signalReadData(data,port,ip);
}

}//namespace SubScribe
