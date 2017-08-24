#ifndef _AD5820AF_H
#define _AD5820AF_H

#include <linux/ioctl.h>
/*                           */

#define AD5820AF_MAGIC 'A'
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
} stAD5820AF_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define AD5820AFIOC_G_MOTORINFO _IOR(AD5820AF_MAGIC, 0, stAD5820AF_MotorInfo)

#define AD5820AFIOC_T_MOVETO _IOW(AD5820AF_MAGIC, 1, u32)

#define AD5820AFIOC_T_SETINFPOS _IOW(AD5820AF_MAGIC, 2, u32)

#define AD5820AFIOC_T_SETMACROPOS _IOW(AD5820AF_MAGIC, 3, u32)

#else
#endif
