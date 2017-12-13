#include "dialog.h"
#include <QtNetwork/QHostAddress>
#include <QMessageBox>

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
    bytessend=str.length ();
    tcpSocket.write (str.toUtf8 ());
}

void Dialog::finshSend (qint64 num)
{

    bytessend-=num;
}
