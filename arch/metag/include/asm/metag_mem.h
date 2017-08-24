/*
 * asm/metag_mem.h
 *
 * Copyright (C) 2000-2007, 2012 Imagination Technologies.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 *
 * Various defines for Meta (memory-mapped) registers.
 */

#ifndef _ASM_METAG_MEM_H_
#define _ASM_METAG_MEM_H_

/*                                                                            
                                                          
                                                                            */
/*
                    
                    
 */

#define LINSYSTEM_BASE  0x00200000
#define LINSYSTEM_LIMIT 0x07FFFFFF

/*                                                                         
                                                                              
 */
#define         LINSYSLFLUSH_S 0

#define     LINSYSRES0_BASE     0x00200000
#define     LINSYSRES0_LIMIT    0x01FFFFFF

#define     LINSYSCUSTOM_BASE 0x02000000
#define     LINSYSCUSTOM_LIMIT   0x02FFFFFF

#define     LINSYSEXPAND_BASE 0x03000000
#define     LINSYSEXPAND_LIMIT   0x03FFFFFF

#define     LINSYSEVENT_BASE  0x04000000
#define         LINSYSEVENT_WR_ATOMIC_UNLOCK    0x04000000
#define         LINSYSEVENT_WR_ATOMIC_LOCK      0x04000040
#define         LINSYSEVENT_WR_CACHE_DISABLE    0x04000080
#define         LINSYSEVENT_WR_CACHE_ENABLE     0x040000C0
#define         LINSYSEVENT_WR_COMBINE_FLUSH    0x04000100
#define         LINSYSEVENT_WR_FENCE            0x04000140
#define     LINSYSEVENT_LIMIT   0x04000FFF

#define     LINSYSCFLUSH_BASE   0x04400000
#define         LINSYSCFLUSH_DCACHE_LINE    0x04400000
#define         LINSYSCFLUSH_ICACHE_LINE    0x04500000
#define         LINSYSCFLUSH_MMCU           0x04700000
#ifndef METAC_1_2
#define         LINSYSCFLUSH_TxMMCU_BASE    0x04700020
#define         LINSYSCFLUSH_TxMMCU_STRIDE  0x00000008
#endif
#define         LINSYSCFLUSH_ADDR_BITS      0x000FFFFF
#define         LINSYSCFLUSH_ADDR_S         0
#define     LINSYSCFLUSH_LIMIT  0x047FFFFF

#define     LINSYSCTRL_BASE     0x04800000
#define     LINSYSCTRL_LIMIT    0x04FFFFFF

#define     LINSYSMTABLE_BASE   0x05000000
#define     LINSYSMTABLE_LIMIT  0x05FFFFFF

#define     LINSYSDIRECT_BASE   0x06000000
#define     LINSYSDIRECT_LIMIT  0x07FFFFFF

#define LINLOCAL_BASE   0x08000000
#define LINLOCAL_LIMIT  0x7FFFFFFF

#define LINCORE_BASE    0x80000000
#define LINCORE_LIMIT   0x87FFFFFF

#define LINCORE_CODE_BASE  0x80000000
#define LINCORE_CODE_LIMIT 0x81FFFFFF

#define LINCORE_DATA_BASE  0x82000000
#define LINCORE_DATA_LIMIT 0x83FFFFFF


/*                                                         */
#define LINCORE_ICACHE_BASE  0x84000000
#define LINCORE_ICACHE_LIMIT 0x85FFFFFF

/*                                                         */
#define LINCORE_DCACHE_BASE  0x86000000
#define LINCORE_DCACHE_LIMIT 0x87FFFFFF

#define LINGLOBAL_BASE  0x88000000
#define LINGLOBAL_LIMIT 0xFFFDFFFF

/*
                         
                         
 */
#define CORE_HWBASE     0x04800000
#define PRIV_HWBASE     0x04810000
#define TRIG_HWBASE     0x04820000
#define SYSC_HWBASE     0x04830000

/*                                                                            
                                                                    
                                                                            */
/*
                                                                         
                                                           
 */

#define T0KICK     0x04800800   /*                       */
#define     TXXKICK_MAX 0xFFFF  /*               */
#define     TnXKICK_STRIDE      0x00001000  /*                       */
#define     TnXKICK_STRIDE_S    12
#define T0KICKI    0x04800808   /*                       */
#define     TXIKICK_OFFSET  0x00000008  /*                        */
#define T1KICK     0x04801800   /*                       */
#define T1KICKI    0x04801808   /*                       */
#define T2KICK     0x04802800   /*                       */
#define T2KICKI    0x04802808   /*                       */
#define T3KICK     0x04803800   /*                       */
#define T3KICKI    0x04803808   /*                       */

/*                                                                            
                                                  
                                                                            */
/*
                                                                       
                                                                          
                                                                      
  
                                                                       
                                                                       
                                                                              
                                                                       
                                                                           
                                                                             
          
 */

#define T0UCTREG0   0x04800000  /*                       */
#define TnUCTRX_STRIDE      0x00001000  /*                       */
#define TXUCTREGn_STRIDE    0x00000008  /*                       */

#define TXUXXRXDT  0x0480FFF0   /*                              */
#define TXUXXRXRQ  0x0480FFF8
#define     TXUXXRXRQ_DREADY_BIT 0x80000000  /*               */
#define     TXUXXRXRQ_DSPEXT_BIT 0x00020000  /*               */
#define     TXUXXRXRQ_RDnWR_BIT  0x00010000  /*               */
#define     TXUXXRXRQ_TX_BITS    0x00003000  /*               */
#define     TXUXXRXRQ_TX_S       12
#define     TXUXXRXRQ_RX_BITS    0x000001F0  /*               */
#define     TXUXXRXRQ_RX_S       4
#define         TXUXXRXRQ_DSPRARD0    0      /*                          */
#define         TXUXXRXRQ_DSPRARD1    1      /*                          */
#define         TXUXXRXRQ_DSPRAWR0    2      /*                           */
#define         TXUXXRXRQ_DSPRAWR2    3      /*                           */
#define         TXUXXRXRQ_DSPRBRD0    4      /*                          */
#define         TXUXXRXRQ_DSPRBRD1    5      /*                          */
#define         TXUXXRXRQ_DSPRBWR0    6      /*                           */
#define         TXUXXRXRQ_DSPRBWR1    7      /*                           */
#define         TXUXXRXRQ_DSPRARINC0  8      /*                            */
#define         TXUXXRXRQ_DSPRARINC1  9      /*                            */
#define         TXUXXRXRQ_DSPRAWINC0 10      /*                             */
#define         TXUXXRXRQ_DSPRAWINC1 11      /*                             */
#define         TXUXXRXRQ_DSPRBRINC0 12      /*                            */
#define         TXUXXRXRQ_DSPRBRINC1 13      /*                            */
#define         TXUXXRXRQ_DSPRBWINC0 14      /*                             */
#define         TXUXXRXRQ_DSPRBWINC1 15      /*                             */

