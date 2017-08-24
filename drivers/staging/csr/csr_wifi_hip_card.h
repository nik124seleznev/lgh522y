/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/*
                                                                               
                             
  
                                                                        
                                                                            
                            
  
                                                                              
 */
#ifndef __CARD_H__
#define __CARD_H__

#include "csr_wifi_hip_card_sdio.h"
#include "csr_wifi_hip_signals.h"
#include "csr_wifi_hip_unifi_udi.h"


/*                                                                            
                  
 */
CsrResult CardEnableInt(card_t *card);

/*                                                                            
               
 */
CsrResult CardGenInt(card_t *card);

/*                                                                            
                   
 */
CsrResult CardPendingInt(card_t *card, u8 *pintr);

/*                                                                            
                   
 */
CsrResult CardDisableInt(card_t *card);

/*                                                                            
                 
 */
CsrResult CardClearInt(card_t *card);

/*                                                                            
                
 */
void CardDisable(card_t *card);

/*                                                                            
                   
 */
CsrResult CardIntEnabled(card_t *card, u8 *enabled);

/*                                                                            
                      
 */
u16 CardGetDataSlotSize(card_t *card);

/*                                                                            
                      
 */
CsrResult CardWriteBulkData(card_t *card, card_signal_t *csptr, unifi_TrafficQueue queue);


/*                                                                            
                              
 */
void CardClearFromHostDataSlot(card_t *card, const s16 aSlotNum);

#ifdef CSR_WIFI_REQUEUE_PACKET_TO_HAL
/*                                                                            
                                                                        
                                
 */

void CardClearFromHostDataSlotWithoutFreeingBulkData(card_t *card, const s16 aSlotNum);
#endif

/*                                                                            
                                 
 */
u16 CardGetFreeFromHostDataSlots(card_t *card);

u16 CardAreAllFromHostDataSlotsEmpty(card_t *card);

CsrResult card_start_processor(card_t *card, enum unifi_dbg_processors_select which);

CsrResult card_wait_for_firmware_to_start(card_t *card, u32 *paddr);

CsrResult unifi_dl_firmware(card_t *card, void *arg);
CsrResult unifi_dl_patch(card_t *card, void *arg, u32 boot_ctrl);
CsrResult unifi_do_loader_op(card_t *card, u32 op_addr, u8 opcode);
void* unifi_dl_fw_read_start(card_t *card, s8 is_fw);

CsrResult unifi_coredump_handle_request(card_t *card);

CsrResult ConvertCsrSdioToCsrHipResult(card_t *card, CsrResult csrResult);
#ifdef CSR_WIFI_HIP_DEBUG_OFFLINE
void unifi_debug_log_to_buf(const char *fmt, ...);
void unifi_debug_string_to_buf(const char *str);
void unifi_debug_hex_to_buf(const char *buff, u16 length);
#endif

#endif /*            */
