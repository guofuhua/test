#include "QSerialReadThread.h"
#include "Include/mydebug.h"
#include "LocalCommon.h"
#include "Tools/QTFSystemUtils.h"

QSerialReadThread::QSerialReadThread(QObject *parent) :
    QThread(parent)
{
}

void QSerialReadThread::readData()
{
    static unsigned char buf[RECEIVE_BUFFER_LENGTH];
    memset(buf, 0, sizeof(buf));
    int read_byte = readSerialPortData(m_serial,buf,RECEIVE_BUFFER_LENGTH);
//    DEB("receivelenth--:%d\n", read_byte);
    if(read_byte > 0)//收到数据
    {
        read_data_times++;
        DEB("receivelenth--:%d\n", read_byte);
        m_TempData.insert(m_TempData.size(),(char*)buf, read_byte);
        //        //test---begin
        //        QByteArray bAryTempData;
        //        bAryTempData.insert(bAryTempData.size(),(char*)buf, read_byte);
        //        QString strDate =( bAryTempData.right(read_byte).toHex()) + "\n";
        //        DEB("TAX date::%s", strDate.toStdString().c_str());
        //        //test---end
    }
    if (0 != read_data_times) {
        read_times++;
    }
}

void QSerialReadThread::resetReadTimes()
{
    read_times = 0;
    read_data_times = 0;
}

void QSerialReadThread::checkData()
{
    bool ret = false;
    if (SERIAL_READ_DIR == m_status) {
        ret = isLKJDirReadOK(m_TempData);
        if (ret) {
            emit signalDirInfo(m_TempData);
            resetReadTimes();
            m_status = SERIAL_IDLE;
            INFO("read LKJ dir OK.\n");
        }
    }
    else if (SERIAL_READ_FILE == m_status) {
        ret = isLKJFileReadOK(m_TempData);
        if (ret) {
            emit signalFile(m_TempData.mid(6));
            resetReadTimes();
            m_status = SERIAL_IDLE;
            INFO("read LKJ File OK.\n");
        }
    } else if (SERIAL_READ_SYNC_FILE == m_status) {
        ret = isLKJFileReadOK(m_TempData);
        if (ret) {
            emit signalSyncFile(m_TempData.mid(6));
            resetReadTimes();
            m_status = SERIAL_IDLE;
            INFO("read sync LKJ File OK.\n");
        }
    }
    if (read_times - read_data_times > 10) {
        readWrongFlag(READ_INTERRUPT_FLAG);
        resetReadTimes();
        m_status = SERIAL_IDLE;
    }
    if ((0 == read_times) && (0 == read_data_times)){
        m_readStatus = WAIT_FOR_READ;
    } else if (read_times == read_data_times) {
        m_readStatus = SERIAL_READING;
    } else if (read_times > read_data_times) {
        m_readStatus = READ_INTERRUPT;
    } else {
        m_readStatus = IDLE;
    }

    DEB("run_flag:%d, status:%d, reading status:%d, file size:%d, read times:%d, read_data_times:%d, data size:%d\n", \
        run_flag, m_status, m_readStatus, m_fileSize, read_times, read_data_times, m_TempData.size());
}

void QSerialReadThread::run()
{
    FUNC_IN;
    m_serial = new CSerialportDevice("/dev/ttyS1");
    if (!openComPort(m_serial)) {
        ERR("openComPort failed!!\n");
        return;
    }

    //setGpio(SERIAL_RW_GPIO, SERIAL_READ_ENABLE);
    read_data_times = 0;
    read_times = 0;
    run_flag = true;
    m_status = 0;
    while (run_flag)
    {
        usleep(250000);
        //TRACE("mutex lock[in]\n");
        mutex.lock();

        if (SERIAL_READ_DIR == m_status) {
            readData();
            checkData();
        }
        else if ((SERIAL_READ_FILE == m_status) || (SERIAL_READ_SYNC_FILE == m_status)) {
            readData();
            checkData();
        }
        //TRACE("mutex unlock[out]\n");
        mutex.unlock();
    }
}

