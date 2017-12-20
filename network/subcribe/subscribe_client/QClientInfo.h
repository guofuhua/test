#ifndef QCLIENTINFO_H
#define QCLIENTINFO_H

#include <QObject>
#include "QTcpRequestClient.h"
namespace SubScribe{

class QClientInfo : public QObject
{
    Q_OBJECT
public:
    explicit QClientInfo(QString ServerIP, int ServerPort, bool keepAlive,qint8 type, QObject *parent = 0);
    QString serverIP;
    int     serverPort;
    bool    m_keepAlive;
    qint8   m_type;//订阅类型,2为临时,其他为永久
    void    (*receiveData)(QByteArray data);
    QTcpRequestClient *registerTcp;
    bool    registerDone;
    void sendRegisterQuery(int port) const;
    void receiveReadData(QByteArray data,quint16 port,QString ip);

signals:
    void signalReadData(QByteArray data,quint16 port,QString ip);

public slots:
    
};
}//namespace SubScribe

#endif // QCLIENTINFO_H
