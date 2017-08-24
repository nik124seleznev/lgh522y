#ifndef __CCCI_IPC_TASK_ID_H__
#define __CCCI_IPC_TASK_ID_H__
//                                                                              
//                                                         
//                                                          
#ifdef __IPC_ID_TABLE
#define X_IPC_MODULE_CONF(a,b,c,d) {c,b},
#else 
#define X_IPC_MODULE_CONF(a,b,c,d)
#endif


#define AP_UNIFY_ID_FLAG (1<<31)
#define MD_UNIFY_ID_FLAG (0<<31)

//                                                          
#define AGPS_MD_MOD_L4C 0
#define AGPS_MD_MOD_L4C_2 1
#define AGPS_MD_MOD_L4C_3 2
#define AGPS_MD_MOD_L4C_4 3
//                     
//           

#define AGPS_AP_MOD_MMI 0   
//                     
//               
#define GPS_AP_MOD 2
//                                                                          
X_IPC_MODULE_CONF(1, AGPS_MD_MOD_L4C, MD_UNIFY_ID_FLAG|0, 1)
X_IPC_MODULE_CONF(1, AGPS_MD_MOD_L4C_2, MD_UNIFY_ID_FLAG|1, 1)
X_IPC_MODULE_CONF(1, AGPS_MD_MOD_L4C_3, MD_UNIFY_ID_FLAG|2, 1)
X_IPC_MODULE_CONF(1, AGPS_MD_MOD_L4C_4, MD_UNIFY_ID_FLAG|3, 1)
//            
//                                                                          
X_IPC_MODULE_CONF(1, AGPS_AP_MOD_MMI, AP_UNIFY_ID_FLAG|0, 1)
X_IPC_MODULE_CONF(1, GPS_AP_MOD, AP_UNIFY_ID_FLAG|2, 1)
//            
//                                                                         

#endif
