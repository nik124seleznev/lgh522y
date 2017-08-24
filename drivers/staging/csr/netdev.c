/*
 * ---------------------------------------------------------------------------
 * FILE:     netdev.c
 *
 * PURPOSE:
 *      This file provides the upper edge interface to the linux netdevice
 *      and wireless extensions.
 *      It is part of the porting exercise.
 *
 * Copyright (C) 2005-2010 by Cambridge Silicon Radio Ltd.
 *
 * Refer to LICENSE.txt included with this source code for details on
 * the license terms.
 *
 * ---------------------------------------------------------------------------
 */

/*
                 
                                                                 
  
                                                               
                                                                    
                                                                           
                                                                         
                                                                              
                                                                             
                                                                         
                                                                           
                                                                          
                                                                        
                        
  
                                                                               
                                                                           
                                                                             
                                                                       
                                                                         
                           
  
                                                                   
                                                                           
                              
 */

#include <linux/types.h>
#include <linux/etherdevice.h>
#include <linux/mutex.h>
#include <linux/semaphore.h>
#include <linux/vmalloc.h>
#include "csr_wifi_hip_unifi.h"
#include "csr_wifi_hip_conversions.h"
#include "unifi_priv.h"
#include <net/pkt_sched.h>


/*                                                               */
#ifdef CSR_SUPPORT_WEXT
extern struct iw_handler_def unifi_iw_handler_def;
#endif /*                  */
static void check_ba_frame_age_timeout( unifi_priv_t *priv,
                                            netInterface_priv_t *interfacePriv,
                                            ba_session_rx_struct *ba_session);
static void process_ba_frame(unifi_priv_t *priv,
                             netInterface_priv_t *interfacePriv,
                             ba_session_rx_struct *ba_session,
                             frame_desc_struct *frame_desc);
static void process_ba_complete(unifi_priv_t *priv, netInterface_priv_t *interfacePriv);
static void process_ma_packet_error_ind(unifi_priv_t *priv, CSR_SIGNAL *signal, bulk_data_param_t *bulkdata);
static void process_amsdu(unifi_priv_t *priv, CSR_SIGNAL *signal, bulk_data_param_t *bulkdata);
static int uf_net_open(struct net_device *dev);
static int uf_net_ioctl(struct net_device *dev, struct ifreq *rq, int cmd);
static int uf_net_stop(struct net_device *dev);
static struct net_device_stats *uf_net_get_stats(struct net_device *dev);
static u16 uf_net_select_queue(struct net_device *dev, struct sk_buff *skb);
static netdev_tx_t uf_net_xmit(struct sk_buff *skb, struct net_device *dev);
static void uf_set_multicast_list(struct net_device *dev);


typedef int (*tx_signal_handler)(unifi_priv_t *priv, struct sk_buff *skb, const struct ethhdr *ehdr, CSR_PRIORITY priority);

#ifdef CONFIG_NET_SCHED
/*
                                
                                                          
 */

/*
                                                                
                                                                          
                                                                       
                        
  
                                                                          
                                                                          
                                                      
                                                                         
                                                                       
                                                                    
                                                                            
                                            
  
                                                                     
                                                                      
                                                         
  
 */

struct uf_sched_data
{
    /*                        */
    struct tcf_proto *filter_list;
    /*                */
    struct Qdisc *queues[UNIFI_TRAFFIC_Q_MAX];
};

struct uf_tx_packet_data {
    /*                               */
    unifi_TrafficQueue queue;
    /*                                              */
    CSR_PRIORITY priority;
    /*       */
    unsigned long host_tag;
};

#endif /*                  */

static const struct net_device_ops uf_netdev_ops =
{
    .ndo_open = uf_net_open,
    .ndo_stop = uf_net_stop,
    .ndo_start_xmit = uf_net_xmit,
    .ndo_do_ioctl = uf_net_ioctl,
    .ndo_get_stats = uf_net_get_stats, /*                         */
    .ndo_set_rx_mode = uf_set_multicast_list,
    .ndo_select_queue = uf_net_select_queue,
};

static u8 oui_rfc1042[P80211_OUI_LEN] = { 0x00, 0x00, 0x00 };
static u8 oui_8021h[P80211_OUI_LEN]   = { 0x00, 0x00, 0xf8 };


/*                                                */
static void netdev_mlme_event_handler(ul_client_t  *client,
                                      const u8 *sig_packed, int sig_len,
                                      const bulk_data_param_t *bulkdata,
                                      int dir);

#ifdef CSR_SUPPORT_WEXT
/*                                                                  
                            
 */
static struct notifier_block uf_netdev_notifier;
#endif

/*
                                                                              
                      
  
                                                                     
                           
                                                                 
                                             
  
              
                                                                     
                                 
                                                                               
                                                                              
                                                               
  
            
                                         
  
          
                                                                        
                                                              
                                                                              
 */
unifi_priv_t *
uf_alloc_netdevice(CsrSdioFunction *sdio_dev, int bus_id)
{
    struct net_device *dev;
    unifi_priv_t *priv;
    netInterface_priv_t *interfacePriv;
#ifdef CSR_SUPPORT_WEXT
    int rc;
#endif
    unsigned char i; /*            */

    /*
                                                          
                                   
                                                                        
                                            
                                                                               
                                                         
     */
    dev = alloc_etherdev_mq(sizeof(unifi_priv_t) + sizeof(netInterface_priv_t), UNIFI_TRAFFIC_Q_MAX);

    if (dev == NULL) {
        return NULL;
    }

    /*                                         */
    interfacePriv = (netInterface_priv_t *)netdev_priv(dev);
    priv = (unifi_priv_t *)(interfacePriv + 1);
    interfacePriv->privPtr = priv;
    interfacePriv->InterfaceTag = 0;


    /*                                                      */
    for(i=0; i<CSR_WIFI_NUM_INTERFACES; i++) {
        priv->netdev[i] = NULL;
        priv->interfacePriv[i] = NULL;
    }
    priv->netdev[0] = dev;
    priv->interfacePriv[0] = interfacePriv;

    /*                                    */
    dev->netdev_ops = &uf_netdev_ops;

#ifdef CSR_SUPPORT_WEXT
    dev->wireless_handlers = &unifi_iw_handler_def;
#if IW_HANDLER_VERSION < 6
    dev->get_wireless_stats = unifi_get_wireless_stats;
#endif /*                    */
#endif /*                  */

    /*                                                        */
    dev->needed_headroom = 32;

    /*                               */
    priv->instance = bus_id;
    /*                                        */
    priv->sdio = sdio_dev;

    sdio_dev->driverData = (void*)priv;
    /*                                    */
    priv->init_progress = UNIFI_INIT_NONE;

    priv->prev_queue = 0;

    /*
                                              
                                                                 
                                                                      
                                                                          
                           
     */
    ul_init_clients(priv);

    /*
                                                                   
                                                            
     */
    priv->netdev_client = ul_register_client(priv,
            0,
            netdev_mlme_event_handler);
    if (priv->netdev_client == NULL) {
        unifi_error(priv,
                "Failed to register a unifi client for background netdev processing\n");
        free_netdev(priv->netdev[0]);
        return NULL;
    }
    unifi_trace(priv, UDBG2, "Netdev %p client (id:%d s:0x%X) is registered\n",
            dev, priv->netdev_client->client_id, priv->netdev_client->sender_id);

    priv->sta_wmm_capabilities = 0;

#if (defined(CSR_WIFI_SECURITY_WAPI_ENABLE) && defined(CSR_SUPPORT_SME))
    priv->wapi_multicast_filter = 0;
    priv->wapi_unicast_filter = 0;
    priv->wapi_unicast_queued_pkt_filter = 0;
#ifdef CSR_WIFI_SECURITY_WAPI_QOSCTRL_MIC_WORKAROUND
    priv->isWapiConnection = FALSE;
#endif
#endif

    /*                              */
    interfacePriv->queueEnabled[0] = 1;
    interfacePriv->queueEnabled[1] = 1;
    interfacePriv->queueEnabled[2] = 1;
    interfacePriv->queueEnabled[3] = 1;

#ifdef CSR_SUPPORT_SME
    priv->allPeerDozing = 0;
#endif
    /*
                                        
     */
    /*
                                                                            
                                  
     */
    if (use_5g) {
        priv->if_index = CSR_INDEX_5G;
        unifi_info(priv, "Using the 802.11a radio\n");
    } else {
        priv->if_index = CSR_INDEX_2G4;
    }

    /*                                */
    priv->bh_thread.thread_task = NULL;
    priv->bh_thread.block_thread = 1;
    init_waitqueue_head(&priv->bh_thread.wakeup_q);
    priv->bh_thread.wakeup_flag = 0;
    sprintf(priv->bh_thread.name, "uf_bh_thread");

    /*                                             */
    interfacePriv->connected = UnifiConnectedUnknown;  /*                         */

#ifdef USE_DRIVER_LOCK
    sema_init(&priv->lock, 1);
#endif /*                 */

    spin_lock_init(&priv->send_signal_lock);

    spin_lock_init(&priv->m4_lock);
    sema_init(&priv->ba_mutex, 1);

#if (defined(CSR_WIFI_SECURITY_WAPI_ENABLE) && defined(CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION))
    spin_lock_init(&priv->wapi_lock);
#endif

#ifdef CSR_SUPPORT_SME
    spin_lock_init(&priv->staRecord_lock);
    spin_lock_init(&priv->tx_q_lock);
#endif

    /*                                       */
    priv->unifi_workqueue = create_singlethread_workqueue("unifi_workq");
    if (priv->unifi_workqueue == NULL) {
        /*                                */
        ul_deregister_client(priv->netdev_client);
        free_netdev(priv->netdev[0]);
        return NULL;
    }

#ifdef CSR_SUPPORT_SME
    /*                                                    */
    INIT_WORK(&priv->multicast_list_task, uf_multicast_list_wq);

    /*                                    */
    INIT_WORK(&interfacePriv->send_m4_ready_task, uf_send_m4_ready_wq);

#if (defined(CSR_WIFI_SECURITY_WAPI_ENABLE) && defined(CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION))
    /*                                                                                        */
    INIT_WORK(&interfacePriv->send_pkt_to_encrypt, uf_send_pkt_to_encrypt);
#endif
#endif

    priv->ref_count = 1;

    priv->amp_client = NULL;
    priv->coredump_mode = 0;
    priv->ptest_mode = 0;
    priv->wol_suspend = FALSE;
    INIT_LIST_HEAD(&interfacePriv->rx_uncontrolled_list);
    INIT_LIST_HEAD(&interfacePriv->rx_controlled_list);
    sema_init(&priv->rx_q_sem, 1);

#ifdef CSR_SUPPORT_WEXT
    interfacePriv->netdev_callback_registered = FALSE;
    interfacePriv->wait_netdev_change = FALSE;
    /*                                               */
    if ((rc = register_netdevice_notifier(&uf_netdev_notifier)) == 0) {
        interfacePriv->netdev_callback_registered = TRUE;
    }
    else {
        unifi_warning(priv, "Failed to register netdevice notifier : %d %p\n", rc, dev);
    }
#endif /*                  */

#ifdef CSR_WIFI_SPLIT_PATCH
    /*                              */
    priv->pending_mode_set.common.destination = 0xaaaa;
#endif

    return priv;
} /*                      */

/*
                                                                             
                                           
  
                                                                     
                           
                                                                 
                                             
  
              
                                        
                                                                     
                                 
                                                                               
                                                                              
                                                               
  
            
                                         
  
          
                                                                                            
                                                                
                                                                        
                                                              
                                                                              
 */
u8
uf_alloc_netdevice_for_other_interfaces(unifi_priv_t *priv, u16 interfaceTag)
{
    struct net_device *dev;
    netInterface_priv_t *interfacePriv;

    /*
                                                          
                                   
                                                                        
                                            
                                                                               
                                                         
     */
    dev = alloc_etherdev_mq(sizeof(netInterface_priv_t), 1);
    if (dev == NULL) {
        return FALSE;
    }

    if (interfaceTag >= CSR_WIFI_NUM_INTERFACES) {
        unifi_error(priv, "uf_alloc_netdevice_for_other_interfaces bad interfaceTag\n");
        return FALSE;
    }

    /*                                         */
    interfacePriv = (netInterface_priv_t *)netdev_priv(dev);
    interfacePriv->privPtr = priv;
    interfacePriv->InterfaceTag = interfaceTag;
    priv->netdev[interfaceTag] = dev;
    priv->interfacePriv[interfacePriv->InterfaceTag] = interfacePriv;

    /*                                             */
    interfacePriv->connected = UnifiConnectedUnknown;  /*                         */
    INIT_LIST_HEAD(&interfacePriv->rx_uncontrolled_list);
    INIT_LIST_HEAD(&interfacePriv->rx_controlled_list);

    /*                                    */
    dev->netdev_ops = &uf_netdev_ops;

#ifdef CSR_SUPPORT_WEXT
    dev->wireless_handlers = &unifi_iw_handler_def;
#if IW_HANDLER_VERSION < 6
    dev->get_wireless_stats = unifi_get_wireless_stats;
#endif /*                    */
#endif /*                  */
    return TRUE;
} /*                      */



/*
                                                                              
                     
  
                                                                           
                                                        
  
              
                                               
  
            
             
                                                                              
 */
