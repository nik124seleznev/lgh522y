#ifndef __ETM_REGISTER_H
#define __ETM_REGISTER_H

/*           */
#define ETMCR		(0x0<<2)	/*                         */
#define ETMCCR		(0x1<<2)	/*                               */
#define ETMPCR      (0x1<<2)    /*                                */ 
#define ETMTRIGGER	(0x2<<2)	/*                          */
#define ETMTSR      (0x3<<2)    /*                         */
//                                
#define ETMSR		(0x4<<2)	/*                   */
#define ETMCONFIG   (0x4<<2)    /*                                */ 
#define ETMSCR		(0x5<<2)	/*                                 */
/*                                          */
#define ETMTSSCR	(0x6<<2)
//                                         
#define ETMTEEVR	(0x8<<2)	/*                              */
#define ETMTECR1	(0x9<<2)	/*                                */
//                                 
#define ETMFFLR		(0xb<<2)	/*                           */
//                       
#define ETMVDEVR	(0xc<<2)	//                        
#define ETMVDCR1	(0xd<<2)	//                            
#define ETMSYNCPR   (0xd<<2)    //                                
#define ETMVDCR2	(0xe<<2)	//                            
#define ETMVDCR3	(0xf<<2)	//                            
//                       
#define ETMTRID     (0x10<<2)   //                  
#define ETMVICTLR   (0x20<<2)   //                               
#define ETMVIIECTLR (0x21<<2)   //                                          
#define ETMVISSCTLR (0x22<<2)   //                                     
/*                                      */
#define ETMACVR1	(0x100<<2)
/*                                      */
#define ETMACVR2	(0x102<<2)
/*                                      */
#define ETMACVR3	(0x104<<2)
/*                                      */
#define ETMACVR4	(0x106<<2)
/*                                      */
#define ETMACVR5	(0x108<<2)
/*                                      */
#define ETMACVR6	(0x10a<<2)
/*                                      */
#define ETMACVR7	(0x10c<<2)
/*                                      */
#define ETMACVR8	(0x10e<<2)
/*                                      */
#define ETMACVR9	(0x110<<2)
/*                                       */
#define ETMACVR10	(0x112<<2)
/*                                       */
#define ETMACVR11	(0x114<<2)
/*                                       */
#define ETMACVR12	(0x116<<2)
/*                                       */
#define ETMACVR13	(0x118<<2)
/*                                       */
#define ETMACVR14	(0x11a<<2)
/*                                       */
#define ETMACVR15	(0x11c<<2)
/*                                       */
#define ETMACVR16	(0x11e<<2)
/*                                            */
#define ETMACTR1	(0x120<<2)
/*                                            */
#define ETMACTR2	(0x122<<2)
/*                                            */
#define ETMACTR3	(0x124<<2)
/*                                            */
#define ETMACTR4	(0x126<<2)
/*                                            */
#define ETMACTR5	(0x128<<2)
/*                                            */
#define ETMACTR6	(0x12a<<2)
/*                                            */
#define ETMACTR7	(0x12c<<2)
/*                                            */
#define ETMACTR8	(0x12e<<2)
/*                                            */
#define ETMACTR9	(0x130<<2)
/*                                             */
#define ETMACTR10	(0x132<<2)
/*                                             */
#define ETMACTR11	(0x134<<2)
/*                                             */
#define ETMACTR12	(0x136<<2)
/*                                             */
#define ETMACTR13	(0x138<<2)
/*                                             */
#define ETMACTR14	(0x13a<<2)
/*                                             */
#define ETMACTR15	(0x13c<<2)
/*                                             */
#define ETMACTR16	(0x13e<<2)
//                                                                                                                     
#define ETMDCVR1	(0x30<<2)	//                                 
#define ETMDCVR2	(0x32<<2)	//                                 
#define ETMDCVR3	(0x34<<2)	//                                 
#define ETMDCVR4	(0x36<<2)	//                                 
#define ETMDCVR5	(0x38<<2)	//                                 
#define ETMDCVR6	(0x3a<<2)	//                                 
#define ETMDCVR7	(0x3c<<2)	//                                 
#define ETMDCVR8	(0x3e<<2)	//                                 

#define ETMDCMR1	(0x40<<2)	//                                
#define ETMDCMR2	(0x42<<2)	//                                
#define ETMDCMR3	(0x44<<2)	//                                
#define ETMDCMR4	(0x46<<2)	//                                
#define ETMDCMR5	(0x48<<2)	//                                
#define ETMDCMR6	(0x4a<<2)	//                                
#define ETMDCMR7	(0x4c<<2)	//                                
#define ETMDCMR8	(0x4e<<2)	//                                

