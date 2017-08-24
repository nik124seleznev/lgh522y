/*
                                                                                          
*/

/*                 
                                                             

                                                                                  
                                                                            
                                                                                  
                        
*/



/*
                  
  
                        
       
                         
  
                        
                                                                                                                                                                                                     
                                                            
  
                      
       
                          
                                                              

  
                      
       
                           
  
                   
                                                                                                     
                                                                        
  
                      
                                                                      
                                                                 
  
                        
       
                                                     
  
                        
       
                           
  
                        
       
                                
  
                   
                                                                             
                                                   
  
                      
       
                            
  
                      
                                                                           
                                     
  
                   
                                                                                                                                                    
                                                                                                            
  
                   
                                                                                
                          
  
                           
                                                                                
    
  
                           
                                                                                                                    
                             
  
                           
                                                                                                                    
                                                                      
  
                   
                                                                             
                           
  
                   
                                                                                                      
                                                         
  
                           
       
                                                                       
  
                         
                                                 
                               
                                                   
                                                                                  
                                              
                                                                          
  
                   
                                                                                                                                   
                                                                
  
                      
                                                                                       
                                                                                     
  
                      
                                                                                                             
    
  
                      
                                                                                    
    
  
                   
                                                                                                           
                                                     
  
                   
                                                                                                                                              
                                                        
  
                           
       
                                                                                                                           
                                                                          
  
                   
                                                                                                                                             
                                                   
  
                   
                                                                                                                                 
                                                  
  
                   
                                                                     
                                      
  
                   
                                                                                                                                                              
                                                  
  
                   
       
                                                                            
  
                   
                                                                                 
                                     
  
                   
       
                                   
  
                   
       
                                                                 
  
                   
       
                                                                                                
  
                         
       
                                                                     
  
                   
       
                                                
  
                         
       
                                                 
  
                   
       
                                                                                                              
  
                   
       
                                                                                                              
  
                   
       
                                                      
  
                   
       
                                                                          
                                                   
  
                   
       
                                                                     
  
                   
       
                                                
  
                           
       
                                                    
                                              
  
                           
  
                  
  
                           
  
                                            
  
                           
  
                                   
  
                   
  
                                                                               
  
                         
                                                
                                   
  
                   
                                                                     
                                                      
  
                   
                                                                     
                                                                        
  
                   
                                                                     
                                                                                    
  
                   
                                                                     
                      
  
                   
                                                                     
                                                      
  
                   
                                                                     
                                         
  
                           
                                             
                                                    
  
                   
                                                                     
                                                          
  
                   
                                                               
                                                                                   
  
                   
                                                               
                                                                    
  
                   
                                                               
                                     
  
                           
                                             
                               
                                                               
  
                           
                                             
                                                               
  
                   
                                                          
                            
  
                           
                                             
                              
  
                           
                                             
                                                                          
  
                   
                                                               
                                           
                                                                                                                                    
                                                                                                                  
                                                                                                                         
  
                   
                                                          
                                                                                
                                                        
  
                           
                                             
                                    
  
                           
                                             
                                                      
                       
  
                           
                                             
                                          
  
                           
                                             
                                     
  
                   
                                                          
                                                                                                      
  
                   
                                                          
                                                                            
  
                           
                                             
                                    
  
                           
                                             
                                              
                                             
  
                   
                                                          
                                                                       
                                                           
                                                           
  
                   
                                                               
                                             
  
                           
                                             
                                
                                                              
                                                                                                                                    
                                                              
                                                                                                                                         
                                                              
                                                                                                           
                                                              
                                           
                                     
                                 
                                                              
                                      
                                                              
                                        
                                                              
                                                                                                 
                                                              
                                                                             
                                                              
                                                                         
                                                              
                                                                                                                       
                                                              
                                                           
                                                              
                                 
                                                              
                                 
                                                              
                                                                           
                                                              
                                
                                                              
                           
                                                              
                                           
                                                              
                           
                                                              
                                   
                                                              
                                          
                                                              
                          
  
                                                             
                   
                                                             
                                                                         
                                              
                                     
  
                                              
                                                             
  
                                              
                                                                          
  
                                              
                                                                          
  
*/

