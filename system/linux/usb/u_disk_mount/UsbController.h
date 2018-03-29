/**
 * @addtogroup module_genericGateway
 * @{
 */

/**
 * @file
 * @brief USB控制器，管理USB插拔及挂载。
 * @details 
 * @version 1.0.0
 * @author sky.houfei
 * @date 2016-3-18
 */


#ifndef _USB_USBCONTROLLER_H_
#define _USB_USBCONTROLLER_H_


#ifdef __cplusplus
extern "C" {
#endif

//******************************************************************************
#include <stdbool.h>


//******************************************************************************
/**
 * @brief USB控制器初始化，准备USB的监听服务。
 * @return ret, int，如果初始化成功，则返回0，否则为-1.
 */
int UsbController_Init(void);


/**
 * @brief USB设备挂载监听。
 * @details 如果USB之前没有挂载且当前可以挂载，则挂载。
 * \n 如果USB之前挂载成功，此时设备已经被拔出，则卸载。
 */
void UsbController_MountMonitor(void);


/**
* @brief 是否已经挂载成功。
* @return bool s_isMounted, USB设备挂载成功，则返回 true, 否则返回false。
*/
bool UsbController_IsMounted(void);


#ifdef __cplusplus
}
#endif


#endif  // _USB_USBCONTROLLER_H_

/** @} */
