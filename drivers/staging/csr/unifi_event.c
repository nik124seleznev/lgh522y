/*
 * ***************************************************************************
 *  FILE:     unifi_event.c
 *
 *  PURPOSE:
 *      Process the signals received by UniFi.
 *      It is part of the porting exercise.
 *
 * Copyright (C) 2009 by Cambridge Silicon Radio Ltd.
 *
 * Refer to LICENSE.txt included with this source code for details on
 * the license terms.
 *
 * ***************************************************************************
 */


/*
                 
                                                                              
                                                                       
                                                                           
                                
                                                              
                                                                          
                                      
                                                               
                                                                
 */

#include "csr_wifi_hip_unifi.h"
#include "csr_wifi_hip_conversions.h"
#include "unifi_priv.h"


/*
                                                                              
                  
  
                                       
  
                                                      
  
              
                                                     
                                                    
                                                  
                                                        
                                                
                                                      
  
            
             
  
                                                                              
 */
static void send_to_client(unifi_priv_t *priv, ul_client_t *client,
        int receiver_id,
        unsigned char *sigdata, int siglen,
        const bulk_data_param_t *bulkdata)
{
    if (client && client->event_hook) {
        /*                        
                                                                          
                                                                  
                                                            */

        client->event_hook(client, sigdata, siglen, bulkdata, UDI_TO_HOST);
    }
}

/*
                                                                              
                        
  
                                        
  
                                                                 
                                        
  
              
                            
                                                                                       
                                                 
                                                                              
                                                                              
                                                                          
                                                 
            
                                                           
                                                              
                                                                              
 */
