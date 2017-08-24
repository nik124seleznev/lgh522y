/**
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Shujuan Chen <shujuan.chen@stericsson.com> for ST-Ericsson.
 * Author: Jonas Linde <jonas.linde@stericsson.com> for ST-Ericsson.
 * Author: Joakim Bech <joakim.xx.bech@stericsson.com> for ST-Ericsson.
 * Author: Berne Hebark <berne.herbark@stericsson.com> for ST-Ericsson.
 * Author: Niklas Hernaeus <niklas.hernaeus@stericsson.com> for ST-Ericsson.
 * License terms: GNU General Public License (GPL) version 2
 */

#ifndef __CRYP_IRQP_H_
#define __CRYP_IRQP_H_

#include "cryp_irq.h"

/* 
  
                                  
                          
                                                  
                          
                                           
                          
                                            
                          
                                             
                          
                                                
                          
                                
                          
                                                
                          
                                                    
                          
                      
                      
                   
                 
  
                                                    
 */

/* 
                       
                                 
                             
                               
                                        
                                
                                          
                                 
                                             
                                
                                           
                               
                               
                               
                               
                               
                               
                               
                               
                                   
                                   
                                   
                                   
                                   
                                             
                                           
                                            
                                   
                                                          
                    
                    
                    
                                                     
                   
                   
                   
 */
struct cryp_register {
	u32 cr;			/*                          */
	u32 sr;			/*                          */
	u32 din;		/*                          */
	u32 din_size;		/*                          */
	u32 dout;		/*                          */
	u32 dout_size;		/*                           */
	u32 dmacr;		/*                          */
	u32 imsc;		/*                                   */
	u32 ris;		/*                                  */
	u32 mis;		/*                                  */

	u32 key_1_l;		/*                 */
	u32 key_1_r;		/*                 */
	u32 key_2_l;		/*                 */
	u32 key_2_r;		/*                 */
	u32 key_3_l;		/*                 */
	u32 key_3_r;		/*                 */
	u32 key_4_l;		/*                 */
	u32 key_4_r;		/*                 */

	u32 init_vect_0_l;	/*                */
	u32 init_vect_0_r;	/*                */
	u32 init_vect_1_l;	/*                */
	u32 init_vect_1_r;	/*                */

	u32 cryp_unused1[(0x80 - 0x58) / sizeof(u32)];	/*                  */
	u32 itcr;		/*                                  */
	u32 itip;		/*                                */
	u32 itop;		/*                                 */
	u32 cryp_unused2[(0xFE0 - 0x8C) / sizeof(u32)];	/*                  */

	u32 periphId0;		/*                                            */
	u32 periphId1;		/*     */
	u32 periphId2;		/*     */
	u32 periphId3;		/*     */

	u32 pcellId0;		/*                                       */
	u32 pcellId1;		/*     */
	u32 pcellId2;		/*     */
	u32 pcellId3;		/*     */
};

#endif