int
uf_free_netdevice(unifi_priv_t *priv)
{
    int i;
    unsigned long flags;

    unifi_trace(priv, UDBG1, "uf_free_netdevice\n");

    if (!priv) {
        return -EINVAL;
    }

    /*
                                                 
     */
    uf_release_firmware_files(priv);

#if (defined CSR_SUPPORT_SME) && (defined CSR_SUPPORT_WEXT)
    if (priv->connection_config.mlmeAssociateReqInformationElements) {
        kfree(priv->connection_config.mlmeAssociateReqInformationElements);
    }
    priv->connection_config.mlmeAssociateReqInformationElements = NULL;
    priv->connection_config.mlmeAssociateReqInformationElementsLength = 0;

    if (priv->mib_data.length) {
        vfree(priv->mib_data.data);
    }
    priv->mib_data.data = NULL;
    priv->mib_data.length = 0;

#endif /*                                    */

    /*                                                    */
    spin_lock_irqsave(&priv->m4_lock, flags);
    for (i = 0; i < CSR_WIFI_NUM_INTERFACES; i++) {
        netInterface_priv_t *interfacePriv = priv->interfacePriv[i];
        if (interfacePriv->m4_bulk_data.data_length > 0) {
            unifi_trace(priv, UDBG5, "uf_free_netdevice: free M4 bulkdata %d\n", i);
            unifi_net_data_free(priv, &interfacePriv->m4_bulk_data);
        }
    }
    spin_unlock_irqrestore(&priv->m4_lock, flags);

#if (defined(CSR_WIFI_SECURITY_WAPI_ENABLE) && defined(CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION))
    /*                                                    */
    spin_lock_irqsave(&priv->wapi_lock, flags);
    for (i = 0; i < CSR_WIFI_NUM_INTERFACES; i++) {
        netInterface_priv_t *interfacePriv = priv->interfacePriv[i];
        if (interfacePriv->wapi_unicast_bulk_data.data_length > 0) {
            unifi_trace(priv, UDBG5, "uf_free_netdevice: free WAPI PKT bulk data %d\n", i);
            unifi_net_data_free(priv, &interfacePriv->wapi_unicast_bulk_data);
        }
    }
    spin_unlock_irqrestore(&priv->wapi_lock, flags);
#endif

#ifdef CSR_SUPPORT_WEXT
    /*                                                 */
    unregister_netdevice_notifier(&uf_netdev_notifier);
#endif /*                  */

#ifdef CSR_SUPPORT_SME
    /*                                             */
    cancel_work_sync(&priv->multicast_list_task);
#endif
/*                         */
    flush_workqueue(priv->unifi_workqueue);
    destroy_workqueue(priv->unifi_workqueue);

    /*                                                                   
                                                                         
     */
    for (i=CSR_WIFI_NUM_INTERFACES-1; i>=0; i--) {
        /*                                                       */
        if (priv->netdev[i]) {
            unifi_error(priv, "uf_free_netdevice: netdev %d %p\n", i, priv->netdev[i]);
            free_netdev(priv->netdev[i]);
        }
    }

    return 0;
} /*                     */


/*
                                                                              
               
  
                                                      
  
              
                                       
  
            
             
                                                                              
 */
static int
uf_net_open(struct net_device *dev)
{
    netInterface_priv_t *interfacePriv = (netInterface_priv_t *)netdev_priv(dev);
    unifi_priv_t *priv = interfacePriv->privPtr;

    /*                                                             */
    if (priv->init_progress != UNIFI_INIT_COMPLETED) {
        unifi_warning(priv, "%s: unifi not ready, failing net_open\n", __FUNCTION__);
        return -EINVAL;
    }

#if (defined CSR_NATIVE_LINUX) && (defined UNIFI_SNIFF_ARPHRD) && defined(CSR_SUPPORT_WEXT)
    /*
                                                                     
                                               
                                                                   
                                                                   
                  
     */
    if (priv->wext_conf.mode == IW_MODE_MONITOR) {
        int err;
        err = uf_start_sniff(priv);
        if (err) {
            return err;
        }
        netif_carrier_on(dev);
    }
#endif

#ifdef CSR_SUPPORT_WEXT
    if (interfacePriv->wait_netdev_change) {
        unifi_trace(priv, UDBG1, "%s: Waiting for NETDEV_CHANGE, assume connected\n",
                    __FUNCTION__);
        interfacePriv->connected = UnifiConnected;
        interfacePriv->wait_netdev_change = FALSE;
    }
#endif

    netif_tx_start_all_queues(dev);

    return 0;
} /*               */


static int
uf_net_stop(struct net_device *dev)
{
#if defined(CSR_NATIVE_LINUX) && defined(UNIFI_SNIFF_ARPHRD) && defined(CSR_SUPPORT_WEXT)
    netInterface_priv_t *interfacePriv = (netInterface_priv_t*)netdev_priv(dev);
    unifi_priv_t *priv = interfacePriv->privPtr;

    /*                                  */
    if (priv->wext_conf.mode == IW_MODE_MONITOR) {
        if (priv->card) {
            int err;
            err = unifi_reset_state(priv, dev->dev_addr, 1);
            if (err) {
                return err;
            }
        }
    }
#endif

    netif_tx_stop_all_queues(dev);

    return 0;
} /*               */


/*                                      */
static int
uf_net_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
    int rc;

    rc = -EOPNOTSUPP;

    return rc;
} /*                */



static struct net_device_stats *
uf_net_get_stats(struct net_device *dev)
{
    netInterface_priv_t *interfacePriv = (netInterface_priv_t *)netdev_priv(dev);

    return &interfacePriv->stats;
} /*                    */

static CSR_PRIORITY uf_get_packet_priority(unifi_priv_t *priv, netInterface_priv_t *interfacePriv, struct sk_buff *skb, const int proto)
{
    CSR_PRIORITY priority = CSR_CONTENTION;

    priority = (CSR_PRIORITY) (skb->priority >> 5);

    if (priority == CSR_QOS_UP0) { /*   */

        unifi_trace(priv, UDBG5, "uf_get_packet_priority: proto = 0x%.4X\n", proto);

        switch (proto) {
            case 0x0800:        /*      */
            case 0x814C:        /*      */
            case 0x880C:        /*      */
                priority = (CSR_PRIORITY) (skb->data[1 + ETH_HLEN] >> 5);
                break;

            case 0x8100:        /*      */
                priority = (CSR_PRIORITY) (skb->data[0 + ETH_HLEN] >> 5);
                break;

            case 0x86DD:        /*      */
                priority = (CSR_PRIORITY) ((skb->data[0 + ETH_HLEN] & 0x0E) >> 1);
                break;

            default:
                priority = CSR_QOS_UP0;
                break;
        }
    }

    /*                                                                                                   
                */
    if (interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_STA ||
        interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_P2PCLI) {
        unifi_TrafficQueue queue;

        /*                                                   
                                                                               */
        queue = unifi_frame_priority_to_queue(priority);

        while (queue > UNIFI_TRAFFIC_Q_BK && !interfacePriv->queueEnabled[queue]) {
            queue--;
            priority = unifi_get_default_downgrade_priority(queue);
        }
    }

    unifi_trace(priv, UDBG5, "Packet priority = %d\n", priority);

    return priority;
}

/*
 */
/*
                                                                              
                       
  
              
                                                               
                                      
                                                          
                                                              
  
  
            
                     
                                                                              
 */
CSR_PRIORITY
get_packet_priority(unifi_priv_t *priv, struct sk_buff *skb, const struct ethhdr *ehdr, netInterface_priv_t *interfacePriv)
{
    CSR_PRIORITY priority = CSR_CONTENTION;
    const int proto = ntohs(ehdr->h_proto);

    u8 interfaceMode = interfacePriv->interfaceMode;

    /*                                    */
    switch(interfaceMode)
    {
        case CSR_WIFI_ROUTER_CTRL_MODE_STA:
        case CSR_WIFI_ROUTER_CTRL_MODE_P2PCLI:
            unifi_trace(priv, UDBG4, "mode is STA \n");
            if ((priv->sta_wmm_capabilities & QOS_CAPABILITY_WMM_ENABLED) == 1) {
                priority = uf_get_packet_priority(priv, interfacePriv, skb, proto);
            } else {
                priority = CSR_CONTENTION;
            }
            break;
#ifdef CSR_SUPPORT_SME
        case CSR_WIFI_ROUTER_CTRL_MODE_AP:
        case CSR_WIFI_ROUTER_CTRL_MODE_P2PGO:
        case CSR_WIFI_ROUTER_CTRL_MODE_IBSS:
            {
                CsrWifiRouterCtrlStaInfo_t * dstStaInfo =
                    CsrWifiRouterCtrlGetStationRecordFromPeerMacAddress(priv,ehdr->h_dest, interfacePriv->InterfaceTag);
                unifi_trace(priv, UDBG4, "mode is AP \n");
                if (!(ehdr->h_dest[0] & 0x01) && dstStaInfo && dstStaInfo->wmmOrQosEnabled) {
                    /*                                      */
                    priority = uf_get_packet_priority(priv, interfacePriv, skb, proto);
                } else {
                    /*                                                                      */
                    unifi_trace(priv, UDBG4, "Destination is not QSTA or BroadCast/Multicast\n");
                    priority = CSR_CONTENTION;
                }
            }
            break;
#endif
        default:
            unifi_trace(priv, UDBG3, " mode unknown in %s func, mode=%x\n", __FUNCTION__, interfaceMode);
    }
    unifi_trace(priv, UDBG5, "priority = %x\n", priority);

    return priority;
}

/*
                                                                              
                       
  
                                                                       
  
              
                                      
                              
  
            
                   
                                                                              
 */
static u16
uf_net_select_queue(struct net_device *dev, struct sk_buff *skb)
{
    netInterface_priv_t *interfacePriv = (netInterface_priv_t *)netdev_priv(dev);
    unifi_priv_t *priv = (unifi_priv_t *)interfacePriv->privPtr;
    struct ethhdr ehdr;
    unifi_TrafficQueue queue;
    int proto;
    CSR_PRIORITY priority;

    memcpy(&ehdr, skb->data, ETH_HLEN);
    proto = ntohs(ehdr.h_proto);

    /*                                                   */
    if ((proto != ETH_P_PAE)
#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
            && (proto != ETH_P_WAI)
#endif
       ) {
        /*              */
        priority = get_packet_priority(priv, skb, &ehdr, interfacePriv);
        queue = unifi_frame_priority_to_queue(priority);
    } else {
        /*         */
        queue = UNIFI_TRAFFIC_Q_EAPOL;
    }


    return (u16)queue;
} /*                       */

int
skb_add_llc_snap(struct net_device *dev, struct sk_buff *skb, int proto)
{
    llc_snap_hdr_t *snap;
    netInterface_priv_t *interfacePriv = (netInterface_priv_t *)netdev_priv(dev);
    unifi_priv_t *priv = interfacePriv->privPtr;
    int headroom;

    /*                                   */
    headroom = skb_headroom(skb);
    /*                                             */

    if (proto < 0x600) {
        /*                                          */
        /*                                            */
        unifi_trace(priv, UDBG3, "802.3 len: %d\n", skb->len);

        /*                              */
        skb_trim(skb, proto);
    } else if (proto == ETH_P_8021Q) {

        /*                                        */
        u16 vlan_snap = *(u16*)skb->data;
        /*                                                               */
        if (headroom < 14) {
            unifi_trace(priv, UDBG3, "cant append vlan snap: debug\n");
            return -1;
        }
        /*                       */
        snap = (llc_snap_hdr_t *)skb_push(skb, 4);
        snap->dsap = snap->ssap = 0xAA;
        snap->ctrl = 0x03;
        memcpy(snap->oui, oui_rfc1042, P80211_OUI_LEN);

        /*                       */
        snap = (llc_snap_hdr_t *)skb_push(skb, 10);
        snap->dsap = snap->ssap = 0xAA;
        snap->ctrl = 0x03;
        memcpy(snap->oui, oui_rfc1042, P80211_OUI_LEN);

        /*                                   */
        snap->protocol = htons(proto);
        *(u16*)(snap + 1) = vlan_snap;

    } else
    {
        /*                                      */
        unifi_trace(priv, UDBG3, "DIXII len: %d\n", skb->len);

        /*                                                 */
        if (headroom < sizeof(llc_snap_hdr_t)) {
            unifi_trace(priv, UDBG3, "cant append snap: debug\n");
            return -1;
        }
        /*              */
        snap = (llc_snap_hdr_t *)skb_push(skb, sizeof(llc_snap_hdr_t));
        snap->dsap = snap->ssap = 0xAA;
        snap->ctrl = 0x03;
        /*                          */
        if ((proto == ETH_P_AARP) || (proto == ETH_P_IPX)) {
            memcpy(snap->oui, oui_8021h, P80211_OUI_LEN);
        } else {
            memcpy(snap->oui, oui_rfc1042, P80211_OUI_LEN);
        }
        snap->protocol = htons(proto);
    }

    return 0;
} /*                    */

