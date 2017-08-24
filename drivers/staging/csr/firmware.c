/*
 * ---------------------------------------------------------------------------
 *  FILE:     firmware.c
 *
 *  PURPOSE:
 *      Implements the f/w related HIP core lib API.
 *      It is part of the porting exercise in Linux.
 *
 *      Also, it contains example code for reading the loader and f/w files
 *      from the userspace and starting the SME in Linux.
 *
 * Copyright (C) 2005-2009 by Cambridge Silicon Radio Ltd.
 *
 * Refer to LICENSE.txt included with this source code for details on
 * the license terms.
 *
 * ---------------------------------------------------------------------------
 */
#include <linux/kmod.h>
#include <linux/vmalloc.h>
#include <linux/firmware.h>
#include <asm/uaccess.h>
#include "csr_wifi_hip_unifi.h"
#include "csr_wifi_hip_unifi_udi.h"
#include "unifiio.h"
#include "unifi_priv.h"

/*
                                                                              
  
                                              
  
                                                                              
 */


/*
                                                                              
                       
  
                                                            
                                                                     
                                                                                
                                                                        
                          
                                                                               
  
              
                                                  
                                                    
                                                                      
                                                         
  
            
                                         
  
                                                                              
 */
void*
unifi_fw_read_start(void *ospriv, s8 is_fw, const card_info_t *info)
{
    unifi_priv_t *priv = (unifi_priv_t*)ospriv;
    CSR_UNUSED(info);

    if (is_fw == UNIFI_FW_STA) {
        /*                                                                  */
        if (priv->fw_sta.dl_data == NULL) {
            unifi_trace(priv, UDBG2, "Attempt reload of sta f/w\n");
            uf_request_firmware_files(priv, UNIFI_FW_STA);
        }
        /*                                       */
        if (priv->fw_sta.dl_data != NULL) {
            return &priv->fw_sta;
        }

    } else {
        unifi_error(priv, "downloading firmware... unknown request: %d\n", is_fw);
    }

    return NULL;
} /*                       */



/*
                                                                              
                      
  
                                                                   
                          
                                                
  
              
                                                  
                                                                     
  
                                                                              
 */
void
unifi_fw_read_stop(void *ospriv, void *dlpriv)
{
    unifi_priv_t *priv = (unifi_priv_t*)ospriv;
    struct dlpriv *dl_struct = (struct dlpriv *)dlpriv;

    if (dl_struct != NULL) {
        if (dl_struct->dl_data != NULL) {
            unifi_trace(priv, UDBG2, "Release f/w buffer %p, %d bytes\n",
                        dl_struct->dl_data, dl_struct->dl_len);
        }
        uf_release_firmware(priv, dl_struct);
    }

} /*                      */


/*
                                                                              
                        
  
                                                                      
                                                                               
                                                       
  
                                                                            
               
  
              
                                                  
                                                        
                                                 
  
           
                                           
                                                                              
 */
void *
unifi_fw_open_buffer(void *ospriv, void *fwbuf, u32 len)
{
    unifi_priv_t *priv = (unifi_priv_t*)ospriv;

    if (fwbuf == NULL) {
        return NULL;
    }
    priv->fw_conv.dl_data = fwbuf;
    priv->fw_conv.dl_len = len;
    priv->fw_conv.fw_desc = NULL;   /*                                  */

    return &priv->fw_conv;
}

/*
                                                                              
                         
  
                                                                         
                                                                               
                                                       
  
  
              
                                                  
                                                        
  
           
                                           
                                                                              
 */
void unifi_fw_close_buffer(void *ospriv, void *fwbuf)
{
}

/*
                                                                              
                 
  
                                                                             
                          
  
              
                                                  
                                                                      
                                                              
                                                             
                                                                               
  
           
                                                                         
                                                                              
 */
s32
unifi_fw_read(void *ospriv, void *arg, u32 offset, void *buf, u32 len)
{
    const struct dlpriv *dlpriv = arg;

    if (offset >= dlpriv->dl_len) {
        /*                */
        return 0;
    }

    if ((offset + len) > dlpriv->dl_len) {
        /*                                  */
        return -1;
    }

    memcpy(buf, dlpriv->dl_data+offset, len);

    return len;

} /*                 */




#define UNIFIHELPER_INIT_MODE_SMEUSER   2
#define UNIFIHELPER_INIT_MODE_NATIVE    1