#ifndef _GL_OS_H
#define _GL_OS_H

/*                                                                              
                                                     
                                                                                
*/
/*                                                                              
                                
                                                                                
 */
#define CFG_MAX_WLAN_DEVICES                1	/*                                  */

#define CFG_MAX_TXQ_NUM                     4	/*                                                 */


#define CFG_USE_SPIN_LOCK_BOTTOM_HALF       0	/*                                                 
                                                     */

#define CFG_TX_PADDING_SMALL_ETH_PACKET     0	/*                                                   
                                                               
                    */

#define CFG_TX_STOP_NETIF_QUEUE_THRESHOLD   256	/*         */

#define CFG_TX_STOP_NETIF_PER_QUEUE_THRESHOLD   64	/*         */
#define CFG_TX_START_NETIF_PER_QUEUE_THRESHOLD  32	/*         */


#define ETH_P_1X                            0x888E
#define IPTOS_PREC_OFFSET                   5
#define USER_PRIORITY_DEFAULT               0

#define ETH_WPI_1X                         0x88B4

#define ETH_HLEN                                14
#define ETH_TYPE_LEN_OFFSET                     12
#define ETH_P_IP                                0x0800
#define ETH_P_1X                                0x888E
#define ETH_P_PRE_1X                            0x88C7

#define IPVERSION                               4
#define IP_HEADER_LEN                           20

#define IPVH_VERSION_OFFSET                     4	/*                   */
#define IPVH_VERSION_MASK                       0xF0
#define IPTOS_PREC_OFFSET                       5
#define IPTOS_PREC_MASK                         0xE0

#define SOURCE_PORT_LEN                         2
/*                                       */
#define LOOK_AHEAD_LEN                          (ETH_HLEN + IP_HEADER_LEN + SOURCE_PORT_LEN)

/*                */
#define ETH_LLC_OFFSET                          (ETH_HLEN)
#define ETH_LLC_LEN                             3
#define ETH_LLC_DSAP_SNAP                       0xAA
#define ETH_LLC_SSAP_SNAP                       0xAA
#define ETH_LLC_CONTROL_UNNUMBERED_INFORMATION  0x03

/*                */
#define ETH_SNAP_OFFSET                         (ETH_HLEN + ETH_LLC_LEN)
#define ETH_SNAP_LEN                            5
#define ETH_SNAP_BT_SIG_OUI_0                   0x00
#define ETH_SNAP_BT_SIG_OUI_1                   0x19
#define ETH_SNAP_BT_SIG_OUI_2                   0x58

#define BOW_PROTOCOL_ID_SECURITY_FRAME          0x0003


#if defined(MT6620)
#define CHIP_NAME    "MT6620"
#elif defined(MT5931)
#define CHIP_NAME    "MT5931"
#elif defined(MT6628)
#define CHIP_NAME    "MT6628"
#endif

#define DRV_NAME "["CHIP_NAME"]: "

/*                                      
                                                        
 */
#ifndef CONFIG_ANDROID
#define CONFIG_ANDROID      0
#endif

/*                                     */
#define CFG_CFG80211_IE_BUF_LEN     (512)

/*                                                                              
                                                          
                                                                                
*/
#include <linux/version.h>	/*                            */

#include <linux/kernel.h>	/*          */

#include <linux/timer.h>	/*                   */
#include <linux/jiffies.h>	/*         */
#include <linux/delay.h>	/*                         */

#if CONFIG_ANDROID
#include <linux/wakelock.h>
#endif

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 12)
#include <linux/irq.h>		/*              */
#endif

#include <linux/netdevice.h>	/*                                            */
#include <linux/etherdevice.h>	/*                               */
#include <linux/wireless.h>	/*                      */
#include <linux/if_arp.h>
#include <linux/inetdevice.h>	/*                  */

#include <linux/ip.h>		/*              */

