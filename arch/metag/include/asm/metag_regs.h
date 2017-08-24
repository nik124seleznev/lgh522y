/*
 * asm/metag_regs.h
 *
 * Copyright (C) 2000-2007, 2012 Imagination Technologies.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation.
 *
 * Various defines for Meta core (non memory-mapped) registers.
 */

#ifndef _ASM_METAG_REGS_H_
#define _ASM_METAG_REGS_H_

/*
                                                               
                                                               
 */
#define TXUCT_ID    0x0     /*                   */
#ifdef METAC_1_2
#define     TXUCT_MASK  0xFF0FFFFF  /*                   */
#else
#define     TXUCT_MASK  0xFF1FFFFF  /*                   */
#endif
#define     TGUCT_MASK  0x00000000  /*                   */
#define TXUD0_ID    0x1     /*                */
#define TXUD1_ID    0x2
#define     TXUDX_MASK  0xFFFFFFFF  /*                  */
#define     TGUDX_MASK  0xFFFF0000  /*                           */
#define     TXUDXDSP_MASK   0x0F0FFFFF  /*                */
#define     TGUDXDSP_MASK   0x0E0E0000  /*                     */
#define TXUA0_ID    0x3     /*                   */
#define TXUA1_ID    0x4
#define     TXUAX_MASK  0x0000FFFF  /*                   */
#define     TGUAX_MASK  0x0000FF00  /*                   */
#define TXUPC_ID    0x5     /*              */
#define     TXUPC_MASK  0x00000003  /*                   */
#define     TGUPC_MASK  0x00000000  /*                   */
#define TXUPORT_ID  0x6     /*                         */
#define TXUTR_ID    0x7
#define     TXUTR_MASK  0x0000005F  /*                      */
#define     TGUTR_MASK  0x00000000  /*                   */
#ifdef METAC_2_1
#define TXUTT_ID    0x8
#define     TXUTT_MASK  0x0000000F  /*                  */
#define     TGUTT_MASK  0x00000010  /*                  */
#define TXUFP_ID    0x9     /*          */
#define     TXUFP_MASK  0x0000FFFF  /*                   */
#define     TGUFP_MASK  0x00000000  /*                   */
#endif /*           */

#ifdef METAC_1_2
#define TXUXX_MASKS { TXUCT_MASK, TXUDX_MASK, TXUDX_MASK, TXUAX_MASK, \
		      TXUAX_MASK, TXUPC_MASK,          0, TXUTR_MASK, \
		      0, 0, 0, 0, 0, 0, 0, 0                          }
#define TGUXX_MASKS { TGUCT_MASK, TGUDX_MASK, TGUDX_MASK, TGUAX_MASK, \
		      TGUAX_MASK, TGUPC_MASK,          0, TGUTR_MASK, \
		      0, 0, 0, 0, 0, 0, 0, 0                          }
#else /*           */
#define TXUXX_MASKS { TXUCT_MASK, TXUDX_MASK, TXUDX_MASK, TXUAX_MASK, \
		      TXUAX_MASK, TXUPC_MASK,          0, TXUTR_MASK, \
		      TXUTT_MASK, TXUFP_MASK,          0,          0, \
			       0,          0,          0,          0  }
#define TGUXX_MASKS { TGUCT_MASK, TGUDX_MASK, TGUDX_MASK, TGUAX_MASK, \
		      TGUAX_MASK, TGUPC_MASK,          0, TGUTR_MASK, \
		      TGUTT_MASK, TGUFP_MASK,          0,          0, \
			       0,          0,          0,          0  }
#endif /*            */

#define TXUXXDSP_MASKS { 0, TXUDXDSP_MASK, TXUDXDSP_MASK, 0, 0, 0, 0, 0, \
			 0, 0, 0, 0, 0, 0, 0, 0                          }
#define TGUXXDSP_MASKS { 0, TGUDXDSP_MASK, TGUDXDSP_MASK, 0, 0, 0, 0, 0, \
			 0, 0, 0, 0, 0, 0, 0, 0                          }

/*                                                                          
                                                          
                                                                          */
/*
                           
 */
/*                                                         */
#define D0Re0 D0.0
/*                                  */
#define D0Ar6 D0.1
/*                                  */
#define D0Ar4 D0.2
/*                                                                       */
#define D0Ar2 D0.3
/*                                                                           */
#define D0FrT D0.4
/*                                                                     */
/*                                                                     */
/*                                                                     */
/*                                                     */
/*         */
/*         */
/*         */
/*         */
/*         */
/*         */
/*         */
/*
                            
 */
/*                                                                        */
#define D1Re0 D1.0
/*                                  */
#define D1Ar5 D1.1
/*                                  */
#define D1Ar3 D1.2
/*                                                                              */
#define D1Ar1 D1.3
/*                                                                              */
#define D1RtP D1.4
/*                                                           */
/*                                                           */
/*                                                           */
/*                                                     */
/*         */
/*         */
/*         */
/*         */
/*         */
/*         */
/*         */
/*
                            
 */
/*                                   */
#define A0StP A0.0
/*                                                                       */
#define A0FrP A0.1
/*         */
/*         */
/*                                                     */
/*         */
/*         */
/*         */
/*
                            
 */
/*                                                         */
#define A1GbP A1.0
/*                                                                     */
#define A1LbP A1.1
/*         */
/*         */
/*                                                     */
/*         */
/*         */
/*         */
#ifdef METAC_2_1
/*                                                   */
/*                                                         */
#define A0IStP A0IReg
/*                                                          */
#define A1IGbP A1IReg
#endif
/*
                                                                         
                              
 */
