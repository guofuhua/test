#include "SystemSetting/QSystemSetting.h"
#include "ShareMem/CShrMem_SystemMode.h"

int main()
{
    QSystemSetting::getInstance()->setU6AMode(SYSMODEUN6A);
    sync();
}