#include <linux/string.h>	/*                                */
#include <linux/stddef.h>	/*                      */

#include <linux/proc_fs.h>	/*                                          */

#include <linux/rtnetlink.h>	/*                                   */
#include <net/addrconf.h>	/*             */
#include <net/if_inet6.h>	/*             */
#include <linux/kthread.h>	/*                                      */
#include <asm/uaccess.h>	/*                      */
#include <linux/fs.h>		/*                       */
#include <linux/vmalloc.h>

#include <linux/kfifo.h>	/*                     */
#include <linux/cdev.h>		/*                    */

#include <linux/firmware.h>	/*                       */

#if defined(_HIF_SDIO)
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_func.h>
#endif

#include <linux/random.h>

#include <linux/lockdep.h>

#include <asm/io.h>		/*                  */

#if WIRELESS_EXT > 12
#include <net/iw_handler.h>
#endif

#include "version.h"
#include "config.h"

#if CFG_ENABLE_WIFI_DIRECT_CFG_80211
#include <linux/wireless.h>
#include <net/cfg80211.h>
#endif

#include <linux/module.h>

#if defined(MTK_WLAN_ARP_OFFLOAD)
#include <net/rtnetlink.h>
#include <net/sock.h>
#endif


#include "gl_typedef.h"
#include "typedef.h"
#include "queue.h"
#include "gl_kal.h"
#if CFG_CHIP_RESET_SUPPORT
#include "gl_rst.h"
#endif
#include "hif.h"


#include "debug.h"

#include "wlan_lib.h"
#include "wlan_oid.h"

#if CFG_ENABLE_AEE_MSG
#include <linux/aee.h>
#endif

extern BOOLEAN fgIsBusAccessFailed;

/*                                                                              
                                                
                                                                                
*/
#define GLUE_FLAG_HALT          BIT(0)
#define GLUE_FLAG_INT           BIT(1)
#define GLUE_FLAG_OID           BIT(2)
#define GLUE_FLAG_TIMEOUT       BIT(3)
#define GLUE_FLAG_TXREQ         BIT(4)
#define GLUE_FLAG_SUB_MOD_INIT  BIT(5)
#define GLUE_FLAG_SUB_MOD_EXIT  BIT(6)
#define GLUE_FLAG_SUB_MOD_MULTICAST  BIT(7)
#define GLUE_FLAG_FRAME_FILTER      BIT(8)
#define GLUE_FLAG_HALT_BIT          (0)
#define GLUE_FLAG_INT_BIT           (1)
#define GLUE_FLAG_OID_BIT           (2)
#define GLUE_FLAG_TIMEOUT_BIT       (3)
#define GLUE_FLAG_TXREQ_BIT         (4)
#define GLUE_FLAG_SUB_MOD_INIT_BIT  (5)
#define GLUE_FLAG_SUB_MOD_EXIT_BIT  (6)
#define GLUE_FLAG_SUB_MOD_MULTICAST_BIT  (7)
#define GLUE_FLAG_FRAME_FILTER_BIT  (8)



#define GLUE_BOW_KFIFO_DEPTH        (1024)
/*                                                         */
#define GLUE_BOW_DEVICE_NAME        "ampc0"


/*                                                                              
                                                 
                                                                                
*/
typedef struct _GL_WPA_INFO_T {
	UINT_32 u4WpaVersion;
	UINT_32 u4KeyMgmt;
	UINT_32 u4CipherGroup;
	UINT_32 u4CipherPairwise;
	UINT_32 u4AuthAlg;
	BOOLEAN fgPrivacyInvoke;
#if CFG_SUPPORT_802_11W
	UINT_32 u4Mfp;
#endif
} GL_WPA_INFO_T, *P_GL_WPA_INFO_T;

typedef enum _ENUM_RSSI_TRIGGER_TYPE {
	ENUM_RSSI_TRIGGER_NONE,
	ENUM_RSSI_TRIGGER_GREATER,
	ENUM_RSSI_TRIGGER_LESS,
	ENUM_RSSI_TRIGGER_TRIGGERED,
	ENUM_RSSI_TRIGGER_NUM
} ENUM_RSSI_TRIGGER_TYPE;