/*                                                                         
                                                           
                                                                         */
/*
                                                                     
                                        
 
                            
                            
                            
           
 
                                                                        
                                                                        
                                                                            
                                                                         
                                      
 
                                                                  
                                                                     
 
                                                                           
                                                                        
                                                                       
                                                                          
                                                                  
 
                       
                          
                                    
 
                                                                           
                                                                     
                                                                   
 
                                                                   
                                                                     
 
                                                                   
 
                                                                           
                              
                                                          
 
                                                                        
                                                                         
                                                                            
 
                              
 
                                                                           
                                                                     
                          
 
                                                                        
                                                                    
                                                                   
                                                                  
 
                                                                          
                                                      
 
                                                                         
                                                                        
        
 
                                                           
                                                                               
                                                                               
               
                                                                               
                                                                               
                                                                               
                                                   
                       
                     
                     
                     
                                                    
                                                                              
                                 
                                         
                                                           
                                                                        
                                                                          
                                               
 
                                                                            
                                                                   
 
                                        
                                  
                       
                    
 
                                                                           
                                                                          
                                           
 
                            
                        
                            
                    
 
                                                                            
 
                                                                          
                                                                     
                                          
 
                                                                     
                            
                        
                                                
                                                
                                                
                                      
                    
 
                                                                           
                                                      
 
                     
                            
         
                          
                                                                           
                              
                                                                
                                 
          
                       
                     
                           
         
                         
                                                                  
                                  
                       
                    
          


                                                                           
                                                
                                                                           
 
                                                                      
                                                               
*/
#define TXENABLE    CT.0    /*                                          */
#define TXMODE      CT.1
#define TXSTATUS    CT.2    /*                    */
#define TXRPT       CT.3
#define TXTIMER     CT.4
#define TXL1START   CT.5
#define TXL1END     CT.6
#define TXL1COUNT   CT.7
#define TXL2START   CT.8
#define TXL2END     CT.9
#define TXL2COUNT   CT.10
#define TXBPOBITS   CT.11
#define TXMRSIZE    CT.12
#define TXTIMERI    CT.13
#define TXDRCTRL    CT.14  /*                    */
#define TXDRSIZE    CT.15
#define TXCATCH0    CT.16
#define TXCATCH1    CT.17
#define TXCATCH2    CT.18
#define TXCATCH3    CT.19

#ifdef METAC_2_1
#define TXDEFR      CT.20
#define TXCPRS      CT.21
#endif

#define TXINTERN0   CT.23
#define TXAMAREG0   CT.24
#define TXAMAREG1   CT.25
#define TXAMAREG2   CT.26
#define TXAMAREG3   CT.27
#define TXDIVTIME   CT.28   /*                    */
#define TXPRIVEXT   CT.29   /*                    */
#define TXTACTCYC   CT.30
#define TXIDLECYC   CT.31

/*                                                                            
                                                    
                                                                            */
/*
                                                                        
                                                                       
 */

/*
                                                                    
 */
#define TXENABLE_REGNUM 0
#define TXENABLE_THREAD_BITS       0x00000700
#define TXENABLE_THREAD_S          8
#define TXENABLE_REV_STEP_BITS     0x000000F0
#define TXENABLE_REV_STEP_S        4

/*
                                                               
 */
#define TXMODE_REGNUM 1
#define     TXMODE_DEFAULT  0   /*                            */

/*
                                                                        
                                                                 
                                               
 */
#define TXSTATUS_REGNUM 2
#define TXSTATUS_PSTAT_BIT         0x00020000   /*                          */
#define TXSTATUS_PSTAT_S           17
#define TXSTATUS_ISTAT_BIT         0x00010000   /*                          */
#define TXSTATUS_ISTAT_S           16

/*
                                                                
 */
#define TXRPT_REGNUM     3  /*                                          */
#define TXTIMER_REGNUM   4  /*                                          */
#define TXL1START_REGNUM 5  /*                                          */
#define TXL1END_REGNUM   6
#define TXL1COUNT_REGNUM 7
#define TXL2START_REGNUM 8  /*                                          */
#define TXL2END_REGNUM   9
#define TXL2COUNT_REGNUM 10
#define TXBPOBITS_REGNUM 11 /*                                          */
#define TXTIMERI_REGNUM  13 /*                                          */

/*
                                                                      
                        
 */
#define TXDIVTIME_REGNUM 28
#define     TXDIVTIME_DIV_BITS 0x000000FF
#define     TXDIVTIME_DIV_S    0
#define     TXDIVTIME_DIV_MIN  0x00000001   /*                          */
#define     TXDIVTIME_DIV_MAX  0x00000100   /*                          */
#define     TXDIVTIME_BASE_HZ  1000000      /*                          */

/*
                                                                     
                                                               
                                     
 */