//         
#define ETMCNTRLDVR1	(0x50<<2)	/*                                   */
#define ETMCNTRLDVR2	(0x51<<2)	/*                                   */
#define ETMCNTRLDVR3	(0x52<<2)	/*                                   */
#define ETMCNTRLDVR4	(0x53<<2)	/*                                   */
#define ETMCNTENR1	(0x54<<2)	/*                                   */
#define ETMCNTENR2	(0x55<<2)	/*                                   */
#define ETMCNTENR3	(0x56<<2)	/*                                   */
#define ETMCNTENR4	(0x57<<2)	/*                                   */
#define ETMCNTRLDEVR1	(0x58<<2)	/*                                   */
#define ETMCNTRLDEVR2	(0x59<<2)	/*                                   */
#define ETMCNTRLDEVR3	(0x5a<<2)	/*                                   */
#define ETMCNTRLDEVR4	(0x5b<<2)	/*                                   */
#define ETMCNTVR1	(0x5c<<2)	/*                            */
#define ETMCNTVR2	(0x5d<<2)	/*                            */
#define ETMCNTVR3	(0x5e<<2)	/*                            */
#define ETMCNTVR4	(0x5f<<2)	/*                            */
/*                                              */
#define ETMSQabEVR1	(0x60<<2)
/*                                              */
#define ETMSQabEVR2	(0x61<<2)
/*                                              */
#define ETMSQabEVR3	(0x62<<2)
/*                                              */
#define ETMSQabEVR4	(0x63<<2)
/*                                              */
#define ETMSQabEVR5	(0x64<<2)
/*                                              */
#define ETMSQabEVR6	(0x65<<2)
#define ETMSQR		(0x67<<2)	/*                                    */
/*                                   */
#define ETMEXTOUTEVR1	(0x68<<2)
/*                                   */
#define ETMEXTOUTEVR2	(0x69<<2)
/*                                   */
#define ETMEXTOUTEVR3	(0x6a<<2)
/*                                   */
#define ETMEXTOUTEVR4	(0x6b<<2)
/*                                         */
#define ETMCIDCVR1	(0x6c<<2)
/*                                         */
#define ETMCIDCVR2	(0x6d<<2)
/*                                         */
#define ETMCIDCVR3	(0x6e<<2)
/*                                      */
#define ETMCIDCMR	(0x6f<<2)
/*                                     */
#define ETMIMPSPEC0	(0x70<<2)
/*                                     */
#define ETMIMPSPEC1	(0x71<<2)
/*                                     */
#define ETMIMPSPEC2	(0x72<<2)
/*                                     */
#define ETMIMPSPEC3	(0x73<<2)
/*                                     */
#define ETMIMPSPEC4	(0x74<<2)
/*                                     */
#define ETMIMPSPEC5	(0x75<<2)
/*                                     */
#define ETMIMPSPEC6	(0x76<<2)
/*                                     */
#define ETMIMPSPEC7	(0x77<<2)
/*                */
#define ETMIDR0     (0x78<<2)
/*                */
#define ETMIDR1     (0x79<<2)
/*                */
#define ETMIDR2     (0x7a<<2)
/*                */
#define ETMIDR3     (0x7b<<2)
/*                */
#define ETMIDR4     (0x7c<<2)
/*                */
#define ETMIDR5     (0x7d<<2)
/*                */
#define ETMIDR6     (0x7e<<2)
/*                */
#define ETMIDR7     (0x7f<<2)
/*                                         */
#define ETMRSCTLR0  (0x80<<2)
/*                                         */
#define ETMRSCTLR1  (0x81<<2)
/*                                         */
#define ETMRSCTLR2  (0x82<<2)
/*                                         */
#define ETMRSCTLR3  (0x83<<2)
/*                                         */
#define ETMRSCTLR4  (0x84<<2)
/*                                         */
#define ETMRSCTLR5  (0x85<<2)
/*                                         */
#define ETMRSCTLR6  (0x86<<2)
/*                                         */
#define ETMRSCTLR7  (0x87<<2)
/*                                         */
#define ETMRSCTLR8  (0x88<<2)
/*                                         */
#define ETMRSCTLR9  (0x89<<2)
/*                                     */
#define ETMSYNCFR	(0x78<<2)
#define ETMIDR		(0x79<<2)	/*               */
/*                                        */
#define ETMCCER		(0x7a<<2)
/*                                             */
#define ETMEXTINSELR	(0x7b<<2)
/*                                                      */
#define ETMTESSEICR	(0x7c<<2)
/*                                        */
#define ETMEIBCR	(0x7d<<2)
#define ETMTSEVR	(0x7e<<2)	/*                            */
#define ETMAUXCR	(0x7f<<2)	/*                              */
#define ETMTRACEIDR	(0x80<<2)	/*                               */
#define ETMVMIDCVR	(0x90<<2)	/*                                  */
#define ETMOSLAR	(0xc0<<2)	/*                           */
#define ETMOSLSR	(0xc1<<2)	/*                           */
#define ETMOSSRR	(0xc2<<2)	/*                                */
/*                                     */
#define ETMPDCR		(0xc4<<2)
/*                                    */
#define ETMPDSR		(0xc5<<2)
/*                                    */
#define ETMITCTRL	(0x3c0<<2)
#define ETMCLAIMSET	(0x3e8<<2)	/*                          */
#define ETMCLAIMCLR	(0x3e9<<2)	/*                            */
#define ETMLAR		(0x3ec<<2)	/*                        */
#define ETMLSR		(0x3ed<<2)	/*                        */
#define ETMAUTHSTATUS	(0x3ee<<2)	/*                                  */
#define ETMDEVID	(0x3f2<<2)	/*                                 */
#define ETMDEVTYPE	(0x3f3<<2)	/*                        */
#define ETMPIDR4	(0x3f4<<2)	/*                            */
#define ETMPIDR5	(0x3f5<<2)	/*                            */
#define ETMPIDR6	(0x3f6<<2)	/*                            */
#define ETMPIDR7	(0x3f7<<2)	/*                            */
#define ETMPIDR0	(0x3f8<<2)	/*                            */
#define ETMPIDR1	(0x3f9<<2)	/*                            */
#define ETMPIDR2	(0x3fa<<2)	/*                            */
#define ETMPIDR3	(0x3fb<<2)	/*                            */
#define ETMCIDR0	(0x3fc<<2)	/*                           */
#define ETMCIDR1	(0x3fd<<2)	/*                           */
#define ETMCIDR2	(0x3fe<<2)	/*                           */
#define ETMCIDR3	(0x3ff<<2)	/*                           */

