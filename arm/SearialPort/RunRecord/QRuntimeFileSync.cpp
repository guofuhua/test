#include "QRuntimeFileSync.h"
#include "Tools/QTFDevUtils.h"
#include  <QDir>
#include "Include/mydebug.h"
#include <QDebug>
#include "QLKJFileCfg.h"
#define LKJ_FILE_PATH   ("LKJFile/File")

QRuntimeFileSync::QRuntimeFileSync(QObject *parent) :
    QObject(parent)
{
    m_serial = NULL;
    m_fileInfo.clear();
    if (init()) {
        m_runOK = true;
    } else {
        m_runOK = false;
    }
    m_firstSync = true;
    m_status = SERIAL_IDLE;
    m_time = QDateTime::currentDateTime();
    connect(this, SIGNAL(signalDoSync()), this, SLOT(slotDoSync()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotDoSync()));
}

QRuntimeFileSync::~QRuntimeFileSync()
{
    FUNC_IN;
    if (m_serial != NULL) {
        delete m_serial;
        m_serial = NULL;
    }
}

bool QRuntimeFileSync::init()
{
    FUNC_IN;
    if (!QTFDevUtils::getInstance()->isSDMount(m_sdPath)) {
        FUNC_OUT;
        return false;
    }
    if (!QTFDevUtils::getInstance()->isSDCanWrite()) {
        FUNC_OUT;
        return false;
    }

    QString LKJ_Dir = m_sdPath + LKJ_FILE_PATH;
    QDir dircty(LKJ_Dir);
    //如无目录
    if (!dircty.exists())
    {
        dircty.mkdir(LKJ_Dir);
    }
    if (!dircty.exists()) {
        FUNC_OUT;
        return false;
    }
    //
    if (m_serial != NULL) {
        delete m_serial;
        m_serial = NULL;
    }
    m_serial = new CSerialportDevice("/dev/ttyS1");
    if (!m_serialRead.openComPort(m_serial)) {
        FUNC_OUT;
        return false;
    }

    FUNC_OUT;
    return true;
}

bool QRuntimeFileSync::readLKJDirInfo()
{
    FUNC_IN;
    bool ret = false;
    if (m_runOK) {
        if (m_serialRead.readLKJDirInfo(m_serial)) {
            ret = true;
        }
    }
    DEB("m_runOK = %d, ret = %d\n", m_runOK, ret);
    return ret;
}

bool QRuntimeFileSync::syncDir()
{
    FUNC_IN;
    if (!m_runOK) {
        return false;
    }
    if (0 != m_fileInfo.size()) {
        int i = 0;
        QStringList allRuntimeFile;
        for (i = 0; i < m_fileInfo.size(); i++) {
            allRuntimeFile.append(m_fileInfo.at(i).name);
        }
        QString LKJ_Dir = m_sdPath + LKJ_FILE_PATH;
        QDir dircty(LKJ_Dir);
        QStringList allFile = dircty.entryList(QDir::Files);
        QStringList cfgFiles = QLKJFileCfg::getInstance()->getSyncFiles();
        if (cfgFiles.empty()) {
            WARN("config runtime file dir is empty!\n");
            QLKJFileCfg::getInstance()->setSyncFiles(allRuntimeFile);
            return true;
        }
        if (allFile.empty()) {
            WARN("local runtime file dir is empty!\n");
            return true;
        }

        bool changed = false;
        for (i = 0; i < allFile.size(); i++) {
            if (!allRuntimeFile.contains(allFile.at(i))) {
                if (QFile::remove(allFile.at(i))) {
                    ERR("%s remove failed!\n", allFile.at(i).toStdString().c_str());
                }
            } else if (!cfgFiles.contains(allFile.at(i))) {
                cfgFiles.insert(0, allFile.at(i));
                INFO("LKJ config record new file :%s!!!\n", allFile.at(i).toStdString().c_str());
                changed = true;
            }
        }

        for (i = cfgFiles.size() - 1; i >= 0; i--) {
            if (!allRuntimeFile.contains(cfgFiles.at(i))) {
                cfgFiles.removeAt(i);
                changed = true;
            }
        }

        int j = 0;
        m_newFileList.clear();
        for (i = 0; i < allRuntimeFile.size(); i++) {
            if (cfgFiles.contains(allRuntimeFile.at(i))) {
                if (cfgFiles.at(j) != allRuntimeFile.at(i)) {
                    cfgFiles.swap(j, cfgFiles.indexOf(allRuntimeFile.at(i)));
                    changed = true;
                }
                j++;
            } else {
                m_newFileList.append(m_fileInfo.at(i));
                INFO("syncDir new file :%s\n", allRuntimeFile.at(i).toStdString().c_str());
            }
        }

        if (changed) {
            QLKJFileCfg::getInstance()->setSyncFiles(cfgFiles);
            emit signalNewFile(ConvertFileInfo());
            if (m_firstSync) {m_firstSync = false;}
            INFO("config Sync Files changed!!!\n");
        }
    }

    return true;
}

