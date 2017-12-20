#include "QAnalysisOrgFile.h"
#include <QFile>
#include <QFileInfo>
#include "Include/mydebug.h"
#include "Include/entity.h"

#define NULL_VALUE_MAXINT  (0x8000000)  //定义空值整数，若速度、限速等整数型变量为此值时，表示为空值
#define NULL_VALUE_STRING  ("@")        //定义空值字符串，@不常见，若信号机、工况状态等字符串型变量为此值时，表示为空值
#define NULL_VALUE_DATE  (QDate::fromString("1999-12-31", Qt::ISODate))        //36525=1999-12-31，定义此值，原因是转储原始文件中不可能出现1999年
extern const char* g_proName;

QAnalysisOrgFile::QAnalysisOrgFile(QObject *parent) :
    QObject(parent)
{
    m_FieldConvert = new QFieldConvert(this);
    m_CZBMAnalysis = new QCZBMAnalysis(this);
    m_bPingDiao = false;
}

void QAnalysisOrgFile::ReadHead(QString FileName, RLKJRTFileHeadInfo &HeadInfo)
{
    QByteArray msJsFile;
    QFile file(FileName);
    char EventHead[256];
    {
        if (!QFile::exists(FileName))
            return;
        if (!file.open(QFile::ReadOnly))
        {
            ERR("file :%s open failed\n", file.fileName().toStdString().c_str());
            return;
        }

        try
        {
            msJsFile = file.readAll();
            if (msJsFile.size() < 256)
                return;

            //读文件头256字节
            memset(EventHead, 0, sizeof(EventHead));
            memcpy(EventHead, msJsFile.constData(), sizeof(EventHead));
            if ((EventHead[0] != 0xB0) || (EventHead[1] != 0xF0)) {
                return; //文件标志0xB0F0
            }
            ReadHeadInfo(HeadInfo, EventHead, 256);
            strncpy(HeadInfo.strOrgFileName, FileName.toStdString().c_str(), 50); //原始文件名
        }
        catch (int) {}
    }
}


void QAnalysisOrgFile::ConvertToLkj(QString strFile, QLKJRuntimeFile *LkjFile)
{
    QByteArray msJsFile;
    RCommonInfo Info;
    uchar nEvent, nEvent2;
    char EventHead[256];
    //    char T[64];
    int i, nLen, nPos, nReadNum;
    QString strTemp;
    bool blnJoin;
    QFile file(strFile);

    LkjFile->Clear();
    InitCommonInfo(m_tPreviousInfo);
    if (!QFile::exists(strFile))
        return;
    if (!file.open(QFile::ReadOnly))
    {
        ERR("file :%s open failed\n", file.fileName().toStdString().c_str());
        return;
    }
    m_LKjFile = LkjFile;
    try
    {
        msJsFile = file.readAll();
        if (msJsFile.size() < 256)
            return;

        //读文件头256字节
        memset(EventHead, 0, sizeof(EventHead));
        memcpy(EventHead, msJsFile.constData(), sizeof(EventHead));
        if ((EventHead[0] != 0xB0) || (EventHead[1] != 0xF0)) {
            return; //文件标志0xB0F0
        }

        //        QFileInfo fileName(file);
        ReadHeadInfo(LkjFile->HeadInfo, EventHead, 256);
        strncpy(LkjFile->HeadInfo.strOrgFileName, strFile.toStdString().c_str(), 50);
        m_tPreviousInfo.nJKLineID = LkjFile->HeadInfo.nJKLineID; //监控交路号
        m_tPreviousInfo.nDataLineID = LkjFile->HeadInfo.nDataLineID; //数据交路号
        m_tPreviousInfo.nStation = LkjFile->HeadInfo.nStartStation; //车站号x
        m_tPreviousInfo.dtEventDate = QDateTime::fromTime_t(LkjFile->HeadInfo.dtKCDataTime).date(); //事件发生日期
        //增加文件头
        AddHeadInfoToLKJFile(LkjFile->HeadInfo, LkjFile->m_Records);
        //--------------------------------

        //读文件事件
        int j = 256;
        int size = msJsFile.size();
        while (j < size - 3)
        {
            nEvent = msJsFile.at(j++);
            if (nEvent <= 0x99) {
                continue;
            }
            //0x99 = 153
            //初始化Info
            InitCommonInfo(Info);
            blnJoin = false;

            //提取行信息
            QByteArray T_(1, nEvent);//?
            nLen = 1;
            nPos = j;
            T_.append(msJsFile.mid(j, 64));
            nReadNum = T_.size();
            j += nReadNum - 1;
            //nReadNum := msJsFile.Read(T[1], 64);
            nEvent2 = T_[1];
            for (i = 1; i < nReadNum; i++)
            {
                if (T_.at(i) > 0x99)
                {
                    //将事件与0xF0合并成一条揭示
                    if (T_.at(i) == 0xF0) {
                        blnJoin = true;
                    }
                    //将事件与0xF1合并成一条揭示
                    if (T_.at(i) == 0xF1) {
                        blnJoin = true;
                    }
                    //将事件0xCE与0xBA02合并成一条揭示
                    if (nEvent == 0xCE) {
                        if ((T_.at(i) == 0xBA) && (T_.at(i+1) == 0x02)) {
                            blnJoin = true;
                        }
                    }
                    //下列情况下，下一事件不处理
                    if ((nEvent == 0xA0) && (nEvent2 == 0x13)) {
                        if ((T_.at(i) == 0xA0) && (T_.at(i+1) == 0x14)) {
                            nPos = nPos + 2;
                        }
                    }

                    j = nPos;
                    break;
                }

                nPos = nPos + 1;
                nLen = nLen + 1;
            }

            //处理行信息，事件类别1字节+内容+未知1字节+校验和1字节
            if (nLen > 5) {
                DealOneFileInfo(Info, T_.data(), nLen);
            }

            //------------------------------------------------

            //处理不单独翻译的事件
            while (blnJoin)
            {
                blnJoin = false;

                //提取行信息
                T_.clear();
                T_.append(msJsFile.at(j++));
                nLen = 1;
                nPos = j;
                T_.append(msJsFile.mid(j, 64));
                nReadNum = T_.size();
                j += nReadNum - 1;
                DEB("nReadNum=%d\n", nReadNum);
                for (i = 1; i < nReadNum; i++)
                {
                    if (T_.at(i) > 0x99)
                    {
                        //将事件与0xF0合并成一条揭示
                        if (T_.at(i) == 0xF0) {
                            blnJoin = true;
                        }
                        //将事件与0xF1合并成一条揭示
                        if (T_.at(i) == 0xF1) {
                            blnJoin = true;
                        }
                        //将事件0xCE与0xBA02合并成一条揭示
                        if (nEvent == 0xCE) {
                            if ((T_.at(i) == 0xBA) && (T_.at(i+1) == 0x02)) {
                                blnJoin = true;
                            }
                        }

                        j = nPos;
                        break;
                    }
                    if ((T_.at(i) & 0x0F) > 0x09)
                    {
                        j = nPos;
                        break;
                    }

                    nPos = nPos + 1;
                    nLen = nLen + 1;
                }

                //处理行信息，事件类别1字节+内容+未知1字节+校验和1字节
                if (!Info.Disp.isEmpty()) {
                    if (nLen > 3) {
//                        qDebug() << Info.Disp;
                        DealOneFileInfo(Info, T_.data(), nLen);
                    }
                }
            }

            //生成事件记录列表
            if (!Info.Disp.isEmpty())
            {
                AdjustCommonInfo(Info); //调整Info
                Info.nStreamPostion = j;
                //将车次拆分成两条翻译，前一条为车次,后条为本补客货
                if (Info.Disp == "车次")
                {
                    strTemp = Info.ShuoMing;
                    Info.ShuoMing.clear();
                    Info.Rec = LkjFile->m_Records.size() + 1;
//                    TRACE("---\n");
                    LkjFile->m_Records.append(FileRowToLkjRec(Info));

                    Info.Disp = "本补客货";
                    Info.OTHER = strTemp;
                    Info.Rec = LkjFile->m_Records.size() + 1;
                    LkjFile->m_Records.append(FileRowToLkjRec(Info));
                }
                else
                {
                    Info.Rec = LkjFile->m_Records.size() + 1;
                    LkjFile->m_Records.append(FileRowToLkjRec(Info));
                }

                //根据Info，生成最近的历史数据FPreviousInfo
                if (nEvent != 0xD7) {
                    MakePreviousInfo(Info, nEvent);
                }
            } else
            {
                /*
            if IsShowExceptInfo(nEvent, nEvent2) then
            {
              Info.Disp := Format('%.02x - %.02x', [nEvent, nEvent2]);
              Info.Rec := LkjFile->m_Records.Count+1;
              LkjFile->m_Records.append(FileRowToLkjRec(Info));
            }
            */
            }
        }

        DealWithJgNumber(LkjFile);
        DealWithPosChance(LkjFile);
        //文件头-终点车站号
        if (m_tPreviousInfo.nStation != NULL_VALUE_MAXINT) {
            LkjFile->HeadInfo.nEndStation = m_tPreviousInfo.nStation;
        }
    }
    catch(int)
    {
        return;
    }

    m_LKjFile = NULL;
}

#if (0)
void QAnalysisOrgFile::ConvertToLkjFiles(QStringList strFileList, QLKJRuntimeFile *LkjFile)
{
    QStringList orderList;
    QDateTime dtMin;
    RLKJRTFileHeadInfo header;
    int i,nIndex;
    QStringList tempList = strFileList;
    //按文件的头信息中的运行记录生成时间排序,从小到大
    try
    {
        while (tempList.size() > 0)
        {
            dtMin = QDateTime::fromString("3017-10-17", Qt::ISODate);
            nIndex = -1;
            for (i = 0; i < tempList.size(); i++)
            {
                ReadHead(tempList[i],header);
                if (dtMin > header.DTFileHeadDt)
                {
                    dtMin = header.DTFileHeadDt;
                    nIndex = i;
                }
            }
            if (-1 != nIndex) {
                orderList.append(tempList.takeAt(nIndex));
            }
        }

        //将多个文件的运行记录组合成一个列表，并以时间最小的文件的头文件信息为
        //整个结构的头文件
        LkjFile->Clear();
        for (i = 0; i < orderList.size(); i++)
        {
            if (i == 0)
            {
                ReadHead(orderList[i],header);
            }
            ConvertToLkj(orderList[i], LkjFile);
        }
        LkjFile->HeadInfo = header;
    }
    catch (int)
    {
        return;
    }
}
#endif

int QAnalysisOrgFile::BCD2INT(uchar src)
{
    return ((src>>4/* & 0xf*/) * 10 + (src & 0xf));
}

int QAnalysisOrgFile::MoreBCD2INT(char *Buf, int nBegin, int nLen)
{
    int i;
    int ret = 0;
    for (i = 0; i < nLen; i++)
    {
        ret = ret*100 + BCD2INT(Buf[nBegin+i]);
    }
    return ret;
}

QString QAnalysisOrgFile::GetTime(char *Buf, int nBegin)
{
    uchar hh, nn, ss;
    QString ret(NULL_VALUE_STRING);
    hh = BCD2INT(Buf[nBegin]);
    nn = BCD2INT(Buf[nBegin+1]);
    ss = BCD2INT(Buf[nBegin+2]);
    if ((hh<=23) && (nn<=59) && (ss<=59)) {
        ret.sprintf("%.02d:%.02d:%.02d", hh, nn, ss);
    }
    return ret;
}

int QAnalysisOrgFile::GetGLB(char *Buf, int nBegin)
{
    int result = 0;
    int nValue = MoreBCD2INT(Buf, nBegin, 4);

    if (nValue >= 10000000) {
        nValue = -(nValue - 10000000);
    }
    result = nValue % 8388608;
    if (((nValue / 8388608) % 2) != 0) {
        result = -result;
    }
    return result;
}

int QAnalysisOrgFile::GetJL(char *Buf, int nBegin)
{
    int nValue;
    int result = 0;
    nValue = MoreBCD2INT(Buf, nBegin, 3);

    result = nValue % 885678;
    if (((nValue / 885678) % 2) != 0) { result = -result;}

    if (result > 60000) {
        result = result - 65536;
        //OutputDebugString('');
    }
    return result;
}

int QAnalysisOrgFile::GetSpeed(char *Buf, int nBegin)
{
    return MoreBCD2INT(Buf, nBegin, 2);
}

int QAnalysisOrgFile::GetLimitSpeed(char *Buf, int nBegin)
{
    return MoreBCD2INT(Buf, nBegin, 2);
}

int QAnalysisOrgFile::GetLieGuanPressure(char *Buf, int nBegin)
{
    return MoreBCD2INT(Buf, nBegin, 2);
}

int QAnalysisOrgFile::GetGangPressure(char *Buf, int nBegin)
{
    return MoreBCD2INT(Buf, nBegin, 2);
}

QString QAnalysisOrgFile::GetPZTypeName(char *Buf, int nBegin)
{
    QString Result = "未知";
    switch (MoreBCD2INT(Buf, nBegin, 1))
    {
    case 1: Result = "路票凭证号";
        break;
    case 2: Result = "绿色许可证凭证号";
        break;
    case 3: Result = "临时路票凭证号";
        break;
    case 4: Result = "临时绿色许可证凭证号";
        break;
    }
    return Result;
}


void QAnalysisOrgFile::InitCommonInfo(RCommonInfo &Info)
{
    Info.Rec = 0;                            //全程记录行号
    Info.Disp.clear();                       //事件描述
    Info.Hms = NULL_VALUE_STRING;            //时分秒
    Info.Glb = NULL_VALUE_MAXINT;            //公里标
    Info.Xhj = NULL_VALUE_STRING;            //信号机
    Info.Xht_code = NULL_VALUE_MAXINT;       //信号机类型
    Info.Xhj_no = NULL_VALUE_MAXINT;         //信号机编号6244
    Info.Xh_code = NULL_VALUE_MAXINT;        //信号（色灯或平调信号）
    Info.Speed = NULL_VALUE_MAXINT;          //速度
    Info.Shoub = NULL_VALUE_MAXINT;          //手柄状态（与工况状态取同一值）
    Info.Hand = NULL_VALUE_STRING;           //工况状态
    Info.Gya = NULL_VALUE_MAXINT;            //管压
    Info.Rota = NULL_VALUE_MAXINT;           //转速
    Info.S_lmt = NULL_VALUE_MAXINT;          //限度
    Info.Jl = NULL_VALUE_MAXINT;             //距离
    Info.Gangy = NULL_VALUE_MAXINT;          //闸压（缸压）
    Info.OTHER = NULL_VALUE_STRING;          //其它
    Info.Signal = NULL_VALUE_STRING;         //信号（色灯或平调信号）x
    Info.ShuoMing = NULL_VALUE_STRING;
    Info.Jg1 = NULL_VALUE_MAXINT;            //均缸1
    Info.Jg2 = NULL_VALUE_MAXINT;            //均缸2
    Info.JKZT = NULL_VALUE_MAXINT;           //

    Info.nJKLineID = NULL_VALUE_MAXINT;      //当前交路号
    Info.nDataLineID = NULL_VALUE_MAXINT;    //当前数据交路号
    Info.nStation = NULL_VALUE_MAXINT;       //当前车站号

    Info.strSpeedGrade = NULL_VALUE_STRING;  //速度等级
    Info.dtEventDate = NULL_VALUE_DATE;      //事件发生日期
}

void QAnalysisOrgFile::AdjustCommonInfo(RCommonInfo &Info)
{
    QTime dtNow, dtOld;

    if (Info.Hms == NULL_VALUE_STRING) {Info.Hms = m_tPreviousInfo.Hms;}            //时分秒
    if (Info.Glb == NULL_VALUE_MAXINT) {Info.Glb = m_tPreviousInfo.Glb;}             //公里标
    if (Info.Xhj == NULL_VALUE_STRING) {Info.Xhj = m_tPreviousInfo.Xhj;}            //???信号机
    if (Info.Xht_code == NULL_VALUE_MAXINT) {Info.Xht_code = m_tPreviousInfo.Xht_code;}        //???信号机类型
    if (Info.Xhj_no == NULL_VALUE_MAXINT) {Info.Xhj_no = m_tPreviousInfo.Xhj_no;}          //???信号机编号6244
    if (Info.Xh_code == NULL_VALUE_MAXINT) {Info.Xh_code = m_tPreviousInfo.Xh_code;}         //???信号（色灯或平调信号）
    if (Info.Speed == NULL_VALUE_MAXINT) {Info.Speed = m_tPreviousInfo.Speed; }          //速度
    if (Info.Shoub == NULL_VALUE_MAXINT) {Info.Shoub = m_tPreviousInfo.Shoub;}           //手柄状态（与工况状态取同一值）
    if (Info.Hand == NULL_VALUE_STRING) {Info.Hand = m_tPreviousInfo.Hand;}           //工况状态
    if (Info.Gya == NULL_VALUE_MAXINT) {Info.Gya = m_tPreviousInfo.Gya;}             //管压
    if (Info.Rota == NULL_VALUE_MAXINT) {Info.Rota = m_tPreviousInfo.Rota;}            //转速
    if (Info.S_lmt == NULL_VALUE_MAXINT) {Info.S_lmt = m_tPreviousInfo.S_lmt;}           //限度
    if (Info.Jl == NULL_VALUE_MAXINT) {Info.Jl = m_tPreviousInfo.Jl;}              //距离
    if (Info.Gangy == NULL_VALUE_MAXINT) {Info.Gangy = m_tPreviousInfo.Gangy;}           //闸压（缸压）
    if (Info.Signal == NULL_VALUE_STRING) {Info.Signal = m_tPreviousInfo.Signal;}         //信号（色灯或平调信号）x
    if (Info.Jg1 == NULL_VALUE_MAXINT) {Info.Jg1 = m_tPreviousInfo.Jg1;}             //均缸1
    if (Info.Jg2 == NULL_VALUE_MAXINT) {Info.Jg2 = m_tPreviousInfo.Jg2;}             //均缸2
    if (Info.JKZT == NULL_VALUE_MAXINT) {Info.JKZT = m_tPreviousInfo.JKZT;}            //

    if (Info.nJKLineID == NULL_VALUE_MAXINT) {Info.nJKLineID = m_tPreviousInfo.nJKLineID;}   //监控交路号
    if (Info.nDataLineID == NULL_VALUE_MAXINT) {Info.nDataLineID = m_tPreviousInfo.nDataLineID;}   //数据交路号
    if (Info.nStation == NULL_VALUE_MAXINT) {Info.nStation = m_tPreviousInfo.nStation;}   //车站号

    //事件发生日期
    if (Info.dtEventDate == NULL_VALUE_DATE)
    {
        Info.dtEventDate = m_tPreviousInfo.dtEventDate;

        if ((Info.Hms != NULL_VALUE_STRING) && (m_tPreviousInfo.Hms != NULL_VALUE_STRING))
        {
            if (Info.dtEventDate != NULL_VALUE_DATE)
            {
                dtNow = QTime::fromString(Info.Hms);
                dtOld = QTime::fromString(m_tPreviousInfo.Hms);
                if (dtNow < dtOld) {Info.dtEventDate.addDays(1);}
            }
        }
    }
}

