#ifndef _OV8825AF_H
#define _OV8825AF_H

#include <linux/ioctl.h>
/*                           */

#define OV8825AF_MAGIC 'A'
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
} stOV8825AF_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define OV8825AFIOC_G_MOTORINFO _IOR(OV8825AF_MAGIC, 0, stOV8825AF_MotorInfo)

#define OV8825AFIOC_T_MOVETO _IOW(OV8825AF_MAGIC, 1, u32)

#define OV8825AFIOC_T_SETINFPOS _IOW(OV8825AF_MAGIC, 2, u32)

#define OV8825AFIOC_T_SETMACROPOS _IOW(OV8825AF_MAGIC, 3, u32)

#else
#endif
