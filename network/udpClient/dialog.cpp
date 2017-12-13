#include "dialog.h"
#include <QDebug>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent){
    setupUi(this);
    setWindowTitle ("udpClient");

 }

void Dialog::on_sendTextEdit_textChanged()
{
    udpSocket.writeDatagram (sendTextEdit->toPlainText().toUtf8(),QHostAddress::Broadcast,8888);
}

