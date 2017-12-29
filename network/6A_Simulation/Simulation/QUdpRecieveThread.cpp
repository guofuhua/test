#include "QUdpRecieveThread.h"
#include "comment.h"
TVersionInfo g_tVersionInfo;
TImageInfo g_tImageInfo;//视频图片报文
QUdpRecieveThread::QUdpRecieveThread(QObject *parent) : QObject(parent)
{
//    QThread* epThread = new QThread(this);
//    m_pudpSocket = new  QUdpSocket(this);
//    m_pudpSocket->bind(QHostAddress::Any,7000);
//    connect( m_pudpSocket,SIGNAL(readyRead()),this,SLOT(slotReceiveMessage()),Qt::DirectConnection);


//    this->moveToThread(epThread);

//    epThread->start();
    m_pudpSocket = new  QUdpSocket(this);
    m_pudpSocket->bind(QHostAddress::Any,7000);
    connect( m_pudpSocket,SIGNAL(readyRead()),this,SLOT(slotReceiveMessage()));
}

void QUdpRecieveThread::slotReceiveMessage()
{

    qDebug()<<"from thread slot:" <<QThread::currentThreadId();

   while ( m_pudpSocket->hasPendingDatagrams())
   {
       QByteArray datagram;
       datagram.resize( m_pudpSocket->pendingDatagramSize());
       QHostAddress sender;
       quint16 senderPort;
        m_pudpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
       qDebug() << "slotReceiveMessage" <<datagram.size(); //processTheDatagram(datagram);<<sender.toString()<<senderPort
      QString strDate =( datagram.right(datagram.size()).toHex());

      qDebug()<<"date::"<<strDate;
      PrePtlData(datagram);
   }



}

void QUdpRecieveThread::PrePtlData(QByteArray tempData)
{
    qDebug() << "PrePtlData" << tempData.toHex();
    while(RECEIVE_BUFFER_LENGTH <= tempData.length())
    {
        //解析 指定 协议 版本报文-视频监控板卡 (17)  ;视频图片报文
        if( proviseData("\xAA\xAA",&tempData) >= 0)
        {
            continue;
        }

        //如果没有得相关解析 ，丢去一字节
        tempData.remove(0,1);
    }

}
//
//FUCTION: 解析具体的数据
//PARAMETER：_data:串口缓冲数据  _lgth:长度的数据
//RETURN：校验结果
int QUdpRecieveThread::proviseData( const char * _heatData,QByteArray * srcData)
{
    int iLength=0;

    for(int i = 0 ;srcData->length() >= (i+4) ;i++)
    {
        iLength = srcData->at(i+2) +srcData->at(i+3)*256;
//        qDebug("src %x, %x, %x", srcData->at(i), srcData->at(i+1), srcData->at(i+4));
        if(((char) _heatData[0]==srcData->at(i) )&&((char) _heatData[1]==srcData->at(i+1) ))
        {
//            qDebug() << "src " << (uint)srcData->at(i) << (uint)srcData->at(i+1);
            if(verifyData((unsigned char*)srcData->mid(i,iLength).data(),iLength))
            {
                uchar type =srcData->at(i+4) ;
                switch(type)
                {
                    case 0x01:   //版本报文
                    {
                       dealVersionData((srcData->mid(i,iLength+i)));
                        srcData->remove(0,i+iLength);
                    }
                    break;
                    case 0x51://视频图片报文
                    {

                        dealImageData((srcData->mid(i,iLength+i)));
                        srcData->remove(0,i+iLength);
                    }
                    break;
//                    case 0xFE:
//                    {
//                        srcData->remove(0,i+iLength);
//                        return i;
//                    }
//                    break;
                }
                emit signalReceiveType(type);
                return i;
            }
            //qDebug()<<"xxxxxxxx=========="<< i;

        }
    }
    return -1;
}

//
//FUCTION: 处理版本报文
//PARAMETER：
//RETURN：
void QUdpRecieveThread::dealVersionData(QByteArray _bAryTempData)
{


    QString strDate =( _bAryTempData.right(_bAryTempData.size()).toHex());

    qDebug()<<"version date::"<<strDate;
    int  iLength = _bAryTempData.at(2) +_bAryTempData.at(3)*256;
    uchar natedata = _bAryTempData.at(5);
    if((iLength==8) && (natedata==17))
    {
        uchar Version = _bAryTempData.at(6);
        g_tVersionInfo.m_uHardwaveVersion = (Version>>4)&0x0f;
        g_tVersionInfo.m_uSoftwaveVersion = (Version&0x0f);
        qDebug() << "dealVersionData" << g_tVersionInfo.m_uHardwaveVersion<<g_tVersionInfo.m_uSoftwaveVersion;
    }


}
//
//FUCTION: 处理视频图片报文
//PARAMETER：
//RETURN：
void QUdpRecieveThread::dealImageData(QByteArray _bAryTempData)
{
    QString strDate =( _bAryTempData.right(_bAryTempData.size()).toHex()) + "\n";


    qDebug()<<"image date::"<<strDate;

    int  iLength = _bAryTempData.at(2) +_bAryTempData.at(3)*256;
    uchar uTemp;
    if(iLength>=12)
    {
        uTemp =  _bAryTempData.at(6);
        g_tImageInfo.m_bHardDiskFault = (uTemp>>2)&0x01;//硬盘故障
        g_tImageInfo.m_bCard2Fault = (uTemp>>1)&0x01;;//采集卡2故障
        g_tImageInfo.m_bCard1Fault = (uTemp)&0x01;;//采集卡1故障

        //摄像头自检 -16通道 每位一通道
        g_tImageInfo.m_uVideoCheck = (ushort) (_bAryTempData.at(7) +_bAryTempData.at(8)*256);

        g_tImageInfo.m_uImageChannel = _bAryTempData.at(9);//图像通道
        g_tImageInfo.m_uImageType =  _bAryTempData.at(10);//图像格式
    }
    if(iLength>12)//图像数据（N 字节)
    {
       g_tImageInfo.m_bytePictureData=_bAryTempData.mid(11,iLength-12);//图像数据 数据

    }

}
//
//FUCTION: 校验指定长度的数据
//PARAMETER：_data:串口缓冲数据  _lgth:长度的数据
//RETURN：校验结果
bool QUdpRecieveThread::verifyData(unsigned char* _data,int _lgth)
{
    unsigned char _num = 0;
//    if ((uint)_lgth < 1025) {
//        QByteArray test((char *)_data, _lgth);
//        qDebug() << "verifyData" << test.toHex();
//    } else {
//        qDebug() << "verifyData" << "length" << _lgth;
//    }

    for(int i = 0; i < _lgth-1; i++) _num += _data[i];

    if(_data[_lgth-1] != _num)
    {
        qDebug("STM32 data check failed!");
        return false;
    }
    return true;
}