void QAnalysisOrgFile::MakePreviousInfo(RCommonInfo &Info, uchar nType)
{
    if (Info.Hms != NULL_VALUE_STRING) {m_tPreviousInfo.Hms = Info.Hms;}            //时分秒
    if (Info.Glb != NULL_VALUE_MAXINT) {m_tPreviousInfo.Glb = Info.Glb;}             //公里标

    if (nType != 0xCE)
    {
        if (Info.Xhj != NULL_VALUE_STRING) {m_tPreviousInfo.Xhj = Info.Xhj;}            //???信号机
        if (Info.Xht_code != NULL_VALUE_MAXINT) {m_tPreviousInfo.Xht_code = Info.Xht_code;}        //???信号机类型
        if (Info.Xhj_no != NULL_VALUE_MAXINT) {m_tPreviousInfo.Xhj_no = Info.Xhj_no;}          //???信号机编号6244
    }

    if (Info.Xh_code != NULL_VALUE_MAXINT) {m_tPreviousInfo.Xh_code = Info.Xh_code;}         //???信号（色灯或平调信号）
    if (Info.Speed != NULL_VALUE_MAXINT) {m_tPreviousInfo.Speed = Info.Speed;}           //速度
    if (Info.Shoub != NULL_VALUE_MAXINT) {m_tPreviousInfo.Shoub = Info.Shoub;}           //手柄状态（与工况状态取同一值）
    if (Info.Hand != NULL_VALUE_STRING) {m_tPreviousInfo.Hand = Info.Hand;}           //工况状态
    if (Info.Gya != NULL_VALUE_MAXINT) {m_tPreviousInfo.Gya = Info.Gya;}             //管压
    if (Info.Rota != NULL_VALUE_MAXINT) {m_tPreviousInfo.Rota = Info.Rota;}            //转速
    if (Info.S_lmt != NULL_VALUE_MAXINT) {m_tPreviousInfo.S_lmt = Info.S_lmt;}           //限度
    if (Info.Jl != NULL_VALUE_MAXINT) {m_tPreviousInfo.Jl = Info.Jl;}              //距离
    if (Info.Gangy != NULL_VALUE_MAXINT) {m_tPreviousInfo.Gangy = Info.Gangy;}           //闸压（缸压）
    if (Info.Signal != NULL_VALUE_STRING) {m_tPreviousInfo.Signal = Info.Signal;}         //信号（色灯或平调信号）x
    if (Info.Jg1 != NULL_VALUE_MAXINT) {m_tPreviousInfo.Jg1 = Info.Jg1;}             //均缸1
    if (Info.Jg2 != NULL_VALUE_MAXINT) {m_tPreviousInfo.Jg2 = Info.Jg2;}             //均缸2
    if (Info.JKZT != NULL_VALUE_MAXINT) {m_tPreviousInfo.JKZT = Info.JKZT;}            //

    if (Info.nJKLineID != NULL_VALUE_MAXINT) {m_tPreviousInfo.nJKLineID = Info.nJKLineID;}   //监控交路号
    if (Info.nDataLineID != NULL_VALUE_MAXINT) {m_tPreviousInfo.nDataLineID = Info.nDataLineID;}   //数据交路号
    if (Info.nStation != NULL_VALUE_MAXINT) {m_tPreviousInfo.nStation = Info.nStation;}   //车站号

    if (Info.dtEventDate != NULL_VALUE_DATE) {m_tPreviousInfo.dtEventDate = Info.dtEventDate;}   //事件发生日期
}

QString QAnalysisOrgFile::GetLamp(quint16 nWord)
{
    uchar nType;
    QString strLamp;
    int intLampNum;
    bool bSplash;
    intLampNum = 0;
    strLamp = NULL_VALUE_STRING;

    nType = nWord & 0x00FF;
    bSplash = (nWord & 0x0800) == 0x0800;

    if ((nType & 0x01) == 0x01)
    {
        strLamp = "绿灯";
        intLampNum = intLampNum + 1;
    }
    if ((nType & 0x02) == 0x02)
    {
        strLamp = "绿黄";
        intLampNum = intLampNum + 1;
    }
    if ((nType & 0x04) == 0x04)
    {
        strLamp = "黄灯";
        intLampNum = intLampNum + 1;
    }
    if ((nType & 0x08) == 0x08)
    {
        strLamp = "黄2"; //闪灯
        intLampNum = intLampNum + 1;
    }
    if ((nType & 0x10) == 0x10)
    {
        strLamp = "双黄"; //闪灯
        intLampNum = intLampNum + 1;
    }
    if ((nType & 0x20) == 0x20)
    {
        strLamp = "红黄"; //闪灯
        intLampNum = intLampNum + 1;
    }
    if ((nType & 0x40) == 0x40)
    {
        strLamp = "红灯";
        intLampNum = intLampNum + 1;
    }
    if ((nType & 0x80) == 0x80)
    {
        strLamp = "白灯"; //Xh_code=7
        intLampNum = intLampNum + 1;
    }

    if (bSplash) {
        if ((nType & 0x08)||(nType & 0x10)||(nType & 0x20)){
            strLamp = strLamp + "闪";
        }
    }
    if (intLampNum == 0) {
        strLamp = "灭灯"; //Xh_code=8
    }
    if ((m_bPingDiao) && (intLampNum == 0))
    {
        strLamp = m_tPreviousInfo.Signal;
    }
    if (intLampNum > 1) {
        strLamp = "多灯";
    }

    return strLamp;
}

EM_LampSign QAnalysisOrgFile::GetLampType(QString strLamp)
{
    EM_LampSign result;
    if (strLamp == "绿灯") {result = lsGreen;}
    else if (strLamp == "绿黄") {result = lsGreenYellow;}
    else if (strLamp == "黄灯") {result = lsYellow;}
    else if (strLamp == "黄2") {result = lsYellow2;}
    else if (strLamp == "双黄") {result = lsDoubleYellow;}
    else if (strLamp == "红黄") {result = lsRedYellow;}
    else if (strLamp == "红灯") {result = lsRed;}
    else if (strLamp == "白灯") {result = lsWhite;}
    else if (strLamp == "黄2闪") {result = lsYellow2S;} //闪灯
    else if (strLamp == "双黄闪") {result = lsDoubleYellowS;} //闪灯
    else if (strLamp == "红黄闪") {result = lsRedYellowS;} //闪灯
    else if (strLamp == "灭灯") {result = lsClose;}
    else if (strLamp == "多灯") {result = lsMulti;}
    else if (strLamp == "启动") {result = lsPDQiDong;}
    else if (strLamp == "推进") { result = lsPDTuiJin;}
    else if (strLamp == "连接") {result = lsPDLianJie;}
    else if (strLamp == "十车") {result = lsPDShiChe;}
    else if (strLamp == "五车") {result = lsPDWuChe;}
    else if (strLamp == "三车") {result = lsPDSanChe;}
    else {result = lsGreen;;}
    return result;
}

EM_LKJSignType QAnalysisOrgFile::GetXhjType(uchar nxhj_type)
{
    EM_LKJSignType result;
    switch (nxhj_type)
    {
    case 1: result = stInOut;break;
    case 2: result = stOut;break;
    case 3: result = stIn;break;
    case 4: result = stNormal;break;
    case 5: result = stPre;break;
    case 6: result = stAllow; //'容许';
        break;
    case 7: result = stDivision;  //'分割';
        break;
    case 9: result = stPre;     //'1预告';
        break;
    case 10: result = stPre;  //'接近';
        break;
        // $80: result = '平调';
    default:
        result = stNone;
        break;
    }
    return result;
}


void QAnalysisOrgFile::ReadHeadInfo(RLKJRTFileHeadInfo &Head, char *Buf, int Len)
{
    if (Len < 135) {
        ERR("data is too little!!! len = %d\n", Len);
        return;
    }
    uchar nByte;
    //解析赋值
    clearRLKJRTFileHeadInfo(Head);
    QString datetime = QString("%1-%2-%3 %4:%5:%6").arg(2000 + BCD2INT(Buf[2])).arg(BCD2INT(Buf[3])).arg(BCD2INT(Buf[4])).arg(BCD2INT(Buf[5])).arg(BCD2INT(Buf[6])).arg(BCD2INT(Buf[7]));//文件头时间
    Head.DTFileHeadDt = QDateTime::fromString(datetime, "yyyy-M-d h:m:s").toTime_t();
    //机车客货类别(货,客)代码[数字]
    nByte = BCD2INT(Buf[9]) % 4;
    if ((nByte % 2) == 1)
        Head.TrainType = ttPassenger;
    else
        Head.TrainType = ttCargo;
    //本机、补机
    if ((nByte / 2) == 1)
        Head.BenBu = bbBu;
    else
        Head.BenBu = bbBen;
    //    DEB("sizeof:%d\n", sizeof(Head.strTrainHead));
    memset(Head.strTrainHead, 0, sizeof(Head.strTrainHead));
    memcpy(Head.strTrainHead, &Buf[10], 4);
    //    Head.strTrainHead = trim(chr(Buf[10])+ chr(Buf[11])+ chr(Buf[12])+ chr(Buf[13]));  //车次头
    Head.nTrainNo = MoreBCD2INT(Buf, 14, 3);  //车次号
    Head.nDataLineID = MoreBCD2INT(Buf, 17, 1); //数据交路号
    Head.nJKLineID = MoreBCD2INT(Buf, 18, 3) & 0xFFFF; //交路号
    Head.nStartStation = MoreBCD2INT(Buf, 21, 3) & 0xFFFF; //始发站
    Head.nEndStation = Head.nStartStation; //终点站 //LkjFile.HeadInfo.nEndStation = LkjFile->m_Records[LkjFile->m_Records.Count - 1].CommonRec.nStation;
    Head.nFirstDriverNO = MoreBCD2INT(Buf, 24, 4); //司机工号
    Head.nSecondDriverNO = MoreBCD2INT(Buf, 28, 4); //副司机工号
    Head.nTotalWeight = MoreBCD2INT(Buf, 34, 3) & 0xFFFF; //总重
    Head.nLoadWeight = MoreBCD2INT(Buf, 37, 3) & 0xFFFF; //载重
    Head.nKeCheNum = MoreBCD2INT(Buf, 40, 2); //客车数量
    Head.nZhongCheNum = MoreBCD2INT(Buf, 42, 2); //重车数量
    Head.nKongCheNum = MoreBCD2INT(Buf, 44, 2); //空车数量
    Head.nFeiYunYongChe = MoreBCD2INT(Buf, 46, 2); //非运用车
    Head.nDaiKeChe = MoreBCD2INT(Buf, 48, 2); //代客车
    Head.nShouChe = MoreBCD2INT(Buf, 50, 2); //守车
    Head.nSum = MoreBCD2INT(Buf, 52, 2); //合计 辆数
    Head.nLength = MoreBCD2INT(Buf, 54, 2); //计长
    Head.nLocoType = MoreBCD2INT(Buf, 56, 3) & 0xFFFF; //机车类型号(DF11)代码[数字]
    Head.nLocoID = MoreBCD2INT(Buf, 60, 3) & 0xFFFF; //机车编号
    Head.nLunJing = MoreBCD2INT(Buf, 64, 3) & 0xFFFF;  //轮径
    Head.nBeiZhouLunJin = MoreBCD2INT(Buf, 67, 3) & 0xFFFF;  //备轴轮径
    QString version = QString("%1-%2-%3").arg(MoreBCD2INT(Buf, 70, 2)).arg(BCD2INT(Buf[72])).arg(BCD2INT(Buf[73]));
    //    DEB("sizeof:%d\n", sizeof(Head.strDisVerOne));
    memset(Head.strDisVerOne, 0, sizeof(Head.strDisVerOne));
    strncpy(Head.strDisVerOne, version.toStdString().c_str(), version.size());//一端显示版本
    strcpy(Head.strDisVerTwo, Head.strDisVerOne);
    //    Head.strDisVerOne = QString::number(BCD2INT(Buf[70])) + QString::number(BCD2INT(Buf[71])) +
    //      '-' + QString::number(BCD2INT(Buf[72])) + '-' + QString::number(BCD2INT(Buf[73]));  //一端显示版本
    //    Head.strDisVerTwo = Head.strDisVerOne;
    Head.nJKVersion = MoreBCD2INT(Buf, 78,4); //监控版本
    Head.nDataVersion = MoreBCD2INT(Buf, 82,4); //数据版本

    nByte = Buf[86]; //软件厂家
    if (nByte == 0x53)
        Head.Factory = sfSiWei;
    else
        Head.Factory = sfZhuZhou;

    Head.nDeviceNo = MoreBCD2INT(Buf, 89, 3) & 0xFFFF; //装置号
    QString ICDump = QString("%1-%2").arg(BCD2INT(Buf[96])).arg(BCD2INT(Buf[97]));  //IC卡转储日期
    //    DEB("sizeof:%d\n", sizeof(Head.strICDump));
    memset(Head.strICDump, 0, sizeof(Head.strICDump));
    strncpy(Head.strICDump, ICDump.toStdString().c_str(), ICDump.size());//IC卡转储日期
    Head.nSpeedChannelNo = MoreBCD2INT(Buf, 128, 2) & 0xFFFF; //当前速度通道号
    Head.nMaxLmtSpd = MoreBCD2INT(Buf, 131, 2) & 0xFFFF; //输入最高限速
    //===HeadInfo.strOrgFileName = ExtractFileName(orgFileName);  //原始文件名
    Head.dtKCDataTime = Head.DTFileHeadDt;
}

