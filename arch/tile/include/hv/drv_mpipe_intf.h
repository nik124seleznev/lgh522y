/*
 * Copyright 2011 Tilera Corporation. All Rights Reserved.
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

#ifndef _SYS_HV_DRV_MPIPE_INTF_H
#define _SYS_HV_DRV_MPIPE_INTF_H

#include <arch/mpipe.h>
#include <arch/mpipe_constants.h>


/*                                */
#define HV_MPIPE_NUM_BUFFER_STACKS \
  (MPIPE_MMIO_INIT_DAT_GX36_1__BUFFER_STACK_MASK_WIDTH)

/*                              */
#define HV_MPIPE_NUM_NOTIF_RINGS (MPIPE_NUM_NOTIF_RINGS)

/*                              */
#define HV_MPIPE_NUM_NOTIF_GROUPS (MPIPE_NUM_NOTIF_GROUPS)

/*                            */
#define HV_MPIPE_NUM_BUCKETS (MPIPE_NUM_BUCKETS)

/*                                 */
#define HV_MPIPE_NUM_LO_BUCKETS 4096

/*                               */
#define HV_MPIPE_NUM_HI_BUCKETS \
  (HV_MPIPE_NUM_BUCKETS - HV_MPIPE_NUM_LO_BUCKETS)

/*                             */
#define HV_MPIPE_NUM_EDMA_RINGS \
  (MPIPE_MMIO_INIT_DAT_GX36_1__EDMA_POST_MASK_WIDTH)




/*                                                     */
#define HV_MPIPE_ALLOC_FIXED 0x01

/*                                              */
#define HV_MPIPE_CONFIG_MMIO_OFFSET \
  (MPIPE_MMIO_ADDR__REGION_VAL_CFG << MPIPE_MMIO_ADDR__REGION_SHIFT)

/*                                           */
#define HV_MPIPE_CONFIG_MMIO_SIZE (64 * 1024)

/*                                              */
#define HV_MPIPE_FAST_MMIO_OFFSET \
  (MPIPE_MMIO_ADDR__REGION_VAL_IDMA << MPIPE_MMIO_ADDR__REGION_SHIFT)

/*                                                                    */
#define HV_MPIPE_FAST_MMIO_SIZE \
  ((MPIPE_MMIO_ADDR__REGION_VAL_BSM + 1 - MPIPE_MMIO_ADDR__REGION_VAL_IDMA) \
   << MPIPE_MMIO_ADDR__REGION_SHIFT)


/*
                                                                    
                                                                      
                              
 */

/*                                                */
#define HV_MPIPE_ALLOC_BUFFER_STACKS_BITS \
  MPIPE_MMIO_INIT_DAT_GX36_1__BUFFER_STACK_MASK_WIDTH

/*                                              */
#define HV_MPIPE_ALLOC_BUFFER_STACKS_RES_PER_BIT \
  (HV_MPIPE_NUM_BUFFER_STACKS / HV_MPIPE_ALLOC_BUFFER_STACKS_BITS)

/*                                             */
#define HV_MPIPE_ALLOC_NOTIF_RINGS_BITS \
  MPIPE_MMIO_INIT_DAT_GX36_0__NOTIF_RING_MASK_WIDTH

/*                                           */
#define HV_MPIPE_ALLOC_NOTIF_RINGS_RES_PER_BIT \
  (HV_MPIPE_NUM_NOTIF_RINGS / HV_MPIPE_ALLOC_NOTIF_RINGS_BITS)

/*                                              */
#define HV_MPIPE_ALLOC_NOTIF_GROUPS_BITS \
  HV_MPIPE_NUM_NOTIF_GROUPS

/*                                            */
#define HV_MPIPE_ALLOC_NOTIF_GROUPS_RES_PER_BIT \
  (HV_MPIPE_NUM_NOTIF_GROUPS / HV_MPIPE_ALLOC_NOTIF_GROUPS_BITS)

/*                                             */
#define HV_MPIPE_ALLOC_LO_BUCKETS_BITS \
  MPIPE_MMIO_INIT_DAT_GX36_0__BUCKET_RELEASE_MASK_LO_WIDTH

/*                                             */
#define HV_MPIPE_ALLOC_LO_BUCKETS_RES_PER_BIT \
  (HV_MPIPE_NUM_LO_BUCKETS / HV_MPIPE_ALLOC_LO_BUCKETS_BITS)

/*                                             */
#define HV_MPIPE_ALLOC_HI_BUCKETS_BITS \
  MPIPE_MMIO_INIT_DAT_GX36_0__BUCKET_RELEASE_MASK_HI_WIDTH

/*                                           */
#define HV_MPIPE_ALLOC_HI_BUCKETS_RES_PER_BIT \
  (HV_MPIPE_NUM_HI_BUCKETS / HV_MPIPE_ALLOC_HI_BUCKETS_BITS)

