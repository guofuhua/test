#ifndef DIALOG_H
#define DIALOG_H

#include "ui_dialog.h"
#include <QtNetwork/QtNetwork>

class Dialog : public QDialog, private Ui::Dialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
private:
    QUdpSocket udpSocket;


private slots:
    void on_sendTextEdit_textChanged();
};

#endif // DIALOG_H