#ifdef CSR_SUPPORT_SME
static int
_identify_sme_ma_pkt_ind(unifi_priv_t *priv,
                         const s8 *oui, u16 protocol,
                         const CSR_SIGNAL *signal,
                         bulk_data_param_t *bulkdata,
                         const unsigned char *daddr,
                         const unsigned char *saddr)
{
    CSR_MA_PACKET_INDICATION *pkt_ind = (CSR_MA_PACKET_INDICATION*)&signal->u.MaPacketIndication;
    int r;
    u8 i;

    unifi_trace(priv, UDBG5,
            "_identify_sme_ma_pkt_ind -->\n");
    for (i = 0; i < MAX_MA_UNIDATA_IND_FILTERS; i++) {
        if (priv->sme_unidata_ind_filters[i].in_use) {
            if (!memcmp(oui, priv->sme_unidata_ind_filters[i].oui, 3) &&
                    (protocol == priv->sme_unidata_ind_filters[i].protocol)) {

                /*                */
                if (priv->sme_cli) {
                    /*
                                                                                     
                                                                                      
                     */
                    unifi_trace(priv, UDBG1,
                            "_identify_sme_ma_pkt_ind: handle=%d, encap=%d, proto=%x\n",
                            i, priv->sme_unidata_ind_filters[i].encapsulation,
                            priv->sme_unidata_ind_filters[i].protocol);
                    if (priv->sme_unidata_ind_filters[i].encapsulation == CSR_WIFI_ROUTER_ENCAPSULATION_ETHERNET) {
                        struct sk_buff *skb;
                        /*                                        */
                        skb = (struct sk_buff*)bulkdata->d[0].os_net_buf_ptr;
                        skb->len = bulkdata->d[0].data_length;

                        unifi_trace(priv, UDBG1,
                                "_identify_sme_ma_pkt_ind: skb_80211_to_ether -->\n");
                        r = skb_80211_to_ether(priv, skb, daddr, saddr,
                                signal, bulkdata);
                        unifi_trace(priv, UDBG1,
                                "_identify_sme_ma_pkt_ind: skb_80211_to_ether <--\n");
                        if (r) {
                            return -EINVAL;
                        }

                        /*                                       */
                        bulkdata->d[0].os_data_ptr = skb->data;
                        bulkdata->d[0].data_length = skb->len;
                    } else {
                        /*                                       */
                        bulkdata->d[0].os_data_ptr -= 2*ETH_ALEN;
                        bulkdata->d[0].data_length += 2*ETH_ALEN;
                        memcpy((void*)bulkdata->d[0].os_data_ptr, daddr, ETH_ALEN);
                        memcpy((void*)bulkdata->d[0].os_data_ptr + ETH_ALEN, saddr, ETH_ALEN);
                    }

                    unifi_trace(priv, UDBG1,
                            "_identify_sme_ma_pkt_ind: unifi_sys_ma_pkt_ind -->\n");
                    CsrWifiRouterMaPacketIndSend(priv->sme_unidata_ind_filters[i].appHandle,
                            (pkt_ind->VirtualInterfaceIdentifier & 0xff),
                            i,
                            pkt_ind->ReceptionStatus,
                            bulkdata->d[0].data_length,
                            (u8*)bulkdata->d[0].os_data_ptr,
                            NULL,
                            pkt_ind->Rssi,
                            pkt_ind->Snr,
                            pkt_ind->ReceivedRate);


                    unifi_trace(priv, UDBG1,
                            "_identify_sme_ma_pkt_ind: unifi_sys_ma_pkt_ind <--\n");
                }

                return 1;
            }
        }
    }

    return -1;
}
#endif /*                 */

/*
                                                                              
                      
  
                                                                 
                                                                  
                                                                 
  
              
                                                                   
                                                                    
                                                
                                           
  
            
                                                                    
                                                               
                                                                              
 */
int
skb_80211_to_ether(unifi_priv_t *priv, struct sk_buff *skb,
                   const unsigned char *daddr, const unsigned char *saddr,
                   const CSR_SIGNAL *signal,
                   bulk_data_param_t *bulkdata)
{
    unsigned char *payload;
    int payload_length;
    struct ethhdr *eth;
    llc_snap_hdr_t *snap;
    int headroom;
#define UF_VLAN_LLC_HEADER_SIZE     18
    static const u8 vlan_inner_snap[] = { 0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00 };
#if defined(CSR_NATIVE_SOFTMAC) && defined(CSR_SUPPORT_SME)
    const CSR_MA_PACKET_INDICATION *pkt_ind = &signal->u.MaPacketIndication;
#endif

    if(skb== NULL || daddr == NULL || saddr == NULL){
        unifi_error(priv,"skb_80211_to_ether: PBC fail\n");
        return 1;
    }

    payload = skb->data;
    payload_length = skb->len;

    snap = (llc_snap_hdr_t *)payload;
    eth  = (struct ethhdr *)payload;

    /*                           */
    headroom = skb_headroom(skb);

    /*
                                     
     */
    if ((payload_length >= sizeof(llc_snap_hdr_t)) &&
            (snap->dsap == 0xAA) &&
            (snap->ssap == 0xAA) &&
            (snap->ctrl == 0x03) &&
            (snap->oui[0] == 0) &&
            (snap->oui[1] == 0) &&
            ((snap->oui[2] == 0) || (snap->oui[2] == 0xF8)))
    {
        /*                                */
        if ((snap->oui[2] == 0) &&
                ((ntohs(snap->protocol) == ETH_P_AARP) || (ntohs(snap->protocol) == ETH_P_IPX)))
        {
            u16 len;

            unifi_trace(priv, UDBG3, "%s len: %d\n",
                    (ntohs(snap->protocol) == ETH_P_AARP) ? "ETH_P_AARP" : "ETH_P_IPX",
                    payload_length);

            /*                                                 */
            if (headroom < (2 * ETH_ALEN + 2)) {
                unifi_warning(priv, "headroom not available to skb_push ether header\n");
                return -1;
            }

            /*                                         */
            len = htons(skb->len);
            memcpy(skb_push(skb, 2), &len, 2);
            memcpy(skb_push(skb, ETH_ALEN), saddr, ETH_ALEN);
            memcpy(skb_push(skb, ETH_ALEN), daddr, ETH_ALEN);

            return 0;
        }
        /*                */
        if ((snap->oui[2] == 0) && (ntohs(snap->protocol) == ETH_P_8021Q))
        {
            /*
                                                                                     
              
                                                             
                                                                    
                                                                    
                                                                    
                                                                    
             */
            u16 vlan_snap;

            if (payload_length < UF_VLAN_LLC_HEADER_SIZE) {
                unifi_warning(priv, "VLAN SNAP header too short: %d bytes\n", payload_length);
                return -1;
            }

            if (memcmp(payload + 10, vlan_inner_snap, 6)) {
                unifi_warning(priv, "VLAN malformatted SNAP header.\n");
                return -1;
            }

            unifi_trace(priv, UDBG3, "VLAN SNAP: %02x-%02x\n", payload[8], payload[9]);
            unifi_trace(priv, UDBG3, "VLAN len: %d\n", payload_length);

            /*                         */

            vlan_snap = *((u16*)(payload + 8));

            /*                                         */
            eth->h_proto = snap->protocol;

            memcpy(eth->h_dest, daddr, ETH_ALEN);
            memcpy(eth->h_source, saddr, ETH_ALEN);
            *(u16*)(eth + 1) = vlan_snap;
            return 0;
        }

        /*                             */
        unifi_trace(priv, UDBG3, "SNAP+RFC1042 len: %d\n", payload_length);

        /*                                */
        skb_pull(skb, sizeof(llc_snap_hdr_t));

        /*                                                                            
                                       
         */
        /*                                          */
        eth = (struct ethhdr *)skb_push(skb, ETH_HLEN);
        memcpy(eth->h_dest, daddr, ETH_ALEN);
        memcpy(eth->h_source, saddr, ETH_ALEN);
        /*                                           */
        eth->h_proto = snap->protocol;
    } else {
        u16 len;

        /*                                                 */
        if (headroom < (2 * ETH_ALEN + 2)) {
            unifi_warning(priv, "headroom not available to skb_push ether header\n");
            return -1;
        }
        /*                                         */
        len = htons(skb->len);
        memcpy(skb_push(skb, 2), &len, 2);
        memcpy(skb_push(skb, ETH_ALEN), saddr, ETH_ALEN);
        memcpy(skb_push(skb, ETH_ALEN), daddr, ETH_ALEN);

        return 1;
    }

    return 0;
} /*                      */


static CsrWifiRouterCtrlPortAction verify_port(unifi_priv_t *priv, unsigned char *address, int queue, u16 interfaceTag)
{
#ifdef CSR_NATIVE_LINUX
#ifdef CSR_SUPPORT_WEXT
    if (queue == UF_CONTROLLED_PORT_Q) {
        return priv->wext_conf.block_controlled_port;
    } else {
        return CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_OPEN;
    }
#else
    return CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_OPEN; /*                                 */
#endif
#else
    return uf_sme_port_state(priv, address, queue, interfaceTag);
#endif
}

/*
                                                                              
                             
  
  
                                                              
                                  
                                                                
                                         
  
              
                                                                
                                                                        
                                                                                                          
                                                                  
                                                                                  
                                                                  
                                           
                                      
                                                                          
  
            
                                      
                                                                              
 */