#define TXPRIVEXT_REGNUM 29
#define     TXPRIVEXT_COPRO_BITS    0xFF000000 /*                  */
#define     TXPRIVEXT_COPRO_S       24
#ifndef METAC_1_2
#define     TXPRIVEXT_TXTIMER_BIT   0x00080000 /*                */
#define     TXPRIVEXT_TRACE_BIT     0x00040000 /*                      */
#endif
#define     TXPRIVEXT_TXTRIGGER_BIT 0x00020000 /*                      */
#define     TXPRIVEXT_TXGBLCREG_BIT 0x00010000 /*                    */
#define     TXPRIVEXT_CBPRIV_BIT    0x00008000 /*                   */
#define     TXPRIVEXT_ILOCK_BIT     0x00004000 /*                */
#define     TXPRIVEXT_TXITACCYC_BIT 0x00002000 /*                     */
#define     TXPRIVEXT_TXDIVTIME_BIT 0x00001000 /*                */
#define     TXPRIVEXT_TXAMAREGX_BIT 0x00000800 /*                */
#define     TXPRIVEXT_TXTIMERI_BIT  0x00000400 /*                */
#define     TXPRIVEXT_TXSTATUS_BIT  0x00000200 /*                */
#define     TXPRIVEXT_TXDISABLE_BIT 0x00000100 /*                */
#ifndef METAC_1_2
#define     TXPRIVEXT_MINIMON_BIT   0x00000080 /*                       */
#define     TXPRIVEXT_OLDBCCON_BIT  0x00000020 /*                            */
#define     TXPRIVEXT_ALIGNREW_BIT  0x00000010 /*                        */
#endif
#define     TXPRIVEXT_KEEPPRI_BIT   0x00000008 /*                            */
#define     TXPRIVEXT_TXTOGGLEI_BIT 0x00000001 /*                */

/*
                                                                  
 */
#define TXTACTCYC_REGNUM  30
#define     TXTACTCYC_COUNT_MASK    0x00FFFFFF

/*
                                          
 */
#define TXIDLECYC_REGNUM  31
#define     TXIDLECYC_COUNT_MASK    0x00FFFFFF

/*                                                                            
                                             
                                                                            */
/*
                                                                         
                                                                     
 */


#ifndef METAC_1_2
/*
                                                                          
                     
 */
#define TXMODE_RDCPEN_BIT       0x00800000
#endif

/*
                                
 */
#define TXMODE_A1ADDR_BITS  0x00007000
#define TXMODE_A1ADDR_S     12
#define TXMODE_A0ADDR_BITS  0x00000700
#define TXMODE_A0ADDR_S     8
#define     TXMODE_AXADDR_MODULO 3
#define     TXMODE_AXADDR_REVB   4
#define     TXMODE_AXADDR_REVW   5
#define     TXMODE_AXADDR_REVD   6
#define     TXMODE_AXADDR_REVL   7

/*
                                                                     
 */
#define TXMODE_DXOVERSCALE_BIT  0x00000080

/*
                                                         
 */
#define TXMODE_M8_BIT         0x00000040

/*
                                                                         
 */
#define TXMODE_DXACCSAT_BIT 0x00000020 /*                            */

/*
                                                                       
 */
#define TXMODE_DXSAT_BIT    0x00000010

/*
                                                                           
 */
#define TXMODE_DXROUNDING_BIT   0x00000008

/*
                                                                  
 */
#define TXMODE_DXPRODSHIFT_BIT  0x00000004

/*
                                                                   
 */
#define TXMODE_DXARITH_BITS 0x00000003
#define     TXMODE_DXARITH_32  3
#define     TXMODE_DXARITH_32H 2
#define     TXMODE_DXARITH_S16 1
#define     TXMODE_DXARITH_16  0

/*
                                                                          
 */
#define TXMRSIZE_REGNUM 12
#define     TXMRSIZE_MIN    0x0002  /*                                 */
#define     TXMRSIZE_MAX    0xFFFF

/*
                                                                         
                                       
 */
#define TXDRCTRL_REGNUM 14
#define     TXDRCTRL_SINESIZE_BITS  0x0F000000
#define     TXDRCTRL_SINESIZE_S     24
#define     TXDRCTRL_RAMSZPOW_BITS  0x001F0000  /*                         */
#define     TXDRCTRL_RAMSZPOW_S     16
#define     TXDRCTRL_D1RSZAND_BITS  0x0000F000  /*                      */
#define     TXDRCTRL_D1RSZAND_S     12
#define     TXDRCTRL_D0RSZAND_BITS  0x000000F0  /*                      */
#define     TXDRCTRL_D0RSZAND_S     4
/*                                                                    */
#define     TXDRCTRL_DXSIZE(Pow, AndBits) \
				((((~(AndBits)) & 0x0f) + 1) << ((Pow)-4))

/*
                                                                        
 */
#define TXDRSIZE_REGNUM 15
#define     TXDRSIZE_R1MOD_BITS       0xFFFF0000
#define     TXDRSIZE_R1MOD_S          16
#define     TXDRSIZE_R0MOD_BITS       0x0000FFFF
#define     TXDRSIZE_R0MOD_S          0

#define     TXDRSIZE_RBRAD_SCALE_BITS 0x70000000
#define     TXDRSIZE_RBRAD_SCALE_S    28
#define     TXDRSIZE_RBMODSIZE_BITS   0x0FFF0000
#define     TXDRSIZE_RBMODSIZE_S      16
#define     TXDRSIZE_RARAD_SCALE_BITS 0x00007000
#define     TXDRSIZE_RARAD_SCALE_S    12
#define     TXDRSIZE_RAMODSIZE_BITS   0x00000FFF
#define     TXDRSIZE_RAMODSIZE_S      0

/*                                                                            
                                                         
                                                                            */

/*
                                               
 */
#define TXDEFR_REGNUM 20
#define     TXDEFR_DEFAULT  0   /*                            */

/*
                                                                           
                          
 */
#define     TXDEFR_BUS_ERR_BIT    0x80000000  /*                          */
#define     TXDEFR_BUS_ERRI_BIT   0x40000000  /*                      */
#define     TXDEFR_BUS_STATE_BITS 0x3F000000  /*                      */
#define     TXDEFR_BUS_STATE_S    24
#define     TXDEFR_BUS_TRIG_BIT   0x00800000  /*                         */

/*
                                                                          
                                                                          
 */
