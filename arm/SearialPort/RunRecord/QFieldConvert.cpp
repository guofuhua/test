#include "QFieldConvert.h"

QFieldConvert::QFieldConvert(QObject *parent) :
    QObject(parent)
{
    InitEventDisp();
}

void QFieldConvert::InitEventDisp()
{
    m_EventDisp.insert("文件开始", File_Headinfo_dtBegin);
    m_EventDisp.insert("厂家标志", File_Headinfo_Factory);
    m_EventDisp.insert("本补客货", File_Headinfo_KeHuo);
    m_EventDisp.insert("车次", File_Headinfo_CheCi);
    m_EventDisp.insert("数据交路号", File_Headinfo_DataJL);
    m_EventDisp.insert("交路号", File_Headinfo_JLH);
    m_EventDisp.insert("司机号", File_Headinfo_Driver);
    m_EventDisp.insert("副司机号", File_Headinfo_SubDriver);
    m_EventDisp.insert("辆数", File_Headinfo_LiangShu);
    m_EventDisp.insert("计长", File_Headinfo_JiChang);
    m_EventDisp.insert("记录机车号", File_Headinfo_TrainNo);
    m_EventDisp.insert("记录机车型号", File_Headinfo_TrainType);
    m_EventDisp.insert("记录装置号", File_Headinfo_LkjID);
    m_EventDisp.insert("总重", File_Headinfo_TotalWeight);
    m_EventDisp.insert("载重", File_Headinfo_ZZhong);
    m_EventDisp.insert("车站号", File_Headinfo_StartStation);

    m_EventDisp.insert("限速变化", CommonRec_Event_SpeedLmtChange);
    m_EventDisp.insert("揭示输入", CommonRec_Event_InputReveal);
    m_EventDisp.insert("揭示查询", CommonRec_Event_RevealQuery);
    m_EventDisp.insert("IC卡拔出", CommonRec_Event_PopIC);
    m_EventDisp.insert("IC卡插入", CommonRec_Event_PushIC);
    m_EventDisp.insert("IC卡验证码", CommonRec_Event_Verify);
    m_EventDisp.insert("管压变化", CommonRec_Event_GanYChange);
    m_EventDisp.insert("闸缸压力变化", CommonRec_Event_GangYChange);
    m_EventDisp.insert("定标键", CommonRec_Event_Pos);
    m_EventDisp.insert("侧线选择", CommonRec_Event_CeXian);
    m_EventDisp.insert("信号突变", CommonRec_Event_XingHaoTuBian);


    m_EventDisp.insert("速度变化", CommonRec_Event_SpeedChange);
    m_EventDisp.insert("机车信号变化", CommonRec_Event_SignalChange);
    m_EventDisp.insert("车位向前", CommonRec_Event_TrainPosForward);
    m_EventDisp.insert("车位向后", CommonRec_Event_TrainPosBack);
    m_EventDisp.insert("车位对中", CommonRec_Event_TrainPosReset);
    m_EventDisp.insert("过机校正", CommonRec_Event_GuoJiJiaoZheng);

    m_EventDisp.insert("常用制动", CommonRec_Event_ChangYongBrake);
    m_EventDisp.insert("紧急制动", CommonRec_Event_JinJiBrake);
    m_EventDisp.insert("卸载动作", CommonRec_Event_XieZai);
    m_EventDisp.insert("公里标突变", CommonRec_Event_GLBTuBian);


    m_EventDisp.insert("绿/绿黄确认", CommonRec_Event_EnsureGreenLight);
    m_EventDisp.insert("过机不校", 0);
    //版本TESTDLL
    m_EventDisp.insert("手柄防溜报警开始", CommonRec_Event_FangLiuStart);
    m_EventDisp.insert("手柄防溜报警结束", CommonRec_Event_FangLiuEnd);
    m_EventDisp.insert("相位防溜报警开始", CommonRec_Event_FangLiuStart);
    m_EventDisp.insert("相位防溜报警结束", CommonRec_Event_FangLiuEnd);
    m_EventDisp.insert("管压防溜报警开始", CommonRec_Event_FangLiuStart);
    m_EventDisp.insert("管压防溜报警结束", CommonRec_Event_FangLiuEnd);
    //旧版本TESTDLL
    m_EventDisp.insert("防溜报警开始", CommonRec_Event_FangLiuStart);
    m_EventDisp.insert("防溜报警结束", CommonRec_Event_FangLiuEnd);
    m_EventDisp.insert("过分相", CommonRec_Event_GuoFX);
    m_EventDisp.insert("过信号机", CommonRec_Event_SectionSignal);
    m_EventDisp.insert("临时限速开始", CommonRec_Event_LSXSStart);
    m_EventDisp.insert("临时限速结束", CommonRec_Event_LSXSEnd);
    m_EventDisp.insert("开车对标", CommonRec_Event_DuiBiao);
    m_EventDisp.insert("进站", CommonRec_Event_EnterStation);
    m_EventDisp.insert("出站", CommonRec_Event_LeaveStation);

    m_EventDisp.insert("过站中心", CommonRec_Event_CrossStationMiddle);

    m_EventDisp.insert("轮对空转", CommonRec_Event_KongZhuan);
    m_EventDisp.insert("空转结束", CommonRec_Event_KongZhuanEnd);
    m_EventDisp.insert("自停停车", CommonRec_Event_ZiTing);
    m_EventDisp.insert("区间停车", CommonRec_Event_StopInRect);
    m_EventDisp.insert("区间开车", CommonRec_Event_StartInRect);
    m_EventDisp.insert("机外停车", CommonRec_Event_StopOutSignal);
    m_EventDisp.insert("机外开车", CommonRec_Event_StartOutSignal);
    m_EventDisp.insert("站内停车", CommonRec_Event_StopInStation);
    m_EventDisp.insert("站内开车", CommonRec_Event_StartInStation);
    m_EventDisp.insert("进入降级", CommonRec_Event_JinRuJiangJi);

    m_EventDisp.insert("调车停车", CommonRec_Event_DiaoCheStop);
    m_EventDisp.insert("调车开车", CommonRec_Event_DiaoCheStart);
    m_EventDisp.insert("降级开车", CommonRec_Event_StartInJiangJi);
    m_EventDisp.insert("降级停车", CommonRec_Event_StopInJiangJi);
    m_EventDisp.insert("进入调车", CommonRec_Event_Diaoche);
    m_EventDisp.insert("退出调车", CommonRec_Event_DiaocheJS);
    m_EventDisp.insert("车位向前", CommonRec_Event_TrainPosForward);
    m_EventDisp.insert("车位向后", CommonRec_Event_TrainPosBack);
    m_EventDisp.insert("车位对中", CommonRec_Event_TrainPosReset);

    //tdw add
    m_EventDisp.insert("降级运行", CommonRec_Event_JinRuJiangJi);
    m_EventDisp.insert("特殊前行", CommonRec_Event_TeShuQianXin);
    m_EventDisp.insert("开机", CommonRec_Event_KaiJi);
    m_EventDisp.insert("关机", CommonRec_Event_GuanJi);
    m_EventDisp.insert("输入最高限速", CommonRec_Event_InputMaxSpeed);
    m_EventDisp.insert("前端巡检1", CommonRec_Event_QDXJ1);
    m_EventDisp.insert("前端巡检2", CommonRec_Event_QDXJ2);
    m_EventDisp.insert("报警开始", CommonRec_Event_AlarmStart);
    m_EventDisp.insert("报警结束", CommonRec_Event_AlarmEnd);
    m_EventDisp.insert("解锁键", CommonRec_Event_UnlockKey);
    m_EventDisp.insert("凭证号输入", CommonRec_Event_PZNoInput);
    m_EventDisp.insert("退出出段", CommonRec_Event_ExitChuDuan);
    m_EventDisp.insert("机车工况变化", CommonRec_Event_GKBH);

    m_EventDisp.insert("A机单机", CommonRec_Event_AB);
    m_EventDisp.insert("B机单机", CommonRec_Event_AB);
    m_EventDisp.insert("A主B备", CommonRec_Event_AB);
    m_EventDisp.insert("A备B主", CommonRec_Event_AB);
    m_EventDisp.insert("过揭示起点", CommonRec_Event_CrossRevealBegin);
    m_EventDisp.insert("过揭示终点", CommonRec_Event_CrossRevealEnd);

    m_EventDisp.insert("进入调车监控", CommonRec_Event_DiaocheJK);
    m_EventDisp.insert("退出调车监控", CommonRec_Event_DiaoCheExtJK);
    m_EventDisp.insert("调车灯显变化", CommonRec_Event_DiaoCheSignChange);
    m_EventDisp.insert("进路信息更新", CommonRec_Event_JinLuGenXin);
}