void QRuntimeFileSync::calcuteTimeout(int size)
{
    int s = size / 1024;
    DEB("time out %d seconds\n", s);
    m_time = QDateTime::currentDateTime().addSecs(s);
}

QByteArray QRuntimeFileSync::ConvertFileInfo()
{
    QByteArray data;
    char buf[4];
    for (int i = 0; i < m_fileInfo.size(); i++) {
        QDateTime dateTime = m_fileInfo.at(i).Time;
        uchar month = dateTime.date().month();
        uchar day = dateTime.date().day();
        uchar hour = dateTime.time().hour();
        uchar minute = dateTime.time().minute();
        memcpy(&buf, &m_fileInfo.at(i).nTrainNo, 4);
        data.append(buf, 4);
        memcpy(&buf, &m_fileInfo.at(i).nDriverNo, 4);
        data.append(buf, 4);
        data.append(month);
        data.append(day);
        data.append(hour);
        data.append(minute);
        memcpy(&buf, &m_fileInfo.at(i).size, 4);
        data.append(buf, 4);
    }
    return data;
}

bool QRuntimeFileSync::syncFile()
{
    FUNC_IN;
    if (!m_runOK) {
        ERR("init falied!\n");
        return false;
    }
    if (SERIAL_IDLE != m_status) {
        DEB("serial is busy!\n");
        return false;
    }
    if (0 != m_fileInfo.size()) {
        m_fileInfo.first().size;
        QString LKJ_Dir = m_sdPath + LKJ_FILE_PATH;
        QString filePath = QDir::cleanPath(QString("%1/%2").arg(LKJ_Dir).arg(m_fileInfo.first().name));
        QFileInfo file(filePath);
        if (file.exists()) {
            if (file.size() != m_fileInfo.first().size) {
                INFO("local file(%s) size:%lld, SOC file size:%d\n", file.filePath().toStdString().c_str(), file.size(), m_fileInfo.first().size);
                if (m_serialRead.readSpecialLKJFile(m_serial, m_fileInfo.first(), file.size())) {
                    m_status = SERIAL_READ_SYNC_FILE;
                    m_currentReadFile = file.fileName();
                    calcuteTimeout(m_fileInfo.first().size);
                } else {
                    ERR("read sync first file failed\n");
                }
            } else {
                DEB("first file not need sync! file size = %lld bytes\n", file.size());
            }
        } else {
            INFO("need sync first file!\n");
            if (m_serialRead.readLKJFile(m_serial, m_fileInfo.first())) {
                m_status = SERIAL_READ_FILE;
                m_currentReadFile = file.fileName();
                calcuteTimeout(m_fileInfo.first().size);
                //                m_newFileList.append(m_fileInfo.first());
            } else {
                ERR("read LKJ file failed\n");
            }
        }
    } else {
        DEB("SOC file dir is empty ?!!\n");
    }

    return true;
}

void QRuntimeFileSync::doSync()
{
    FUNC_IN;
    QTimer::singleShot(5000, this, SLOT(slotDoSync()));
    m_timer.start(REFRESH_DIR_INTERVAL);
}

void QRuntimeFileSync::slotDoSync()
{
    FUNC_IN;
    if ((SERIAL_READ_DIR == m_status) || (SERIAL_READ_FILE == m_status))
    {
        if (m_time < QDateTime::currentDateTime()) {
            qDebug() << m_time;
            m_status = SERIAL_IDLE;
            emit signalSetNewState(m_status);
        } else {
            INFO(".\n");
        }
    }

    if (SERIAL_IDLE != m_status) {
        return;
    }

    bool ret = readLKJDirInfo();
    if (ret)
    {
        m_status = SERIAL_READ_DIR;
        m_time = QDateTime::currentDateTime().addSecs(30);
        emit signalSetNewState(m_status);
    } else {
        ERR("readLKJDirInfo return false\n");
    }
}

