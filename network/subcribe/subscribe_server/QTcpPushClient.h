#ifndef QTCPREQUESTCLIENT_H
#define QTCPREQUESTCLIENT_H

#include "../SubscribeCommon/QTcpConnect.h"

namespace SubScribe{

class QTcpPushClient : public QTcpConnect
{
    Q_OBJECT
public:
    explicit QTcpPushClient(QString ip,  int port, bool keepAlive, QObject *parent = 0);
    ~QTcpPushClient();
    void disconnectServer();
    bool connectServer();
    bool sendBeatHeat();
    bool sendData(QByteArray data);
    
private:
    void processReadyRead(QByteArray data);

public:
    bool m_bSocketConnected;    //网络连接状态标志
    int m_lastReceiveTm;
    bool m_bKeepAlive;

private:
    QString m_ip;               //远端主机IP
    int m_port;                 //远端主机端口

signals:
    
public slots:
    void slotLostConnection();
    
};
}//namespace SubScribe

#endif // QTCPREQUESTCLIENT_H