/*                                             */
#define HV_MPIPE_ALLOC_EDMA_RINGS_BITS \
  MPIPE_MMIO_INIT_DAT_GX36_1__EDMA_POST_MASK_WIDTH

/*                                           */
#define HV_MPIPE_ALLOC_EDMA_RINGS_RES_PER_BIT \
  (HV_MPIPE_NUM_EDMA_RINGS / HV_MPIPE_ALLOC_EDMA_RINGS_BITS)




/*                                                            */
typedef struct
{
  /*                   */
  uint64_t ring_mask[4];

} gxio_mpipe_notif_group_bits_t;


/*                                                  */
typedef MPIPE_LBL_INIT_DAT_BSTS_TBL_t gxio_mpipe_bucket_info_t;



/*                          */
typedef struct
{
  /*              */
  uint8_t stacks[8];

} gxio_mpipe_rules_stacks_t;


/*                             */
typedef struct
{
  /*              */
  uint8_t octets[6];

} gxio_mpipe_rules_dmac_t;


/*          */
typedef uint16_t gxio_mpipe_rules_vlan_t;



/*                                               */
#define GXIO_MPIPE_LINK_NAME_LEN  32


/*                                                                   
                                                                        
                                            */
typedef struct
{
  /*                   */
  char name[GXIO_MPIPE_LINK_NAME_LEN];
}
_gxio_mpipe_link_name_t;

/*                                                 */
#define GXIO_MPIPE_SYMBOL_NAME_LEN  128


/*                                                                     
                                                                        
                                            */
typedef struct
{
  /*                   */
  char name[GXIO_MPIPE_SYMBOL_NAME_LEN];
}
_gxio_mpipe_symbol_name_t;


/*                                   */
typedef struct
{
  /*               */
  uint8_t mac[6];
}
_gxio_mpipe_link_mac_t;



/*                                                                    
                                                                       
                                                    
  
                                                                          
                                                                              
                                                                    
 */
#define GXIO_MPIPE_LINK_DATA               0x00000001UL

/*                                                       
  
                                                                          
                                                                              
                                                                    
 */
#define GXIO_MPIPE_LINK_NO_DATA            0x00000002UL

/*                                                                       
                                                                     
                                                                        
                                               
  
                                                                          
                                                                              
                                                                    
 */
#define GXIO_MPIPE_LINK_EXCL_DATA          0x00000004UL

/*                                                                           
                                                                          
                                                                         
                                
  
                                                                            
                                                                               
                                                                     
 */
#define GXIO_MPIPE_LINK_STATS              0x00000008UL

/*                                                        
  
                                                                            
                                                                               
                                                                     
 */
#define GXIO_MPIPE_LINK_NO_STATS           0x00000010UL

/*                                                                        
                                                                  
                                                                        
                                                             
                                                    
  
                                                                            
                                                                           
                                                                      
                                                                           
                                       
  
                                                                            
                                                                               
                                                                     
 */
#define GXIO_MPIPE_LINK_EXCL_STATS         0x00000020UL

/*                                                                          
                                                                   
                                                                    
                                
  
                                                                        
                                                                             
                                                                   
 */
#define GXIO_MPIPE_LINK_CTL                0x00000040UL

/*                                                          
  
                                                                        
                                                                             
                                                                   
 */
#define GXIO_MPIPE_LINK_NO_CTL             0x00000080UL

/*                                                                        
                                                                        
                                                                         
                                                                       
                        
  
                                                                            
                                                                   
  
                                                                        
                                                                             
                                                                   
 */
#define GXIO_MPIPE_LINK_EXCL_CTL           0x00000100UL

/*                                                                        
                                                                          
                                                                         
                                                          
                                                                  
                                                                           
                                                                           
 */
#define GXIO_MPIPE_LINK_AUTO_UP            0x00000200UL

/*                                                                        
                                                                            
                                                                          
                                                                         
                                                             
                                                                      
                                                                      
                                             
 */
#define GXIO_MPIPE_LINK_AUTO_UPDOWN        0x00000400UL

/*                                                                 
                                                                         
                                                                         
                                                  
                                                                  
                                                                           
                                                                           
 */
#define GXIO_MPIPE_LINK_AUTO_DOWN          0x00000800UL

/*                                                                 
                                                                     
                                                     
                                                             
                                                                      
                                                                      
                                             
 */
#define GXIO_MPIPE_LINK_AUTO_NONE          0x00001000UL

/*                                                                        
                                                                  
                                                                   
                                                                          
                                                                       
                                                                        
                                                                           
                                                                         
                                                                   
            
 */
#define GXIO_MPIPE_LINK_WAIT               0x00002000UL


/*
                                                                         
                                                   
 */

/*                                                                      
                                                                            
                                                                       
                                       */
#define GXIO_MPIPE_LINK_RECEIVE_JUMBO      0x010000

