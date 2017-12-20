#include <QCoreApplication>

#include "../../../ProjectCommon/ShareMem/CShrMem_DriverState.h"
#include "../../../Common/Tools/QTFSystemUtils.h"
void run();
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
if(QTFSystemUtils::getInstance()->checkProgramIsRunning("Alarm_test")) return 0;

    int nKeepSec = 0;
    int nState=0;
    while(1)
    {
        nKeepSec++;
        if (nKeepSec > 15)
        {
            nKeepSec = 0;
            if (nState == 0)
            {
                nState=1;
            }
            else
            {
                nState=0;
            }
        }
        CShrMem_DriverState::getInstance()->setKeepSec(nKeepSec);
        CShrMem_DriverState::getInstance()->setAluState(nState);
        sleep(1);
    }
    return a.exec();
}

void run()
{

}