#define         TXUXXRXRQ_ACC0L0     16      /*                              */
#define         TXUXXRXRQ_ACC1L0     17      /*                              */
#define         TXUXXRXRQ_ACC2L0     18      /*                              */
#define         TXUXXRXRQ_ACC3L0     19      /*                              */
#define         TXUXXRXRQ_ACC0HI     20      /*                          */
#define         TXUXXRXRQ_ACC1HI     21      /*                          */
#define         TXUXXRXRQ_ACC2HI     22      /*                          */
#define         TXUXXRXRQ_ACC3HI     23      /*                          */
#define     TXUXXRXRQ_UXX_BITS   0x0000000F  /*               */
#define     TXUXXRXRQ_UXX_S      0

/*                                                                            
                                                                
                                                                            */
/*
                                                                         
                                                                        
                                                                
 */

#define TXPRIVREG_STRIDE    0x8 /*                               */
#define TXPRIVREG_STRIDE_S  3

/*
                                                                          
                                                 
 */
#define T0PIOREG    0x04810100
#define T1PIOREG    0x04810108
#define T2PIOREG    0x04810110
#define T3PIOREG    0x04810118

/*
                                                                 
                                                                   
                                  
 */
#define T0PSYREG    0x04810180
#define T1PSYREG    0x04810188
#define T2PSYREG    0x04810190
#define T3PSYREG    0x04810198

/*
                     
                     
 */

/*                                                            
                                                               */
#define     TXPIOREG_ADDR_BITS  0x1F0000 /*                    */
#define     TXPIOREG_ADDR_S     16

/*                                       */
#define     TXPIOREG_CORE_BIT       (1<<((0x04800000>>16)&0x1F))
#define     TXPIOREG_PRIV_BIT       (1<<((0x04810000>>16)&0x1F))
#define     TXPIOREG_TRIG_BIT       (1<<((0x04820000>>16)&0x1F))
#define     TXPIOREG_SYSC_BIT       (1<<((0x04830000>>16)&0x1F))

#define     TXPIOREG_WRC_BIT          0x00080000  /*                      */
#define     TXPIOREG_LOCALBUS_RW_BIT  0x00040000  /*                      */
#define     TXPIOREG_SYSREGBUS_RD_BIT 0x00020000  /*                        */
#define     TXPIOREG_SYSREGBUS_WR_BIT 0x00010000  /*                       */

/*                                */
#define T0PRIVCORE 0x04800828
#define         TXPRIVCORE_TXBKICK_BIT   0x001  /*                      */
#define         TXPRIVCORE_TXIKICK_BIT   0x002  /*                      */
#define         TXPRIVCORE_TXAMAREGX_BIT 0x004  /*                      */
#define TnPRIVCORE_STRIDE 0x00001000

#define T0PRIVSYSR 0x04810000
#define     TnPRIVSYSR_STRIDE   0x00000008
#define     TnPRIVSYSR_STRIDE_S 3
#define     TXPRIVSYSR_CFLUSH_BIT     0x01
#define     TXPRIVSYSR_MTABLE_BIT     0x02
#define     TXPRIVSYSR_DIRECT_BIT     0x04
#ifdef METAC_1_2
#define     TXPRIVSYSR_ALL_BITS       0x07
#else
#define     TXPRIVSYSR_CORE_BIT       0x08
#define     TXPRIVSYSR_CORECODE_BIT   0x10
#define     TXPRIVSYSR_ALL_BITS       0x1F
#endif
#define T1PRIVSYSR 0x04810008
#define T2PRIVSYSR 0x04810010
#define T3PRIVSYSR 0x04810018

/*                                                                            
                                                                     
                                                                            */
/*
                                                                         
                                                                            
                      
 */

#define HWSTATMETA  0x04820000  /*                                 */
#define         HWSTATMETA_T0HALT_BITS 0xF
#define         HWSTATMETA_T0HALT_S    0
#define     HWSTATMETA_T0BHALT_BIT 0x1  /*                 */
#define     HWSTATMETA_T0IHALT_BIT 0x2  /*                 */
#define     HWSTATMETA_T0PHALT_BIT 0x4  /*                   */
#define     HWSTATMETA_T0AMATR_BIT 0x8  /*             */
#define     HWSTATMETA_TnINT_S     4    /*                     */
#define HWSTATEXT   0x04820010  /*                                       */
#define HWSTATEXT2  0x04820018  /*                                       */
#define HWSTATEXT4  0x04820020  /*                                       */
#define HWSTATEXT6  0x04820028  /*                                        */
#define HWLEVELEXT  0x04820030  /*                                         */
#define HWLEVELEXT2 0x04820038  /*                                         */
#define HWLEVELEXT4 0x04820040  /*                                         */
#define HWLEVELEXT6 0x04820048  /*                                          */
#define     HWLEVELEXT_XXX_LEVEL 1  /*                                 */
#define     HWLEVELEXT_XXX_EDGE  0
#define HWMASKEXT   0x04820050  /*                                        */
#define HWMASKEXT2  0x04820058  /*                                        */
#define HWMASKEXT4  0x04820060  /*                                        */
#define HWMASKEXT6  0x04820068  /*                                         */
#define T0VECINT_BHALT  0x04820500  /*                                */
#define     TXVECXXX_BITS   0xF       /*                                  */
#define     TXVECXXX_S  0
#define T0VECINT_IHALT  0x04820508  /*                */
#define T0VECINT_PHALT  0x04820510  /*                    */
#define T0VECINT_AMATR  0x04820518  /*             */
#define     TnVECINT_STRIDE 0x00000020  /*                   */
#define HWVEC0EXT   0x04820700  /*                                     */
#define HWVEC20EXT  0x04821700  /*                                     */
#define HWVEC40EXT  0x04822700  /*                                     */
#define HWVEC60EXT  0x04823700  /*                                      */
#define     HWVECnEXT_STRIDE 0x00000008 /*                    */
#define HWVECnEXT_DEBUG 0x1         /*                               */

/*
                                          
                                          
 */
#define CODEB0ADDR         0x0480FF00  /*                   */
#define     CODEBXADDR_MATCHX_BITS 0xFFFFFFFC
#define     CODEBXADDR_MATCHX_S    2
#define CODEB0CTRL         0x0480FF08  /*         */
#define     CODEBXCTRL_MATEN_BIT   0x80000000   /*                 */
#define     CODEBXCTRL_MATTXEN_BIT 0x10000000   /*                      */
#define     CODEBXCTRL_HITC_BITS   0x00FF0000   /*               */
#define     CODEBXCTRL_HITC_S      16
#define           CODEBXHITC_NEXT  0xFF     /*                         */
#define           CODEBXHITC_HIT1  0x00     /*                         */
#define     CODEBXCTRL_MMASK_BITS  0x0000FFFC   /*                      */
#define     CODEBXCTRL_MMASK_S     2
#define     CODEBXCTRL_MATLTX_BITS 0x00000003   /*                          */
#define     CODEBXCTRL_MATLTX_S    0            /*                          */
#define CODEBnXXXX_STRIDE      0x00000010  /*                               */
#define CODEBnXXXX_STRIDE_S    4
#define CODEBnXXXX_LIMIT       3           /*          */

/*
                                        
                                        
 */
