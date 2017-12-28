#include <QApplication>
#include "mainwindow.h"
#include <QTranslator>
#include <QFile>
#include <QDebug>
#include "SimulationDialog.h"

#define LANGUAGE_CN_FILE  ("chese.qm")
#define LANGUAGE_EN_FILE  ("english.qm")

void InitUiByLanguage(const QString strLanguage);
QTranslator *m_translator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    m_translator = new QTranslator();   //QTranslator 必须是全局的，不然会恢复默认值。
    InitUiByLanguage("chese");
    qApp->installTranslator(m_translator);

//    MainWindow w;
//    w.show();

    SimulationDialog dialog;
    dialog.show();
//    qDebug() << dialog.exec();

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
    }
    else
    {
        qDebug() << "[warning]auth client language file does not exists ...";
    }
}
