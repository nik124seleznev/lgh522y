#ifndef _GAF001AF_H
#define _GAF001AF_H

#include <linux/ioctl.h>
//                         

#define GAF001AF_MAGIC 'A'
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
} stGAF001AF_MotorInfo;


//               
//                           
//                             
//                      
//                               
//                                   
//                                   
#define GAF001AFIOC_G_MOTORINFO _IOR(GAF001AF_MAGIC,0,stGAF001AF_MotorInfo)

#define GAF001AFIOC_T_MOVETO _IOW(GAF001AF_MAGIC,1,unsigned long)

#define GAF001AFIOC_T_SETINFPOS _IOW(GAF001AF_MAGIC,2,unsigned long)

#define GAF001AFIOC_T_SETMACROPOS _IOW(GAF001AF_MAGIC,3,unsigned long)

#define GAF001AFIOC_T_SETPARA _IOW(GAF001AF_MAGIC,5,unsigned long)

#else
#endif
