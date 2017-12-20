#include "QLKJRuntimeFile.h"
#include "QAnalysisOrgFile.h"
#include "Tools/QTFDevUtils.h"
#include <QDir>
#include "QLKJFileCfg.h"
#include "Include/entity.h"

QLKJRuntimeFile::QLKJRuntimeFile(QObject *parent) :
    QObject(parent)
{
    bLoaded = false;
}

void QLKJRuntimeFile::GetHeadInfoFromOrg(QString orgFileName, RLKJRTFileHeadInfo &HeadInfo)
{
    QAnalysisOrgFile AnalysisFile;
    AnalysisFile.ReadHead(orgFileName, HeadInfo);
    DebugHeadInfo(HeadInfo);
}

void QLKJRuntimeFile::Clear()
{
    m_Records.clear();
    JieShiInfoArr.clear();
}

void QLKJRuntimeFile::DebugHeadInfo(RLKJRTFileHeadInfo info)
{
    qDebug() << "time" << QDateTime::fromTime_t(info.dtKCDataTime).toString(Qt::ISODate);
    printf("LocoType:%d\tTrain type:%s\tLocoID:%d\tTrain Head:%s\t Train No:%d\tLun Jing:%d\tBei Zhou Lun Jing:%d\n",\
           info.nLocoType, info.strTrainType, info.nLocoID, info.strTrainHead, info.nTrainNo, info.nLunJing, info.nBeiZhouLunJin);
    printf("distance:%d\tJKLineID:%d\tDataLineID:%d\tDriverNo:%d\tDriverName:%s\tsub DriverNo:%d\tsub DriverName:%s\n", \
           info.nDistance, info.nJKLineID, info.nDataLineID, info.nFirstDriverNO, info.strDriverName, info.nSecondDriverNO, info.strSubDriverName);
    printf("StartStation:%d\tEndStation:%d\tstrStart:%s\tstrEnd:%s\tLocoJie:%s\tDeviceNo:%d\tTotalWeight:%d\n", \
           info.nStartStation, info.nEndStation, info.strStartStation, info.strEndStation, info.nLocoJie, info.nDeviceNo, info.nTotalWeight);
    printf("Sum:%d\tLoadWeight:%d\tLength:%d\tKeCheNum:%d\tZhongCheNum:%d\tKongCheNum:%d\tFeiYunYongChe:%d\n",\
           info.nSum, info.nLoadWeight, info.nLength, info.nKeCheNum, info.nZhongCheNum, info.nKongCheNum, info.nFeiYunYongChe);
    printf("DaiKeChe:%d\tShouChe:%d\tJKVersion:%d\tDataVersion:%d\tFactory:%d\tTrainType:%d\tBenBu:%d\n",\
           info.nDaiKeChe, info.nShouChe, info.nJKVersion, info.nDataVersion, info.Factory, info.TrainType, info.BenBu);

    qDebug() << "DTFileHeadDt" << QDateTime::fromTime_t(info.DTFileHeadDt).toString(Qt::ISODate);
    printf("Pressure:%d\tLmtSpd:%d\tChannelNo:%d\tVerOne:%s\tVerTwo:%s\tICDump:%s\tUpDown:%d\n", \
           info.nStandardPressure, info.nMaxLmtSpd, info.nSpeedChannelNo, info.strDisVerOne, info.strDisVerTwo, info.strICDump, info.nUpDown);
    printf("OrgFileName:%s\n", info.strOrgFileName);
}

void QLKJRuntimeFile::SyncFile()
{
    QString sdPath;
    if (QTFDevUtils::getInstance()->isSDMount(sdPath)) {
        QString LKJ_Dir = sdPath + "LKJFile/File";
        QStringList filesList = QLKJFileCfg::getInstance()->getSyncFiles();
        qDebug() << filesList;
        int index = filesList.indexOf(QLKJFileCfg::getInstance()->getUploadFile());
        if (-1 == index) {
            index = filesList.size() - 1;
        }
        int No = QLKJFileCfg::getInstance()->getUploadNo();
        int i = 0;
        QStringList filePathList;
        for (i = index; i >= 0; i--) {
            QString filePath = QDir::cleanPath(QString("%1/%2").arg(LKJ_Dir).arg(filesList.at(i)));
            filePathList.append(filePath);
            if (i == 8) {
                break;
            }
        }
        qDebug() << filePathList;
        for (i = 0; i < filePathList.size(); i++) {
            LoadFromOrgFile(filePathList.at(i));
        }
        if (0 != No) {
            for (i = No - 1; i < m_Records.size(); i++) {
                DebugOneRecord(m_Records.at(i).CommonRec);
            }
        }
    }
    qDebug() << "no ----";
}