static u8 check_routing_pkt_data_ind(unifi_priv_t *priv,
        u8 *sigdata,
        const bulk_data_param_t* bulkdata,
        u8 *freeBulkData,
        netInterface_priv_t *interfacePriv)
{
    u16  frmCtrl, receptionStatus, frmCtrlSubType;
    u8 *macHdrLocation;
    u8 interfaceTag;
    u8 isDataFrame;
    u8 isProtocolVerInvalid = FALSE;
    u8 isDataFrameSubTypeNoData = FALSE;

#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
    static const u8 wapiProtocolIdSNAPHeader[] = {0x88,0xb4};
    static const u8 wapiProtocolIdSNAPHeaderOffset = 6;
    u8 *destAddr;
    u8 *srcAddr;
    u8 isWapiUnicastPkt = FALSE;

#ifdef CSR_WIFI_SECURITY_WAPI_QOSCTRL_MIC_WORKAROUND
    u16 qosControl;
#endif

    u8 llcSnapHeaderOffset = 0;

    destAddr = (u8 *) bulkdata->d[0].os_data_ptr + MAC_HEADER_ADDR1_OFFSET;
    srcAddr  = (u8 *) bulkdata->d[0].os_data_ptr + MAC_HEADER_ADDR2_OFFSET;

    /*                                          */
    isWapiUnicastPkt = (!(destAddr[0] & 0x01)) ? TRUE : FALSE;
#endif

#define CSR_WIFI_MA_PKT_IND_RECEPTION_STATUS_OFFSET    sizeof(CSR_SIGNAL_PRIMITIVE_HEADER) + 22

    *freeBulkData = FALSE;

    /*                                                       */
    macHdrLocation = (u8 *) bulkdata->d[0].os_data_ptr;
    /*                                                */
    frmCtrl = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(macHdrLocation);

    /*                                                          */
    interfaceTag = (CSR_GET_UINT16_FROM_LITTLE_ENDIAN(sigdata + 14)) & 0xff;

    /*                                                    */
    receptionStatus = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(sigdata + CSR_WIFI_MA_PKT_IND_RECEPTION_STATUS_OFFSET);

    /*                                                                             */
    isDataFrame = ((frmCtrl & IEEE80211_FC_TYPE_MASK) == (IEEE802_11_FC_TYPE_DATA & IEEE80211_FC_TYPE_MASK)) ? TRUE : FALSE;
    /*                                        */
    isProtocolVerInvalid = (frmCtrl & IEEE80211_FC_PROTO_VERSION_MASK) ? TRUE : FALSE;
    frmCtrlSubType = (frmCtrl & IEEE80211_FC_SUBTYPE_MASK) >> FRAME_CONTROL_SUBTYPE_FIELD_OFFSET;
    /*                                                                    */
    isDataFrameSubTypeNoData = (((frmCtrlSubType>0x03)&&(frmCtrlSubType<0x08)) || (frmCtrlSubType>0x0B)) ? TRUE : FALSE;
    if ((receptionStatus == CSR_MICHAEL_MIC_ERROR) &&
        ((!isDataFrame) || isProtocolVerInvalid || (isDataFrame && isDataFrameSubTypeNoData))) {
        /*                                                                                                             
                                                           
         */
        *freeBulkData = TRUE;
        unifi_trace(priv, UDBG4, "Discarding this frame and ignoring the MIC failure as this is a garbage/non-data/no data frame\n");
        return FALSE;
     }

#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE

    if (receptionStatus == CSR_MICHAEL_MIC_ERROR) {

        if (interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_STA) {

#ifdef CSR_WIFI_SECURITY_WAPI_QOSCTRL_MIC_WORKAROUND
            if ((isDataFrame) &&
                ((IEEE802_11_FC_TYPE_QOS_DATA & IEEE80211_FC_SUBTYPE_MASK) == (frmCtrl & IEEE80211_FC_SUBTYPE_MASK)) &&
                (priv->isWapiConnection))
            {
            	qosControl = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(macHdrLocation + (((frmCtrl & IEEE802_11_FC_TO_DS_MASK) && (frmCtrl & IEEE802_11_FC_FROM_DS_MASK)) ? 30 : 24) );

            	unifi_trace(priv, UDBG4, "check_routing_pkt_data_ind() :: Value of the QoS control field - 0x%04x \n", qosControl);

                if (qosControl & IEEE802_11_QC_NON_TID_BITS_MASK)
                {
                	unifi_trace(priv, UDBG4, "Ignore the MIC failure and pass the MPDU to the stack when any of bits [4-15] is set in the QoS control field\n");

            		/*                                                                      */
            		((bulk_data_param_t*)bulkdata)->d[0].data_length = bulkdata->d[0].data_length - 32;

            		/*                                                         */
            		*(sigdata + CSR_WIFI_MA_PKT_IND_RECEPTION_STATUS_OFFSET)     = 0x00;
            		*(sigdata + CSR_WIFI_MA_PKT_IND_RECEPTION_STATUS_OFFSET+1)   = 0x00;

            		*freeBulkData = FALSE;

            		return FALSE;
                }
            }
#endif
            /*                                                         
                                                                                                                                                              
                                                                                                                                
                                                               
             */
#ifndef CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION
    	if ((priv->wapi_multicast_filter == 0) || isWapiUnicastPkt) {
#else
        /*                                                                                    
                                                 */
    	if ( ((priv->wapi_multicast_filter == 0) && !isWapiUnicastPkt) ||
             ((priv->wapi_unicast_filter   == 0) &&  isWapiUnicastPkt) ) {
#endif
                /*                 */
                *freeBulkData = TRUE;
                unifi_trace(priv, UDBG4, "Discarding the contents of the frame with MIC failure \n");

                if (isWapiUnicastPkt &&
                    ((uf_sme_port_state(priv,srcAddr,UF_CONTROLLED_PORT_Q,interfaceTag) != CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_OPEN)||
#ifndef CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION
                    (priv->wapi_unicast_filter) ||
#endif
                    (priv->wapi_unicast_queued_pkt_filter))) {

                    /*                                                                                             
                                                                                                                  
                                                                                          
                                                         
                                                                   
                                                                            
                     */
                    unifi_trace(priv, UDBG4, "Ignoring the MIC failure as either a. CONTROL PORT isn't OPEN or b. Unicast filter is set or c. WAPI AP using old key for buffered pkts\n");

                    /*                       */
                    return FALSE;

                }/*                               */

                unifi_trace(priv, UDBG6, "check_routing_pkt_data_ind - MIC FAILURE : interfaceTag %x Src Addr %x:%x:%x:%x:%x:%x\n",
                            interfaceTag, srcAddr[0], srcAddr[1], srcAddr[2], srcAddr[3], srcAddr[4], srcAddr[5]);
                unifi_trace(priv, UDBG6, "check_routing_pkt_data_ind - MIC FAILURE : Dest Addr %x:%x:%x:%x:%x:%x\n",
                            destAddr[0], destAddr[1], destAddr[2], destAddr[3], destAddr[4], destAddr[5]);
                unifi_trace(priv, UDBG6, "check_routing_pkt_data_ind - MIC FAILURE : Control Port State - 0x%.4X \n",
                            uf_sme_port_state(priv,srcAddr,UF_CONTROLLED_PORT_Q,interfaceTag));

                unifi_error(priv, "MIC failure in %s\n", __FUNCTION__);

                /*                                 */
                return TRUE;
            }
        }/*          */
        else {
            /*                           */
            *freeBulkData = TRUE;
            unifi_error(priv, "MIC failure in %s\n", __FUNCTION__);
            return TRUE;
         } /*         */
    }/*           */
#else
    if (receptionStatus == CSR_MICHAEL_MIC_ERROR) {
        *freeBulkData = TRUE;
        unifi_error(priv, "MIC failure in %s\n", __FUNCTION__);
        return TRUE;
    }
#endif /*                             */

    unifi_trace(priv, UDBG4, "frmCtrl = 0x%04x %s\n",
                frmCtrl,
                (((frmCtrl & 0x000c)>>FRAME_CONTROL_TYPE_FIELD_OFFSET) == IEEE802_11_FRAMETYPE_MANAGEMENT) ?
                    "Mgt" : "Ctrl/Data");

#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
    /*                                                                                               
                                                                          
     */
    if ((interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_STA)&&
        isWapiUnicastPkt &&
        (receptionStatus == CSR_RX_SUCCESS) &&
        (priv->wapi_unicast_queued_pkt_filter==1)) {

        unifi_trace(priv, UDBG6, "check_routing_pkt_data_ind(): WAPI unicast pkt received when the (wapi_unicast_queued_pkt_filter) is set\n");

        if (isDataFrame) {
            switch(frmCtrl & IEEE80211_FC_SUBTYPE_MASK) {
                case IEEE802_11_FC_TYPE_QOS_DATA & IEEE80211_FC_SUBTYPE_MASK:
                    llcSnapHeaderOffset = MAC_HEADER_SIZE + 2;
                    break;
                case IEEE802_11_FC_TYPE_QOS_NULL & IEEE80211_FC_SUBTYPE_MASK:
                case IEEE802_11_FC_TYPE_NULL & IEEE80211_FC_SUBTYPE_MASK:
                    break;
                default:
                    llcSnapHeaderOffset = MAC_HEADER_SIZE;
            }
        }

        if (llcSnapHeaderOffset > 0) {
        	/*                  */
            unifi_trace(priv, UDBG6, "check_routing_pkt_data_ind(): SNAP header found & its offset %d\n",llcSnapHeaderOffset);
            if (memcmp((u8 *)(bulkdata->d[0].os_data_ptr+llcSnapHeaderOffset+wapiProtocolIdSNAPHeaderOffset),
                       wapiProtocolIdSNAPHeader,sizeof(wapiProtocolIdSNAPHeader))) {

            	unifi_trace(priv, UDBG6, "check_routing_pkt_data_ind(): This is a data & NOT a WAI protocol packet\n");
                /*                                                                                                */
                priv->wapi_unicast_queued_pkt_filter = 0;
                unifi_trace(priv, UDBG4, "check_routing_pkt_data_ind(): WAPI AP has started using the new unicast key, no more MIC failures expected (reset filter)\n");
            }
            else {
                unifi_trace(priv, UDBG6, "check_routing_pkt_data_ind(): WAPI - This is a WAI protocol packet\n");
            }
        }
	}
#endif


    switch ((frmCtrl & 0x000c)>>FRAME_CONTROL_TYPE_FIELD_OFFSET) {
        case IEEE802_11_FRAMETYPE_MANAGEMENT:
            *freeBulkData = TRUE;       /*                                    */

            /*                                                       */
            if((interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_P2P)&&\
               ((CSR_WIFI_80211_GET_FRAME_SUBTYPE(macHdrLocation)) == CSR_WIFI_80211_FRAME_SUBTYPE_BEACON)){

                u8 *pSsid, *pSsidLen;
                static u8 P2PWildCardSsid[CSR_WIFI_P2P_WILDCARD_SSID_LENGTH] = {'D', 'I', 'R', 'E', 'C', 'T', '-'};

                pSsidLen = macHdrLocation + MAC_HEADER_SIZE + CSR_WIFI_BEACON_FIXED_LENGTH;
                pSsid = pSsidLen + 2;

                if(*(pSsidLen + 1) >= CSR_WIFI_P2P_WILDCARD_SSID_LENGTH){
                    if(memcmp(pSsid, P2PWildCardSsid, CSR_WIFI_P2P_WILDCARD_SSID_LENGTH) == 0){
                        unifi_trace(priv, UDBG6, "Received a P2P Beacon, pass it to SME\n");
                        return TRUE;
                    }
                }
                unifi_trace(priv, UDBG6, "Received a Legacy AP beacon in P2P mode, drop it\n");
                return FALSE;
            }
            return TRUE;                /*              */
        case IEEE802_11_FRAMETYPE_DATA:
        case IEEE802_11_FRAMETYPE_CONTROL:
            *freeBulkData = FALSE;      /*                                     */
            return FALSE;
        default:
            unifi_error(priv, "Unhandled frame type %04x\n", frmCtrl);
            *freeBulkData = TRUE;       /*                                  */
            return FALSE;
    }
}

/*
                                                                              
                               
  
                                        
  
                                                                 
                                        
  
              
                                                     
                                                        
                                                
                                                      
  
            
             
  
          
                                                                          
                                                                          
                                                                          
                                                                      
                                                                  
                                                                    
                                               
                                                                              
 */
static void
unifi_process_receive_event(void *ospriv,
                            u8 *sigdata, u32 siglen,
                            const bulk_data_param_t *bulkdata)
{
    unifi_priv_t *priv = (unifi_priv_t*)ospriv;
    int i, receiver_id;
    int client_id;
    s16 signal_id;
    u8 pktIndToSme = FALSE, freeBulkData = FALSE;

    unifi_trace(priv, UDBG5, "unifi_process_receive_event: "
                "%04x %04x %04x %04x %04x %04x %04x %04x (%d)\n",
                CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*0) & 0xFFFF,
                CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*1) & 0xFFFF,
                CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*2) & 0xFFFF,
                CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*3) & 0xFFFF,
                CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*4) & 0xFFFF,
                CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*5) & 0xFFFF,
                CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*6) & 0xFFFF,
                CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*7) & 0xFFFF,
                siglen);

    receiver_id = CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)) & 0xFF00;
    client_id = (receiver_id & 0x0F00) >> UDI_SENDER_ID_SHIFT;
    signal_id = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(sigdata);



    /*                                                                            */
    if (signal_id == CSR_MA_PACKET_INDICATION_ID)
    {
#define CSR_MA_PACKET_INDICATION_INTERFACETAG_OFFSET    14
        u8 interfaceTag;
        netInterface_priv_t *interfacePriv;

        /*                                                          */
        interfaceTag = (CSR_GET_UINT16_FROM_LITTLE_ENDIAN(sigdata + CSR_MA_PACKET_INDICATION_INTERFACETAG_OFFSET)) & 0xff;
        interfacePriv = priv->interfacePriv[interfaceTag];

        /*                                                  */
#ifdef CSR_SUPPORT_SME
        if (interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_IBSS)
        {
            u8 *saddr;
            /*                                           */
            saddr = (u8 *) bulkdata->d[0].os_data_ptr + MAC_HEADER_ADDR2_OFFSET;
            unifi_trace(priv, UDBG5,
                                    "Updating sta activity in IBSS interfaceTag %x Src Addr %x:%x:%x:%x:%x:%x\n",
                                    interfaceTag, saddr[0], saddr[1], saddr[2], saddr[3], saddr[4], saddr[5]);

            uf_update_sta_activity(priv, interfaceTag, saddr);
        }
#endif

        pktIndToSme = check_routing_pkt_data_ind(priv, sigdata, bulkdata, &freeBulkData, interfacePriv);

        unifi_trace(priv, UDBG6, "RX: packet entry point to driver from HIP,pkt to SME ?(%s) \n", (pktIndToSme)? "YES":"NO");

    }

    if (pktIndToSme)
    {
        /*                                  */
        if(sigdata != NULL && bulkdata != NULL){
            send_to_client(priv, priv->sme_cli, receiver_id, sigdata, siglen, bulkdata);
        }
        else{
            unifi_error(priv, "unifi_receive_event2: sigdata or Bulkdata is NULL \n");
        }
#ifdef CSR_NATIVE_LINUX
        send_to_client(priv, priv->wext_client,
                receiver_id,
                sigdata, siglen, bulkdata);
#endif
    }
    else
    {
        /*                                                            
                                                                               
         */
        if (!receiver_id) {
               if(signal_id == CSR_MA_VIF_AVAILABILITY_INDICATION_ID) {
                      uf_process_ma_vif_availibility_ind(priv, sigdata, siglen);
               }
               else if (signal_id != CSR_MA_PACKET_INDICATION_ID) {
                      send_to_client(priv, priv->sme_cli, receiver_id, sigdata, siglen, bulkdata);
#ifdef CSR_NATIVE_LINUX
                      send_to_client(priv, priv->wext_client,
                                     receiver_id,
                                     sigdata, siglen, bulkdata);
#endif
               }
               else
               {

#if (defined(CSR_SUPPORT_SME) && defined(CSR_WIFI_SECURITY_WAPI_ENABLE))
                   #define CSR_MA_PACKET_INDICATION_RECEPTION_STATUS_OFFSET    sizeof(CSR_SIGNAL_PRIMITIVE_HEADER) + 22
                   netInterface_priv_t *interfacePriv;
                   u8 interfaceTag;
                   u16 receptionStatus = CSR_RX_SUCCESS;

                   /*                                                          */
                   interfaceTag = (CSR_GET_UINT16_FROM_LITTLE_ENDIAN(sigdata + CSR_MA_PACKET_INDICATION_INTERFACETAG_OFFSET)) & 0xff;
                   interfacePriv = priv->interfacePriv[interfaceTag];

                   /*                       */
                   receptionStatus = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(sigdata + CSR_MA_PACKET_INDICATION_RECEPTION_STATUS_OFFSET);

                   /*                                                                               */
                   if ((!freeBulkData) &&
                       (interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_STA) &&
                       (receptionStatus == CSR_MICHAEL_MIC_ERROR) &&
                       ((priv->wapi_multicast_filter == 1)
#ifdef CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION
                         || (priv->wapi_unicast_filter == 1)
#endif
                       ))
                   {
                       CSR_SIGNAL signal;
                       u8 *destAddr;
                       CsrResult res;
                       u16 interfaceTag = 0;
                       u8 isMcastPkt = TRUE;

                       unifi_trace(priv, UDBG6, "Received a WAPI data packet when the Unicast/Multicast filter is set\n");
                       res = read_unpack_signal(sigdata, &signal);
                       if (res) {
                           unifi_error(priv, "Received unknown or corrupted signal (0x%x).\n",
                                       CSR_GET_UINT16_FROM_LITTLE_ENDIAN(sigdata));
                           return;
                       }

                       /*                                                           */
                       destAddr = (u8 *) bulkdata->d[0].os_data_ptr + MAC_HEADER_ADDR1_OFFSET;
                       isMcastPkt = (destAddr[0] & 0x01) ? TRUE : FALSE;
                       unifi_trace(priv, UDBG6,
                                   "1.MPDU type: (%s), 2.Multicast filter: (%s), 3. Unicast filter: (%s)\n",
                                   ((isMcastPkt) ? "Multiast":"Unicast"),
                                   ((priv->wapi_multicast_filter) ? "Enabled":"Disabled"),
                                   ((priv->wapi_unicast_filter)  ? "Enabled":"Disabled"));

                       if (((isMcastPkt) && (priv->wapi_multicast_filter == 1))
#ifdef CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION
                           || ((!isMcastPkt) && (priv->wapi_unicast_filter == 1))
#endif
                          )
                        {
                            unifi_trace(priv, UDBG4, "Sending the WAPI MPDU for MIC check\n");
                            CsrWifiRouterCtrlWapiRxMicCheckIndSend(priv->CSR_WIFI_SME_IFACEQUEUE, 0, interfaceTag, siglen, sigdata, bulkdata->d[0].data_length, (u8*)bulkdata->d[0].os_data_ptr);

                            for (i = 0; i < UNIFI_MAX_DATA_REFERENCES; i++) {
                                if (bulkdata->d[i].data_length != 0) {
                                    unifi_net_data_free(priv, (void *)&bulkdata->d[i]);
                                }
                           }
                           return;
                       }
                   } /*                             */
#endif /*                                                */
               }
        }

        /*                                                    
                                                                                   
                                                                        
          
                                                                                      
                                                                                  
         */
        if(!freeBulkData){
            if ((client_id < MAX_UDI_CLIENTS) &&
                    (&priv->ul_clients[client_id] != priv->logging_client)) {
            	unifi_trace(priv, UDBG6, "Call the registered clients handler callback func\n");
                send_to_client(priv, &priv->ul_clients[client_id],
                        receiver_id,
                        sigdata, siglen, bulkdata);
            }
        }
    }

    /*
                                                                          
     */
    switch (signal_id)
    {
#ifdef UNIFI_SNIFF_ARPHRD
        case CSR_MA_SNIFFDATA_INDICATION_ID:
#endif
            break;

        case CSR_MA_PACKET_INDICATION_ID:
            if (!freeBulkData)
            {
                break;
            }
            /*                  */
        default:
            for (i = 0; i < UNIFI_MAX_DATA_REFERENCES; i++) {
                if (bulkdata->d[i].data_length != 0) {
                    unifi_net_data_free(priv, (void *)&bulkdata->d[i]);
                }
            }
    }

} /*                               */


