#include "QTcpSendRunRecord.h"
#include "Tools/QTFDevUtils.h"
#include "QLKJFileCfg.h"
#include <QDir>
#include "QLKJRuntimeFile.h"
#include "Include/mydebug.h"

#define SUBSCRIBE_TRAINITEM_CLIENT  (20258)
#define SUBSCRIBE_LKJFILE_SERVER  (10159)
#define LKJ_FILE_PATH   ("LKJFile/File")
#define DEFAULT_ABNORMAL_SLEEP  (600)

extern void log_callback(const char* file, const char* message);
void toSDCard(const std::string& file, const char *message, bool bIsSuccess);

QTcpSendRecThread::QTcpSendRecThread(QObject *parent) :
    QThread(parent)
{
    m_pSubAlarmSvr = NULL;
    isSendOver = false;
}

void QTcpSendRecThread::SendTrainItem()
{
}

void QTcpSendRecThread::setSendOver(bool isOver)
{
    m_mutex.lock();
    isSendOver = isOver;
    m_mutex.unlock();
}

bool QTcpSendRecThread::getSendOver()
{
    bool ret = false;
    m_mutex.lock();
    ret = isSendOver;
    m_mutex.unlock();
    return ret;
}

void QTcpSendRecThread::run()
{
    if (NULL == m_pSubAlarmSvr) {
        m_pSubAlarmSvr = new SubServer::SubscribeServer(SUBSCRIBE_LKJFILE_SERVER,"AV4EXP_LKJFileReader_Model","AV4EXP_LKJFileReader_Model.log",toSDCard,0);
    }
    bool isLastOne = false;
    QString sdPath;
    QLKJRuntimeFile analysisTool;
    QStringList filesList;
    int index;
    QString fileName;
    QString filePath;

    while(true)
    {
        sleep(1);   //避免死循环过度占用CPU设置间隔
        if (getSendOver()) {
            continue;   //发送完成，等待数据更新
        }

        if (QTFDevUtils::getInstance()->isSDMount(sdPath)) {
            filesList = QLKJFileCfg::getInstance()->getSyncFiles();
            if (filesList.isEmpty()) {
                ERR("Config File No Record!\n");
                sleep(DEFAULT_ABNORMAL_SLEEP);  //配置文件中运行记录文件信息为空，过一段时间再获取
                continue;
            }
            qDebug() << filesList;
            index = filesList.indexOf(QLKJFileCfg::getInstance()->getUploadFile());
            if (-1 == index) {  //没有已上传的文件，从最早的开始上传
                index = filesList.size() - 1;
                isLastOne = false;
            } else if (index > 0) { //有未发送的运行记录文件，从未发送完成的运行记录文件开始发送。
                index--;
                isLastOne = false;
            } else {    //只有最新的运行记录未发送。
                index = 0;
                isLastOne = true;
            }

            /* 从SD卡中解析运行记录文件。 */
            fileName = filesList.at(index);
            filePath = QDir::cleanPath(QString("%1/%2%3").arg(sdPath).arg(LKJ_FILE_PATH).arg(fileName));
            analysisTool.LoadFromOrgFile(filePath);

            /* 从未发送的记录开始发送，并记录已发送的记录。 */
            index = QLKJFileCfg::getInstance()->getUploadNo();
            for (; index < analysisTool.m_Records.size(); index++) {
                m_pSubAlarmSvr->BroadCastData(analysisTool.getBroadcastData(analysisTool.m_Records.at(index).CommonRec));
                if (0 == index % 100) {
                    QLKJFileCfg::getInstance()->setUploadNo(index);
                }
                usleep(500000);
            }
            if (isLastOne) {    //当前最新文件发送完成，设置发送完成标记，并记录当前已发送条数。
                QLKJFileCfg::getInstance()->setUploadNo(index);
                setSendOver(true);
            } else {    //发送完成一个之前的记录文件，设置该文件已发送并重置发送条数。
                QLKJFileCfg::getInstance()->setUploadFile(fileName);
                QLKJFileCfg::getInstance()->setUploadNo(0);
            }
        } else {
            ERR("no sd card insert!!!\n");
            sleep(DEFAULT_ABNORMAL_SLEEP);  //当前没有SD卡插入，等待10分钟后再检测
        }
        qDebug() << "no ----";
    }
}

void QTcpSendRecThread::slotNewData()
{
    setSendOver(false);
}

//FUCTION: 　写入日志直接写SD卡
//PARAMETER：file  日志文件   str   写入内容
//RETURN：No
void toSDCard(const std::string& file, const char *message, bool bIsSuccess)
{
    bIsSuccess = bIsSuccess;
    log_callback(file.c_str(), message);
}