int prepare_and_add_macheader(unifi_priv_t *priv, struct sk_buff *skb, struct sk_buff *newSkb,
                              CSR_PRIORITY priority,
                              bulk_data_param_t *bulkdata,
                              u16 interfaceTag,
                              const u8 *daddr,
                              const u8 *saddr,
                              u8 protection)
{
    u16 fc = 0;
    u8 qc = 0;
    u8 macHeaderLengthInBytes = MAC_HEADER_SIZE, *bufPtr = NULL;
    bulk_data_param_t data_ptrs;
    CsrResult csrResult;
    int headroom =0;
    u8 direction = 0;
    netInterface_priv_t *interfacePriv = priv->interfacePriv[interfaceTag];
    u8 *addressOne;
    u8 bQosNull = false;

    if (skb == NULL) {
        unifi_error(priv,"prepare_and_add_macheader: Invalid SKB reference\n");
        return -1;
    }

    /*                                                                          */
    if (priority != CSR_CONTENTION) {
        /*                                    */
        if (priority == CSR_MANAGEMENT) {
            fc |= cpu_to_le16(IEEE802_11_FC_TYPE_DATA);
        } else {
            /*                   */
            macHeaderLengthInBytes += QOS_CONTROL_HEADER_SIZE;

            if (skb->len) {

                fc |= cpu_to_le16(IEEE802_11_FC_TYPE_QOS_DATA);
            } else {
                fc |= cpu_to_le16(IEEE802_11_FC_TYPE_QOS_NULL);
                bQosNull = true;
            }
        }
    } else {
        if(skb->len == 0) {
            fc |= cpu_to_le16(IEEE802_11_FC_TYPE_NULL);
        } else {
            fc |= cpu_to_le16(IEEE802_11_FC_TYPE_DATA);
        }
    }

    switch (interfacePriv->interfaceMode)
    {
        case  CSR_WIFI_ROUTER_CTRL_MODE_STA:
        case CSR_WIFI_ROUTER_CTRL_MODE_P2PCLI:
            direction = 2;
            fc |= cpu_to_le16(IEEE802_11_FC_TO_DS_MASK);
            break;
        case  CSR_WIFI_ROUTER_CTRL_MODE_IBSS:
            direction = 0;
            break;
        case  CSR_WIFI_ROUTER_CTRL_MODE_AP:
        case CSR_WIFI_ROUTER_CTRL_MODE_P2PGO:
            direction = 1;
            fc |= cpu_to_le16(IEEE802_11_FC_FROM_DS_MASK);
            break;
        case CSR_WIFI_ROUTER_CTRL_MODE_AMP:
            if (priority == CSR_MANAGEMENT ) {

                direction = 2;
                fc |= cpu_to_le16(IEEE802_11_FC_TO_DS_MASK);
            } else {
                /*                                              */
                direction = 3;
                fc |= cpu_to_le16(IEEE802_11_FC_TO_DS_MASK | IEEE802_11_FC_FROM_DS_MASK);
                macHeaderLengthInBytes += 6;
            }
            break;
        default:
            unifi_warning(priv, "prepare_and_add_macheader: Unknown mode %d\n",
                          interfacePriv->interfaceMode);
    }


    /*                                                               */
    /*                                     */

    if(protection) {
        fc |= cpu_to_le16(IEEE802_11_FC_PROTECTED_MASK);
    }

    /*                                                  */
    headroom = skb_headroom(skb);

    if (headroom < macHeaderLengthInBytes) {
        unifi_trace(priv, UDBG5,
                    "prepare_and_add_macheader: Allocate headroom extra %d bytes\n",
                    macHeaderLengthInBytes);

        csrResult = unifi_net_data_malloc(priv, &data_ptrs.d[0], skb->len + macHeaderLengthInBytes);

        if (csrResult != CSR_RESULT_SUCCESS) {
            unifi_error(priv, " failed to allocate request_data. in %s func\n", __FUNCTION__);
            return -1;
        }
        newSkb = (struct sk_buff *)(data_ptrs.d[0].os_net_buf_ptr);
        newSkb->len = skb->len + macHeaderLengthInBytes;

        memcpy((void*)data_ptrs.d[0].os_data_ptr + macHeaderLengthInBytes,
                skb->data, skb->len);

        bulkdata->d[0].os_data_ptr = newSkb->data;
        bulkdata->d[0].os_net_buf_ptr = (unsigned char*)newSkb;
        bulkdata->d[0].data_length = newSkb->len;

        bufPtr = (u8*)data_ptrs.d[0].os_data_ptr;

        /*                                    */
            kfree_skb(skb);
    } else {

        /*                                                          */
        bufPtr = (u8*)skb_push(skb, macHeaderLengthInBytes);
        bulkdata->d[0].os_data_ptr = skb->data;
        bulkdata->d[0].os_net_buf_ptr = (unsigned char*)skb;
        bulkdata->d[0].data_length = skb->len;
    }

    /*                             */

    memset(bufPtr, 0, macHeaderLengthInBytes);

    /*                         */
    memcpy(bufPtr, &fc, sizeof(fc));
    bufPtr += sizeof(fc);
    macHeaderLengthInBytes -= sizeof(fc);

    /*                                    */
    bufPtr += 2;
    macHeaderLengthInBytes -= 2;

    switch(direction)
    {
        case 0:
            /*                                              */
            /*                                                          */
            memcpy(bufPtr, daddr, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;

            /*                                     */
            memcpy(bufPtr, saddr, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;

            /*                                                                   */
            memcpy(bufPtr, &interfacePriv->bssid, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;
            break;
        case 1:
           /*                                                 */
            memcpy(bufPtr, daddr, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;
            /*                                     */
            memcpy(bufPtr, &interfacePriv->bssid, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;

            /*                                                     */
            memcpy(bufPtr, saddr, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;
            break;
        case  2:
            /*                                                                      */
            memcpy(bufPtr, &interfacePriv->bssid, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;

            /*                                                     */
            memcpy(bufPtr, saddr, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;

            /*                                                                            */
            memcpy(bufPtr, daddr, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;
            break;
        case 3:
            memcpy(bufPtr, &interfacePriv->bssid, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;

            /*                                                     */
            memcpy(bufPtr, saddr, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;

            /*                                                                            */
            memcpy(bufPtr, daddr, ETH_ALEN);
            bufPtr += ETH_ALEN;
            macHeaderLengthInBytes -= ETH_ALEN;
            break;
        default:
            unifi_error(priv,"Unknown direction =%d : Not handled now\n",direction);
            return -1;
    }
    /*                                                      */
    bufPtr += 2;
    macHeaderLengthInBytes -= 2;

    if (3 == direction) {
        /*                                     */
        memcpy(bufPtr, saddr, ETH_ALEN);
        bufPtr += ETH_ALEN;
        macHeaderLengthInBytes -= ETH_ALEN;
    }

    /*                                                        */
    if ((priority != CSR_CONTENTION) && (macHeaderLengthInBytes >= QOS_CONTROL_HEADER_SIZE)) {

        if (priority > 7) {
            unifi_trace(priv, UDBG1, "data packets priority is more than 7, priority = %x\n", priority);
            qc |= 7;
        } else {
            qc |= priority;
        }
        /*                  
                                                                                                                    
                             
        */

        addressOne = bufPtr- ADDRESS_ONE_OFFSET;

        if (addressOne[0] & 0x1) {
            /*                                                       */
            qc |= 1 << 5;
        }
        /*                          */
        if(interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_STA ||
           interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_IBSS ||
           interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_P2PCLI) {
           /*                                                        */
        } else {
            if(bQosNull) {
                qc |= 1 << 4;
            }
        }

        /*                                        */
        bufPtr[0] = qc;
        /*                 */
        bufPtr[1] = 0;
        macHeaderLengthInBytes -= QOS_CONTROL_HEADER_SIZE;
    }
    if (macHeaderLengthInBytes) {
        unifi_warning(priv, " Mac header not appended properly\n");
        return -1;
    }
    return 0;
}

/*
                                                                              
                       
  
                                                                     
                                                                    
  
              
                                                                
                                                                        
                                                              
  
            
                                      
                                                                              
 */

static int
send_ma_pkt_request(unifi_priv_t *priv, struct sk_buff *skb, const struct ethhdr *ehdr, CSR_PRIORITY priority)
{
    int r;
    u16 i;
    u8 eapolStore = FALSE;
    struct sk_buff *newSkb = NULL;
    bulk_data_param_t bulkdata;
    const int proto = ntohs(ehdr->h_proto);
    u16 interfaceTag;
    CsrWifiMacAddress peerAddress;
    CSR_TRANSMISSION_CONTROL transmissionControl = CSR_NO_CONFIRM_REQUIRED;
    s8 protection;
    netInterface_priv_t *interfacePriv = NULL;
    CSR_RATE TransmitRate = (CSR_RATE)0;

    unifi_trace(priv, UDBG5, "entering send_ma_pkt_request\n");

    /*                                                      */
    for (i = 0; i < CSR_WIFI_NUM_INTERFACES; i++) {
        if (!memcmp(priv->netdev[i]->dev_addr, ehdr->h_source, ETH_ALEN)) {
            interfaceTag = i;
            interfacePriv = priv->interfacePriv[interfaceTag];
            break;
        }
    }

    if (interfacePriv == NULL) {
        /*                        */
        interfaceTag = 0;
        interfacePriv = priv->interfacePriv[interfaceTag];
        unifi_warning(priv, "Mac address not matching ... debugging needed\n");
        interfacePriv->stats.tx_dropped++;
        kfree_skb(skb);
        return -1;
    }

    /*                                */
    if (skb_add_llc_snap(priv->netdev[interfaceTag], skb, proto) != 0) {
        /*                */
        unifi_error(priv, "skb_add_llc_snap failed.\n");
        kfree_skb(skb);
        return -1;
    }

    bulkdata.d[0].os_data_ptr = skb->data;
    bulkdata.d[0].os_net_buf_ptr = (unsigned char*)skb;
    bulkdata.d[0].net_buf_length = bulkdata.d[0].data_length = skb->len;
    bulkdata.d[1].os_data_ptr = NULL;
    bulkdata.d[1].os_net_buf_ptr = NULL;
    bulkdata.d[1].net_buf_length = bulkdata.d[1].data_length = 0;

#ifdef CSR_SUPPORT_SME
    /*                                                             */
    if ((interfacePriv->interfaceMode != CSR_WIFI_ROUTER_CTRL_MODE_AP) &&
        (interfacePriv->interfaceMode != CSR_WIFI_ROUTER_CTRL_MODE_P2PGO)) {
        unifi_ta_sample(priv->card, CSR_WIFI_ROUTER_CTRL_PROTOCOL_DIRECTION_TX,
                        &bulkdata.d[0], ehdr->h_source,
                        priv->netdev[interfaceTag]->dev_addr,
                        jiffies_to_msecs(jiffies),
                        0);     /*                       */
    }
#endif /*                 */

    if ((proto == ETH_P_PAE)
#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
            || (proto == ETH_P_WAI)
#endif
       )
    {
        /*                        */
        if (0 == uf_verify_m4(priv, bulkdata.d[0].os_data_ptr, bulkdata.d[0].data_length)) {
            eapolStore = TRUE;
        }
    }

#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
    if (proto == ETH_P_WAI)
     {
        protection = 0; /*                                   */
     }
   else
     {
#endif
#ifdef CSR_SUPPORT_SME
    if ((protection = uf_get_protection_bit_from_interfacemode(priv, interfaceTag, ehdr->h_dest)) < 0) {
        unifi_warning(priv, "unicast address, but destination not in station record database\n");
        unifi_net_data_free(priv, &bulkdata.d[0]);
        return -1;
    }
#else
    protection = 0;
#endif
#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
   }
#endif

    /*                                                    */
    if (prepare_and_add_macheader(priv, skb, newSkb, priority, &bulkdata, interfaceTag, ehdr->h_dest, ehdr->h_source, protection)) {
        unifi_error(priv, "failed to create MAC header\n");
        unifi_net_data_free(priv, &bulkdata.d[0]);
        return -1;
    }

    /*                                                                                 
                                                                                                       
                               
     */
    memcpy(peerAddress.a, ((u8 *) bulkdata.d[0].os_data_ptr) + 4, ETH_ALEN);

    unifi_trace(priv, UDBG5, "RA[0]=%x, RA[1]=%x, RA[2]=%x, RA[3]=%x, RA[4]=%x, RA[5]=%x\n",
                peerAddress.a[0],peerAddress.a[1], peerAddress.a[2], peerAddress.a[3],
                peerAddress.a[4],peerAddress.a[5]);


    if ((proto == ETH_P_PAE)
#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
            || (proto == ETH_P_WAI)
#endif
       )
    {
        CSR_SIGNAL signal;
        CSR_MA_PACKET_REQUEST *req = &signal.u.MaPacketRequest;

        /*                           */
        memset(&signal, 0, sizeof(CSR_SIGNAL));

        /*                         */
        signal.SignalPrimitiveHeader.SignalId = CSR_MA_PACKET_REQUEST_ID;
        signal.SignalPrimitiveHeader.ReceiverProcessId = 0;
        signal.SignalPrimitiveHeader.SenderProcessId = priv->netdev_client->sender_id;

        transmissionControl = req->TransmissionControl = 0;
#ifdef CSR_SUPPORT_SME
        if (eapolStore)
        {
            netInterface_priv_t *netpriv = (netInterface_priv_t *)netdev_priv(priv->netdev[interfaceTag]);

            /*                        */

            req->Priority = priority;
            unifi_trace(priv, UDBG3, "Tx Frame with Priority: %x\n", req->Priority);

            /*                           */
            req->TransmitRate = 0;
            req->HostTag = CSR_WIFI_EAPOL_M4_HOST_TAG;
            /*                                                  */
            memcpy(req->Ra.x, ((u8 *) bulkdata.d[0].os_data_ptr) + 4, ETH_ALEN);

            spin_lock(&priv->m4_lock);
            /*                                   */
            interfacePriv->m4_signal = signal;
            interfacePriv->m4_bulk_data.net_buf_length = bulkdata.d[0].net_buf_length;
            interfacePriv->m4_bulk_data.data_length = bulkdata.d[0].data_length;
            interfacePriv->m4_bulk_data.os_data_ptr = bulkdata.d[0].os_data_ptr;
            interfacePriv->m4_bulk_data.os_net_buf_ptr = bulkdata.d[0].os_net_buf_ptr;
            spin_unlock(&priv->m4_lock);

            /*                                                                      
                                                                       
                                                                            
             */
            queue_work(priv->unifi_workqueue, &netpriv->send_m4_ready_task);

            return 0;
        }
#endif
    }/*                   */

#if (defined(CSR_WIFI_SECURITY_WAPI_ENABLE) && defined(CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION))
    if ((CSR_WIFI_ROUTER_CTRL_MODE_STA == interfacePriv->interfaceMode) && \
        (priv->wapi_unicast_filter) && \
        (proto != ETH_P_PAE) && \
        (proto != ETH_P_WAI) && \
        (skb->len > 0))
    {
        CSR_SIGNAL signal;
        CSR_MA_PACKET_REQUEST *req = &signal.u.MaPacketRequest;
        netInterface_priv_t *netpriv = (netInterface_priv_t *)netdev_priv(priv->netdev[interfaceTag]);

        unifi_trace(priv, UDBG4, "send_ma_pkt_request() - WAPI unicast data packet when USKID = 1 \n");

        /*                           */
        memset(&signal, 0, sizeof(CSR_SIGNAL));
        /*                         */
        signal.SignalPrimitiveHeader.SignalId = CSR_MA_PACKET_REQUEST_ID;
        signal.SignalPrimitiveHeader.ReceiverProcessId = 0;
        signal.SignalPrimitiveHeader.SenderProcessId = priv->netdev_client->sender_id;

        /*                        */
        req->TransmissionControl = 0;
        req->Priority = priority;
        unifi_trace(priv, UDBG3, "Tx Frame with Priority: %x\n", req->Priority);
        req->TransmitRate = (CSR_RATE) 0; /*                           */
        req->HostTag = 0xffffffff;        /*                       */
        /*                                                  */
        memcpy(req->Ra.x, ((u8 *) bulkdata.d[0].os_data_ptr) + 4, ETH_ALEN);

        /*                                   */
        spin_lock(&priv->wapi_lock);
        interfacePriv->wapi_unicast_ma_pkt_sig = signal;
        interfacePriv->wapi_unicast_bulk_data.net_buf_length = bulkdata.d[0].net_buf_length;
        interfacePriv->wapi_unicast_bulk_data.data_length = bulkdata.d[0].data_length;
        interfacePriv->wapi_unicast_bulk_data.os_data_ptr = bulkdata.d[0].os_data_ptr;
        interfacePriv->wapi_unicast_bulk_data.os_net_buf_ptr = bulkdata.d[0].os_net_buf_ptr;
        spin_unlock(&priv->wapi_lock);

        /*                                                                             
                                                                   
                                                                        
         */
        queue_work(priv->unifi_workqueue, &netpriv->send_pkt_to_encrypt);

        return 0;
    }
#endif

    if(priv->cmanrTestMode)
    {
        TransmitRate = priv->cmanrTestModeTransmitRate;
        unifi_trace(priv, UDBG2, "send_ma_pkt_request: cmanrTestModeTransmitRate = %d TransmitRate=%d\n",
                    priv->cmanrTestModeTransmitRate,
                    TransmitRate
                   );
    }

    /*                */
    /*                                                                                                                              */
    r = uf_process_ma_packet_req(priv,
                                 peerAddress.a,
                                 0xffffffff,  /*                       */
                                 interfaceTag,
                                 transmissionControl,
                                 TransmitRate,
                                 priority,
                                 priv->netdev_client->sender_id,
                                 &bulkdata);

    if (r) {
        unifi_trace(priv, UDBG1, "(HIP validation failure) r = %x\n", r);
        unifi_net_data_free(priv, &bulkdata.d[0]);
        return -1;
    }

    unifi_trace(priv, UDBG3, "leaving send_ma_pkt_request, UNITDATA result code = %d\n", r);

    return r;
} /*                       */

/*
                                                                              
               
  
                                                                        
                        
  
              
                                        
                                                
  
            
                                                                         
                                  
                    
  
  
          
                                                                    
                                                                              
 */
static netdev_tx_t
uf_net_xmit(struct sk_buff *skb, struct net_device *dev)
{
    netInterface_priv_t *interfacePriv = (netInterface_priv_t *)netdev_priv(dev);
    unifi_priv_t *priv = interfacePriv->privPtr;
    struct ethhdr ehdr;
    int proto, port;
    int result;
    static tx_signal_handler tx_handler;
    CSR_PRIORITY priority;
    CsrWifiRouterCtrlPortAction port_action;

    unifi_trace(priv, UDBG5, "unifi_net_xmit: skb = %x\n", skb);

    memcpy(&ehdr, skb->data, ETH_HLEN);
    proto = ntohs(ehdr.h_proto);
    priority = get_packet_priority(priv, skb, &ehdr, interfacePriv);

    /*                                                   */
    tx_handler = send_ma_pkt_request;

    /*                                                   */
    if ((proto != ETH_P_PAE)
#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
            && (proto != ETH_P_WAI)
#endif
       ) {
        port = UF_CONTROLLED_PORT_Q;
    } else {
        /*         */
        port = UF_UNCONTROLLED_PORT_Q;
    }

    /*                               */
    port_action = verify_port(priv
        , (((CSR_WIFI_ROUTER_CTRL_MODE_STA == interfacePriv->interfaceMode)||(CSR_WIFI_ROUTER_CTRL_MODE_P2PCLI== interfacePriv->interfaceMode))? interfacePriv->bssid.a: ehdr.h_dest)
        , port
        , interfacePriv->InterfaceTag);

    if (port_action == CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_OPEN) {
        unifi_trace(priv, UDBG5,
                    "uf_net_xmit: %s controlled port open\n",
                    port ? "" : "un");
        /*                            */
        skb_pull(skb, ETH_HLEN);
        result = tx_handler(priv, skb, &ehdr, priority);
    } else {

        /*                                 */
        unifi_trace(priv, UDBG2,
                "uf_net_xmit: %s controlled port %s\n",
                port ? "" : "un", port_action==CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_CLOSED_BLOCK ? "blocked" : "closed");
        interfacePriv->stats.tx_dropped++;
        kfree_skb(skb);

        return NETDEV_TX_OK;
    }

    if (result == NETDEV_TX_OK) {
#if (defined(CSR_WIFI_SECURITY_WAPI_ENABLE) && defined(CSR_WIFI_SECURITY_WAPI_SW_ENCRYPTION))
    	/*                                                                       */
    	if (!((CSR_WIFI_ROUTER_CTRL_MODE_STA == interfacePriv->interfaceMode) &&
              (priv->wapi_unicast_filter == 1)))
        {
            dev->trans_start = jiffies;
            /*                                                               
                                            
             */
            interfacePriv->stats.tx_packets++;
            /*                               */
            interfacePriv->stats.tx_bytes += skb->len;

        }
#else
    	dev->trans_start = jiffies;

        /*
                                                                         
                                        
         */
        interfacePriv->stats.tx_packets++;
        /*                               */
        interfacePriv->stats.tx_bytes += skb->len;
#endif
    } else if (result < 0) {

        /*                                                              
                                                                          
                                                   
         */
        interfacePriv->stats.tx_dropped++;
        unifi_trace(priv, UDBG1, "unifi_net_xmit: (Packet Drop), dropped count = %x\n", interfacePriv->stats.tx_dropped);
        result = NETDEV_TX_OK;
    }

    /*                                                    */

    return result;
} /*               */

/*
                                                                              
                    
                      
  
                                                                          
                                         
                                                                        
                                                                    
                                                                          
                                                       
  
              
                                                   
  
            
                                                            
                                                                              
 */
void
unifi_pause_xmit(void *ospriv, unifi_TrafficQueue queue)
{
    unifi_priv_t *priv = ospriv;
    int i; /*                        */

    unifi_trace(priv, UDBG2, "Stopping queue %d\n", queue);

    for(i=0;i<CSR_WIFI_NUM_INTERFACES;i++)
    {
        if (netif_running(priv->netdev[i]))
        {
            netif_stop_subqueue(priv->netdev[i], (u16)queue);
        }
    }

#ifdef CSR_SUPPORT_SME
    if(queue<=3) {
        routerStartBuffering(priv,queue);
        unifi_trace(priv,UDBG2,"Start buffering %d\n", queue);
     } else {
        routerStartBuffering(priv,0);
        unifi_error(priv, "Start buffering %d defaulting to 0\n", queue);
     }
#endif

} /*                    */

void
unifi_restart_xmit(void *ospriv, unifi_TrafficQueue queue)
{
    unifi_priv_t *priv = ospriv;
    int i=0; /*                        */

    unifi_trace(priv, UDBG2, "Waking queue %d\n", queue);

    for(i=0;i<CSR_WIFI_NUM_INTERFACES;i++)
    {
        if (netif_running(priv->netdev[i]))
        {
            netif_wake_subqueue(priv->netdev[i], (u16)queue);
        }
    }

#ifdef CSR_SUPPORT_SME
    if(queue <=3) {
        routerStopBuffering(priv,queue);
        uf_send_buffered_frames(priv,queue);
    } else {
        routerStopBuffering(priv,0);
        uf_send_buffered_frames(priv,0);
    }
#endif
} /*                      */


static void
indicate_rx_skb(unifi_priv_t *priv, u16 ifTag, u8* dst_a, u8* src_a, struct sk_buff *skb, CSR_SIGNAL *signal,
                bulk_data_param_t *bulkdata)
{
    int r, sr = 0;
    struct net_device *dev;

#ifdef CSR_SUPPORT_SME
    llc_snap_hdr_t *snap;

    snap = (llc_snap_hdr_t *)skb->data;

    sr = _identify_sme_ma_pkt_ind(priv,
                                  snap->oui, ntohs(snap->protocol),
                                  signal,
                                  bulkdata,
                                  dst_a, src_a );
#endif

    /*
                                      
                                                                      
                   
     */
    r = skb_80211_to_ether(priv, skb, dst_a, src_a,
                           signal, bulkdata);
    if (r == -1) {
        /*                            */
        priv->interfacePriv[ifTag]->stats.rx_errors++;
        priv->interfacePriv[ifTag]->stats.rx_frame_errors++;
        unifi_net_data_free(priv, &bulkdata->d[0]);
        unifi_notice(priv, "indicate_rx_skb: Discard unknown frame.\n");
        return;
    }

    /*                                                                 
                                                                      
                                                                               
     */
    if (r == 1 && sr == 1) {
        unifi_net_data_free(priv, &bulkdata->d[0]);
        unifi_trace(priv, UDBG5, "indicate_rx_skb: Data given to subscription"
                "API, not being given to kernel\n");
        return;
    }

    dev = priv->netdev[ifTag];
    /*                                           */
    /*                       */
    skb->dev = dev;
    skb->protocol = eth_type_trans(skb, dev);
    skb->ip_summed = CHECKSUM_UNNECESSARY;

    /*                              */
    if (skb->len > (dev->mtu + ETH_HLEN)) {
        /*                                         */
        /*                                    */
        unifi_error(priv, "%s: oversize frame (%d > %d)\n",
                    dev->name,
                    skb->len, dev->mtu + ETH_HLEN);

        /*                            */
        priv->interfacePriv[ifTag]->stats.rx_errors++;
        priv->interfacePriv[ifTag]->stats.rx_length_errors++;
        unifi_net_data_free(priv, &bulkdata->d[0]);
        return;
    }


    if(priv->cmanrTestMode)
    {
        const CSR_MA_PACKET_INDICATION *pkt_ind = &signal->u.MaPacketIndication;
        priv->cmanrTestModeTransmitRate = pkt_ind->ReceivedRate;
        unifi_trace(priv, UDBG2, "indicate_rx_skb: cmanrTestModeTransmitRate=%d\n", priv->cmanrTestModeTransmitRate);
    }

    /*                       */
#ifdef CSR_WIFI_USE_NETIF_RX
        netif_rx(skb);
#else
        netif_rx_ni(skb);
#endif

    if (dev != NULL) {
        dev->last_rx = jiffies;
    }

    /*               */
    priv->interfacePriv[ifTag]->stats.rx_packets++;
    priv->interfacePriv[ifTag]->stats.rx_bytes += bulkdata->d[0].data_length;

    return;
}

void
uf_process_rx_pending_queue(unifi_priv_t *priv, int queue,
                            CsrWifiMacAddress source_address,
                            int indicate, u16 interfaceTag)
{
    rx_buffered_packets_t *rx_q_item;
    struct list_head *rx_list;
    struct list_head *n;
    struct list_head *l_h;
    static const CsrWifiMacAddress broadcast_address = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    netInterface_priv_t *interfacePriv = priv->interfacePriv[interfaceTag];

    if (interfaceTag >= CSR_WIFI_NUM_INTERFACES) {
        unifi_error(priv, "uf_process_rx_pending_queue bad interfaceTag\n");
        return;
    }

    if (queue == UF_CONTROLLED_PORT_Q) {
        rx_list = &interfacePriv->rx_controlled_list;
    } else {
        rx_list = &interfacePriv->rx_uncontrolled_list;
    }

    down(&priv->rx_q_sem);
    list_for_each_safe(l_h, n, rx_list) {
        rx_q_item = list_entry(l_h, rx_buffered_packets_t, q);

        /*                                     */
        if (memcmp(broadcast_address.a, source_address.a, ETH_ALEN) &&
                memcmp(rx_q_item->sa.a, source_address.a, ETH_ALEN)) {

            unifi_trace(priv, UDBG2,
                        "uf_process_rx_pending_queue: Skipping sa=%02X%02X%02X%02X%02X%02X skb=%p, bulkdata=%p\n",
                        rx_q_item->sa.a[0], rx_q_item->sa.a[1],
                        rx_q_item->sa.a[2], rx_q_item->sa.a[3],
                        rx_q_item->sa.a[4], rx_q_item->sa.a[5],
                        rx_q_item->skb, &rx_q_item->bulkdata.d[0]);
            continue;
        }

        list_del(l_h);


        unifi_trace(priv, UDBG2,
                    "uf_process_rx_pending_queue: Was Blocked skb=%p, bulkdata=%p\n",
                    rx_q_item->skb, &rx_q_item->bulkdata);

        if (indicate) {
            indicate_rx_skb(priv, interfaceTag, rx_q_item->da.a, rx_q_item->sa.a, rx_q_item->skb, &rx_q_item->signal, &rx_q_item->bulkdata);
        } else {
            interfacePriv->stats.rx_dropped++;
            unifi_net_data_free(priv, &rx_q_item->bulkdata.d[0]);
        }

        /*                                                          */
        kfree(rx_q_item);
    }
    up(&priv->rx_q_sem);
}

/*
                                                                              
                        
  
                                                              
                                                                
                                                                 
                                                                     
  
              
                                                    
  
            
                                                                              
 */
void
uf_resume_data_plane(unifi_priv_t *priv, int queue,
                     CsrWifiMacAddress peer_address,
                     u16 interfaceTag)
{
#ifdef CSR_SUPPORT_WEXT
    netInterface_priv_t *interfacePriv = priv->interfacePriv[interfaceTag];
#endif

    if (interfaceTag >= CSR_WIFI_NUM_INTERFACES) {
        unifi_error(priv, "uf_resume_data_plane bad interfaceTag\n");
        return;
    }

    unifi_trace(priv, UDBG2, "Resuming netif\n");

    /*
                                                                        
                                                                       
                           
     */
#ifdef CSR_SUPPORT_WEXT
    if (!interfacePriv->wait_netdev_change)
#endif
    {
#ifdef CONFIG_NET_SCHED
        if (netif_running(priv->netdev[interfaceTag])) {
            netif_tx_schedule_all(priv->netdev[interfaceTag]);
        }
#endif
        uf_process_rx_pending_queue(priv, queue, peer_address, 1,interfaceTag);
    }
} /*                        */


void uf_free_pending_rx_packets(unifi_priv_t *priv, int queue, CsrWifiMacAddress peer_address,u16 interfaceTag)
{
    uf_process_rx_pending_queue(priv, queue, peer_address, 0,interfaceTag);

} /*                              */


/*
                                                                              
            
  
                                                                      
                                      
  
              
             
  
            
             
                                                                              
 */
static void
unifi_rx(unifi_priv_t *priv, CSR_SIGNAL *signal, bulk_data_param_t *bulkdata)
{
    u16 interfaceTag;
    bulk_data_desc_t *pData;
    const CSR_MA_PACKET_INDICATION *pkt_ind = &signal->u.MaPacketIndication;
    struct sk_buff *skb;
    CsrWifiRouterCtrlPortAction port_action;
    u8 dataFrameType;
    int proto;
    int queue;

    u8 da[ETH_ALEN], sa[ETH_ALEN];
    u8 toDs, fromDs, frameType, macHeaderLengthInBytes = MAC_HEADER_SIZE;
    u16 frameControl;
    netInterface_priv_t *interfacePriv;
    struct ethhdr ehdr;

    interfaceTag = (pkt_ind->VirtualInterfaceIdentifier & 0xff);
    interfacePriv = priv->interfacePriv[interfaceTag];

    /*                                                          */
    if (interfaceTag >= CSR_WIFI_NUM_INTERFACES)
    {
        unifi_error(priv, "%s: MA-PACKET indication with bad interfaceTag %d\n", __FUNCTION__, interfaceTag);
        unifi_net_data_free(priv,&bulkdata->d[0]);
        return;
    }

    /*                                                         */
    if (!interfacePriv->netdev_registered)
    {
        unifi_error(priv, "%s: MA-PACKET indication with unallocated interfaceTag %d\n", __FUNCTION__, interfaceTag);
        unifi_net_data_free(priv, &bulkdata->d[0]);
        return;
    }

    if (bulkdata->d[0].data_length == 0) {
        unifi_warning(priv, "%s: MA-PACKET indication with zero bulk data\n", __FUNCTION__);
        unifi_net_data_free(priv,&bulkdata->d[0]);
        return;
    }


    skb = (struct sk_buff*)bulkdata->d[0].os_net_buf_ptr;
    skb->len = bulkdata->d[0].data_length;

    /*                        */
    toDs = (skb->data[1] & 0x01) ? 1 : 0;
    fromDs = (skb->data[1] & 0x02) ? 1 : 0;

    memcpy(da,(skb->data+4+toDs*12),ETH_ALEN);/*               */
    memcpy(sa,(skb->data+10+fromDs*(6+toDs*8)),ETH_ALEN); /*                  */


    pData = &bulkdata->d[0];
    frameControl = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(pData->os_data_ptr);
    frameType = ((frameControl & 0x000C) >> 2);

    dataFrameType =((frameControl & 0x00f0) >> 4);
    unifi_trace(priv, UDBG6,
                "%s: Receive Data Frame Type %d \n", __FUNCTION__,dataFrameType);

    switch(dataFrameType)
    {
        case QOS_DATA:
        case QOS_DATA_NULL:
            /*                                                        */
            if (fromDs && toDs)
            {
                /*                                 */
                macHeaderLengthInBytes += (QOS_CONTROL_HEADER_SIZE + 6);
            }
            else
            {
                macHeaderLengthInBytes += QOS_CONTROL_HEADER_SIZE;
            }

            /*                                                                  */
            if (frameControl & FRAME_CONTROL_ORDER_BIT)
                macHeaderLengthInBytes += HT_CONTROL_HEADER_SIZE;
            break;
        default:
            if (fromDs && toDs)
                macHeaderLengthInBytes += 6;
    }

    /*                                                          */
    switch(dataFrameType)
    {
        case DATA_NULL:
        case QOS_DATA_NULL:
            /*                                                         
                                                            
             */
            proto = 0x0;
            break;
        default:
            {
                llc_snap_hdr_t *snap;
                /*                                                            
                                                           
                 */
                snap = (llc_snap_hdr_t *) (skb->data + macHeaderLengthInBytes);

                /*                                                              */
                ehdr.h_proto = snap->protocol;
                memcpy(ehdr.h_dest, da, ETH_ALEN);
                memcpy(ehdr.h_source, sa, ETH_ALEN);
            }
            proto = ntohs(ehdr.h_proto);
    }
    unifi_trace(priv, UDBG3, "in unifi_rx protocol from snap header = 0x%x\n", proto);

    if ((proto != ETH_P_PAE)
#ifdef CSR_WIFI_SECURITY_WAPI_ENABLE
            && (proto != ETH_P_WAI)
#endif
       ) {
        queue = UF_CONTROLLED_PORT_Q;
    } else {
        queue = UF_UNCONTROLLED_PORT_Q;
    }

    port_action = verify_port(priv, (unsigned char*)sa, queue, interfaceTag);
    unifi_trace(priv, UDBG3, "in unifi_rx port action is = 0x%x & queue = %x\n", port_action, queue);

#ifdef CSR_SUPPORT_SME
    /*                                                                 */
    if((interfacePriv->interfaceMode != CSR_WIFI_ROUTER_CTRL_MODE_AP) &&
            (interfacePriv->interfaceMode != CSR_WIFI_ROUTER_CTRL_MODE_P2PGO))
    {
        /*                                                                     */
        skb_pull(skb, macHeaderLengthInBytes);
        pData->os_data_ptr = skb->data;
        pData->data_length -= macHeaderLengthInBytes;

        if (pData->data_length) {
            unifi_ta_sample(priv->card, CSR_WIFI_ROUTER_CTRL_PROTOCOL_DIRECTION_RX,
                            &bulkdata->d[0],
                            sa, priv->netdev[interfaceTag]->dev_addr,
                            jiffies_to_msecs(jiffies),
                            pkt_ind->ReceivedRate);
        }
    } else {

        /*                                 */
        CsrWifiRouterCtrlStaInfo_t * srcStaInfo =
            CsrWifiRouterCtrlGetStationRecordFromPeerMacAddress(priv,sa,interfaceTag);

        /*                                                           
                                                                             */

         if(srcStaInfo == NULL) {
            CsrWifiMacAddress peerMacAddress;
            /*                  */
            memcpy(peerMacAddress.a,sa,ETH_ALEN);
            unifi_trace(priv, UDBG1, "%s: Unexpected frame from peer = %x:%x:%x:%x:%x:%x\n", __FUNCTION__,
            sa[0], sa[1],sa[2], sa[3], sa[4],sa[5]);
            CsrWifiRouterCtrlUnexpectedFrameIndSend(priv->CSR_WIFI_SME_IFACEQUEUE,0,interfaceTag,peerMacAddress);
            unifi_net_data_free(priv, &bulkdata->d[0]);
            return;
        }

       /*                                      */
        if (port_action != CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_OPEN) {
            /*                            */
            CsrWifiMacAddress peerMacAddress;
            memcpy(peerMacAddress.a,sa,ETH_ALEN);
            unifi_trace(priv, UDBG3, "%s: Port is not open: unexpected frame from peer = %x:%x:%x:%x:%x:%x\n",
                        __FUNCTION__, sa[0], sa[1],sa[2], sa[3], sa[4],sa[5]);

            CsrWifiRouterCtrlUnexpectedFrameIndSend(priv->CSR_WIFI_SME_IFACEQUEUE,0,interfaceTag,peerMacAddress);
            interfacePriv->stats.rx_dropped++;
            unifi_net_data_free(priv, &bulkdata->d[0]);
            unifi_notice(priv, "%s: Dropping packet, proto=0x%04x, %s port\n", __FUNCTION__,
                         proto, queue ? "Controlled" : "Un-controlled");
            return;
        }

         /*                                                              */
        if((dataFrameType == QOS_DATA_NULL) || (dataFrameType == DATA_NULL)){
            unifi_trace(priv, UDBG5, "%s: Null Frame Received and Freed\n", __FUNCTION__);
            unifi_net_data_free(priv, &bulkdata->d[0]);
            return;
        }

        /*                                                                    */
        /*                                                                  */
        /*                                      
                                                     
                                 
           */
        if(!(uf_ap_process_data_pdu(priv, skb, &ehdr, srcStaInfo,
             signal,
             bulkdata,
             macHeaderLengthInBytes)))
        {
            return;
        }
        unifi_trace(priv, UDBG5, "unifi_rx: no specific AP handling process as normal frame, MAC Header len %d\n",macHeaderLengthInBytes);
        /*                                                 */
        skb_pull(skb, macHeaderLengthInBytes);
        pData->os_data_ptr = skb->data;
        pData->data_length -= macHeaderLengthInBytes;
        pData->os_net_buf_ptr = (unsigned char*)skb;
        pData->net_buf_length = skb->len;
    }
#endif /*                 */


    /*                                                                      
                         
     */
    if (pData->data_length == 0) {
        if (((frameControl & 0x00f0) >> 4) != QOS_DATA_NULL &&
            ((frameControl & 0x00f0) >> 4) != DATA_NULL) {
            unifi_trace(priv, UDBG1, "Zero length frame, but not null-data %04x\n", frameControl);
        }
        unifi_net_data_free(priv, &bulkdata->d[0]);
        return;
    }

    if (port_action == CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_CLOSED_DISCARD) {
        /*                            */
        interfacePriv->stats.rx_dropped++;
        unifi_net_data_free(priv, &bulkdata->d[0]);
        unifi_notice(priv, "%s: Dropping packet, proto=0x%04x, %s port\n",
                     __FUNCTION__, proto, queue ? "controlled" : "uncontrolled");
        return;
    } else if ( (port_action == CSR_WIFI_ROUTER_CTRL_PORT_ACTION_8021X_PORT_CLOSED_BLOCK) ||
                   (interfacePriv->connected != UnifiConnected) ) {

        /*                                      */
        rx_buffered_packets_t *rx_q_item;
        struct list_head *rx_list;

        rx_q_item = kmalloc(sizeof(rx_buffered_packets_t),
                GFP_KERNEL);
        if (rx_q_item == NULL) {
            unifi_error(priv, "%s: Failed to allocate %d bytes for rx packet record\n",
                        __FUNCTION__, sizeof(rx_buffered_packets_t));
            interfacePriv->stats.rx_dropped++;
            unifi_net_data_free(priv, &bulkdata->d[0]);
            return;
        }

        INIT_LIST_HEAD(&rx_q_item->q);
        rx_q_item->bulkdata = *bulkdata;
        rx_q_item->skb = skb;
        rx_q_item->signal = *signal;
        memcpy(rx_q_item->sa.a, sa, ETH_ALEN);
        memcpy(rx_q_item->da.a, da, ETH_ALEN);
        unifi_trace(priv, UDBG2, "%s: Blocked skb=%p, bulkdata=%p\n",
                    __FUNCTION__, rx_q_item->skb, &rx_q_item->bulkdata);

        if (queue == UF_CONTROLLED_PORT_Q) {
            rx_list = &interfacePriv->rx_controlled_list;
        } else {
            rx_list = &interfacePriv->rx_uncontrolled_list;
        }

        /*                              */
        down(&priv->rx_q_sem);
        list_add_tail(&rx_q_item->q, rx_list);
        up(&priv->rx_q_sem);

        return;

    }

    indicate_rx_skb(priv, interfaceTag, da, sa, skb, signal, bulkdata);

} /*            */

static void process_ma_packet_cfm(unifi_priv_t *priv, CSR_SIGNAL *signal, bulk_data_param_t *bulkdata)
{
    u16 interfaceTag;
    const CSR_MA_PACKET_CONFIRM *pkt_cfm = &signal->u.MaPacketConfirm;
    netInterface_priv_t *interfacePriv;

    interfaceTag = (pkt_cfm->VirtualInterfaceIdentifier & 0xff);
    interfacePriv = priv->interfacePriv[interfaceTag];

    /*                                                          */
    if (interfaceTag >= CSR_WIFI_NUM_INTERFACES)
    {
        unifi_error(priv, "%s: MA-PACKET confirm with bad interfaceTag %d\n", __FUNCTION__, interfaceTag);
        return;
    }
#ifdef CSR_SUPPORT_SME
    if(interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_AP ||
       interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_P2PGO) {

        uf_process_ma_pkt_cfm_for_ap(priv,interfaceTag,pkt_cfm);
    } else if (interfacePriv->m4_sent && (pkt_cfm->HostTag == interfacePriv->m4_hostTag)) {
        /*                                                                                   */
        CsrResult result = pkt_cfm->TransmissionStatus == CSR_TX_SUCCESSFUL?CSR_RESULT_SUCCESS:CSR_RESULT_FAILURE;
        CsrWifiMacAddress peerMacAddress;
        memcpy(peerMacAddress.a, interfacePriv->m4_signal.u.MaPacketRequest.Ra.x, ETH_ALEN);

        unifi_trace(priv, UDBG1, "%s: Sending M4 Transmit CFM\n", __FUNCTION__);
        CsrWifiRouterCtrlM4TransmittedIndSend(priv->CSR_WIFI_SME_IFACEQUEUE, 0,
                                              interfaceTag,
                                              peerMacAddress,
                                              result);
        interfacePriv->m4_sent = FALSE;
        interfacePriv->m4_hostTag = 0xffffffff;
    }
#endif
    return;
}


/*
                                                                              
            
  
                                                                      
                                      
  
              
             
  
            
             
                                                                              
 */
static void process_ma_packet_ind(unifi_priv_t *priv, CSR_SIGNAL *signal, bulk_data_param_t *bulkdata)
{
    u16 interfaceTag;
    bulk_data_desc_t *pData;
    CSR_MA_PACKET_INDICATION *pkt_ind = (CSR_MA_PACKET_INDICATION*)&signal->u.MaPacketIndication;
    struct sk_buff *skb;
    u16 frameControl;
    netInterface_priv_t *interfacePriv;
    u8 da[ETH_ALEN], sa[ETH_ALEN];
    u8 *bssid = NULL, *ba_addr = NULL;
    u8 toDs, fromDs, frameType;
    u8 i =0;

#ifdef CSR_SUPPORT_SME
    u8 dataFrameType = 0;
    u8 powerSaveChanged = FALSE;
    u8 pmBit = 0;
    CsrWifiRouterCtrlStaInfo_t *srcStaInfo = NULL;
    u16 qosControl;

#endif

    interfaceTag = (pkt_ind->VirtualInterfaceIdentifier & 0xff);
    interfacePriv = priv->interfacePriv[interfaceTag];


    /*                                                          */
    if (interfaceTag >= CSR_WIFI_NUM_INTERFACES)
    {
        unifi_error(priv, "%s: MA-PACKET indication with bad interfaceTag %d\n", __FUNCTION__, interfaceTag);
        unifi_net_data_free(priv,&bulkdata->d[0]);
        return;
    }

    /*                                                         */
    if (!interfacePriv->netdev_registered)
    {
        unifi_error(priv, "%s: MA-PACKET indication with unallocated interfaceTag %d\n", __FUNCTION__, interfaceTag);
        unifi_net_data_free(priv, &bulkdata->d[0]);
        return;
    }

    if (bulkdata->d[0].data_length == 0) {
        unifi_warning(priv, "%s: MA-PACKET indication with zero bulk data\n", __FUNCTION__);
        unifi_net_data_free(priv,&bulkdata->d[0]);
        return;
    }
    /*                                                                               
                          */
    /*                                                                                                                         */
    if(pkt_ind->ReceptionStatus != CSR_RX_SUCCESS)
    {
        unifi_warning(priv, "%s: MA-PACKET indication with status = %d\n",__FUNCTION__, pkt_ind->ReceptionStatus);
        unifi_net_data_free(priv,&bulkdata->d[0]);
        return;
    }


    skb = (struct sk_buff*)bulkdata->d[0].os_net_buf_ptr;
    skb->len = bulkdata->d[0].data_length;

    /*                        */
    toDs = (skb->data[1] & 0x01) ? 1 : 0;
    fromDs = (skb->data[1] & 0x02) ? 1 : 0;

    memcpy(da,(skb->data+4+toDs*12),ETH_ALEN);/*               */
    memcpy(sa,(skb->data+10+fromDs*(6+toDs*8)),ETH_ALEN); /*                  */

    /*                                                            */
    if (toDs && fromDs)
    {
        unifi_trace(priv, UDBG6, "4 address frame - don't try to find BSSID\n");
        bssid = NULL;
    }
    else
    {
        bssid = (u8 *) (skb->data + 4 + 12 - (fromDs * 6) - (toDs * 12));
    }

    pData = &bulkdata->d[0];
    frameControl = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(pData->os_data_ptr);
    frameType = ((frameControl & 0x000C) >> 2);

    unifi_trace(priv, UDBG3, "Rx Frame Type: %d sn: %d\n",frameType,
         (le16_to_cpu(*((u16*)(bulkdata->d[0].os_data_ptr + IEEE802_11_SEQUENCE_CONTROL_OFFSET))) >> 4) & 0xfff);
    if(frameType == IEEE802_11_FRAMETYPE_CONTROL){
#ifdef CSR_SUPPORT_SME
        unifi_trace(priv, UDBG6, "%s: Received Control Frame\n", __FUNCTION__);

        if((frameControl & 0x00f0) == 0x00A0){
            /*                           */
            u8 pmBit = (frameControl & 0x1000)?0x01:0x00;
            unifi_trace(priv, UDBG6, "%s: Received PS-POLL Frame\n", __FUNCTION__);

            uf_process_ps_poll(priv,sa,da,pmBit,interfaceTag);
        }
        else {
            unifi_warning(priv, "%s: Non PS-POLL control frame is received\n", __FUNCTION__);
        }
#endif
        unifi_net_data_free(priv,&bulkdata->d[0]);
        return;
    }
    if(frameType != IEEE802_11_FRAMETYPE_DATA) {
        unifi_warning(priv, "%s: Non control Non Data frame is received\n",__FUNCTION__);
        unifi_net_data_free(priv,&bulkdata->d[0]);
        return;
    }

#ifdef CSR_SUPPORT_SME
    if((interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_AP) ||
       (interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_P2PGO)){

        srcStaInfo = CsrWifiRouterCtrlGetStationRecordFromPeerMacAddress(priv,sa,interfaceTag);

        if(srcStaInfo == NULL) {
            CsrWifiMacAddress peerMacAddress;
            /*                  */
            memcpy(peerMacAddress.a,sa,ETH_ALEN);
            unifi_trace(priv, UDBG1, "%s: Unexpected frame from peer = %x:%x:%x:%x:%x:%x\n", __FUNCTION__,
            sa[0], sa[1],sa[2], sa[3], sa[4],sa[5]);
            CsrWifiRouterCtrlUnexpectedFrameIndSend(priv->CSR_WIFI_SME_IFACEQUEUE,0,interfaceTag,peerMacAddress);
            unifi_net_data_free(priv, &bulkdata->d[0]);
            return;
        }

        /*
                                                                                  
                                                     

                                                                                      
                                                                                   
        */

        pmBit = (frameControl & 0x1000)?0x01:0x00;
        powerSaveChanged = uf_process_pm_bit_for_peer(priv,srcStaInfo,pmBit,interfaceTag);

        /*                                   */
        srcStaInfo->activity_flag = TRUE;

        /*                                                                                                   
                                                                                     */

        dataFrameType = ((frameControl & 0x00f0) >> 4);

        if((powerSaveChanged == FALSE)&&(srcStaInfo->wmmOrQosEnabled == TRUE)&&
        (srcStaInfo->currentPeerState == CSR_WIFI_ROUTER_CTRL_PEER_CONNECTED_POWER_SAVE)){

            if((dataFrameType == QOS_DATA) || (dataFrameType == QOS_DATA_NULL)){

                /*
                                                                                      
                                                                                      
                 */
                if((frameControl & IEEE802_11_FC_TO_DS_MASK) && (frameControl & IEEE802_11_FC_FROM_DS_MASK)){
                    qosControl= CSR_GET_UINT16_FROM_LITTLE_ENDIAN(pData->os_data_ptr + 30);
                }
                else{
                    qosControl = CSR_GET_UINT16_FROM_LITTLE_ENDIAN(pData->os_data_ptr + 24);
                }
                unifi_trace(priv, UDBG5, "%s: Check if U-APSD operations are triggered for qosControl: 0x%x\n",__FUNCTION__,qosControl);
                uf_process_wmm_deliver_ac_uapsd(priv,srcStaInfo,qosControl,interfaceTag);
            }
        }
    }

#endif

    if( ((frameControl & 0x00f0) >> 4) == QOS_DATA) {
        u8 *qos_control_ptr = (u8*)bulkdata->d[0].os_data_ptr + (((frameControl & IEEE802_11_FC_TO_DS_MASK) && (frameControl & IEEE802_11_FC_FROM_DS_MASK))?30: 24);
        int tID = *qos_control_ptr & IEEE802_11_QC_TID_MASK; /*                               */
        ba_session_rx_struct *ba_session;
        u8 ba_session_idx = 0;
        /*                               */
        if(interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_AP ||
           interfacePriv->interfaceMode == CSR_WIFI_ROUTER_CTRL_MODE_P2PGO){
            ba_addr = sa;
        }else{
            ba_addr = bssid;
        }

        down(&priv->ba_mutex);
        for (ba_session_idx=0; ba_session_idx < MAX_SUPPORTED_BA_SESSIONS_RX; ba_session_idx++){
            ba_session = interfacePriv->ba_session_rx[ba_session_idx];
            if (ba_session){
                unifi_trace(priv, UDBG6, "found ba_session=0x%x ba_session_idx=%d", ba_session, ba_session_idx);
                if ((!memcmp(ba_session->macAddress.a, ba_addr, ETH_ALEN)) && (ba_session->tID == tID)){
                        frame_desc_struct frame_desc;
                        frame_desc.bulkdata = *bulkdata;
                        frame_desc.signal = *signal;
                        frame_desc.sn = (le16_to_cpu(*((u16*)(bulkdata->d[0].os_data_ptr + IEEE802_11_SEQUENCE_CONTROL_OFFSET))) >> 4) & 0xfff;
                        frame_desc.active = TRUE;
                        unifi_trace(priv, UDBG6, "%s: calling process_ba_frame (session=%d)\n", __FUNCTION__, ba_session_idx);
                        process_ba_frame(priv, interfacePriv, ba_session, &frame_desc);
                        up(&priv->ba_mutex);
                        process_ba_complete(priv, interfacePriv);
                        break;
                }
            }
        }
        if (ba_session_idx == MAX_SUPPORTED_BA_SESSIONS_RX){
            up(&priv->ba_mutex);
            unifi_trace(priv, UDBG6, "%s: calling process_amsdu()", __FUNCTION__);
            process_amsdu(priv, signal, bulkdata);
        }
    } else {
        unifi_trace(priv, UDBG6, "calling unifi_rx()");
        unifi_rx(priv, signal, bulkdata);
    }

    /*                                                          
                                                                    
                                                                       
      
                                                                            
                              
     */
    down(&priv->ba_mutex);
    for (i=0; i < MAX_SUPPORTED_BA_SESSIONS_RX; i++){
        ba_session_rx_struct *ba_session;
        ba_session = interfacePriv->ba_session_rx[i];
            if (ba_session){
                check_ba_frame_age_timeout(priv, interfacePriv, ba_session);
            }
    }
    up(&priv->ba_mutex);
    process_ba_complete(priv, interfacePriv);

}
/*
                                                                              
                         
  
                                                                
                                         
  
              
                                               
  
            
             
  
          
                                                                              
 */

static void
uf_set_multicast_list(struct net_device *dev)
{
    netInterface_priv_t *interfacePriv = (netInterface_priv_t *)netdev_priv(dev);
    unifi_priv_t *priv = interfacePriv->privPtr;

#ifdef CSR_NATIVE_LINUX
    unifi_trace(priv, UDBG3, "uf_set_multicast_list unsupported\n");
    return;
#else

    u8 *mc_list = interfacePriv->mc_list;
    struct netdev_hw_addr *mc_addr;
    int mc_addr_count;

    if (priv->init_progress != UNIFI_INIT_COMPLETED) {
        return;
    }

    mc_addr_count = netdev_mc_count(dev);

    unifi_trace(priv, UDBG3,
            "uf_set_multicast_list (count=%d)\n", mc_addr_count);


    /*                   */
    if (mc_addr_count > UNIFI_MAX_MULTICAST_ADDRESSES) {
        return;
    }

    /*                                                  */
    interfacePriv->mc_list_count = mc_addr_count;
    netdev_hw_addr_list_for_each(mc_addr, &dev->mc) {
        memcpy(mc_list, mc_addr->addr, ETH_ALEN);
        mc_list += ETH_ALEN;
    }

    /*                                 */
    queue_work(priv->unifi_workqueue, &priv->multicast_list_task);
#endif

} /*                         */

/*
                                                                              
                             
  
                                                                               
                           
                                                                             
                                                         
                                                                          
  
              
                                                       
                                                       
                                                              
                                                                                 
                                                                      
                                               
  
            
             
                                                                              
 */
static void
netdev_mlme_event_handler(ul_client_t *pcli, const u8 *sig_packed, int sig_len,
                          const bulk_data_param_t *bulkdata_o, int dir)
{
    CSR_SIGNAL signal;
    unifi_priv_t *priv = uf_find_instance(pcli->instance);
    int id, r;
    bulk_data_param_t bulkdata;

    /*                     */
    if (sig_packed == NULL) {
        return;
    }

    /*
                                                                      
                       
     */
    bulkdata = *bulkdata_o;

    /*                         */
    r = read_unpack_signal(sig_packed, &signal);
    if (r) {
        /*
                                                                          
                                                                    
         */
        unifi_trace(priv, UDBG1,
                    "Netdev - Received unknown signal 0x%.4X.\n",
                    CSR_GET_UINT16_FROM_LITTLE_ENDIAN(sig_packed));
        return;
    }

    id = signal.SignalPrimitiveHeader.SignalId;
    unifi_trace(priv, UDBG3, "Netdev - Process signal 0x%.4X\n", id);

    /*
                                                  
     */
    switch (id) {
        case CSR_MA_PACKET_ERROR_INDICATION_ID:
            process_ma_packet_error_ind(priv, &signal, &bulkdata);
            break;
        case CSR_MA_PACKET_INDICATION_ID:
            process_ma_packet_ind(priv, &signal, &bulkdata);
            break;
        case  CSR_MA_PACKET_CONFIRM_ID:
            process_ma_packet_cfm(priv, &signal, &bulkdata);
            break;
#ifdef CSR_SUPPORT_SME
        case CSR_MLME_SET_TIM_CONFIRM_ID:
            /*                                                                                
                                                                   
             */
            uf_handle_tim_cfm(priv, &signal.u.MlmeSetTimConfirm, signal.SignalPrimitiveHeader.ReceiverProcessId);
            break;
#endif
        case CSR_DEBUG_STRING_INDICATION_ID:
            debug_string_indication(priv, bulkdata.d[0].os_data_ptr, bulkdata.d[0].data_length);
            break;

        case CSR_DEBUG_WORD16_INDICATION_ID:
            debug_word16_indication(priv, &signal);
            break;

        case CSR_DEBUG_GENERIC_CONFIRM_ID:
        case CSR_DEBUG_GENERIC_INDICATION_ID:
            debug_generic_indication(priv, &signal);
            break;
        default:
            break;
    }

} /*                             */


/*
                                                                              
                   
  
                                                                         
  
              
                                                      
                                            
                                               
  
            
            
  
          
                                                                              
 */
void uf_net_get_name(struct net_device *dev, char *name, int len)
{
    *name = '\0';
    if (dev) {
        strlcpy(name, dev->name, (len > IFNAMSIZ) ? IFNAMSIZ : len);
    }

} /*                 */

#ifdef CSR_SUPPORT_WEXT

/*
                                                                              
                   
  
                                                       
  
              
                                                    
                                                    
                                          
  
            
            
  
          
                                                                     
                                                                              
 */
static int
uf_netdev_event(struct notifier_block *notif, unsigned long event, void* ptr) {
    struct net_device *netdev = ptr;
    netInterface_priv_t *interfacePriv = (netInterface_priv_t *)netdev_priv(netdev);
    unifi_priv_t *priv = NULL;
    static const CsrWifiMacAddress broadcast_address = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

    /*                                                                         
                                    
     */
    if (uf_find_netdev_priv(interfacePriv) == -1) {
        unifi_trace(NULL, UDBG1, "uf_netdev_event: ignore e=%d, ptr=%p, priv=%p %s\n",
                    event, ptr, interfacePriv, netdev->name);
        return 0;
    }

    switch(event) {
    case NETDEV_CHANGE:
        priv = interfacePriv->privPtr;
        unifi_trace(priv, UDBG1, "NETDEV_CHANGE: %p %s %s waiting for it\n",
                    ptr,
                    netdev->name,
                    interfacePriv->wait_netdev_change ? "" : "not");

        if (interfacePriv->wait_netdev_change) {
            netif_tx_wake_all_queues(priv->netdev[interfacePriv->InterfaceTag]);
            interfacePriv->connected = UnifiConnected;
            interfacePriv->wait_netdev_change = FALSE;
            /*                                                                                                 */
            uf_process_rx_pending_queue(priv, UF_UNCONTROLLED_PORT_Q, broadcast_address, 1,interfacePriv->InterfaceTag);
            uf_process_rx_pending_queue(priv, UF_CONTROLLED_PORT_Q, broadcast_address, 1,interfacePriv->InterfaceTag);
        }
        break;

    default:
        break;
    }
    return 0;
}

static struct notifier_block uf_netdev_notifier = {
    .notifier_call = uf_netdev_event,
};
#endif /*                  */


static void
        process_amsdu(unifi_priv_t *priv, CSR_SIGNAL *signal, bulk_data_param_t *bulkdata)
{
    u32 offset;
    u32 length = bulkdata->d[0].data_length;
    u32 subframe_length, subframe_body_length, dot11_hdr_size;
    u8 *ptr;
    bulk_data_param_t subframe_bulkdata;
    u8 *dot11_hdr_ptr = (u8*)bulkdata->d[0].os_data_ptr;
    CsrResult csrResult;
    u16 frameControl;
    u8 *qos_control_ptr;

    frameControl = le16_to_cpu(*((u16*)dot11_hdr_ptr));
    qos_control_ptr = dot11_hdr_ptr + (((frameControl & IEEE802_11_FC_TO_DS_MASK) && (frameControl & IEEE802_11_FC_FROM_DS_MASK))?30: 24);
    if(!(*qos_control_ptr & IEEE802_11_QC_A_MSDU_PRESENT)) {
        unifi_trace(priv, UDBG6, "%s: calling unifi_rx()", __FUNCTION__);
        unifi_rx(priv, signal, bulkdata);
        return;
    }
    *qos_control_ptr &= ~(IEEE802_11_QC_A_MSDU_PRESENT);

    ptr = qos_control_ptr + 2;
    offset = dot11_hdr_size = ptr - dot11_hdr_ptr;

    while(length > (offset + sizeof(struct ethhdr) + sizeof(llc_snap_hdr_t))) {
        subframe_body_length = ntohs(((struct ethhdr*)ptr)->h_proto);
        if(subframe_body_length > IEEE802_11_MAX_DATA_LEN) {
            unifi_error(priv, "%s: bad subframe_body_length = %d\n", __FUNCTION__, subframe_body_length);
            break;
        }
        subframe_length = sizeof(struct ethhdr) + subframe_body_length;
        memset(&subframe_bulkdata, 0, sizeof(bulk_data_param_t));

        csrResult = unifi_net_data_malloc(priv, &subframe_bulkdata.d[0], dot11_hdr_size + subframe_body_length);

        if (csrResult != CSR_RESULT_SUCCESS) {
            unifi_error(priv, "%s: unifi_net_data_malloc failed\n", __FUNCTION__);
            break;
        }

        memcpy((u8*)subframe_bulkdata.d[0].os_data_ptr, dot11_hdr_ptr, dot11_hdr_size);


        /*                                                                                        */
        if ((frameControl & IEEE802_11_FC_TO_DS_MASK) && !(frameControl & IEEE802_11_FC_FROM_DS_MASK)){
                memcpy((u8*)subframe_bulkdata.d[0].os_data_ptr + IEEE802_11_ADDR3_OFFSET, ((struct ethhdr*)ptr)->h_dest, ETH_ALEN);
        }
        else if (!(frameControl & IEEE802_11_FC_TO_DS_MASK) && (frameControl & IEEE802_11_FC_FROM_DS_MASK)){
                memcpy((u8*)subframe_bulkdata.d[0].os_data_ptr + IEEE802_11_ADDR3_OFFSET,
                         ((struct ethhdr*)ptr)->h_source,
                           ETH_ALEN);
        }

        memcpy((u8*)subframe_bulkdata.d[0].os_data_ptr + dot11_hdr_size,
                ptr + sizeof(struct ethhdr),
                             subframe_body_length);
        unifi_trace(priv, UDBG6, "%s: calling unifi_rx. length = %d subframe_length = %d\n", __FUNCTION__, length, subframe_length);
        unifi_rx(priv, signal, &subframe_bulkdata);

        subframe_length = (subframe_length + 3)&(~0x3);
        ptr += subframe_length;
        offset += subframe_length;
    }
    unifi_net_data_free(priv, &bulkdata->d[0]);
}


#define SN_TO_INDEX(__ba_session, __sn) (((__sn - __ba_session->start_sn) & 0xFFF) % __ba_session->wind_size)


#define ADVANCE_EXPECTED_SN(__ba_session) \
{ \
    __ba_session->expected_sn++; \
    __ba_session->expected_sn &= 0xFFF; \
}

#define FREE_BUFFER_SLOT(__ba_session, __index) \
{ \
    __ba_session->occupied_slots--; \
    __ba_session->buffer[__index].active = FALSE; \
    ADVANCE_EXPECTED_SN(__ba_session); \
}

static void add_frame_to_ba_complete(unifi_priv_t *priv,
                          netInterface_priv_t *interfacePriv,
                          frame_desc_struct *frame_desc)
{
    interfacePriv->ba_complete[interfacePriv->ba_complete_index] = *frame_desc;
    interfacePriv->ba_complete_index++;
}


static void update_expected_sn(unifi_priv_t *priv,
                          netInterface_priv_t *interfacePriv,
                          ba_session_rx_struct *ba_session,
                          u16 sn)
{
    int i, j;
    u16 gap;

    gap = (sn - ba_session->expected_sn) & 0xFFF;
    unifi_trace(priv, UDBG6, "%s: process the frames up to new_expected_sn = %d gap = %d\n", __FUNCTION__, sn, gap);
    for(j = 0; j < gap && j < ba_session->wind_size; j++) {
        i = SN_TO_INDEX(ba_session, ba_session->expected_sn);
        unifi_trace(priv, UDBG6, "%s: process the slot index = %d\n", __FUNCTION__, i);
        if(ba_session->buffer[i].active) {
            add_frame_to_ba_complete(priv, interfacePriv, &ba_session->buffer[i]);
            unifi_trace(priv, UDBG6, "%s: process the frame at index = %d expected_sn = %d\n", __FUNCTION__, i, ba_session->expected_sn);
            FREE_BUFFER_SLOT(ba_session, i);
        } else {
            unifi_trace(priv, UDBG6, "%s: empty slot at index = %d\n", __FUNCTION__, i);
            ADVANCE_EXPECTED_SN(ba_session);
        }
    }
    ba_session->expected_sn = sn;
}


static void complete_ready_sequence(unifi_priv_t *priv,
                               netInterface_priv_t *interfacePriv,
                               ba_session_rx_struct *ba_session)
{
    int i;

    i = SN_TO_INDEX(ba_session, ba_session->expected_sn);
    while (ba_session->buffer[i].active) {
        add_frame_to_ba_complete(priv, interfacePriv, &ba_session->buffer[i]);
        unifi_trace(priv, UDBG6, "%s: completed stored frame(expected_sn=%d) at i = %d\n", __FUNCTION__, ba_session->expected_sn, i);
        FREE_BUFFER_SLOT(ba_session, i);
        i = SN_TO_INDEX(ba_session, ba_session->expected_sn);
    }
}


void scroll_ba_window(unifi_priv_t *priv,
                                netInterface_priv_t *interfacePriv,
                                ba_session_rx_struct *ba_session,
                                u16 sn)
{
    if(((sn - ba_session->expected_sn) & 0xFFF) <= 2048) {
        update_expected_sn(priv, interfacePriv, ba_session, sn);
        complete_ready_sequence(priv, interfacePriv, ba_session);
    }
}


static int consume_frame_or_get_buffer_index(unifi_priv_t *priv,
                                            netInterface_priv_t *interfacePriv,
                                            ba_session_rx_struct *ba_session,
                                            u16 sn,
                                            frame_desc_struct *frame_desc) {
    int i;
    u16 sn_temp;

    if(((sn - ba_session->expected_sn) & 0xFFF) <= 2048) {

        /*                                                       */
        if(!ba_session->trigger_ba_after_ssn){
            ba_session->trigger_ba_after_ssn = TRUE;
        }

        sn_temp = ba_session->expected_sn + ba_session->wind_size;
        unifi_trace(priv, UDBG6, "%s: new frame: sn=%d\n", __FUNCTION__, sn);
        if(!(((sn - sn_temp) & 0xFFF) > 2048)) {
            u16 new_expected_sn;
            unifi_trace(priv, UDBG6, "%s: frame is out of window\n", __FUNCTION__);
            sn_temp = (sn - ba_session->wind_size) & 0xFFF;
            new_expected_sn = (sn_temp + 1) & 0xFFF;
            update_expected_sn(priv, interfacePriv, ba_session, new_expected_sn);
        }
        i = -1;
        if (sn == ba_session->expected_sn) {
            unifi_trace(priv, UDBG6, "%s: sn = ba_session->expected_sn = %d\n", __FUNCTION__, sn);
            ADVANCE_EXPECTED_SN(ba_session);
            add_frame_to_ba_complete(priv, interfacePriv, frame_desc);
        } else {
            i = SN_TO_INDEX(ba_session, sn);
            unifi_trace(priv, UDBG6, "%s: sn(%d) != ba_session->expected_sn(%d), i = %d\n", __FUNCTION__, sn, ba_session->expected_sn, i);
            if (ba_session->buffer[i].active) {
                unifi_trace(priv, UDBG6, "%s: free frame at i = %d\n", __FUNCTION__, i);
                i = -1;
                unifi_net_data_free(priv, &frame_desc->bulkdata.d[0]);
            }
        }
    } else {
        i = -1;
        if(!ba_session->trigger_ba_after_ssn){
            unifi_trace(priv, UDBG6, "%s: frame before ssn, pass it up: sn=%d\n", __FUNCTION__, sn);
            add_frame_to_ba_complete(priv, interfacePriv, frame_desc);
        }else{
            unifi_trace(priv, UDBG6, "%s: old frame, drop: sn=%d, expected_sn=%d\n", __FUNCTION__, sn, ba_session->expected_sn);
            unifi_net_data_free(priv, &frame_desc->bulkdata.d[0]);
        }
    }
    return i;
}



static void process_ba_frame(unifi_priv_t *priv,
                                             netInterface_priv_t *interfacePriv,
                                             ba_session_rx_struct *ba_session,
                                             frame_desc_struct *frame_desc)
{
    int i;
    u16 sn = frame_desc->sn;

    if (ba_session->timeout) {
        mod_timer(&ba_session->timer, (jiffies + usecs_to_jiffies((ba_session->timeout) * 1024)));
    }
    unifi_trace(priv, UDBG6, "%s: got frame(sn=%d)\n", __FUNCTION__, sn);

    i = consume_frame_or_get_buffer_index(priv, interfacePriv, ba_session, sn, frame_desc);
    if(i >= 0) {
        unifi_trace(priv, UDBG6, "%s: store frame(sn=%d) at i = %d\n", __FUNCTION__, sn, i);
        ba_session->buffer[i] = *frame_desc;
        ba_session->buffer[i].recv_time = CsrTimeGet(NULL);
        ba_session->occupied_slots++;
    } else {
        unifi_trace(priv, UDBG6, "%s: frame consumed - sn = %d\n", __FUNCTION__, sn);
    }
    complete_ready_sequence(priv, interfacePriv, ba_session);
}


static void process_ba_complete(unifi_priv_t *priv, netInterface_priv_t *interfacePriv)
{
    frame_desc_struct *frame_desc;
    u8 i;

    for(i = 0; i < interfacePriv->ba_complete_index; i++) {
        frame_desc = &interfacePriv->ba_complete[i];
        unifi_trace(priv, UDBG6, "%s: calling process_amsdu()\n", __FUNCTION__);
        process_amsdu(priv, &frame_desc->signal, &frame_desc->bulkdata);
    }
    interfacePriv->ba_complete_index = 0;

}


/*                                                     
                                                       
             
 */
static void check_ba_frame_age_timeout( unifi_priv_t *priv,
                                        netInterface_priv_t *interfacePriv,
                                        ba_session_rx_struct *ba_session)
{
    u32 now;
    u32 age;
    u8 i, j;
    u16 sn_temp;

    /*                                                        
                                      
     */
    u8 gap=1;

    now = CsrTimeGet(NULL);

    if (ba_session->occupied_slots)
    {
        /*                                                               
                                                                            
                                                                       
                                                          
         */
        sn_temp = (ba_session->expected_sn + 1) & 0xFFF;

        for(j = 0; j < ba_session->wind_size; j++)
        {
            i = SN_TO_INDEX(ba_session, sn_temp);

            if(ba_session->buffer[i].active)
            {
                unifi_trace(priv, UDBG6, "check age at slot index = %d sn = %d recv_time = %u now = %u\n",
                                        i,
                                        ba_session->buffer[i].sn,
                                        ba_session->buffer[i].recv_time,
                                        now);

                if (ba_session->buffer[i].recv_time > now)
                {
                    /*            */
                    age = CsrTimeAdd((u32)CsrTimeSub(CSR_SCHED_TIME_MAX, ba_session->buffer[i].recv_time), now);
                }
                else
                {
                    age = (u32)CsrTimeSub(now, ba_session->buffer[i].recv_time);
                }

                if (age >= CSR_WIFI_BA_MPDU_FRAME_AGE_TIMEOUT)
                {
                    unifi_trace(priv, UDBG2, "release the frame at index = %d gap = %d expected_sn = %d sn = %d\n",
                                            i,
                                            gap,
                                            ba_session->expected_sn,
                                            ba_session->buffer[i].sn);

                    /*                                                                   */
                    while (gap--)
                    {
                        ADVANCE_EXPECTED_SN(ba_session);
                    }
                    add_frame_to_ba_complete(priv, interfacePriv, &ba_session->buffer[i]);
                    FREE_BUFFER_SLOT(ba_session, i);
                    complete_ready_sequence(priv, interfacePriv, ba_session);
                }
                break;

            }
            else
            {
                /*                                            */
                sn_temp = (sn_temp + 1) & 0xFFF;
                gap++;
            }
        }
    }
}


static void process_ma_packet_error_ind(unifi_priv_t *priv, CSR_SIGNAL *signal, bulk_data_param_t *bulkdata)
{
    u16 interfaceTag;
    const CSR_MA_PACKET_ERROR_INDICATION *pkt_err_ind = &signal->u.MaPacketErrorIndication;
    netInterface_priv_t *interfacePriv;
    ba_session_rx_struct *ba_session;
    u8 ba_session_idx = 0;
    CSR_PRIORITY        UserPriority;
    CSR_SEQUENCE_NUMBER sn;

    interfaceTag = (pkt_err_ind->VirtualInterfaceIdentifier & 0xff);


    /*                                                          */
    if (interfaceTag >= CSR_WIFI_NUM_INTERFACES)
    {
        unifi_error(priv, "%s: MaPacketErrorIndication indication with bad interfaceTag %d\n", __FUNCTION__, interfaceTag);
        return;
    }

    interfacePriv = priv->interfacePriv[interfaceTag];
    UserPriority = pkt_err_ind->UserPriority;
    if(UserPriority > 15) {
        unifi_error(priv, "%s: MaPacketErrorIndication indication with bad UserPriority=%d\n", __FUNCTION__, UserPriority);
    }
    sn = pkt_err_ind->SequenceNumber;

    down(&priv->ba_mutex);
    /*                                                                                        */
    for (ba_session_idx=0; ba_session_idx < MAX_SUPPORTED_BA_SESSIONS_RX; ba_session_idx++){
        ba_session = interfacePriv->ba_session_rx[ba_session_idx];
        if (ba_session){
            if ((!memcmp(ba_session->macAddress.a, pkt_err_ind->PeerQstaAddress.x, ETH_ALEN)) && (ba_session->tID == UserPriority)){
                if (ba_session->timeout) {
                    mod_timer(&ba_session->timer, (jiffies + usecs_to_jiffies((ba_session->timeout) * 1024)));
                }
                scroll_ba_window(priv, interfacePriv, ba_session, sn);
                break;
            }
        }
    }

    up(&priv->ba_mutex);
    process_ba_complete(priv, interfacePriv);
}


