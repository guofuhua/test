#include "QConnectClient.h"
#include <QHostAddress>
#include "/project/ProjectCommon/Include/mydebug.h"
#include <QThread>

namespace SubScribe{

QConnectClient::QConnectClient(int nSocketDescriptor, QObject *parent) :
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
void QConnectClient::processReadyRead(QByteArray data)
{
    QDataStream _in(&data,QIODevice::ReadOnly);
    unsigned char startFlag;

    _in >> startFlag;

    INFO("QConnectClient::processReadyRead, startFlag:0x%x\n", startFlag);

    switch(startFlag)
    {
    case TCP_HEAD_HEARTBEAT:
    {
        dealBeatHeart();
        break;
    }
    case TCP_HEAD_SUBSCRIBE_PUSH:
    {
        int sz = data.size() - ADDITIONAL_LENGTH;   //数据减去发送时附加的1字节0xAC和4字节订阅服务器端口号
        dealSubscribeData(_in, sz);
        sendBeatHeat();
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

QConnectClient::~QConnectClient()
{
    FUNC_IN;
}

//
//FUCTION: 收到心跳包处理
//PARAMETER：No
//RETURN：No
void QConnectClient::dealBeatHeart()
{
    sendBeatHeat();
    return;
}

//
//FUCTION: 收到订阅推送数据处理
//PARAMETER：_in,收到的数据; sz,数据长度
//RETURN：No
void QConnectClient::dealSubscribeData(QDataStream &_in, int sz)
{
    QString ip = m_tcpSocket.peerAddress().toString();
    quint32 port;
    _in >> port;
    QByteArray array;
    array.resize(sz);
    _in.readRawData(array.data(), sz);
    emit signalReadData(array, port, ip);
}

//
//FUCTION: 发送心跳包
//PARAMETER：No
//RETURN：No
bool QConnectClient::sendBeatHeat()
{
    QByteArray _sendBytes;
    QDataStream _sendStream( &_sendBytes,QIODevice::ReadWrite);
    _sendStream.setVersion(QDataStream::Qt_4_6);
    _sendStream<< uchar(TCP_HEAD_HEARTBEAT);
    return sendBytes(_sendBytes);
}

//
//FUCTION: TCP失去连接
//PARAMETER：No
//RETURN：No
void QConnectClient::slotLostConnection()
{
    INFO("========================= this connect delete later ===================== \n");
    QString ipAndPort = getIPAndPort();
    emit sigCallManageDeleteClient(ipAndPort);
}

//
//FUCTION: 获取TCP连接信息字符串
//PARAMETER：No
//RETURN：当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"
QString QConnectClient::getIPAndPort()
{
    QString strIPAndPort = m_tcpSocket.localAddress().toString() + tr(":%1-").arg(m_tcpSocket.localPort()) + m_tcpSocket.peerAddress().toString() + tr(":%1").arg(m_tcpSocket.peerPort());
    return strIPAndPort;
}
}//namespace SubScribe
