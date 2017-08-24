#ifndef _DW9714AF_H
#define _DW9714AF_H

#include <linux/ioctl.h>
/*                           */

#define DW9714AF_MAGIC 'A'
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
} stDW9714AF_MotorInfo;


/*            */
/*                    */
#ifdef LensdrvCM3
typedef struct {
	/*                                                */
	float Aperture;
	/*                                                     */
	float FilterDensity;
	/*                                                                              */
	float FocalLength;
	/*                                                          */
	float FocalDistance;
	/*                            */
	u16 u4OIS_Mode;
	/*        */
	u16 Facing;
	/*                                                                               */
	float OpticalAxisAng[2];
	/*                                                   */
	float Position[3];
	/*                   */
	float FocusRange;
	/*       */
	u16 State;
	/*      */
	float InfoAvalibleMinFocusDistance;
	float InfoAvalibleApertures;
	float InfoAvalibleFilterDensity;
	u16 InfoAvalibleOptStabilization;
	float InfoAvalibleFocalLength;
	float InfoAvalibleHypeDistance;
} stDW9714AF_MotorMETAInfo;
#endif

/*                 */
/*                             */
/*                               */
/*                        */
/*                                 */
/*                                     */
/*                                     */
#define DW9714AFIOC_G_MOTORINFO _IOR(DW9714AF_MAGIC, 0, stDW9714AF_MotorInfo)

#define DW9714AFIOC_T_MOVETO _IOW(DW9714AF_MAGIC, 1, u32)

#define DW9714AFIOC_T_SETINFPOS _IOW(DW9714AF_MAGIC, 2, u32)

#define DW9714AFIOC_T_SETMACROPOS _IOW(DW9714AF_MAGIC, 3, u32)
#ifdef LensdrvCM3
#define DW9714AFIOC_G_MOTORMETAINFO _IOR(DW9714AF_MAGIC, 4, stDW9714AF_MotorMETAInfo)
#endif

#else
#endif
