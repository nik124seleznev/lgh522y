#ifdef CONFIG_MTK_MT8193_HDMI_SUPPORT

#include "mt8193_ctrl.h"
#include <linux/kernel.h>
#include <linux/delay.h>
#include <asm/div64.h>

#include <mach/devs.h>
#include <mach/mt_typedefs.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>

/*                           */
void vSetClk(void)
{
	/*                     */
	/*                     */
	/*                     */
}

u8 fgMT8193DDCByteWrite(u8 ui1Device, u8 ui1Data_Addr, u8 u1Data)
{
	u8 fgResult = 0;

	/*                                                                  */


	if (fgResult == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

u8 fgMT8193DDCDataWrite(u8 ui1Device, u8 ui1Data_Addr, u8 u1Count, const u8 *pr_u1Data)
{
	u8 fgResult = 0;

	/*                                                                  */


	if (fgResult == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}

}

u8 fgMT8193DDCByteRead(u8 ui1Device, u8 ui1Data_Addr, u8 *pu1Data)
{
	u8 fgResult = 0;


	/*                                                                */


	if (fgResult == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

u8 fgMT8193DDCDataRead(u8 ui1Device, u8 ui1Data_Addr, u8 u1Count, u8 *pu1Data)
{
	u8 fgResult = 0;


	/*                                                                */


	if (fgResult == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

#endif