void QRuntimeFileSync::slotAnalysisDir(QByteArray data)
{
    FUNC_IN;
    int i = 0;

    bool analysisOK = true;
    QByteArray dirInfo = data.mid(6);
    m_fileInfo.clear();
    while ((i + SIWEI_DIR_INFO_LEN) < dirInfo.size()) {
        LKJFileInfo fileInfo;
        QByteArray OneFileData = dirInfo.mid(i, SIWEI_DIR_INFO_LEN);
        i += SIWEI_DIR_INFO_LEN;
        if (m_serialRead.AnalysisOneFileInfo(OneFileData, fileInfo)) {
            m_fileInfo.append(fileInfo);
        } else {
            analysisOK = false;
            break;
        }
    }
    if (analysisOK) {
        INFO("analysis OK\n");
    } else {
        ERR("analysis ERROR!\n");
    }
    if (SERIAL_READ_DIR == m_status) {
        m_status = SERIAL_IDLE;
    }
    //    sleep(5);
    bool ret = syncDir();
    if (ret)
    {
        if (m_newFileList.size() > 0) {
            if (m_serialRead.readLKJFile(m_serial, m_newFileList.first())) {
                m_status = SERIAL_READ_FILE;
                m_currentReadFile = m_newFileList.first().name;
                calcuteTimeout(m_newFileList.first().size);
            } else {
                ERR("read LKJ file failed\n");
            }
        }/* else {
            DEB("no need load file!\n");
        }*/
    } else {
        ERR("syncDir ERROR:\n");
    }
    syncFile();
    if (m_firstSync) {
        emit signalNewFile(ConvertFileInfo());
        m_firstSync = false;
    }
    emit signalSetNewState(m_status);
}

void QRuntimeFileSync::slotWrongFlag(quint16 flag)
{
    FUNC_IN;
    ERR("wrong flag = 0x%x \n", flag);
    switch (flag)
    {
    case READ_ERR_FLAG:
    case READ_INTERRUPT_FLAG:
        m_status = SERIAL_IDLE;
        emit signalSetNewState(m_status);
        break;
    default:
        m_status = SERIAL_IDLE;
        emit signalSetNewState(m_status);
        break;
    }
}

void QRuntimeFileSync::slotReceiveFile(QByteArray data)
{
    QString LKJ_Dir = m_sdPath + LKJ_FILE_PATH;
    QString filePath = QDir::cleanPath(QString("%1/%2").arg(LKJ_Dir).arg(m_currentReadFile));
    QFile file(filePath);
    if (!file.open(QFile::ReadWrite))
    {
        ERR("file :%s open failed\n", file.fileName().toStdString().c_str());
        return;
    }
    qint64 size = data.size();
    qint64 write_num = file.write(data);
    if (write_num != size)
    {
        ERR("write error :write %lld bytes, actual need write %lld bytes.\n", write_num, size);
    }
    file.close();
    INFO("create file:%s\n", file.fileName().toStdString().c_str());

    if (0 != m_newFileList.size()) {
        DEB("%s, status:%d, new file count:%d\n", m_newFileList.first().name.toStdString().c_str(), m_status,  m_newFileList.size());
    }
    if (SERIAL_READ_FILE == m_status) {
        m_status = SERIAL_IDLE;
    }
}

void QRuntimeFileSync::slotReceiveSyncFile(QByteArray data)
{
    QString LKJ_Dir = m_sdPath + LKJ_FILE_PATH;
    QString filePath = QDir::cleanPath(QString("%1/%2").arg(LKJ_Dir).arg(m_currentReadFile));
    QFile file(filePath);
    if (!file.open(QFile::Append))
    {
        ERR("file :%s open failed\n", file.fileName().toStdString().c_str());
        return;
    }
    qint64 size = data.size();
    qint64 write_num = file.write(data);
    if (write_num != size)
    {
        ERR("write error :write %lld bytes, actual need write %lld bytes.\n", write_num, size);
    }
    file.close();
    INFO("append file:%s\n", file.fileName().toStdString().c_str());

    DEB("sync file name:%s, status:%d, SOC file count:%d\n", m_fileInfo.first().name.toStdString().c_str(), m_status,  m_fileInfo.size());
    if (SERIAL_READ_SYNC_FILE == m_status) {
        m_status = SERIAL_IDLE;
    }

    emit signalNewData(ConvertFileInfo());
}
