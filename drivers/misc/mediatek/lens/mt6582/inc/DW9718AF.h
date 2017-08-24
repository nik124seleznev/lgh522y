#ifndef _DW9718AF_H
#define _DW9718AF_H

#include <linux/ioctl.h>
//                         

#define DW9718AF_MAGIC 'A'
//                                  


//          
typedef struct {
//                
unsigned long u4CurrentPosition;
//              
unsigned long u4MacroPosition;
//                 
unsigned long u4InfPosition;
//            
bool          bIsMotorMoving;
//           
bool          bIsMotorOpen;
//           
bool          bIsSupportSR;
} stDW9718AF_MotorInfo;


//          
//                  
#ifdef LensdrvCM3
typedef struct {
	//                                              
	float Aperture;
	//                                                   
	float FilterDensity;
	//                                                                            
	float FocalLength;
	//                                                        
	float FocalDistance;
	//                           
	u16 u4OIS_Mode;
	//      
	u16 Facing;
	//                                                                             
	float OpticalAxisAng[2];
	//                                                 
	float Position[3];
	//                  
	float FocusRange;
	//     
	u16 State;
	//    
	float InfoAvalibleMinFocusDistance;
	float InfoAvalibleApertures;
	float InfoAvalibleFilterDensity;
	u16 InfoAvalibleOptStabilization;
	float InfoAvalibleFocalLength;
	float InfoAvalibleHypeDistance;
}stDW9718AF_MotorMETAInfo;
#endif

//               
//                           
//                             
//                                   
//                               
//                                   
//                                   
#define DW9718AFIOC_G_MOTORINFO _IOR(DW9718AF_MAGIC,0,stDW9718AF_MotorInfo)

#define DW9718AFIOC_T_MOVETO _IOW(DW9718AF_MAGIC,1,unsigned long)

#define DW9718AFIOC_T_SETINFPOS _IOW(DW9718AF_MAGIC,2,unsigned long)

#define DW9718AFIOC_T_SETMACROPOS _IOW(DW9718AF_MAGIC,3,unsigned long)
#ifdef LensdrvCM3
#define DW9718AFIOC_G_MOTORMETAINFO _IOR(DW9718AF_MAGIC,4,stDW9718AF_MotorMETAInfo)
#endif

#else
#endif
