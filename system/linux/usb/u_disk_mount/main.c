#include <stdio.h>
#include "UsbController.h"

int main(int argc, char** argv)
{
	int secondCount = 0;
	UsbController_Init();
	
	
	while (1)
	{
		sleep(1);
		secondCount++;
		if (secondCount >= 5)
		{
			secondCount = 0;
			UsbController_MountMonitor();
		}
	}
}
