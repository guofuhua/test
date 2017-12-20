#include <QCoreApplication>
#include <QDebug>
#include <QStringList>
#include <QString>
#include <QDir>

int compare(QString strVersion1, QString strVersion2);
bool isVersionStr(QString strVersion);
static void findRecursion(const QString &path, const QString &pattern, QStringList *result);
QString getLargeVersion(QString V1, QString V2);

#define UPGRADE_CONFIG_FILE "config/General_Update_Model.ini"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QStringList files;
    QString path = QDir::cleanPath("/mnt/hgfs/dm3730");
    QString fileName = "AV4*Pack_*.tar.gz";
    QString AV4Pack;
    QString AV4ExpPack;
    QString AV4Version;
    QString AV4ExpVersion;

    findRecursion(path, fileName.isEmpty() ? QString("*") : fileName, &files);
    for (int i = 0; i < files.size(); ++i) {
        QString Name = QFileInfo(files.at(i)).fileName();
        qDebug() << Name;
        Name = Name.remove(".tar.gz");
        if (Name.contains("AV4ExpPack_"))
        {
            Name = Name.remove("AV4ExpPack_");
            if (isVersionStr(Name) && (compare(AV4ExpVersion, Name) < 0))
            {
                AV4ExpPack = files.at(i);
                AV4ExpVersion = Name;
            }
        }
        else if (Name.contains("AV4Pack_"))
        {
            Name = Name.remove("AV4Pack_");
            if (isVersionStr(Name) && (compare(AV4Version, Name) < 0))
            {
                AV4Pack = files.at(i);
                AV4Version = Name;
            }
        }
    }
    qDebug() <<"AV4ExpVersion"<< AV4ExpVersion;
    qDebug() <<"AV4ExpPack"<< AV4ExpPack;
    qDebug() <<"AV4Version"<< AV4Version;
    qDebug() <<"AV4Pack"<< AV4Pack;

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

static void findRecursion(const QString &path, const QString &pattern, QStringList *result)
{
    QDir currentDir(path);
    const QString prefix = path + QLatin1Char('/');
    foreach (const QString &match, currentDir.entryList(QStringList(pattern), QDir::Files | QDir::NoSymLinks))
        result->append(prefix + match);
}