#define DATAW0ADDR         0x0480FF40  /*                   */
#define     DATAWXADDR_MATCHR_BITS 0xFFFFFFF8
#define     DATAWXADDR_MATCHR_S    3
#define     DATAWXADDR_MATCHW_BITS 0xFFFFFFFF
#define     DATAWXADDR_MATCHW_S    0
#define DATAW0CTRL         0x0480FF48  /*         */
#define     DATAWXCTRL_MATRD_BIT   0x80000000   /*               */
#ifndef METAC_1_2
#define     DATAWXCTRL_MATNOTTX_BIT 0x20000000  /*                       */
#endif
#define     DATAWXCTRL_MATWR_BIT   0x40000000   /*               */
#define     DATAWXCTRL_MATTXEN_BIT 0x10000000   /*                      */
#define     DATAWXCTRL_WRSIZE_BITS 0x0F000000   /*                  */
#define     DATAWXCTRL_WRSIZE_S    24
#define         DATAWWRSIZE_ANY   0         /*                              */
#define         DATAWWRSIZE_8BIT  1     /*                    */
#define         DATAWWRSIZE_16BIT 2
#define         DATAWWRSIZE_32BIT 3
#define         DATAWWRSIZE_64BIT 4
#define     DATAWXCTRL_HITC_BITS   0x00FF0000   /*               */
#define     DATAWXCTRL_HITC_S      16
#define           DATAWXHITC_NEXT  0xFF     /*                         */
#define           DATAWXHITC_HIT1  0x00     /*                         */
#define     DATAWXCTRL_MMASK_BITS 0x0000FFF8    /*                      */
#define     DATAWXCTRL_MMASK_S    3
#define     DATAWXCTRL_MATLTX_BITS 0x00000003   /*                          */
#define     DATAWXCTRL_MATLTX_S    0            /*                          */
#define DATAW0DMATCH0       0x0480FF50 /*                  */
#define DATAW0DMATCH1       0x0480FF58
#define DATAW0DMASK0        0x0480FF60 /*                       */
#define DATAW0DMASK1        0x0480FF68
#define DATAWnXXXX_STRIDE      0x00000040  /*                               */
#define DATAWnXXXX_STRIDE_S    6
#define DATAWnXXXX_LIMIT       1           /*          */

/*
                                                   
                                                   
 */

/*                                  */
#define T0AMAREG4   0x04800810
#define     TXAMAREG4_POOLSIZE_BITS 0x3FFFFF00
#define     TXAMAREG4_POOLSIZE_S    8
#define     TXAMAREG4_AVALUE_BITS   0x000000FF
#define     TXAMAREG4_AVALUE_S  0
#define T0AMAREG5   0x04800818
#define     TXAMAREG5_POOLC_BITS    0x07FFFFFF
#define         TXAMAREG5_POOLC_S       0
#define T0AMAREG6   0x04800820
#define     TXAMAREG6_DLINEDEF_BITS 0x00FFFFF0
#define         TXAMAREG6_DLINEDEF_S    0
#define TnAMAREGX_STRIDE    0x00001000

/*
                                               
                                               
 */
#define MMCU_ENTRY_S         4            /*                              */
#define MMCU_ENTRY_ADDR_BITS 0xFFFFF000   /*                              */
#define MMCU_ENTRY_ADDR_S    12           /*                              */
#define MMCU_ENTRY_CWIN_BITS 0x000000C0   /*                              */
#define MMCU_ENTRY_CWIN_S    6
#define     MMCU_CWIN_UNCACHED  0 /*                         */
#define     MMCU_CWIN_BURST     1 /*                      */
#define     MMCU_CWIN_C1SET     2 /*                      */
#define     MMCU_CWIN_CACHED    3 /*                         */
#define MMCU_ENTRY_CACHE_BIT 0x00000080   /*                               */
#define     MMCU_ECACHE1_FULL_BIT  0x00000040 /*                  */
#define     MMCU_ECACHE0_BURST_BIT 0x00000040 /*                  */
#define MMCU_ENTRY_SYS_BIT   0x00000010   /*                               */
#define MMCU_ENTRY_WRC_BIT   0x00000008   /*                               */
#define MMCU_ENTRY_PRIV_BIT  0x00000004   /*                               */
#define MMCU_ENTRY_WR_BIT    0x00000002   /*                               */
#define MMCU_ENTRY_VAL_BIT   0x00000001   /*                               */

#ifdef METAC_2_1
/*
                                                                           
                                                                     
 */
#define MMCU_E1ENT_ADDR_BITS 0xFFFFFFC0   /*                              */
#define MMCU_E1ENT_ADDR_S    6            /*                              */
#define MMCU_E1ENT_PGSZ_BITS 0x0000001E   /*                              */
#define MMCU_E1ENT_PGSZ_S    1
#define     MMCU_E1ENT_PGSZ0_POWER   12   /*               */
#define     MMCU_E1ENT_PGSZ_MAX      10   /*                       */
#define MMCU_E1ENT_MINIM_BIT 0x00000020
#endif /*           */

/*                                      */
#define MMCU_TABLE_PHYS_ADDR        0x04830010
#define     MMCU_TABLE_PHYS_ADDR_BITS   0xFFFFFFFC
#ifdef METAC_2_1
#define     MMCU_TABLE_PHYS_EXTEND      0x00000001     /*           */
#endif
#define MMCU_DCACHE_CTRL_ADDR       0x04830018
#define     MMCU_xCACHE_CTRL_ENABLE_BIT     0x00000001
#define     MMCU_xCACHE_CTRL_PARTITION_BIT  0x00000000 /*                  */
#define MMCU_ICACHE_CTRL_ADDR       0x04830020

#ifdef METAC_2_1

/*
                                                                      
  
                                                                       
                                                         
 */
#define LINSYSMEMT0L_BASE   0x05000000
#define LINSYSMEMT0L_LIMIT  0x051FFFFF
#define     LINSYSMEMTnX_STRIDE     0x00200000  /*                       */
#define     LINSYSMEMTnX_STRIDE_S   21
#define     LINSYSMEMTXG_OFFSET     0x00800000  /*                        */
#define     LINSYSMEMTXG_OFFSET_S   23
#define LINSYSMEMT1L_BASE   0x05200000
#define LINSYSMEMT1L_LIMIT  0x053FFFFF
#define LINSYSMEMT2L_BASE   0x05400000
#define LINSYSMEMT2L_LIMIT  0x055FFFFF
#define LINSYSMEMT3L_BASE   0x05600000
#define LINSYSMEMT3L_LIMIT  0x057FFFFF
#define LINSYSMEMT0G_BASE   0x05800000
#define LINSYSMEMT0G_LIMIT  0x059FFFFF
#define LINSYSMEMT1G_BASE   0x05A00000
#define LINSYSMEMT1G_LIMIT  0x05BFFFFF
#define LINSYSMEMT2G_BASE   0x05C00000
#define LINSYSMEMT2G_LIMIT  0x05DFFFFF
#define LINSYSMEMT3G_BASE   0x05E00000
#define LINSYSMEMT3G_LIMIT  0x05FFFFFF

/*
                                                                       
                                               
 */
