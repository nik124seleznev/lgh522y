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

#if CFG_ENABLE_EARLY_SUSPEND
#include <linux/earlysuspend.h>
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
	struct in_ifaddr *ifa = (struct in_ifaddr *)ptr;
	struct net_device *prDev = ifa->ifa_dev->dev;
	P_GLUE_INFO_T prGlueInfo = NULL;

	if (prDev == NULL) {
		/*                                                          */
		return NOTIFY_DONE;
	}

	if ((strncmp(prDev->name, "p2p", 3) != 0) && (strncmp(prDev->name, "wlan", 4) != 0)) {
		/*                                             */
		return NOTIFY_DONE;
	}
#if 0				/*                       */
	{
		/*                                                                */
		prGlueInfo->fgIsDad = FALSE;
	}
#endif				/*                       */

	prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	if (prGlueInfo == NULL) {
		DBGLOG(REQ, INFO, ("netdev_event: prGlueInfo is empty.\n"));
		return NOTIFY_DONE;
	}

	if (prGlueInfo->fgIsInSuspendMode == FALSE) {
		/*                                                                                                                     */
		return NOTIFY_DONE;
	}

	kalSetNetAddressFromInterface(prGlueInfo, prDev, TRUE);

	return NOTIFY_DONE;

}


#if 0				/*                       */
static int net6dev_event(struct notifier_block *nb, unsigned long notification, void *ptr)
{
	struct inet6_ifaddr *ifa = (struct inet6_ifaddr *)ptr;
	struct net_device *prDev = ifa->idev->dev;
	P_GLUE_INFO_T prGlueInfo = NULL;

	if (prDev == NULL) {
		DBGLOG(REQ, INFO, ("net6dev_event: device is empty.\n"));
		return NOTIFY_DONE;
	}

	if ((strncmp(prDev->name, "p2p", 3) != 0) && (strncmp(prDev->name, "wlan", 4) != 0)) {
		DBGLOG(REQ, INFO, ("net6dev_event: xxx\n"));
		return NOTIFY_DONE;
	}

	if (strncmp(prDev->name, "p2p", 3) == 0) {
		/*                                                                                */
		/*                                                                                     */
		prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
	} else {		/*       */
		prGlueInfo = (P_GLUE_INFO_T) netdev_priv(prDev);
	}

	if (prGlueInfo == NULL) {
		DBGLOG(REQ, INFO, ("netdev_event: prGlueInfo is empty.\n"));
		return NOTIFY_DONE;
	}
	/*                                                                 */
	prGlueInfo->fgIs6Dad = FALSE;

	return NOTIFY_DONE;
}
#endif				/*                       */


static struct notifier_block inetaddr_notifier = {
	.notifier_call = netdev_event,
};

#if 0				/*                       */
static struct notifier_block inet6addr_notifier = {
	.notifier_call = net6dev_event,
};
#endif				/*                       */


void wlanRegisterNotifier(void)
{
	register_inetaddr_notifier(&inetaddr_notifier);
#if 0				/*                       */
	register_inet6addr_notifier(&inet6addr_notifier);
#endif				/*                       */
}

void wlanUnregisterNotifier(void)
{
	unregister_inetaddr_notifier(&inetaddr_notifier);
#if 0				/*                       */
	unregister_inetaddr_notifier(&inet6addr_notifier);
#endif				/*                       */
}

#if CFG_ENABLE_EARLY_SUSPEND
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
#endif

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
		DBGLOG(INIT, INFO, ("[nvram_read] : failed to open!!\n"));
		return -1;
	}

	do {
		if ((fd->f_op == NULL) || (fd->f_op->read == NULL)) {
			DBGLOG(INIT, INFO, ("[nvram_read] : file can not be read!!\n"));
			break;
		}

		if (fd->f_pos != offset) {
			if (fd->f_op->llseek) {
				if (fd->f_op->llseek(fd, offset, 0) != offset) {
					DBGLOG(INIT, INFO, ("[nvram_read] : failed to seek!!\n"));
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
		DBGLOG(INIT, INFO, ("[nvram_write] : failed to open!!\n"));
		return -1;
	}

	do {
		if ((fd->f_op == NULL) || (fd->f_op->write == NULL)) {
			DBGLOG(INIT, INFO, ("[nvram_write] : file can not be write!!\n"));
			break;
		}
		/*           */
		if (fd->f_pos != offset) {
			if (fd->f_op->llseek) {
				if (fd->f_op->llseek(fd, offset, 0) != offset) {
					DBGLOG(INIT, INFO, ("[nvram_write] : failed to seek!!\n"));
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