void QAnalysisOrgFile::AddHeadInfoToLKJFile(const RLKJRTFileHeadInfo &Head, TLkjCommonRecLst &LkjCommonRecLst)
{
    QLKJCommonRec LKJCommonRec;
    LKJCommonRec.CommonRec.nRow = 1;
    LKJCommonRec.CommonRec.strDisp = "文件开始";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QDateTime::fromTime_t(Head.dtKCDataTime).toString("yyyy-MM-dd");
    LKJCommonRec.CommonRec.nEvent = m_FieldConvert->GetnEvent(LKJCommonRec.CommonRec.strDisp);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 2;
    LKJCommonRec.CommonRec.strDisp = "厂家标志";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    if (sfSiWei == Head.Factory) {
        LKJCommonRec.CommonRec.strOther = "思维";
    } else {
        LKJCommonRec.CommonRec.strOther = "株洲所";
    }
    LKJCommonRec.CommonRec.nEvent = m_FieldConvert->GetnEvent(LKJCommonRec.CommonRec.strDisp);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 3;
    LKJCommonRec.CommonRec.strDisp = "本补客货";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    if (ttPassenger == Head.TrainType) {
        LKJCommonRec.CommonRec.strOther = "客本";
    } else if (ttCargo == Head.TrainType) {
        LKJCommonRec.CommonRec.strOther = "货本";
    } else {
        LKJCommonRec.CommonRec.strOther = "客/货";
    }
    LKJCommonRec.CommonRec.nEvent = m_FieldConvert->GetnEvent(LKJCommonRec.CommonRec.strDisp);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 4;
    LKJCommonRec.CommonRec.strDisp = "车次";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString("%1%2").arg(Head.strTrainHead).arg(Head.nTrainNo);
    LKJCommonRec.CommonRec.nEvent = m_FieldConvert->GetnEvent(LKJCommonRec.CommonRec.strDisp);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 5;
    LKJCommonRec.CommonRec.strDisp = "数据交路号";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nDataLineID);
    LKJCommonRec.CommonRec.nEvent = m_FieldConvert->GetnEvent(LKJCommonRec.CommonRec.strDisp);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 6;
    LKJCommonRec.CommonRec.strDisp = "监控交路号";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nJKLineID);
    LKJCommonRec.CommonRec.nEvent = m_FieldConvert->GetnEvent(LKJCommonRec.CommonRec.strDisp);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 7;
    LKJCommonRec.CommonRec.strDisp = "车站号";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nStartStation);
    LKJCommonRec.CommonRec.nEvent = m_FieldConvert->GetnEvent(LKJCommonRec.CommonRec.strDisp);
    LKJCommonRec.CommonRec.strXhj = m_CZBMAnalysis->GetStationName(Head.nDataLineID, Head.nStartStation);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nEvent = 0;  //重置为0
    LKJCommonRec.CommonRec.strXhj.clear();

    LKJCommonRec.CommonRec.nRow = 8;
    LKJCommonRec.CommonRec.strDisp = "司机号";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nFirstDriverNO);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 9;
    LKJCommonRec.CommonRec.strDisp = "副司机号";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nSecondDriverNO);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 10;
    LKJCommonRec.CommonRec.strDisp = "总重";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nTotalWeight);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 11;
    LKJCommonRec.CommonRec.strDisp = "辆数";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nSum);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 12;
    LKJCommonRec.CommonRec.strDisp = "计长";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString("%1.%2").arg(Head.nLength / 10).arg(Head.nLength % 10);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 13;
    LKJCommonRec.CommonRec.strDisp = "载重";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nLoadWeight);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 14;
    LKJCommonRec.CommonRec.strDisp = "客车";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nKeCheNum);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 15;
    LKJCommonRec.CommonRec.strDisp = "重车";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nZhongCheNum);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 16;
    LKJCommonRec.CommonRec.strDisp = "空车";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nKeCheNum);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 17;
    LKJCommonRec.CommonRec.strDisp = "非运用车";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nFeiYunYongChe);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 18;
    LKJCommonRec.CommonRec.strDisp = "代客车";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nDaiKeChe);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 19;
    LKJCommonRec.CommonRec.strDisp = "守车";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nShouChe);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 20;
    if (Head.nMaxLmtSpd == 0) {
        LKJCommonRec.CommonRec.strDisp = "按监控交路限速";
    } else {
        LKJCommonRec.CommonRec.strDisp = "输入最高限速";
        LKJCommonRec.CommonRec.strOther = QString("%1Km/h").arg(Head.nMaxLmtSpd);
    }
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 21;
    LKJCommonRec.CommonRec.strDisp = "机车号修改";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nLocoID);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 22;
    LKJCommonRec.CommonRec.strDisp = "机车型号修改";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nLocoType);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 23;
    LKJCommonRec.CommonRec.strDisp = "轮径修改";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString("%1.%2").arg(Head.nLunJing / 10).arg(Head.nLunJing % 10);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 24;
    LKJCommonRec.CommonRec.strDisp = "备轴轮径";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString("%1.%2").arg(Head.nBeiZhouLunJin / 10).arg(Head.nBeiZhouLunJin % 10);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 25;
    LKJCommonRec.CommonRec.strDisp = "装置号修改";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nDeviceNo);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 26;
    LKJCommonRec.CommonRec.strDisp = "当前速度通道号";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = QString::number(Head.nSpeedChannelNo);
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 27;
    LKJCommonRec.CommonRec.strDisp = "一端显示版本";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = Head.strDisVerOne;
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 28;
    LKJCommonRec.CommonRec.strDisp = "二端显示版本";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = Head.strDisVerTwo;
    LkjCommonRecLst.append(LKJCommonRec);

    LKJCommonRec.CommonRec.nRow = 29;
    LKJCommonRec.CommonRec.strDisp = "IC卡转储时间";
    LKJCommonRec.CommonRec.DTEvent = Head.dtKCDataTime;
    LKJCommonRec.CommonRec.strOther = Head.strICDump;
    LkjCommonRecLst.append(LKJCommonRec);
}

void QAnalysisOrgFile::AddJieShiInfoToLKJFile(RLKJJieShiInfo &JieShiInfo)
{
//    DEB("add jie shi Info LKJ File!\n");
    if (NULL == m_LKjFile)
    {
        return ;
    }
    m_LKjFile->JieShiInfoArr.append(JieShiInfo);
}


QLKJCommonRec QAnalysisOrgFile::FileRowToLkjRec(RCommonInfo Info)
{
    QLKJCommonRec Result;

    Result.CommonRec.nRow = Info.Rec;
    Result.CommonRec.strDisp = Info.Disp;
    Result.CommonRec.nEvent = m_FieldConvert->GetnEvent(Info.Disp);
    Result.CommonRec.DTEvent = QDateTime(Info.dtEventDate, QTime::fromString(Info.Hms)).toTime_t();
//    qDebug() << Info.Rec << Info.Hms << Result.CommonRec.DTEvent;
    Result.CommonRec.strGK = (Info.Hand != NULL_VALUE_STRING ? Info.Hand : "");
    Result.CommonRec.nCoord = (Info.Glb != NULL_VALUE_MAXINT ? Info.Glb: 0);
    Result.CommonRec.nDistance = (Info.Jl != NULL_VALUE_MAXINT ? Info.Jl: 0);
    Result.CommonRec.strXhj = (Info.Xhj != NULL_VALUE_STRING ? Info.Xhj : "");
    //    qDebug() << Info.Xhj << Info.Xhj.toAscii().toHex();
    Result.CommonRec.strSignal = (Info.Signal != NULL_VALUE_STRING ? Info.Signal : "");
    Result.CommonRec.nLieGuanPressure = (Info.Gya != NULL_VALUE_MAXINT ? Info.Gya : 0);
    Result.CommonRec.nGangPressure = (Info.Gangy != NULL_VALUE_MAXINT ? Info.Gangy : 0);
    Result.CommonRec.nJG1Pressure = (Info.Jg1 != NULL_VALUE_MAXINT ? Info.Jg1 : 0);
    Result.CommonRec.nJG2Pressure = (Info.Jg2 != NULL_VALUE_MAXINT ? Info.Jg2 : 0);
    Result.CommonRec.nSpeed = (Info.Speed != NULL_VALUE_MAXINT ? Info.Speed : 0);
    Result.CommonRec.nLimitSpeed = (Info.S_lmt != NULL_VALUE_MAXINT ? Info.S_lmt : 0);
    Result.CommonRec.nRotate = (Info.Rota != NULL_VALUE_MAXINT ? Info.Rota : 0);
    Result.CommonRec.strOther = (Info.OTHER != NULL_VALUE_STRING ? Info.OTHER : "");
    Result.CommonRec.ShuoMing = (Info.ShuoMing != NULL_VALUE_STRING ? Info.ShuoMing : "");
    Result.CommonRec.JKZT = (Info.JKZT != NULL_VALUE_MAXINT ? Info.JKZT : 0);

    if (Info.Signal != NULL_VALUE_STRING) { Result.CommonRec.LampSign = GetLampType(Info.Signal);}
    Result.CommonRec.SignType = (Info.Xht_code != NULL_VALUE_MAXINT ? GetXhjType(Info.Xht_code) : stNone);
    Result.CommonRec.nLampNo = (Info.Xhj_no != NULL_VALUE_MAXINT ? Info.Xhj_no : 0);

    if (Info.Shoub != NULL_VALUE_MAXINT)
    {
        Result.CommonRec.nShoub = Info.Shoub;
        Result.CommonRec.WorkZero = m_FieldConvert->ConvertWorkZero(Result.CommonRec.nShoub);
        Result.CommonRec.WorkDrag = m_FieldConvert->ConvertWorkDrag(Result.CommonRec.nShoub);
        Result.CommonRec.HandPos = m_FieldConvert->ConvertHandPos(Result.CommonRec.nShoub);
    }

    Result.CommonRec.nJKLineID = (Info.nJKLineID != NULL_VALUE_MAXINT ? Info.nJKLineID : 0);
    Result.CommonRec.nDataLineID = (Info.nDataLineID != NULL_VALUE_MAXINT ? Info.nDataLineID : 0);
    Result.CommonRec.nStation = (Info.nStation != NULL_VALUE_MAXINT ? Info.nStation : 0);
    Result.CommonRec.nStreamPostion = Info.nStreamPostion;

    return Result;
}


