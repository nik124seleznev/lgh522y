#ifndef _BU6429AF_H
#define _BU6429AF_H

#include <linux/ioctl.h>
/*                           */

#define BU6429AF_MAGIC 'A'
/*                                    */


/*            */
typedef struct {
/*                  */
	u32 u4CurrentPosition;
/*                */
	u32 u4MacroPosition;
/*                   */
	u32 u4InfPosition;
/*              */
	bool bIsMotorMoving;
/*             */
	bool bIsMotorOpen;
/*             */
	bool bIsSupportSR;
} stBU6429AF_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define BU6429AFIOC_G_MOTORINFO _IOR(BU6429AF_MAGIC, 0, stBU6429AF_MotorInfo)

#define BU6429AFIOC_T_MOVETO _IOW(BU6429AF_MAGIC, 1, u32)

#define BU6429AFIOC_T_SETINFPOS _IOW(BU6429AF_MAGIC, 2, u32)

#define BU6429AFIOC_T_SETMACROPOS _IOW(BU6429AF_MAGIC, 3, u32)

#else
#endif