#define MMCU_T0LOCAL_TABLE_PHYS0    0x04830700
#define   MMCU_TnX_TABLE_PHYSX_STRIDE    0x20   /*                   */
#define   MMCU_TnX_TABLE_PHYSX_STRIDE_S  5
#define   MMCU_TXG_TABLE_PHYSX_OFFSET    0x10   /*                     */
#define   MMCU_TXG_TABLE_PHYSX_OFFSET_S  4
#define     MMCU_TBLPHYS0_DCCTRL_BITS       0x000000DF  /*              */
#define     MMCU_TBLPHYS0_ENTLB_BIT         0x00000020  /*              */
#define     MMCU_TBLPHYS0_TBLSZ_BITS        0x00000F00  /*                */
#define     MMCU_TBLPHYS0_TBLSZ_S           8
#define         MMCU_TBLPHYS0_TBLSZ0_POWER      22  /*         */
#define         MMCU_TBLPHYS0_TBLSZ_MAX         9   /*         */
#define     MMCU_TBLPHYS0_LINBASE_BITS      0xFFC00000  /*             */
#define     MMCU_TBLPHYS0_LINBASE_S         22

#define MMCU_T0LOCAL_TABLE_PHYS1    0x04830708
#define     MMCU_TBLPHYS1_ADDR_BITS         0xFFFFFFFC  /*               */
#define     MMCU_TBLPHYS1_ADDR_S            2

#define MMCU_T0GLOBAL_TABLE_PHYS0   0x04830710
#define MMCU_T0GLOBAL_TABLE_PHYS1   0x04830718
#define MMCU_T1LOCAL_TABLE_PHYS0    0x04830720
#define MMCU_T1LOCAL_TABLE_PHYS1    0x04830728
#define MMCU_T1GLOBAL_TABLE_PHYS0   0x04830730
#define MMCU_T1GLOBAL_TABLE_PHYS1   0x04830738
#define MMCU_T2LOCAL_TABLE_PHYS0    0x04830740
#define MMCU_T2LOCAL_TABLE_PHYS1    0x04830748
#define MMCU_T2GLOBAL_TABLE_PHYS0   0x04830750
#define MMCU_T2GLOBAL_TABLE_PHYS1   0x04830758
#define MMCU_T3LOCAL_TABLE_PHYS0    0x04830760
#define MMCU_T3LOCAL_TABLE_PHYS1    0x04830768
#define MMCU_T3GLOBAL_TABLE_PHYS0   0x04830770
#define MMCU_T3GLOBAL_TABLE_PHYS1   0x04830778

#define MMCU_T0EBWCCTRL             0x04830640
#define     MMCU_TnEBWCCTRL_BITS    0x00000007
#define     MMCU_TnEBWCCTRL_S       0
#define         MMCU_TnEBWCCCTRL_DISABLE_ALL 0
#define         MMCU_TnEBWCCCTRL_ABIT25      1
#define         MMCU_TnEBWCCCTRL_ABIT26      2
#define         MMCU_TnEBWCCCTRL_ABIT27      3
#define         MMCU_TnEBWCCCTRL_ABIT28      4
#define         MMCU_TnEBWCCCTRL_ABIT29      5
#define         MMCU_TnEBWCCCTRL_ABIT30      6
#define         MMCU_TnEBWCCCTRL_ENABLE_ALL  7
#define MMCU_TnEBWCCTRL_STRIDE      8

#endif /*           */


/*                                           */
#define METAC_ID                0x04830000
#define     METAC_ID_MAJOR_BITS     0xFF000000
#define     METAC_ID_MAJOR_S        24
#define     METAC_ID_MINOR_BITS     0x00FF0000
#define     METAC_ID_MINOR_S        16
#define     METAC_ID_REV_BITS       0x0000FF00
#define     METAC_ID_REV_S          8
#define     METAC_ID_MAINT_BITS     0x000000FF
#define     METAC_ID_MAINT_S        0

#ifdef METAC_2_1
/*                                            */
#define METAC_ID2               0x04830008
#define     METAC_ID2_DESIGNER_BITS 0xFFFF0000  /*                      */
#define     METAC_ID2_DESIGNER_S    16
#define     METAC_ID2_MINOR2_BITS   0x00000F00  /*                       */
#define     METAC_ID2_MINOR2_S      8
#define     METAC_ID2_CONFIG_BITS   0x000000FF  /*                       */
#define     METAC_ID2_CONFIG_S      0

/*                                                           */
#define METAC_CORE_ID           0x04831000
#define     METAC_COREID_GROUP_BITS   0xFF000000
#define     METAC_COREID_GROUP_S      24
#define         METAC_COREID_GROUP_METAG  0x14
#define     METAC_COREID_ID_BITS      0x00FF0000
#define     METAC_COREID_ID_S         16
#define         METAC_COREID_ID_W32       0x10   /*                        */
#define     METAC_COREID_CONFIG_BITS  0x0000FFFF
#define     METAC_COREID_CONFIG_S     0
#define       METAC_COREID_CFGCACHE_BITS    0x0007
#define       METAC_COREID_CFGCACHE_S       0
#define           METAC_COREID_CFGCACHE_NOM       0
#define           METAC_COREID_CFGCACHE_TYPE0     1
#define           METAC_COREID_CFGCACHE_NOMMU     1 /*                 */
#define           METAC_COREID_CFGCACHE_NOCACHE   2
#define           METAC_COREID_CFGCACHE_PRIVNOMMU 3
#define       METAC_COREID_CFGDSP_BITS      0x0038
#define       METAC_COREID_CFGDSP_S         3
#define           METAC_COREID_CFGDSP_NOM       0
#define           METAC_COREID_CFGDSP_MIN       1
#define       METAC_COREID_NOFPACC_BIT      0x0040 /*                     */
#define       METAC_COREID_CFGFPU_BITS      0x0180
#define       METAC_COREID_CFGFPU_S         7
#define           METAC_COREID_CFGFPU_NOM       0
#define           METAC_COREID_CFGFPU_SNGL      1
#define           METAC_COREID_CFGFPU_DBL       2
#define       METAC_COREID_NOAMA_BIT        0x0200 /*                       */
#define       METAC_COREID_NOCOH_BIT        0x0400 /*                         */

/*                           */
#define METAC_CORE_REV          0x04831008
#define     METAC_COREREV_DESIGN_BITS   0xFF000000
#define     METAC_COREREV_DESIGN_S      24
#define     METAC_COREREV_MAJOR_BITS    0x00FF0000
#define     METAC_COREREV_MAJOR_S       16
#define     METAC_COREREV_MINOR_BITS    0x0000FF00
#define     METAC_COREREV_MINOR_S       8
#define     METAC_COREREV_MAINT_BITS    0x000000FF
#define     METAC_COREREV_MAINT_S       0

/*                                                    */
#define METAC_CORE_DESIGNER1    0x04831010      /*                 */
#define METAC_CORE_DESIGNER2    0x04831018      /*                 */