/*                                                                         
                                                                          
                                                                            
                                                             
  
                                                                        
                                                                           
                                                                          
                                                                       
                                                                          
                                                                           
                                                                    
                                                                          
                    */
#define GXIO_MPIPE_LINK_SEND_PAUSE         0x020000

/*                                                                     
                                                                         
                                                                        
                                                        */
#define GXIO_MPIPE_LINK_RECEIVE_PAUSE      0x030000

/*                                                                         
                                                                            
                                                                        
                                
  
                                                                          
                                                                           
                                                                    
                                                                          
                                                                        
                                                                      
                                                                     
                                                       
 */
#define GXIO_MPIPE_LINK_MAC                0x040000

/*                                                                         
                                                                        
                                                                           
                                                 */
#define GXIO_MPIPE_LINK_DISCARD_IF_DOWN    0x050000

/*                                                                       
                                                                           
                                                            
                                                        */
#define GXIO_MPIPE_LINK_POSSIBLE_STATE     0x060000

/*                                                                      
                                                                           
                                                                            
                                                                        
                                                                            
                                                                            
                         */
#define GXIO_MPIPE_LINK_CURRENT_STATE      0x070000

/*                                                                        
                                                                          
                                                                            
                                                                          
                                                                         
                                                                        
                                                                   
                                                                          
                                                                       
                                                                           
                             
  
                                                                         
                                                                         
                                                                      
                                                                        
                                    
 */
#define GXIO_MPIPE_LINK_DESIRED_STATE      0x080000



/*                                                      */
#define GXIO_MPIPE_LINK_10M        0x0000000000000001UL

/*                                                       */
#define GXIO_MPIPE_LINK_100M       0x0000000000000002UL

/*                                                     */
#define GXIO_MPIPE_LINK_1G         0x0000000000000004UL

/*                                                      */
#define GXIO_MPIPE_LINK_10G        0x0000000000000008UL

/*                                                      */
#define GXIO_MPIPE_LINK_20G        0x0000000000000010UL

/*                                                      */
#define GXIO_MPIPE_LINK_25G        0x0000000000000020UL

/*                                                      */
#define GXIO_MPIPE_LINK_50G        0x0000000000000040UL

/*                                                                   
                                                                 
                                                                       
                        */
#define GXIO_MPIPE_LINK_ANYSPEED   0x0000000000000800UL

/*                                                                      
                                                                        
                                                                          
                                                                        
                                                                    */
#define GXIO_MPIPE_LINK_SPEED_MASK 0x0000000000000FFFUL

/*                                                                     
                                                                   
               */
#define GXIO_MPIPE_LINK_LOOP_MAC   0x0000000000001000UL

/*                                                                     
                                                                       
              */
#define GXIO_MPIPE_LINK_LOOP_PHY   0x0000000000002000UL

/*                                                                    
                                                                    
                                                                  
                                                                   
                                                                 */
#define GXIO_MPIPE_LINK_LOOP_EXT   0x0000000000004000UL

/*                            */
#define GXIO_MPIPE_LINK_LOOP_MASK  0x000000000000F000UL

/*                                                              
                                                                  
                                                                 */
#define GXIO_MPIPE_LINK_FDX        0x0000000000010000UL

/*                                                              
                                                                  
                                                                 */
#define GXIO_MPIPE_LINK_HDX        0x0000000000020000UL


/*                      */
typedef struct
{
  /*                  */
  uint16_t size;

  /*                */
  int16_t priority;

  /*                                 */
  uint8_t headroom;

  /*                                 */
  uint8_t tailroom;

  /*                                        */
  uint16_t capacity;

  /*                                                     */
  uint16_t bucket_mask;

  /*                                                       */
  uint16_t bucket_first;

  /*                        */
  gxio_mpipe_rules_stacks_t stacks;

  /*                       */
  uint32_t channel_bits;

  /*                       */
  uint16_t num_dmacs;

  /*                       */
  uint16_t num_vlans;

  /*                              */
  uint8_t dmacs_and_vlans[];

} gxio_mpipe_rules_rule_t;


/*                              */
typedef struct
{
  /*                                             */
  uint16_t tail;

  /*                                               */
  uint16_t head;

  /*                    */
  uint8_t rules[4096 - 4];

} gxio_mpipe_rules_list_t;




/*                                                                 
                                                         */
typedef struct
{
  /*                                                    */
  uint64_t ingress_drops;
  /*                                                                      */
  uint64_t ingress_drops_no_buf;
  /*                                                                        
                       */
  uint64_t ingress_drops_ipkt;
  /*                                                                       */
  uint64_t ingress_drops_cls_lb;
  /*                                   */
  uint64_t ingress_packets;
  /*                                  */
  uint64_t egress_packets;
  /*                                 */
  uint64_t ingress_bytes;
  /*                                */
  uint64_t egress_bytes;
}
gxio_mpipe_stats_t;


#endif /*                          */
