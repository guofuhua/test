#include "fileconvert.h"
#include "ui_fileconvert.h"
#include <QFileDialog>
#include "Convert.c"

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
