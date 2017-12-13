#ifndef DIALOG_H
#define DIALOG_H

#include "ui_dialog.h"
#include <QtNetwork/QtNetwork>
#include <QByteArray>
class Dialog : public QDialog, private Ui::Dialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
private:
    QUdpSocket udpSocket;
    QByteArray receiveBuf;
private slots:
    void receiveMessage(void);
};

#endif // DIALOG_H
