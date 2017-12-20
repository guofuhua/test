#include "QTcpDataTransmit.h"
#include "Include/mydebug.h"
#include <linux/socket.h>
#include <netinet/tcp.h>

#define TCP_READ_TIMEOUT    (3000)  //3000 ms
#define TCP_HEAD_LENGTH     (8) //8字节头字节
#define DATA_LENGTH         (4) //4个字节长度
#define FLAG_BYTE_LENGTH    (2) //2字节标记位
#define CHECK_BYTE_LENGTH   (1) //异或校验位长度1字节
#define CHECK_CONNECTION_INTERVAL  (5000)  //5000 ms
#define DEVICE_NUM_LENGTH   (4) //
#define ERR_FLAG            (0xFFFF)

#define BUFFER_FLAG_INDEX   (DATA_LENGTH)
#define BUFFER_DEVICE_INDEX (BUFFER_FLAG_INDEX + FLAG_BYTE_LENGTH)
#define BUFFER_DATA_INDEX   (BUFFER_DEVICE_INDEX + DEVICE_NUM_LENGTH)
#define CHECK_BIT_INDEX(x)  (BUFFER_DATA_INDEX + (x))
#define BUFFER_TOTAL_SIZE(x)    (CHECK_BIT_INDEX(x) + CHECK_BYTE_LENGTH)

QTcpDataTransmit::QTcpDataTransmit(QObject *parent) :
    QObject(parent)
{
    ClientType = 0;
    Passive = false;
    m_bSocketConnected = false;
    connect(&m_tcpSocket,SIGNAL(disconnected()),this,SLOT(slotDisConnect()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotCheckConnection()));
    m_tcpSocket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
}

//
//FUCTION:  ＴＣＰ连接
//PARAMETER： No
//RETURN：No
bool QTcpDataTransmit::sendHead()
{
    char Head[TCP_HEAD_LENGTH];
    if (Passive)
    {
        Head[0] = WORK_MODE_RECEIVE;
    }
    else
    {
        Head[0] = WORK_MODE_SENDER;
    }
    Head[1] = ClientType;
    memcpy(&Head[2],&DeviceNum, 4);
    Head[6] = 1;
    Head[7] = 1;

    return sendTcp(Head, TCP_HEAD_LENGTH);
}

