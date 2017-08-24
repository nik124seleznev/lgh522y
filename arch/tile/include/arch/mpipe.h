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

#ifndef __ARCH_MPIPE_H__
#define __ARCH_MPIPE_H__

#include <arch/abi.h>
#include <arch/mpipe_def.h>

#ifndef __ASSEMBLER__

/*
                                           
                                                                             
                                                                            
                                                                        
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*           */
    uint_reg_t __reserved_0  : 3;
    /*                          */
    uint_reg_t ring          : 8;
    /*                       */
    uint_reg_t bucket        : 13;
    /*                          */
    uint_reg_t ring_enable   : 1;
    /*                       */
    uint_reg_t bucket_enable : 1;
    /*
                                                                         
                                                                    
     */
    uint_reg_t region        : 3;
    /*           */
    uint_reg_t __reserved_1  : 6;
    /*                                                                      */
    uint_reg_t svc_dom       : 5;
    /*           */
    uint_reg_t __reserved_2  : 24;
#else   /*                */
    uint_reg_t __reserved_2  : 24;
    uint_reg_t svc_dom       : 5;
    uint_reg_t __reserved_1  : 6;
    uint_reg_t region        : 3;
    uint_reg_t bucket_enable : 1;
    uint_reg_t ring_enable   : 1;
    uint_reg_t bucket        : 13;
    uint_reg_t ring          : 8;
    uint_reg_t __reserved_0  : 3;
#endif
  };

  uint_reg_t word;
} MPIPE_IDMA_RELEASE_REGION_ADDR_t;

/*
                                                                           
                                                                         
                                                      
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*
                                                                      
                                                                             
                              
     */
    uint_reg_t count      : 16;
    /*           */
    uint_reg_t __reserved : 48;
#else   /*                */
    uint_reg_t __reserved : 48;
    uint_reg_t count      : 16;
#endif
  };

  uint_reg_t word;
} MPIPE_IDMA_RELEASE_REGION_VAL_t;

/*
                                            
                                                                       
                                                                             
                                                                         
                                                                  
                  
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*           */
    uint_reg_t __reserved_0 : 3;
    /*                             */
    uint_reg_t stack        : 5;
    /*           */
    uint_reg_t __reserved_1 : 18;
    /*
                                                                         
                                                                            
     */
    uint_reg_t region       : 3;
    /*           */
    uint_reg_t __reserved_2 : 6;
    /*                                                                      */
    uint_reg_t svc_dom      : 5;
    /*           */
    uint_reg_t __reserved_3 : 24;
#else   /*                */
    uint_reg_t __reserved_3 : 24;
    uint_reg_t svc_dom      : 5;
    uint_reg_t __reserved_2 : 6;
    uint_reg_t region       : 3;
    uint_reg_t __reserved_1 : 18;
    uint_reg_t stack        : 5;
    uint_reg_t __reserved_0 : 3;
#endif
  };

  uint_reg_t word;
} MPIPE_BSM_REGION_ADDR_t;

/*
                                          
                                                                       
                                                                             
                                                                        
                                                                          
                   
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*           */
    uint_reg_t __reserved_0 : 7;
    /*
                                                                              
     */
    int_reg_t va           : 35;
    /*           */
    uint_reg_t __reserved_1 : 6;
    /*
                                                                          
                                                                     
     */
    uint_reg_t stack_idx    : 5;
    /*           */
    uint_reg_t __reserved_2 : 5;
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
    uint_reg_t __reserved_2 : 5;
    uint_reg_t stack_idx    : 5;
    uint_reg_t __reserved_1 : 6;
    int_reg_t va           : 35;
    uint_reg_t __reserved_0 : 7;
#endif
  };

  uint_reg_t word;
} MPIPE_BSM_REGION_VAL_t;

/*
                                       
                                                                        
                                                           
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*           */
    uint_reg_t __reserved_0 : 3;
    /*                          */
    uint_reg_t ring         : 5;
    /*           */
    uint_reg_t __reserved_1 : 18;
    /*
                                                                         
                                                                       
     */
    uint_reg_t region       : 3;
    /*           */
    uint_reg_t __reserved_2 : 6;
    /*                                                                      */
    uint_reg_t svc_dom      : 5;
    /*           */
    uint_reg_t __reserved_3 : 24;
#else   /*                */
    uint_reg_t __reserved_3 : 24;
    uint_reg_t svc_dom      : 5;
    uint_reg_t __reserved_2 : 6;
    uint_reg_t region       : 3;
    uint_reg_t __reserved_1 : 18;
    uint_reg_t ring         : 5;
    uint_reg_t __reserved_0 : 3;
#endif
  };

  uint_reg_t word;
} MPIPE_EDMA_POST_REGION_ADDR_t;

/*
                                     
                                                                        
                                                 
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*
                                                                            
                                                                             
                                                                         
                                                                           
                                                   
      
                                                                      
                                                                            
                                                                          
                                                                      
     */
    uint_reg_t ring_idx   : 16;
    /*
                                                                           
                                                                         
                                                                            
                                                                            
                                                     
      
                                                                      
                                                                            
                                                                             
                                                                            
                                                                      
     */
    uint_reg_t count      : 16;
    /*
                                                                         
                                                                            
                                                                            
                                                                           
      
                                                                           
                                                       
     */
    uint_reg_t gen        : 1;
    /*           */
    uint_reg_t __reserved : 31;
#else   /*                */
    uint_reg_t __reserved : 31;
    uint_reg_t gen        : 1;
    uint_reg_t count      : 16;
    uint_reg_t ring_idx   : 16;
#endif
  };

  uint_reg_t word;
} MPIPE_EDMA_POST_REGION_VAL_t;

/*
                                    
                                                                      
                                                                       
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*                                              */
    uint_reg_t notifring  : 8;
    /*                          */
    uint_reg_t count      : 16;
    /*                                    */
    uint_reg_t group      : 5;
    /*                              */
    uint_reg_t mode       : 3;
    /*           */
    uint_reg_t __reserved : 32;
#else   /*                */
    uint_reg_t __reserved : 32;
    uint_reg_t mode       : 3;
    uint_reg_t group      : 5;
    uint_reg_t count      : 16;
    uint_reg_t notifring  : 8;
#endif
  };

  uint_reg_t word;
} MPIPE_LBL_INIT_DAT_BSTS_TBL_t;
#endif /*                         */

#endif /*                            */
