#include <QCoreApplication>
#include<qdir.h>
#include <qdebug.h>
#include<QtAlgorithms>
void findallfile(QString path);
bool compareBarData(const QFileInfo &barAmount1, const QFileInfo &barAmount2);
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    findallfile("/proc");
    return a.exec();
}

void findallfile(QString path)
{
    QDir *dir=new QDir(path);
    QStringList filter;
    //filter<<"*.yuv";
    //dir->setNameFilters(filter);
    //dir->setSorting(QDir::Name);
QMap test;
QFile tes;
process_line();
    QList<QFileInfo> *fileInfo=new QList<QFileInfo>(dir->entryInfoList(filter));
    int toalfile=fileInfo->count();
    qDebug()<<"total file "<<toalfile;
    qSort(fileInfo->begin(), fileInfo->end(), compareBarData);
    for(int i=0;i<toalfile;i++)
    {
        //filePath
        qDebug()<<fileInfo->at(i).fileName();
    }
}

bool compareBarData(const QFileInfo &barAmount1, const QFileInfo &barAmount2)
{
    QString str1 = barAmount1.fileName().mid(9);
    str1 = str1.section(".yuv",0,0);
    //qDebug() << barAmount1.fileName() + str1;
    QString str2 = barAmount2.fileName().mid(9);
    str2 = str2.section(".yuv",0,0);
    //qDebug() << barAmount2.fileName() + str2;
    bool ok;
    int dec1 = str1.toInt(&ok, 10);
    int dec2 = str2.toInt(&ok, 10);
    if (dec1 < dec2)
    {
        //qDebug() << "true";
        return true;
    }
    //qDebug() << "false";
    return false;
}
