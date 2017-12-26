#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_timer.setSingleShot(true);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    syncRunFile.moveToThread(&thread2);
    thread2.start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    int delay_time = ui->lineEdit->text().toInt();
    m_timer.start(delay_time * 1000);

    syncRunFile.doSync();
}

void MainWindow::on_stopButton_clicked()
{
    m_timer.stop();
}
#define MESSAGE \
     tr("<p>timer end" \
                "<p> timer clock " )
void MainWindow::slotTimeOut()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::information(this, tr("QMessageBox::information()"), MESSAGE);
    if (reply == QMessageBox::Ok)
        ui->label->setText(tr("OK"));
    else
        ui->label->setText(tr("Escape"));
}

void MainWindow::on_triggerButton_clicked()
{
    m_timer.setInterval(1);
}