/*
                                                                              
                      
  
                                                                 
                            
                                                             
                                            
  
              
                                                     
  
            
             
                                                                              
 */
int
uf_run_unifihelper(unifi_priv_t *priv)
{
#ifdef ANDROID_BUILD
    char *prog = "/system/bin/unififw";
#else
    char *prog = "/usr/sbin/unififw";
#endif /*               */

    char *argv[6], *envp[4];
    char inst_str[8];
    char init_mode[8];
    int i, r;

#if (defined CSR_SME_USERSPACE) && (!defined CSR_SUPPORT_WEXT)
    unifi_trace(priv, UDBG1, "SME userspace build: run unifi_helper manually\n");
    return 0;
#endif

    unifi_trace(priv, UDBG1, "starting %s\n", prog);

    snprintf(inst_str,   8, "%d", priv->instance);
#if (defined CSR_SME_USERSPACE)
    snprintf(init_mode, 8, "%d", UNIFIHELPER_INIT_MODE_SMEUSER);
#else
    snprintf(init_mode, 8, "%d", UNIFIHELPER_INIT_MODE_NATIVE);
#endif /*                   */

    i = 0;
    argv[i++] = prog;
    argv[i++] = inst_str;
    argv[i++] = init_mode;
    argv[i++] = 0;
    argv[i] = 0;
    /*                                                */

    /*                             */
    i = 0;
    envp[i++] = "HOME=/";
    envp[i++] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
    envp[i] = 0;
    /*                                           */

    unifi_trace(priv, UDBG2, "running %s %s %s\n", argv[0], argv[1], argv[2]);

    r = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);

    return r;
} /*                      */

#ifdef CSR_WIFI_SPLIT_PATCH
static u8 is_ap_mode(unifi_priv_t *priv)
{
    if (priv == NULL || priv->interfacePriv[0] == NULL)
    {
        return FALSE;
    }

    /*                                  */
    return(CSR_WIFI_HIP_IS_AP_FW(priv->interfacePriv[0]->interfaceMode));
}
#endif

/*
                                                                              
                             
  
                                              
  
              
                                                     
                                                                      
  
            
             
                                                                              
 */
int uf_request_firmware_files(unifi_priv_t *priv, int is_fw)
{
    /*                                             */
    const struct firmware *fw_entry;
    int postfix;
#define UNIFI_MAX_FW_PATH_LEN       32
    char fw_name[UNIFI_MAX_FW_PATH_LEN];
    int r;

#if (defined CSR_SUPPORT_SME) && (defined CSR_SUPPORT_WEXT)
    if (priv->mib_data.length) {
        vfree(priv->mib_data.data);
        priv->mib_data.data = NULL;
        priv->mib_data.length = 0;
    }
#endif /*                                    */

    postfix = priv->instance;

    if (is_fw == UNIFI_FW_STA) {
        /*                               */
        uf_release_firmware(priv, &priv->fw_sta);
#ifdef CSR_WIFI_SPLIT_PATCH
        scnprintf(fw_name, UNIFI_MAX_FW_PATH_LEN, "unifi-sdio-%d/%s",
                  postfix, (is_ap_mode(priv) ? "ap.xbv" : "staonly.xbv") );
#else
        scnprintf(fw_name, UNIFI_MAX_FW_PATH_LEN, "unifi-sdio-%d/%s",
                  postfix, "sta.xbv" );
#endif
        r = request_firmware(&fw_entry, fw_name, priv->unifi_device);
        if (r == 0) {
            priv->fw_sta.dl_data = fw_entry->data;
            priv->fw_sta.dl_len = fw_entry->size;
            priv->fw_sta.fw_desc = (void *)fw_entry;
        } else {
            unifi_trace(priv, UDBG2, "Firmware file not available\n");
        }
    }

    return 0;

} /*                             */

/*
                                                                              
                             
  
                                                        
  
              
                                                     
  
            
             
                                                                              
 */
int uf_release_firmware_files(unifi_priv_t *priv)
{
    uf_release_firmware(priv, &priv->fw_sta);

    return 0;
}

/*
                                                                              
                       
  
                                                      
  
              
                                                     
                                                             
  
            
             
                                                                              
 */
int uf_release_firmware(unifi_priv_t *priv, struct dlpriv *to_free)
{
    if (to_free != NULL) {
        release_firmware((const struct firmware *)to_free->fw_desc);
        to_free->fw_desc = NULL;
        to_free->dl_data = NULL;
        to_free->dl_len = 0;
    }
    return 0;
}