/*                                                                        */
#define METAC_CORE_CONFIG2      0x04831020
#define     METAC_CORECFG2_COREDBGTYPE_BITS 0x60000000   /*                 */
#define     METAC_CORECFG2_COREDBGTYPE_S    29
#define     METAC_CORECFG2_DCSMALL_BIT      0x04000000   /*                  */
#define     METAC_CORECFG2_ICSMALL_BIT      0x02000000   /*                  */
#define     METAC_CORECFG2_DCSZNP_BITS      0x01C00000   /*                    */
#define     METAC_CORECFG2_DCSZNP_S         22
#define     METAC_CORECFG2_ICSZNP_BITS      0x00380000  /*                    */
#define     METAC_CORECFG2_ICSZNP_S         19
#define     METAC_CORECFG2_DCSZ_BITS        0x00070000   /*                 */
#define     METAC_CORECFG2_DCSZ_S           16
#define         METAC_CORECFG2_xCSZ_4K          0        /*                  */
#define         METAC_CORECFG2_xCSZ_8K          1
#define         METAC_CORECFG2_xCSZ_16K         2
#define         METAC_CORECFG2_xCSZ_32K         3
#define         METAC_CORECFG2_xCSZ_64K         4
#define     METAC_CORE_C2ICSZ_BITS          0x0000E000   /*                 */
#define     METAC_CORE_C2ICSZ_S             13
#define     METAC_CORE_GBLACC_BITS          0x00001800   /*                      */
#define     METAC_CORE_GBLACC_S             11
#define     METAC_CORE_GBLDXR_BITS          0x00000700   /*                      */
#define     METAC_CORE_GBLDXR_S             8
#define     METAC_CORE_GBLAXR_BITS          0x000000E0   /*                      */
#define     METAC_CORE_GBLAXR_S             5
#define     METAC_CORE_RTTRACE_BIT          0x00000010
#define     METAC_CORE_WATCHN_BITS          0x0000000C   /*                  */
#define     METAC_CORE_WATCHN_S             2
#define     METAC_CORE_BREAKN_BITS          0x00000003   /*                  */
#define     METAC_CORE_BREAKN_S             0

/*                                                                        */
#define METAC_CORE_CONFIG3      0x04831028
#define     METAC_CORECFG3_L2C_REV_ID_BITS          0x000F0000   /*                      */
#define     METAC_CORECFG3_L2C_REV_ID_S             16
#define     METAC_CORECFG3_L2C_LINE_SIZE_BITS       0x00003000   /*              */
#define     METAC_CORECFG3_L2C_LINE_SIZE_S          12
#define         METAC_CORECFG3_L2C_LINE_SIZE_64B    0x0          /*          */
#define     METAC_CORECFG3_L2C_NUM_WAYS_BITS        0x00000F00   /*                         */
#define     METAC_CORECFG3_L2C_NUM_WAYS_S           8
#define     METAC_CORECFG3_L2C_SIZE_BITS            0x000000F0   /*               */
#define     METAC_CORECFG3_L2C_SIZE_S               4
#define     METAC_CORECFG3_L2C_UNIFIED_BIT          0x00000004   /*                */
#define     METAC_CORECFG3_L2C_UNIFIED_S            2
#define       METAC_CORECFG3_L2C_UNIFIED_UNIFIED    1            /*                     */
#define       METAC_CORECFG3_L2C_UNIFIED_SEPARATE   0            /*                      */
#define     METAC_CORECFG3_L2C_MODE_BIT             0x00000002   /*             */
#define     METAC_CORECFG3_L2C_MODE_S               1
#define       METAC_CORECFG3_L2C_MODE_WRITE_BACK    1            /*              */
#define       METAC_CORECFG3_L2C_MODE_WRITE_THROUGH 0            /*                 */
#define     METAC_CORECFG3_L2C_HAVE_L2C_BIT         0x00000001   /*          */
#define     METAC_CORECFG3_L2C_HAVE_L2C_S           0

#endif /*           */

#define SYSC_CACHE_MMU_CONFIG       0x04830028
#ifdef METAC_2_1
#define     SYSC_CMMUCFG_DCSKEWABLE_BIT 0x00000040
#define     SYSC_CMMUCFG_ICSKEWABLE_BIT 0x00000020
#define     SYSC_CMMUCFG_DCSKEWOFF_BIT  0x00000010  /*                            */
#define     SYSC_CMMUCFG_ICSKEWOFF_BIT  0x00000008  /*                            */
#define     SYSC_CMMUCFG_MODE_BITS      0x00000007  /*                     */
#define     SYSC_CMMUCFG_MODE_S         0
#define         SYSC_CMMUCFG_ON             0x7
#define         SYSC_CMMUCFG_EBYPASS        0x6   /*                       */
#define         SYSC_CMMUCFG_EBYPASSIC      0x4   /*                    */
#define         SYSC_CMMUCFG_EBYPASSDC      0x2   /*                    */
#endif /*           */
/*                                    */
#define         SYSC_CMMUCFG_MMU_ON_BIT     0x1
#define         SYSC_CMMUCFG_DC_ON_BIT      0x2
#define         SYSC_CMMUCFG_IC_ON_BIT      0x4

#define SYSC_JTAG_THREAD            0x04830030
#define     SYSC_JTAG_TX_BITS           0x00000003 /*                 */
#define     SYSC_JTAG_TX_S              0
#define     SYSC_JTAG_PRIV_BIT          0x00000004
#ifdef METAC_2_1
#define     SYSC_JTAG_SLAVETX_BITS      0x00000018
#define     SYSC_JTAG_SLAVETX_S         3
#endif /*           */

#define SYSC_DCACHE_FLUSH           0x04830038
#define SYSC_ICACHE_FLUSH           0x04830040
#define  SYSC_xCACHE_FLUSH_INIT     0x1
#define MMCU_DIRECTMAP0_ADDR        0x04830080 /*                      */
#define     MMCU_DIRECTMAPn_STRIDE      0x00000010 /*                   */
#define     MMCU_DIRECTMAPn_S           4
#define         MMCU_DIRECTMAPn_ADDR_BITS       0xFF800000
#define         MMCU_DIRECTMAPn_ADDR_S          23
#define         MMCU_DIRECTMAPn_ADDR_SCALE      0x00800000 /*            */
#ifdef METAC_2_1
/*
                                                                  
                                                                         
                                                                        
 */
#define         MMCU_DIRECTMAPn_ENHANCE_BIT     0x00000001 /*              */
#define         MMCU_DIRECTMAPn_DCCTRL_BITS     0x000000DF /*             */
#define         MMCU_DIRECTMAPn_DCCTRL_S        0
#define         MMCU_DIRECTMAPn_ICCTRL_BITS     0x0000C000 /*             */
#define         MMCU_DIRECTMAPn_ICCTRL_S        8
#define         MMCU_DIRECTMAPn_ENTLB_BIT       0x00000020 /*              */
#define         MMCU_DIRECTMAPn_ICCWIN_BITS     0x0000C000 /*                 */
#define         MMCU_DIRECTMAPn_ICCWIN_S        14
#endif /*           */

#define MMCU_DIRECTMAP1_ADDR        0x04830090
#define MMCU_DIRECTMAP2_ADDR        0x048300a0
#define MMCU_DIRECTMAP3_ADDR        0x048300b0

/*
                                                                         
                                                                       
                
 */
