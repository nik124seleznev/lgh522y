/*
 * Copyright 2012 Tilera Corporation. All Rights Reserved.
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation, version 2.
 *
 *   This program is distributed in the hope that it will be useful, but
 *   WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, GOOD TITLE or
 *   NON INFRINGEMENT.  See the GNU General Public License for
 *   more details.
 */

/*                                      */


#ifndef __ARCH_MPIPE_SHM_H__
#define __ARCH_MPIPE_SHM_H__

#include <arch/abi.h>
#include <arch/mpipe_shm_def.h>

#ifndef __ASSEMBLER__
/* 
                         
                                                                           
                                                                          
                                             
 */

__extension__
typedef union
{
  struct
  {
    /*        */

#ifndef __BIG_ENDIAN__
    /* 
                                                                             
                                                                           
                                                                      
                                                                 
     */
    uint_reg_t gen        : 1;
    /*                           */
    uint_reg_t r0         : 7;
    /*                                                 */
    uint_reg_t csum       : 1;
    /* 
                                                                          
                                                                       
     */
    uint_reg_t ns         : 1;
    /* 
                                                                              
     */
    uint_reg_t notif      : 1;
    /* 
                                                                           
                                                                           
              
     */
    uint_reg_t bound      : 1;
    /*                           */
    uint_reg_t r1         : 4;
    /* 
                                                                          
                                                                       
                                                                            
                                                                            
                                                                           
                                                                     
                                                       
     */
    uint_reg_t xfer_size  : 14;
    /*                           */
    uint_reg_t r2         : 2;
    /* 
                                                                           
                                                                     
                                                                           
                                                             
     */
    uint_reg_t csum_dest  : 8;
    /* 
                                                                      
                                                                      
                                                                     
                                                              
     */
    uint_reg_t csum_start : 8;
    /* 
                                                                            
                                                                           
                                                                             
                                                                            
                                                 
     */
    uint_reg_t csum_seed  : 16;
#else   /*                */
    uint_reg_t csum_seed  : 16;
    uint_reg_t csum_start : 8;
    uint_reg_t csum_dest  : 8;
    uint_reg_t r2         : 2;
    uint_reg_t xfer_size  : 14;
    uint_reg_t r1         : 4;
    uint_reg_t bound      : 1;
    uint_reg_t notif      : 1;
    uint_reg_t ns         : 1;
    uint_reg_t csum       : 1;
    uint_reg_t r0         : 7;
    uint_reg_t gen        : 1;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /*                                                       */
    int_reg_t va           : 42;
    /*            */
    uint_reg_t __reserved_0 : 6;
    /*                                                          */
    uint_reg_t stack_idx    : 5;
    /*            */
    uint_reg_t __reserved_1 : 3;
    /* 
                                                                           
                                                                         
                                                                           
                                                                 
     */
    uint_reg_t inst         : 1;
    /*            */
    uint_reg_t __reserved_2 : 1;
    /* 
                                                                           
                                                                        
                                                                          
     */
    uint_reg_t hwb          : 1;
    /* 
                                                                           
                                                                          
                                                                            
                                                                     
                                         
                    
                    
                    
                     
                     
                     
                      
                      
     */
    uint_reg_t size         : 3;
    /* 
                                                                        
                                                                             
                                                  
     */
    uint_reg_t c            : 2;
#else   /*                */
    uint_reg_t c            : 2;
    uint_reg_t size         : 3;
    uint_reg_t hwb          : 1;
    uint_reg_t __reserved_2 : 1;
    uint_reg_t inst         : 1;
    uint_reg_t __reserved_1 : 3;
    uint_reg_t stack_idx    : 5;
    uint_reg_t __reserved_0 : 6;
    int_reg_t va           : 42;
#endif

  };

  /*              */
  uint_reg_t words[2];
} MPIPE_EDMA_DESC_t;

/* 
                           
                                                                 
                                                                            
                                                               
 */

