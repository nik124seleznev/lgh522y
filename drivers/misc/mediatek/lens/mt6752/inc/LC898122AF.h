#ifndef _LC898122AF_H
#define _LC898122AF_H

#include <linux/ioctl.h>
//                         

#define LC898122AF_MAGIC 'A'
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
} stLC898122AF_MotorInfo;

//               
//                           
//                             
//                                   
//                               
//                                   
//                                   
#define LC898122AFIOC_G_MOTORINFO _IOR(LC898122AF_MAGIC,0,stLC898122AF_MotorInfo)

#define LC898122AFIOC_T_MOVETO _IOW(LC898122AF_MAGIC,1,unsigned long)

#define LC898122AFIOC_T_SETINFPOS _IOW(LC898122AF_MAGIC,2,unsigned long)

#define LC898122AFIOC_T_SETMACROPOS _IOW(LC898122AF_MAGIC,3,unsigned long)

#else
#endif
