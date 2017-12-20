#ifndef QRUNTIMEFILESYNC_H
#define QRUNTIMEFILESYNC_H

#include <QObject>
#include "QSerialWrite.h"
#include "Entity/LKJRuntimeTypes.h"
#include <QStringList>
#include <QTimer>
#include "LocalCommon.h"

class QRuntimeFileSync : public QObject
{
    Q_OBJECT
public:
    explicit QRuntimeFileSync(QObject *parent = 0);
    ~QRuntimeFileSync();
    bool m_runOK;
    bool m_firstSync;
    QString m_sdPath;
    bool init();
    QSerialWrite m_serialRead;
    CSerialportDevice *m_serial;
    bool readLKJDirInfo();
    bool syncDir();
    QList<LKJFileInfo> m_fileInfo;
    QList<LKJFileInfo> m_newFileList;
    QString m_currentReadFile;
    void doSync();
    EM_SerialStatus m_status;
    QDateTime m_time;
    QTimer m_timer;

    bool syncFile();
    void calcuteTimeout(int size);
    QByteArray ConvertFileInfo();
signals:
    void signalDoSync();
    void signalSetNewState(int);
    void signalNewData(QByteArray);
    void signalNewFile(QByteArray);
    
public slots:
    void slotDoSync();
    void slotAnalysisDir(QByteArray data);
    void slotWrongFlag(quint16 flag);
    void slotReceiveFile(QByteArray data);
    void slotReceiveSyncFile(QByteArray data);
    
};

#endif // QRUNTIMEFILESYNC_H