#define     TXDEFR_BUS_ICTRL_BIT  0x00000080  /*                          */

/*
                                                   
                                                   
 */

/*                       */
#define TXAMAREG0_REGNUM 24
#ifdef METAC_1_2
#define     TXAMAREG0_CTRL_BITS       0x07000000
#else /*           */
#define     TXAMAREG0_RCOFF_BIT       0x08000000
#define     TXAMAREG0_DLINEHLT_BIT    0x04000000
#define     TXAMAREG0_DLINEDIS_BIT    0x02000000
#define     TXAMAREG0_CYCSTRICT_BIT   0x01000000
#define     TXAMAREG0_CTRL_BITS       (TXAMAREG0_RCOFF_BIT |    \
				       TXAMAREG0_DLINEHLT_BIT | \
				       TXAMAREG0_DLINEDIS_BIT | \
				       TXAMAREG0_CYCSTRICT_BIT)
#endif /*            */
#define     TXAMAREG0_CTRL_S           24
#define     TXAMAREG0_MDM_BIT         0x00400000
#define     TXAMAREG0_MPF_BIT         0x00200000
#define     TXAMAREG0_MPE_BIT         0x00100000
#define     TXAMAREG0_MASK_BITS       (TXAMAREG0_MDM_BIT | \
				       TXAMAREG0_MPF_BIT | \
				       TXAMAREG0_MPE_BIT)
#define     TXAMAREG0_MASK_S          20
#define     TXAMAREG0_SDM_BIT         0x00040000
#define     TXAMAREG0_SPF_BIT         0x00020000
#define     TXAMAREG0_SPE_BIT         0x00010000
#define     TXAMAREG0_STATUS_BITS     (TXAMAREG0_SDM_BIT | \
				       TXAMAREG0_SPF_BIT | \
				       TXAMAREG0_SPE_BIT)
#define     TXAMAREG0_STATUS_S        16
#define     TXAMAREG0_PRIORITY_BITS   0x0000FF00
#define     TXAMAREG0_PRIORITY_S      8
#define     TXAMAREG0_BVALUE_BITS     0x000000FF
#define     TXAMAREG0_BVALUE_S  0

#define TXAMAREG1_REGNUM 25
#define     TXAMAREG1_DELAYC_BITS     0x07FFFFFF
#define     TXAMAREG1_DELAYC_S  0

#define TXAMAREG2_REGNUM 26
#ifdef METAC_1_2
#define     TXAMAREG2_DLINEC_BITS     0x00FFFFFF
#define     TXAMAREG2_DLINEC_S        0
#else /*           */
#define     TXAMAREG2_IRQPRIORITY_BIT 0xFF000000
#define     TXAMAREG2_IRQPRIORITY_S   24
#define     TXAMAREG2_DLINEC_BITS     0x00FFFFF0
#define     TXAMAREG2_DLINEC_S        4
#endif /*            */

#define TXAMAREG3_REGNUM 27
#define     TXAMAREG2_AMABLOCK_BIT    0x00080000
#define     TXAMAREG2_AMAC_BITS       0x0000FFFF
#define     TXAMAREG2_AMAC_S          0

/*                                                                            
                                                
                                                                            */
/*
                                                           
 */

/*
                                                            
 */
#define     TXMODE_FPURMODE_BITS     0x00030000
#define     TXMODE_FPURMODE_S        16
#define     TXMODE_FPURMODEWRITE_BIT 0x00040000  /*                        */

/*
                                                                         
                                                                            
                                             
 */
#define     TXDEFR_FPE_FE_BITS       0x003F0000  /*                      */
#define     TXDEFR_FPE_FE_S          16

#define     TXDEFR_FPE_INEXACT_FE_BIT   0x010000
#define     TXDEFR_FPE_UNDERFLOW_FE_BIT 0x020000
#define     TXDEFR_FPE_OVERFLOW_FE_BIT  0x040000
#define     TXDEFR_FPE_DIVBYZERO_FE_BIT 0x080000
#define     TXDEFR_FPE_INVALID_FE_BIT   0x100000
#define     TXDEFR_FPE_DENORMAL_FE_BIT  0x200000

#define     TXDEFR_FPE_ICTRL_BITS    0x000003F   /*                     */
#define     TXDEFR_FPE_ICTRL_S       0

#define     TXDEFR_FPE_INEXACT_ICTRL_BIT   0x01
#define     TXDEFR_FPE_UNDERFLOW_ICTRL_BIT 0x02
#define     TXDEFR_FPE_OVERFLOW_ICTRL_BIT  0x04
#define     TXDEFR_FPE_DIVBYZERO_ICTRL_BIT 0x08
#define     TXDEFR_FPE_INVALID_ICTRL_BIT   0x10
#define     TXDEFR_FPE_DENORMAL_ICTRL_BIT  0x20

/*
                              
                              
 */

/*
                                                                    
 */
#define METAG_FPURMODE_TONEAREST  0x0      /*         */
#define METAG_FPURMODE_TOWARDZERO 0x1
#define METAG_FPURMODE_UPWARD     0x2
#define METAG_FPURMODE_DOWNWARD   0x3

/*
                                                                            
                                                                          
                                                                            
                                                                       
                                                                           
                               
 */
#define TXMODE_FPURMODE_SET(FPURMode) \
	(TXMODE_FPURMODEWRITE_BIT + ((FPURMode)<<TXMODE_FPURMODE_S))

/*
                                                                        
                                                   
 */
#define TXMODE_FPURMODE_RESET (TXMODE_FPURMODEWRITE_BIT)

