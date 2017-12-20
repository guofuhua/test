#ifndef QTCPSERVERTEST_H
#define QTCPSERVERTEST_H

#include <QObject>
#include "TCPAlarm/QTcpAlarmServer.h"

class QTcpServerTest : public QObject
{
    Q_OBJECT
public:
    explicit QTcpServerTest(QObject *parent = 0);
    QTcpAlarmServer *m_tcpServer;
    
signals:
    
public slots:
    
};

#endif // QTCPSERVERTEST_H