void QLKJRuntimeFile::LoadFromOrgFile(QString FileName)
{
    QAnalysisOrgFile AnalysisFile;
    qDebug() << m_Records.size();
    AnalysisFile.ConvertToLkj(FileName, this);
    DebugHeadInfo(HeadInfo);
    qDebug() << JieShiInfoArr.size();
    qDebug() << m_Records.size();
    DebugJieShiInfo();
    DebugRecords();
}

//void QLKJRuntimeFile::LoadFromOrgFileList(QStringList FileNames)
//{
//    QAnalysisOrgFile AnalysisFile;
//    qDebug() << m_Records.size();
//    AnalysisFile.ConvertToLkjFiles(FileNames, this);

//    qDebug() << bLoaded;
//    qDebug() << JieShiInfoArr.size();
//    qDebug() << m_Records.size();
//    DebugJieShiInfo();
//    DebugRecords();
//}

void QLKJRuntimeFile::DebugJieShiInfo()
{
    int i = 0;
    for (i = 0; i < JieShiInfoArr.size(); i++) {
        DebugOneJieShiInfo(JieShiInfoArr.at(i));
    }
}

void QLKJRuntimeFile::DebugOneJieShiInfo(RLKJJieShiInfo info)
{
    QString strout;
    QString strtemp;

    //命令号
    strout.append(strtemp.sprintf("%5d ", info.nCmdNo));
    //交路
    //        strout.append(strtemp.sprintf("%5d ", info.nJiaoLuNo));
    //揭示类型
    strout.append(CheseFill(info.strTypeName, 20, true));
    //上下行
    strout.append(CheseFill(info.strUpDown, 6));
    //站号
    //        strout.append(strtemp.sprintf("%5d ", info.nStationNo));
    //主线或者三线
    strout.append(CheseFill(info.strLineType, 6));
    //正/反向
    strout.append(CheseFill(info.strDirectType, 6));
    //开始时间
    strout.append(info.dtBegin.toString("yyyy-MM-dd hh:mm:ss")+" ");
    //结束时间
    strout.append(info.dtEnd.toString("yyyy-MM-dd hh:mm:ss")+" ");
    //公务线路号
    strout.append(strtemp.sprintf("%5d ", info.nGWLineNo));
    //开始公里标
    strout.append(strtemp.sprintf("%5d ", info.nBeginGLB));
    //结束公里标
    strout.append(strtemp.sprintf("%5d ", info.nEndGLB));
    //客车限速
    strout.append(strtemp.sprintf("%5d ", info.nKeCheLimitSpeed));
    //货车限速
    strout.append(strtemp.sprintf("%5d ", info.nHuoCheLimitSpeed));
    //限速长度
    strout.append(strtemp.sprintf("%5d ", info.nLimitDistance));
    //是否启用
    //        strout.append(bActive ? "true " : "false");
    //文件名
    //        strout.append(info.strFileName + " ");
    //序号
    //        strout.append(strtemp.sprintf("%5d ", info.nNo));
    //机车lkj开始时间
    //        strout.append(info.dtLKJBegin.toString("yyyy-MM-dd hh:mm:ss")+" ");
    //机车LKJ结束时间
    //        strout.append(info.dtLKJEnd.toString("yyyy-MM-dd hh:mm:ss")+" ");
    qDebug() << strout;
}

void QLKJRuntimeFile::DebugRecords()
{
    int i = 0;
    for (i = 0; i < m_Records.size(); i++) {
        DebugOneRecord(m_Records.at(i).CommonRec);
    }
}

QString QLKJRuntimeFile::CheseFill(QString info, int W, bool left)
{
    int i = info.size();
    int ascii = 0;
    int chese = 0;
    for (i = 0; i < info.size(); i++)
    {
        if (info.at(i) < 0x80) {
            ascii++;
        } else {
            chese++;
        }
    }
    int use_space = chese * 2 / 3 + ascii;
    //    if (chese%3) {
    //        qDebug() << info.size() << info << "--------------------------------------";
    //    }

    if (use_space < W) {
        if (left) {
            info += QString(W - use_space, ' ');
        } else {
            info.insert(0, QString(W - use_space, ' '));
        }
    }

    return info;
}