#if CFG_ENABLE_WIFI_DIRECT
typedef enum _ENUM_SUB_MODULE_IDX_T {
	P2P_MODULE = 0,
	SUB_MODULE_NUM
} ENUM_SUB_MODULE_IDX_T;

typedef enum _ENUM_NET_REG_STATE_T {
	ENUM_NET_REG_STATE_UNREGISTERED,
	ENUM_NET_REG_STATE_REGISTERING,
	ENUM_NET_REG_STATE_REGISTERED,
	ENUM_NET_REG_STATE_UNREGISTERING,
	ENUM_NET_REG_STATE_NUM
} ENUM_NET_REG_STATE_T;

#endif

typedef struct _GL_IO_REQ_T {
	QUE_ENTRY_T rQueEntry;
	/*                                    */
	BOOL fgRead;
	BOOL fgWaitResp;
#if CFG_ENABLE_WIFI_DIRECT
	BOOL fgIsP2pOid;
#endif
	P_ADAPTER_T prAdapter;
	PFN_OID_HANDLER_FUNC pfnOidHandler;
	PVOID pvInfoBuf;
	UINT_32 u4InfoBufLen;
	PUINT_32 pu4QryInfoLen;
	WLAN_STATUS rStatus;
	UINT_32 u4Flag;
} GL_IO_REQ_T, *P_GL_IO_REQ_T;

#if CFG_ENABLE_BT_OVER_WIFI
typedef struct _GL_BOW_INFO {
	BOOLEAN fgIsRegistered;
	dev_t u4DeviceNumber;	/*                       */
/*                                                                           */ */
	struct kfifo rKfifo;	/*                                */
	spinlock_t rSpinLock;	/*                     */
	struct cdev cdev;
	UINT_32 u4FreqInKHz;	/*           */

	UINT_8 aucRole[CFG_BOW_PHYSICAL_LINK_NUM];	/*                            */
	ENUM_BOW_DEVICE_STATE aeState[CFG_BOW_PHYSICAL_LINK_NUM];
	PARAM_MAC_ADDRESS arPeerAddr[CFG_BOW_PHYSICAL_LINK_NUM];

	wait_queue_head_t outq;

#if CFG_BOW_SEPARATE_DATA_PATH
	/*               */
	struct net_device *prDevHandler;
	BOOLEAN fgIsNetRegistered;
#endif

} GL_BOW_INFO, *P_GL_BOW_INFO;
#endif

/*
                                             
*/
typedef struct _GL_P2P_INFO_T GL_P2P_INFO_T, *P_GL_P2P_INFO_T;

struct _GLUE_INFO_T {
	/*               */
	struct net_device *prDevHandler;

	/*                                        */
	INT_32 i4DevIdx;

	/*                   */
	struct net_device_stats rNetDevStats;

	/*                                       */
	struct iw_statistics rIwStats;

	/*                                       */
	spinlock_t rSpinLock[SPIN_LOCK_NUM];

	/*                     */
	struct semaphore ioctl_sem;

	UINT_32 u4Flag;		/*               */
	UINT_32 u4PendFlag;
	/*                        */
	UINT_32 u4OidCompleteFlag;
	UINT_32 u4ReadyFlag;	/*                        */

	/*                                                                 
                                                   
   
                                                                     
                                                       
  */
	INT_32 ai4TxPendingFrameNumPerQueue[4][CFG_MAX_TXQ_NUM];
	INT_32 i4TxPendingFrameNum;
	INT_32 i4TxPendingSecurityFrameNum;

	/*                                 */
	GL_IO_REQ_T OidEntry;

	/*               */
	REG_INFO_T rRegInfo;

	/*          */
	struct firmware *prFw;

	/*                                    */
	/*                                    */
	GL_HIF_INFO_T rHifInfo;

	/*                                      */
	GL_WPA_INFO_T rWpaInfo;


