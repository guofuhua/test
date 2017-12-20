#ifndef QCONNECTCLIENT_H
#define QCONNECTCLIENT_H

#include "../SubscribeCommon/QTcpConnect.h"
#include <QHostAddress>
namespace SubScribe{

class QTcpReplyClient : public QTcpConnect
{
    Q_OBJECT
public:
    explicit QTcpReplyClient(int nSocketDescriptor, QObject *parent = 0);
    void processReadyRead(QByteArray data);
    QString getIPAndPort();//当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"
    void sendSubscribeSuccess();
    ~QTcpReplyClient();
    
private:
    void dealSubscribe(QDataStream &_in);

signals:
    void sigCallManageDeleteClient(QString m_strIPPort);
    void sigSubscribeQuery(QString ip, int port, bool keepCon,qint8 timelimit, QTcpReplyClient*);
    
public slots:
    void slotLostConnection();
};
}//namespace SubScribe

#endif // QCONNECTCLIENT_H