void QLKJRuntimeFile::DebugOneRecord(RCommonRec info)
{
    //temp.("%.23s\tpid:%5d\tCPU:%.2f\tMEM:%.2f\tRSS:%d\n");
    QString temp;
    QString strout;
    strout.append(temp.sprintf("%d\t", info.nRow));               //全程记录行号
    strout.append(CheseFill(info.strDisp, 20, true));        //事件描述
    //    strout.append(temp.sprintf("%.20s\t", info.strDisp.toStdString().c_str()));        //事件描述
    //    qDebug() << info.strDisp.toAscii().toHex();
    strout.append(QDateTime::fromTime_t(info.DTEvent).toString("yyyy-MM-dd hh:mm:ss")+"  ");
    //    strout.append(temp.sprintf("%s\t", info.DTEvent.toString(Qt::ISODate).toStdString().c_str()));      //事件发生时间
    strout.append(temp.sprintf("%3d  ", info.nCoord));             //公里标
    strout.append(temp.sprintf("%5d  ", info.nDistance));          //距信号机距离
    strout.append(TranslateLamp(info.LampSign)+" ");   //灯信号
    //    strout.append(temp.sprintf("%s\t", TranslateLamp(info.LampSign).toStdString().c_str()));   //灯信号
    //    strout.append(temp.sprintf("%s\t", info.strSignal.toStdString().c_str()));      //色灯
    //    strout.append(temp.sprintf("%d\t", info.nLampNo));            //信号机编号6244
    //    strout.append(temp.sprintf("%s\t", TranslateSignType(info.SignType).toStdString().c_str()));//信号机类型
    strout.append(CheseFill(info.strXhj, 10));         //信号机
    //    strout.append(temp.sprintf("%s\t", info.strXhj.toStdString().c_str()));         //信号机
    strout.append(temp.sprintf("%5d ", info.nSpeed));             //运行速度
    strout.append(temp.sprintf("%5d ", info.nLimitSpeed));        //限制速度
    //    strout.append(temp.sprintf("0x%x ", info.nShoub));           //手柄状态
    strout.append(CheseFill(GetInfo_GK(info.nShoub), 10));          //工况状态
    //    strout.append(temp.sprintf("%s\t", info.strGK.toStdString().c_str()));          //工况状态
    //    strout.append(temp.sprintf("%s\t", TranslateWorkZero(info.WorkZero).toStdString().c_str()));//零位[零, info.非]
    //    strout.append(temp.sprintf("%s\t", TranslateHandPos(info.HandPos).toStdString().c_str()));//前后[前, info.后]
    //    strout.append(temp.sprintf("%s\t", TranslateWorkDrag(info.WorkDrag).toStdString().c_str()));//牵制[牵, info.制]
    strout.append(temp.sprintf("%5d ", info.nLieGuanPressure));   //列管压力 - Gya
    strout.append(temp.sprintf("%5d ", info.nGangPressure));      //缸压力
    strout.append(temp.sprintf("%5d ", info.nRotate));            //转速
    strout.append(temp.sprintf("%5d ", info.nJG1Pressure));       //均缸1压力
    strout.append(temp.sprintf("%5d ", info.nJG2Pressure));       //均缸2压力
    strout.append(CheseFill(info.strOther, 20));       //其他
    //    strout.append(temp.sprintf("%s\t", info.strOther.toStdString().c_str()));       //其他
    strout.append(temp.sprintf("%5d ", info.nEvent));             //事件数字代码
    //    strout.append(temp.sprintf("%5d ", info.nJKLineID));          //当前交路号
    strout.append(temp.sprintf("%5d ", info.nDataLineID));        //当前数据交路号
    strout.append(temp.sprintf("%5d ", info.nStation));           //已过车站号
    //    strout.append(temp.sprintf("%d\t", info.nToJKLineID));        //上一个站的交路号
    //    strout.append(temp.sprintf("%d\t", info.nToDataLineID));      //上一个站的数据交路号
    //    strout.append(temp.sprintf("%d\t", info.nToStation));         //上一个站编号
    //    strout.append(temp.sprintf("%d\t", info.nStationIndex));      //从始发站开始战间编号
    //    strout.append(temp.sprintf("%s\t", info.ShuoMing.toStdString().c_str()));       //说明
    //    strout.append(temp.sprintf("%d\t", info.JKZT));               //监控状态  监控＝1，平调＝2，调监＝3
    //    strout.append(temp.sprintf("%d\t", info.nValidJG));           //有效均缸号
    //    strout.append(temp.sprintf("%d\t", info.nStreamPostion));     //流位置
    //    strout.append(temp.sprintf("%s\t", info.strCaption.toStdString().c_str()));     //标题
    qDebug() << strout;
    if (!info.ShuoMing.isEmpty()) {
        qDebug() << info.ShuoMing;       //说明
    }
}