//
//FUCTION: 通过串口从MCU收指定长度的数据
//PARAMETER：_buf:串口缓冲数据  _size:长度的数据
//RETURN：实际收到的数据长度
int QSerialReadThread::readSerialPortData(CSerialportDevice* Serialport,unsigned char *_buf,int _size)
{
    FUNC_IN;
    if(_buf == NULL)return 0;
    int _left = _size;
    int _ret = 0;

    while(_left>0)
    {
        _ret = Serialport->readData(_buf,_left);

        if(_ret < 0)
        {
            DEB("receive data error\n");
            return _ret;
        }
        else if(_ret == 0)
        {
//            DEB("receive data timeout\n");
            return _size - _left;
        }
        //        DEB("receive data  %d\n",_ret);
        _left -= _ret;
        _buf += _ret;
    }
    return _size - _left;
}

bool QSerialReadThread::isLKJDirReadOK(QByteArray AryTempData)
{
    FUNC_IN;
    if (AryTempData.size() < 2)
    {
        return false;
    }
    quint16 wrong_flag = (AryTempData.at(0)<<8)|AryTempData.at(1);
    DEB("0x%x\n", wrong_flag);
    if (READ_DIR_FLAG == wrong_flag)
    {
        if (AryTempData.size() < 7)
        {
            return false;
        }
        int len = AryTempData.at(4) << 8 | AryTempData.at(5);
        DEB("len = %d, data size = %d\n", len, AryTempData.size());
        if (len + 7 > AryTempData.size()) {
            FUNC_OUT;
            return false;
        }
        char *pbuf = AryTempData.data() + 6;
        if (!XORcheck(pbuf, AryTempData.at(len + 6), len)) {
            FUNC_OUT;
            return false;
        }
    } else {
        readWrongFlag(wrong_flag);
        FUNC_OUT;
        return false;
    }
    return true;
}

void QSerialReadThread::slotSetNewState(int state)
{
    FUNC_IN;
    //TRACE("mutex lock[in]\n");
    mutex.lock();
    m_TempData.clear();
    m_serial->clearFlush();
    m_status = state;
    DEB("set state = %d\n", state);
    m_fileSize = 0;
    //TRACE("mutex unlock[out]\n");
    mutex.unlock();
}


bool QSerialReadThread::openComPort(CSerialportDevice* Serialport)
{
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

//
//FUCTION:
//PARAMETER： No
//RETURN：No
bool QSerialReadThread::XORcheck(char *buffer, char check_bit, int len)
{
    FUNC_IN;
    if ((NULL == buffer) || (len < 1))
    {
        ERR("param is invalid!!\n");
        return false;
    }

    char XOR_bit = 0;
    for (int i = 0; i < len; i++)
    {
        XOR_bit ^= buffer[i];
    }

    if (XOR_bit == check_bit)
    {
        return true;
    }
    else
    {
        ERR("XORcheck failed, check bit 0x%x, calculate value 0x%x !!\n", check_bit, XOR_bit);
        return false;
    }
}

void QSerialReadThread::readWrongFlag(quint16 wrong_flag)
{
    emit signalWrongFlag(wrong_flag);
    ERR("read wrong flag=0x%x\n", wrong_flag);
    resetReadTimes();
}

bool QSerialReadThread::isLKJFileReadOK(QByteArray AryTempData)
{
    FUNC_IN;
    if (AryTempData.size() < 2)
    {
        return false;
    }
    if (AryTempData.size() < m_fileSize)
    {
        DEB("fileSize = %d, data size = %d\n", m_fileSize, AryTempData.size());
        return false;
    }

    quint16 wrong_flag = (AryTempData.at(0)<<8)|AryTempData.at(1);
    DEB("0x%x\n", wrong_flag);
    if (READ_FILE_FLAG == wrong_flag)
    {
        if (AryTempData.size() < 6)
        {
            return false;
        }

        int len = AryTempData.at(4) << 8 | AryTempData.at(5);
        len |= AryTempData.at(2) << 24;
        len |= AryTempData.at(3) << 16;
        m_fileSize = len + 6;
        DEB("len = %d, data size = %d, fileSize = %d\n", len, AryTempData.size(), m_fileSize);
        if (AryTempData.size() < m_fileSize)
        {
            DEB("fileSize = %d, data size = %d\n", m_fileSize, AryTempData.size());
            return false;
        }
    } else {
        readWrongFlag(wrong_flag);
        FUNC_OUT;
        return false;
    }
    return true;
}
