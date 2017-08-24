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

#ifndef __ARCH_TRIO_H__
#define __ARCH_TRIO_H__

#include <arch/abi.h>
#include <arch/trio_def.h>

#ifndef __ASSEMBLER__

/*
                                                      
                                                                       
                                            
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*                                       */
    uint_reg_t reg_addr     : 12;
    /*                 */
    uint_reg_t fn           : 3;
    /*               */
    uint_reg_t dev          : 5;
    /*            */
    uint_reg_t bus          : 8;
    /*                                                                      */
    uint_reg_t type         : 1;
    /*           */
    uint_reg_t __reserved_0 : 1;
    /*
                                                        
                                 
     */
    uint_reg_t mac          : 2;
    /*           */
    uint_reg_t __reserved_1 : 32;
#else   /*                */
    uint_reg_t __reserved_1 : 32;
    uint_reg_t mac          : 2;
    uint_reg_t __reserved_0 : 1;
    uint_reg_t type         : 1;
    uint_reg_t bus          : 8;
    uint_reg_t dev          : 5;
    uint_reg_t fn           : 3;
    uint_reg_t reg_addr     : 12;
#endif
  };

  uint_reg_t word;
} TRIO_TILE_PIO_REGION_SETUP_CFG_ADDR_t;
#endif /*                         */

#endif /*                           */
