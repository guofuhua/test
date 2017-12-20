#ifndef QTCPREQUESTCLIENT_H
#define QTCPREQUESTCLIENT_H

#include "Subscribe/SubscribeCommon/QTcpConnect.h"

class QUploadClient : public SubScribe::QTcpConnect
{
    Q_OBJECT
public:
    explicit QUploadClient(QString serverIP,  int serverPort, int deviceNum, QObject *parent = 0);
    ~QUploadClient();
    void disconnectServer();
    bool connectServer();
    bool sendData(quint16 flag, QString data);
    
private:
    void dealSubscribeResult();
    void processReadyRead(QByteArray data);

public:
    bool m_bSocketConnected;    //网络连接状态标志

    bool sendBytes_test(char *_sendData, int data_size);
    bool sendBytes_read_test(char *_sendData, int data_size);
    bool sendReply(char *startFlag);
private:
    QString m_ip;               //远端主机IP
    int m_port;                 //远端主机端口
    int m_deviceNum;


signals:
    void sigSubscribeDone(QUploadClient* regTcp);
    
public slots:
    void slotLostConnection();
    void slotReadReply();
    
};

#endif // QTCPREQUESTCLIENT_H
