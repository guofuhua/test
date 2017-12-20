#include <QCoreApplication>
#include <QSettings>
#include <QStringList>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QSettings settings("LKJFileCfg.ini", QSettings::IniFormat);

    QStringList ret = settings.value("LKJ/SyncFiles").toStringList();
    QStringList ori = ret;
    qDebug() << ori;
    ret.removeDuplicates();
    qDebug() << ret;
    int i;
    int j = 0;
    for (i = 0; i < ori.size(); i++) {
        if (ori.at(i) == ret.at(j))
            qDebug() << ori.at(i) << ret.at(j++);
        else
            qDebug() << ori.at(i);
    }
    return a.exec();
}
