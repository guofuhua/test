#include "QTcpConnect.h"
#include <QHostAddress>
#include "/project/ProjectCommon/Include/mydebug.h"

namespace SubScribe{

QTcpConnect::QTcpConnect(QObject *parent) :
    QObject(parent)
{
    m_blockSize = 0;
}


//
//FUCTION:  发送ＴＣＰ数据
//PARAMETER： _sendData: 发送数据
//RETURN：发送 是否成功
bool QTcpConnect::sendBytes(QByteArray &_sendData)
{
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
    QByteArray SendBlock;
    QDataStream sendStream(&SendBlock,QIODevice::WriteOnly);
    sendStream.setVersion(QDataStream::Qt_4_6);
    sendStream<<(quint32) _sendData.size();
    sendStream.writeRawData(_sendData.data(),_sendData.size());
    m_tcpSocket.write(SendBlock);
    bIsSuccess = m_tcpSocket.waitForBytesWritten();
    m_SendDataMutex.unlock();
    return bIsSuccess;
}

//
//FUCTION:  ＴＣＰ有数据待处理
//PARAMETER： No
//RETURN：No
void QTcpConnect::slotReadData()
{
    QDataStream _receiveStream(&m_tcpSocket);
    _receiveStream.setVersion(QDataStream::Qt_4_6);

    while(m_tcpSocket.bytesAvailable() > 0){
        if(m_blockSize==0) //如果是刚开始接收数据
        {
            //判断接收的数据是否有两字节，也就是文件的大小信息
            //如果有则保存到blockSize变量中，没有则返回，继续接收数据
            if(m_tcpSocket.bytesAvailable() < (int)sizeof(quint32)) return;
            _receiveStream >> m_blockSize;
        }

        //如果没有得到全部的数据，则返回，继续接收数据
        if(m_tcpSocket.bytesAvailable() < m_blockSize) return;
        QByteArray receiveBytes;
        this->writeByteArray(_receiveStream,receiveBytes,m_blockSize);
        processReadyRead(receiveBytes);
        receiveBytes.clear();
        m_blockSize = 0;
    }
}

//
//FUCTION:把数据分包发送
//PARAMETER：
//RETURN：No
void QTcpConnect::writeByteArray(QDataStream &in, QByteArray &ba,quint32 len)
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

}//namespace SubScribe
