#include "QTcpRequestClient.h"
#include "../SubscribeCommon/PublicFun.h"
#include "/project/ProjectCommon/Include/mydebug.h"
#include <QThread>

namespace SubScribe{
QTcpRequestClient::QTcpRequestClient(QString serverIP, int serverPort, QObject *parent) :
    QTcpConnect(parent)
{
    m_ip = serverIP;               //ip addresss
    m_port = serverPort;                 //ip port
    m_bSocketConnected = false;
    m_lastReceiveTm = PublicFun::getSystemUsedTime();
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
    connect(&m_tcpSocket,SIGNAL(disconnected()),this,SLOT(slotLostConnection()));
}

QTcpRequestClient::~QTcpRequestClient()
{
    FUNC_IN;
}

//
//FUCTION: 解析收到的数据
//PARAMETER： data, 接收到的数据
//RETURN：No
void QTcpRequestClient::processReadyRead(QByteArray data)
{
    QDataStream _in(&data,QIODevice::ReadOnly);

    m_lastReceiveTm = PublicFun::getSystemUsedTime();
    unsigned char startFlag;

    _in >> startFlag;

    INFO("QTcpRequestClient::processReadyRead, startFlag:0x%x\n", startFlag);

    switch(startFlag)
    {
    case TCP_HEAD_SUBSCRIBE_REPLY:
    {
        dealSubscribeResult();
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
//FUCTION:  ＴＣＰ 断开
//PARAMETER： No
//RETURN：No
void QTcpRequestClient::disconnectServer()
{
    m_tcpSocket.disconnectFromHost();
    if(m_tcpSocket.state() == QTcpSocket::ConnectedState){
        m_tcpSocket.waitForDisconnected(1000);
    }
    m_tcpSocket.close();
    m_bSocketConnected = false;
    m_lastReceiveTm = PublicFun::getSystemUsedTime();
    m_blockSize = 0;
}

//
//FUCTION:  ＴＣＰ连接
//PARAMETER： No
//RETURN：No
bool QTcpRequestClient::connectServer()
{
    m_tcpSocket.abort();
    m_tcpSocket.connectToHost(m_ip,m_port);
    if (m_tcpSocket.waitForConnected(1000) == false)
    {
        ERR("connect failed\n");
        return false;
    }
    INFO("success to connect to : %s, %d\n", m_ip.toStdString().c_str(), m_port);
    m_bSocketConnected = true;
    m_blockSize = 0;
    m_lastReceiveTm = PublicFun::getSystemUsedTime();
    return true;
}

//
//FUCTION:ＴＣＰ连接建立后处理
//PARAMETER： No
//RETURN：No
void QTcpRequestClient::slotLostConnection()
{
    INFO("============================== disconnect from host =========================\n");
    m_bSocketConnected = false;
    m_lastReceiveTm = PublicFun::getSystemUsedTime();
    m_blockSize = 0;
}

//
//FUCTION: 收到订阅查询回复处理
//PARAMETER：No
//RETURN：No
void QTcpRequestClient::dealSubscribeResult()
{
    INFO("============================== subscribe query success ========================== \n");
    emit sigSubscribeDone(this);
}

//
//FUCTION: 向服务端发送订阅请求
//PARAMETER：port,客户端的订阅接收端口;keepAlive,保持持续连接;type,订阅类型,2是临时订阅请求,关机不保存
//RETURN：No
bool QTcpRequestClient::sendScribeRequest(int port, bool keepAlive, qint8 type)
{
    QByteArray _sendBytes;
    QDataStream _sendStream( &_sendBytes,QIODevice::ReadWrite);
    _sendStream.setVersion(QDataStream::Qt_4_6);
    _sendStream << uchar(TCP_HEAD_SUBSCRIBE_REQUEST);
    _sendStream << port;
    _sendStream << keepAlive;
    _sendStream << type;
    if(sendBytes(_sendBytes) == false)
    {
        QString strError= QString("QTcpRequestClient::sendScribeRequest:--send register query false--- \n");
//        QLog::getInstance()->logToSDCard("subscribeClient.log", strError.toStdString().c_str());
        ERR("%s", strError.toStdString().c_str());
        disconnectServer();
        return false;
    }
    return true;
}
}//namespace SubScribe
