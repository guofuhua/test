#ifndef QCONNECTCLIENT_H
#define QCONNECTCLIENT_H

#include "../SubscribeCommon/QTcpConnect.h"
#include <QHostAddress>
namespace SubScribe{

class QConnectClient : public QTcpConnect
{
    Q_OBJECT
public:
    explicit QConnectClient(int nSocketDescriptor, QObject *parent = 0);
    void processReadyRead(QByteArray data);
    ~QConnectClient();
    
    void dealBeatHeart();
    void dealSubscribeData(QDataStream &_in, int sz);
    bool sendBeatHeat();
    QString getIPAndPort();//当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"
signals:
    void signalReadData(QByteArray data,quint16 port,QString ip);
    void sigSubscribeDone(QTcpConnect* regTcp);
    void sigCallManageDeleteClient(QString m_strIPPort);
    
public slots:
    void slotLostConnection();
};
}//namespace SubScribe

#endif // QCONNECTCLIENT_H
