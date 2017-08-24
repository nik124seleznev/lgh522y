#ifndef _BU6424AF_H
#define _BU6424AF_H

#include <linux/ioctl.h>
/*                           */

#define BU6424AF_MAGIC 'A'
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
} stBU6424AF_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define BU6424AFIOC_G_MOTORINFO _IOR(BU6424AF_MAGIC, 0, stBU6424AF_MotorInfo)

#define BU6424AFIOC_T_MOVETO _IOW(BU6424AF_MAGIC, 1, u32)

#define BU6424AFIOC_T_SETINFPOS _IOW(BU6424AF_MAGIC, 2, u32)

#define BU6424AFIOC_T_SETMACROPOS _IOW(BU6424AF_MAGIC, 3, u32)

#else
#endif
