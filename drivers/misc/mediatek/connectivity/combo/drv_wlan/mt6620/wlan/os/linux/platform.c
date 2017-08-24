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
#include <linux/version.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/fs.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 12)
#include <linux/uaccess.h>
#endif

#include "gl_os.h"

#ifndef CONFIG_X86
#if defined(CONFIG_HAS_EARLY_SUSPEND)
#include <linux/earlysuspend.h>
#endif
#endif


#define FIX_ALPS00409409406


#ifdef FIX_ALPS00409409406
extern atomic_t fgIsUnderEarlierSuspend;
#else
extern BOOLEAN fgIsUnderEarlierSuspend;
#endif


/*                                                                              
                                                
                                                                                
*/
#define WIFI_NVRAM_FILE_NAME   "/data/nvram/APCFG/APRDEB/WIFI"
#define WIFI_NVRAM_CUSTOM_NAME "/data/nvram/APCFG/APRDEB/WIFI_CUSTOM"


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



static int netdev_event(struct notifier_block *nb, unsigned long notification, void *ptr)
{
	struct in_device *in_dev;	/*                 */
	UINT_8 ip[4] = { 0 };
	UINT_32 u4NumIPv4 = 0;
/*                     */
#if 0
	UINT_8 ip6[16] = { 0 };	/*                                                 */
	UINT_32 u4NumIPv6 = 0;
#endif
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
	struct net_device *prDev = ifa->ifa_dev->dev;
	UINT_32 i;
	P_PARAM_NETWORK_ADDRESS_IP prParamIpAddr;
	P_GLUE_INFO_T prGlueInfo = NULL;

	if (prDev == NULL) {
		DBGLOG(REQ, INFO, ("netdev_event: device is empty.\n"));
		return NOTIFY_DONE;
	}

	if ((strncmp(prDev->name, "p2p", 3) != 0) && (strncmp(prDev->name, "wlan", 4) != 0)) {
		DBGLOG(REQ, INFO, ("netdev_event: xxx\n"));
		return NOTIFY_DONE;
	}

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));

	if (prGlueInfo == NULL) {
		DBGLOG(REQ, INFO, ("netdev_event: prGlueInfo is empty.\n"));
		return NOTIFY_DONE;
	}
	ASSERT(prGlueInfo);

