#ifndef QANALYSISORGFILE_H
#define QANALYSISORGFILE_H

#include <QObject>
//#include "LKJRuntimeTypes.h"
#include "QLKJRuntimeFile.h"
#include "QFieldConvert.h"
#include "QCZBMAnalysis.h"
#include <QStringList>


//为了和TmpTest.Ado兼容，和QLKJRuntimeFile中的RCommonRec兼容，特定义此记录
typedef struct
{
    //以下和TmpTest.Ado兼容，变量名与表格字段名一致
    int Rec;              //全程记录行号
    QString Disp;         //事件描述，重点考虑
    QString Hms;          //=时分秒，重点考虑
    int Glb;              //=公里标
    QString Xhj;          //=信号机
    int Xht_code;         //=信号机类型
    int Xhj_no;           //=信号机编号6244
    int Xh_code;          //=信号（色灯或平调信号）
    int Speed;            //=速度
    int Shoub;            //=手柄状态（与工况状态取同一值）
    QString Hand;         //=工况状态
    int Gya;              //=管压
    int Rota;             //=转速（电流）
    int S_lmt;            //=限速
    int Jl;               //=距离
    int Gangy;            //=闸压（缸压）
    QString OTHER;        //其它
    QString Signal;       //=信号（色灯或平调信号）
    QString ShuoMing;
    int Jg1;              //=均缸1
    int Jg2;              //=均缸2
    int JKZT;             //=

    //以下和QLKJRuntimeFile中的RCommonRec兼容，定义部分变量
    int nJKLineID;        //当前监控交路号
    int nDataLineID;      //当前数据交路号
    int nStation;         //当前车站号

    //以下为处理方便，自己定义使用的变量
    QString strSpeedGrade;//速度等级
    QDate dtEventDate;//事件发生日期
    int nStreamPostion;   //当前流位置
}RCommonInfo;

class QAnalysisOrgFile : public QObject
{
    Q_OBJECT
public:
    explicit QAnalysisOrgFile(QObject *parent = 0);

    //{功能: 读文件的头}
    void ReadHead(QString FileName, RLKJRTFileHeadInfo &HeadInfo);
    //{功能：读取原始运行记录文件，写入TLKJRuntimeFile}
    void ConvertToLkj(QString strFile, QLKJRuntimeFile *LkjFile);
    //{功能：读取原始运行记录文件，写入TLKJRuntimeFile}
//    void ConvertToLkjFiles(QStringList strFileList, QLKJRuntimeFile *LkjFile);
private:
    //保存最新的RCommonInfo完整数据，以便给翻译出来的事件空值字段赋值
    RCommonInfo m_tPreviousInfo;
    QLKJRuntimeFile *m_LKjFile;
    bool m_bPingDiao;
    //上一个交路号
    int m_nPriorDataLineNo;
    //上一个车站号
    int m_nPriorStationNo;
    QFieldConvert *m_FieldConvert;
    QCZBMAnalysis *m_CZBMAnalysis;

private:
    int BCD2INT(uchar src);
    int MoreBCD2INT(char *Buf, int nBegin, int nLen);
    //返回时间
    QString GetTime(char *Buf, int nBegin);
    //返回公路标
    int GetGLB(char *Buf, int nBegin);
    //返回距离
    int GetJL(char *Buf, int nBegin);
    //返回速度
    int GetSpeed(char *Buf, int nBegin);
    //返回限速
    int GetLimitSpeed(char *Buf, int nBegin);
    int GetLieGuanPressure(char *Buf, int nBegin);
    int GetGangPressure(char *Buf, int nBegin);
    //返回凭证类型
    QString GetPZTypeName(char *Buf, int nBegin);

    QLKJCommonRec FileRowToLkjRec(RCommonInfo  Info);
    //读文件头
    void ReadHeadInfo(RLKJRTFileHeadInfo &Head, char *Buf, int Len); //读文件头信息
    //在运行记录中增加文件头信息
    void AddHeadInfoToLKJFile(const RLKJRTFileHeadInfo &Head, TLkjCommonRecLst &LkjCommonRecLst);
    //在运行记录中增加揭示信息
    void AddJieShiInfoToLKJFile(RLKJJieShiInfo &JieShiInfo);
    //处理揭示信息是否启用
    void SetJieShiInfoLKJDT(int nCmdNo, QDateTime dtLKJ, int nXType);
    //初始化RCommonInfo
    void InitCommonInfo(RCommonInfo &Info);
    //调整RCommonInfo
    void AdjustCommonInfo(RCommonInfo &Info);
    //根据当前RCommonInfo，生成最新的RCommonInfo完整数据
    void MakePreviousInfo(RCommonInfo &Info, uchar nType);

    QString GetLamp(quint16 nWord);
    EM_LampSign GetLampType(QString strLamp);
    EM_LKJSignType GetXhjType(uchar nxhj_type);
    QString GetSD(quint16 nWord);
    QString GetInfo_GK(uchar nType);
    //功能：处理均缸数据，确定当前使用的为哪一个均缸
    void DealWithJgNumber(QLKJRuntimeFile *LkjFile);
    //功能：处理车位调整记录
    void DealWithPosChance(QLKJRuntimeFile *LkjFile);
protected:
    void DealOneFileInfo(RCommonInfo &Info, char *Buf, int Len);
private:
    void MakeOneLkjRec_A0(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_A4(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_A8(RCommonInfo &Info, char *Buf, int Len);

    void MakeOneLkjRec_B1(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_B4(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_B6(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_B7(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_B8(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_B9(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_BE(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_BA(RCommonInfo &Info, char *Buf, int Len);

    void MakeOneLkjRec_C0(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C1(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C2(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C3(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C4(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C5(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C6(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C7(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C8(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_C9(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_CA(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_CB(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_CC(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_CD(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_CE(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_CF(RCommonInfo &Info, char *Buf, int Len);

    void MakeOneLkjRec_D0(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_D1(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_D2(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_D3(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_D7(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_DA(RCommonInfo &Info, char *Buf, int Len);

    void MakeOneLkjRec_DB(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_DC(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_DD(RCommonInfo &Info, char *Buf, int Len);

    void MakeOneLkjRec_E0(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_E1(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_E3(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_E5(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_E6(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_E7(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_EB(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_EC(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_ED(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_EE(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_EF(RCommonInfo &Info, char *Buf, int Len);

    void MakeOneLkjRec_F0(RCommonInfo &Info, char *Buf, int Len);
    void MakeOneLkjRec_F1(RCommonInfo &Info, char *Buf, int Len);

    void clearRLKJRTFileHeadInfo(RLKJRTFileHeadInfo &data);
    void clearRLKJJieShiInfo(RLKJJieShiInfo &data);
    QString trim(char* buf, int len);
signals:
    
public slots:
    
};


#endif // QANALYSISORGFILE_H
