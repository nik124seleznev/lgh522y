#ifndef _MT9P017AF_H
#define _MT9P017AF_H

#include <linux/ioctl.h>
/*                           */

#define MT9P017AF_MAGIC 'A'
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
} stMT9P017AF_MotorInfo;

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define MT9P017AFIOC_G_MOTORINFO _IOR(MT9P017AF_MAGIC, 0, stMT9P017AF_MotorInfo)

#define MT9P017AFIOC_T_MOVETO _IOW(MT9P017AF_MAGIC, 1, u32)

#define MT9P017AFIOC_T_SETINFPOS _IOW(MT9P017AF_MAGIC, 2, u32)

#define MT9P017AFIOC_T_SETMACROPOS _IOW(MT9P017AF_MAGIC, 3, u32)

#else
#endif