/*
                                                                            
                           
 */
#define TXSTATUS_FPACTIVE_BIT  0x01000000

/*
                                                                    
                                                                      
                                        
 */
#define METAG_FPU_FE_INEXACT   0x01
#define METAG_FPU_FE_UNDERFLOW 0x02
#define METAG_FPU_FE_OVERFLOW  0x04
#define METAG_FPU_FE_DIVBYZERO 0x08
#define METAG_FPU_FE_INVALID   0x10
#define METAG_FPU_FE_DENORMAL  0x20
#define METAG_FPU_FE_ALL_EXCEPT (METAG_FPU_FE_INEXACT   | \
				 METAG_FPU_FE_UNDERFLOW | \
				 METAG_FPU_FE_OVERFLOW  | \
				 METAG_FPU_FE_DIVBYZERO | \
				 METAG_FPU_FE_INVALID   | \
				 METAG_FPU_FE_DENORMAL)

/*                                                                            
                                                                   
                                                                            */
/*
                                                                          
                                                                             
                      
 */

/*
                                                                       
 */
#define TXENABLE_MAJOR_REV_BITS    0xFF000000
#define TXENABLE_MAJOR_REV_S       24
#define TXENABLE_MINOR_REV_BITS    0x00FF0000
#define TXENABLE_MINOR_REV_S       16
#define TXENABLE_CLASS_BITS        0x0000F000
#define TXENABLE_CLASS_S           12
#define TXENABLE_CLASS_DSP             0x0 /*               */
#define TXENABLE_CLASS_LDSP            0x8 /*                    */
#define TXENABLE_CLASS_GP              0xC /*                           */
#define     TXENABLE_CLASSALT_LFPU       0x2 /*                           */
#define     TXENABLE_CLASSALT_FPUR8      0x1 /*                             */
#define TXENABLE_MTXARCH_BIT       0x00000800
#define TXENABLE_STEP_REV_BITS     0x000000F0
#define TXENABLE_STEP_REV_S        4
#define TXENABLE_STOPPED_BIT       0x00000004   /*                        */
#define TXENABLE_OFF_BIT           0x00000002   /*                        */
#define TXENABLE_ENABLE_BIT        0x00000001   /*                */

/*
                                                                        
 */
#define TXSTATUS_CB1MARKER_BIT     0x00800000   /*                        */
#define TXSTATUS_CBMARKER_BIT      0x00400000   /*                         */
#define TXSTATUS_MEM_FAULT_BITS    0x00300000
#define TXSTATUS_MEM_FAULT_S       20
#define     TXSTATUS_MEMFAULT_NONE  0x0 /*                          */
#define     TXSTATUS_MEMFAULT_GEN   0x1 /*                          */
#define     TXSTATUS_MEMFAULT_PF    0x2 /*                          */
#define     TXSTATUS_MEMFAULT_RO    0x3 /*                          */
#define TXSTATUS_MAJOR_HALT_BITS   0x000C0000
#define TXSTATUS_MAJOR_HALT_S      18
#define     TXSTATUS_MAJHALT_TRAP 0x0   /*                          */
#define     TXSTATUS_MAJHALT_INST 0x1   /*                          */
#define     TXSTATUS_MAJHALT_PRIV 0x2   /*                          */
#define     TXSTATUS_MAJHALT_MEM  0x3   /*                          */
#define TXSTATUS_L_STEP_BITS       0x00000800   /*                          */
#define TXSTATUS_LSM_STEP_BITS     0x00000700   /*                          */
#define TXSTATUS_LSM_STEP_S        8
#define TXSTATUS_FLAG_BITS         0x0000001F   /*                          */
#define TXSTATUS_SCC_BIT           0x00000010   /*                          */
#define TXSTATUS_SCF_LZ_BIT        0x00000008   /*                          */
#define TXSTATUS_SCF_HZ_BIT        0x00000004   /*                          */
#define TXSTATUS_SCF_HC_BIT        0x00000002   /*                          */
#define TXSTATUS_SCF_LC_BIT        0x00000001   /*                          */
#define TXSTATUS_CF_Z_BIT          0x00000008   /*                          */
#define TXSTATUS_CF_N_BIT          0x00000004   /*                          */
#define TXSTATUS_CF_O_BIT          0x00000002   /*                          */
#define TXSTATUS_CF_C_BIT          0x00000001   /*                          */

/*
                                                                           
                                                           
 */
#define TXCATCH0_REGNUM 16
#define TXCATCH1_REGNUM 17
#define     TXCATCH1_ADDR_BITS   0xFFFFFFFF   /*                       */
#define     TXCATCH1_ADDR_S      0
#define TXCATCH2_REGNUM 18
#define     TXCATCH2_DATA0_BITS  0xFFFFFFFF   /*                       */
#define     TXCATCH2_DATA0_S     0
#define TXCATCH3_REGNUM 19
#define     TXCATCH3_DATA1_BITS  0xFFFFFFFF   /*                        */
#define     TXCATCH3_DATA1_S     0

/*
                                   
                                   
 */

