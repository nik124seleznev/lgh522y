/*                                                                            
  
            
            
                   
  
           
           
     
  
               
               
     
  
          
          
     
  
                                                                            */

#ifndef __EEMCS_SYSMSG_H
#define __EEMCS_SYSMSG_H


//                                 
//                      
#include "eemcs_ccci.h"
#include "eemcs_kal.h"


/*                                                                              
                            
                                                                               */


/*                                                                              
                         
                                                                               */


/*                       */
//                                                                           
#define EMCS_ERR_NONE           0
#define EMCS_ERR_TIMEOUT        20 /*                                      */
#define EMCS_ERR_BT_STATUS      21 /*                      */
#define EMCS_ERR_CMDCRC         22 /*                   */
#define EMCS_ERR_LOAD_BIN       23 /*                         */
#define EMCS_ERR_MSG_OVERFLOW   24 /*                           */
#define EMCS_ERR_PKT_OVERFLOW   25 /*                                     */
#define EMCS_ERR_INVALID_PARA   26 /*                                    */
#define EMCS_ERR_GET_OWNER      27 /*                             */     
#define EMCS_ERR_NOMEM          28
#define EMCS_ERR_NOINIT         29
#define EMCS_ERR_INVAL_PARA     30
#define EMCS_ERR_TX_FAIL        31
#define EMCS_ERR_RX_FAIL        32
#define EMCS_ERROR_BUSY         33
#define EMCS_ERROR_NODEV        34

#define CURR_MD_ID (0)
#ifdef CCCI_SDIO_HEAD
#define CCCI_SYSMSG_HEADER_ROOM (sizeof(SDIO_H)+sizeof(CCCI_BUFF_T))
#else
#define CCCI_SYSMSG_HEADER_ROOM (sizeof(CCCI_BUFF_T))
#endif
#define CCCI_SYSMSG_MAX_REQ_NUM (5)


#ifdef _EEMCS_SYSMSG_UT_
  #define ccci_sysmsg_ch_write_desc_to_q(ch_num,desc_p)      
#else
  #define ccci_sysmsg_ch_write_desc_to_q(ch_num,desc_p)     eemcs_ccci_UL_write_skb_to_swq(ch_num, desc_p)
#endif
#define ccci_ch_register(ch_num,cb,para)                    eemcs_ccci_register_callback(ch_num,cb,para) 

extern int eemcs_sysmsg_mod_init(void);
extern void eemcs_sysmsg_exit(void);
extern int get_sim_type(int, int*);
extern int enable_get_sim_type(int, unsigned int);

#endif
