#include <QCoreApplication>
#include "../../../AV4/AV4_TrainItemAnalysis_Model/code/Setting/QTrainItemCfg.h"
#include "../../../AV4/AV4_AlarmManage_Model/code/Setting/QAlarmSetting.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTrainItemCfg::getInstance()->RestoreDefaultSetting();
    QAlarmSetting::getInstance()->RestoreDefaultSetting();
    
    return a.exec();
}