	/*                                                                       */
	P_ADAPTER_T prAdapter;

#ifdef WLAN_INCLUDE_PROC
	struct proc_dir_entry *pProcRoot;
#endif				/*                   */

	/*                       */
	ENUM_PARAM_MEDIA_STATE_T eParamMediaStateIndicated;

	/*                          */
	PARAM_DEVICE_POWER_STATE ePowerState;

	struct completion rScanComp;	/*                        */
	struct completion rHaltComp;	/*                                    */
	struct completion rPendComp;	/*                                    */
#if CFG_ENABLE_WIFI_DIRECT
	struct completion rSubModComp;	/*                                          */
#endif
	WLAN_STATUS rPendStatus;

	QUE_T rTxQueue;


	/*             */
	QUE_T rCmdQueue;
	/*                                              */
	/*                                                    */
	/*                                     */
	/*                                       */
	/*                                      */
	/*                                          */

	wait_queue_head_t waitq;
	struct task_struct *main_thread;

	struct timer_list tickfn;


#if CFG_SUPPORT_EXT_CONFIG
	UINT_16 au2ExtCfg[256];	/*                   */
	UINT_32 u4ExtCfgLength;	/*                           */
#endif

#if 1				/*                  */
	/*                                                  */
	UINT_8 aucWapiAssocInfoIEs[42];
	UINT_16 u2WapiAssocInfoIESz;
#endif

#if CFG_ENABLE_BT_OVER_WIFI
	GL_BOW_INFO rBowInfo;
#endif

#if CFG_ENABLE_WIFI_DIRECT
	P_GL_P2P_INFO_T prP2PInfo;
#if CFG_SUPPORT_P2P_RSSI_QUERY
	/*                                       */
	struct iw_statistics rP2pIwStats;
#endif
#endif
	BOOLEAN fgWpsActive;
	UINT_8 aucWSCIE[500];	/*              */
	UINT_16 u2WSCIELen;
	UINT_8 aucWSCAssocInfoIE[200];	/*              */
	UINT_16 u2WSCAssocInfoIELen;

	/*                    */
	BOOLEAN fgNvramAvailable;

	BOOLEAN fgMcrAccessAllowed;

	/*                                */
	BOOLEAN fgIsMacAddrOverride;
	PARAM_MAC_ADDRESS rMacAddrOverride;

	SET_TXPWR_CTRL_T rTxPwr;

	/*                                   */
	struct cfg80211_scan_request *prScanRequest;

	/*                               */
	BOOLEAN fgIsRegistered;

	/*                                   */
	UINT_32 u4RspIeLength;
	UINT_8 aucRspIe[CFG_CFG80211_IE_BUF_LEN];

	UINT_32 u4ReqIeLength;
	UINT_8 aucReqIe[CFG_CFG80211_IE_BUF_LEN];
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
    /*           */
typedef void (*PFN_WLANISR) (int irq, void *dev_id, struct pt_regs *regs);
#else
typedef irqreturn_t(*PFN_WLANISR) (int irq, void *dev_id, struct pt_regs *regs);
#endif

typedef void (*PFN_LINUX_TIMER_FUNC) (unsigned long);


/*                                     
                                         
*/
#if CFG_ENABLE_WIFI_DIRECT
typedef BOOLEAN(*SUB_MODULE_INIT) (P_GLUE_INFO_T prGlueInfo);
typedef BOOLEAN(*SUB_MODULE_EXIT) (P_GLUE_INFO_T prGlueInfo);

typedef struct _SUB_MODULE_HANDLER {
	SUB_MODULE_INIT subModInit;
	SUB_MODULE_EXIT subModExit;
	BOOLEAN fgIsInited;
} SUB_MODULE_HANDLER, *P_SUB_MODULE_HANDLER;

#endif

#if CONFIG_NL80211_TESTMODE

typedef struct _NL80211_DRIVER_TEST_MODE_PARAMS {
	UINT_32 index;
	UINT_32 buflen;
} NL80211_DRIVER_TEST_MODE_PARAMS, *P_NL80211_DRIVER_TEST_MODE_PARAMS;

/*       */
typedef struct _NL80211_DRIVER_SW_CMD_PARAMS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	UINT_8 set;
	unsigned long adr;
	unsigned long data;
} NL80211_DRIVER_SW_CMD_PARAMS, *P_NL80211_DRIVER_SW_CMD_PARAMS;