/*                           */

/*                                                         */
#define ETMLAR_UNLOCK_MAGIC	0xc5acce55
#define ETMOSLAR_LOCK_MAGIC	0xc5acce55

/*                                                                   */
#define ETMCCR_AC_PAIRS		(0xf)
/*                                               */
#define ETMCCR_COUNTERS		(7<<13)
/*                                                             */
#define ETMCCR_SEQUENCER	(1<<16)
/*                                                      */
#define ETMCCR_EXT_INPUT	(7<<17)
/*                                                       */
#define ETMCCR_EXT_OUTPUT	(7<<20)
/*                                                                  */
#define ETMCCR_FIFOFULL		(1<<23)
/*                                                                         */
#define ETMCCR_CIDC		(3<<24)
/*                                                                          */
#define ETMCCR_TRACE_SS_BLOCK	(1<<26)

/*                                                                        */
#define ETMCCER_EXTEND_EXT_INPUT_SELECTOR	(7)
/*                                                                */
#define ETMCCER_EXTEND_EXT_INPUT_BUS		(0xff<<3)
/*                                                                          */
#define ETMCCER_INSTRUMENTATION_RES		(7<<13)
/* 
                                                                               
              
 */
#define ETMCCER_EMBEDDED_ICE_WATCHPOINT_INPUT	(0xf<<16)
/* 
                                                                               
                               
 */
#define ETMCCER_SS_BLOCK_USE_EMBEDDED_ICE_INPUT	(1<<20)
/* 
                                                                          
              
 */
#define ETMCCER_EIBCR				(1<<21)
/*                                                             */
#define ETMCCER_TIMESTAMP			(1<<22)
/*                                                             */
#define ETMCCER_RET_STACK			(1<<23)
/* 
                                                                        
               
 */
#define ETMCCER_DMB_IS_WAYPOINT			(1<<24)
/* 
                                                                         
             
 */
#define ETMCCER_GENERATE_TIMESTAMP_FOR_DMB	(1<<25)
/*                                                                          */
#define ETMCCER_VIRTUALIZATION_EXT		(1<<26)
/* 
                                                                          
                   
 */
#define ETMCCER_REDUCED_FUNCTION_COUNTER	(1<<27)
/* 
                                                                             
                   
 */
#define ETMCCER_TIMESTAMP_ENCODE		(1<<28)
/* 
                                                                            
                       
 */
#define ETMCCER_TIMESTAMP_SIZE			(1<<29)

