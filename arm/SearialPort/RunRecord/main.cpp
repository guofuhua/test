#include <QCoreApplication>
#include "Tools/QTFSystemUtils.h"
#include "QRuntimeFileSync.h"
#include "QSerialReadThread.h"
#include "QLKJRuntimeFile.h"
#include "QLKJFileCfg.h"
#include <QThread>
#include <QDebug>
#include <QDir>
#include "Tools/QTFDevUtils.h"
#include "QUploadRunRecord.h"
#include "Log/QLog.h"
#include "QTcpSendRunRecord.h"

const char* g_proName = "AV4EXP_LKJFileReader_Model";

int main(int argc, char *argv[])
{
    //如果程序已运行，则退出
    if(QTFSystemUtils::getInstance()->checkProgramIsRunning(g_proName))
    {
        printf("error pro %s is running \n", g_proName);
        return 0;
    }
    else
    {
        printf("pro %s is running \n", g_proName);
    }
    QCoreApplication a(argc, argv);
    QLog::getInstance()->logToSDCard("pro AV4EXP_LKJFileReader_Model Start run!\n");

    QThread thread1;
    QThread thread2;
    QUploadRunRecord uploadRecord;
    QRuntimeFileSync syncRunFile;
    QSerialReadThread readThread;
    QTcpSendRecThread sendRecord;

    uploadRecord.moveToThread(&thread1);
    syncRunFile.moveToThread(&thread2);
    QObject::connect(&readThread, SIGNAL(signalSyncFile(QByteArray)), &syncRunFile, SLOT(slotReceiveSyncFile(QByteArray)));
    QObject::connect(&readThread, SIGNAL(signalFile(QByteArray)), &syncRunFile, SLOT(slotReceiveFile(QByteArray)));
    QObject::connect(&readThread, SIGNAL(signalDirInfo(QByteArray)), &syncRunFile, SLOT(slotAnalysisDir(QByteArray)));
    QObject::connect(&readThread, SIGNAL(signalWrongFlag(quint16)), &syncRunFile, SLOT(slotWrongFlag(quint16)));
    QObject::connect(&syncRunFile, SIGNAL(signalSetNewState(int)), &readThread, SLOT(slotSetNewState(int)), Qt::DirectConnection);
    QObject::connect(&syncRunFile, SIGNAL(signalNewData(QByteArray)), &uploadRecord, SLOT(slotNewFile(QByteArray)));
    QObject::connect(&syncRunFile, SIGNAL(signalNewFile(QByteArray)), &uploadRecord, SLOT(slotNewFile(QByteArray)));
    QObject::connect(&syncRunFile, SIGNAL(signalNewData(QByteArray)), &sendRecord, SLOT(slotNewData()));
    QObject::connect(&syncRunFile, SIGNAL(signalNewFile(QByteArray)), &sendRecord, SLOT(slotNewData()));

    thread1.start();
    thread2.start();
    sendRecord.start();
    uploadRecord.StartRun();
    readThread.start();
    syncRunFile.doSync();
//    QString sdPath;
//    if (QTFDevUtils::getInstance()->isSDMount(sdPath)) {
//        QString LKJ_Dir = sdPath + "/LKJFile";
//        QStringList filesList = QLKJFileCfg::getInstance()->getSyncFiles();
//        QString filePath = QDir::cleanPath(QString("%1/%2").arg(LKJ_Dir).arg(filesList.first()));
//        QLKJRuntimeFile analysisFile;
//        RLKJRTFileHeadInfo headInfo;
//        qDebug() << filesList;
////        analysisFile.GetHeadInfoFromOrg(filePath, headInfo);
//        analysisFile.LoadFromOrgFile(filePath);
//    }
//    QLKJRuntimeFile analysisFile;
//    analysisFile.SyncFile();
    
    return a.exec();
}