__extension__
typedef union
{
  struct
  {
    /*        */

#ifndef __BIG_ENDIAN__
    /* 
                                                                      
                                                                   
                                                
     */
    uint_reg_t notif_ring   : 8;
    /*                                                                  */
    uint_reg_t channel      : 5;
    /*            */
    uint_reg_t __reserved_0 : 1;
    /* 
                 
                                                                           
                                                                           
                                         
     */
    uint_reg_t me           : 1;
    /* 
                        
                                                                             
                                        
     */
    uint_reg_t tr           : 1;
    /* 
                                                                           
                                                                            
                                                                            
                                                                            
                                                                             
                   
     */
    uint_reg_t l2_size      : 14;
    /* 
                 
                                                                          
                                                      
     */
    uint_reg_t ce           : 1;
    /* 
                   
                                                                           
                                                                       
                                       
     */
    uint_reg_t ct           : 1;
    /* 
                                                                           
                                                                    
     */
    uint_reg_t bucket_id    : 13;
    /*            */
    uint_reg_t __reserved_1 : 3;
    /* 
                
                                                                           
                                                                          
                                                      
     */
    uint_reg_t cs           : 1;
    /* 
                                
                                                                          
                                                                            
     */
    uint_reg_t nr           : 1;
    /* 
                                                                       
                                                                        
                                      
     */
    uint_reg_t dest         : 2;
    /* 
                                              
                                                                           
                                                                         
                                                                            
                                                          
     */
    uint_reg_t sq           : 1;
    /* 
                        
                                                                             
                                                                      
                                                                   
     */
    uint_reg_t ts           : 1;
    /* 
                                     
                                                                             
                                                                             
                                                                 
                         
     */
    uint_reg_t ps           : 1;
    /* 
                    
                                                                         
                                                                      
                                                                           
     */
    uint_reg_t be           : 1;
    /* 
                                                                         
                                           
     */
    uint_reg_t ctr0         : 5;
    /*            */
    uint_reg_t __reserved_2 : 3;
#else   /*                */
    uint_reg_t __reserved_2 : 3;
    uint_reg_t ctr0         : 5;
    uint_reg_t be           : 1;
    uint_reg_t ps           : 1;
    uint_reg_t ts           : 1;
    uint_reg_t sq           : 1;
    uint_reg_t dest         : 2;
    uint_reg_t nr           : 1;
    uint_reg_t cs           : 1;
    uint_reg_t __reserved_1 : 3;
    uint_reg_t bucket_id    : 13;
    uint_reg_t ct           : 1;
    uint_reg_t ce           : 1;
    uint_reg_t l2_size      : 14;
    uint_reg_t tr           : 1;
    uint_reg_t me           : 1;
    uint_reg_t __reserved_0 : 1;
    uint_reg_t channel      : 5;
    uint_reg_t notif_ring   : 8;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /* 
                                                                         
                                           
     */
    uint_reg_t ctr1          : 5;
    /*            */
    uint_reg_t __reserved_3  : 3;
    /* 
                                                                       
                                                         
     */
    uint_reg_t csum_start    : 8;
    /* 
                                                                         
                                                                            
                                                                           
                                                                       
                                                                           
                                                                         
                                                                            
                                                                         
                                                                           
                                                                         
     */
    uint_reg_t csum_seed_val : 16;
    /* 
                                                                        
                
     */
    uint_reg_t custom0       : 32;
#else   /*                */
    uint_reg_t custom0       : 32;
    uint_reg_t csum_seed_val : 16;
    uint_reg_t csum_start    : 8;
    uint_reg_t __reserved_3  : 3;
    uint_reg_t ctr1          : 5;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /* 
                                                                        
                
     */
    uint_reg_t custom1 : 64;
#else   /*                */
    uint_reg_t custom1 : 64;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /* 
                                                                        
                
     */
    uint_reg_t custom2 : 64;
#else   /*                */
    uint_reg_t custom2 : 64;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /* 
                                                                        
                
     */
    uint_reg_t custom3 : 64;
#else   /*                */
    uint_reg_t custom3 : 64;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /* 
                                                                       
                                                                           
                                    
     */
    uint_reg_t gp_sqn     : 16;
    /* 
                                                                       
                                                       
     */
    uint_reg_t packet_sqn : 48;
#else   /*                */
    uint_reg_t packet_sqn : 48;
    uint_reg_t gp_sqn     : 16;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /* 
                                                                            
                                                                           
     */
    uint_reg_t time_stamp_ns  : 32;
    /* 
                                                                            
                                                                       
     */
    uint_reg_t time_stamp_sec : 32;
#else   /*                */
    uint_reg_t time_stamp_sec : 32;
    uint_reg_t time_stamp_ns  : 32;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /*                                                       */
    int_reg_t va           : 42;
    /*            */
    uint_reg_t __reserved_4 : 6;
    /*                                                          */
    uint_reg_t stack_idx    : 5;
    /*            */
    uint_reg_t __reserved_5 : 3;
    /* 
                                                                           
                                                                         
                                                                           
                                                                 
     */
    uint_reg_t inst         : 1;
    /*            */
    uint_reg_t __reserved_6 : 1;
    /* 
                                                                           
                                                                        
                                                                          
     */
    uint_reg_t hwb          : 1;
    /* 
                                                                           
                                                                          
                                                                            
                                                                     
                                         
                    
                    
                    
                     
                     
                     
                      
                      
     */
    uint_reg_t size         : 3;
    /* 
                                                                        
                                                                             
                                                  
     */
    uint_reg_t c            : 2;
#else   /*                */
    uint_reg_t c            : 2;
    uint_reg_t size         : 3;
    uint_reg_t hwb          : 1;
    uint_reg_t __reserved_6 : 1;
    uint_reg_t inst         : 1;
    uint_reg_t __reserved_5 : 3;
    uint_reg_t stack_idx    : 5;
    uint_reg_t __reserved_4 : 6;
    int_reg_t va           : 42;
#endif

  };

  /*              */
  uint_reg_t words[8];
} MPIPE_PDESC_t;
#endif /*                         */

#endif /*                                */
