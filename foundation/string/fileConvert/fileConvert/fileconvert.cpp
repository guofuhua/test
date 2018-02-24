#include "fileconvert.h"
#include "ui_fileconvert.h"
#include <QFileDialog>
#include "Convert.c"
#include <QDebug>

FileConvert::FileConvert(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FileConvert)
{
    ui->setupUi(this);
}

FileConvert::~FileConvert()
{
    delete ui;
}

void FileConvert::on_pushButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Find Files"), QDir::currentPath());
    ui->lineEdit->setText(file);
    ui->label_input->setText(QFileInfo(file).fileName());
    ui->horizontalScrollBar->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    QFileInfo fileInfo(file);
    ui->label_total_size->setText(QString::number(fileInfo.size()));
}

void FileConvert::on_pushButton_2_clicked()
{
    char *buf[3];
    char exe[] = {"./"};
    char format[] = {"bin"};
    buf [0] = exe;
    buf [1] = format;
    buf [2] = (char *)ui->lineEdit->text().toStdString().c_str();
    ConvertMain(3, buf);
    ui->label_output->setText(ui->label_input->text().append(".txt"));
}

void FileConvert::on_pushButton_3_clicked()
{
    char *buf[3];
    char exe[] = {"./"};
    char format[] = {"txt"};
    buf [0] = exe;
    buf [1] = format;
    buf [2] = (char *)ui->lineEdit->text().toStdString().c_str();
    ConvertMain(3, buf);
    ui->label_output->setText(ui->label_input->text().append(".bin"));
}

void FileConvert::on_pushButton_4_clicked()
{
    char *buf[4];
    char exe[] = {"./"};
    char start[] = {"0"};
    buf [0] = exe;
    buf [1] = start;
    buf [2] = (char *)ui->lineEdit_size->text().toStdString().c_str();
    buf [3] = (char *)ui->lineEdit->text().toStdString().c_str();
    CuttingMain(4, buf);
}

void FileConvert::on_horizontalScrollBar_valueChanged(int value)
{
//    qDebug() << value;
    long total = ui->label_total_size->text().toLong();
    long size = value * total / 100;
    ui->lineEdit_size->setText(QString::number(size));
    ui->label_size->setText(QString::number(size));
}
