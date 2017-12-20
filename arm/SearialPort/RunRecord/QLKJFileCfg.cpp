#include "QLKJFileCfg.h"
#include "Tools/QTFDevUtils.h"

#define DEFAULTCFG  "LKJFile/LKJFileCfg.ini"
#define DEFAULTCFGPATH "/media/mmcblk0p1/LKJFile/LKJFileCfg.ini"
#define READ_SETTINGFILE_PATH  QSettings _settings(strCfgPath, QSettings::IniFormat)

QLKJFileCfg::QLKJFileCfg(QObject *parent) :
    QObject(parent)
{

}

void QLKJFileCfg::setSyncFiles(QStringList names)
{
    QString strCfgPath = DEFAULTCFGPATH;
    if (QTFDevUtils::getInstance()->isSDMount(strCfgPath)) {strCfgPath += DEFAULTCFG;}
    READ_SETTINGFILE_PATH;
    m_mutex.lock();
    _settings.setValue("LKJ/SyncFiles", names);
    _settings.setValue("LKJ/fileCount", names.count());
    m_mutex.unlock();
}

int QLKJFileCfg::getFilesCount()
{
    QString strCfgPath = DEFAULTCFGPATH;
    if (QTFDevUtils::getInstance()->isSDMount(strCfgPath)) {strCfgPath += DEFAULTCFG;}
    READ_SETTINGFILE_PATH;
    m_mutex.lock();
    int ret = _settings.value("LKJ/fileCount", 0).toInt();
    m_mutex.unlock();
    return ret;
}

QStringList QLKJFileCfg::getSyncFiles()
{
    QString strCfgPath = DEFAULTCFGPATH;
    if (QTFDevUtils::getInstance()->isSDMount(strCfgPath)) {strCfgPath += DEFAULTCFG;}
    READ_SETTINGFILE_PATH;
    m_mutex.lock();
    QStringList ret = _settings.value("LKJ/SyncFiles").toStringList();
    m_mutex.unlock();
    return ret;
}

void QLKJFileCfg::setUploadFile(QString file)
{
    QString strCfgPath = DEFAULTCFGPATH;
    if (QTFDevUtils::getInstance()->isSDMount(strCfgPath)) {strCfgPath += DEFAULTCFG;}
    READ_SETTINGFILE_PATH;
    m_mutex.lock();
    _settings.setValue("LKJ/UploadFile", file);
    m_mutex.unlock();
}

QString QLKJFileCfg::getUploadFile()
{
    QString strCfgPath = DEFAULTCFGPATH;
    if (QTFDevUtils::getInstance()->isSDMount(strCfgPath)) {strCfgPath += DEFAULTCFG;}
    READ_SETTINGFILE_PATH;
    m_mutex.lock();
    QString ret = _settings.value("LKJ/UploadFile").toString();
    m_mutex.unlock();
    return ret;
}

void QLKJFileCfg::setUploadNo(int No)
{
    QString strCfgPath = DEFAULTCFGPATH;
    if (QTFDevUtils::getInstance()->isSDMount(strCfgPath)) {strCfgPath += DEFAULTCFG;}
    READ_SETTINGFILE_PATH;
    m_mutex.lock();
    _settings.setValue("LKJ/UploadRecord", No);
    m_mutex.unlock();
}

int QLKJFileCfg::getUploadNo()
{
    QString strCfgPath = DEFAULTCFGPATH;
    if (QTFDevUtils::getInstance()->isSDMount(strCfgPath)) {strCfgPath += DEFAULTCFG;}
    READ_SETTINGFILE_PATH;
    m_mutex.lock();
    int ret = _settings.value("LKJ/UploadRecord", 0).toInt();
    m_mutex.unlock();
    return ret;
}
