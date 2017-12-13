#include "dialog.h"
#include <QMessageBox>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
    connect (&tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
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
    connect (tcpSocket,SIGNAL(readyRead()),this,SLOT(receiveData()));
}

void Dialog::receiveData ()
{
    QByteArray tmpbuf;
    tmpbuf=tcpSocket->readAll ();
    receiveBuf.append (tmpbuf);
    showTextEdit->clear ();
    QString str=QString::fromUtf8 (tmpbuf);
    showTextEdit->appendPlainText (str);

}
