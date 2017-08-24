#ifndef _GAF002AF_H
#define _GAF002AF_H

#include <linux/ioctl.h>
//                         

#define GAF002AF_MAGIC 'A'
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
} stGAF002AF_MotorInfo;


//               
//                           
//                             
//                      
//                               
//                                   
//                                   
#define GAF002AFIOC_G_MOTORINFO _IOR(GAF002AF_MAGIC,0,stGAF002AF_MotorInfo)

#define GAF002AFIOC_T_MOVETO _IOW(GAF002AF_MAGIC,1,unsigned long)

#define GAF002AFIOC_T_SETINFPOS _IOW(GAF002AF_MAGIC,2,unsigned long)

#define GAF002AFIOC_T_SETMACROPOS _IOW(GAF002AF_MAGIC,3,unsigned long)

#define GAF002AFIOC_T_SETPARA _IOW(GAF002AF_MAGIC,5,unsigned long)

#else
#endif
