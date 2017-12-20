#ifndef QUPLOADRUNRECORD_H
#define QUPLOADRUNRECORD_H

#include <QObject>
#include "TcpDataTransmit/QTcpDataTransmit.h"

class QUploadRunRecord : public QObject
{
    Q_OBJECT
public:
    explicit QUploadRunRecord(QObject *parent = 0);
    QTcpDataTransmit *m_tcp;
    void StartRun();
    
signals:
    void signalStartRun();
    
public slots:
    void slotNewFile(QByteArray data);
    void slotStartRun();
    
};

#endif // QUPLOADRUNRECORD_H