#define SYSC_DCPART0 0x04830200
#define     SYSC_xCPARTn_STRIDE   0x00000008
#define     SYSC_xCPARTL_AND_BITS 0x0000000F /*                  */
#define     SYSC_xCPARTL_AND_S    0
#define     SYSC_xCPARTG_AND_BITS 0x00000F00 /*                  */
#define     SYSC_xCPARTG_AND_S    8
#define     SYSC_xCPARTL_OR_BITS  0x000F0000 /*                     */
#define     SYSC_xCPARTL_OR_S     16
#ifdef METAC_2_1
#define     SYSC_DCPART_GCON_BIT  0x00100000 /*                       */
#endif /*           */
#define     SYSC_xCPARTG_OR_BITS  0x0F000000 /*                     */
#define     SYSC_xCPARTG_OR_S     24
#define     SYSC_CWRMODE_BIT      0x80000000 /*                      */

#define SYSC_DCPART1 0x04830208
#define SYSC_DCPART2 0x04830210
#define SYSC_DCPART3 0x04830218
#define SYSC_ICPART0 0x04830220
#define SYSC_ICPART1 0x04830228
#define SYSC_ICPART2 0x04830230
#define SYSC_ICPART3 0x04830238

/*
                                              
 */
#define SYSC_MCMDATAX  0x04830300   /*                                 */
#define SYSC_MCMDATAT  0x04830308   /*                                  */
#define SYSC_MCMGCTRL  0x04830310   /*                  */
#define     SYSC_MCMGCTRL_READ_BIT  0x00000001 /*                       */
#define     SYSC_MCMGCTRL_AINC_BIT  0x00000002 /*                        */
#define     SYSC_MCMGCTRL_ADDR_BITS 0x000FFFFC /*                  */
#define     SYSC_MCMGCTRL_ADDR_S    2
#define     SYSC_MCMGCTRL_ID_BITS   0x0FF00000 /*                          */
#define     SYSC_MCMGCTRL_ID_S      20
#define         SYSC_MCMGID_NODEV       0xFF /*                    */
#define         SYSC_MCMGID_DSPRAM0A    0x04 /*                           */
#define         SYSC_MCMGID_DSPRAM0B    0x05 /*                           */
#define         SYSC_MCMGID_DSPRAM1A    0x06 /*                           */
#define         SYSC_MCMGID_DSPRAM1B    0x07 /*                           */
#define         SYSC_MCMGID_DCACHEL     0x08 /*                              */
#ifdef METAC_2_1
#define         SYSC_MCMGID_DCACHETLB   0x09 /*                              */
#endif /*           */
#define         SYSC_MCMGID_DCACHET     0x0A /*                              */
#define         SYSC_MCMGID_DCACHELRU   0x0B /*                              */
#define         SYSC_MCMGID_ICACHEL     0x0C /*                              */
#ifdef METAC_2_1
#define         SYSC_MCMGID_ICACHETLB   0x0D /*                             */
#endif /*           */
#define         SYSC_MCMGID_ICACHET     0x0E /*                              */
#define         SYSC_MCMGID_ICACHELRU   0x0F /*                              */
#define         SYSC_MCMGID_COREIRAM0   0x10 /*                    */
#define         SYSC_MCMGID_COREIRAMn   0x17
#define         SYSC_MCMGID_COREDRAM0   0x18 /*                    */
#define         SYSC_MCMGID_COREDRAMn   0x1F
#ifdef METAC_2_1
#define         SYSC_MCMGID_DCACHEST    0x20 /*                              */
#define         SYSC_MCMGID_ICACHEST    0x21 /*                              */
#define         SYSC_MCMGID_DCACHETLBLRU 0x22 /*                             */
#define         SYSC_MCMGID_ICACHETLBLRU 0x23 /*                             */
#define         SYSC_MCMGID_DCACHESTLRU 0x24 /*                              */
#define         SYSC_MCMGID_ICACHESTLRU 0x25 /*                              */
#define         SYSC_MCMGID_DEBUGTLB    0x26 /*                              */
#define         SYSC_MCMGID_DEBUGST     0x27 /*                              */
#define         SYSC_MCMGID_L2CACHEL    0x30 /*                                */
#define         SYSC_MCMGID_L2CACHET    0x31 /*                              */
#define         SYSC_MCMGID_COPROX0     0x70 /*                       */
#define         SYSC_MCMGID_COPROXn     0x77
#endif /*           */
#define     SYSC_MCMGCTRL_TR31_BIT  0x80000000 /*                          */
#define SYSC_MCMSTATUS 0x04830318   /*                  */
#define     SYSC_MCMSTATUS_IDLE_BIT 0x00000001

/*                    */
#define SYSC_SYS_EVENT            0x04830400
#define     SYSC_SYSEVT_ATOMIC_BIT      0x00000001
#define     SYSC_SYSEVT_CACHEX_BIT      0x00000002
#define SYSC_ATOMIC_LOCK          0x04830408
#define     SYSC_ATOMIC_STATE_TX_BITS 0x0000000F
#define     SYSC_ATOMIC_STATE_TX_S    0
#ifdef METAC_1_2
#define     SYSC_ATOMIC_STATE_DX_BITS 0x000000F0
#define     SYSC_ATOMIC_STATE_DX_S    4
#else /*           */
#define     SYSC_ATOMIC_SOURCE_BIT    0x00000010
#endif /*            */


#ifdef METAC_2_1

/*                                                                             
                 
 */
#define SYSC_TIMER_DIV            0x04830140
#define     SYSC_TIMDIV_BITS      0x000000FF
#define     SYSC_TIMDIV_S         0

/*                                                           */
#define MMCU_LOCAL_EBCTRL   0x04830600
#define MMCU_GLOBAL_EBCTRL  0x04830608
#define     MMCU_EBCTRL_SINGLE_BIT      0x00000020 /*              */
/*
                                                                  
                                                                         
 */
#define     MMCU_EBCTRL_DCCTRL_BITS     0x000000C0 /*             */
#define     MMCU_EBCTRL_DCCTRL_S        0
#define     MMCU_EBCTRL_ICCTRL_BITS     0x0000C000 /*             */
#define     MMCU_EBCTRL_ICCTRL_S        8

/*                                 */
#define MMCU_T0CCM_ICCTRL   0x04830680     /*                          */
#define     MMCU_TnCCM_xxCTRL_STRIDE    8
#define     MMCU_TnCCM_xxCTRL_STRIDE_S  3
#define MMCU_T1CCM_ICCTRL   0x04830688
#define MMCU_T2CCM_ICCTRL   0x04830690
#define MMCU_T3CCM_ICCTRL   0x04830698
#define MMCU_T0CCM_DCCTRL   0x048306C0     /*                          */
#define MMCU_T1CCM_DCCTRL   0x048306C8
#define MMCU_T2CCM_DCCTRL   0x048306D0
#define MMCU_T3CCM_DCCTRL   0x048306D8
#define     MMCU_TnCCM_ENABLE_BIT       0x00000001
#define     MMCU_TnCCM_WIN3_BIT         0x00000002
#define     MMCU_TnCCM_DCWRITE_BIT      0x00000004  /*                */
#define     MMCU_TnCCM_REGSZ_BITS       0x00000F00
#define     MMCU_TnCCM_REGSZ_S          8
#define         MMCU_TnCCM_REGSZ0_POWER      12     /*               */
#define         MMCU_TnCCM_REGSZ_MAXBYTES    0x00080000  /*          */
#define     MMCU_TnCCM_ADDR_BITS        0xFFFFF000
#define     MMCU_TnCCM_ADDR_S           12

