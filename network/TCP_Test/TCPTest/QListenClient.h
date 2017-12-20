#ifndef QLISTENCLIENT_H
#define QLISTENCLIENT_H

#include "Subscribe/SubscribeCommon/QTcpConnect.h"
#include <QHostAddress>
class QListenClient : public SubScribe::QTcpConnect
{
    Q_OBJECT
public:
    explicit QListenClient(int nSocketDescriptor, QObject *parent = 0);
    void processReadyRead(QByteArray data);
    ~QListenClient();
    QString getIPAndPort();//当前程序IP端口-远端程序IP端口,如"127.0.0.1:20886-127.0.0.1:36385"

    bool sendReply(quint16 startFlag);
signals:
    void signalReadData(quint16 flag, QByteArray receiveBytes);
    void sigCallManageDeleteClient(QString m_strIPPort);
    
public slots:
    void slotLostConnection();
};

#endif // QLISTENCLIENT_H
