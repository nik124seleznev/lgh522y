#ifndef _UAPIS390_CMB_H
#define _UAPIS390_CMB_H

#include <linux/types.h>

/* 
                                                                 
                                 
                                          
                                            
                                   
                                               
                                                   
                                                     
                                                           
                                                       
                                                       
                                                                              
  
                                                             
                                                              
               
                                                                
                                                              
                                                              
                               
                                                                   
                    
 */
struct cmbdata {
	__u64 size;
	__u64 elapsed_time;
 /*                           */
	__u64 ssch_rsch_count;
	__u64 sample_count;
	__u64 device_connect_time;
	__u64 function_pending_time;
	__u64 device_disconnect_time;
	__u64 control_unit_queuing_time;
	__u64 device_active_only_time;
 /*                       */
	__u64 device_busy_time;
	__u64 initial_command_response_time;
};

/*                            */
#define BIODASDCMFENABLE	_IO(DASD_IOCTL_LETTER, 32)
/*                            */
#define BIODASDCMFDISABLE	_IO(DASD_IOCTL_LETTER, 33)
/*                               */
#define BIODASDREADALLCMB	_IOWR(DASD_IOCTL_LETTER, 33, struct cmbdata)

#endif /*                 */