/*                               */
#define     TXCATCH0_LDRXX_BITS  0xF8000000  /*                           */
#define     TXCATCH0_LDRXX_S     27
#define     TXCATCH0_LDDST_BITS  0x07FF0000  /*                       */
#define     TXCATCH0_LDDST_S     16
#define         TXCATCH0_LDDST_D1DSP 0x400   /*                            */
#define         TXCATCH0_LDDST_D0DSP 0x200
#define         TXCATCH0_LDDST_TMPLT 0x100
#define         TXCATCH0_LDDST_TR    0x080
#ifdef METAC_2_1
#define         TXCATCH0_LDDST_FPU   0x040
#endif
#define         TXCATCH0_LDDST_PC    0x020
#define         TXCATCH0_LDDST_A1    0x010
#define         TXCATCH0_LDDST_A0    0x008
#define         TXCATCH0_LDDST_D1    0x004
#define         TXCATCH0_LDDST_D0    0x002
#define         TXCATCH0_LDDST_CT    0x001
#ifdef METAC_2_1
#define     TXCATCH0_WATCHSTOP_BIT 0x00004000  /*                             */
#endif
#define     TXCATCH0_WATCHS_BIT  0x00004000  /*                             */
#define     TXCATCH0_WATCH1_BIT  0x00002000  /*                             */
#define     TXCATCH0_WATCH0_BIT  0x00001000  /*                             */
#define     TXCATCH0_FAULT_BITS  0x00000C00  /*                             */
#define     TXCATCH0_FAULT_S     10
#define     TXCATCH0_PRIV_BIT    0x00000200  /*                             */
#define     TXCATCH0_READ_BIT    0x00000100  /*                             */

#ifdef METAC_2_1
/*                               */
#define   TXCATCH0_LNKGET_MARKER_BIT 0x00000008
#define       TXCATCH0_PREPROC_BIT  0x00000004
#endif

/*                                                        */
#define     TXCATCH0_LDM16_BIT   0x00000004  /*               */
#define     TXCATCH0_LDL2L1_BITS 0x00000003  /*                      */
#define     TXCATCH0_LDL2L1_S    0

/*                                                                  */
#define     TXCATCH0_RAXX_BITS   0x0000001F  /*                          */
#define     TXCATCH0_RAXX_S      0

/*                                                             */
#define     TXCATCH0_WMASK_BITS  0x000000FF  /*                      */
#define     TXCATCH0_WMASK_S     0

#ifdef METAC_2_1

/*                                                               */
#define     TXCATCH0_FPURDREG_BITS    0xF8000000
#define     TXCATCH0_FPURDREG_S       27
#define     TXCATCH0_FPUR1REG_BITS    0x07C00000
#define     TXCATCH0_FPUR1REG_S       22
#define     TXCATCH0_FPUSPEC_BITS     0x000F0000
#define     TXCATCH0_FPUSPEC_S        16
#define         TXCATCH0_FPUSPEC_TAG      0xF
#define     TXCATCH0_FPUINSTA_BIT     0x00001000
#define     TXCATCH0_FPUINSTQ_BIT     0x00000800
#define     TXCATCH0_FPUINSTZ_BIT     0x00000400
#define     TXCATCH0_FPUINSTN_BIT     0x00000200
#define     TXCATCH0_FPUINSTO3O_BIT   0x00000100
#define     TXCATCH0_FPUWIDTH_BITS    0x000000C0
#define     TXCATCH0_FPUWIDTH_S       6
#define         TXCATCH0_FPUWIDTH_FLOAT   0
#define         TXCATCH0_FPUWIDTH_DOUBLE  1
#define         TXCATCH0_FPUWIDTH_PAIRED  2
#define     TXCATCH0_FPUOPENC_BITS    0x0000003F
#define     TXCATCH0_FPUOPENC_S       0
#define         TXCATCH0_FPUOPENC_ADD     0  /*                    */
#define         TXCATCH0_FPUOPENC_SUB     1  /*                    */
#define         TXCATCH0_FPUOPENC_MUL     2  /*                    */
#define         TXCATCH0_FPUOPENC_ATOI    3  /*         */
#define         TXCATCH0_FPUOPENC_ATOX    4  /*                    */
#define         TXCATCH0_FPUOPENC_ITOA    5  /*         */
#define         TXCATCH0_FPUOPENC_XTOA    6  /*                    */
#define         TXCATCH0_FPUOPENC_ATOH    7  /*         */
#define         TXCATCH0_FPUOPENC_HTOA    8  /*         */
#define         TXCATCH0_FPUOPENC_DTOF    9  /*         */
#define         TXCATCH0_FPUOPENC_FTOD    10 /*         */
#define         TXCATCH0_FPUOPENC_DTOL    11 /*         */
#define         TXCATCH0_FPUOPENC_LTOD    12 /*         */
#define         TXCATCH0_FPUOPENC_DTOXL   13 /*                    */
#define         TXCATCH0_FPUOPENC_XLTOD   14 /*                    */
#define         TXCATCH0_FPUOPENC_CMP     15 /*                    */
#define         TXCATCH0_FPUOPENC_MIN     16 /*                    */
#define         TXCATCH0_FPUOPENC_MAX     17 /*                    */
#define         TXCATCH0_FPUOPENC_ADDRE   18 /*                    */
#define         TXCATCH0_FPUOPENC_SUBRE   19 /*                    */
#define         TXCATCH0_FPUOPENC_MULRE   20 /*                    */
#define         TXCATCH0_FPUOPENC_MXA     21 /*                             */
#define         TXCATCH0_FPUOPENC_MXAS    22 /*                             */
#define         TXCATCH0_FPUOPENC_MAR     23 /*                    */
#define         TXCATCH0_FPUOPENC_MARS    24 /*                    */
#define         TXCATCH0_FPUOPENC_MUZ     25 /*                             */
#define         TXCATCH0_FPUOPENC_MUZS    26 /*                             */
#define         TXCATCH0_FPUOPENC_RCP     27 /*         */
#define         TXCATCH0_FPUOPENC_RSQ     28 /*         */

