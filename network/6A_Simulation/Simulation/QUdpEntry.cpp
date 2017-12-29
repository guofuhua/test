#include "QUdpEntry.h"
#include <QDebug>

extern QString bigEditDisplay;

QUdpEntry::QUdpEntry(QObject *parent) : QObject(parent)
{
     m_strIPAddr = "192.168.1.58";
     m_uIPPort = 7000;

}
void QUdpEntry::slotSendUdpData(QByteArray &_datagram)
{
    QHostAddress addr(m_strIPAddr);

    QString sendData = tr("send data:");
    sendData.append(QString(_datagram.toHex()));
    sendData.append("\n");
    bigEditDisplay.insert(0, sendData);
    emit signalSendData(bigEditDisplay);
    qDebug() << _datagram.toHex();
    m_udpSocket.writeDatagram(_datagram,addr,m_uIPPort);
}
