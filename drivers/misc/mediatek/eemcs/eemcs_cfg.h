#ifndef __EEMCS_CFG_H__
#define __EEMCS_CFG_H__


//                                               
#define _ECCMNI_SEQ_SUPPORT_   			//                                      
#define __ECCMNI_SUPPORT__
#define __EEMCS_EXPT_SUPPORT__ 			//                      
#define __EEMCS_XBOOT_SUPPORT__    		//                         

#define  ENABLE_AEE_MD_EE				//                    
#define  ENABLE_MD_WDT_PROCESS			//                                                      
//                                                               


//                                               
#define  EE_INIT_TIMER		(2*HZ)		//                                
#define  EE_HS1_TIMER		(10*HZ)		//                                                         
#define  EE_HS2_TIMER		(5*HZ)		//                                                    

#define CCCI_MTU_3456B					//                                                         
//                                                                                                   

#define MD_EX_LOG_SIZE 		(512)       //                                 
#define MD_EX_MEM_SIZE 		(2*1024)    //                                                     
#define MD_EX_BOOT_TRA_START_OFF (0x400)   //                                                    
#define MD_EX_BOOT_TRA_END_OFF   (0x780)   //                                                   


//                                                
//                                                            
//                                                                         
//                                                                           
//                                                                           
//                                                 
//                                                  

//                        
//                                                                     
//                                                            

#ifdef _EEMCS_CCCI_LB_UT
#ifndef _EEMCS_BOOT_UT
#define _EEMCS_BOOT_UT
#endif
#endif


#endif //               

