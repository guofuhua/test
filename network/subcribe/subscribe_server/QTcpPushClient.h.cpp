#include "QTcpPushClient.h"
#include "../SubscribeCommon/PublicFun.h"
#include "/project/ProjectCommon/Include/mydebug.h"
#include <QThread>

namespace SubScribe{
QTcpPushClient::QTcpPushClient(QString ip, int port, bool keepAlive, QObject *parent) :
    QTcpConnect(parent)
{
    m_ip = ip;               //ip addresss
    m_port = port;                 //ip port
    m_bSocketConnected = false;
    m_bKeepAlive = keepAlive;
    m_lastReceiveTm = PublicFun::getSystemUsedTime();
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
    connect(&m_tcpSocket,SIGNAL(disconnected()),this,SLOT(slotLostConnection()));
}

QTcpPushClient::~QTcpPushClient()
{
    FUNC_IN;
}

//
//FUCTION: 解析收到的数据
//PARAMETER： data, 接收到的数据
//RETURN：No
void QTcpPushClient::processReadyRead(QByteArray data)
{
    QDataStream _in(&data,QIODevice::ReadOnly);

    m_lastReceiveTm = PublicFun::getSystemUsedTime();
    unsigned char startFlag;

    _in >> startFlag;

    INFO("QTcpPushClient::processReadyRead, startFlag:0x%x\n", startFlag);

    switch(startFlag)
    {
    case TCP_HEAD_HEARTBEAT:
    {
        INFO("receive heartbeat package!\n");
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
//FUCTION: 发送心跳包
//PARAMETER：No
//RETURN：No
bool QTcpPushClient::sendBeatHeat()
{
    QByteArray _sendBytes;
    QDataStream _sendStream( &_sendBytes,QIODevice::ReadWrite);
    _sendStream.setVersion(QDataStream::Qt_4_6);
    _sendStream<< uchar(TCP_HEAD_HEARTBEAT);
    return sendData(_sendBytes);
}

//
//FUCTION: 发送ＴＣＰ数据
//PARAMETER：data,数据
//RETURN：No
bool QTcpPushClient::sendData(QByteArray data)
{
    bool ret = sendBytes(data);
    if(ret)
    {
        m_lastReceiveTm = PublicFun::getSystemUsedTime();
    }
    return ret;
}

//
//FUCTION:  ＴＣＰ 断开
//PARAMETER： No
//RETURN：No
void QTcpPushClient::disconnectServer()
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
bool QTcpPushClient::connectServer()
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
//FUCTION:ＴＣＰ失去连接后处理
//PARAMETER： No
//RETURN：No
void QTcpPushClient::slotLostConnection()
{
    INFO("============================== disconnect from host =========================\n");
    m_bSocketConnected = false;
    m_lastReceiveTm = PublicFun::getSystemUsedTime();
    m_blockSize = 0;
}

}//namespace SubScribe
