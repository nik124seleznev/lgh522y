#ifndef _CAM_CAL_H
#define _CAM_CAL_H

#include <linux/ioctl.h>
#ifdef CONFIG_COMPAT
//      
#include <linux/fs.h>
#include <linux/compat.h>
#endif


#define CAM_CALAGIC 'i'
//                                  
//                           
//                             
//                      
//                               
//                                   
//                                   

/*                                                                              
 
                                                                               */

//             
#define CAM_CALIOC_S_WRITE            _IOW(CAM_CALAGIC,0,stCAM_CAL_INFO_STRUCT)
//            
#define CAM_CALIOC_G_READ            _IOWR(CAM_CALAGIC,5,stCAM_CAL_INFO_STRUCT)


#ifdef CONFIG_COMPAT
#define COMPAT_CAM_CALIOC_G_READ            _IOWR(CAM_CALAGIC,5, COMPAT_stCAM_CAL_INFO_STRUCT)

#endif

#endif //          


