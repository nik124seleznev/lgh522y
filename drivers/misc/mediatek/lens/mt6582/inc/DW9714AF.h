#ifndef _DW9714AF_H
#define _DW9714AF_H

#include <linux/ioctl.h>
/*                           */

#define DW9714AF_MAGIC 'A'
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
} stDW9714AF_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define DW9714AFIOC_G_MOTORINFO _IOR(DW9714AF_MAGIC, 0, stDW9714AF_MotorInfo)

#define DW9714AFIOC_T_MOVETO _IOW(DW9714AF_MAGIC, 1, unsigned long)

#define DW9714AFIOC_T_SETINFPOS _IOW(DW9714AF_MAGIC, 2, unsigned long)

#define DW9714AFIOC_T_SETMACROPOS _IOW(DW9714AF_MAGIC, 3, unsigned long)

#else
#endif
