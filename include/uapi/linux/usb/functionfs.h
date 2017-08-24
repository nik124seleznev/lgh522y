#ifndef _UAPI__LINUX_FUNCTIONFS_H__
#define _UAPI__LINUX_FUNCTIONFS_H__


#include <linux/types.h>
#include <linux/ioctl.h>

#include <linux/usb/ch9.h>


enum {
	FUNCTIONFS_DESCRIPTORS_MAGIC = 1,
	FUNCTIONFS_STRINGS_MAGIC     = 2
};

#define FUNCTIONFS_SS_DESC_MAGIC 0x0055DE5C

#ifndef __KERNEL__

/*                                     */
struct usb_endpoint_descriptor_no_audio {
	__u8  bLength;
	__u8  bDescriptorType;

	__u8  bEndpointAddress;
	__u8  bmAttributes;
	__le16 wMaxPacketSize;
	__u8  bInterval;
} __attribute__((packed));


/*
                                              
 */

struct usb_functionfs_descs_head {
	__le32 magic;
	__le32 length;
	__le32 fs_count;
	__le32 hs_count;
} __attribute__((packed));

/*
                      
  
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
  
                                                                      
                                                                      
  
                                                              
                                                              
                                                              
                                                              
                                                              
 */

struct usb_functionfs_strings_head {
	__le32 magic;
	__le32 length;
	__le32 str_count;
	__le32 lang_count;
} __attribute__((packed));

/*
                  
  
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
  
                                                                           
                                                            
  
                                                                             
                                                                             
                                                                             
                                                                             
  
                                                                      
                                                                      
         
 */

#endif


/*
                                                                             
                                                                        
                                
 */

enum usb_functionfs_event_type {
	FUNCTIONFS_BIND,
	FUNCTIONFS_UNBIND,

	FUNCTIONFS_ENABLE,
	FUNCTIONFS_DISABLE,

	FUNCTIONFS_SETUP,

	FUNCTIONFS_SUSPEND,
	FUNCTIONFS_RESUME
};

/*                                                            
                                  
 */
struct usb_functionfs_event {
	union {
		/*                                                  
                                                       */
		struct usb_ctrlrequest	setup;
	} __attribute__((packed)) u;

	/*                                */
	__u8				type;
	__u8				_pad[3];
} __attribute__((packed));


/*                 */
/*                         */

/*                                                              
                                                           
                                                                 
                                                    
                                                                   
                                                                   
 */
#define	FUNCTIONFS_FIFO_STATUS	_IO('g', 1)

/*                                          */
#define	FUNCTIONFS_FIFO_FLUSH	_IO('g', 2)

/*                                                              
                                                  
 */
#define	FUNCTIONFS_CLEAR_HALT	_IO('g', 3)

/*                         */

/*
                                                                     
                                                                 
                   
 */
#define	FUNCTIONFS_INTERFACE_REVMAP	_IO('g', 128)

/*
                                                                    
                          
 */
#define	FUNCTIONFS_ENDPOINT_REVMAP	_IO('g', 129)



#endif /*                             */
