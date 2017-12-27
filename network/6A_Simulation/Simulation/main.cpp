#include <QApplication>
#include "mainwindow.h"
#include <QTranslator>
#include <QFile>
#include <QDebug>

#define LANGUAGE_CN_FILE  ("chese.qm")
#define LANGUAGE_EN_FILE  ("english.qm")

void InitUiByLanguage(const QString strLanguage);
QTranslator *m_translator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    m_translator = new QTranslator();   //QTranslator ������ȫ�ֵģ���Ȼ��ָ�Ĭ��ֵ��
    InitUiByLanguage("chese");

    MainWindow w;
    w.show();
    
    return a.exec();
}


void InitUiByLanguage(const QString strLanguage)
{
    if (strLanguage.isEmpty())
    {
        return;
    }

    QString strLanguageFile;
    if(strLanguage.compare("english") == 0)
    {
        strLanguageFile =qApp->applicationDirPath() + QString("/languages/%1").arg(LANGUAGE_EN_FILE);
    } else {
        strLanguageFile =qApp->applicationDirPath() + QString("/languages/%1").arg(LANGUAGE_CN_FILE);
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