#endif /*           */

/*
                                         
                                         
 */
#ifdef METAC_2_1
/*                                                                */
#define PERF_ICORE0   0x0480FFD0
#define PERF_ICORE1   0x0480FFD8
#define     PERFI_CTRL_BITS    0x0000000F
#define     PERFI_CTRL_S       0
#define         PERFI_CAH_DMISS    0x0  /*                                  */
#define         PERFI_CAH_IMISS    0x1  /*                                  */
#define         PERFI_TLB_DMISS    0x2  /*                                 */
#define         PERFI_TLB_IMISS    0x3  /*                                 */
#define         PERFI_TLB_DWRHITS  0x4  /*                                 */
#define         PERFI_TLB_DWRMISS  0x5  /*                                 */
#define         PERFI_CAH_DLFETCH  0x8  /*                          */
#define         PERFI_CAH_ILFETCH  0x9  /*                          */
#define         PERFI_CAH_DWFETCH  0xA  /*                          */
#define         PERFI_CAH_IWFETCH  0xB  /*                          */
#endif /*           */

/*                                                          */
#define PERF_COUNT0 0x0480FFE0
#define PERF_COUNT1 0x0480FFE8

/*                                 */
#define PERF_COUNT_BITS  0x00ffffff /*                   */

#define PERF_THREAD_BITS 0x0f000000 /*                             */
#define PERF_THREAD_S    24

#define PERF_CTRL_BITS   0xf0000000 /*                      */
#define PERF_CTRL_S      28

#define    PERFCTRL_SUPER   0  /*                    */
#define    PERFCTRL_REWIND  1  /*                              */
#ifdef METAC_2_1
#define    PERFCTRL_SUPREW  2  /*                                           */

#define    PERFCTRL_CYCLES  3  /*                             */

#define    PERFCTRL_PREDBC  4  /*                                */
#define    PERFCTRL_MISPBC  5  /*                                   */
#define    PERFCTRL_PREDRT  6  /*                    */
#define    PERFCTRL_MISPRT  7  /*                       */
#endif /*           */

#define    PERFCTRL_DHITS   8  /*             */
#define    PERFCTRL_IHITS   9  /*             */
#define    PERFCTRL_IMISS   10 /*                               */
#ifdef METAC_2_1
#define    PERFCTRL_DCSTALL 11 /*                                     */
#define    PERFCTRL_ICSTALL 12 /*                                     */

#define    PERFCTRL_INT     13 /*                                          */
#define    PERFCTRL_EXT     15 /*                                   */
#endif /*           */

#ifdef METAC_2_1
/*                                                                             
                 
 */
#define PERF_CHAN0 0x04830150
#define PERF_CHAN1 0x04830158
#define     PERF_CHAN_BITS    0x0000000F
#define     PERF_CHAN_S       0
#define         PERFCHAN_WRC_WRBURST   0x0   /*                            */
#define         PERFCHAN_WRC_WRITE     0x1   /*                            */
#define         PERFCHAN_WRC_RDBURST   0x2   /*                            */
#define         PERFCHAN_WRC_READ      0x3   /*                            */
#define         PERFCHAN_PREARB_DELAY  0x4   /*                            */
					     /*                            */
#define         PERFCHAN_XBAR_HOLDWRAP 0x5   /*                            */
#define         PERFCHAN_XBAR_HOLDSBUS 0x6   /*                            */
#define         PERFCHAN_XBAR_HOLDCREG 0x9   /*                            */
#define         PERFCHAN_L2C_MISS      0x6   /*                            */
#define         PERFCHAN_L2C_HIT       0x7   /*                            */
#define         PERFCHAN_L2C_WRITEBACK 0x8   /*                            */
					     /*                            */
#define         PERFCHAN_INPUT_CREG    0xB   /*                            */
#define         PERFCHAN_INPUT_INTR    0xC   /*                            */
#define         PERFCHAN_INPUT_WRC     0xD   /*                            */

/*                                                     */
#define         PERFCHAN_XBAR_HOLDINTR 0x8   /*                            */
#define         PERFCHAN_INPUT_SBUS    0xA   /*                            */
/*                        */

#define         PERFCHAN_MAINARB_DELAY 0xF   /*                            */

#endif /*           */

#ifdef METAC_2_1
/*
                           
                           
  
                                                                       
              
 */
#define WRCOMB_CONFIG0             0x04830100
#define     WRCOMB_LFFEn_BIT           0x00004000  /*                             */
#define     WRCOMB_ENABLE_BIT          0x00002000  /*                       */
#define     WRCOMB_TIMEOUT_ENABLE_BIT  0x00001000  /*                      */
#define     WRCOMB_TIMEOUT_COUNT_BITS  0x000003FF
#define     WRCOMB_TIMEOUT_COUNT_S     0
#define WRCOMB_CONFIG4             0x04830180
#define     WRCOMB_PARTALLOC_BITS      0x000000C0
#define     WRCOMB_PARTALLOC_S         64
#define     WRCOMB_PARTSIZE_BITS       0x00000030
#define     WRCOMB_PARTSIZE_S          4
#define     WRCOMB_PARTOFFSET_BITS     0x0000000F
#define     WRCOMB_PARTOFFSET_S        0
#define WRCOMB_CONFIG_STRIDE       8
#endif /*           */

#ifdef METAC_2_1
/*
                           
                           
  
                                                                     
              
 */
#define ARBITER_ARBCONFIG0       0x04830120
#define     ARBCFG_BPRIORITY_BIT     0x02000000
#define     ARBCFG_IPRIORITY_BIT     0x01000000
#define     ARBCFG_PAGE_BITS         0x00FF0000
#define     ARBCFG_PAGE_S            16
#define     ARBCFG_BBASE_BITS        0x0000FF00
#define     ARGCFG_BBASE_S           8
#define     ARBCFG_IBASE_BITS        0x000000FF
#define     ARBCFG_IBASE_S           0
#define ARBITER_TTECONFIG0       0x04820160
#define     ARBTTE_IUPPER_BITS       0xFF000000
#define     ARBTTE_IUPPER_S          24
#define     ARBTTE_ILOWER_BITS       0x00FF0000
#define     ARBTTE_ILOWER_S          16
#define     ARBTTE_BUPPER_BITS       0x0000FF00
#define     ARBTTE_BUPPER_S          8
#define     ARBTTE_BLOWER_BITS       0x000000FF
#define     ARBTTE_BLOWER_S          0
#define ARBITER_STRIDE           8
#endif /*           */

/*
                           
                                         
 */

/*                                                        */
#define EXPAND_T0WRCOMBINE         0x03000000
#ifdef METAC_2_1
#define     EXPWRC_LFFEn_BIT           0x00004000  /*                             */
#endif /*           */
#define     EXPWRC_ENABLE_BIT          0x00002000  /*                       */
#define     EXPWRC_TIMEOUT_ENABLE_BIT  0x00001000  /*                      */
#define     EXPWRC_TIMEOUT_COUNT_BITS  0x000003FF
#define     EXPWRC_TIMEOUT_COUNT_S     0
#define EXPAND_TnWRCOMBINE_STRIDE  0x00000008

