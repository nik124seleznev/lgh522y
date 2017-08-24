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

#ifndef __ARCH_TRIO_PCIE_INTFC_H__
#define __ARCH_TRIO_PCIE_INTFC_H__

#include <arch/abi.h>
#include <arch/trio_pcie_intfc_def.h>

#ifndef __ASSEMBLER__

/*
                      
                                 
 */

__extension__
typedef union
{
  struct
  {
#ifndef __BIG_ENDIAN__
    /*                                                         */
    uint_reg_t strap_state      : 3;
    /*           */
    uint_reg_t __reserved_0     : 1;
    /*
                                                                         
                                                                      
     */
    uint_reg_t ovd_dev_type     : 1;
    /*                                                  */
    uint_reg_t ovd_dev_type_val : 4;
    /*                                 */
    uint_reg_t train_mode       : 2;
    /*           */
    uint_reg_t __reserved_1     : 1;
    /*
                                                                             
                                                                            
                                                                        
                                                                            
                                                                           
                                                
     */
    uint_reg_t rx_lane_flip     : 1;
    /*
                                                                             
                                                                            
                                                                        
                                                                            
                                                                           
                                                
     */
    uint_reg_t tx_lane_flip     : 1;
    /*
                                                                             
                 
     */
    uint_reg_t stream_width     : 2;
    /*
                                                                            
                 
     */
    uint_reg_t stream_rate      : 2;
    /*           */
    uint_reg_t __reserved_2     : 46;
#else   /*                */
    uint_reg_t __reserved_2     : 46;
    uint_reg_t stream_rate      : 2;
    uint_reg_t stream_width     : 2;
    uint_reg_t tx_lane_flip     : 1;
    uint_reg_t rx_lane_flip     : 1;
    uint_reg_t __reserved_1     : 1;
    uint_reg_t train_mode       : 2;
    uint_reg_t ovd_dev_type_val : 4;
    uint_reg_t ovd_dev_type     : 1;
    uint_reg_t __reserved_0     : 1;
    uint_reg_t strap_state      : 3;
#endif
  };

  uint_reg_t word;
} TRIO_PCIE_INTFC_PORT_CONFIG_t;

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
    uint_reg_t dl_up        : 1;
    /*
                                                                             
     */
    uint_reg_t dl_down_cnt  : 7;
    /*                                                                      */
    uint_reg_t clock_ready  : 1;
    /*           */
    uint_reg_t __reserved_0 : 7;
    /*                     */
    uint_reg_t device_rev   : 8;
    /*                    */
    uint_reg_t ltssm_state  : 6;
    /*                                     */
    uint_reg_t pm_state     : 3;
    /*           */
    uint_reg_t __reserved_1 : 31;
#else   /*                */
    uint_reg_t __reserved_1 : 31;
    uint_reg_t pm_state     : 3;
    uint_reg_t ltssm_state  : 6;
    uint_reg_t device_rev   : 8;
    uint_reg_t __reserved_0 : 7;
    uint_reg_t clock_ready  : 1;
    uint_reg_t dl_down_cnt  : 7;
    uint_reg_t dl_up        : 1;
#endif
  };

  uint_reg_t word;
} TRIO_PCIE_INTFC_PORT_STATUS_t;

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
    uint_reg_t tx0_data_ae_lvl : 7;
    /*           */
    uint_reg_t __reserved_0    : 1;
    /*                                  */
    uint_reg_t tx1_data_ae_lvl : 7;
    /*           */
    uint_reg_t __reserved_1    : 1;
    /*                                 */
    uint_reg_t tx0_data_af_lvl : 7;
    /*           */
    uint_reg_t __reserved_2    : 1;
    /*                                 */
    uint_reg_t tx1_data_af_lvl : 7;
    /*           */
    uint_reg_t __reserved_3    : 1;
    /*                                 */
    uint_reg_t tx0_info_af_lvl : 5;
    /*           */
    uint_reg_t __reserved_4    : 3;
    /*                                 */
    uint_reg_t tx1_info_af_lvl : 5;
    /*           */
    uint_reg_t __reserved_5    : 3;
    /*
                                                                       
                                                                            
                                                                             
                                                                       
                                                                            
                                                                            
                               
     */
    uint_reg_t min_np_credits  : 8;
    /*
                                                                       
                                                                             
                                                                        
                                                                       
                                                                            
                                                                            
                               
     */
    uint_reg_t min_p_credits   : 8;
#else   /*                */
    uint_reg_t min_p_credits   : 8;
    uint_reg_t min_np_credits  : 8;
    uint_reg_t __reserved_5    : 3;
    uint_reg_t tx1_info_af_lvl : 5;
    uint_reg_t __reserved_4    : 3;
    uint_reg_t tx0_info_af_lvl : 5;
    uint_reg_t __reserved_3    : 1;
    uint_reg_t tx1_data_af_lvl : 7;
    uint_reg_t __reserved_2    : 1;
    uint_reg_t tx0_data_af_lvl : 7;
    uint_reg_t __reserved_1    : 1;
    uint_reg_t tx1_data_ae_lvl : 7;
    uint_reg_t __reserved_0    : 1;
    uint_reg_t tx0_data_ae_lvl : 7;
#endif
  };

  uint_reg_t word;
} TRIO_PCIE_INTFC_TX_FIFO_CTL_t;
#endif /*                         */

#endif /*                                      */
