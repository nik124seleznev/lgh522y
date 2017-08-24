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


#ifndef __ARCH_TRIO_SHM_H__
#define __ARCH_TRIO_SHM_H__

#include <arch/abi.h>
#include <arch/trio_shm_def.h>

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
    /*                             */
    int_reg_t va           : 42;
    /* 
                                                        
                    
                    
                    
                     
                     
                     
                      
                      
     */
    uint_reg_t bsz          : 3;
    /* 
                                                      
                                   
                                                                           
                                                                            
                                                                             
                                                                            
                
     */
    uint_reg_t c            : 1;
    /* 
                                                                        
                                                                         
               
     */
    uint_reg_t notif        : 1;
    /* 
                                                                     
                                                                            
              
               
               
               
                
                
                
              
                                                                      
     */
    uint_reg_t smod         : 1;
    /* 
                                                                         
                                                    
     */
    uint_reg_t xsize        : 14;
    /*            */
    uint_reg_t __reserved_0 : 1;
    /* 
                                                                             
                                                                           
                                                                      
                                                                 
     */
    uint_reg_t gen          : 1;
#else   /*                */
    uint_reg_t gen          : 1;
    uint_reg_t __reserved_0 : 1;
    uint_reg_t xsize        : 14;
    uint_reg_t smod         : 1;
    uint_reg_t notif        : 1;
    uint_reg_t c            : 1;
    uint_reg_t bsz          : 3;
    int_reg_t va           : 42;
#endif

    /*        */

#ifndef __BIG_ENDIAN__
    /*                  */
    uint_reg_t io_address : 64;
#else   /*                */
    uint_reg_t io_address : 64;
#endif

  };

  /*              */
  uint_reg_t words[2];
} TRIO_DMA_DESC_t;
#endif /*                         */

#endif /*                               */