struct iw_encode_exts {
	__u32 ext_flags;	/*                   */
	__u8 tx_seq[IW_ENCODE_SEQ_MAX_SIZE];	/*             */
	__u8 rx_seq[IW_ENCODE_SEQ_MAX_SIZE];	/*             */
	__u8 addr[MAC_ADDR_LEN];	/*                                            
                                             
                          */
	__u16 alg;		/*                   */
	__u16 key_len;
	__u8 key[32];
};

/*            */
typedef struct _NL80211_DRIVER_SET_KEY_EXTS {
	NL80211_DRIVER_TEST_MODE_PARAMS hdr;
	UINT_8 key_index;
	UINT_8 key_len;
	struct iw_encode_exts ext;
} NL80211_DRIVER_SET_KEY_EXTS, *P_NL80211_DRIVER_SET_KEY_EXTS;


#endif

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
/*                                                                            */
/*                                                                            */
/*                                                                            */
#if CFG_USE_SPIN_LOCK_BOTTOM_HALF
#define GLUE_SPIN_LOCK_DECLARATION()
#define GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
	    { \
		if (rLockCategory < SPIN_LOCK_NUM) \
		spin_lock_bh(&(prGlueInfo->rSpinLock[rLockCategory])); \
	    }
#define GLUE_RELEASE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
	    { \
		if (rLockCategory < SPIN_LOCK_NUM) \
		spin_unlock_bh(&(prGlueInfo->rSpinLock[rLockCategory])); \
	    }
#else				/*                                */
#define GLUE_SPIN_LOCK_DECLARATION()                        UINT_32 __u4Flags = 0
#define GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
	    { \
		if (rLockCategory < SPIN_LOCK_NUM) \
		spin_lock_irqsave(&(prGlueInfo)->rSpinLock[rLockCategory], __u4Flags); \
	    }
#define GLUE_RELEASE_SPIN_LOCK(prGlueInfo, rLockCategory)   \
	    { \
		if (rLockCategory < SPIN_LOCK_NUM) \
		spin_unlock_irqrestore(&(prGlueInfo->rSpinLock[rLockCategory]), __u4Flags); \
	    }
#endif				/*                                */


/*                                                                            */
/*                                                                            */
/*                                                                            */
#define GLUE_GET_PKT_QUEUE_ENTRY(_p)    \
	    (&(((struct sk_buff *)(_p))->cb[0]))

#define GLUE_GET_PKT_DESCRIPTOR(_prQueueEntry)  \
	    ((P_NATIVE_PACKET) ((UINT_32)_prQueueEntry - offsetof(struct sk_buff, cb[0])))

#define  GLUE_SET_PKT_FLAG_802_11(_p)  \
            (*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) |= BIT(7))

#define GLUE_SET_PKT_FLAG_1X(_p)  \
            (*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) |= BIT(6))

#define GLUE_SET_PKT_FLAG_PAL(_p)  \
            (*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) |= BIT(5))

#define GLUE_SET_PKT_FLAG_P2P(_p)  \
            (*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) |= BIT(4))



#define GLUE_SET_PKT_TID(_p, _tid)  \
            (*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) |= (((UINT_8)((_tid) & (BITS(0, 3))))))


#define GLUE_SET_PKT_FRAME_LEN(_p, _u2PayloadLen) \
            (*((PUINT_16) & (((struct sk_buff *)(_p))->cb[6])) = (UINT_16)(_u2PayloadLen))

#define GLUE_GET_PKT_FRAME_LEN(_p)    \
            (*((PUINT_16) & (((struct sk_buff *)(_p))->cb[6])) )


#define  GLUE_GET_PKT_IS_802_11(_p)        \
            ((*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) ) & (BIT(7)))

