#include "dialog.h"
#include <QtNetwork/QHostAddress>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

Dialog::Dialog(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
    bytessend=1;
    connect(&tcpSocket,SIGNAL(connected()),this,SLOT(connectRemote()));
    connect (&tcpSocket,SIGNAL(bytesWritten(qint64)),this,SLOT(finshSend(qint64)));
}

void Dialog::on_connectionButton_clicked()
{
    tcpSocket.connectToHost (QHostAddress(tr("192.168.1.93")),1234);
    connectionButton->setEnabled (false);
}

void Dialog::connectRemote ()
{
    bytessend=0;
}

void Dialog::on_inputTextEdit_textChanged()
{
    if(bytessend!=0) return;
    QString str=inputTextEdit->toPlainText ();
//    bytessend=str.length ();
//    tcpSocket.write (str.toUtf8 ());
    SendData(1, str.toUtf8());
}

void Dialog::finshSend (qint64 num)
{
    bytessend-=num;
    qDebug() << bytessend << num;
}

//��TCP�������˷�������
bool Dialog::SendData(uchar data_type, QByteArray data)
{
    QByteArray array;
    QDataStream _data( &array,QIODevice::ReadWrite);
    _data.setVersion(QDataStream::Qt_4_6);
    //TCP���������
    _data << (data_type);
    _data << (data);
    //����ͨ��TCP���ӷ��͵������
    bool ret = sendBytes(array);
    if (!ret)
    {
        qDebug() << " sendBytes failed!!!!";
    }

    qDebug() << "logToSDCard <==> " << data_type;
    return ret;
}

//
//FUCTION:  ���ͣԣã�����
//PARAMETER�� _sendData: ��������
//RETURN������ �Ƿ�ɹ�
bool Dialog::sendBytes(QByteArray &_sendData)
{
    if (tcpSocket.isValid() == false)
    {
        qDebug("socket invalid!!!!\n");
        return false;
    }

    for(int i = 0; i < 10 ;i++)
    {
        if(m_WriteDataMutex.tryLock())
        {
            break;
        }
        if(i >= 5)
        {
            qDebug("get write mutex error \n");
            return false;
        }
#ifdef Q_OS_WIN
        Sleep(10);
#else
        usleep(10000);
#endif
    }

    bool bIsSuccess = false;
    QByteArray SendBlock;
    QDataStream sendStream(&SendBlock,QIODevice::WriteOnly);
    sendStream.setVersion(QDataStream::Qt_4_6);
    sendStream<<(quint32) _sendData.size();
    sendStream.writeRawData(_sendData.data(),_sendData.size());

    bytessend=SendBlock.size();
    qDebug() <<(quint32) _sendData.size();
    tcpSocket.write(SendBlock);
    bIsSuccess = tcpSocket.flush();

    SendBlock.clear();
    m_WriteDataMutex.unlock();

    return bIsSuccess;
}

void Dialog::uploadFile()
{
    QFileDialog::Options options;
//        options |= QFileDialog::DontUseNativeDialog;
    QString selectedFilter;
    QString fileNamePath = QFileDialog::getOpenFileName(this,
                                                    tr("QFileDialog::getOpenFileName()"),
                                                    QDir::currentPath(),
                                                    tr("All Files (*);;Text Files (*.txt)"),
                                                    &selectedFilter,
                                                    options);
    if (!fileNamePath.isEmpty()) {
    //![4]
        QFile *file = new QFile(fileNamePath);
        QString fileName = QFileInfo(*file).fileName();
        if (!file->open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to open the file %1: %2.")
                                     .arg(fileName).arg(file->errorString()));
            delete file;
            return;
        }
        qDebug() << fileName;

        QByteArray data = file->readAll();
        QByteArray SendBlock;
        QDataStream sendStream(&SendBlock,QIODevice::WriteOnly);
        sendStream.setVersion(QDataStream::Qt_4_6);
        sendStream << fileName;
        sendStream << (int)data.size();
        sendStream << data;
        SendData(2, SendBlock);
        file->close();
        delete file;
    }
}

void Dialog::on_uploadButton_clicked()
{
    uploadFile();
}
