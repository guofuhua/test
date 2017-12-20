#ifndef QTCPCONNECT_H
#define QTCPCONNECT_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>

namespace SubScribe{

#define ADDITIONAL_LENGTH   (5) //1字节0xAC，4字节附加订阅服务器端口号
#define TCP_HEAD_SUBSCRIBE_REQUEST  (0xAA)  //订阅客户端向服务端发送订阅请求
#define TCP_HEAD_SUBSCRIBE_REPLY    (0xAB)  //订阅客户端收到服务端返回的确认订阅请求
#define TCP_HEAD_HEARTBEAT          (0xFF)  //TCP连接心跳包
#define TCP_HEAD_SUBSCRIBE_PUSH     (0xAC)  //收到订阅服务端推送的订阅内容

class QTcpConnect : public QObject
{
    Q_OBJECT
public:
    explicit QTcpConnect(QObject *parent = 0);
    //发送数据
    bool sendBytes(QByteArray &_out);
    QTcpSocket m_tcpSocket;     //socket连接
    QMutex m_SendDataMutex;    //发送ＴＣＰ数据锁
    quint32 m_blockSize;        //data length
    
    void writeByteArray(QDataStream &in, QByteArray &ba, quint32 len);
    virtual void processReadyRead(QByteArray data)=0;//处理TCP接收到的数据
signals:
    
public slots:
    void slotReadData();
};
}//namespace SubScribe

#endif // QTCPCONNECT_H