void QAnalysisOrgFile::DealOneFileInfo(RCommonInfo &Info, char *Buf, int Len)
{
    int i, n;

    //过滤结尾的不合法字符
    n = Len - 1;
    for (i = n; i >= 0; i--)
    {
        if ((Buf[i] & 0x0F) > 9) {
            Len = Len - 1;
        } else {
            break;
        }
    }
    if (Len <= 3) {
        return;
    }

    //根据事件类型，分类处理
    switch (Buf[0])
    {
    case 0xC0: MakeOneLkjRec_C0(Info, Buf, Len); //关机
        break;
    case 0xC1: MakeOneLkjRec_C1(Info, Buf, Len); //开机
        break;
    case 0xC2: MakeOneLkjRec_C2(Info, Buf, Len); //公里标突变
        break;
    case 0xC3: MakeOneLkjRec_C3(Info, Buf, Len); //座标增
        break;
    case 0xC4: MakeOneLkjRec_C4(Info, Buf, Len); //座标减
        break;
    case 0xC5: MakeOneLkjRec_C5(Info, Buf, Len); //过机不校
        break;
    case 0xC6: MakeOneLkjRec_C6(Info, Buf, Len); //过机校正
        break;
    case 0xC7: MakeOneLkjRec_C7(Info, Buf, Len); //过站中心
        break;
    case 0xC8: MakeOneLkjRec_C8(Info, Buf, Len); //报警开始
        break;
    case 0xC9: MakeOneLkjRec_C9(Info, Buf, Len); //报警结束
        break;
    case 0xCA: MakeOneLkjRec_CA(Info, Buf, Len); //手柄防溜报警开始
        break;
    case 0xCB: MakeOneLkjRec_CB(Info, Buf, Len); //手柄防溜报警结束
        break;
    case 0xCC: MakeOneLkjRec_CC(Info, Buf, Len); //进站道岔 出站道岔
        break;
    case 0xCD: MakeOneLkjRec_CD(Info, Buf, Len); //日期变化
        break;
    case 0xCE: MakeOneLkjRec_CE(Info, Buf, Len); //过信号机
        break;
    case 0xCF: MakeOneLkjRec_CF(Info, Buf, Len); //正线终止
        break;
    case 0xD0: MakeOneLkjRec_D0(Info, Buf, Len); //站内停车
        break;
    case 0xD1: MakeOneLkjRec_D1(Info, Buf, Len); //站内开车
        break;
    case 0xD2: MakeOneLkjRec_D2(Info, Buf, Len); //轮对空转
        break;
    case 0xD3: MakeOneLkjRec_D3(Info, Buf, Len); //空转结束
        break;
    case 0xD7: MakeOneLkjRec_D7(Info, Buf, Len); //轮径修正
        break;
    case 0xE0: MakeOneLkjRec_E0(Info, Buf, Len); //机车信号变化
        break;
    case 0xE1: MakeOneLkjRec_E1(Info, Buf, Len); //制式电平变化
        break;
    case 0xE3: MakeOneLkjRec_E3(Info, Buf, Len); //机车工况变化
        break;
    case 0xE5: MakeOneLkjRec_E5(Info, Buf, Len); //平调信号变化
        break;
    case 0xE6: MakeOneLkjRec_E6(Info, Buf, Len); //速度变化
        break;
    case 0xE7: MakeOneLkjRec_E7(Info, Buf, Len); //转速变化
        break;
    case 0xEB: MakeOneLkjRec_EB(Info, Buf, Len); //管压变化
        break;
    case 0xEC: MakeOneLkjRec_EC(Info, Buf, Len); //限速变化
        break;
    case 0xED: MakeOneLkjRec_ED(Info, Buf, Len); //定量记录
        break;
    case 0xEE: MakeOneLkjRec_EE(Info, Buf, Len); //闸缸压力变化
        break;
    case 0xEF: MakeOneLkjRec_EF(Info, Buf, Len); //均缸压力变化
        break;

    case 0xA0: MakeOneLkjRec_A0(Info, Buf, Len); //A机模块通讯故障...
        break;
    case 0xA4: MakeOneLkjRec_A4(Info, Buf, Len); //A机模块通讯恢复...
        break;
    case 0xA8: MakeOneLkjRec_A8(Info, Buf, Len); //日期修改...
        break;
    case 0xB1: MakeOneLkjRec_B1(Info, Buf, Len); //提示更新...
        break;
    case 0xB4: MakeOneLkjRec_B4(Info, Buf, Len); //A主B备...
        break;
    case 0xB6: MakeOneLkjRec_B6(Info, Buf, Len); //+++文档没有，分析补充
        break;
    case 0xB7: MakeOneLkjRec_B7(Info, Buf, Len);
        break;
    case 0xB8: MakeOneLkjRec_B8(Info, Buf, Len);
        break;
    case 0xB9: MakeOneLkjRec_B9(Info, Buf, Len); //空转报警开始
        break;
    case 0xBE: MakeOneLkjRec_BE(Info, Buf, Len);
        break;
    case 0xDA: MakeOneLkjRec_DA(Info, Buf, Len);
        break;
    case 0xDB: MakeOneLkjRec_DB(Info, Buf, Len);
        break;
    case 0xDC: MakeOneLkjRec_DC(Info, Buf, Len);
        break;
    case 0xDD: MakeOneLkjRec_DD(Info, Buf, Len);
        break;
        //下面不单独翻译
    case 0xBA: MakeOneLkjRec_BA(Info, Buf, Len); //if Buf[1] = 0x02 then MakeOneLkjRec_BA02(Info, Buf, Len);
        break;
    case 0xF0: MakeOneLkjRec_F0(Info, Buf, Len);
        break;
    case 0xF1: MakeOneLkjRec_F1(Info, Buf, Len);
        break;
    default:
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_A0(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType, nByte;
    QString strTemp;
    //解析时间
    Info.Hms = GetTime(Buf, 2);

    //解析其它
    nType = Buf[1];
    switch (nType)
    {
    case 0x13:
        if (Len == 16)
        {
            Info.Disp = "速度通道切换";
            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.OTHER.sprintf("%d->%d", BCD2INT(Buf[12]), BCD2INT(Buf[13]));
        }
        break;
    case 0x14:
        if (Len == 8)
        {
            Info.Disp = "速度通道号";
            Info.OTHER = QString::number(BCD2INT(Buf[5]));
        }
        break;
    case 0x26:
    case 0x27:
        if (Len == 15)
        {
            if (nType == 0x26) {Info.Disp = "A数字入检测";}
            else if (nType == 0x27) {Info.Disp = "B数字入检测";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            strTemp = "";
            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {strTemp = "绿灯";}
            else if (nByte == 1) {strTemp = "绿黄";}
            else if (nByte == 2) {strTemp = "黄灯";}
            else if (nByte == 3) {strTemp = "黄2灯";}
            else if (nByte == 4) {strTemp = "双黄灯";}
            else if (nByte == 5) {strTemp = "红黄灯";}
            else if (nByte == 6) {strTemp = "红灯";}
            else if (nByte == 7) {strTemp = "白灯";}
            else if (nByte == 8) {strTemp = "速度0";}
            else if (nByte == 9) {strTemp = "速度1";}
            else if (nByte == 10) {strTemp = "速度2";}
            else if (nByte == 11) {strTemp = "UM71制式";}
            else if (nByte == 12) {strTemp = "电平信号";}
            else if (nByte == 13) {strTemp = "备用1";}
            else if (nByte == 14) {strTemp = "备用2";}
            else if (nByte == 15) {strTemp = "备用3";}
            if (strTemp != "") {Info.ShuoMing = QString("%1-%2").arg(nByte).arg(strTemp);}
            else {Info.ShuoMing.sprintf("保留%.02xH", nByte);}
        }
        break;
    case 0x30:
    case 0x31:
        if (Len == 15)
        {
            if (nType == 0x30) {Info.Disp = "A数字出检测";}
            else if (nType == 0x31) {Info.Disp = "B数字出检测";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            strTemp = "";
            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {strTemp = "卸载";}
            else if (nByte == 1) {strTemp = "减压";}
            else if (nByte == 2) {strTemp = "关风";}
            else if (nByte == 3) {strTemp = "备用1";}
            else if (nByte == 4) {strTemp = "备用2";}
            else if (nByte == 5) {strTemp = "备用3";}
            else if (nByte == 6) {strTemp = "紧急";}

            if (strTemp != "") {Info.ShuoMing = QString("%1-%2").arg(nByte).arg(strTemp);}
            else {Info.ShuoMing.sprintf("保留%.02xH", nByte);}
        }
        break;
    case 0x34:
    case 0x35:
        if (Len == 15)
        {
            if (nType == 0x34) {Info.Disp = "A模块故障";}
            else if (nType == 0x35) {Info.Disp = "B模块故障";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            strTemp = "";
            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {strTemp = "信息处理板A";}
            else if (nByte == 1) {strTemp = "信息处理板B";}
            else if (nByte == 2) {strTemp = "通讯板A";}
            else if (nByte == 3) {strTemp = "通讯板B";}
            else if (nByte == 4) {strTemp = "一端显示器";}
            else if (nByte == 5) {strTemp = "二端显示器";}
            else if (nByte == 6) {strTemp = "黑匣子";}
            else if (nByte == 7) {strTemp = "串行机车信号";}
            else if (nByte == 8) {strTemp = "扩展通信板A";}
            else if (nByte == 9) {strTemp = "扩展通信板B";}
            else if (nByte == 10) {strTemp = "无线数传";}

            if (strTemp != "") {Info.ShuoMing = QString("%1-%2").arg(nByte).arg(strTemp);}
            else {Info.ShuoMing.sprintf("保留%.02xH", nByte);}
        }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_A4(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType, nByte;
    QString strTemp;
    //解析时间
    Info.Hms = GetTime(Buf, 2);

    //解析其它
    nType = Buf[1];
    switch (nType)
    {
    case 0x26:
    case 0x27:
        if (Len == 15)
        {
            if (nType == 0x26) {Info.Disp = "A数字入恢复";}
            else if (nType == 0x27) {Info.Disp = "B数字入恢复";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            strTemp = "";
            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {strTemp = "绿灯";}
            else if (nByte == 1) {strTemp = "绿黄";}
            else if (nByte == 2) {strTemp = "黄灯";}
            else if (nByte == 3) {strTemp = "黄2灯";}
            else if (nByte == 4) {strTemp = "双黄灯";}
            else if (nByte == 5) {strTemp = "红黄灯";}
            else if (nByte == 6) {strTemp = "红灯";}
            else if (nByte == 7) {strTemp = "白灯";}
            else if (nByte == 8) {strTemp = "速度0";}
            else if (nByte == 9) {strTemp = "速度1";}
            else if (nByte == 10) {strTemp = "速度2";}
            else if (nByte == 11) {strTemp = "UM71制式";}
            else if (nByte == 12) {strTemp = "电平信号";}
            else if (nByte == 13) {strTemp = "备用1";}
            else if (nByte == 14) {strTemp = "备用2";}
            else if (nByte == 15) {strTemp = "备用3";}
            if (strTemp != "") {Info.ShuoMing = QString("%1-%2").arg(nByte).arg(strTemp);}
            else {Info.ShuoMing.sprintf("保留%.02xH", nByte);}
        }
        break;
    case 0x30:
    case 0x31:
        if (Len == 15)
        {
            if (nType == 0x30) {Info.Disp = "A数字出恢复";}
            else if (nType == 0x31) {Info.Disp = "B数字出恢复";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            strTemp = "";
            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {strTemp = "卸载";}
            else if (nByte == 1) {strTemp = "减压";}
            else if (nByte == 2) {strTemp = "关风";}
            else if (nByte == 3) {strTemp = "备用1";}
            else if (nByte == 4) {strTemp = "备用2";}
            else if (nByte == 5) {strTemp = "备用3";}
            else if (nByte == 6) {strTemp = "紧急";}

            if (strTemp != "") {Info.ShuoMing = QString("%1-%2").arg(nByte).arg(strTemp);}
            else {Info.ShuoMing.sprintf("保留%.02xH", nByte);}
        }
        break;
    case 0x34:
    case 0x35:
        if (Len == 15)
        {
            if (nType == 0x34) {Info.Disp = "A模块恢复";}
            else if (nType == 0x35) {Info.Disp = "B模块恢复";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            strTemp = "";
            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {strTemp = "信息处理板A";}
            else if (nByte == 1) {strTemp = "信息处理板B";}
            else if (nByte == 2) {strTemp = "通讯板A";}
            else if (nByte == 3) {strTemp = "通讯板B";}
            else if (nByte == 4) {strTemp = "一端显示器";}
            else if (nByte == 5) {strTemp = "二端显示器";}
            else if (nByte == 6) {strTemp = "黑匣子";}
            else if (nByte == 7) {strTemp = "串行机车信号";}
            else if (nByte == 8) {strTemp = "扩展通信板A";}
            else if (nByte == 9) {strTemp = "扩展通信板B";}
            else if (nByte == 10) {strTemp = "无线数传";}

            if (strTemp != "") {Info.ShuoMing = QString("%1-%2").arg(nByte).arg(strTemp);}
            else {Info.ShuoMing.sprintf("保留%.02xH", nByte);}
        }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_A8(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType, nByte;
    quint16 nWord;
    QString strTemp;
    QString QStr;
    //解析时间
    Info.Hms = GetTime(Buf, 2);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x05:
        if (Len == 10)
        {
            Info.Disp = "交路号"; //监控交路号
            Info.OTHER = QString::number(MoreBCD2INT(Buf, 5, 3));
            Info.nJKLineID = MoreBCD2INT(Buf, 5, 3);
        }
        break;
    case 0x18:
        if (Len == 8)
        {
            Info.Disp = "数据交路号";
            Info.OTHER = QString::number(BCD2INT(Buf[5]));
            //如果数据交路号发生改变  需要记录当前交路号和车站号
            m_nPriorDataLineNo = m_tPreviousInfo.nDataLineID;
            m_nPriorStationNo = m_tPreviousInfo.nStation;

            m_tPreviousInfo.nDataLineID = BCD2INT(Buf[5]);
        }
        break;
    case 0x09:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x17:
        if (Len == 9)
        {
            if (nType == 0x09) {Info.Disp = "客车";}
            else if (nType == 0x10) {Info.Disp = "重车";}
            else if (nType == 0x11) {Info.Disp = "空车";}
            else if (nType == 0x12) {Info.Disp = "非运用车";}
            else if (nType == 0x13) {Info.Disp = "代客车";}
            else if (nType == 0x14) {Info.Disp = "守车";}
            else if (nType == 0x15) {Info.Disp = "辆数";}
            else if (nType == 0x17) {Info.Disp = "调车批号";}

            Info.OTHER = QString::number(MoreBCD2INT(Buf, 5, 2));
        }
        break;
    case 0x06:
    case 0x27:
    case 0x29:
        if (Len == 10)
        {
            if (nType == 0x06) {Info.Disp = "车站号";}
            else if (nType == 0x27) {Info.Disp = "总重";}
            else if (nType == 0x29) {Info.Disp = "载重";}

            Info.OTHER = QString::number(MoreBCD2INT(Buf, 5, 3));

            if (Info.Disp == "车站号")
            {
                m_tPreviousInfo.nStation = MoreBCD2INT(Buf, 5, 3);
                Info.Xhj.sprintf("%d-%d", m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
            }
        }
        break;
    case 0x04:
    case 0x21:
        if (Len == 11)
        {
            if (nType == 0x21) {Info.Disp = "司机号";}
            else if (nType == 0x04) {Info.Disp = "副司机号";}

            Info.OTHER = QString::number(MoreBCD2INT(Buf, 5, 4));
        }
        break;
    case 0x16:
        if (Len == 9)
        {
            Info.Disp = "计长";
            Info.OTHER.sprintf("%0.1f", MoreBCD2INT(Buf, 5, 2) / 10.0);
        }
        break;
    case 0x20:
        if (Len == 16)
        {
            Info.Disp = "车次"; //如果是车次，拆分成两条记录
            strTemp = trim(&Buf[7], 4);
            Info.OTHER = strTemp + QString::number(MoreBCD2INT(Buf, 11, 3));
            //--------------------------------
            //nByte = Buf[5]; //客货类型[1]
            nByte = Buf[6]; //客货本补[1]
            if (nByte == 0) {Info.ShuoMing = "货本";}
            else if (nByte == 1) {Info.ShuoMing = "客本";}
            else if (nByte == 2) {Info.ShuoMing = "货补";}
            else if (nByte == 3) {Info.ShuoMing = "客补";}
        }
        break;
    case 0x24:
        if (Len == 8)
        {
            Info.Disp = "本补客货";

            nByte = Buf[5];
            if (nByte == 0) {Info.OTHER = "货本";}
            else if (nByte == 1) {Info.OTHER = "客本";}
            else if (nByte == 2) {Info.OTHER = "货补";}
            else if (nByte == 3) {Info.OTHER = "客补";}
        }
        break;
    case 0x43:
        if (Len == 10)
        {
            Info.Disp = "防撞辆数变化";
            Info.OTHER = "???";
        }
        break;
    case 0x22:
    case 0x23:
    case 0x90:
    case 0x91:
    case 0x94:
    case 0x95:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0x99:
        //if (Len >= 15)
    {
        if (nType == 0x22) {Info.Disp = "输入支线无效";}
        else if (nType == 0x23) {Info.Disp = "输入侧线无效";}
        else if (nType == 0x90) {Info.Disp = "上电装置号";}
        else if (nType == 0x91) {Info.Disp = "上电机车型号";}
        else if (nType == 0x94) {Info.Disp = "显示器1通信超时";}
        else if (nType == 0x95) {Info.Disp = "显示器1通信超时恢复";}
        else if (nType == 0x96) {Info.Disp = "显示器2通信超时";}
        else if (nType == 0x97) {Info.Disp = "显示器2通信超时恢复";}
        else if (nType == 0x98) {Info.Disp = "显示器1版本号变化";}
        else if (nType == 0x99) {Info.Disp = "显示器2版本号变化";}

        Info.OTHER = "???";
    }
        break;
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x28:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x34:
    case 0x36:
    case 0x37:
    case 0x38:
    case 0x39:
    case 0x40:
    case 0x42:
    case 0x45:
        //if (Len >= 15)
    {
        if (nType == 0x01) {Info.Disp = "日期修改";}
        else if (nType == 0x02) {Info.Disp = "时间修改";}
        else if (nType == 0x03) {Info.Disp = "轮径修改";}
        else if (nType == 0x28) {Info.Disp = "备用轮径修改";}
        else if (nType == 0x30) {Info.Disp = "机车号修改";}
        else if (nType == 0x31) {Info.Disp = "装置号修改";}
        else if (nType == 0x32) {Info.Disp = "机车型号修改";}
        else if (nType == 0x34) {Info.Disp = "默认辆数修改";}
        else if (nType == 0x36) {Info.Disp = "默认计长修改";}
        else if (nType == 0x37) {Info.Disp = "机车类型修改";}
        else if (nType == 0x38) {Info.Disp = "机车AB节修改";}
        else if (nType == 0x39) {Info.Disp = "柴机脉冲数修改";}
        else if (nType == 0x40) {Info.Disp = "速度表量程修改";}
        else if (nType == 0x42) {Info.Disp = "GPS校时";}
        else if (nType == 0x45) {Info.Disp = "默认总重修改";}

        Info.OTHER = "???";
    }
        break;
    case 0x54: //文档没有，分析补充
        if (Len == 9)
        {
            Info.Disp = "输入车站错误";
            Info.OTHER = QString::number(MoreBCD2INT(Buf, 5, 2)); //原始存储文件存储有问题，与0x06不统一
        }
        break;
    case 0x55: //文档没有，分析补充
        if (Len == 24)
        {
            Info.Disp = "输入最高限速";
            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.Speed = GetSpeed(Buf, 12);
            Info.S_lmt = GetLimitSpeed(Buf, 14);

            //色灯信号
            nWord = MoreBCD2INT(Buf, 16, 2);
            Info.Signal = GetLamp(nWord);
            Info.OTHER = GetSD(nWord);
            m_tPreviousInfo.strSpeedGrade = Info.OTHER;

            //???工况状态 原始文件没有相关字节，D:\上海运行记录文件\55302-10121.0629如何求得
            //Info.Hand = "???";
            Info.Hand = "";
            Info.ShuoMing = QString("原输入限速：%1    输入限速：%2").arg(MoreBCD2INT(Buf, 18, 2)).arg(MoreBCD2INT(Buf, 20, 2));
        }
        break;
    case 0x58: //文档没有，分析补充
        if (Len == 19)
        {
            Info.Disp = "IC卡验证码";

            nWord = MoreBCD2INT(Buf, 5, 2);
            if (nWord == 101) {Info.OTHER = "512K未加密IC卡";}
            else if (nWord == 102) {Info.OTHER = "2M未加密IC卡";}
            else if (nWord == 103) {Info.OTHER = "2M加密IC卡";}
            else if (nWord == 104) {Info.OTHER = "4M加密IC卡";}
            else if (nWord == 105) {Info.OTHER = "8M加密IC卡";}

            nWord = MoreBCD2INT(Buf, 7, 2);
            if (nWord == 1) {strTemp = " 株洲所";}
            else if (nWord == 2) {strTemp = "思维公司";}
            else {strTemp = "未知";}
            Info.ShuoMing = QString("生产厂家: %1;   生产日期：%2%3%4   生产序号：%5").arg(strTemp).arg(2000+BCD2INT(Buf[11])).arg(BCD2INT(Buf[12])).arg(BCD2INT(Buf[13])).arg(MoreBCD2INT(Buf, 14, 3));
        }
        break;
    case 0x59:
    {
        //A8 59   15 57 18   01 25 94 86 00   10 68   00 51 00 63 12 47
        //A8 59   15 57 22   01 25 95 39 00   10 15   00 51 00 63 12 10
        Info.Disp = "警惕报警开始";

        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);
    }
        break;
    case 0x60:
    {
        //A8 60   15 57 49   01 25 98 34 00 07 20 00 00 00 63 07 12 94
        Info.Disp = "警惕报警结束";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);
    }
        break;
    case 0x61:
    {
        //A8 61 14 39 13 20 59
    }
        break;
    case 0x62:
    {
        //A8 62  14 39 13  00 16 49 02 00 04 43  01 20 27
        Info.Disp = "警惕确认";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        //        Info.Speed = GetSpeed(Buf, 12);
        //        Info.S_lmt = GetLimitSpeed(Buf, 14);
    }
        break;
    case 0x87: //文档没有，分析补充
        if (Len == 55)
        {
            Info.Disp = "软件版本";
            Info.ShuoMing.sprintf("监控A程序版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[5]), BCD2INT(Buf[6]), BCD2INT(Buf[7]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("监控B程序版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[8]), BCD2INT(Buf[9]), BCD2INT(Buf[10]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("监控A数据版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[11]), BCD2INT(Buf[12]), BCD2INT(Buf[13]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("监控B数据版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[14]), BCD2INT(Buf[15]), BCD2INT(Buf[16]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("A机通信版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[17]), BCD2INT(Buf[18]), BCD2INT(Buf[19]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("B机通信版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[20]), BCD2INT(Buf[21]), BCD2INT(Buf[22]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("A机扩展通信版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[23]), BCD2INT(Buf[24]), BCD2INT(Buf[25]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("B机扩展通信版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[26]), BCD2INT(Buf[27]), BCD2INT(Buf[28]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("A机地面信息版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[29]), BCD2INT(Buf[30]), BCD2INT(Buf[31]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("B机地面信息版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[32]), BCD2INT(Buf[33]), BCD2INT(Buf[34]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("监控A参数版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[35]), BCD2INT(Buf[36]), BCD2INT(Buf[37]));
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("监控B参数版本：%.02d-%.02d-%.02d；", 2000+BCD2INT(Buf[38]), BCD2INT(Buf[39]), BCD2INT(Buf[40]));
        }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_B1(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType, nByte;
    quint16 nJSType;
    QString strTemp, strLine;
    RLKJJieShiInfo JieShiInfo;
    QDateTime dtTemp;
    QString QStr;
    //解析时间、公里标
    Info.Hms = GetTime(Buf, 2);
//    DEB("B1-----\n");

    clearRLKJJieShiInfo(JieShiInfo);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x01:
    case 0x10:
        if (Len == 48)
        {
            //B1 01 10 50 11 00 00 99 01 32 00 30 32 00 03 10 12 00 00 12 31 23 59 00 04 21 88 00 00 00 00 00 08 00 00 00 25 00 25 00 00 00 00 09 50 02 16 97
            //B1 01 10 50 11 00 01 99 01 32 00 30 32 00 03 10 12 00 00 12 31 23 59 00 04 21 88 00 00 00 00 00 09 00 00 00 25 00 25 00 00 00 00 09 50 02 16 99
            //B1 01 10 50 11 00 02 99 01 32 00 30 32 00 03 10 12 00 00 12 31 23 59 00 04 21 88 00 00 00 00 00 10 00 00 00 25 00 25 00 00 00 00 09 50 02 16 09
            //B1 01 10 50 11 00 03 99 01 32 00 30 32 00 03 10 12 00 00 12 31 23 59 00 04 21 88 00 00 00 00 00 12 00 00 00 25 00 25 00 00 00 00 09 50 02 16 06
            //B1 01 10 50 11 00 04 99 01 32 00 30 32 00 03 10 12 00 00 12 31 23 59 00 04 21 88 00 00 00 00 00 13 00 00 00 25 00 25 00 00 00 00 09 50 02 16 08
            if (nType == 0x01) {Info.Disp = "揭示输入";}
            else if (nType == 0x10) {Info.Disp = "揭示重新输入";}

            JieShiInfo.nCmdNo = MoreBCD2INT(Buf, 42, 4);

            Info.OTHER.sprintf("命令%d", JieShiInfo.nCmdNo);
            Info.ShuoMing.sprintf("序号：%d；", MoreBCD2INT(Buf, 5, 2));

            nJSType = MoreBCD2INT(Buf, 8, 2) & 0xFF; //揭示类型
            if (nJSType == 0) {strTemp = "";}
            else if (nJSType == 1) {strTemp = "临时限速";}
            else if (nJSType == 2) {strTemp = "站间停用每日";}      //非公里标，显示TMIS站名  //特殊揭示  //电话闭塞临时
            else if (nJSType == 3) {strTemp = "车站限速每日";}      //非公里标，显示TMIS站名
            else if (nJSType == 4) {strTemp = "侧线限速每日";}      //非公里标，显示TMIS站名
            else if (nJSType == 5) {strTemp = "乘降所限速每日";}
            else if (nJSType == 6) {strTemp = "绿色许可证每日";}    //非公里标，显示TMIS站名  //特殊揭示 //绿色凭证临时
            else if (nJSType == 7) {strTemp = "特定引导每日";}      //非公里标，显示TMIS站名  //特殊揭示
            else if (nJSType == 129) {strTemp = "昼夜限速";}
            else if (nJSType == 130) {strTemp = "站间停用昼夜";}    //非公里标，显示TMIS站名  //特殊揭示  //电话闭塞昼夜
            else if (nJSType == 131) {strTemp = "车站限速昼夜";}    //非公里标，显示TMIS站名
            else if (nJSType == 132) {strTemp = "侧线限速昼夜";}    //非公里标，显示TMIS站名
            else if (nJSType == 133) {strTemp = "乘降所限速昼夜";}
            else if (nJSType == 134) {strTemp = "绿色许可证昼夜";}  //非公里标，显示TMIS站名  //特殊揭示 //绿色凭证昼夜
            else if (nJSType == 135) {strTemp = "特定引导昼夜";}    //非公里标，显示TMIS站名  //特殊揭示
            else if (nJSType == 30) {strTemp = "施工揭示";}
            else if (nJSType == 31) {strTemp = "防汛揭示每日";}
            else if (nJSType == 32) {strTemp = "降弓提示揭示";}
            else if (nJSType == 159) {strTemp = "防汛揭示";}
            else {strTemp = QString::number(nJSType);}
            Info.ShuoMing = Info.ShuoMing + QString("揭示类型：%1；").arg(strTemp);

            JieShiInfo.strTypeName = strTemp;

            nByte = BCD2INT(Buf[14]); //上下行
            if (nByte == 1) {strTemp = "下行";}
            else if (nByte == 2) {strTemp = "上行";}
            else if (nByte == 3) {strTemp = "上下行";}
            else {strTemp = "未知上下行";}
            strLine = strTemp;
            JieShiInfo.strUpDown = strTemp;

            nByte = BCD2INT(Buf[13]) & 0x01; //主线/三线
            if (nByte == 0) {strTemp = "主线";}
            else if (nByte == 1) {strTemp = "三线";}
            else {strTemp = "";}
            strLine = strLine + "/" + strTemp;
            JieShiInfo.strLineType = strTemp;

            nByte = (BCD2INT(Buf[13]) / 10) & 0x01; //正向/反向
            if (nByte == 0) {strTemp = "正向";}
            else if (nByte == 1) {strTemp = "反向";}
            else {strTemp = "";}
            strLine = strLine + "/" + strTemp;
            JieShiInfo.strDirectType = strTemp;

            Info.ShuoMing = Info.ShuoMing +
                    QStr.sprintf("从%.02d-%.02d %.02d:%.02d始到%.02d-%.02d %.02d:%.02d止；",
                                 BCD2INT(Buf[15]), BCD2INT(Buf[16]), BCD2INT(Buf[17]),
                                 BCD2INT(Buf[18]), BCD2INT(Buf[19]), BCD2INT(Buf[20]),
                                 BCD2INT(Buf[21]), BCD2INT(Buf[22]));
            //原来使用的是1999年，会出现2月没有29号的问题。

            dtTemp = QDateTime::fromString("2000-01-01-00-00-00", "yyyy-MM-dd-hh-mm-ss");

            QString time = QString("2000-%1-%2-%3-%4-1").arg(BCD2INT(Buf[15])).arg(BCD2INT(Buf[16])).arg(BCD2INT(Buf[17])).arg(BCD2INT(Buf[18]));
            QDateTime dateTime = QDateTime::fromString(time, "yyyy-MM-dd-hh-mm-ss");
            if (dateTime.isValid()) {
                JieShiInfo.dtBegin = dateTime;
            } else {
                JieShiInfo.dtBegin = dtTemp;
            }
            time = QString("2000-%1-%2-%3-%4-0").arg(BCD2INT(Buf[19])).arg(BCD2INT(Buf[20])).arg(BCD2INT(Buf[21])).arg(BCD2INT(Buf[22]));
            dateTime = QDateTime::fromString(time, "yyyy-MM-dd-hh-mm-ss");

            if (dateTime.isValid()) {
                JieShiInfo.dtEnd = dateTime;
            } else {
                JieShiInfo.dtEnd = dtTemp;
            }

            //        JieShiInfo.dt} = EncodeDateTime(2000, BCD2INT(Buf[19]), BCD2INT(Buf[20]),
            //          BCD2INT(Buf[21]), BCD2INT(Buf[22]), 0, 0);

            JieShiInfo.nGWLineNo = MoreBCD2INT(Buf, 10, 3);
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("工务线号:%d；", JieShiInfo.nGWLineNo);

            Info.ShuoMing = Info.ShuoMing + strLine + "；";

            //非公里标，显示TMIS站名，别的显示起止公里标
            switch (nJSType)
            {
            case 2:
            case 3:
            case 4:
            case 6:
            case 7:
            case 130:
            case 131:
            case 132:
            case 134:
            case 135:
                Info.ShuoMing = Info.ShuoMing + QStr.sprintf("TIMS站号：%d；", MoreBCD2INT(Buf, 23, 4));
                Info.ShuoMing = Info.ShuoMing + QStr.sprintf("侧线股道号：%d；", MoreBCD2INT(Buf, 29, 4));
                break;
            default:
                JieShiInfo.nBeginGLB = MoreBCD2INT(Buf, 23, 4);
                JieShiInfo.nEndGLB =  MoreBCD2INT(Buf, 29, 4);

                Info.ShuoMing = Info.ShuoMing + QStr.sprintf("坐标范围:%.03fKm -- %.03fKm；",
                                                             MoreBCD2INT(Buf, 23, 4)/1000.0, MoreBCD2INT(Buf, 29, 4)/1000.0);
                nByte = BCD2INT(Buf[27]) & 0x07; //起始重复公里标序号
                if ((nByte == 0) || (3 == nByte) || (4 == nByte) || (7 == nByte)) {Info.ShuoMing = Info.ShuoMing + QStr.sprintf("起始重复公里标序号：%d；", nByte*4 + BCD2INT(Buf[28]));}
                else {Info.ShuoMing = Info.ShuoMing + QStr.sprintf("起始重复公里标序号：长链地段%d；", nByte*4 + BCD2INT(Buf[28]));}
                nByte = BCD2INT(Buf[33]) & 0x07; //结束重复标公里标序号
                if ((nByte == 0) || (3 == nByte) || (4 == nByte) || (7 == nByte)) {Info.ShuoMing = Info.ShuoMing + QStr.sprintf("结束重复标公里标序号：%d；", nByte*4 + BCD2INT(Buf[34]));}
                else {Info.ShuoMing = Info.ShuoMing + QStr.sprintf("结束重复标公里标序号：长链地段%d；", nByte*4 + BCD2INT(Buf[34]));}
                JieShiInfo.nLimitDistance = MoreBCD2INT(Buf, 39, 3);
                Info.ShuoMing = Info.ShuoMing + QStr.sprintf("限速长度：%dm；", MoreBCD2INT(Buf, 39, 3));
                break;
            }

            JieShiInfo.nKeCheLimitSpeed = MoreBCD2INT(Buf, 35, 2);
            JieShiInfo.nHuoCheLimitSpeed = MoreBCD2INT(Buf, 37, 2);
            Info.ShuoMing = Info.ShuoMing + QStr.sprintf("限速:%d(客)/%d(货)", MoreBCD2INT(Buf, 35, 2), MoreBCD2INT(Buf, 37, 2));
            JieShiInfo.dtLKJBegin = QDateTime::fromTime_t(0);
            JieShiInfo.dtLKJEnd = QDateTime::fromTime_t(0);
            AddJieShiInfoToLKJFile(JieShiInfo);
        }
        break;
    case 0x02:
    case 0x03:
    case 0x15:
    case 0x16:
        if (Len == 18)
        {
            if (nType == 0x02) {Info.Disp = "临时限速开始";}
            else if (nType == 0x03) {Info.Disp = "临时限速结束";}
            /*Info.Disp = "揭示结束"*/
            else if (nType == 0x15) {Info.Disp = "过揭示起点";}
            else if (nType == 0x16) {Info.Disp = "过揭示终点";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.OTHER.sprintf("命令%d", MoreBCD2INT(Buf, 12, 4));
            //处理揭示信息
            if (nType == 0x15)
            {
                SetJieShiInfoLKJDT(MoreBCD2INT(Buf, 12, 4),
                                   QDateTime(m_tPreviousInfo.dtEventDate, QTime::fromString(Info.Hms)), 1);
            }
            if (nType == 0x03) {
                SetJieShiInfoLKJDT(MoreBCD2INT(Buf, 12, 4),
                                   QDateTime(m_tPreviousInfo.dtEventDate, QTime::fromString(Info.Hms)), 2);
            }
        }
        break;
    case 0x11:
    case 0x12:
        if (Len == 16)
        {
            if (nType == 0x11) {Info.Disp = "揭示查询";}
            else if (nType == 0x12) {Info.Disp = "揭示更新";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.OTHER.sprintf("%d条", MoreBCD2INT(Buf, 12, 2));
        }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_B4(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType;
    //解析时间、公里标
    Info.Hms = GetTime(Buf, 2);
    Info.Glb = GetGLB(Buf, 5);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
        if (Len == 16)
        {
            if (nType == 0x01) {Info.Disp = "A机单机";}
            else if (nType == 0x02) {Info.Disp = "B机单机";}
            else if (nType == 0x03) {Info.Disp = "A主B备";}
            else if (nType == 0x04) {Info.Disp = "A备B主";}

            Info.Jl = GetJL(Buf, 9);
            Info.OTHER = m_tPreviousInfo.strSpeedGrade;
        }
        break;
    case 0x05:
    case 0x06:
        if (Len == 22)
        {
            if (nType == 0x05) {Info.Disp = "主机发送控制同步";}
            else if (nType == 0x06) {Info.Disp = "主机发送揭示同步";}

            Info.Jl = GetJL(Buf, 9);
            Info.OTHER = "???";
        }
        break;
    case 0x07:
        if (Len == 21)
        {
            Info.Disp = "主机发送按键";
            Info.Jl = GetJL(Buf, 9);
            Info.OTHER = "???";
        }
        break;
    case 0x08:
    case 0x09:
    case 0x10:
    case 0x11:
    case 0x12:
        if (Len == 18)
        {
            if (nType == 0x08) {Info.Disp = "主机发送校正";}
            else if (nType == 0x09) {Info.Disp = "发送揭示更新";}
            else if (nType == 0x10) {Info.Disp = "主机发送支线";}
            else if (nType == 0x11) {Info.Disp = "主机发送侧线";}
            else if (nType == 0x12) {Info.Disp = "对方制动";}

            Info.Jl = GetJL(Buf, 9);
            Info.OTHER = "???";
        }
        break;
    case 0x13:
        if (Len == 34)
        {
            Info.Disp = "制动原因";
            Info.Jl = GetJL(Buf, 9);
            Info.OTHER = "???";
        }
        break;
    case 0x14:
        if (Len == 15)
        {
            Info.Disp = "实际开关输出";
            Info.OTHER = "???";
        }
        break;
    case 0x57:
    {
        Info.Disp = "";
    }

    }
}

void QAnalysisOrgFile::MakeOneLkjRec_B6(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType, nByte;
    int intTemp;
    //解析时间
    Info.Hms = GetTime(Buf, 2);
    Info.Glb = GetGLB(Buf, 5);
    Info.Jl = GetJL(Buf, 9);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x33:
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
        if (Len == 14)
        {
            if (nType == 0x33) {Info.Disp = "EMU通讯中断";}
            else if (nType == 0x40) {Info.Disp = "ATP控制结束";}
            else if (nType == 0x41) {Info.Disp = "ATP控制开始";}
            else if (nType == 0x42) {Info.Disp = "监控隔离位";}
            else if (nType == 0x43) {Info.Disp = "监控正常位";}
        }
        break;
    case 0x35:
    case 0x36:
    case 0x37:
        if (Len == 20)
        {
            if (nType == 0x35) {Info.Disp = "ATP速度变化";}
            else if (nType == 0x36) {Info.Disp = "ATP限速变化";}
            else if (nType == 0x37) {Info.Disp = "ATP目标限速变化";}

            Info.Speed = GetSpeed(Buf, 12);
            Info.S_lmt = GetLimitSpeed(Buf, 14);
            intTemp = MoreBCD2INT(Buf, 16, 2);
            Info.ShuoMing.sprintf("ATP速度：%d ATP限速：%d 目标限速:%d", Info.Speed, Info.S_lmt, intTemp);
        }
        break;
    case 0x38:
        if (Len == 15)
        {
            Info.Disp = "ATP等级变化";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.ShuoMing = "控制等级：CTCS0/1";}
            else if (nByte == 1) {Info.ShuoMing = "控制等级：01(备用)";}
            else if (nByte == 2) {Info.ShuoMing = "控制等级：CTCS2";}
            else if (nByte == 3) {Info.ShuoMing = "控制等级：CTCS3";}
            else if (nByte == 4) {Info.ShuoMing = "控制等级：CTCS4";}
            else if (nByte == 5) {Info.ShuoMing = "控制等级：05(保留)";}
            else if (nByte == 6) {Info.ShuoMing = "控制等级：06(备用)";}
            else if (nByte == 7) {Info.ShuoMing = "控制等级：07(未知)";}
            else {Info.ShuoMing = "";}
        }
        break;
    case 0x39:
        if (Len == 15)
        {
            Info.Disp = "ATP模式变化";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.ShuoMing = "控制模式：00(未知)";}
            else if (nByte == 1) {Info.ShuoMing = "控制模式：FS";}
            else if (nByte == 2) {Info.ShuoMing = "控制模式：PS";}
            else if (nByte == 3) {Info.ShuoMing = "控制模式：IS";}
            else if (nByte == 4) {Info.ShuoMing = "控制模式：OS";}
            else if (nByte == 5) {Info.ShuoMing = "控制模式：SH";}
            else if (nByte == 6) {Info.ShuoMing = "控制模式：SB";}
            else if (nByte == 7) {Info.ShuoMing = "控制模式：CS";}
            else if (nByte == 8) {Info.ShuoMing = "控制模式：RO";}
            else if (nByte == 9) {Info.ShuoMing = "控制模式：CO";}
            else if (nByte == 10) {Info.ShuoMing = "控制模式：BF";}
            else if ((nByte >= 11) && (nByte <= 15)) {Info.ShuoMing.sprintf("%d(无定义)", nByte);}
            else {Info.ShuoMing = "";}
        }
        break;
    case 0x50:
        if (Len == 15)
        {
            Info.Disp = "ATP传输状态";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.OTHER = "INIT";}
            else if (nByte == 1) {Info.OTHER = "FFFE_ERR";}
            else if (nByte == 2) {Info.OTHER = "CRC_ERROR";}
            else if (nByte == 3) {Info.OTHER = "DYN";}
            else if (nByte == 4) {Info.OTHER = "MSG_LOSS";}
            else if (nByte == 5) {Info.OTHER = "DELAY";}
            else if (nByte == 6) {Info.OTHER = "OK";}
            else if (nByte == 7) {Info.OTHER = "LOCK";}
            else {Info.OTHER = "备用";}
        }
        break;
    case 0x51:
        if (Len == 25)
        {
            Info.Disp = "ATP应答器信息";
        }
        break;
    case 0x52:
        if (Len == 18)
        {
            Info.Disp = "ATP轨道回路编号";
            Info.ShuoMing.sprintf("ATP速度等级：%d；当前轨道回路编号：%d", BCD2INT(Buf[13]), MoreBCD2INT(Buf, 14, 2));
        }
        break;
    case 0x54:
        if (Len == 15)
        {
            Info.Disp = "ATP司机操作";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.OTHER = "备用";}
            else if (nByte == 5) {Info.OTHER = "下行指定";}
            else if (nByte == 6) {Info.OTHER = "上行指定";}
            else if (nByte == 7) {Info.OTHER = "CTCS2->CTCS0切换";}
            else if (nByte == 8) {Info.OTHER = "CTCS0->CTCS2切换";}
            else if (nByte == 12) {Info.OTHER = "起动";}
            else if (nByte == 13) {Info.OTHER = "目视";}
            else if (nByte == 14) {Info.OTHER = "调车";}
            else if (nByte == 15) {Info.OTHER = "缓解";}
            else if (nByte == 16) {Info.OTHER = "预警";}
            else {Info.OTHER = "备用";}
        }
        break;
    case 0x55:
        if (Len == 25)
        {
            Info.Disp = "已过应答器";
        }
        break;
    case 0x56:
        if (Len == 15)
        {
            Info.Disp = "ATP紧急变化";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.OTHER = "非动作";}
            else if (nByte == 1) {Info.OTHER = "动作";}
        }
        break;
    case 0x57:
        if (Len == 15)
        {
            Info.Disp = "ATP常用变化";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.OTHER = "常用未动作";}
            else if (nByte == 1) {Info.OTHER = "常用1动作";}
            else if (nByte == 2) {Info.OTHER = "备用";}
            else if (nByte == 3) {Info.OTHER = "备用";}
            else if (nByte == 4) {Info.OTHER = "常用4动作";}
            else if (nByte == 5) {Info.OTHER = "备用";}
            else if (nByte == 6) {Info.OTHER = "常用7动作";}
            else if (nByte == 7) {Info.OTHER = "备用";}
        }
        break;
    case 0x58:
        if (Len == 15)
        {
            Info.Disp = "ATP卸载变化";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.OTHER = "非动作";}
            else if (nByte == 1) {Info.OTHER = "卸载";}
        }
        break;
    case 0x59:
        if (Len == 17)
        {
            Info.Disp = "ATP报警状态";
        }
        break;
    case 0x60:
        if (Len == 18)
        {
            Info.Disp = "ATP目标距离";
        }
        break;
    case 0x64:
        if (Len == 15)
        {
            Info.Disp = "ATP地面故障";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.OTHER = "列车信号正常";}
            else if (nByte == 1) {Info.OTHER = "列车信号故障";}
            else if ((nByte >= 2) && (nByte <= 15)) {Info.OTHER = "备用";}
            else {Info.OTHER = "";}
        }
        break;
    case 0x69:
        if (Len == 15)
        {
            Info.Disp = "ATP机车信号";
        }
        break;
    case 0x79:
        if (Len == 15)
        {
            Info.Disp = "隔离开关状态";

            nByte = BCD2INT(Buf[12]);
            if (nByte == 0) {Info.OTHER = "正常位";}
            else if (nByte == 1) {Info.OTHER = "隔离位";}
        }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_B7(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType;
    //解析时间
    Info.Hms = GetTime(Buf, 2);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x01:
    {
        //B7 01 22 25 45 00 11 54 42 00 03 97 01 94 22 70
        //tdw 增加
        //Info.Disp = "降级运行";
        //Info.Glb = GetGLB(Buf, 5);
        //Info.Jl = GetJL(Buf, 9);
    }
        break;
    case 0x04:
        if (Len == 16)
        {
            Info.Disp = "过分相";
            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.OTHER = m_tPreviousInfo.strSpeedGrade;
        }
        break;
    case 0x06:
    case 0x07:
        if (Len == 14)
        {
            if (nType == 0x06) {Info.Disp = "调用反向数据";}
            else if (nType == 0x07) {Info.Disp = "退出反向数据";}
            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.OTHER = m_tPreviousInfo.strSpeedGrade;
        }
        break;
    case 0x08:
        //tdw 注释
        if (Len == 9)
        {
            Info.Disp = "进入降级";

            Info.Glb = m_tPreviousInfo.Glb;
            Info.Xhj = m_tPreviousInfo.Xhj;
            Info.Xht_code = m_tPreviousInfo.Xht_code;
            Info.Xhj_no = m_tPreviousInfo.Xhj_no;
            Info.Xh_code = m_tPreviousInfo.Xh_code;
            Info.Speed = m_tPreviousInfo.Speed;
            Info.Shoub = m_tPreviousInfo.Shoub;
            Info.Hand = m_tPreviousInfo.Hand;
            Info.Gya = m_tPreviousInfo.Gya;
            Info.Rota = m_tPreviousInfo.Rota;
            Info.S_lmt = m_tPreviousInfo.S_lmt;
            Info.Jl = m_tPreviousInfo.Jl;
            Info.Gangy = m_tPreviousInfo.Gangy;
            Info.Signal = m_tPreviousInfo.Signal;
            Info.Jg1 = m_tPreviousInfo.Jg1;
            Info.Jg2 = m_tPreviousInfo.Jg2;
            Info.JKZT = m_tPreviousInfo.JKZT;
        }
        break;
    case 0x12:
    {
        //鸣笛开始
        //B7 12  20 15 46  00 01 31 47 00  08 94  02  20 11
        //B7 12  22 25 45  00 11 54 42 00  03 97  01  22 93
        //B7 12  18 46 19  00 00 50 45 00  00 00  02  16 23
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);

        if (BCD2INT(Buf[12]) == 0x01) {
            Info.Disp = "I端鸣笛开始";}
        else if (BCD2INT(Buf[12]) == 0x02) {
            Info.Disp = "II端鸣笛开始";}
    }
        break;
    case 0x13:
    {
        //鸣笛结束
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);

        if (BCD2INT(Buf[12]) == 0x01) {
            Info.Disp = "I端鸣笛结束";}
        else if (BCD2INT(Buf[12]) == 0x02) {
            Info.Disp = "II端鸣笛结束";}
    }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_B8(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType, nByte;
    quint16 nWord;
    QString strTemp;
    //解析时间
    Info.Hms = GetTime(Buf, 2);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x01:
        if (Len == 22)
        {
            Info.Disp = "开车对标";
            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            //前方机编号[3] 前方机类型[1]
            Info.Xhj_no = MoreBCD2INT(Buf, 12, 3);
            Info.Xht_code = BCD2INT(Buf[15]);
            //Info.Xhj.sprintf("%d-%d", [m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation]);
            Info.Xhj = m_CZBMAnalysis->GetStationName(m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
            Info.OTHER = m_tPreviousInfo.strSpeedGrade;
        }
        break;
    case 0x02:
    case 0x03:
    case 0x04:
        if (Len == 21)
        {
            if (nType == 0x02) {Info.Disp = "车位向前";}
            else if (nType == 0x03) {Info.Disp = "车位向后";}
            else if (nType == 0x04) {Info.Disp = "车位对中";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            //前方机编号[3] 前方机类型[1]
            strTemp = "";
            nByte = BCD2INT(Buf[15]);
            if (nByte == 1) {strTemp = "进出站";}
            if (nByte == 2) {strTemp = "出站";}
            if (nByte == 3) {strTemp = "进站";}
            if (nByte == 4) {strTemp = "通过";}
            if (nByte == 5) {strTemp = "预告";}
            if (nByte == 6) {strTemp = "容许";}
            if (nByte == 7) {strTemp = "分割";}
            Info.Xhj_no = MoreBCD2INT(Buf, 12, 3) % 100000;
            Info.Xht_code = nByte;
            Info.Xhj = QString("%1%2").arg(strTemp).arg(Info.Xhj_no);

            Info.OTHER.sprintf("调整距离：%d", Info.Jl);
        }
        break;
    case 0x05:
    {
        Info.Disp = "解锁键";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);
        //B8 05 23 00 54 01 04 63 28 00 01 85 00 00 00 50 20 96
    }
        break;
    case 0x06:
    {
        Info.Disp = "确认键";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);
        //B8 06 23 00 55 01 04 63 28 00 01 85 00 00 00 50 20 90 //确认键
    }
        break;
    case 0x08:
    {
        switch (MoreBCD2INT(Buf, 18, 1))
        {
        case 1: Info.Disp = "路票解锁";break;
        case  3: Info.Disp = "临时路票解锁"; break;
        case  5: Info.Disp = "绿证解锁";break;
        case  6: Info.Disp = "临时绿证解锁";break;
        case  8: Info.Disp = "股道无码通过确认";break;
        case  9: Info.Disp = "股道无码开车确认";break;
        case  10: Info.Disp = "双频点式解锁";break;
        case  11: Info.Disp = "特殊发码确认";break;
        case  12: Info.Disp = "大秦特殊发码确认";break;
        }
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);

        nWord = MoreBCD2INT(Buf, 16, 2);
        Info.Signal = GetLamp(nWord);
        Info.OTHER = GetSD(nWord);
    }
        break;
    case 0x10:
    case 0x11:
        if (Len == 22)
        {
            if (nType == 0x10) {Info.Disp = "支线选择";}
            else if (nType == 0x11) {Info.Disp = "侧线选择";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.Speed = GetSpeed(Buf, 12);
            Info.S_lmt = GetLimitSpeed(Buf, 14);

            //色灯信号
            nWord = MoreBCD2INT(Buf, 16, 2);
            Info.Signal = GetLamp(nWord);
            Info.OTHER = GetSD(nWord);
            m_tPreviousInfo.strSpeedGrade = Info.OTHER;

            Info.OTHER = QString::number(MoreBCD2INT(Buf, 18, 2));
        }
        break;
    case 0x14:
    case 0x15:
    case 0x18:
        if (Len == 20)
        {
            if (nType == 0x14) {Info.Disp = "进入调车";}
            else if (nType == 0x15) {Info.Disp = "退出调车";}
            else if (nType == 0x18) {
                Info.Disp = "警惕键";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.Speed = GetSpeed(Buf, 12);
            Info.S_lmt = GetLimitSpeed(Buf, 14);

            //色灯信号
            nWord = MoreBCD2INT(Buf, 16, 2);
            Info.Signal = GetLamp(nWord);


            Info.OTHER = GetSD(nWord);
            m_tPreviousInfo.strSpeedGrade = Info.OTHER;
        }
        break;
    case 0x19:
    case 0x20:
    case 0x21:
        if (Len == 18)
        {
            if (nType == 0x19) {Info.Disp = "前端巡检1";}
            else if (nType == 0x20) {Info.Disp = "后端巡检";}
            else if (nType == 0x21) {Info.Disp = "前端巡检2";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);

            //前方机编号[3] 前方机类型[1]
            strTemp = "";
            nByte = BCD2INT(Buf[15]);
            if (nByte == 1) {strTemp = "进出站";}
            if (nByte == 2) {strTemp = "出站";}
            if (nByte == 3) {strTemp = "进站";}
            if (nByte == 4) {strTemp = "通过";}
            if (nByte == 5) {strTemp = "预告";}
            if (nByte == 6) {strTemp = "容许";}
            if (nByte == 7) {strTemp = "分割";}
            Info.Xhj_no = MoreBCD2INT(Buf, 12, 3) % 100000;
            Info.Xht_code = nByte;
            Info.Xhj = QString("%1%2").arg(strTemp).arg(Info.Xhj_no);
        }
        break;
    case 0x16:
    case 0x17:
    case 0x39:
    case 0x40:
        if (Len == 11)
        {
            if (nType == 0x16) {Info.Disp = "出段";}
            else if (nType == 0x17) {Info.Disp = "入段";}
            else if (nType == 0x39) {Info.Disp = "退出出段";}
            else if (nType == 0x40) {Info.Disp = "退出入段";}

            Info.Speed = GetSpeed(Buf, 5);
            Info.S_lmt = GetLimitSpeed(Buf, 7);
            Info.OTHER = m_tPreviousInfo.strSpeedGrade;
        }
        break;
    case 0x27:
        if (Len == 18)
        {
            Info.Disp = "定标键";
            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
            Info.Speed = GetSpeed(Buf, 12);
            Info.S_lmt = GetLimitSpeed(Buf, 14);
            Info.OTHER = m_tPreviousInfo.strSpeedGrade;
        }
        break;
    case 0x34:
    case 0x35:
    case 0x41:
        if (Len == 14)
        {
            if (nType == 0x34)
            {
                Info.Disp = "IC卡插入";
                Info.OTHER = m_tPreviousInfo.strSpeedGrade;
            }
            else if (nType == 0x35) {Info.Disp = "IC卡拔出";}
            else if (nType == 0x41) {Info.Disp = "参数确认";}

            Info.Glb = GetGLB(Buf, 5);
            Info.Jl = GetJL(Buf, 9);
        }
        break;
    case 0x46:
    {
        Info.Disp = "凭证号输入";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);
        Info.OTHER = QString::number(MoreBCD2INT(Buf, 20, 1) % 100000) ;
    }
        break;
    case 0x47:
    {
        Info.Disp = "临时命令号输入";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);

        Info.OTHER.sprintf("凭证类型:%s 命令号:%d",
                           GetPZTypeName(Buf, 18).toStdString().c_str(), MoreBCD2INT(Buf, 19, 4));

        //B8 47 23 00 39 01 04 63 28 00 01 85 00 00 00 50 10 56 01 00 07 19 90 20 42
    }
        break;
    case 0x48:
    {
        //B8 48 23 19 22 00 43 15 28 00 00 58 00 00 00 48 10 40 20 00
        Info.Disp = "绿/绿黄确认";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);

        Info.OTHER = m_tPreviousInfo.strSpeedGrade;

        //B8 47 23 00 39 01 04 63 28 00 01 85 00 00 00 50 10 56 01 00 07 19 90 20 42
    }
        break;
    case 0x56:
    {
        //B8 56  15 35 43  00 00 00 00  00 08 00  00 00  00 85  00 00 16 32
        //B8 56  22 40 47  00 00 00 00  00 20 00  00 00  01 42  00 16 16 42
        //B8 56  14 32 52  00 44 41 40  00 06 38  00 00  00 21  11 52 12 71
        Info.Disp = "转入20km/h限速";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);
    }
        break;
    case 0x58:
    {
        //tdw增加
        Info.Disp = "特殊前行";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);
        //Info.OTHER = QString::number(BCD2INT(Buf[20]));
        //Info.Rota = MoreBCD2INT(Buf, 12, 2);
    }
        break;
    case 0x26:
    {
        //tdw增加
        //B8 26  18 13 24  00 42 61 85  00 08 00 00 00 00 85 00 03 72 12 00 05 00 95 16 77
        //命令序号:37212解锁命令号:50095
        Info.Disp = "取消揭示";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        Info.Speed = GetSpeed(Buf, 12);
        Info.S_lmt = GetLimitSpeed(Buf, 14);
        //Info.OTHER = QString::number(BCD2INT(Buf[20]));
        //Info.Rota = MoreBCD2INT(Buf, 12, 2);
    }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_B9(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    uchar nType;
    Info.Hms = GetTime(Buf, 2);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x06:
    {
        //B9 06   15 55 02   01 25 76 60 00   16 85  12 08
        Info.Disp = "空转报警开始";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        break;
    }
    case 0x07:
    {
        //B9 07   15 55 16   01 25 76 76 00   16 69   12 85
        Info.Disp = "空转报警结束";
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
        break;
    }
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_BE(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType;
    //解析时间
    Info.Hms = GetTime(Buf, 2);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x02:
        if (Len == 13)
        {
            Info.Disp = "各通道速度";
            Info.OTHER.sprintf("v0=%d,v1=%d,v2=%d", MoreBCD2INT(Buf, 5, 2), MoreBCD2INT(Buf, 7, 2), MoreBCD2INT(Buf, 9, 2));
            Info.ShuoMing = Info.OTHER;
        }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_BA(RCommonInfo &Info, char *Buf, int Len)
{
    //  nByte: Integer;
    QString strTemp;
    switch (Buf[1])
    {
    case 0x02:
    {
        if (Len != 14)
        {
            return;
        }
        //解析赋值
        m_tPreviousInfo.nDataLineID = BCD2INT(Buf[7]);
        m_tPreviousInfo.nStation = MoreBCD2INT(Buf, 8, 2);
        Info.OTHER.sprintf("%d %d-%d", m_tPreviousInfo.nStation, m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);

        //特别处理进站信号机
        if (Info.Disp == "进站")
        {
            //    Info.Xhj.sprintf("%d-%d", [m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation]);
            Info.Xhj = m_CZBMAnalysis->GetStationName(m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
            m_tPreviousInfo.Xhj.sprintf("%d-%d", m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
        }
        if (Info.Disp == "进出站")
        {
            Info.Xhj.sprintf("%d-%d", m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
            //    Info.Xhj = m_CZBMAnalysis->GetStationName(m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
            //    m_tPreviousInfo.Xhj.sprintf("%d-%d", [m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation]);
        }
        if (Info.Disp == "出站")
        {
            Info.Xhj = m_CZBMAnalysis->GetStationName(m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
            // m_tPreviousInfo.Xhj.sprintf("%d-%d", [m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation]);
        }
    }
        break;
    case 0x35:
    {
        Info.Disp = "调监系统配置";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    case 0x60:
    {
        Info.Disp = "进入调车监控";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    case 0x61:
    {
        Info.Disp = "退出调车监控";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    case 0x63:
    {
        Info.Disp = "进入非集中区";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    case 0x66:
    {
        //      BA 66 07 26 28  00 21 97 25  00 24 00 09 00 03 00 00 24 00 08 63 00 33 00 06 55 35 00 01 00 25 11 73
        //      BA 66 07 26 28  00 21 97 25  01 24 00 01 00 03 00 00 22 00 01 97 00 43 00 00 00 24 00 01 00 00 11 70

        //      BA 66 07 31 02  00 22 05 78  00 16 00 09 00 03 00 00 24 00 00 04 00 33 00 06 55 35 00 01 00 00 11 64   开放
        //      BA 66 07 31 02  00 22 05 78  01 16 00 01 00 03 00 00 22 00 01 97 00 43 00 00 00 24 00 01 00 00 11 93
        //      BA 66 07 31 02  00 22 05 78  02 18 00 00 00 03 00 00 36 00 13 41 00 43 00 00 00 22 00 01 00 00 11 55

        //BA 66 07 31 07  00 22 05 96  00 24 00 09 00 03 00 00 22 00 01 97 00 33 00 06 55 35 00 01 00 25 11 72   开放

        //      BA 66 06 19 12  00 21 81 93  00 08 00 09 00 03 00 00 23 00 00 43 00 43 00 06 55 35 00 01 00 25 11 68

        //BA 66 07 31 08  00 22 05 97  01 26 00 00 00 03 00 00 36 00 13 41 00 43 00 00 00 22 00 01 00 00 11 95

        //BA 66 07 35 02  00 22 06 51  00 18 00 08 00 03 00 00 22 00 01 36 00 33 00 06 55 35 00 01 00 00 11 82    关闭
        //      BA 66 19 25 51  00 21 26 05  00 18 02 00 00 03 03 37 92 00 08 85 00 18 00 06 55 35 00 01 00 00 11 37
        //BA 66 07 35 02  00 22 06 51  00 18 00 08 00 03 00 00 22 00 01 36 00 33 00 06 55 35 00 01 00 00 11 82    关闭
        //      BA 66 19 26 24  00 21 26 05  00 18 02 00 00 03 03 37 92 00 08 85 00 18 00 06 55 35 00 01 00 00 11 67
        //BA 66 06 22 36  00 21 91 69  00 10 00 24 00 03 03 68 64 00 02 71 00 33 00 06 55 35 00 01 00 25 11 18
        //      BA 66 06 29 13  00 21 91 81  00 02 00 24 00 03 03 68 64 00 32 58 00 33 00 06 55 35 00 01 00 00 11 54
        //      BA 66 19 15 46  00 21 26 05  00 18 02 00 00 03 03 37 92 00 08 85 00 18 00 06 55 35 00 01 00 00 11 88

        Info.Disp = "进路信息更新";
        Info.Hms = GetTime(Buf, 2);
        Info.Glb = GetGLB(Buf, 5);
        if (Buf[12] == 0x9) {
            Info.OTHER = "开放";}
        else if ((Buf[12] == 0x8) || (Buf[12] == 0x24)) {
            Info.OTHER = "关闭";}
    }
        break;
    case 0x67:
    {
        Info.Disp = "存车信息";
        Info.Hms = GetTime(Buf, 2);
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl =  GetJL(Buf, 9);
    }
        break;
    case 0x70:
    {
        Info.Disp = "前对上下行";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    case 0x71:
    {
        Info.Disp = "站号场号变换";
        Info.Hms = GetTime(Buf, 2);
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl =  GetJL(Buf, 9);
    }
        break;
    case 0x72:
    {
        Info.Disp = "区段号变换";
        Info.Hms = GetTime(Buf, 2);
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl =  GetJL(Buf, 9);
        //信号机
        //      nByte = BCD2INT(Buf[12]);
        //      if (nByte == 0) {strTemp = "预告";
        strTemp = "预告";
        Info.Xhj_no = MoreBCD2INT(Buf, 12, 3);
        Info.Xht_code = 0;
        Info.Xhj = QString("%1%2").arg(strTemp).arg(Info.Xhj_no);
        // BA 72 10 05 20 00 00 02 34 00 00 00 03 37 92 01 75 19 98
        Info.OTHER = "区段号:" + QString::number(MoreBCD2INT(Buf, 15, 2));
    }
        break;
    case 0x75:
    {
        Info.Disp = "申请作业单";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    case 0x76:
    {
        //BA 76  10 51 47  00 00 38 39  00 01 27  00 01 58 00 20 00 00  19 13
        Info.Disp = "辆数变化";
        Info.Hms = GetTime(Buf, 2);
        Info.Glb = GetGLB(Buf, 5);
        Info.Jl = GetJL(Buf, 9);
    }
        break;
    case 0x77:
    {
        Info.Disp = "接风管状态变换";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    case 0x79:
    {
        Info.Disp = "调监设备故障";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    case 0x86:
    {
        Info.Disp = "站场图信息";
        Info.Hms = GetTime(Buf, 2);
    }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_C0(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "关机";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_C1(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "开机";
    Info.Hms = GetTime(Buf, 1);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;

    //开机日期
    m_tPreviousInfo.dtEventDate = QDate(2000 + BCD2INT(Buf[4]), BCD2INT(Buf[5]), BCD2INT(Buf[6]));
}

void QAnalysisOrgFile::MakeOneLkjRec_C2(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "公里标突变";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_C3(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "座标增";
    Info.Hms = GetTime(Buf, 1);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_C4(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "座标减";
    Info.Hms = GetTime(Buf, 1);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_C5(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "过机不校";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_C6(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    int intTemp;
    //解析赋值
    Info.Disp = "过机校正";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);

    intTemp = MoreBCD2INT(Buf, 11, 2);
    if (intTemp > 999)
    {
        intTemp = -(intTemp % 1000);
    }
    Info.OTHER.sprintf("%d", intTemp);
    Info.ShuoMing.sprintf("轮径值：%.01f", MoreBCD2INT(Buf, 14, 3) / 10.0);
}

void QAnalysisOrgFile::MakeOneLkjRec_C7(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "过站中心";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
    if ((m_nPriorDataLineNo != 0) && (m_tPreviousInfo.nStation == m_nPriorStationNo))
    {
        Info.Xhj = m_CZBMAnalysis->GetStationName(m_nPriorDataLineNo, m_nPriorStationNo);
        m_nPriorStationNo = 0;
        m_nPriorDataLineNo = 0;
    } else {
        Info.Xhj = m_CZBMAnalysis->GetStationName(m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_C8(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    //解析赋值
    Info.Disp = "报警开始";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;
}

void QAnalysisOrgFile::MakeOneLkjRec_C9(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    //解析赋值
    Info.Disp = "报警结束";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;
}

void QAnalysisOrgFile::MakeOneLkjRec_CA(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    //解析赋值
    Info.Disp = "手柄防溜报警开始";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;
}

void QAnalysisOrgFile::MakeOneLkjRec_CB(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    //解析赋值
    Info.Disp = "手柄防溜报警结束";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;
}

void QAnalysisOrgFile::MakeOneLkjRec_CC(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    uchar nByte;
    //解析赋值
    Info.Disp = "道岔";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;

    nByte = BCD2INT(Buf[15]);
    if (nByte == 1) { Info.Disp = "进站道岔";}
    else if (nByte == 2) { Info.Disp = "出站道岔";}
}

void QAnalysisOrgFile::MakeOneLkjRec_CD(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "日期变化";
    Info.Hms = GetTime(Buf, 1);

    //日期
    m_tPreviousInfo.dtEventDate = QDate(2000 + BCD2INT(Buf[4]), BCD2INT(Buf[5]), BCD2INT(Buf[6]));
    Info.OTHER = m_tPreviousInfo.dtEventDate.toString("yy-MM-dd");
}

void QAnalysisOrgFile::MakeOneLkjRec_CE(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    uchar nByte;
    quint16 nWord;
    QString strTemp;
    //解析赋值
    Info.Disp = "过信号机";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    if (Info.Speed == 0)
    {
        DEB("speed is zero!\n");
    }
    //已过机编号[3]注48 已过机类型[1]注17
    strTemp = "";
    nByte = BCD2INT(Buf[18]);
    if (nByte == 1) { strTemp = "进出站";}
    else if (nByte == 2) { strTemp = "出站";}
    else if (nByte == 3) { strTemp = "进站";}
    else if (nByte == 4) { strTemp = "通过";}
    else if (nByte == 5) { strTemp = "预告";}
    else if (nByte == 6) { strTemp = "容许";}
    else if (nByte == 7) { strTemp = "分割";}
    if ((0 < nByte) && (nByte < 4)) { Info.Disp = strTemp;}
    Info.Xhj_no = MoreBCD2INT(Buf, 15, 3) % 100000;
    Info.Xht_code = nByte;
    Info.Xhj = QString("%1%2").arg(strTemp).arg(Info.Xhj_no);

    //前方机编号[3] 前方机类型[1]注17
    strTemp = "";
    nByte = BCD2INT(Buf[22]);
    if (nByte == 1) { strTemp = "进出站";}
    else if (nByte == 2) { strTemp = "出站";}
    else if (nByte == 3) { strTemp = "进站";}
    else if (nByte == 4) { strTemp = "通过";}
    else if (nByte == 5) { strTemp = "预告";}
    else if (nByte == 6) { strTemp = "容许";}
    else if (nByte == 7) { strTemp = "分割";}
    m_tPreviousInfo.Xhj_no = MoreBCD2INT(Buf, 19, 3) % 100000;
    m_tPreviousInfo.Xht_code = nByte;
    m_tPreviousInfo.Xhj = QString("%1%2").arg(strTemp).arg(m_tPreviousInfo.Xhj_no);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 23, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;

    //自闭类型
    if (Info.Disp == "过信号机")
    {
        nByte = BCD2INT(Buf[25]);
        if (nByte == 0) { Info.ShuoMing = "自闭类型：自闭";}
        else if (nByte == 1) { Info.ShuoMing = "自闭类型：半自闭";}
    }

    //特别处理进站信号机
    if (Info.Disp == "进站")
    {
        //Info.Xhj.sprintf("%d-%d", [m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation]);
        Info.Xhj = m_CZBMAnalysis->GetStationName(m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
        m_tPreviousInfo.Xhj.sprintf("%d-%d", m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
    }
    if (Info.Disp == "出站")
    {
        Info.Xhj = m_CZBMAnalysis->GetStationName(m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
    }
    if (Info.Disp == "进出站")
    {
        Info.Xhj.sprintf("%d-%d", m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_CF(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "正线终止";
    Info.Hms = GetTime(Buf, 1);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;

    //信号机
    Info.Xht_code = m_tPreviousInfo.Xht_code;
    Info.Xhj_no = m_tPreviousInfo.Xhj_no;
    Info.Xhj.sprintf("出站%d", Info.Xhj_no);
}

void QAnalysisOrgFile::MakeOneLkjRec_D0(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    uchar nByte;
    QString strTemp;
    //解析赋值

    Info.Disp = "停车";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    //  if Info.Hms == "21:33:19")
    //    OutputDebugString("dd");
    //前方机编号[3] 前方机类型[1]
    Info.Xhj_no = MoreBCD2INT(Buf, 11, 3);
    Info.Xht_code = BCD2INT(Buf[14]);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;

    //开车  4 20 60=站内   10 30 50 2 6 10 12 14 18 22=调车     40 00 08 16 =降级
    strTemp = "";
    //D0 10 22 31 00 17 51 21 00 11 41 00 17 38 03 10 25 00 00 04 60 20 21
    //D0 21 33 19 00 23 04 85 00 19 58 00 23 24 05 11 52 00 00 04 60 20 01
    //D0 17 05 20 01 02 90 97 00 02 23 00 00 82 02 10 56 05 50 00 00 20 68
    //D0 17 22 31 00 01 64 22 00 02 10 00 01 66 02 10 56 05 50 00 00 20 27
    //D0 16 45 18 00 89 54 98 00 00 58 00 89 54 04 10 25 00 00 01 10 20 75 (区间停车)
    nByte = BCD2INT(Buf[21]) & 0x07;
    switch (nByte)
    {
    case 0: strTemp = "降级";
        break;
    case 1:
    case 2:
    case 3: strTemp = "调车";
        break;
    case 4: {
        strTemp = "站内";
        //如果前方信号机是进站信号机，则是机外停车
        if (Info.Xht_code == 3){
            strTemp = "机外";}
        else if ((Info.Xht_code == 4) || (Info.Xht_code == 5)) {
            strTemp = "区间";
        }
    }
        break;
    case 5:
    case 6: strTemp = "调车";
        break;
    }
    Info.Speed = 0;
    Info.Disp = strTemp + Info.Disp;
}

void QAnalysisOrgFile::MakeOneLkjRec_D1(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    uchar nByte;
    QString strTemp;
    //解析赋值
    Info.Disp = "开车";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //前方编号[3] 前方机类型[1]
    Info.Xhj_no = MoreBCD2INT(Buf, 15, 3);
    Info.Xht_code = BCD2INT(Buf[18]);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 19, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;

    Info.Gya = GetLieGuanPressure(Buf, 21);
    Info.Gangy = GetGangPressure(Buf, 23);

    //开车  4 20 60=站内   10 30 50 2 6 10 12 14 18 22=调车     40 00 08 16 =降级
    //D1 21 52 09 00 23 04 85 00 19 58 00 01 01 05 00 23 24 05 11 52 05 70 00 00 20 73
    strTemp = "";
    nByte = BCD2INT(Buf[25]) & 0x07;
    switch (nByte)
    {
    case 0: strTemp = "降级"; break;
    case 1:
    case 2:
    case 3: strTemp = "调车";
        break;
    case 4:{
        strTemp = "站内";
        //如果前方信号机是进站信号机，则是机外停车
        if (Info.Xht_code == 3){
            strTemp = "机外";}
        else if ((Info.Xht_code == 4) || (Info.Xht_code == 5)) {
            strTemp = "区间";}
    }
        break;
    case 5:
    case 6: strTemp = "调车";
        break;
    }

    //  if (nByte == 4) {strTemp = "站内";
    //  if (nByte == 2) {strTemp = "调车";
    //  if (nByte == 6) {strTemp = "调车";
    //  if (nByte == 0) {strTemp = "降级";
    //
    //  if (nByte == 1) {strTemp = "调车"; //红黄SD1
    //  if (nByte == 3) {strTemp = "调车";
    //  if (nByte == 5) {strTemp = "调车";

    Info.Disp = strTemp + Info.Disp;

    //信号机
    //if m_tPreviousInfo.Xhj == NULL_VALUE_STRING) Info.Xhj.sprintf("出站%d", [Info.Xhj_no]);
    //if (Info.Disp == "站内开车") or (Info.Disp == "降级开车")) Info.Xhj.sprintf("出站%d", [Info.Xhj_no]);
    if (m_tPreviousInfo.Xhj == NULL_VALUE_STRING) {
        if ((m_tPreviousInfo.nDataLineID != NULL_VALUE_MAXINT) && (m_tPreviousInfo.nStation != NULL_VALUE_MAXINT)) {
            Info.Xhj.sprintf("%d-%d", m_tPreviousInfo.nDataLineID, m_tPreviousInfo.nStation);
        }
    }
    if ((Info.Disp == "降级开车") || (Info.Disp == "调车开车")) {Info.Xhj.sprintf("出站%d", Info.Xhj_no);}
}

void QAnalysisOrgFile::MakeOneLkjRec_D2(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //D2 15 55 02    01 25 76 60 00   16 85   00 00 00   85   12 20
    //D2 15 55 17    01 25 77 71 00   15 74   00 45 00   85   12 72

    //解析赋值
    Info.Disp = "轮对空转";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_D3(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //D3   15 55 16  01 25 76 76 00   16 69   00 08   00 85   12 05
    //解析赋值
    Info.Disp = "空转结束";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_D7(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "轮径修正";
    Info.Xhj.sprintf("%.01f", MoreBCD2INT(Buf, 1, 3) / 10.0);
}

void QAnalysisOrgFile::MakeOneLkjRec_DA(RCommonInfo &Info, char *Buf, int Len)
{
    uchar nType, nByte;
    QString strTemp;
    //解析时间
    Info.Hms = GetTime(Buf, 2);
    Info.Glb = GetGLB(Buf, 5);
    Info.Jl = GetJL(Buf, 9);

    //解析其它
    nType = Buf[1];
    switch (nType) {
    case 0x01:
        if (Len == 20)
        {
            Info.Disp = "工务线路信息";
            Info.OTHER = m_tPreviousInfo.strSpeedGrade;

            strTemp = "";
            nByte = BCD2INT(Buf[15]) & 0x0F;
            if ((nByte & 0x03) == 0x00) {strTemp = strTemp + "方向-0；";}
            else if ((nByte & 0x03) == 0x01) {strTemp = strTemp + "下行；";}
            else if ((nByte & 0x03) == 0x02) {strTemp = strTemp + "上行；";}
            else if ((nByte & 0x03) == 0x03) {strTemp = strTemp + "上下行；";}
            if ((nByte & 0x04) == 0x04) {strTemp = strTemp + "三线；";}
            else {strTemp = strTemp + "主线；";}
            if ((nByte & 0x08) == 0x08) {strTemp = strTemp + "反向";}
            else {strTemp = strTemp + "正向";}
            Info.ShuoMing = QString("工务线路号: %1;  %2;  重复公里标序号：%3").arg(MoreBCD2INT(Buf, 12, 3)).arg(strTemp).arg(BCD2INT(Buf[16]));

            nByte = BCD2INT(Buf[17]);
            if (nByte == 1) {Info.ShuoMing = Info.ShuoMing + ";  长链标志a";}
        }
        break;
    case 0x03:
        if (Len == 19)
        {
            Info.Disp = "机车信号序号";

            Info.Speed = GetSpeed(Buf, 12);
            Info.S_lmt = GetLimitSpeed(Buf, 14);
            Info.OTHER = m_tPreviousInfo.strSpeedGrade;

            strTemp = "";
            nByte = BCD2INT(Buf[16]);
            if (nByte == 1) {strTemp = "信号序号；L3码";}
            else if (nByte == 2) {strTemp = "信号序号；L2码";}
            else if (nByte == 3) {strTemp = "信号序号；L码";}
            else if (nByte == 4) {strTemp = "信号序号；LU码";}
            else if (nByte == 5) {strTemp = "信号序号；LU2码";}
            else if (nByte == 6) {strTemp = "信号序号；U码";}
            else if (nByte == 7) {strTemp = "信号序号；U2S码";}
            else if (nByte == 8) {strTemp = "信号序号；U2码";}
            else if (nByte == 9) {strTemp = "信号序号；U3码";}
            else {strTemp.sprintf("信号序号；%d", nByte);}
            Info.ShuoMing = strTemp;
        }
        break;
    }
}

void QAnalysisOrgFile::MakeOneLkjRec_DB(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    //DB 15 58 09 01 26 00 07 00 05 47 00 81 00 63 00 01 06 00 00 00 12 40
    //解析赋值
    Info.Disp = "紧急制动";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;

    Info.Gya = GetLieGuanPressure(Buf, 17);
    Info.Gangy = GetGangPressure(Buf, 19);
}

void QAnalysisOrgFile::MakeOneLkjRec_DC(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    //DD 15 57 28 01 25 96 27 00 09 27 00 51 00 63 00 01 06 00 00 00 12 07
    //DC 15 57 28 01 25 96 27 00 09 27 00 51 00 63 00 01 06 00 00 00 12 08
    //解析赋值
    Info.Disp = "常用制动";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;

    Info.Gya = GetLieGuanPressure(Buf, 17);
    Info.Gangy = GetGangPressure(Buf, 19);
}

void QAnalysisOrgFile::MakeOneLkjRec_DD(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    //DD  07 50 35  00 41 15 09  00 10 36  01 23  01 25  10 25 06 10 00 00 12 33
    //解析赋值
    Info.Disp = "卸载动作";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;

    Info.Gya = GetLieGuanPressure(Buf, 17);
    Info.Gangy = GetGangPressure(Buf, 19);
}

void QAnalysisOrgFile::MakeOneLkjRec_E0(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    //解析赋值
    Info.Disp = "机车信号变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;
}

void QAnalysisOrgFile::MakeOneLkjRec_E1(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "制式电平变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;

    //色灯信号
    /*nWord = MoreBCD2INT(Buf, 15, 2);
     Info.Signal = GetLamp(nWord);
     Info.OTHER = GetSD(nWord);
     m_tPreviousInfo.strSpeedGrade = Info.OTHER;  */
}

void QAnalysisOrgFile::MakeOneLkjRec_E3(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    uchar nByte;
    //解析赋值
    Info.Disp = "机车工况变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;

    //???工况状态，与文档不一致，有待总结规律
    nWord = MoreBCD2INT(Buf, 15, 2);
    nByte = nWord & 0xFF;
    Info.Shoub = nByte;
    Info.Hand = GetInfo_GK(nByte & 0x1F);
}

void QAnalysisOrgFile::MakeOneLkjRec_E5(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    uchar nByte;
    QString strTemp;
    //解析赋值
    // E5 02 36 42 00 01 38 53 06 55 17 00 00 00 03 41 23 44
    // F0 00 00 00 03 03 90 03 30 04 30 23 16
    Info.Disp = "调车灯显变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    strTemp = "";
    nByte = BCD2INT(Buf[15]);
    if (nByte == 0) {strTemp = "平调0";} //无定义
    else if (nByte == 1) {strTemp = "停车";}
    else if (nByte == 2) {strTemp = "推进";}
    else if (nByte == 3) {strTemp = "启动";}
    else if (nByte == 4) {strTemp = "连接";}
    else if (nByte == 5) {strTemp = "溜放";}
    else if (nByte == 6) {strTemp = "减速";}
    else if (nByte == 7) {strTemp = "十车";}
    else if (nByte == 8) {strTemp = "五车";}
    else if (nByte == 9) {strTemp = "三车";}
    else if (nByte == 10) {strTemp = "牵出稍动";}
    else if (nByte == 11) {strTemp = "收放权";}
    else if (nByte == 12) {strTemp = "平调12";} //无定义
    else if (nByte == 13) {strTemp = "推进稍动";}
    else if (nByte == 14) {strTemp = "故障停车";}
    else if (nByte == 15) {strTemp = "平调15";} //无定义
    else if (nByte == 16) {strTemp = "紧急停车1";}
    else if (nByte == 17) {strTemp = "紧急停车2";}
    else if (nByte == 18) {strTemp = "紧急停车3";}
    else if (nByte == 19) {strTemp = "紧急停车4";}
    else if (nByte == 20) {strTemp = "紧急停车5";}
    else if (nByte == 21) {strTemp = "紧急停车6";}
    else if (nByte == 22) {strTemp = "紧急停车7";}
    else if (nByte == 23) {strTemp = "紧急停车8";}
    else if (nByte == 24) {strTemp = "解锁1";}
    else if (nByte == 25) {strTemp = "解锁2";}
    else if (nByte == 26) {strTemp = "解锁3";}
    else if (nByte == 27) {strTemp = "解锁4";}
    else if (nByte == 28) {strTemp = "解锁5";}
    else if (nByte == 29) {strTemp = "解锁6";}
    else if (nByte == 30) {strTemp = "解锁7";}
    else if (nByte == 31) {strTemp = "解锁8";}
    else if (nByte == 35) {strTemp = "一车";}
    else if (nByte == 40)
    {
        m_bPingDiao = true;
        strTemp = "平调开始";
    } else if (nByte == 41)
    {
        m_bPingDiao = false;
        strTemp = "平调结束";
    }
    if (strTemp != "") {Info.Signal = strTemp;}

    if (strTemp == "") {Info.Disp = "";} //===测试用，正式时删除
}

void QAnalysisOrgFile::MakeOneLkjRec_E6(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "速度变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_E7(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "转速变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Rota = MoreBCD2INT(Buf, 11, 2);
}

void QAnalysisOrgFile::MakeOneLkjRec_EB(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "管压变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Gya = GetLieGuanPressure(Buf, 11);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_EC(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    //EC 02 36 42 00 01 38 53 06 55 17 00 00 00 41 23 40
    Info.Disp = "限速变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_ED(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    quint16 nWord;
    uchar nByte;
    //解析赋值
    Info.Disp = "定量记录";
    Info.Hms = GetTime(Buf, 1);
//    qDebug() << Info.Hms;
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Speed = GetSpeed(Buf, 11);
    Info.S_lmt = GetLimitSpeed(Buf, 13);

    //色灯信号
    nWord = MoreBCD2INT(Buf, 15, 2);
    Info.Signal = GetLamp(nWord);
    Info.OTHER = GetSD(nWord);
    m_tPreviousInfo.strSpeedGrade = Info.OTHER;

    //???工况状态，与文档不一致，有待总结规律
    nWord = MoreBCD2INT(Buf, 17, 2);
    nByte = nWord & 0xFF;
    Info.Shoub = nByte;
    Info.Hand = GetInfo_GK(nByte & 0x1F);

    Info.Gya = GetLieGuanPressure(Buf, 19);
    Info.Gangy = GetGangPressure(Buf, 21);
    //Info.Jg1 = MoreBCD2INT(Buf, 23, 2); //思维分析软件中，此内容未处理显示
    //Info.Jg2 = MoreBCD2INT(Buf, 25, 2); //思维分析软件中，此内容未处理显示
    Info.Rota = MoreBCD2INT(Buf, 35, 2);
}

void QAnalysisOrgFile::MakeOneLkjRec_EE(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "闸缸压力变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Gangy = GetGangPressure(Buf, 11);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_EF(RCommonInfo &Info, char *Buf, int Len)
{
    Len = Len;
    //解析赋值
    Info.Disp = "均缸压力变化";
    Info.Hms = GetTime(Buf, 1);
    Info.Glb = GetGLB(Buf, 4);
    Info.Jl = GetJL(Buf, 8);
    Info.Jg1 = MoreBCD2INT(Buf, 11, 2);
    Info.Jg2 = MoreBCD2INT(Buf, 13, 2);
    Info.OTHER = m_tPreviousInfo.strSpeedGrade;
}

void QAnalysisOrgFile::MakeOneLkjRec_F0(RCommonInfo &Info, char *Buf, int Len)
{
    if (Len != 13) {return;}
    //F0  00 00  00 03  03 90  03 30  04 30  23 16
    //解析赋值
    Info.Speed = GetSpeed(Buf, 1);
    Info.S_lmt = GetLimitSpeed(Buf, 3);
    Info.Gya = GetLieGuanPressure(Buf, 5);
    Info.Gangy = GetGangPressure(Buf, 7);
    Info.Rota = MoreBCD2INT(Buf, 9, 2);
}

void QAnalysisOrgFile::MakeOneLkjRec_F1(RCommonInfo &Info, char *Buf, int Len)
{
    if (Len != 15) {return;}

    //解析赋值
    Info.Speed = GetSpeed(Buf, 1);
    Info.S_lmt = GetLimitSpeed(Buf, 3);
    Info.Gya = GetLieGuanPressure(Buf, 5);
    Info.Gangy = GetGangPressure(Buf, 7);
    //Info.Rota = MoreBCD2INT(Buf, 9, 2); //电流，思维分析软件中，此内容未处理显示
    //MoreBCD2INT(Buf, 11, 2); //电压
}


void QAnalysisOrgFile::DealWithPosChance(QLKJRuntimeFile *LkjFile)
{
    int i;
    int nSignalDistance;
    nSignalDistance = 0;
    for (i = 0; i < LkjFile->m_Records.size(); i++)
    {
        switch (LkjFile->m_Records[i].CommonRec.nEvent)
        {
        case CommonRec_Event_TrainPosForward :
        {
            LkjFile->m_Records[i].CommonRec.strOther = QString::number(LkjFile->m_Records[i].CommonRec.nDistance);
            break;
        }
        case CommonRec_Event_TrainPosBack :
        {
            LkjFile->m_Records[i].CommonRec.strOther =
                    QString::number(nSignalDistance - LkjFile->m_Records[i].CommonRec.nDistance);
            break;
        }
        case CommonRec_Event_TrainPosReset :
        {
            if ((LkjFile->m_Records[i].CommonRec.nDistance < 300)
                    || (nSignalDistance - LkjFile->m_Records[i].CommonRec.nDistance < 300))
            {
                if ((LkjFile->m_Records[i].CommonRec.nDistance <
                     nSignalDistance - LkjFile->m_Records[i].CommonRec.nDistance) || (nSignalDistance == 0)) {
                    LkjFile->m_Records[i].CommonRec.strOther = "调整距离: " + QString::number(LkjFile->m_Records[i].CommonRec.nDistance);}
                else {
                    LkjFile->m_Records[i].CommonRec.strOther = "调整距离: " + QString::number(nSignalDistance - LkjFile->m_Records[i].CommonRec.nDistance);
                }
            }
            break;
        }
        case CommonRec_Event_SectionSignal :
        {
            nSignalDistance = LkjFile->m_Records[i].CommonRec.nDistance;
            break;
        }
        case CommonRec_Event_InOutStation :
        {
            nSignalDistance = LkjFile->m_Records[i].CommonRec.nDistance;
            break;
        }
        case CommonRec_Event_EnterStation :
        {
            nSignalDistance = LkjFile->m_Records[i].CommonRec.nDistance;
            break;
        }
        case CommonRec_Event_LeaveStation :
        {
            nSignalDistance = LkjFile->m_Records[i].CommonRec.nDistance;
            break;
        }
        }
    }
}


void QAnalysisOrgFile::DealWithJgNumber(QLKJRuntimeFile *LkjFile)
{
    int i;
    int CurrentValue1, CurrentValue2;
    bool bFit1, bFit2;
    int nIndex;

    nIndex = -1;
    for (i = 0; i < LkjFile->m_Records.size(); i++)
    {
        LkjFile->m_Records[i].CommonRec.nValidJG = 0;
        CurrentValue1 = LkjFile->m_Records[i].CommonRec.nJG1Pressure;
        CurrentValue2 = LkjFile->m_Records[i].CommonRec.nJG2Pressure;
        if (LkjFile->m_Records[i].CommonRec.nLieGuanPressure >= 500)
        {
            if (CurrentValue1 >= LkjFile->m_Records[i].CommonRec.nLieGuanPressure)
                bFit1 = true;
            else
                bFit1 = false;
            if (CurrentValue2 >= LkjFile->m_Records[i].CommonRec.nLieGuanPressure)
                bFit2 = true;
            else
                bFit2 = false;

            if ((bFit1 && bFit2) || (!bFit1 && !bFit2))
            {
                if ((i > 0) && (LkjFile->m_Records[i - 1].CommonRec.nValidJG != 0)){
                    LkjFile->m_Records[i].CommonRec.nValidJG = LkjFile->m_Records[i - 1].CommonRec.nValidJG;
                }
            }

            if (bFit1)
            {
                if (nIndex == -1){
                    nIndex = i;
                }
                LkjFile->m_Records[i].CommonRec.nValidJG = 1;
            }


            if (bFit2)
            {
                if (nIndex == -1){
                    nIndex = i;
                }
                LkjFile->m_Records[i].CommonRec.nValidJG = 2;
            }
        }
        else
        {
            if (i > 0) {
                LkjFile->m_Records[i].CommonRec.nValidJG = LkjFile->m_Records[i - 1].CommonRec.nValidJG;
            }
        }

    }

    if (nIndex >= LkjFile->m_Records.size() - 1) {
        return;
    }

    for(i = nIndex; i >= 0; i--)
    {
        LkjFile->m_Records[i].CommonRec.nValidJG = LkjFile->m_Records[i + 1].CommonRec.nValidJG;
    }
}


void QAnalysisOrgFile::clearRLKJRTFileHeadInfo(RLKJRTFileHeadInfo &data)
{
    data.dtKCDataTime = 0;
    data.nLocoType = 0;          //机车类型号(DF11)代码[数字]
    //    DEB("sizeof:%d\n", sizeof(data.strTrainType));
    memset(data.strTrainType, 0, sizeof(data.strTrainType));
    data.nLocoID = 0;            //机车编号
    memset(data.strTrainHead, 0, sizeof(data.strTrainHead));
    data.nTrainNo = 0;           //车次号
    data.nLunJing = 0;           //轮径
    data.nBeiZhouLunJin = 0;     //备轴轮径
    data.nDistance = 0;          //走行距离
    data.nJKLineID = 0;          //交路号
    data.nDataLineID = 0;        //数据交路号
    data.nFirstDriverNO = 0;     //司机工号
    memset(data.strDriverName, 0, sizeof(data.strDriverName));
    data.nSecondDriverNO = 0;    //副司机工号
    memset(data.strSubDriverName, 0, sizeof(data.strSubDriverName));
    data.nStartStation = 0;      //始发站
    data.nEndStation = 0;        //终点站
    memset(data.strStartStation, 0, sizeof(data.strStartStation));
    memset(data.strEndStation, 0, sizeof(data.strEndStation));
    memset(data.nLocoJie, 0, sizeof(data.nLocoJie));
    data.nDeviceNo = 0;          //装置号
    data.nTotalWeight = 0;       //总重
    data.nSum = 0;               //合计(辆数)
    data.nLoadWeight = 0;        //载重
    data.nLength = 0;            //计长
    data.nKeCheNum = 0;          //客车数量
    data.nZhongCheNum = 0;       //重车数量
    data.nKongCheNum = 0;        //空车数量
    data.nFeiYunYongChe = 0;     //非运用车
    data.nDaiKeChe = 0;          //代客车
    data.nShouChe = 0;           //守车
    data.nJKVersion = 0;         //监控版本
    data.nDataVersion = 0;       //数据版本
    data.DTFileHeadDt = 0;      //文件头时间
    data.Factory = sfSiWei;  //软件厂家
    data.TrainType = ttPassenger;  //机车客货类别(货,客)代码[数字]
    data.BenBu = bbBen;      //本机、补机
    data.nStandardPressure = 0;  //标准管压
    data.nMaxLmtSpd = 0;         //输入最高限速
    data.nSpeedChannelNo = 0;    //当前速度通道号
    memset(data.strDisVerOne, 0, sizeof(data.strDisVerOne));
    memset(data.strDisVerTwo, 0, sizeof(data.strDisVerTwo));
    memset(data.strICDump, 0, sizeof(data.strICDump));
    data.nUpDown = 0;            // 1 上行  2 下行
    memset(data.strOrgFileName, 0, sizeof(data.strOrgFileName));
}

void QAnalysisOrgFile::clearRLKJJieShiInfo(RLKJJieShiInfo &data)
{
    //命令号
    data.nCmdNo = 0;
    //交路
    data.nJiaoLuNo = 0;
    //揭示类型
    data.strTypeName.clear();
    //上下行
    data.strUpDown.clear();
    //站号
    data.nStationNo = 0;
    //主线或者三线
    data.strLineType.clear();
    //正/反向
    data.strDirectType.clear();
    //开始时间
    data.dtBegin = QDateTime::fromTime_t(36525);
    //结束时间
    data.dtEnd = QDateTime::fromTime_t(36525);
    //公务线路号
    data.nGWLineNo = 0;
    //开始公里标
    data.nBeginGLB = 0;
    //结束公里标
    data.nEndGLB = 0;
    //客车限速
    data.nKeCheLimitSpeed = 0;
    //货车限速
    data.nHuoCheLimitSpeed = 0;
    //限速长度
    data.nLimitDistance = 0;
    //是否启用
    data.bActive = false;
    //文件名
    data.strFileName.clear();
    //序号
    data.nNo = 0;
    //机车lkj开始时间
    data.dtLKJBegin = QDateTime::fromTime_t(36525);
    //机车LKJ结束时间
    data.dtLKJEnd = QDateTime::fromTime_t(36525);
}

QString QAnalysisOrgFile::trim(char *buf, int len)
{
    return QString::fromLocal8Bit(buf, len);
}


QString QAnalysisOrgFile::GetSD(quint16 nWord)
{
    QString strTemp;
    QString result(NULL_VALUE_STRING);
    if ((nWord & 0x0100) == 0x0100) { strTemp = strTemp + '1';}
    if ((nWord & 0x0200) == 0x0200) { strTemp = strTemp + '2';}
    if ((nWord & 0x0400) == 0x0400) { strTemp = strTemp + '3';}
    if (!strTemp.isEmpty()) { result = "SD"+strTemp;}
    return result;
}

QString QAnalysisOrgFile::GetInfo_GK(uchar nType)
{
    QString strInfo;

    nType = nType & 0x1F;
    switch (nType)
    {
    case 0: strInfo = "加载"; //灭灯
        break;
    case 1: strInfo = "卸载";
        break;
    case 2: strInfo = "加前"; //灭灯
        break;
    case 3: strInfo = "卸前";
        break;
    case 4: strInfo = "加后"; //灭灯
        break;
    case 5: strInfo = "卸后";
        break;
    case 6: strInfo = "加"; //灭灯
        break;
    case 7: strInfo = "卸  ";
        break;
    case 8: strInfo = "加  牵"; //灭灯
        break;
    case 9: strInfo = "卸  牵";
        break;
    case 10: strInfo = "加前牵"; //灭灯
        break;
    case 11: strInfo = "卸前牵";
        break;
    case 12: strInfo = "加后牵"; //灭灯
        break;
    case 13: strInfo = "卸后牵";
        break;
    case 14: strInfo = "加牵"; //灭灯
        break;
    case 15: strInfo = "卸牵";
        break;
    case 16: strInfo = "加  制"; //灭灯
        break;
    case 17: strInfo = "卸  制";
        break;
    case 18: strInfo = "加前制"; //灭灯
        break;
    case 19: strInfo = "卸前制";
        break;
    case 20: strInfo = "加后制"; //灭灯
        break;
    case 21: strInfo = "卸后制";
        break;
    case 22: strInfo = "加制"; //灭灯
        break;
    case 23: strInfo = "卸制";
        break;
    case 24: strInfo = "加  "; //灭灯
        break;
    case 25: strInfo = "卸  ";
        break;
    case 26: strInfo = "加前"; //灭灯
        break;
    case 27: strInfo = "卸前";
        break;
    case 28: strInfo = "加后"; //灭灯
        break;
    case 29: strInfo = "卸后";
        break;
    case 30: strInfo = "加载"; //灭灯
        break;
    case 31: strInfo = "卸载";
        break;
    case 32: strInfo = "加  "; //灭灯
        break;
    default:
        strInfo = "Err";
        break;
    }

    return strInfo;
}


void QAnalysisOrgFile::SetJieShiInfoLKJDT(int nCmdNo, QDateTime dtLKJ, int nXType)
{
    int nIndex;
    for (nIndex = 0 ; nIndex < (m_LKjFile->JieShiInfoArr.size()); nIndex++)
    {
        if (m_LKjFile->JieShiInfoArr[nIndex].nCmdNo == nCmdNo)
        {
            if (nXType == 1)
            {
                m_LKjFile->JieShiInfoArr[nIndex].bActive = true;
                m_LKjFile->JieShiInfoArr[nIndex].dtLKJBegin = dtLKJ;
            } else if (nXType == 2)
            {
                m_LKjFile->JieShiInfoArr[nIndex].dtLKJEnd = dtLKJ;
            }
        }
    }
}
