#ifndef QTCPDATATRANSMIT_H
#define QTCPDATATRANSMIT_H

#include <QObject>
#include <QTcpSocket>
#include <QMutex>
#include <QDateTime>
#include <QTimer>

#define WORK_MODE_SENDER    (0x1)
#define WORK_MODE_RECEIVE   (0x2)
#define TCP_BUFFER_SIZE    (10240)  //10k

class QTcpDataTransmit : public QObject
{
    Q_OBJECT
public:
    explicit QTcpDataTransmit(QObject *parent = 0);
    bool Connect();//连接服务器，连接前必需设置好DeviceNum,ClientType,Passive,host,port
    bool SendData(quint16 Flag, char *Data, int Size);//发送标志位与数据，Size是Data的长度
    void Disconnect();//断开与服务器的连接

private:
    bool sendHead();
    bool SendDataOnece(const quint16 Flag, const char *Data, const int Size);//发送标志位与数据，Size是Data的长度
    bool sendTcp(char *_sendData, int len);
    bool XORcheck(char *buffer, char check_bit, int len);
    void writeByteArray(QDataStream &in, QByteArray &ba, quint32 len);
    bool ConnectServer();
    void cleanSocket();
    void setSocketKeepAlive();

public:
    int DeviceNum;      //设备号
    char ClientType;    //客户端类型
    bool Passive;       //True就表示为被动模式，为False表示为主动模式，默认为False
    QString host;       //远端主机IP
    int port;           //远端主机端口
    QTcpSocket m_tcpSocket; //socket连接

private:
    QMutex m_SendDataMutex; //发送ＴＣＰ数据锁
    bool m_bSocketConnected;    //网络连接状态标志
    char m_buffer[TCP_BUFFER_SIZE];
    QTimer m_timer; //
    
signals:
    void sigReadData(quint16, char*, int, bool &);//Qt::DirectConnection 连接时使用直接连接

private slots:
    void slotDisConnect();
    void slotReadData();
    void slotSendReply(quint16 flag);
    void slotCheckConnection();
    void slotSendData(quint16 flag,char* buf,int ok);
    
};

#endif // QTCPDATATRANSMIT_H
