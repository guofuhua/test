#include "QUploadClient.h"
#include "Subscribe/SubscribeCommon/PublicFun.h"
#include "mydebug.h"
#include <QThread>
#include "SystemSetting/QSystemSetting.h"

QUploadClient::QUploadClient(QString serverIP, int serverPort, int deviceNum, QObject *parent) :
    QTcpConnect(parent)
{
    m_ip = serverIP;               //ip addresss
    m_port = serverPort;                 //ip port
    m_bSocketConnected = false;
    m_deviceNum = deviceNum;
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReadReply()));
    connect(&m_tcpSocket,SIGNAL(disconnected()),this,SLOT(slotLostConnection()));
}

QUploadClient::~QUploadClient()
{
    FUNC_IN;
}

//
//FUCTION:  ＴＣＰ有数据待处理
//PARAMETER： No
//RETURN：No
void QUploadClient::slotReadReply()
{
//    QDataStream _receiveStream(&m_tcpSocket);
//    _receiveStream.setVersion(QDataStream::Qt_4_6);

    qint64 length = 0;
    char *buffer = NULL;
    qint64 read_num = 0;
    int data_len = 0;
    char check_bit = 0;
    DEB("bytesAvailable:%lld\n", m_tcpSocket.bytesAvailable());
    if (m_tcpSocket.bytesAvailable() > 4)
    {
        length = m_tcpSocket.bytesAvailable();
        buffer = (char*)malloc(length);
        if (buffer == NULL)
        {
            ERR("malloc error \n");
            return;
        }
        memset(buffer, 0, length);

        read_num = m_tcpSocket.read(buffer, length);
        for (int len = 0; len < 4; len++)
        {
            printf("0x%x  ", buffer[len]);
        }
        printf("\n");
//        data_len = (buffer[0] << 24);
//        data_len += (buffer[1] << 16);
//        data_len += (buffer[2] << 8);
//        data_len += buffer[3];
        memcpy(&data_len, buffer, 4);
        printf("read_num:=%lld, data_len==:%d, length=:%lld\n",read_num, data_len, length);
        for (int len = 4; len < read_num; len++)
        {
            printf("0x%x  ", buffer[len]);
        }
        printf("\n");
        if (data_len+4 == read_num)
        {
            check_bit = buffer[read_num-1];
            char check_temp = 0;
            for (int i=4; i < read_num-1; i++)
            {
                check_temp ^= buffer[i];
            }
            printf("check_bit:0x%x, check_temp:0x%x\n", check_bit, check_temp);
            if (check_bit == check_temp)
            {
                if (!sendReply(&buffer[4]))
                {
                    ERR("sendReply error\n");
                }
                else
                {
                    INFO("success\n");
                }
            }
        }
    }
}

bool QUploadClient::sendReply(char *startFlag)
{
    if (!m_tcpSocket.isValid())
    {
        ERR("socket invalid!!!!\n");
        return false;
    }
    if(!m_SendDataMutex.tryLock())
    {
        ERR("get write mutex error \n");
        return false;
    }

    bool bIsSuccess = false;
    m_tcpSocket.write(startFlag, 2);
    bIsSuccess = m_tcpSocket.waitForBytesWritten(1000);
    m_SendDataMutex.unlock();
    return bIsSuccess;
}

//
//FUCTION: 解析收到的数据
//PARAMETER： data, 接收到的数据
//RETURN：No
void QUploadClient::processReadyRead(QByteArray data)
{
    QDataStream _in(&data,QIODevice::ReadOnly);

    unsigned char startFlag;

    _in >> startFlag;

    INFO("QUploadClient::processReadyRead, startFlag:0x%x\n", startFlag);

    switch(startFlag)
    {
    case TCP_HEAD_SUBSCRIBE_REPLY:
    {
        dealSubscribeResult();
        break;
    }

    default:
    {
        QString Msg = QString("unknown type %1 \n").arg(startFlag);
        ERR("%s", Msg.toStdString().c_str());
        //        QLog::getInstance()->logToSDCard("AV4EXP_GPSManage_Model.log", Msg.toStdString().c_str());
        break;
    }
    }
}

//
//FUCTION:  ＴＣＰ 断开
//PARAMETER： No
//RETURN：No
void QUploadClient::disconnectServer()
{
    m_tcpSocket.disconnectFromHost();
    if(m_tcpSocket.state() == QTcpSocket::ConnectedState){
        m_tcpSocket.waitForDisconnected(1000);
    }
    m_tcpSocket.close();
    m_bSocketConnected = false;
    m_blockSize = 0;
}