#define  GLUE_GET_PKT_IS_1X(_p)        \
            ((*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) ) & (BIT(6)))

#define GLUE_GET_PKT_TID(_p)        \
            ((*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) ) & (BITS(0, 3)))


#define GLUE_GET_PKT_IS_PAL(_p)        \
            ((*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) ) & (BIT(5)))

#define GLUE_GET_PKT_IS_P2P(_p)        \
            ((*((PUINT_8) & (((struct sk_buff *)(_p))->cb[4])) ) & (BIT(4)))


#define GLUE_SET_PKT_HEADER_LEN(_p, _ucMacHeaderLen)    \
            (*((PUINT_8) & (((struct sk_buff *)(_p))->cb[5])) = (UINT_8)(_ucMacHeaderLen))

#define GLUE_GET_PKT_HEADER_LEN(_p) \
            (*((PUINT_8) & (((struct sk_buff *)(_p))->cb[5])) )

#define GLUE_SET_PKT_ARRIVAL_TIME(_p, _rSysTime) \
            (*((POS_SYSTIME) & (((struct sk_buff *)(_p))->cb[8])) = (OS_SYSTIME)(_rSysTime))

#define GLUE_GET_PKT_ARRIVAL_TIME(_p)    \
            (*((POS_SYSTIME) & (((struct sk_buff *)(_p))->cb[8])) )

/*                                                     */
#define GLUE_CHK_DEV(prDev) \
    ((prDev && *((P_GLUE_INFO_T *) netdev_priv(prDev))) ? TRUE : FALSE)

#define GLUE_CHK_PR2(prDev, pr2) \
    ((GLUE_CHK_DEV(prDev) && pr2) ? TRUE : FALSE)

#define GLUE_CHK_PR3(prDev, pr2, pr3) \
    ((GLUE_CHK_PR2(prDev, pr2) && pr3) ? TRUE : FALSE)

#define GLUE_CHK_PR4(prDev, pr2, pr3, pr4) \
    ((GLUE_CHK_PR3(prDev, pr2, pr3) && pr4) ? TRUE : FALSE)

#define GLUE_SET_EVENT(pr) \
	kalSetEvent(pr)

#define GLUE_INC_REF_CNT(_refCount)     atomic_inc((atomic_t *)&(_refCount))
#define GLUE_DEC_REF_CNT(_refCount)     atomic_dec((atomic_t *)&(_refCount))


#define DbgPrint(...)
/*                                                                              
                                                            
                                                                                
*/
#ifdef WLAN_INCLUDE_PROC
INT_32 procRemoveProcfs(struct net_device *prDev, char *pucDevName);

INT_32 procInitProcfs(struct net_device *prDev, char *pucDevName);
#endif				/*                   */

#if CFG_ENABLE_BT_OVER_WIFI
BOOLEAN glRegisterAmpc(P_GLUE_INFO_T prGlueInfo);

BOOLEAN glUnregisterAmpc(P_GLUE_INFO_T prGlueInfo);
#endif

#if CFG_ENABLE_WIFI_DIRECT

VOID wlanSubModRunInit(P_GLUE_INFO_T prGlueInfo);

VOID wlanSubModRunExit(P_GLUE_INFO_T prGlueInfo);

BOOLEAN wlanSubModInit(P_GLUE_INFO_T prGlueInfo);

BOOLEAN wlanSubModExit(P_GLUE_INFO_T prGlueInfo);

VOID
wlanSubModRegisterInitExit(SUB_MODULE_INIT rSubModInit,
			   SUB_MODULE_EXIT rSubModExit, ENUM_SUB_MODULE_IDX_T eSubModIdx);

BOOLEAN wlanExportGlueInfo(P_GLUE_INFO_T *prGlueInfoExpAddr);

BOOLEAN wlanIsLaunched(VOID);

void p2pSetMulticastListWorkQueueWrapper(P_GLUE_INFO_T prGlueInfo);



#endif

/*                                                                              
                                                
                                                                                
*/

#endif				/*          */
