#include "QLog.h"
#include "QLogProcess.h"

QLog::QLog(QObject *parent) :
    QObject(parent)
{
    QLogProcess* pLog = QLogProcess::getInstance();
    pLog->moveToThread(&m_thread);
    connect(this, SIGNAL(sigLogToSDCard(QString,QString)), pLog, SLOT(slotLogToSDCard(QString,QString)));
    connect(this, SIGNAL(sigLogToSDCardDefault(QString)), pLog, SLOT(slotLogToSDCardDefault(QString)));
    m_thread.start();
}


void QLog::logToSDCard(const char *str)
{
    QString text = QString(str);
    emit sigLogToSDCardDefault(text);
}


void QLog::logToSDCard(const std::string &file, const char *str)
{
    QString fileName = QString(file.c_str());
    QString text = QString(str);
    emit sigLogToSDCard(fileName, text);
}
