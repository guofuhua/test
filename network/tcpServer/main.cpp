#include <QtGui/QApplication>
#include "dialog.h"
#include <QDebug>
#include <fstream>

void outputMessage(QtMsgType type, const char *msg);

std::ofstream g_OutputDebug;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifndef QT_DEBUG
    g_OutputDebug.open(QString(QCoreApplication::applicationDirPath() + QString("/logTcp.txt")).toStdString().c_str(),\
                       std::ios::out | std::ios::trunc);
    qInstallMsgHandler(outputMessage);    //×¢²áMessageHandler
#endif    // QT_DEBUG

    Dialog w;
    w.show();

    int ret = a.exec();
    qDebug() << "close programmer";
#ifndef QT_DEBUG
    g_OutputDebug.flush();
    g_OutputDebug.close();
#endif
    return ret;
}



void outputMessage(QtMsgType type, const char *msg)
{
    QString text;
    switch(type)
    {
    case QtDebugMsg:
        text = QString("Debug:   ");
        break;

    case QtWarningMsg:
        text = QString("Warning: ");
        break;

    case QtCriticalMsg:
        text = QString("Critical:");
        break;

    case QtFatalMsg:
        text = QString("Fatal:   ");
        break;

    default:
        text = QString("Debug:   ");
    }

    //QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    //QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString current_date = QString("(%1)").arg(current_date_time);
    std::string message = qPrintable(QString("%1 %2 %3").arg(text).arg(current_date).arg(msg));

    g_OutputDebug << message << "\n";
    g_OutputDebug.flush();
}