QString QLKJRuntimeFile::TranslateWorkZero(EM_WorkZero WorkZero)
{
    QString str;
    switch(WorkZero)
    {
    case wAtZero:
        str = "零位";
        break;
    case wNotZero:
        str = "非零位";
        break;
    default:
        str = "None";
        break;
    }

    return str;
}

QString QLKJRuntimeFile::TranslateHandPos(EM_HandPos HandPos)
{
    QString str;
    switch(HandPos)
    {
    case hpForword:
        str = "向前";
        break;
    case hpMiddle:
        str = "中";
        break;
    case hpBack:
        str = "向后";
        break;
    case hpInvalid:
        str = "非";
        break;
    default:
        str = "None";
        break;
    }

    return str;
}

QString QLKJRuntimeFile::TranslateWorkDrag(EM_WorkDrag WorkDrag)
{
    QString str;
    switch(WorkDrag)
    {
    case wdDrag:
        str = "牵";
        break;
    case wdMiddle:
        str = "中";
        break;
    case wdBrake:
        str = "制动";
        break;
    case wdInvalid:
        str = "非";
        break;
    default:
        str = "None";
        break;
    }

    return str;
}

QString QLKJRuntimeFile::TranslateSignType(EM_LKJSignType Sign)
{
    QString str;
    switch(Sign)
    {
    case stNormal:
        str = "通过";
        break;
    case stPre:
        str = "预告";
        break;
    case stInOut:
        str = "进出站";
        break;
    case stIn:
        str = "进站";
        break;
    case stOut:
        str = "出站";
        break;
    case stStation:
        str = "中心站";
        break;
    case stAllow:
        str = "容许";
        break;
    case stDivision:
        str = "分割";
        break;
    default:
        str = "无";
        break;
    }
    return str;
}

QString QLKJRuntimeFile::TranslateLamp(EM_LampSign LampSign)
{
    QString str;
    switch(LampSign)
    {
    case lsGreen:
        str = "绿灯";
        break;
    case lsGreenYellow:
        str = "绿黄";
        break;
    case lsYellow:
        str = "黄色";
        break;
    case lsYellow2:
        str = "黄2";
        break;
    case lsDoubleYellow:
        str = "双黄";
        break;
    case lsYellow2S:
        str = "黄2闪";
        break;
    case lsDoubleYellowS:
        str = "双黄闪";
        break;
    case lsRed:
        str = "红灯";
        break;
    case lsRedYellow:
        str = "红黄";
        break;
    case lsRedYellowS:
        str = "红黄闪";
        break;
    case lsWhite:
        str = "白灯";
        break;
    case lsMulti:
        str = "多灯";
        break;
    case lsClose:
        str = "灭灯";
        break;
    case lsPDNone:
        str = "00";
        break;
    case lsPDTingChe:
        str = "停车";
        break;
    case lsPDTuiJin:
        str = "推进";
        break;
    case lsPDQiDong:
        str = "启动";
        break;
    case lsPDLianJie:
        str = "连接";
        break;
    case lsPDLiuFang:
        str = "溜放";
        break;
    case lsPDJianSu:
        str = "减速";
        break;
    case lsPDShiChe:
        str = "十车";
        break;
    case lsPDWuChe:
        str = "五车";
        break;
    case lsPDSanChe:
        str = "三车";
        break;
    case lsPDQianChuShaoDong:
        str = "牵出稍动";
        break;
    case lsPDShouFangQuan:
        str = "收放权";
        break;
    case lsPD12:
        str = "12H";
        break;
    case lsPDTuiJinShaoDong:
        str = "推进稍动";
        break;
    case lsPDGuZhangTingChe:
        str = "故障停车";
        break;
    case lsPD15:
        str = "15H";
        break;
    case lsPDJinJiTingChe1:
        str = "紧急停车1";
        break;
    case lsPDJinJiTingChe2:
        str = "紧急停车2";
        break;
    case lsPDJinJiTingChe3:
        str = "紧急停车3";
        break;
    case lsPDJinJiTingChe4:
        str = "紧急停车4";
        break;
    case lsPDJinJiTingChe5:
        str = "紧急停车5";
        break;
    case lsPDJinJiTingChe6:
        str = "紧急停车6";
        break;
    case lsPDJinJiTingChe7:
        str = "紧急停车7";
        break;
    case lsPDJinJiTingChe8:
        str = "紧急停车8";
        break;
    case lsPDJieSuo1:
        str = "解锁1";
        break;
    case lsPDJieSuo2:
        str = "解锁2";
        break;
    case lsPDJieSuo3:
        str = "解锁3";
        break;
    case lsPDJieSuo4:
        str = "解锁4";
        break;
    case lsPDJieSuo5:
        str = "解锁5";
        break;
    case lsPDJieSuo6:
        str = "解锁6";
        break;
    case lsPDJieSuo7:
        str = "解锁7";
        break;
    case lsPDJieSuo8:
        str = "解锁8";
        break;
    case lsPDYiChe:
        str = "一车";
        break;
    default:
        str = "None";
        break;
    }
    return CheseFill(str, 10);
}

