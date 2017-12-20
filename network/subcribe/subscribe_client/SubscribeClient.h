#ifndef SUBSCRIBECLIENT_H
#define SUBSCRIBECLIENT_H

#include <QObject>
#include <QTimer>
#include <QStringList>
#include "../SubscribeCommon/QTcpServerExt.h"
#include "QConnectClient.h"
#include "QTcpRequestClient.h"
#include "QClientInfo.h"

namespace SubScribe{

typedef struct{
    QString serverIP;
    int     serverPort;
    bool    keepAlive;//保持持续连接
    qint8   type;//订阅类型,2为临时,其他为永久
    QTcpRequestClient *registerTcp;
    bool    registerDone;
}subcribeInfo;

class SubscribeClient : public QObject
{
    Q_OBJECT
public:
    explicit SubscribeClient(QObject *parent = 0);
    bool init(int port);//初始化订阅客户端
    bool AddSubscribe(QString subscribeServerIP, int subscribeServerPort, bool keepAlive,qint8 subscribeType=0);
    bool AddSubscribe(QClientInfo &clientInfo);

private:
    void sendRegisterQuery(const subcribeInfo *info);

private:
    int m_port; //订阅客户端监听端口
    QTcpServerExt *m_server;    //订阅客户端监听端口服务
    QTimer *m_managerTM;    //申请注册订阅，直到注册成功
    QList<subcribeInfo> m_subscribeList;
    QList<QClientInfo *> m_clientInfoList;
    //保存订阅服务器列表
    QStringList m_listIPAndPorts;  //IP_Port
    QList<QConnectClient *> m_listClients;  //与订阅服务器建立连接的客户端列表

signals:
    void signalPushNotify(QByteArray,quint16,QString);    //订阅服务器的IP端口及推送的内容
    
public slots:
    void slotCheckSubscibeState();
    void slotSubscribeDone_N(QTcpRequestClient *regTcp);
    void slotNewCom(int socketDescriptor);
    void slotDeleteOneClient(QString strIPAndPort);
    void slotManagerTM();//检查TCP连接
    void slotSubscribeDone(QTcpRequestClient *regTcp);
    void slotReceiveData(QByteArray data, quint16 port, QString ip);
    
};
}//name space SubScribe

#endif // SUBSCRIBECLIENT_H