#ifdef FIX_ALPS00409409406
	/*                          */
	in_dev = in_dev_get(prDev);
	if (!in_dev)
		return;

	/*              */
	if (!in_dev->ifa_list || !in_dev->ifa_list->ifa_local) {
		/*                */
		in_dev_put(in_dev);
		DBGLOG(REQ, INFO, ("ip is not avaliable.\n"));
		return;
	}
	/*                           */
	kalMemCopy(ip, &(in_dev->ifa_list->ifa_local), sizeof(ip));
	/*                */
	in_dev_put(in_dev);

	DBGLOG(REQ, INFO, ("ip is %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]));
#else
	/*                          */
	if (!prDev || !(prDev->ip_ptr) ||
	    !((struct in_device *)(prDev->ip_ptr))->ifa_list ||
	    !(&(((struct in_device *)(prDev->ip_ptr))->ifa_list->ifa_local))) {
		DBGLOG(REQ, INFO, ("ip is not avaliable.\n"));
		return NOTIFY_DONE;
	}

	kalMemCopy(ip, &(((struct in_device *)(prDev->ip_ptr))->ifa_list->ifa_local), sizeof(ip));
	DBGLOG(REQ, INFO, ("ip is %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]));
#endif

	/*                                                                  */
	if (!((ip[0] == 0) && (ip[1] == 0) && (ip[2] == 0) && (ip[3] == 0))) {
		u4NumIPv4++;
	}
#if defined(MTK_WLAN_ARP_OFFLOAD)
	if (NETDEV_UP == notification
	    && PARAM_MEDIA_STATE_CONNECTED == prGlueInfo->eParamMediaStateIndicated) {
		PARAM_CUSTOM_SW_CTRL_STRUC_T SwCtrlInfo;
		UINT_32 u4SetInfoLen;
		WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;

		SwCtrlInfo.u4Id = 0x90110000;
		SwCtrlInfo.u4Data = 1;

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetSwCtrlWrite,
				   (PVOID) & SwCtrlInfo,
				   sizeof(SwCtrlInfo), FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			DBGLOG(REQ, INFO, ("ARP OFFLOAD fail 0x%lx\n", rStatus));
		} else {
			DBGLOG(REQ, INFO, ("ARP OFFLOAD success\n"));
		}
	}
#endif


#ifdef FIX_ALPS00409409406
	if (atomic_read(&fgIsUnderEarlierSuspend) == 0) {
#else
	if (fgIsUnderEarlierSuspend == false) {
#endif
		DBGLOG(REQ, INFO,
		       ("netdev_event: PARAM_MEDIA_STATE_DISCONNECTED. (%d)\n",
			prGlueInfo->eParamMediaStateIndicated));
		return NOTIFY_DONE;
	}
/*                     */
#if 0
	if (!prDev || !(prDev->ip6_ptr) ||
	    !((struct in_device *)(prDev->ip6_ptr))->ifa_list ||
	    !(&(((struct in_device *)(prDev->ip6_ptr))->ifa_list->ifa_local))) {
		printk(KERN_INFO "ipv6 is not avaliable.\n");
		return NOTIFY_DONE;
	}

	kalMemCopy(ip6, &(((struct in_device *)(prDev->ip6_ptr))->ifa_list->ifa_local),
		   sizeof(ip6));
	printk(KERN_INFO "ipv6 is %d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d.%d\n", ip6[0],
	       ip6[1], ip6[2], ip6[3], ip6[4], ip6[5], ip6[6], ip6[7], ip6[8], ip6[9], ip6[10],
	       ip6[11], ip6[12], ip6[13], ip6[14], ip6[15]
	    );

	/*                                                                  */
	if (!((ip6[0] == 0) &&
	      (ip6[1] == 0) && (ip6[2] == 0) && (ip6[3] == 0) && (ip6[4] == 0) && (ip6[5] == 0))) {
		/*              */
	}
#endif

	/*                                                            */
	/*                           */
	/*  */
	/*                                                                                  */
	/*                                                      */

	/*                           */
	{
		WLAN_STATUS rStatus = WLAN_STATUS_FAILURE;
		UINT_32 u4SetInfoLen = 0;
		UINT_8 aucBuf[32] = { 0 };
		UINT_32 u4Len = OFFSET_OF(PARAM_NETWORK_ADDRESS_LIST, arAddress);
		P_PARAM_NETWORK_ADDRESS_LIST prParamNetAddrList =
		    (P_PARAM_NETWORK_ADDRESS_LIST) aucBuf;
		P_PARAM_NETWORK_ADDRESS prParamNetAddr = prParamNetAddrList->arAddress;

/*                     */
#if 0
		prParamNetAddrList->u4AddressCount = u4NumIPv4 + u4NumIPv6;
#else
		prParamNetAddrList->u4AddressCount = u4NumIPv4;
#endif
		prParamNetAddrList->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;
		for (i = 0; i < u4NumIPv4; i++) {
			prParamNetAddr->u2AddressLength = sizeof(PARAM_NETWORK_ADDRESS_IP);	/*     */
			prParamNetAddr->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;
#if 0
			kalMemCopy(prParamNetAddr->aucAddress, ip, sizeof(ip));
			prParamNetAddr =
			    (P_PARAM_NETWORK_ADDRESS) ((UINT_32) prParamNetAddr + sizeof(ip));
			u4Len += OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress) + sizeof(ip);
#else
			prParamIpAddr = (P_PARAM_NETWORK_ADDRESS_IP) prParamNetAddr->aucAddress;
			kalMemCopy(&prParamIpAddr->in_addr, ip, sizeof(ip));
			prParamNetAddr =
			    (P_PARAM_NETWORK_ADDRESS) ((UINT_32) prParamNetAddr +
						       sizeof(PARAM_NETWORK_ADDRESS));
			u4Len +=
			    OFFSET_OF(PARAM_NETWORK_ADDRESS,
				      aucAddress) + sizeof(PARAM_NETWORK_ADDRESS);
#endif
		}
/*                     */
#if 0
		for (i = 0; i < u4NumIPv6; i++) {
			prParamNetAddr->u2AddressLength = 6;
			prParamNetAddr->u2AddressType = PARAM_PROTOCOL_ID_TCP_IP;
			kalMemCopy(prParamNetAddr->aucAddress, ip6, sizeof(ip6));
			prParamNetAddr =
			    (P_PARAM_NETWORK_ADDRESS) ((UINT_32) prParamNetAddr + sizeof(ip6));
			u4Len += OFFSET_OF(PARAM_NETWORK_ADDRESS, aucAddress) + sizeof(ip6);
		}
#endif
		ASSERT(u4Len <= sizeof(aucBuf));

		DBGLOG(REQ, INFO, ("kalIoctl (0x%x, 0x%x)\n", prGlueInfo, prParamNetAddrList));

		rStatus = kalIoctl(prGlueInfo,
				   wlanoidSetNetworkAddress,
				   (PVOID) prParamNetAddrList,
				   u4Len, FALSE, FALSE, TRUE, FALSE, &u4SetInfoLen);

		if (rStatus != WLAN_STATUS_SUCCESS) {
			DBGLOG(REQ, INFO, ("set HW pattern filter fail 0x%lx\n", rStatus));
		}
	}

	return NOTIFY_DONE;

}

static struct notifier_block inetaddr_notifier = {
	.notifier_call = netdev_event,
};

void wlanRegisterNotifier(void)
{
	register_inetaddr_notifier(&inetaddr_notifier);
}

/*                                      */

void wlanUnregisterNotifier(void)
{
	unregister_inetaddr_notifier(&inetaddr_notifier);
}

/*                                        */

#ifndef CONFIG_X86
#if defined(CONFIG_HAS_EARLYSUSPEND)


/*                                                                            */
/* 
                                                          
 
                                                                         
                                                                        
 
                                                
*/
/*                                                                            */

int glRegisterEarlySuspend(struct early_suspend *prDesc,
			   early_suspend_callback wlanSuspend, late_resume_callback wlanResume)
{
	int ret = 0;

	if (NULL != wlanSuspend)
		prDesc->suspend = wlanSuspend;
	else {
		DBGLOG(REQ, INFO, ("glRegisterEarlySuspend wlanSuspend ERROR.\n"));
		ret = -1;
	}

	if (NULL != wlanResume)
		prDesc->resume = wlanResume;
	else {
		DBGLOG(REQ, INFO, ("glRegisterEarlySuspend wlanResume ERROR.\n"));
		ret = -1;
	}

	register_early_suspend(prDesc);
	return ret;
}

/*                                        */

/*                                                                            */
/* 
                                                             
 
                                                   
*/
/*                                                                            */

int glUnregisterEarlySuspend(struct early_suspend *prDesc)
{
	int ret = 0;

	unregister_early_suspend(prDesc);

	prDesc->suspend = NULL;
	prDesc->resume = NULL;

	return ret;
}

/*                                          */
#endif
#endif				/*             */

/*                                                                            */
/* 
                                                                 
 
            
                    
               
                  
             
               
         
                                            
*/
/*                                                                            */
static int nvram_read(char *filename, char *buf, ssize_t len, int offset)
{
#if CFG_SUPPORT_NVRAM
	struct file *fd;
	int retLen = -1;

	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);

	fd = filp_open(filename, O_RDONLY, 0644);

	if (IS_ERR(fd)) {
		DBGLOG(INIT, INFO, ("[MT6620][nvram_read] : failed to open!!\n"));
		return -1;
	}

	do {
		if ((fd->f_op == NULL) || (fd->f_op->read == NULL)) {
			DBGLOG(INIT, INFO, ("[MT6620][nvram_read] : file can not be read!!\n"));
			break;
		}

		if (fd->f_pos != offset) {
			if (fd->f_op->llseek) {
				if (fd->f_op->llseek(fd, offset, 0) != offset) {
					DBGLOG(INIT, INFO,
					       ("[MT6620][nvram_read] : failed to seek!!\n"));
					break;
				}
			} else {
				fd->f_pos = offset;
			}
		}

		retLen = fd->f_op->read(fd, buf, len, &fd->f_pos);

	} while (FALSE);

	filp_close(fd, NULL);

	set_fs(old_fs);

	return retLen;

#else				/*                    */

	return -EIO;

#endif
}


/*                                                                            */
/* 
                                                               
 
            
                    
               
               
                  
         
                                               
*/
/*                                                                            */
static int nvram_write(char *filename, char *buf, ssize_t len, int offset)
{
#if CFG_SUPPORT_NVRAM
	struct file *fd;
	int retLen = -1;

	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);

	fd = filp_open(filename, O_WRONLY | O_CREAT, 0644);

	if (IS_ERR(fd)) {
		DBGLOG(INIT, INFO, ("[MT6620][nvram_write] : failed to open!!\n"));
		return -1;
	}

	do {
		if ((fd->f_op == NULL) || (fd->f_op->write == NULL)) {
			DBGLOG(INIT, INFO, ("[MT6620][nvram_write] : file can not be write!!\n"));
			break;
		}
		/*           */
		if (fd->f_pos != offset) {
			if (fd->f_op->llseek) {
				if (fd->f_op->llseek(fd, offset, 0) != offset) {
					DBGLOG(INIT, INFO,
					       ("[MT6620][nvram_write] : failed to seek!!\n"));
					break;
				}
			} else {
				fd->f_pos = offset;
			}
		}

		retLen = fd->f_op->write(fd, buf, len, &fd->f_pos);

	} while (FALSE);

	filp_close(fd, NULL);

	set_fs(old_fs);

	return retLen;

#else				/*                     */

	return -EIO;

#endif
}


/*                                                                            */
/* 
                                      
 
            
                      
                    
             
                   
         
                
                 
*/
/*                                                                            */
BOOLEAN kalCfgDataRead16(IN P_GLUE_INFO_T prGlueInfo, IN UINT_32 u4Offset, OUT PUINT_16 pu2Data)
{
	if (pu2Data == NULL) {
		return FALSE;
	}

	if (nvram_read(WIFI_NVRAM_FILE_NAME,
		       (char *)pu2Data,
		       sizeof(unsigned short), u4Offset) != sizeof(unsigned short)) {
		return FALSE;
	} else {
		return TRUE;
	}
}


/*                                                                            */
/* 
                                      
 
            
                      
                    
                  
         
                
                 
*/
/*                                                                            */
BOOLEAN kalCfgDataWrite16(IN P_GLUE_INFO_T prGlueInfo, UINT_32 u4Offset, UINT_16 u2Data)
{
	if (nvram_write(WIFI_NVRAM_FILE_NAME,
			(char *)&u2Data,
			sizeof(unsigned short), u4Offset) != sizeof(unsigned short)) {
		return FALSE;
	} else {
		return TRUE;
	}
}