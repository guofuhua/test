#include "dialog.h"
#include <QMessageBox>

void writeByteArrays(QDataStream &in, QByteArray &ba,quint32 len);

Dialog::Dialog(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
    connect (&tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    m_blockSize = 0;
}

void Dialog::on_listenButton_clicked()
{
    tcpServer.listen (QHostAddress(tr("192.168.1.93")),1234);
    listenButton->setEnabled (false);
}

void Dialog::acceptConnection ()
{
    tcpSocket=tcpServer.nextPendingConnection ();
    if(tcpSocket==NULL) return;
    label->setText (tr("A connection come in"));
    receiveBuf.resize(0);
    connect (tcpSocket,SIGNAL(readyRead()),this,SLOT(slotReadData()));
}

void Dialog::receiveData (QByteArray tmpbuf)
{
    receiveBuf.append (tmpbuf);
    showTextEdit->clear ();
    QString str=QString::fromUtf8 (tmpbuf);
    showTextEdit->appendPlainText (str);

}

//
//FUCTION:  �ԣã������ݴ�����
//PARAMETER�� No
//RETURN��No
void Dialog::slotReadData()
{
    QDataStream _receiveStream(tcpSocket);
    _receiveStream.setVersion(QDataStream::Qt_4_6);

    QByteArray receiveBytes;

goon:
    if(m_blockSize==0) //����Ǹտ�ʼ��������
    {
        //�жϽ��յ������Ƿ������ֽڣ�Ҳ�����ļ��Ĵ�С��Ϣ
        //������򱣴浽blockSize�����У�û���򷵻أ�������������
        if(tcpSocket->bytesAvailable() < (int)sizeof(quint32)) return;
        _receiveStream >> m_blockSize;
        qDebug() << "blocksize" << m_blockSize;
        if(m_blockSize>2147480000)
        {
            m_blockSize = 0;
            return;
        }
    }

    //���û�еõ�ȫ�������ݣ��򷵻أ�������������
    if(tcpSocket->bytesAvailable() < m_blockSize) return;
    writeByteArrays(_receiveStream, receiveBytes, m_blockSize);
    parseReceiveBytes(&receiveBytes);
    receiveBytes.clear();
    m_blockSize = 0;
    if(tcpSocket->bytesAvailable()>0)
    {
        goto goon;
    }
}


//
//FUCTION:�����ݷְ�����
//PARAMETER��
//RETURN��No
void writeByteArrays(QDataStream &in, QByteArray &ba,quint32 len)
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

//
//FUCTION: �����յ�������
//PARAMETER�� No
//RETURN��No
void Dialog::parseReceiveBytes(QByteArray* receiveBytes)
{
    QDataStream _receiveStream(receiveBytes,QIODevice::ReadOnly);
    _receiveStream.setVersion(QDataStream::Qt_4_6);
    unsigned char _startFlag;
    _receiveStream >> _startFlag;
    qDebug() << _startFlag;
    switch(_startFlag)
    {
    case 1://��Ϣ
    {
        QByteArray data;
        _receiveStream >> data;
        receiveData(data);
        break;
    }
    case 2:
    {
        QByteArray data;
        _receiveStream >> data;
        getFile(data);
        break;
    }
    default:
    {
//        qDebug()<<" unrecongnise type " << _startFlag;
    }
        break;
    }
}

//
void Dialog::getFile(QByteArray databuf)
{
    QDataStream _in(&databuf,QIODevice::ReadWrite);
    _in.setVersion(QDataStream::Qt_4_6);
    QString strFileName;
    QByteArray array;
    int length;
    _in >> strFileName;
    _in >> length;
    _in >> array;
    qDebug() << strFileName << " : " << length;
    if(array.size()!=length)
    {
        qDebug("Tcp Receive file error\n");
        return ;
    }
    QFile filetcp(strFileName);
    if(!filetcp.open(QIODevice::WriteOnly))
    {
        qDebug("open file failed\n");
    }
    filetcp.write(array.data(),length);
    filetcp.close();
    qDebug("receive OK!\n");
}
