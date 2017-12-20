#ifndef SUBSCRIBECLIENT_H
#define SUBSCRIBECLIENT_H

#include <QObject>
#include <QTimer>
#include <QStringList>
#include "..//SubscribeCommon/QTcpServerExt.h"
#include "QTcpReplyClient.h"
#include "QTcpPushClient.h"

namespace SubScribe{
typedef struct{
    QString ip;
    int     port;
    bool    keepAlive;
    QTcpPushClient* conSocket;
}subscribeItem;


class SubscribeServer : public QObject
{
    Q_OBJECT
public:
    explicit SubscribeServer(QObject *parent = 0);
    void init(int port, const char *proName);//初始化订阅服务器
    bool AddSubscribe(QString subscribeServerIP, int subscribeServerPort, bool keepAlive,qint8 subscribeType=0);

private:
    bool BroadCastData(QByteArray data);
    void initSubscribeList();
    void saveSubscribe(QString ip, int port, bool keepCon);
    void addSubscribeItem(QString ip, int port, bool keepAlive, QTcpPushClient* conSocket);

private:
    int m_port; //订阅客户端监听端口
    QTcpServerExt *m_server;    //订阅客户端监听端口服务
    QTimer *m_managerTM;    //申请注册订阅，直到注册成功
    QList<subscribeItem> m_subscribeList;
    //保存订阅服务器列表
    QStringList m_listIPAndPorts;  //IP_Port
    QList<QTcpReplyClient *> m_listClients;  //与订阅服务器建立连接的客户端列表
    QString m_config_file;

signals:
    void signalPushNotify(QString, quint16, QByteArray);    //订阅服务器的IP端口及推送的内容
    
public slots:
    void slotNewCom(int socketDescriptor);
    void slotDeleteOneClient(QString strIPAndPort);
    void slotManagerTM();//检查TCP连接
    void slotSubscribeQuery(QString ip, int port, bool keepCon, qint8 type, QTcpReplyClient *pClientJob);
    void slotBroadCast(QByteArray data);
    void slotInit(int port, QString proName);
    
};
}//name space SubScribe

#endif // SUBSCRIBECLIENT_H
