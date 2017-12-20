#ifndef QSERIALWRITE_H
#define QSERIALWRITE_H

#include <QObject>
#include "SerialPort/CSerialportDevice.h"
#include "Entity/LKJRuntimeTypes.h"

class QSerialWrite : public QObject
{
    Q_OBJECT
public:
    explicit QSerialWrite(QObject *parent = 0);
    bool openComPort(CSerialportDevice *Serialport);
    bool readLKJDirInfo(CSerialportDevice *Serialport);
    bool AnalysisOneFileInfo(QByteArray data, LKJFileInfo &fileInfo);
    bool readLKJFile(CSerialportDevice *Serialport, LKJFileInfo &fileInfo);
    void intToByte(int number, unsigned char *buf, unsigned int len);
    void readFileHead(LKJFileInfo &fileInfo, unsigned char *buf, int offset = 0);
    void setGpio(int gpio, int status);
    bool writeData(CSerialportDevice *Serialport, unsigned char *_buf,int _size);
    bool readSpecialLKJFile(CSerialportDevice *Serialport, LKJFileInfo &fileInfo, int offset);

signals:
    
public slots:
    
};

#endif // QSERIALWRITE_H
