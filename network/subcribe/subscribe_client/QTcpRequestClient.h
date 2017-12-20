#ifndef QTCPREQUESTCLIENT_H
#define QTCPREQUESTCLIENT_H

#include "../SubscribeCommon/QTcpConnect.h"

namespace SubScribe{

class QTcpRequestClient : public QTcpConnect
{
    Q_OBJECT
public:
    explicit QTcpRequestClient(QString serverIP,  int serverPort, QObject *parent = 0);
    ~QTcpRequestClient();
    void disconnectServer();
    bool connectServer();
    bool sendScribeRequest(int port, bool keepAlive, qint8 type);
    
private:
    void dealSubscribeResult();
    void processReadyRead(QByteArray data);

public:
    bool m_bSocketConnected;    //网络连接状态标志
    int m_lastReceiveTm;

private:
    QString m_ip;               //远端主机IP
    int m_port;                 //远端主机端口


signals:
    void sigSubscribeDone(QTcpRequestClient* regTcp);
    
public slots:
    void slotLostConnection();
    
};
}//namespace SubScribe

#endif // QTCPREQUESTCLIENT_H
