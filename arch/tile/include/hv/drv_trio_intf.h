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

/* 
                                             
 */

#ifndef _SYS_HV_DRV_TRIO_INTF_H
#define _SYS_HV_DRV_TRIO_INTF_H

#include <arch/trio.h>

/*                                           */
#define TILERA_VENDOR_ID 0x1a41

/*                                        */
#define TILERA_GX36_DEV_ID 0x0200

/*                                                        */
#define TILERA_GX36_RC_DEV_ID 0x2000

/*                                     */
#define TILEGX_NUM_TRIO         2

/*                                               */
#define TILEGX_TRIO_PCIES       3

/*                                          */
struct pcie_port_property
{
  /*                                                                 */
  uint8_t allow_rc: 1;

  /*                                                             */
  uint8_t allow_ep: 1;

  /*                                                        */
  uint8_t allow_sio: 1;

  /*                                                                    
                                                                       */
  uint8_t allow_x1: 1;

  /*                                                                    
                                                                       */
  uint8_t allow_x2: 1;

  /*                                                                    
                                                                       */
  uint8_t allow_x4: 1;

  /*                                                                    
                                                                       */
  uint8_t allow_x8: 1;

  /*            */
  uint8_t reserved: 1;

};

/*                                                                     */
typedef enum pcie_stream_intr_config_sel_e
{
  /*                                                  */
  MEM_MAP_SEL,

  /*                                         */
  PUSH_DMA_SEL,

  /*                                         */
  PULL_DMA_SEL,
}
pcie_stream_intr_config_sel_t;


/*                                                       */
#define HV_TRIO_CONFIG_OFFSET                                        \
  ((unsigned long long)TRIO_MMIO_ADDRESS_SPACE__REGION_VAL_CFG <<   \
    TRIO_MMIO_ADDRESS_SPACE__REGION_SHIFT)

/*                                              */
#define HV_TRIO_CONFIG_SIZE                                 \
  (1ULL << TRIO_CFG_REGION_ADDR__REGION_SHIFT)

/*                                                            
                                                    
                                                    
                                                   
           
 */
#define HV_TRIO_CONFIG_IOREMAP_SIZE                            \
  ((uint64_t) 1 << TRIO_CFG_REGION_ADDR__PROT_SHIFT)

/*                                                       */
#define HV_TRIO_SQ_OFFSET(queue)                                        \
  (((unsigned long long)TRIO_MMIO_ADDRESS_SPACE__REGION_VAL_MAP_SQ <<   \
    TRIO_MMIO_ADDRESS_SPACE__REGION_SHIFT) |                            \
   ((queue) << TRIO_MAP_SQ_REGION_ADDR__SQ_SEL_SHIFT))

/*                                              */
#define HV_TRIO_SQ_SIZE                                 \
  (1ULL << TRIO_MAP_SQ_REGION_ADDR__SQ_SEL_SHIFT)


/*                                                      */
#define HV_TRIO_FIRST_PIO_REGION 8

/*                                             */
#define HV_TRIO_PIO_OFFSET(region)                           \
  (((unsigned long long)(region) + HV_TRIO_FIRST_PIO_REGION) \
   << TRIO_PIO_REGIONS_ADDR__REGION_SHIFT)

/*                                    */
#define HV_TRIO_PIO_SIZE (1ULL << TRIO_PIO_REGIONS_ADDR__ADDR_WIDTH)


/*                                                  */
#define HV_TRIO_PUSH_DMA_OFFSET(ring)                                   \
  (((unsigned long long)TRIO_MMIO_ADDRESS_SPACE__REGION_VAL_PUSH_DMA << \
    TRIO_MMIO_ADDRESS_SPACE__REGION_SHIFT) |                            \
   ((ring) << TRIO_PUSH_DMA_REGION_ADDR__RING_SEL_SHIFT))

/*                                                  */
#define HV_TRIO_PULL_DMA_OFFSET(ring)                                   \
  (((unsigned long long)TRIO_MMIO_ADDRESS_SPACE__REGION_VAL_PULL_DMA << \
    TRIO_MMIO_ADDRESS_SPACE__REGION_SHIFT) |                            \
   ((ring) << TRIO_PULL_DMA_REGION_ADDR__RING_SEL_SHIFT))

/*                                    */
#define HV_TRIO_DMA_REGION_SIZE                         \
  (1ULL << TRIO_PUSH_DMA_REGION_ADDR__RING_SEL_SHIFT)


/*                                                           */
#define HV_TRIO_MEM_MAP_INTR_OFFSET(map)                                 \
  (((unsigned long long)TRIO_MMIO_ADDRESS_SPACE__REGION_VAL_MAP_MEM <<   \
    TRIO_MMIO_ADDRESS_SPACE__REGION_SHIFT) |                            \
   ((map) << TRIO_MAP_MEM_REGION_ADDR__MAP_SEL_SHIFT))

/*                                                  */
#define HV_TRIO_MEM_MAP_INTR_SIZE                                 \
  (1ULL << TRIO_MAP_MEM_REGION_ADDR__MAP_SEL_SHIFT)


/*                                                     */
#define HV_TRIO_ALLOC_FIXED 0x01

/*                                                                    */
#define HV_TRIO_PAGE_SHIFT 12

/*                                                                    */
#define HV_TRIO_PAGE_SIZE (1ull << HV_TRIO_PAGE_SHIFT)


/*                                              */
struct pcie_trio_ports_property
{
  struct pcie_port_property ports[TILEGX_TRIO_PCIES];
};

/*                                 */
#define HV_TRIO_FLAG_TC_SHIFT 4
#define HV_TRIO_FLAG_TC_RMASK 0xf
#define HV_TRIO_FLAG_TC(N) \
  ((((N) & HV_TRIO_FLAG_TC_RMASK) + 1) << HV_TRIO_FLAG_TC_SHIFT)

/*                                     */
#define HV_TRIO_FLAG_VFUNC_SHIFT 8
#define HV_TRIO_FLAG_VFUNC_RMASK 0xff
#define HV_TRIO_FLAG_VFUNC(N) \
  ((((N) & HV_TRIO_FLAG_VFUNC_RMASK) + 1) << HV_TRIO_FLAG_VFUNC_SHIFT)


/*                                        */
#define HV_TRIO_PIO_FLAG_ORDERED (1 << 16)

/*                                                */
#define HV_TRIO_PIO_FLAG_SPACE_SHIFT 17
#define HV_TRIO_PIO_FLAG_SPACE_MASK (0x3 << HV_TRIO_PIO_FLAG_SPACE_SHIFT)
#define HV_TRIO_PIO_FLAG_CONFIG_SPACE (0x1 << HV_TRIO_PIO_FLAG_SPACE_SHIFT)
#define HV_TRIO_PIO_FLAG_IO_SPACE (0x2 << HV_TRIO_PIO_FLAG_SPACE_SHIFT)


#endif /*                         */