/*                                                                    */
#define     TXCATCH1_FPUR2REG_BITS    0xF8000000
#define     TXCATCH1_FPUR2REG_S       27
#define     TXCATCH1_FPUR3REG_BITS    0x07C00000  /*                      */
#define     TXCATCH1_FPUR3REG_S       22
#define     TXCATCH1_FPUIMM16_BITS    0x0000FFFF
#define     TXCATCH1_FPUIMM16_S       0

#endif /*           */

/*
                                                                         
                                   
 */
#define     TXDIVTIME_CBBASE_MASK    0x03FFFE00
#define     TXDIVTIME_CBBASE_LINBASE 0x80000000
#define     TXDIVTIME_CBBASE_LINBOFF 0x00000000 /*            */
#define     TXDIVTIME_CBBASE_LINIOFF 0x00000100 /*            */

/*
                                                                            
                                                                           
                                                                     
                                  
 */
#define     TXDIVTIME_RPDIRTY_BIT   0x80000000

/*
                                                                          
                                                                           
  
                                                                        
                                                                           
                    
 */
#define     TXDIVTIME_IRQENC_BITS   0x0F000000
#define     TXDIVTIME_IRQENC_S      24

/*
                                                                          
                                                                            
                                                                 
                                                                     
                                                                            
                                                                            
                                                
 */
#define     TXDIVTIME_RPMASK_BITS 0x003F0000   /*                       */
#define     TXDIVTIME_RPMASK_S    16

/*
                                                                     
                                                                          
 */
#define     TXPRIVEXT_TXSTEP_BIT    0x00000004
#define     TXPRIVEXT_MEMCHECK_BIT  0x00000002

/*
                                                                              
 */
#define TXINTERN0_REGNUM 23
#define     TXINTERN0_LOCK2_BITS  0xF0000000
#define     TXINTERN0_LOCK2_S     28
#define     TXINTERN0_LOCK1_BITS  0x0F000000
#define     TXINTERN0_LOCK1_S     24
#define     TXINTERN0_TIFDF_BITS  0x0000F000
#define     TXINTERN0_TIFDF_S     12
#define     TXINTERN0_TIFIB_BITS  0x00000F00
#define     TXINTERN0_TIFIB_S     8
#define     TXINTERN0_TIFAF_BITS  0x000000F0
#define     TXINTERN0_TIFAF_S     4
#define     TXINTERN0_MSTATE_BITS 0x0000000F
#define     TXINTERN0_MSTATE_S    0

/*
                                                                           
                                                                          
 */
#define TXSTAT_REGNUM  0
#define     TXSTAT_TIMER_BIT    0x00000001
#define     TXSTAT_TIMER_S      0
#define     TXSTAT_KICK_BIT     0x00000002
#define     TXSTAT_KICK_S       1
#define     TXSTAT_DEFER_BIT    0x00000008
#define     TXSTAT_DEFER_S      3
#define     TXSTAT_EXTTRIG_BITS 0x0000FFF0
#define     TXSTAT_EXTTRIG_S    4
#define     TXSTAT_FPE_BITS     0x003F0000
#define     TXSTAT_FPE_S        16
#define     TXSTAT_FPE_DENORMAL_BIT    0x00200000
#define     TXSTAT_FPE_DENORMAL_S      21
#define     TXSTAT_FPE_INVALID_BIT     0x00100000
#define     TXSTAT_FPE_INVALID_S       20
#define     TXSTAT_FPE_DIVBYZERO_BIT   0x00080000
#define     TXSTAT_FPE_DIVBYZERO_S     19
#define     TXSTAT_FPE_OVERFLOW_BIT    0x00040000
#define     TXSTAT_FPE_OVERFLOW_S      18
#define     TXSTAT_FPE_UNDERFLOW_BIT   0x00020000
#define     TXSTAT_FPE_UNDERFLOW_S     17
#define     TXSTAT_FPE_INEXACT_BIT     0x00010000
#define     TXSTAT_FPE_INEXACT_S       16
#define     TXSTAT_BUSERR_BIT          0x00800000   /*                            */
#define     TXSTAT_BUSERR_S            23
#define         TXSTAT_BUSSTATE_BITS     0xFF000000 /*           */
#define         TXSTAT_BUSSTATE_S        24
#define     TXSTAT_KICKCNT_BITS 0xFFFF0000
#define     TXSTAT_KICKCNT_S    16
#define TXMASK_REGNUM  1
#define TXSTATI_REGNUM 2
#define     TXSTATI_BGNDHALT_BIT    0x00000004
#define TXMASKI_REGNUM 3
#define TXPOLL_REGNUM  4
#define TXPOLLI_REGNUM 6

/*
                                                                            
                                                                              
                  
 */
#define     TXDRCTRL_D1PARTOR_BITS  0x00000F00  /*                      */
#define     TXDRCTRL_D1PARTOR_S     8
#define     TXDRCTRL_D0PARTOR_BITS  0x0000000F  /*                      */
#define     TXDRCTRL_D0PARTOR_S     0
/*                                                                       */
#define     TXDRCTRL_DXBASE(Pow, Or)  ((Or)<<((Pow)-4))

/*                                                                            
                                                        
                                                                            */
/*
                                                                          
                                       
 */
#define TTEXEC      TT.0
#define TTCTRL      TT.1
#define TTMARK      TT.2
#define TTREC       TT.3
#define GTEXEC      TT.4

#define TTEXEC_REGNUM               0
#define     TTEXEC_EXTTRIGAND_BITS      0x7F000000
#define     TTEXEC_EXTTRIGAND_S         24
#define     TTEXEC_EXTTRIGEN_BIT        0x00008000
#define     TTEXEC_EXTTRIGMATCH_BITS    0x00007F00
#define     TTEXEC_EXTTRIGMATCH_S       8
#define     TTEXEC_TCMODE_BITS          0x00000003
#define     TTEXEC_TCMODE_S             0

