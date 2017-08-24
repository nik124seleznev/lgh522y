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

#ifndef __ARCH_TRIO_PCIE_RC_H__
#define __ARCH_TRIO_PCIE_RC_H__

#include <arch/abi.h>
#include <arch/trio_pcie_rc_def.h>

#ifndef __ASSEMBLER__

/*                               */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*
                                                                             
     */
    uint_reg_t mps_sup                    : 3;
    /*
                                                                           
                                                                          
                                                         
     */
    uint_reg_t phantom_function_supported : 2;
    /*                                                          */
    uint_reg_t ext_tag_field_supported    : 1;
    /*           */
    uint_reg_t __reserved_0               : 3;
    /*                                                                      */
    uint_reg_t l1_lat                     : 3;
    /*
                                                                            
                    
     */
    uint_reg_t r1                         : 1;
    /*
                                                                           
                         
     */
    uint_reg_t r2                         : 1;
    /*
                                                                           
                    
     */
    uint_reg_t r3                         : 1;
    /*
                                                                    
                                                                           
             
     */
    uint_reg_t rer                        : 1;
    /*           */
    uint_reg_t __reserved_1               : 2;
    /*                                                     */
    uint_reg_t slot_pwr_lim               : 8;
    /*                                                     */
    uint_reg_t slot_pwr_scale             : 2;
    /*           */
    uint_reg_t __reserved_2               : 4;
    /*                                                                      */
    uint_reg_t l0s_lat                    : 1;
    /*           */
    uint_reg_t __reserved_3               : 31;
#else   /*                */
    uint_reg_t __reserved_3               : 31;
    uint_reg_t l0s_lat                    : 1;
    uint_reg_t __reserved_2               : 4;
    uint_reg_t slot_pwr_scale             : 2;
    uint_reg_t slot_pwr_lim               : 8;
    uint_reg_t __reserved_1               : 2;
    uint_reg_t rer                        : 1;
    uint_reg_t r3                         : 1;
    uint_reg_t r2                         : 1;
    uint_reg_t r1                         : 1;
    uint_reg_t l1_lat                     : 3;
    uint_reg_t __reserved_0               : 3;
    uint_reg_t ext_tag_field_supported    : 1;
    uint_reg_t phantom_function_supported : 2;
    uint_reg_t mps_sup                    : 3;
#endif
  };

  uint_reg_t word;
} TRIO_PCIE_RC_DEVICE_CAP_t;

/*                          */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*                                    */
    uint_reg_t cor_err_ena      : 1;
    /*                                  */
    uint_reg_t nf_err_ena       : 1;
    /*                              */
    uint_reg_t fatal_err_ena    : 1;
    /*                                      */
    uint_reg_t ur_ena           : 1;
    /*                          */
    uint_reg_t ro_ena           : 1;
    /*                  */
    uint_reg_t max_payload_size : 3;
    /*                           */
    uint_reg_t ext_tag          : 1;
    /*                         */
    uint_reg_t ph_fn_ena        : 1;
    /*                     */
    uint_reg_t aux_pm_ena       : 1;
    /*                */
    uint_reg_t no_snoop         : 1;
    /*                       */
    uint_reg_t max_read_req_sz  : 3;
    /*           */
    uint_reg_t __reserved       : 49;
#else   /*                */
    uint_reg_t __reserved       : 49;
    uint_reg_t max_read_req_sz  : 3;
    uint_reg_t no_snoop         : 1;
    uint_reg_t aux_pm_ena       : 1;
    uint_reg_t ph_fn_ena        : 1;
    uint_reg_t ext_tag          : 1;
    uint_reg_t max_payload_size : 3;
    uint_reg_t ro_ena           : 1;
    uint_reg_t ur_ena           : 1;
    uint_reg_t fatal_err_ena    : 1;
    uint_reg_t nf_err_ena       : 1;
    uint_reg_t cor_err_ena      : 1;
#endif
  };

  uint_reg_t word;
} TRIO_PCIE_RC_DEVICE_CONTROL_t;
#endif /*                         */

#endif /*                                   */