//
//FUCTION:
//PARAMETER： No
//RETURN：No
bool QTcpDataTransmit::XORcheck(char *buffer, char check_bit, int len)
{
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

//
//FUCTION:  ＴＣＰ 断开
//PARAMETER： No
//RETURN：No
void QTcpDataTransmit::Disconnect()
{
    m_tcpSocket.abort();
    m_bSocketConnected = false;
    if (Passive)
    {
        disconnect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
        m_timer.stop();
    }
}

void QTcpDataTransmit::slotReadData()
{
    qint64 length = 0;
    int data_len = 0;

    length = m_tcpSocket.bytesAvailable();
    if (length > DATA_LENGTH)
    {
        memset(m_buffer, 0, TCP_BUFFER_SIZE);

        if (length > TCP_BUFFER_SIZE)
        {
            length = TCP_BUFFER_SIZE;
        }
        qint64 nread = m_tcpSocket.read(m_buffer, length);
        if (nread < 0)
        {
            ERR("TCP read error!\n");
        }
        else if (nread == 0)
        {
            WARN("no more data is available for reading\n");
        }

        memcpy(&data_len, m_buffer, DATA_LENGTH);

        if (data_len+DATA_LENGTH == nread)
        {
            char check_bit = m_buffer[nread-1];
            char *pvalidData = m_buffer + DATA_LENGTH;
            if (XORcheck(pvalidData, check_bit, data_len - CHECK_BYTE_LENGTH))
            {
                quint16 flag;
                flag = (pvalidData[0] << 8) | pvalidData[1];
                char *pUserData = pvalidData + FLAG_BYTE_LENGTH;
                int UserDataSize = data_len - CHECK_BYTE_LENGTH - FLAG_BYTE_LENGTH;
                bool isSuccess = false;

                emit sigReadData(flag, pUserData, UserDataSize, isSuccess);

                if (isSuccess)
                {
                    slotSendReply(flag);
                }
                else
                {
                    ERR("data processing failed!!!\n");
                    slotSendReply(ERR_FLAG);
                }
            }
            else
            {
                slotSendReply(ERR_FLAG);
            }
        }
        else
        {
            ERR("total is %lld bytes, read %lld bytes, data length=%d.\n", length, nread, data_len);
        }
    }
    cleanSocket();
}

void QTcpDataTransmit::slotSendReply(quint16 flag)
{
    char buffer[FLAG_BYTE_LENGTH];
    buffer[0] = (flag>>8) & 0xff;
    buffer[1] = flag & 0xff;
    if (!sendTcp(buffer, FLAG_BYTE_LENGTH))
    {
        ERR("send reply failed, flag:0x%x\n", flag);
    }
    return;
}

void QTcpDataTransmit::slotCheckConnection()
{
    Connect();
}

void QTcpDataTransmit::slotSendData(quint16 flag, char *buf, int ok)
{
    ERR("slotSendData\n");
    m_tcpSocket.abort();
}

//
//FUCTION:  发送ＴＣＰ数据
//PARAMETER： _sendData: 发送数据, len,要发送数据的长度
//RETURN：发送 是否成功
bool QTcpDataTransmit::sendTcp(char *_sendData, int len)
{
    if ((NULL == _sendData) || (len < 1))
    {
        ERR("param is invalid!!\n");
        return false;
    }
    if (!m_tcpSocket.isValid())
    {
        ERR("socket invalid!!!!\n");
        return false;
    }

    for(int i = 0; i < 10 ;i++)
    {
        if(m_SendDataMutex.tryLock())
        {
            break;
        }
        if(i >= 5)
        {
            ERR("get write mutex error \n");
            return false;
        }
        usleep(10000);
    }

    bool bIsSuccess = false;
    qint64 write_len = m_tcpSocket.write(_sendData, len);
    bIsSuccess = m_tcpSocket.flush();
    m_SendDataMutex.unlock();
    if (len != write_len)
    {
        ERR("tcp will send %d bytes, actual send %lld bytes\n", len, write_len);
    }
    if (bIsSuccess)
    {
    }
    else
    {
        ERR("send tcp data failed\n");
    }
    return bIsSuccess;
}

//
//FUCTION:  ＴＣＰ连接
//PARAMETER： No
//RETURN：No
bool QTcpDataTransmit::Connect()
{
    if (m_bSocketConnected)
    {
        return true;
    }

    if (Passive)
    {
        connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
    }

    bool ret = ConnectServer();
    if (Passive)
    {
        ret = true;
    }

    return ret;
}

bool QTcpDataTransmit::SendData(quint16 Flag, char *Data, int Size)
{
    if (Passive)
    {
        ERR("current Passive mode is true, not support send data to server!\n");
        return false;
    }

    bool ret = SendDataOnece(Flag, Data, Size);
    if (!ret)
    {
        Disconnect();
        usleep(500000); //delay 500 ms try send data again
        ret = SendDataOnece(Flag, Data, Size);
    }
    return ret;
}

//
//FUCTION:  ＴＣＰ连接,开启keepalive属性
//PARAMETER： No
//RETURN：No
void QTcpDataTransmit::setSocketKeepAlive()
{
    int keepalive = 1; // 开启keepalive属性
    int keepidle = 5; // 如该连接在5秒内没有任何数据往来,则进行探测
    int keepinterval = 5; // 探测时发包的时间间隔为5 秒
    int keepcount = 5; // 探测尝试的次数.如果第1次探测包就收到响应了,则后4次的不再发.
    int rs = m_tcpSocket.socketDescriptor();
    setsockopt(rs, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive , sizeof(keepalive ));
    setsockopt(rs, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle , sizeof(keepidle ));
    setsockopt(rs, SOL_TCP, TCP_KEEPINTVL, (void *)&keepinterval , sizeof(keepinterval ));
    setsockopt(rs, SOL_TCP, TCP_KEEPCNT, (void *)&keepcount , sizeof(keepcount ));
}

//
//FUCTION:  ＴＣＰ连接
//PARAMETER： No
//RETURN：No
bool QTcpDataTransmit::ConnectServer()
{
    if (Passive)
    {
        m_timer.start(CHECK_CONNECTION_INTERVAL);
    }
    m_tcpSocket.connectToHost(host,port);
    //
    if (m_tcpSocket.waitForConnected(1000) == false)
    {
        m_bSocketConnected = false;
        ERR("connect %s:%d failed\n", host.toStdString().c_str(), port);
        return false;
    }
    INFO("success to connect to : %s, %d\n", host.toStdString().c_str(), port);
    m_bSocketConnected = true;

    setSocketKeepAlive();

    return sendHead();
}

void QTcpDataTransmit::slotDisConnect()
{
    INFO("QTcpDataTransmit disconnect from %s:%d\n", this->host.toStdString().c_str(), this->port);
    m_bSocketConnected = false;
}

bool QTcpDataTransmit::SendDataOnece(const quint16 Flag, const char *Data, const int Size)
{
    if (!m_bSocketConnected)
    {
        if (!Connect())
        {
            return false;
        }
    }
    if (!m_tcpSocket.isValid())
    {
        ERR("socket invalid!!!!\n");
        return false;
    }

    int total_length = BUFFER_TOTAL_SIZE(Size);
    if (total_length > TCP_BUFFER_SIZE)
    {
        ERR("Size is too large, the buffer size only %d bytes !!!\n", TCP_BUFFER_SIZE);
        return false;
    }

    memset(m_buffer, 0 , TCP_BUFFER_SIZE);

    int data_len = total_length - DATA_LENGTH;
    memcpy(m_buffer, &data_len, DATA_LENGTH);
    m_buffer[BUFFER_FLAG_INDEX] = (Flag>>8) & 0xff;
    m_buffer[BUFFER_FLAG_INDEX + 1] = Flag & 0xff;
    memcpy(&m_buffer[BUFFER_DEVICE_INDEX], &DeviceNum, DEVICE_NUM_LENGTH);
    //
    if (0 != Size)
    {
        memcpy(&m_buffer[BUFFER_DATA_INDEX], Data, Size);
    }
    //
    char check_bit = 0;
    char *pdata = m_buffer + DATA_LENGTH;
    for (int i = 0; i < data_len - CHECK_BYTE_LENGTH; i++)
    {
        check_bit ^= pdata[i];
    }
    m_buffer[CHECK_BIT_INDEX(Size)] = check_bit;

    if(!sendTcp(m_buffer, total_length))
    {
        ERR("sendTcp failed\n");
        return false;
    }

    if (m_tcpSocket.waitForReadyRead(TCP_READ_TIMEOUT))
    {
        char reply_buf[FLAG_BYTE_LENGTH];
        qint64 nread = m_tcpSocket.read(reply_buf, FLAG_BYTE_LENGTH);
        if (nread < 0)
        {
            ERR("TCP read error!\n");
        }
        else if (nread == 0)
        {
            WARN("no more data is available for reading\n");
        }
        cleanSocket();

        quint16 reply_flag = 0;
        reply_flag = (reply_buf[0] << 8) | reply_buf[1];
        if (reply_flag != Flag)
        {
            ERR("not correct reply, reply:0x%x, flag:0x%x\n", reply_flag, Flag);
            return false;
        }
    }
    else
    {
        ERR("read reply bytes---time out---\n");
        return false;
    }

    return true;
}

//
//FUCTION:
//PARAMETER：
//RETURN：No
void QTcpDataTransmit::writeByteArray(QDataStream &in, QByteArray &ba,quint32 len)
{
    ba.clear();
    if (len == 0xffffffff) return;

    const quint32 Step = 1024 * 1024;
    quint32 allocated = 0;
    do {
        int blockSize = qMin(Step, len - allocated);
        ba.resize(allocated + blockSize);
        if (in.readRawData(ba.data() + allocated, blockSize) < blockSize)
        {
            return;
        }

        allocated += blockSize;
    } while (allocated < len);
}

void QTcpDataTransmit::cleanSocket()
{
    QDataStream _receiveStream(&m_tcpSocket);
    _receiveStream.setVersion(QDataStream::Qt_4_6);

    while(m_tcpSocket.bytesAvailable() > 0){
        QByteArray receiveBytes;
        this->writeByteArray(_receiveStream,receiveBytes,m_tcpSocket.bytesAvailable());
        receiveBytes.clear();
    }
}
