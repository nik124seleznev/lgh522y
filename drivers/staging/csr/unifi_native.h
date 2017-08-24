/*
 *****************************************************************************
 *
 * FILE : unifi_native.h
 *
 * PURPOSE : Private header file for unifi driver support to wireless extensions.
 *
 *           UDI = UniFi Debug Interface
 *
 * Copyright (C) 2005-2008 by Cambridge Silicon Radio Ltd.
 *
 * Refer to LICENSE.txt included with this source code for details on
 * the license terms.
 *
 *****************************************************************************
 */
#ifndef __LINUX_UNIFI_NATIVE_H__
#define __LINUX_UNIFI_NATIVE_H__ 1

#include <linux/kernel.h>
#include <linux/if_arp.h>


/*
                                
 */
/*                                     */
typedef struct scan_info {

/*                                  */

    unsigned char *info_elems;
    int info_elem_length;

} scan_info_t;


#define IE_VECTOR_MAXLEN 1024

#ifdef CSR_SUPPORT_WEXT
/*
                                                            
 */
struct wext_config {

    /*                                                    */
    int channel;

    /*                                                                     */
    int mode;

    /*                                                  */
    int capability;

    /*                                                   */
    int beacon_period;

    /*
                           
    */
    /*                                                  */
    int assoc_listen_interval;
    /*                              */

    unsigned char desired_ssid[UNIFI_MAX_SSID_LEN];     /*                                    */
    int power_mode;
    /*                                                             */
    int wakeup_for_dtims;

    /*                                      */
    int wep_key_id;

    wep_key_t wep_keys[NUM_WEPKEYS];

/*                                       */
    int privacy;

    u32 join_failure_timeout;
    u32 auth_failure_timeout;
    u32 assoc_failure_timeout;

    unsigned char generic_ie[IE_VECTOR_MAXLEN];
    int generic_ie_len;

    struct iw_statistics wireless_stats;


    /*                                          */
    unsigned char current_ssid[UNIFI_MAX_SSID_LEN];
    /*                                          */
    unsigned char current_bssid[6];

    /*
                                             
                                             
                                             
    */
    unsigned char wpa_version;

    /*
                        
                                    
                                     
                                    
                                    
                                      
    */
    unsigned char pairwise_cipher_used;
    unsigned char group_cipher_used;

    unsigned int frag_thresh;
    unsigned int rts_thresh;

    /*                                                                */
    unsigned char wmm_bss_uapsd_mask;
    /*                                          */
    unsigned int bss_wmm_capabilities;

    /*                                             */
    int disable_join_on_ssid_set;

    /*           */
#define UNIFI_MAX_SCANS 32
    scan_info_t scan_list[UNIFI_MAX_SCANS];
    int num_scan_info;

    /*                                                    */
/*                                                  */

    /*                                                                     */
    unsigned int flag_associated        : 1;
}; /*                    */

#endif /*                  */


/*
              
 */
/*                                                                
                                                                      
*/

/*
         
 */
/*
                                                      
                                                                       
                                                              
                                                     
*/
void unifi_clear_scan_table(unifi_priv_t *priv);
scan_info_t *unifi_get_scan_report(unifi_priv_t *priv, int index);


/*
                    
 */
const unsigned char *unifi_find_info_element(int id,
                                             const unsigned char *info,
                                             int len);
int unifi_add_info_element(unsigned char *info,
                           int ie_id,
                           const unsigned char *ie_data,
                           int ie_len);

/*
                  
 */
/*                  */
int unifi_autojoin(unifi_priv_t *priv, const char *ssid);
/*
                                                                         
                                                  
*/
int unifi_set_powermode(unifi_priv_t *priv);
int unifi_join_ap(unifi_priv_t *priv, scan_info_t *si);
int unifi_join_bss(unifi_priv_t *priv, unsigned char *macaddr);
int unifi_leave(unifi_priv_t *priv);
unsigned int unifi_get_wmm_bss_capabilities(unifi_priv_t *priv,
                                            unsigned char *ie_vector,
                                            int ie_len, int *ap_capabilities);

/*
                         
 */
int uf_init_wext_interface(unifi_priv_t *priv);
void uf_deinit_wext_interface(unifi_priv_t *priv);

/*
                                                                   
 */
int unifi_reset_state(unifi_priv_t *priv, unsigned char *macaddr, unsigned char set_default_mib);


/*
              
 */
/*                                                    */
int uf_abort_mlme(unifi_priv_t *priv);

int unifi_mlme_blocking_request(unifi_priv_t *priv, ul_client_t *pcli,
                                CSR_SIGNAL *sig, bulk_data_param_t *data_ptrs,
                                int timeout);
void unifi_mlme_copy_reply_and_wakeup_client(ul_client_t *pcli,
                                             CSR_SIGNAL *signal, int signal_len,
                                             const bulk_data_param_t *bulkdata);

/*
                    
 */
const char *lookup_reason_code(int reason);
const char *lookup_result_code(int result);


/*
                    
 */
int uf_sme_init(unifi_priv_t *priv);
void uf_sme_deinit(unifi_priv_t *priv);
int sme_sys_suspend(unifi_priv_t *priv);
int sme_sys_resume(unifi_priv_t *priv);
int sme_mgt_wifi_on(unifi_priv_t *priv);

/*                                                           */
void sme_native_log_event(ul_client_t *client,
                          const u8 *sig_packed, int sig_len,
                          const bulk_data_param_t *bulkdata,
                          int dir);

void sme_native_mlme_event_handler(ul_client_t *pcli,
                                   const u8 *sig_packed, int sig_len,
                                   const bulk_data_param_t *bulkdata,
                                   int dir);

/*                                       */
#define UF_SME_STATS_WQ_TIMEOUT     2000    /*          */
void uf_sme_stats_wq(struct work_struct *work);

void uf_native_process_udi_signal(ul_client_t *pcli,
                                  const u8 *packed_signal,
                                  int packed_signal_len,
                                  const bulk_data_param_t *bulkdata, int dir);
#ifdef UNIFI_SNIFF_ARPHRD
/*
            
 */
int uf_start_sniff(unifi_priv_t *priv);
/*
                                   
                                                             
                                                         
*/
#endif /*                        */

#endif /*                          */
