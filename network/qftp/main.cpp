/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include "ftpwindow.h"
#include <QTextCodec>
#include <QIcon>
#include <QTranslator>
#include <QFile>
#include <QDebug>
#include <fstream>
#include <QDateTime>
#include <QSqlDatabase>

#define LHT_SYNCCLIENT_VERSION_PRODOCUTNAME ("v1.0")
#define LHT_SYNCCLIENT_CN_FILE  ("cn.qm")

void InitUiByLanguage(const QString strLanguage);
void outputMessage(QtMsgType type, const char *msg);

QTranslator *m_translator;
std::ofstream g_OutputDebug;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(ftp);

    QApplication app(argc, argv);

    m_translator = new QTranslator();   //QTranslator 必须是全局的，不然会恢复默认值。
    InitUiByLanguage("cn");
    QString strPath = QApplication::applicationDirPath();
    strPath += "/img/icon.png";
    app.setWindowIcon(QIcon(strPath));
#ifndef QT_DEBUG
    g_OutputDebug.open(QString(QCoreApplication::applicationDirPath() + QString("/Log/log.txt")).toStdString().c_str(),\
                       std::ios::out | std::ios::trunc);
    qInstallMsgHandler(outputMessage);    //注册MessageHandler
#endif    // QT_DEBUG
//    QStringList drivers = QSqlDatabase::drivers();
//    foreach(QString driver, drivers)  //列出Qt5所有支持的数据库类型
//         qDebug() << "\t" << driver;
////        QOptDbEntry es;

    FtpWindow ftpWin;
    ftpWin.show();
    int ret = ftpWin.exec();
    qDebug() << "close programmer";
#ifndef QT_DEBUG
    g_OutputDebug.flush();
    g_OutputDebug.close();
#endif
    return ret;
}

void InitUiByLanguage(const QString strLanguage)
{
    if (strLanguage.isEmpty())
    {
        return;
    }

    QString strLanguageFile;
    if(strLanguage.compare("cn") == 0)
    {
        strLanguageFile =qApp->applicationDirPath() + QString("/languages/%1/%2").arg(LHT_SYNCCLIENT_VERSION_PRODOCUTNAME).arg(LHT_SYNCCLIENT_CN_FILE);
    }

    if (QFile(strLanguageFile).exists())
    {
        m_translator->load(strLanguageFile);
        qApp->installTranslator(m_translator);
    }
    else
    {
        qDebug() << "[warning]auth client language file does not exists ...";
    }
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
