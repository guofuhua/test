#include "qtcpservertest.h"

QTcpServerTest::QTcpServerTest(QObject *parent) :
    QObject(parent)
{
    m_tcpServer = new QTcpAlarmServer();
}
