#ifndef QUDPENTRY_H
#define QUDPENTRY_H
#include <QtNetwork/QtNetwork>
#include <QObject>

class QUdpEntry : public QObject
{
    Q_OBJECT
public:
    static QUdpEntry* getInstance()
      {
          static QUdpEntry instance;
          return &instance;
      }
private:
    explicit QUdpEntry(QObject *parent = 0);

public:
    QUdpSocket m_udpSocket;
    QString m_strIPAddr;
    quint16 m_uIPPort;
signals:
    void signalSendData(QString);

public slots:
    void slotSendUdpData(QByteArray &_datagram);
};

#endif // QUDPENTRY_H
