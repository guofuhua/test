#ifndef QUDPRECIEVETHREAD_H
#define QUDPRECIEVETHREAD_H

#include <QObject>
#include <QThread>
#include <QtNetwork/QtNetwork>
#define SEND_BUFFER_LENGTH      (9)
#define RECEIVE_BUFFER_LENGTH   (8)

class QUdpRecieveThread : public QThread
{
    Q_OBJECT
public:
    QUdpRecieveThread();
    void run();
    int proviseData( const char * _heatData,QByteArray * srcData);
    bool verifyData(unsigned char* _data,int _lgth);
    void PrePtlData(QByteArray tempData);
    void dealVersionData(QByteArray bAryTempData);
    void dealImageData(QByteArray bAryTempData);
public:
private:
    QUdpSocket* m_pudpSocket;

signals:

public slots:
    void slotReceiveMessage();
};

#endif // QUDPRECIEVETHREAD_H