QString QLKJRuntimeFile::GetInfo_GK(uchar nType)
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

//
//FUCTION: 分析串口收到的TAX数据  ，存入TAX数据数组中
//PARAMETER：_data:串口收到的TAX数据,_TXTdata:TAX数据数组
//RETURN：No
#if 0
void QLKJRuntimeFile::toTXTData(unsigned char* _data, RCommonRec &info)
{
    uint  iDataTime = 0;
    memset(_data, 0, TAX_ORIGIN_DATA_LENGTH);
    //TAX数据是否有效
    // int iState = _data[39];//0x01：有数据;   0x00：无数据;  0x02：数据异常
    _data[0] = 0x01;

    QDateTime dateTime = QDateTime::fromTime_t(info.DTEvent);
    uchar year = dateTime.date().year() - 2000;
    uchar month = dateTime.date().month();
    uchar day = dateTime.date().day();
    uchar hour = dateTime.time().hour();
    uchar minute = dateTime.time().minute();
    uchar second = dateTime.time().second();

    iDataTime |= year << 26;
    iDataTime |= month << 22;
    iDataTime |= day << 17;
    iDataTime |= hour << 12;
    iDataTime |= minute << 6;
    iDataTime |= second;
    memcpy(&(_data[3]), &iDataTime, sizeof(uint));

    _data[7] = info.nSpeed & 0xff;
    _data[8] = ((info.nSpeed >> 8) & 0x03) | ((info.nLimitSpeed & 0xf) << 4);
    _data[9] = (info.nLimitSpeed >> 4) & 0xff;
    _data[10] = LampToTax(info.LampSign) & 0xff;
    //手柄位置（0非零,1零位)；
    _data[11] = info.nShoub;
    _data[12] = info.nLampNo & 0xff;
    _data[13] = (info.nLampNo >> 8) & 0xff;
    _data[14] = info.SignType & 0xff;
    //公里标
    _data[15] = info.nCoord & 0xff;
    _data[16] = (info.nCoord >> 8) & 0xff;
    _data[17] = (info.nCoord >> 16) & 0x3f;
    //总重
    _data[18] = HeadInfo.nTotalWeight & 0xff;
    _data[19] = (HeadInfo.nTotalWeight >> 8) & 0xff;
    //计长（单位：0.1米）
    _data[20] = HeadInfo.nLength & 0xff;
    _data[21] = (HeadInfo.nLength >> 8) & 0xff;
    //辆数
    _data[22] = HeadInfo.nSum & 0xff;
    //本/补、客/货
    _data[23] = HeadInfo.TrainType & 0x01;
    _data[23] = (HeadInfo.BenBu & 0x01) << 1;
    //车次
    //区段号（交路号）
    _data[26] = info.nDataLineID & 0xff;
    //车站号
    _data[27] = info.nStation & 0xff;
    info->m_stationNum = _data[27] + _data[43]*256;
    //司机号
    info->m_driverNum = _data[28] + _data[29]*256 + _data[48]*256*256;
    //副司机号
    info->m_assitantDriverNum = _data[30] + _data[31]*256 + _data[49]*256*256;
    //机车号
    info->m_trainNum = _data[32] + _data[33]*256;
    //机车型号
    info->m_trainModel = _data[34]+ _data[52]*256;
    //列车管压力
    info->m_trainPipePress = _data[35] + _data[36]*256;
    //实际交路号  _data[53];

    //制动缸压力
    info->m_breakPreass = _data[54] + _data[55]*256;

    //制动输出    _data[56];
    // 柴速
    info->m_iMotorLevel = _data[57] + _data[58]*256;

    //车次
    info->m_strTrainCode =QString("%1%2%3%4%5").arg((QChar)_data[44]).arg((QChar)_data[45]).arg((QChar)_data[46]).arg((QChar)_data[47]).arg(_data[66] + _data[67]*256 + _data[68]*256*256);
    info->m_strTrainCode =  info->m_strTrainCode.trimmed();

    if(info->m_cheWei == 1)
        info->m_accelerationDirection = SIDE_B_NAME;
    else if (info->m_cheWei  == 2)
        info->m_accelerationDirection = SIDE_A_NAME;
    else
        info->m_accelerationDirection = 'N';


    // b2:1/0-调车/非调车
    //调车标志
    if(_data[37]& 0x04)
    {        //信号机不再刷新，记录为上一次的信号机编号
        info->m_bShuntingFlag = true;

    }else{
        info->m_bShuntingFlag = false;

    }

    if(_data[37]& 0x01){// 真： 降级 / 假：监控
        info->m_bDownLevel = true;
    }else{
        info->m_bDownLevel = false;
    }
}
#endif

