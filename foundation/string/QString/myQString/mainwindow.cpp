#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_lineEdit_textChanged(const QString &arg1)
{
    QString Name = arg1;
    Name = Name.remove(".bin");
//    Name.remove("TF1598_3_");
    QString version = Name.section('_', -1);
    ui->lineEdit_2->setText(version);
    bool ok = false;
    int version_num = version.toInt(&ok);
    if (ok) {
        ui->label->setText(QString("version %1").arg(version_num));
    } else {
        ui->label->setText("not version");
    }
}