//
//FUCTION:  ＴＣＰ连接
//PARAMETER： No
//RETURN：No
bool QUploadClient::connectServer()
{
    m_tcpSocket.abort();
    m_tcpSocket.connectToHost(m_ip,m_port);
    if (m_tcpSocket.waitForConnected(1000) == false)
    {
        ERR("connect failed\n");
        return false;
    }
    INFO("success to connect to : %s, %d\n", m_ip.toStdString().c_str(), m_port);
    m_bSocketConnected = true;
    m_blockSize = 0;

    char Head[8];
    Head[0] = 2;
    Head[1] = 1;
    int number = 1123;

    memcpy(&Head[2],&number, 4);

    printf("0x%x, 0x%x, 0x%x, 0x%x\n", Head[2],Head[3], Head[4],Head[5]);
    Head[6] = 1;
    Head[7] = 1;

    m_tcpSocket.write(Head, 8);
    return true;
}

//
//FUCTION:ＴＣＰ连接建立后处理
//PARAMETER： No
//RETURN：No
void QUploadClient::slotLostConnection()
{
    INFO("============================== disconnect from host =========================\n");
    m_bSocketConnected = false;
    m_blockSize = 0;
}

//
//FUCTION: 收到订阅查询回复处理
//PARAMETER：No
//RETURN：No
void QUploadClient::dealSubscribeResult()
{
    INFO("============================== subscribe query success ========================== \n");
    emit sigSubscribeDone(this);
}

//
//FUCTION: 向服务端发送订阅请求
//PARAMETER：port,客户端的订阅接收端口;keepAlive,保持持续连接;type,订阅类型,2是临时订阅请求,关机不保存
//RETURN：No
bool QUploadClient::sendData(quint16 flag, QString data)
{
    if (!m_bSocketConnected)
    {
        if (!connectServer())
        {
            return false;
        }
    }
    if (!m_tcpSocket.isValid())
    {
        ERR("socket invalid!!!!\n");
        return false;
    }

    char *buffer = NULL;
    int malloc_size = data.size() + 6;

    buffer = (char*)malloc(malloc_size);
    if (buffer == NULL)
    {
        ERR("malloc error \n");
        return false;
    }
    memset(buffer, 0 , malloc_size);
    buffer[0] = (flag>>8)&0xff;
    buffer[1] = flag&0xff;

    quint32 deviceNum = QSystemSetting::getInstance()->getDeviceNumbet().toInt();
    TRACE("flag:0x%x, deviceNum:%u\n", flag, deviceNum);
    buffer[2] = (deviceNum >> 24)&0xff;
    buffer[3] = (deviceNum >> 16)&0xff;
    buffer[4] = (deviceNum >> 8)&0xff;
    buffer[5] = (deviceNum)&0xff;
    memcpy(buffer+6, data.toStdString().c_str(), data.size());
//    QByteArray SendBlock;
//    QDataStream sendStream(&SendBlock,QIODevice::WriteOnly);
//    sendStream.setVersion(QDataStream::Qt_4_6);
//    sendStream << flag;
//    sendStream << deviceNum;

    if(sendBytes_test(buffer, malloc_size) == false)
    {
        QString strError= QString("QUploadClient::sendScribeRequest:---- \n");
        ERR("%s", strError.toStdString().c_str());
        return false;
    }

    if (m_tcpSocket.waitForReadyRead(3000))
    {
        TRACE("---ok---\n");
        QDataStream _receiveStream(&m_tcpSocket);
        _receiveStream.setVersion(QDataStream::Qt_4_6);
        quint16 reply_flag = 0;

        INFO("bytesAvailable:%lld\n", m_tcpSocket.bytesAvailable());
        while(m_tcpSocket.bytesAvailable() > 0){
            INFO("bytes Avali:%lld\n", m_tcpSocket.bytesAvailable());
            _receiveStream >> reply_flag;
            INFO("reply_flag:0x%x, bytesAvailable:%lld\n", reply_flag, m_tcpSocket.bytesAvailable());
            QByteArray receiveBytes;
            this->writeByteArray(_receiveStream,receiveBytes, m_tcpSocket.bytesAvailable());
            receiveBytes.clear();
        }
        if (reply_flag == flag)
        {
            DEB("equal----\n");
        }
        else
        {
            ERR("not correct reply\n");
        }
    }
    else
    {
        TRACE("---time out---\n");
        return false;
    }

    return true;
}