/*                                                                     */
#define ETMSCR_FIFOFULL			(1<<8)
/*                                                                   */
#define ETMSCR_PROCESSORS_MINUS_1	(7<<12)

/*                                                    */
#define ETMIDR_REVISION		(0xf)
/*                                                              */
#define ETMIDR_MINOR		(0xf<<4)
/*                                                              */
#define ETMIDR_MAJOR		(0xf<<8)
#define ETMIDR_MAJOR_MASK       0xf
#define ETMIDR_MAJOR_SHIFT      8
#define ETMIDR_MAJOR_PTM        3
/*                                                                           */
#define ETMIDR_THUMB		(1<<18)
/*                                                                     */
#define ETMIDR_SECURITY		(1<<19)
/*                                                                                     */
#define ETMIDR_ALTERNATIVE		(1<<20)
/*                                             */
#define ETMIDR_IMPLEMENTER	(0xff<<24)

/*                                                    */
#define ETMTECR1_TE_EXCLUDE_INCLUDE_CONTROL	(1<<24)
/*                                                                    */
#define ETMTECR1_TE_SS_ENABLE_CONTROL		(1<<25)

/*                                                     */
#define ETMACTRn_CTXID_COMPARATOR_BIT	8
/*                                                       */
#define ETMACTRn_SECURE_LEVEL_BIT1	10
/*                                                           */
#define ETMACTRn_SECURE_LEVEL_BIT2	12

/*                                         */
#define ETMCR_POWER_DOWN_BIT		0
/*                                              */
#define ETMCR_POWER_DOWN		(1<<0)

//                                  
#define ETMCR_MONITOR_CPRT_BIT	1
//                                              
#define ETMCR_MONITOR_CPRT	(1<<1)

//                                         
#define ETMCR_DATA_ACCESS_BIT		2
//                                              
#define ETMCR_DATA_ACCESS		(3<<2)
//                
#define ETMCR_DATA_ACCESS_NO		0
//                        
#define ETMCR_DATA_ACCESS_VALUE		1
//                          
#define ETMCR_DATA_ACCESS_ADDR		2
//                            
#define ETMCR_DATA_ACCESS_ADDR_VALUE		3

/* 
                                                                         
                                          
 */
#define ETMCR_STALL_PROCESSOR_BIT	7
/* 
                                                                              
                                          
 */
#define ETMCR_STALL_PROCESSOR		(1<<7)
/*                                                           */
#define ETMCR_BRANCH_BROADCAST_BIT	8
/*                                                                */
#define ETMCR_BRANCH_BROADCAST		(1<<8)
/* 
                                                                             
                   
 */
#define ETMCR_DEBUG_REQUEST_BIT		9
/* 
                                                                        
                             
 */
#define ETMCR_DEBUG_REQUEST		(1<<9)
/*                                                       */
#define ETMCR_PROGRAM_BIT		10
/*                                                            */
#define ETMCR_PROGRAM			(1<<10)
/*                                                              */
#define ETMCR_CYCLE_ACCURATE_BIT	12
/*                                                               */
#define ETMCR_ETMEN		(1<<11)
#define ETMCR_ETMEN_BIT	11
/*                                                                   */
#define ETMCR_CYCLE_ACCURATE		(1<<12)
/*                                                                */
#define ETMCR_CONTEXTIDSIZE_BIT		14
/*                                                                     */
#define ETMCR_CONTEXTIDSIZE		(3<<14)

//                                                  
#define ETMCR_SUPPRESS_DATA_BIT		18
//                                                        
#define ETMCR_SUPPRESS_DATA			(1<<18)
//                                                
#define ETMCR_FILTER_CPRT_BIT		19
//                                                      
#define ETMCR_FILTER_CPRT			(1<<19)
//                                                   
#define ETMCR_DATA_ONLY_MODE_BIT		20
//                                                         
#define ETMCR_DATA_ONLY_MODE			(1<<20)

/* 
                                                                         
                                                        
 */
#define ETMCR_INSTU_RESOURCE_ACCESS_BIT	24
/* 
                                                                              
                                                        
 */
#define ETMCR_INSTU_RESOURCE_ACCESS	(1<<24)
/*                                                    */
#define ETMCR_TIMESTAMP_BIT		28
/*                                                         */
#define ETMCR_TIMESTAMP			(1<<28)
/*                                                               */
#define ETMCR_RETURN_STACK_BIT		29
/*                                                                    */
#define ETMCR_RETURN_STACK		(1<<29)
/*                                                    */
#define ETMCR_VMID_BIT			30
/*                                                         */
#define ETMCR_VMID			(1<<30)

/*                                                                           */
#define ETMSR_PROGRAM	(1<<1)

#endif