#ifdef CSR_WIFI_RX_PATH_SPLIT
static u8 signal_buffer_is_full(unifi_priv_t* priv)
{
    return (((priv->rxSignalBuffer.writePointer + 1)% priv->rxSignalBuffer.size) == (priv->rxSignalBuffer.readPointer));
}

void unifi_rx_queue_flush(void *ospriv)
{
    unifi_priv_t *priv = (unifi_priv_t*)ospriv;

    unifi_trace(priv, UDBG4, "rx_wq_handler: RdPtr = %d WritePtr =  %d\n",
                priv->rxSignalBuffer.readPointer,priv->rxSignalBuffer.writePointer);
    if(priv != NULL) {
        u8 readPointer = priv->rxSignalBuffer.readPointer;
        while (readPointer != priv->rxSignalBuffer.writePointer)
        {
             rx_buff_struct_t *buf = &priv->rxSignalBuffer.rx_buff[readPointer];
             unifi_trace(priv, UDBG6, "rx_wq_handler: RdPtr = %d WritePtr =  %d\n",
                         readPointer,priv->rxSignalBuffer.writePointer);
             unifi_process_receive_event(priv, buf->bufptr, buf->sig_len, &buf->data_ptrs);
             readPointer ++;
             if(readPointer >= priv->rxSignalBuffer.size) {
                    readPointer = 0;
             }
        }
        priv->rxSignalBuffer.readPointer = readPointer;
    }
}

