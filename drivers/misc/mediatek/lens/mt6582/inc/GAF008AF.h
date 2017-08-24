#ifndef _GAF008AF_H
#define _GAF008AF_H

#include <linux/ioctl.h>
//                         

#define GAF008AF_MAGIC 'A'
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
} stGAF008AF_MotorInfo;


//               
//                           
//                             
//                      
//                               
//                                   
//                                   
#define GAF008AFIOC_G_MOTORINFO _IOR(GAF008AF_MAGIC,0,stGAF008AF_MotorInfo)

#define GAF008AFIOC_T_MOVETO _IOW(GAF008AF_MAGIC,1,unsigned long)

#define GAF008AFIOC_T_SETINFPOS _IOW(GAF008AF_MAGIC,2,unsigned long)

#define GAF008AFIOC_T_SETMACROPOS _IOW(GAF008AF_MAGIC,3,unsigned long)

#define GAF008AFIOC_T_SETPARA _IOW(GAF008AF_MAGIC,5,unsigned long)

#else
#endif
