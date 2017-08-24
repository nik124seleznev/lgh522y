#ifndef _AD5823_H
#define _AD5823_H

#include <linux/ioctl.h>
/*                           */

#define AD5823_MAGIC 'A'
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
} stAD5823_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define AD5823IOC_G_MOTORINFO _IOR(AD5823_MAGIC, 0, stAD5823_MotorInfo)

#define AD5823IOC_T_MOVETO _IOW(AD5823_MAGIC, 1, u32)

#define AD5823IOC_T_SETINFPOS _IOW(AD5823_MAGIC, 2, u32)

#define AD5823IOC_T_SETMACROPOS _IOW(AD5823_MAGIC, 3, u32)

#else
#endif
