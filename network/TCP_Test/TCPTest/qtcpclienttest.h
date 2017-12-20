#ifndef QTCPCLIENTTEST_H
#define QTCPCLIENTTEST_H

#include <QObject>
#include <QTimer>
#include "TCPAlarm/QTcpAlarmClient.h"

class QTcpClientTest : public QObject
{
    Q_OBJECT
public:
    explicit QTcpClientTest(QObject *parent = 0);
    QTimer timer;
    QTcpAlarmClient client;
    
signals:
    
public slots:
    void slotTimer();
};

#endif // QTCPCLIENTTEST_H