int QFieldConvert::GetnEvent(QString Disp)
{
    return m_EventDisp.value(Disp, 0);
}


EM_WorkZero QFieldConvert::ConvertWorkZero(uchar bHandle)
{
    EM_WorkZero Result;
    if (1 == bHandle % 2) {   //D0
        Result = wAtZero;
    } else {
        Result = wNotZero;
    }
    return Result;
}

EM_WorkDrag QFieldConvert::ConvertWorkDrag(uchar bHandle)
{
    EM_WorkDrag Result = wdInvalid;
    switch ((bHandle / 8) % 4)    //D3D4
    {
    case 0: Result = wdMiddle;
        break;
    case 1: Result = wdDrag;
        break;
    case 2: Result = wdBrake;
        break;
    case 3: Result = wdInvalid;
        break;
    default:
        Result = wdInvalid;
        break;
    }
    return Result;
}

EM_HandPos QFieldConvert::ConvertHandPos(uchar bHandle)
{
    EM_HandPos Result;
    switch ((bHandle / 2) % 4)     //D1D2
    {
    case 0: Result = hpMiddle;
        break;
    case 1: Result = hpForword;
        break;
    case 2: Result = hpBack;
        break;
    default:
        //    3:
        Result = hpInvalid;
        break;
    }
    return Result;
}
