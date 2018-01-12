#ifndef QUDPRECIEVETHREAD_H
#define QUDPRECIEVETHREAD_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#define SEND_BUFFER_LENGTH      (9)
#define RECEIVE_BUFFER_LENGTH   (8)

class QUdpRecieveThread : public QObject
{
    Q_OBJECT
public:
    QUdpRecieveThread(QObject *parent = 0);
    int proviseData( const char * _heatData,QByteArray * srcData);
    bool verifyData(unsigned char* _data,int _lgth);
    void PrePtlData(QByteArray tempData);
    void dealVersionData(QByteArray bAryTempData);
    void dealImageData(QByteArray bAryTempData);
    QString m_strIPAddr;
    quint16 m_uIPPort;

private:
    QUdpSocket* m_pudpSocket;

signals:
    void signalReceiveType(int);
    void signalSendData(QString);

public slots:
    void slotReceiveMessage();
    void slotSendUdpData(QByteArray &_datagram);
};

#endif // QUDPRECIEVETHREAD_H