uchar QLKJRuntimeFile::LampToTax(EM_LampSign LampSign)
{
    uchar taxTrainSignal = 0;
    switch(LampSign)
    {
    case lsGreen:
        taxTrainSignal = 1;
        break;
    case lsGreenYellow:
        taxTrainSignal = 7;
        break;
    case lsYellow:
        taxTrainSignal = 2;
        break;
    case lsYellow2:
        taxTrainSignal = 8;
        break;
    case lsDoubleYellow:
        taxTrainSignal = 3;
        break;
    case lsRed:
        taxTrainSignal = 5;
        break;
    case lsRedYellow:
        taxTrainSignal = 4;
        break;
    case lsWhite:
        taxTrainSignal = 6;
        break;

    default:
        taxTrainSignal = 0;
        break;
    }
    return taxTrainSignal;
}

QByteArray QLKJRuntimeFile::getBroadcastData(const RCommonRec &src)
{
    RCommonFileRec data;
    memset(&data, 0 , sizeof(RCommonFileRec));
    data.Head = HeadInfo;
    data.nRow =             src.nRow;
    data.nEvent =           src.nEvent;
    data.DTEvent =          src.DTEvent;
    data.nCoord =           src.nCoord;
    data.nDistance =        src.nDistance;
    data.LampSign =         src.LampSign;
    data.nLampNo =          src.nLampNo;
    data.SignType =         src.SignType;
    data.nSpeed =           src.nSpeed;
    data.nLimitSpeed =      src.nLimitSpeed;
    data.nShoub =           src.nShoub;
    data.WorkZero =         src.WorkZero;
    data.HandPos =          src.HandPos;
    data.WorkDrag =         src.WorkDrag;
    data.nLieGuanPressure = src.nLieGuanPressure;
    data.nGangPressure =    src.nGangPressure;
    data.nRotate =          src.nRotate;
    data.nJG1Pressure =     src.nJG1Pressure;
    data.nJG2Pressure =     src.nJG2Pressure;
    memcpy(data.strOther, src.strOther.toStdString().c_str(), MIN(20, src.strOther.size()));
    data.nJKLineID =        src.nJKLineID;
    data.nDataLineID =      src.nDataLineID;
    data.nStation =         src.nStation;
    data.nToJKLineID =      src.nToJKLineID;
    data.nToDataLineID =    src.nToDataLineID;
    data.nToStation =       src.nToStation;
    data.nStationIndex =    src.nStationIndex;
    data.JKZT =             src.JKZT;
    data.nValidJG =         src.nValidJG;
    data.nStreamPostion =   src.nStreamPostion;
    return QByteArray((const char *)&data, sizeof(data));
}