#define TTCTRL_REGNUM               1
#define     TTCTRL_TRACETT_BITS         0x00008000
#define     TTCTRL_TRACETT_S            15
#define     TTCTRL_TRACEALL_BITS        0x00002000
#define     TTCTRL_TRACEALL_S           13
#ifdef METAC_2_1
#define     TTCTRL_TRACEALLTAG_BITS     0x00000400
#define     TTCTRL_TRACEALLTAG_S        10
#endif /*           */
#define     TTCTRL_TRACETAG_BITS        0x00000200
#define     TTCTRL_TRACETAG_S           9
#define     TTCTRL_TRACETTPC_BITS       0x00000080
#define     TTCTRL_TRACETTPC_S          7
#define     TTCTRL_TRACEMPC_BITS        0x00000020
#define     TTCTRL_TRACEMPC_S           5
#define     TTCTRL_TRACEEN_BITS         0x00000008
#define     TTCTRL_TRACEEN_S            3
#define     TTCTRL_TRACEEN1_BITS        0x00000004
#define     TTCTRL_TRACEEN1_S           2
#define     TTCTRL_TRACEPC_BITS         0x00000002
#define     TTCTRL_TRACEPC_S            1

#ifdef METAC_2_1
#define TTMARK_REGNUM   2
#define TTMARK_BITS                 0xFFFFFFFF
#define TTMARK_S                    0x0

#define TTREC_REGNUM    3
#define TTREC_BITS                  0xFFFFFFFFFFFFFFFF
#define TTREC_S                     0x0
#endif /*           */

#define GTEXEC_REGNUM               4
#define     GTEXEC_DCRUN_BITS           0x80000000
#define     GTEXEC_DCRUN_S              31
#define     GTEXEC_ICMODE_BITS          0x0C000000
#define     GTEXEC_ICMODE_S             26
#define     GTEXEC_TCMODE_BITS          0x03000000
#define     GTEXEC_TCMODE_S             24
#define     GTEXEC_PERF1CMODE_BITS      0x00040000
#define     GTEXEC_PERF1CMODE_S         18
#define     GTEXEC_PERF0CMODE_BITS      0x00010000
#define     GTEXEC_PERF0CMODE_S         16
#define     GTEXEC_REFMSEL_BITS         0x0000F000
#define     GTEXEC_REFMSEL_S            12
#define     GTEXEC_METRICTH_BITS        0x000003FF
#define     GTEXEC_METRICTH_S           0

#ifdef METAC_2_1
/*
                          
                          
 */
#define TXCLKCTRL_REGNUM        22

/*
                                                                           
                                                                         
                                                       
 */
#define TXCLKCTRL_ALL_OFF       0x02222222
#define TXCLKCTRL_ALL_DEFON     0x01111111
#define TXCLKCTRL_ALL_AUTO      0x02222222

/*
                                                                            
 */
#define TXCLKCTRL_CLOCKIC_BITS  0x03000000
#define TXCLKCTRL_CLOCKIC_S     24
#define TXCLKCTRL_CLOCKDC_BITS  0x00300000
#define TXCLKCTRL_CLOCKDC_S     20
#define TXCLKCTRL_CLOCKFP_BITS  0x00030000
#define TXCLKCTRL_CLOCKFP_S     16
#define TXCLKCTRL_CLOCKD1_BITS  0x00003000
#define TXCLKCTRL_CLOCKD1_S     12
#define TXCLKCTRL_CLOCKD0_BITS  0x00000300
#define TXCLKCTRL_CLOCKD0_S     8
#define TXCLKCTRL_CLOCKA1_BITS  0x00000030
#define TXCLKCTRL_CLOCKA1_S     4
#define TXCLKCTRL_CLOCKA0_BITS  0x00000003
#define TXCLKCTRL_CLOCKA0_S     0

/*
                                                
 */
#define TXCLKCTRL_CLOCKxx_OFF   0
#define TXCLKCTRL_CLOCKxx_DEFON 1
#define TXCLKCTRL_CLOCKxx_AUTO  2

#endif /*           */

#ifdef METAC_2_1
/*
                          
                                       
 */
#define TXSTATUS_IPTOGGLE_BIT           0x80000000 /*                           */
#define TXSTATUS_ISTATE_BIT             0x40000000 /*            */
#define TXSTATUS_IWAIT_BIT              0x20000000 /*                                   */
#define TXSTATUS_IEXCEPT_BIT            0x10000000 /*                               */
#define TXSTATUS_IRPCOUNT_BITS          0x0E000000 /*                               */
#define TXSTATUS_IRPCOUNT_S             25
#define TXSTATUS_IRQSTAT_BITS           0x0000F000 /*                                    */
#define TXSTATUS_IRQSTAT_S              12
#define TXSTATUS_LNKSETOK_BIT           0x00000020 /*                                 */

/*                                              */
#define TXDIVTIME_IACTIVE_BIT           0x00008000 /*                             */
#define TXDIVTIME_INONEST_BIT           0x00004000 /*                       */
#define TXDIVTIME_IREGIDXGATE_BIT       0x00002000 /*                            */
#define TXDIVTIME_IREGIDX_BITS          0x00001E00 /*                          */
#define TXDIVTIME_IREGIDX_S             9
#define TXDIVTIME_NOST_BIT              0x00000100 /*                            */
#endif

#endif /*                    */
