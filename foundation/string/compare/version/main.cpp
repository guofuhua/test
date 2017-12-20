#include <QCoreApplication>
#include <QDebug>
#include <QStringList>

int compare(QString strVersion1, QString strVersion2);
bool isVersionStr(QString strVersion);
QString getLargeVersion(QString V1, QString V2);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QString V1 = "2.3.05.0811";
    QString V2 = "2.3.05.0722";

    if (isVersionStr(V1) && isVersionStr(V2))
    {
        qDebug() << getLargeVersion(V1, V2);
    }
    else
    {
        qDebug() << "no two version";
    }
    return a.exec();
}

//compare strVersion1 and strVersion2, if equal return 0, large return 1, little return -1;
int compare(QString strVersion1, QString strVersion2)
{
    if (strVersion1.isEmpty() || strVersion2.isEmpty())
    {//End cycle condition
        return QString::compare(strVersion1, strVersion2, Qt::CaseInsensitive);;
    }
    bool ok;
    int version1 = strVersion1.section('.', 0, 0).toInt(&ok);
    if (!ok)
    {//not conformity version format "2.3.07"
        return -1;
    }
    int version2 = strVersion2.section('.', 0, 0).toInt(&ok);
    if (!ok)
    {//not conformity version format "2.3.07"
        return 1;
    }
    if (version1 < version2)
    {//version1 is smaller than version2
        return -1;
    }
    else if(version1 > version2)
    {//version1 is larger than version2
        return 1;
    }
    else
    {//this field is equal
        return compare(strVersion1.section('.',1), strVersion2.section('.',1));
    }
}

bool isVersionStr(QString strVersion)
{
    qDebug() << strVersion;
    bool ok;
    strVersion.section('.', 0, 0).toInt(&ok);
    if (!ok)
    {//not conformity version format "2.3.07"
        return ok;
    }
    if (!strVersion.section('.',1).isEmpty())
    {
        return isVersionStr(strVersion.section('.',1));
    }
    return true;
}

QString getLargeVersion(QString V1, QString V2)
{
    QString LargerVersion = V1;
    int ret = compare(V1,V2);
    if (ret < 0)
    {
        LargerVersion = V2;
    }
    return LargerVersion;
}
