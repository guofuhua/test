#ifndef QSERIALREADTHREAD_H
#define QSERIALREADTHREAD_H

#include <QThread>
#include <QMutex>
#include "SerialPort/CSerialportDevice.h"
#include "LocalCommon.h"

class QSerialReadThread : public QThread
{
    Q_OBJECT
public:
    explicit QSerialReadThread(QObject *parent = 0);
    void run();
    CSerialportDevice *m_serial;
    bool run_flag;
    int m_status;
    EM_SerialReadStatus m_readStatus;
    int m_fileSize;
    QByteArray m_TempData;
    QMutex mutex;
    int read_data_times;
    int read_times;
    
    int readSerialPortData(CSerialportDevice *Serialport, unsigned char *_buf, int _size);
    bool isLKJDirReadOK(QByteArray AryTempData);
    bool openComPort(CSerialportDevice *Serialport);
    bool XORcheck(char *buffer, char check_bit, int len);
    bool isLKJFileReadOK(QByteArray AryTempData);
    void readData();
    void resetReadTimes();
    void readWrongFlag(quint16 wrong_flag);
    void checkData();
signals:
    void signalDirInfo(QByteArray data);
    void signalFile(QByteArray data);
    void signalSyncFile(QByteArray data);
    void signalWrongFlag(quint16 flag);
    
public slots:
    void slotSetNewState(int state);
    
};

#endif // QSERIALREADTHREAD_H
