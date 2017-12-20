#include "QSerialWrite.h"
#include "Include/mydebug.h"
#include <QFile>
#include <QDataStream>
#include "LocalCommon.h"
#include "Tools/QTFSystemUtils.h"
#include <QDebug>

QSerialWrite::QSerialWrite(QObject *parent) :
    QObject(parent)
{
    FUNC_IN;
}

bool QSerialWrite::openComPort(CSerialportDevice* Serialport)
{
    FUNC_IN;
    int nOpenCount = 0;
    while(1)
    {
        usleep(100000);
        if (nOpenCount > 10)
        {
            return false;
        }
        if(Serialport->openDevice() == false)
        {
            nOpenCount++;
            continue;
        }
        if(Serialport->setDevice(38400,8,CSerialportDevice::ODD_VALIDATION,1) == false)
        {
            Serialport->closeDevice();
            nOpenCount++;
            continue;
        }
        return true;
    }
}

bool QSerialWrite::readLKJDirInfo(CSerialportDevice *Serialport)
{
    FUNC_IN;
    static unsigned char buf[READ_FILE_HEAD_LEN];
    bool ret = false;
    memset(buf, 0, sizeof(buf));

//    if (!openComPort(Serialport)) {
//        ERR("-------------------------------open comport error--------------------------\n");
//        return false;
//    }

    buf[0] = (READ_DIR_FLAG>>8) & 0xff;
    buf[1] = READ_DIR_FLAG & 0xff;

    ret = writeData(Serialport, buf, SERIAL_FLAG_LEN);

    return ret;
}

bool QSerialWrite::AnalysisOneFileInfo(QByteArray data, LKJFileInfo &fileInfo)
{
    FUNC_IN;
    fileInfo.nTrainNo = data.at(0) + (data.at(1) << 8) + ((data.at(13) & 0xc0) << 10);
    fileInfo.nDriverNo = data.at(4) + (data.at(5) << 8) + (data.at(12) << 16);
    int month = data.at(9) >> 4;
    int day = data.at(10) & 0x1f;
    int hour = ((data.at(10) >> 5) + (data.at(11) << 3)) & 0x1f;
    int minute = data.at(11) >> 2;
    QString datetime = QString("%1-%2-%3-%4").arg(month).arg(day).arg(hour).arg(minute);
    fileInfo.Time = QDateTime::fromString(datetime, "M-d-h-m");
    fileInfo.nfileNo = data.at(2);
    fileInfo.size = (data.at(6) + (data.at(7) << 8)) << 4;
    fileInfo.nAutoStop = data.at(8);
    fileInfo.DumpFlag = data.at(9) & 0x01;
    fileInfo.name = QString("%1_%2_%3_%4").arg(fileInfo.nfileNo).arg(fileInfo.nTrainNo).arg(fileInfo.nDriverNo).arg(datetime);
    DEB("train=%d, driver=%d, file=%d, size=%d, stop=%d, flag=%d\n", fileInfo.nTrainNo, fileInfo.nDriverNo,fileInfo.nfileNo, fileInfo.size, fileInfo.nAutoStop, fileInfo.DumpFlag);
    qDebug() << fileInfo.name << fileInfo.Time.toString(Qt::ISODate);

    return true;
}

bool QSerialWrite::readLKJFile(CSerialportDevice *Serialport, LKJFileInfo &fileInfo)
{
    FUNC_IN;
    static unsigned char buf[READ_FILE_HEAD_LEN];
    bool ret = false;
    memset(buf, 0, sizeof(buf));

//    if (!openComPort(Serialport)) {
//        ERR("-------------------------------open comport error--------------------------\n");
//        return false;
//    }

    readFileHead(fileInfo, buf);
    ret = writeData(Serialport, buf, READ_FILE_HEAD_LEN);
    return ret;
}

bool QSerialWrite::readSpecialLKJFile(CSerialportDevice *Serialport, LKJFileInfo &fileInfo, int offset)
{
    FUNC_IN;
    static unsigned char buf[READ_FILE_HEAD_LEN];
    bool ret = false;
    memset(buf, 0, sizeof(buf));

//    if (!openComPort(Serialport)) {
//        ERR("-------------------------------open comport error--------------------------\n");
//        return false;
//    }

    readFileHead(fileInfo, buf, offset);
    ret = writeData(Serialport, buf, READ_FILE_HEAD_LEN);
    return ret;
}

void QSerialWrite::intToByte(int number, unsigned char *buf, unsigned int len)
{
    FUNC_IN;
    if ((NULL == buf) ||  (len < 1))
    {
        ERR("param is invalid!!, buf=%p, len=%d\n", buf, len);
        return;
    }
    memset(buf, 0, len);
    unsigned int i = len - 1;
    for (i = 0; (i < len) && (i < sizeof(int)); i++) {
        buf[len - 1 - i] = (number >> (i*8)) & 0xff;
    }
}


void QSerialWrite::readFileHead(LKJFileInfo &fileInfo, unsigned char *buf, int offset)
{
    FUNC_IN;
    buf[0] = (READ_FILE_FLAG>>8) & 0xff;
    buf[1] = READ_FILE_FLAG & 0xff;
    buf[2] = fileInfo.nfileNo;
    intToByte(offset, &buf[3], 4);
//    intToByte(fileInfo.nTrainNo, &buf[2], 3);
//    intToByte(fileInfo.nDriverNO, &buf[5], 3);
//    buf[8] = 0;
//    buf[9] = 0;
//    buf[10] = 0;
//    intToByte(0, &buf[11], 3);
//    unsigned char XOR_bit = 0;
//    for (int i = 0; i < 14; i++)
//    {
//        XOR_bit ^= buf[i];
//    }
//    buf[14] = XOR_bit;
}


void QSerialWrite::setGpio(int gpio, int status)
{
    char buf[50];
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "./dm3730_gpio_test.test %d %d", gpio, status);
//    DEB("%s\n", buf);
    int ret = QTFSystemUtils::getInstance()->runShellCommand(buf);
    if (0 != ret)
    {
        ERR("setGpio return %d!!\n", ret);
    }
}

bool QSerialWrite::writeData(CSerialportDevice *Serialport, unsigned char *_buf, int _size)
{
    setGpio(SERIAL_RW_GPIO, SERIAL_WRITE_ENABLE);
    int write_byte = Serialport->writeData(_buf, _size);
    setGpio(SERIAL_RW_GPIO, SERIAL_READ_ENABLE);
    if (_size != write_byte) {
        ERR("write_byte=%d, all byte = %d\n", write_byte, _size);
        return false;
    } else {
        return true;
    }
}
