/*
                                                                            
*/

/*                       
                                                                                

                                                                                    
                                                            
*/



#ifndef _P2P_PRECOMP_H
#define _P2P_PRECOMP_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "gl_os.h"		/*                    */

#include "gl_p2p_os.h"

#include "debug.h"

#include "link.h"
#include "queue.h"


/*                                                                              
                 
                                                                                
 */
#include "wlan_typedef.h"


#include "mac.h"

/*                                   */
#include "wlan_def.h"

#include "roaming_fsm.h"


/*                                                                              
                
                                                                                
 */
/*                                               */
#include "cmd_buf.h"


/*                                   */
#include "nic_cmd_event.h"


/*                                                          */
#include "nic.h"

#include "nic_init_cmd_event.h"

#include "hif_rx.h"
#include "hif_tx.h"

#include "nic_tx.h"

/*                                           */
#include "nic_rx.h"

#include "que_mgt.h"

#if CFG_ENABLE_WIFI_DIRECT
#include "p2p_typedef.h"
#include "p2p_cmd_buf.h"
#include "p2p_nic_cmd_event.h"
#include "p2p_mac.h"
#include "p2p_nic.h"
#endif


/*                                                                              
                 
                                                                                
 */

#include "hem_mbox.h"

#include "scan.h"
#include "bss.h"

#include "wlan_lib.h"
#include "wlan_oid.h"
#include "wlan_bow.h"

#include "wlan_p2p.h"




#include "hal.h"

#if defined(MT6620)
#include "mt6620_reg.h"
#elif defined(MT5931)
#include "mt5931_reg.h"
#endif

#include "rlm.h"
#include "rlm_domain.h"
#include "rlm_protection.h"
#include "rlm_obss.h"
#include "rate.h"


#include "aa_fsm.h"

#include "cnm_timer.h"

#if CFG_ENABLE_BT_OVER_WIFI
#include "bow.h"
#include "bow_fsm.h"
#endif

#include "pwr_mgt.h"


#include "cnm.h"
/*                                                                                    */
#include "cnm_mem.h"
#include "cnm_scan.h"

#include "p2p_rlm_obss.h"
#include "p2p_bss.h"
#include "p2p.h"
/*                                    */
#include "p2p_fsm.h"
#include "p2p_scan.h"
#include "p2p_state.h"
#include "p2p_func.h"
#include "p2p_rlm.h"
#include "p2p_assoc.h"
#include "p2p_ie.h"


#include "privacy.h"

#include "mib.h"

#include "auth.h"
#include "assoc.h"


#include "ais_fsm.h"


#include "adapter.h"


#include "que_mgt.h"
#include "rftest.h"


#if CFG_RSN_MIGRATION
#include "rsn.h"
#include "sec_fsm.h"
#endif

#if CFG_SUPPORT_WAPI
#include "wapi.h"
#endif

/*                                                                              
                  
                                                                                
 */
#include "CFG_Wifi_File.h"

#include "gl_p2p_kal.h"


/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#endif /*               */
