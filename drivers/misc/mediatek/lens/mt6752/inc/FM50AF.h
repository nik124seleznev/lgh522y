#ifndef _FM50AF_H
#define _FM50AF_H

#include <linux/ioctl.h>
/*                           */

#define FM50AF_MAGIC 'A'
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
} stFM50AF_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define FM50AFIOC_G_MOTORINFO _IOR(FM50AF_MAGIC, 0, stFM50AF_MotorInfo)

#define FM50AFIOC_T_MOVETO _IOW(FM50AF_MAGIC, 1, u32)

#define FM50AFIOC_T_SETINFPOS _IOW(FM50AF_MAGIC, 2, u32)

#define FM50AFIOC_T_SETMACROPOS _IOW(FM50AF_MAGIC, 3, u32)

#else
#endif
