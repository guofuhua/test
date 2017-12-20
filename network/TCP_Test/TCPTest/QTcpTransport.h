#ifndef QTCPTRANSPORT_H
#define QTCPTRANSPORT_H

#include <QObject>
#include "QUploadClient.h"
#include "QListenClient.h"
#include "Subscribe/SubscribeCommon/QTcpServerExt.h"
#include <QTimer>

class QTcpTransport : public QObject
{
    Q_OBJECT
public:
    explicit QTcpTransport(QObject *parent = 0);
    bool send(quint16 flag, QString data);
    SubScribe::QTcpServerExt m_server;
    QUploadClient *m_upload;
    QTimer *m_timeridwho;
    
signals:
    void TcpDataNotify(quint16, QByteArray);
    
public slots:
    void slotDeleteOneClient(QString strIPAndPort);
    void slotNewCom(int socketDescriptor);
    void slotDataNotify(quint16 flag, QByteArray data);
    void slotTimerOut();
};

#endif // QTCPTRANSPORT_H