bool QUploadClient::sendBytes_test(char *_sendData, int data_size)
{
    if (!m_tcpSocket.isValid())
    {
        ERR("socket invalid!!!!\n");
        return false;
    }
    int i = 0;
    for(i = 0; i < 10 ;i++)
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
    char *data = NULL;
    int malloc_size = data_size + 9;
    int data_len = data_size+1;
    data = (char*)malloc(malloc_size);
    if (data == NULL)
    {
        ERR("malloc error \n");
        return false;
    }
    memset(data, 0, malloc_size);

    char test[4] = {0x1,0x1,0x1,0x1};
    memcpy(data, test, 4);

    char length[4];
    length[0] = (data_len >> 24)&0xff;
    length[1] = (data_len >> 16)&0xff;
    length[2] = (data_len >> 8)&0xff;
    length[3] = (data_len)&0xff;

    memcpy((data+4), length, 4);
    memcpy((data+8), _sendData, data_size);
    char check_bit = 0;
    for (i = 8; i < malloc_size - 1; i++)
    {
        check_bit ^= data[i];
    }
    data[malloc_size - 1] = check_bit;

    bool bIsSuccess = false;
//    QByteArray SendBlock = QByteArray(test);
//    SendBlock.append(length);
//    printf("0x%x, 0x%x, 0x%x, 0x%x\n", length[0], length[1], length[2], length[3]);
//    QDataStream sendStream(&SendBlock,QIODevice::WriteOnly);
//    sendStream.setVersion(QDataStream::Qt_4_6);
//    sendStream.writeRawData(_sendData.data(),_sendData.size());
//    SendBlock.append(_sendData);
//    printf("0x%x\n", SendBlock.at(0));
    for (int len = 0; len < malloc_size; len++)
    {
        printf("0x%x  ", data[len]);
    }
    printf("\n");
    int write_num = m_tcpSocket.write(data, malloc_size);
    printf("write_num:%d\n", write_num);
    bIsSuccess = m_tcpSocket.waitForBytesWritten();
    m_SendDataMutex.unlock();
    return bIsSuccess;
}

bool QUploadClient::sendBytes_read_test(char *_sendData, int data_size)
{
    if (!m_bSocketConnected)
    {
        if (!connectServer())
        {
            return false;
        }
    }

    if (!m_tcpSocket.isValid())
    {
        ERR("socket invalid!!!!\n");
        return false;
    }
    int i = 0;
    for(i = 0; i < 10 ;i++)
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
    char *data = _sendData;
    int malloc_size = data_size;

//    char length[4];
//    length[0] = (data_len >> 24)&0xff;
//    length[1] = (data_len >> 16)&0xff;
//    length[2] = (data_len >> 8)&0xff;
//    length[3] = (data_len)&0xff;

//    memcpy((data+4), length, 4);
//    if (NULL != _sendData)
//    {
//    memcpy((data+8), _sendData, data_size);
//    }
//    char check_bit = 0;
//    for (i = 8; i < malloc_size - 1; i++)
//    {
//        check_bit ^= data[i];
//    }
//    data[malloc_size - 1] = check_bit;

    bool bIsSuccess = false;
//    QByteArray SendBlock = QByteArray(test);
//    SendBlock.append(length);
//    printf("0x%x, 0x%x, 0x%x, 0x%x\n", length[0], length[1], length[2], length[3]);
//    QDataStream sendStream(&SendBlock,QIODevice::WriteOnly);
//    sendStream.setVersion(QDataStream::Qt_4_6);
//    sendStream.writeRawData(_sendData.data(),_sendData.size());
//    SendBlock.append(_sendData);
//    printf("0x%x\n", SendBlock.at(0));
    for (int len = 0; len < malloc_size; len++)
    {
        printf("0x%x  ", data[len]);
    }
    printf("\n");
    int write_num = m_tcpSocket.write(data, malloc_size);
    printf("write_num:%d\n", write_num);
    bIsSuccess = m_tcpSocket.waitForBytesWritten();
    m_SendDataMutex.unlock();


    if (m_tcpSocket.waitForReadyRead(3000))
    {
        TRACE("---ok---\n");
        QDataStream _receiveStream(&m_tcpSocket);
        _receiveStream.setVersion(QDataStream::Qt_4_6);

        INFO("bytesAvailable:%lld\n", m_tcpSocket.bytesAvailable());
        while(m_tcpSocket.bytesAvailable() > 0){
            //INFO("bytes Avali:%lld\n", m_tcpSocket.bytesAvailable());
            QByteArray receiveBytes;
            this->writeByteArray(_receiveStream,receiveBytes, m_tcpSocket.bytesAvailable());
            INFO(" bytesAvailable:%lld\n", m_tcpSocket.bytesAvailable());
            for (int len = 0; len < receiveBytes.size(); len++)
            {
                printf("0x%x  ", receiveBytes.at(len));
            }
            printf("\n");
            receiveBytes.clear();
        }
    }
    else
    {
        TRACE("---time out---\n");
        return false;
    }
    return bIsSuccess;
}

