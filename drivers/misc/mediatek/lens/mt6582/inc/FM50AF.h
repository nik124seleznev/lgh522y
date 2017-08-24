#ifndef _FM50AF_H
#define _FM50AF_H

#include <linux/ioctl.h>
/*                           */

#define FM50AF_MAGIC 'A'
/*                                    */


/*            */
typedef struct {
/*                  */
	unsigned long u4CurrentPosition;
/*                */
	unsigned long u4MacroPosition;
/*                   */
	unsigned long u4InfPosition;
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

#define FM50AFIOC_T_MOVETO _IOW(FM50AF_MAGIC, 1, unsigned long)

#define FM50AFIOC_T_SETINFPOS _IOW(FM50AF_MAGIC, 2, unsigned long)

#define FM50AFIOC_T_SETMACROPOS _IOW(FM50AF_MAGIC, 3, unsigned long)

#else
#endif
