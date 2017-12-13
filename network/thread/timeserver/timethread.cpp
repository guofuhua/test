
#include "timethread.h"
#include <QtNetwork>
#include <QDateTime>
TimeThread::TimeThread(int socketDescriptor,QObject *parent)
    : QThread(parent), socketDescriptor(socketDescriptor)
{

}
//线程的实质
void TimeThread::run()
{
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }
    QDateTime time;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_3);
    uint time2u = QDateTime::currentDateTime().toTime_t();
    out << time2u;
    tcpSocket.write(block);
    tcpSocket.disconnectFromHost();
    tcpSocket.waitForDisconnected();
}