/*                                                        */
#define EXPAND_T0ARBITER         0x03000020
#define     EXPARB_BPRIORITY_BIT 0x02000000
#define     EXPARB_IPRIORITY_BIT 0x01000000
#define     EXPARB_PAGE_BITS     0x00FF0000
#define     EXPARB_PAGE_S        16
#define     EXPARB_BBASE_BITS    0x0000FF00
#define     EXPARB_BBASE_S       8
#define     EXPARB_IBASE_BITS    0x000000FF
#define     EXPARB_IBASE_S       0
#define EXPAND_TnARBITER_STRIDE  0x00000008

/*                                                            */
#define EXPAND_TIMER_DIV   0x03000040
#define     EXPTIM_DIV_BITS      0x000000FF
#define     EXPTIM_DIV_S         0

/*                                                            */
#define EXPAND_PERFCHAN0   0x03000050
#define EXPAND_PERFCHAN1   0x03000058
#define     EXPPERF_CTRL_BITS    0x0000000F
#define     EXPPERF_CTRL_S       0
#define         EXPPERF_WRC_WRBURST   0x0   /*                            */
#define         EXPPERF_WRC_WRITE     0x1   /*                            */
#define         EXPPERF_WRC_RDBURST   0x2   /*                            */
#define         EXPPERF_WRC_READ      0x3   /*                            */
#define         EXPPERF_PREARB_DELAY  0x4   /*                            */
					    /*                            */
#define         EXPPERF_XBAR_HOLDWRAP 0x5   /*                            */
#define         EXPPERF_XBAR_HOLDSBUS 0x6   /*                            */
#ifdef METAC_1_2
#define         EXPPERF_XBAR_HOLDLBUS 0x7   /*                            */
#else /*           */
#define         EXPPERF_XBAR_HOLDINTR 0x8   /*                            */
#define         EXPPERF_XBAR_HOLDCREG 0x9   /*                            */
					    /*                            */
#define         EXPPERF_INPUT_SBUS    0xA   /*                            */
#define         EXPPERF_INPUT_CREG    0xB   /*                            */
#define         EXPPERF_INPUT_INTR    0xC   /*                            */
#define         EXPPERF_INPUT_WRC     0xD   /*                            */
#endif /*            */
#define         EXPPERF_MAINARB_DELAY 0xF   /*                            */

/*
                       
                                         
 */

/*                        */
#define DBGPORT_MDBGDATAX                    0x0

/*                        */
#define DBGPORT_MDBGDATAT                    0x4

/*                    */
#define DBGPORT_MDBGCTRL0                    0x8
#define     DBGPORT_MDBGCTRL0_ADDR_BITS      0xFFFFFFFC
#define     DBGPORT_MDBGCTRL0_ADDR_S         2
#define     DBGPORT_MDBGCTRL0_AUTOINCR_BIT   0x00000002
#define     DBGPORT_MDBGCTRL0_RD_BIT         0x00000001

/*                    */
#define DBGPORT_MDBGCTRL1                    0xC
#ifdef METAC_2_1
#define    DBGPORT_MDBGCTRL1_DEFERRTHREAD_BITS      0xC0000000
#define    DBGPORT_MDBGCTRL1_DEFERRTHREAD_S         30
#endif /*           */
#define     DBGPORT_MDBGCTRL1_LOCK2_INTERLOCK_BIT   0x20000000
#define     DBGPORT_MDBGCTRL1_ATOMIC_INTERLOCK_BIT  0x10000000
#define     DBGPORT_MDBGCTRL1_TRIGSTATUS_BIT        0x08000000
#define     DBGPORT_MDBGCTRL1_GBLPORT_IDLE_BIT      0x04000000
#define     DBGPORT_MDBGCTRL1_COREMEM_IDLE_BIT      0x02000000
#define     DBGPORT_MDBGCTRL1_READY_BIT             0x01000000
#ifdef METAC_2_1
#define     DBGPORT_MDBGCTRL1_DEFERRID_BITS         0x00E00000
#define     DBGPORT_MDBGCTRL1_DEFERRID_S            21
#define     DBGPORT_MDBGCTRL1_DEFERR_BIT            0x00100000
#endif /*           */
#define     DBGPORT_MDBGCTRL1_WR_ACTIVE_BIT         0x00040000
#define     DBGPORT_MDBGCTRL1_COND_LOCK2_BIT        0x00020000
#define     DBGPORT_MDBGCTRL1_LOCK2_BIT             0x00010000
#define     DBGPORT_MDBGCTRL1_DIAGNOSE_BIT          0x00008000
#define     DBGPORT_MDBGCTRL1_FORCEDIAG_BIT         0x00004000
#define     DBGPORT_MDBGCTRL1_MEMFAULT_BITS         0x00003000
#define     DBGPORT_MDBGCTRL1_MEMFAULT_S            12
#define     DBGPORT_MDBGCTRL1_TRIGGER_BIT           0x00000100
#ifdef METAC_2_1
#define     DBGPORT_MDBGCTRL1_INTSPECIAL_BIT        0x00000080
#define     DBGPORT_MDBGCTRL1_INTRUSIVE_BIT         0x00000040
#endif /*           */
#define     DBGPORT_MDBGCTRL1_THREAD_BITS           0x00000030 /*                             */
#define     DBGPORT_MDBGCTRL1_THREAD_S              4
#define     DBGPORT_MDBGCTRL1_TRANS_SIZE_BITS       0x0000000C
#define     DBGPORT_MDBGCTRL1_TRANS_SIZE_S          2
#define         DBGPORT_MDBGCTRL1_TRANS_SIZE_32_BIT 0x00000000
#define         DBGPORT_MDBGCTRL1_TRANS_SIZE_16_BIT 0x00000004
#define         DBGPORT_MDBGCTRL1_TRANS_SIZE_8_BIT  0x00000008
#define     DBGPORT_MDBGCTRL1_BYTE_ROUND_BITS       0x00000003
#define     DBGPORT_MDBGCTRL1_BYTE_ROUND_S          0
#define         DBGPORT_MDBGCTRL1_BYTE_ROUND_8_BIT  0x00000001
#define         DBGPORT_MDBGCTRL1_BYTE_ROUND_16_BIT 0x00000002


/*                    */
#define SYSC_L2C_INIT              0x048300C0
#define SYSC_L2C_INIT_INIT                  1
#define SYSC_L2C_INIT_IN_PROGRESS           0
#define SYSC_L2C_INIT_COMPLETE              1

#define SYSC_L2C_ENABLE            0x048300D0
#define SYSC_L2C_ENABLE_ENABLE_BIT     0x00000001
#define SYSC_L2C_ENABLE_PFENABLE_BIT   0x00000002

#define SYSC_L2C_PURGE             0x048300C8
#define SYSC_L2C_PURGE_PURGE                1
#define SYSC_L2C_PURGE_IN_PROGRESS          0
#define SYSC_L2C_PURGE_COMPLETE             1

#endif /*                   */
