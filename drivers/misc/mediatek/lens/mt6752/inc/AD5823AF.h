#ifndef _AD5823AF_H
#define _AD5823AF_H

#include <linux/ioctl.h>
/*                           */

#define AD5823AF_MAGIC 'A'
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
} stAD5823AF_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define AD5823AFIOC_G_MOTORINFO _IOR(AD5823AF_MAGIC, 0, stAD5823AF_MotorInfo)

#define AD5823AFIOC_T_MOVETO _IOW(AD5823AF_MAGIC, 1, u32)

#define AD5823AFIOC_T_SETINFPOS _IOW(AD5823AF_MAGIC, 2, u32)

#define AD5823AFIOC_T_SETMACROPOS _IOW(AD5823AF_MAGIC, 3, u32)

#else
#endif