void rx_wq_handler(struct work_struct *work)
{
    unifi_priv_t *priv = container_of(work, unifi_priv_t, rx_work_struct);
    unifi_rx_queue_flush(priv);
}
#endif



/*
                                                                              
                       
  
                                        
  
                                                                 
                                        
  
              
                                                     
                                                        
                                                
                                                      
  
            
             
  
          
                                                                          
                                                                          
                                                                          
                                                                      
                                                                  
                                                                    
                                               
                                                                              
 */
void
unifi_receive_event(void *ospriv,
                    u8 *sigdata, u32 siglen,
                    const bulk_data_param_t *bulkdata)
{
#ifdef CSR_WIFI_RX_PATH_SPLIT
    unifi_priv_t *priv = (unifi_priv_t*)ospriv;
    u8 writePointer;
    int i;
    rx_buff_struct_t * rx_buff;

    unifi_trace(priv, UDBG5, "unifi_receive_event: "
            "%04x %04x %04x %04x %04x %04x %04x %04x (%d)\n",
            CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*0) & 0xFFFF,
            CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*1) & 0xFFFF,
            CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*2) & 0xFFFF,
            CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*3) & 0xFFFF,
            CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*4) & 0xFFFF,
            CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*5) & 0xFFFF,
            CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*6) & 0xFFFF,
            CSR_GET_UINT16_FROM_LITTLE_ENDIAN((sigdata) + sizeof(s16)*7) & 0xFFFF, siglen);
    if(signal_buffer_is_full(priv)) {
        unifi_error(priv,"TO HOST signal queue FULL dropping the PDU\n");
        for (i = 0; i < UNIFI_MAX_DATA_REFERENCES; i++) {
            if (bulkdata->d[i].data_length != 0) {
                unifi_net_data_free(priv, (void *)&bulkdata->d[i]);
            }
        }
        return;
    }
    writePointer = priv->rxSignalBuffer.writePointer;
    rx_buff = &priv->rxSignalBuffer.rx_buff[writePointer];
    memcpy(rx_buff->bufptr,sigdata,siglen);
    rx_buff->sig_len = siglen;
    rx_buff->data_ptrs = *bulkdata;
    writePointer++;
    if(writePointer >= priv->rxSignalBuffer.size) {
        writePointer =0;
    }
    unifi_trace(priv, UDBG4, "unifi_receive_event:writePtr = %d\n",priv->rxSignalBuffer.writePointer);
    priv->rxSignalBuffer.writePointer = writePointer;

#ifndef CSR_WIFI_RX_PATH_SPLIT_DONT_USE_WQ
    queue_work(priv->rx_workqueue, &priv->rx_work_struct);
#endif

#else
    unifi_process_receive_event(ospriv, sigdata, siglen, bulkdata);
#endif
} /*                       */

