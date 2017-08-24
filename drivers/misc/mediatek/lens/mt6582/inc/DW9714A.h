#ifndef _DW9714A_H
#define _DW9714A_H

#include <linux/ioctl.h>
/*                           */

#define DW9714A_MAGIC 'A'
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
} stDW9714A_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define DW9714AIOC_G_MOTORINFO _IOR(DW9714A_MAGIC, 0, stDW9714A_MotorInfo)

#define DW9714AIOC_T_MOVETO _IOW(DW9714A_MAGIC, 1, unsigned long)

#define DW9714AIOC_T_SETINFPOS _IOW(DW9714A_MAGIC, 2, unsigned long)

#define DW9714AIOC_T_SETMACROPOS _IOW(DW9714A_MAGIC, 3, unsigned long)

#else
#endif
