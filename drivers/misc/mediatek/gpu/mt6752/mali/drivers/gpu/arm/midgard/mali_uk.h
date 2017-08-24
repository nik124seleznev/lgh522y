/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





/* 
                  
                                                                     
                                                
 */

#ifndef _UK_H_
#define _UK_H_

#ifdef __cplusplus
extern "C" {
#endif				/*             */

#include <malisw/mali_stdtypes.h>

/* 
                       
     
 */

/* 
                                             
  
                                                                                                                  
                                                                                                            
  
                                                                                                             
                                                                    
  
                                                                                  
  
     
 */

/* 
                                                                                                  
                                                                                              
                                                                
  
                                                                                                 
                                                                                              
                                                                      
  
 */
enum uk_client_id {
	/* 
                                                     
  */
	UK_CLIENT_MALI_T600_BASE,

	/*                                                                               */
	UK_CLIENT_COUNT
};

/* 
                                                                       
                                                                 
                                                               
 */
enum uk_func {
	UKP_FUNC_ID_CHECK_VERSION,   /*                              */
	/* 
                                                                   
                                                                     
                                                                      
                                                                     
  */
	UK_FUNC_ID = 512
};

/* 
                                                                             
                                                                           
                                                                              
                                                                           
                                                                       
                                                                      
                                                                               
                                                                           
                                                                           
                                                           
 */
union uk_header {
	/* 
                                                           
                     
  */
	u32 id;
	/* 
                                                                  
                                                               
                                                            
                                                   
  */
	u32 ret;
	/*
                                                                 
                                                                       
  */
	u64 sizer;
};

/* 
                                                                                                    
                                                                                                          
 */
struct uku_version_check_args {
	union uk_header header;
		  /*                  */
	u16 major;
	   /*                                                                                                       */
	u16 minor;
	   /*                                                                                                        */
	u8 padding[4];
};

/*                      */

/*     *//*                    */

#ifdef __cplusplus
}
#endif				/*             */
#endif				/*        */
