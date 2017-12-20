#include "qtcpclienttest.h"
QTcpClientTest::QTcpClientTest(QObject *parent) :
    QObject(parent)
{
    connect(&timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
    timer.start(10000);
}

void QTcpClientTest::slotTimer()
{
    if(client.SendAlarmNotify(1006,0))
    {
        printf("send alarm notify success\n");
    }
    else
    {
        printf("send alarm notify failed\n");
    }
//    sleep(1);
}
