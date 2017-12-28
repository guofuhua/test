#include "QUdpEntry.h"

QUdpEntry::QUdpEntry(QObject *parent) : QObject(parent)
{
     m_strIPAddr = "192.168.1.58";
     m_uIPPort = 7000;

}
void QUdpEntry::slotSendUdpData(QByteArray &_datagram)
{
    QHostAddress addr(m_strIPAddr);

    m_udpSocket.writeDatagram(_datagram,addr,m_uIPPort);
}
