#include "QTcpReplyClient.h"
#include <QHostAddress>
#include "/project/ProjectCommon/Include/mydebug.h"
#include <QThread>

namespace SubScribe{

QTcpReplyClient::QTcpReplyClient(int nSocketDescriptor, QObject *parent) :
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
//FUCTION: 解析收到的数据
//PARAMETER： data, 接收到的数据
//RETURN：No
void QTcpReplyClient::processReadyRead(QByteArray data)
{
    QDataStream _in(&data,QIODevice::ReadOnly);
    unsigned char startFlag;

    _in >> startFlag;

    INFO("QTcpReplyClient::processReadyRead, startFlag:0x%x\n", startFlag);

    switch(startFlag)
    {
    case TCP_HEAD_SUBSCRIBE_REQUEST:
    {
        dealSubscribe(_in);
        break;
    }
    default:
    {
        QString Msg = QString("unknown type %1 \n").arg(startFlag);
        ERR("%s", Msg.toStdString().c_str());
        //        QLog::getInstance()->logToSDCard("AV4EXP_GPSManage_Model.log", Msg.toStdString().c_str());
        break;
    }
    }
}

//
//FUCTION: 接收到客户端的订阅注册请求
//PARAMETER：_in,客户端信息数据
//RETURN：No
void QTcpReplyClient::dealSubscribe(QDataStream &_in)
{
    QString conIP = m_tcpSocket.peerAddress().toString();
    if(conIP.isEmpty()){
        ERR("peer address empty\n");
        return;
    }

    int port;
    bool keepConnect;
    qint8 Timelimit;
    _in >> port;
    _in >> keepConnect;//
    _in >> Timelimit;//时效性
    INFO("============== get subscribe query port %d, timelimit type 0x%x ===============\n",port, Timelimit);
    emit sigSubscribeQuery(conIP, port, keepConnect,Timelimit, this);
}

//
//FUCTION: 向客户端回复成功订阅标志
//PARAMETER：No
//RETURN：No
void QTcpReplyClient::sendSubscribeSuccess()
{
    QByteArray SendBlock;
    QDataStream sendStream(&SendBlock,QIODevice::WriteOnly);
    sendStream<<(uchar)(TCP_HEAD_SUBSCRIBE_REPLY);

    if(sendBytes(SendBlock) == false)
    {
        ERR("========================== send subscribe result error =======================\n");
    }

    return ;
}

QTcpReplyClient::~QTcpReplyClient()
{
    FUNC_IN;
}

//
//FUCTION: TCP失去连接
//PARAMETER：No
//RETURN：No
void QTcpReplyClient::slotLostConnection()
{
    INFO("========================= this connect delete later ===================== \n");
    QString ipAndPort = getIPAndPort();
    emit sigCallManageDeleteClient(ipAndPort);
}

//
//FUCTION: 获取TCP连接信息字符串
//PARAMETER：No
//RETURN：当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"
QString QTcpReplyClient::getIPAndPort()
{
    QString strIPAndPort = m_tcpSocket.localAddress().toString() + tr(":%1-").arg(m_tcpSocket.localPort()) + m_tcpSocket.peerAddress().toString() + tr(":%1").arg(m_tcpSocket.peerPort());
    return strIPAndPort;
}
}//namespace SubScribe
