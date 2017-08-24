/*
 * f_acm.c -- USB CDC serial (ACM) function driver
 *
 * Copyright (C) 2003 Al Borchers (alborchers@steinerpoint.com)
 * Copyright (C) 2008 by David Brownell
 * Copyright (C) 2008 by Nokia Corporation
 * Copyright (C) 2009 by Samsung Electronics
 * Author: Michal Nazarewicz (m.nazarewicz@samsung.com)
 *
 * This software is distributed under the terms of the GNU General
 * Public License ("GPL") as published by the Free Software Foundation,
 * either version 2 of that License or (at your option) any later version.
 */

/*                       */

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/device.h>
/*                   */
/*                                        */
#include <linux/musb/musb_core.h>
#include <linux/musb/musb_gadget.h>
#include <linux/musb/mtk_musb.h>
#include <linux/usb/cdc.h>
#include <linux/console.h>
#include <linux/tty.h>
#include <linux/vmalloc.h>
#include <linux/kthread.h>
#include <linux/delay.h>
//                                   

#define CONSOLE_BUF (128*1024)
/*
                           
                      
                      
                      
                       
                      
                      
*/
extern char *usb_buf;
extern volatile u32 buf_len;
extern u8 data_ep_num;
extern char *usb_buf_readp;

extern struct usb_ep *acm_in_ep;
extern volatile bool cdc_set_contr;
//                                   
//                                                                
extern bool gadget_is_ready;
extern struct musb *mtk_musb;

//                              

//                                                      

extern u32 send_data(u32 ep_num,u8 *pbuffer,u32 data_len);
extern void format_and_send_string(const char *s,unsigned int count);
extern struct tty_driver *usb_console_device(struct console *co, int *index);

#define MUSB_IECSR (0x0010)
#ifdef CONFIG_OF
#define usb_readb(x)		__raw_readb(((unsigned long)mtk_musb->mregs)+(x))
#define usb_readw(x)		__raw_readw(((unsigned long)mtk_musb->mregs)+(x))
#define usb_readl(x)		__raw_readl(((unsigned long)mtk_musb->mregs)+(x))
#define usb_writeb(x,y)		__raw_writeb(x,(((unsigned long)mtk_musb->mregs)+(y)))
#define usb_writew(x,y)		__raw_writew(x,(((unsigned long)mtk_musb->mregs)+(y)))
#define usb_writel(x,y)		__raw_writel(x,(((unsigned long)mtk_musb->mregs)+(y)))
#else
#define usb_readb(x)		__raw_readb(USB_BASE+(x))
#define usb_readw(x)		__raw_readw(USB_BASE+(x))
#define usb_readl(x)		__raw_readl(USB_BASE+(x))
#define usb_writeb(x,y)		__raw_writeb(x,(USB_BASE+(y)))
#define usb_writew(x,y)		__raw_writew(x,(USB_BASE+(y)))
#define usb_writel(x,y)		__raw_writel(x,(USB_BASE+(y)))
#endif

#define NOTIFY_EP 7
#define DATA_EP 8
#define BULK_P_SIZE 512

static u8 ep0_max_size = 64;
static u32 data_ep_max_size = 512;
u8 data_ep_num = DATA_EP;
u8 notify_ep_num = NOTIFY_EP;
char *usb_buf = NULL;
char *usb_buf_readp = NULL;

volatile u32 buf_len = 0;

struct usb_ep *acm_in_ep = NULL;

/*
                                                                 
                                                         
  
                                                                     
                                                                  
                                                                  
                                                                   
                                  
  
                                                                     
                                                                     
                                                                       
                                                                   
                                                                     
 */
static bool com_opend = false;		//                              
//                                                             
volatile bool cdc_set_contr = false;

//                                  
//                                      

bool gadget_is_ready = false;
static char *usb_hs_config_desc = NULL;
static char *usb_fs_config_desc = NULL;
bool is_high_speed = false;
static struct usb_cdc_line_coding cdc_line =
{
	0x000E1000,
	0,
	0,
	0x8,
};

struct acm_ep_descs {
	struct usb_endpoint_descriptor	*in;
	struct usb_endpoint_descriptor	*out;
	struct usb_endpoint_descriptor	*notify;
};

/*                                                                        */

#define GS_LOG2_NOTIFY_INTERVAL	5	/*                   */
#define GS_NOTIFY_MAXPACKET	10	/*                        */

/*                 */

#define USBCOM_DEVICE_CLASS	0x02
#define USBCOM_DEVICE_SUBCLASS	0x00
#define USBCOM_DEVICE_PROTOCOL	0x00
#define COM_VENDOR_ID		0x0E8D
#define COM_PRO_ID		0x2000

static struct usb_device_descriptor device_desc =
{
	.bLength		= sizeof(device_desc),
	.bDescriptorType	= USB_DT_DEVICE,
	.bcdUSB			= __constant_cpu_to_le16(0x0200),
	.bDeviceClass		= USB_CLASS_MISC,	//               
	.bDeviceSubClass	= 2,	//                       
	.bDeviceProtocol	= 1,	//                       
	.bMaxPacketSize0	= 64,
	.idVendor		= __constant_cpu_to_le16(COM_VENDOR_ID),
	.idProduct		= __constant_cpu_to_le16(COM_PRO_ID),
	.bcdDevice		= __constant_cpu_to_le16(0xffff),
	.iManufacturer		= 0,
	.iProduct		= 0,
	.iSerialNumber		= 0,
	.bNumConfigurations	= 1,
};

/*
                                                 
                       
                           
             
             
              
  
*/

static struct usb_config_descriptor config_desc = {

	sizeof (config_desc),
	USB_DT_CONFIG,
	(sizeof (struct usb_config_descriptor)) +
	(sizeof (struct usb_interface_descriptor) * 2) +
	(sizeof (struct usb_interface_assoc_descriptor)) +
	(sizeof (struct usb_cdc_header_desc)) +
	(sizeof (struct usb_cdc_call_mgmt_descriptor)) +
	(sizeof (struct usb_cdc_acm_descriptor)) +
	(sizeof (struct usb_endpoint_descriptor) * 3) +
	(sizeof(struct usb_cdc_union_desc)),
	2,
	1,
	0,
	0xc0,
	0xFA,
};


/*                                  */
static struct usb_interface_assoc_descriptor
	acm_iad_descriptor = {
	.bLength =		sizeof acm_iad_descriptor,
	.bDescriptorType =	USB_DT_INTERFACE_ASSOCIATION,
	.bFirstInterface =	0,
	/*                             */
	.bInterfaceCount =	2,	//               
	.bFunctionClass =	USB_CLASS_COMM,
	.bFunctionSubClass =	USB_CDC_SUBCLASS_ACM,
	.bFunctionProtocol =	USB_CDC_PROTO_NONE,
	/*                       */
};


static struct usb_interface_descriptor acm_control_interface_desc = {
	.bLength =		USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bInterfaceNumber =	0,
	/*                             */
	.bNumEndpoints =	1,
	.bInterfaceClass =	USB_CLASS_COMM,
	.bInterfaceSubClass =	USB_CDC_SUBCLASS_ACM,
	.bInterfaceProtocol =	USB_CDC_ACM_PROTO_AT_V25TER,
	/*                       */
};

static struct usb_interface_descriptor acm_data_interface_desc = {
	.bLength =		USB_DT_INTERFACE_SIZE,
	.bDescriptorType =	USB_DT_INTERFACE,
	.bInterfaceNumber =	1,
	/*                             */
	.bNumEndpoints =	2,
	.bInterfaceClass =	USB_CLASS_CDC_DATA,
	.bInterfaceSubClass =	0,
	.bInterfaceProtocol =	0,
	/*                       */
};

static struct usb_cdc_header_desc acm_header_desc = {
	.bLength =		sizeof(acm_header_desc),
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_HEADER_TYPE,
	.bcdCDC =		cpu_to_le16(0x0110),
};

static struct usb_cdc_call_mgmt_descriptor
	acm_call_mgmt_descriptor = {
	.bLength =		sizeof(acm_call_mgmt_descriptor),
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_CALL_MANAGEMENT_TYPE,
	.bmCapabilities =	0,
	.bDataInterface =	1,
};

static struct usb_cdc_acm_descriptor acm_descriptor = {
	.bLength =		sizeof(acm_descriptor),
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_ACM_TYPE,
	.bmCapabilities =	USB_CDC_CAP_LINE,
};

static struct usb_cdc_union_desc acm_union_desc = {
	.bLength =		sizeof(acm_union_desc),
	.bDescriptorType =	USB_DT_CS_INTERFACE,
	.bDescriptorSubType =	USB_CDC_UNION_TYPE,
	.bMasterInterface0 =	0,
	.bSlaveInterface0 =	1,
};


/*                                        */
/*                     */

static struct usb_endpoint_descriptor acm_fs_notify_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_IN|NOTIFY_EP,
	.bmAttributes =		USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize =	cpu_to_le16(GS_NOTIFY_MAXPACKET),
	.bInterval =		1 << GS_LOG2_NOTIFY_INTERVAL,
};

static struct usb_endpoint_descriptor acm_fs_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_IN|DATA_EP,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(64),
};

static struct usb_endpoint_descriptor acm_fs_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_OUT|DATA_EP,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(64),
};

static struct usb_descriptor_header *acm_fs_function[] = {
	(struct usb_descriptor_header *) &acm_iad_descriptor,
	(struct usb_descriptor_header *) &acm_control_interface_desc,
	(struct usb_descriptor_header *) &acm_header_desc,
	(struct usb_descriptor_header *) &acm_call_mgmt_descriptor,
	(struct usb_descriptor_header *) &acm_descriptor,
	(struct usb_descriptor_header *) &acm_union_desc,
	(struct usb_descriptor_header *) &acm_fs_notify_desc,
	(struct usb_descriptor_header *) &acm_data_interface_desc,
	(struct usb_descriptor_header *) &acm_fs_in_desc,
	(struct usb_descriptor_header *) &acm_fs_out_desc,
	NULL,
};

/*                     */

static struct usb_endpoint_descriptor acm_hs_notify_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_IN|NOTIFY_EP,
	.bmAttributes =		USB_ENDPOINT_XFER_INT,
	.wMaxPacketSize =	cpu_to_le16(GS_NOTIFY_MAXPACKET),
	.bInterval =		GS_LOG2_NOTIFY_INTERVAL+4,
};

static struct usb_endpoint_descriptor acm_hs_in_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_IN|DATA_EP,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(512),
};

static struct usb_endpoint_descriptor acm_hs_out_desc = {
	.bLength =		USB_DT_ENDPOINT_SIZE,
	.bDescriptorType =	USB_DT_ENDPOINT,
	.bEndpointAddress =	USB_DIR_OUT|DATA_EP,
	.bmAttributes =		USB_ENDPOINT_XFER_BULK,
	.wMaxPacketSize =	cpu_to_le16(512),
};

static struct usb_descriptor_header *acm_hs_function[] = {
	(struct usb_descriptor_header *) &acm_iad_descriptor,
	(struct usb_descriptor_header *) &acm_control_interface_desc,
	(struct usb_descriptor_header *) &acm_header_desc,
	(struct usb_descriptor_header *) &acm_call_mgmt_descriptor,
	(struct usb_descriptor_header *) &acm_descriptor,
	(struct usb_descriptor_header *) &acm_union_desc,
	(struct usb_descriptor_header *) &acm_hs_notify_desc,
	(struct usb_descriptor_header *) &acm_data_interface_desc,
	(struct usb_descriptor_header *) &acm_hs_in_desc,
	(struct usb_descriptor_header *) &acm_hs_out_desc,
	NULL,
};

/*                     */

//                      
//                      
//                     

/*                          */
/*
                                              
                                                       
                                   
                                 
    *//*                 *//*   
  
*/

/*
                                                     
                     */	/*       */
/*                             
  
*/

/*
                                                   
                   
      
  
*/

/*
             
 */
static void usb_write_fifo(u16 len, const u8 *src, u8 ep_num)
{
	void __iomem *fifo = (void __iomem *)MUSB_FIFO_OFFSET(ep_num);
	if (likely((0x01 & (unsigned long) src) == 0)) {
		u16 index = 0;

		/*                                           */
		if ((0x02 & (unsigned long) src) == 0) {
			if (len >= 4) {
				writesl(USB_BASE_TEMP+fifo, src + index, len >> 2);
				index += len & ~0x03;
			}
			if (len & 0x02) {
				usb_writew(*(u16 *)&src[index],fifo + 0 );
				index += 2;
			}
		} else {
			if (len >= 2) {
				writesw(USB_BASE_TEMP+fifo, src + index, len >> 1);
				index += len & ~0x01;
			}
		}
		if (len & 0x01)
			usb_writeb(src[index],fifo + 0);
	} else  {
		/*              */
		writesb(USB_BASE_TEMP+fifo, src, len);
	}
}

/*
                            
 */
static void usb_read_fifo(u16 len, u8 *dst,u8 ep_num)
{
	void __iomem *fifo = (void __iomem *)MUSB_FIFO_OFFSET(ep_num);

	/*                                       */
	if (likely((0x01 & (unsigned long) dst) == 0)) {
		u16 index = 0;

		/*                                                */
		if ((0x02 & (unsigned long) dst) == 0) {
			if (len >= 4) {
				readsl(USB_BASE_TEMP+fifo, dst, len >> 2);
				index = len & ~0x03;
			}
			if (len & 0x02) {
				*(u16 *)&dst[index] = usb_readw(fifo+0);
				index += 2;
			}
		} else {
			if (len >= 2) {
				readsw(USB_BASE_TEMP+fifo, dst, len >> 1);
				index = len & ~0x01;
			}
		}
		if (len & 0x01)
			dst[index] = usb_readb(fifo+0);
	} else {
		/*              */
		readsb(USB_BASE_TEMP+fifo, dst, len);
	}

}

/*
                      
                                                          
 */
static void copy_config_desc(char *buf,bool high_speed)
{
	struct usb_descriptor_header **acm_desc = NULL;
	if(buf == NULL)
		return;
	if(high_speed)
		acm_desc = acm_hs_function;
	else
		acm_desc = acm_fs_function;
	DBG(0,"start copy config descriptor\n");
	memcpy(buf,(char*)&config_desc,sizeof(config_desc));
	buf = buf + sizeof(config_desc);
	DBG(0,"start copy interface desciptore\n");
	while(*acm_desc != NULL)
	{
		DBG(0,"start copy interface desciptore %d\n",(*acm_desc)->bLength);
		memcpy(buf,(char*)(*acm_desc),(*acm_desc)->bLength);
		buf += (*acm_desc)->bLength;
		acm_desc++;
	}
}



/*
                              
                                                                          
 */
static void recv_data(u32 ep_num,u8 *pbuffer,u16 data_len)
{

	u16 rxcsr = 0;
	u16 len = 0;
	u16 count = 0;
	if(!pbuffer) {
		return;
	}
	usb_writeb(ep_num,MUSB_INDEX);
	while(count < data_len) {


		if(ep_num != 0)
		{
			//                          
			rxcsr = usb_readw(MUSB_IECSR + MUSB_RXCSR);
			if((rxcsr & MUSB_RXCSR_RXPKTRDY)==0 ) {
				continue;
			}
			len = min(usb_readw(MUSB_IECSR+MUSB_RXCOUNT),data_len);
			count += len;
			usb_read_fifo(len,pbuffer,ep_num);
			rxcsr = usb_readw(MUSB_IECSR + MUSB_RXCSR);
			rxcsr &=(~MUSB_RXCSR_RXPKTRDY);
			usb_writew(rxcsr,MUSB_IECSR+MUSB_RXCSR);	//                      
			while(usb_readw(MUSB_IECSR+MUSB_RXCSR)&MUSB_RXCSR_RXPKTRDY)
				;	//                       

		} else {
			//                          
			rxcsr = usb_readw(MUSB_IECSR + MUSB_CSR0);
			if((rxcsr & MUSB_CSR0_RXPKTRDY)==0 ) {
				continue;
			}
			len = min(usb_readw(MUSB_IECSR+MUSB_COUNT0),data_len);
			count += len;
			usb_read_fifo(len,pbuffer,ep_num);
			if(count == data_len)
				rxcsr |= MUSB_CSR0_P_DATAEND;	//                                     
			rxcsr |= MUSB_CSR0_P_SVDRXPKTRDY;	//                                      
			usb_writew(rxcsr,MUSB_IECSR + MUSB_CSR0);
			while(usb_readw(MUSB_IECSR+MUSB_CSR0)&MUSB_CSR0_RXPKTRDY)
				;	//                       

		}

	}

	return;
}
#define IS_DISCONNECT (usb_readb(MUSB_INTRUSB)&MUSB_INTR_DISCONNECT)
#define IS_SUSPEND (usb_readb(MUSB_INTRUSB)&MUSB_INTR_SUSPEND)
#define IS_VBUS_VALID (usb_readb(MUSB_DEVCTL)& MUSB_DEVCTL_VBUS)
/*
                       
                  
                  
                  
                  
                  
                  
*/
static int time_out = 50;

/*                                  */
u32 send_data(u32 ep_num,u8 *pbuffer,u32 data_len)
{
	u16 csr;
	u32 count = 0;
	u32 write_bytes = 0;
	int i = 0;
//                   

	bool send_zero = false;
	usb_writeb(ep_num,MUSB_INDEX);
	if(ep_num == 0)
	{
		csr = usb_readw(MUSB_IECSR + MUSB_CSR0);

		while(count < data_len)
		{
			if((data_len - count) < ep0_max_size)
			{
				write_bytes = data_len - count;
			} else {
				write_bytes = ep0_max_size;
			}
			usb_write_fifo(write_bytes,pbuffer + count,ep_num);
			count = count + write_bytes;
			if(count == data_len)
				csr |= MUSB_CSR0_P_DATAEND;	//                                     
			csr |= MUSB_CSR0_TXPKTRDY;	//                                      
			usb_writew(csr,MUSB_IECSR + MUSB_CSR0);
			while(usb_readw(MUSB_IECSR+MUSB_CSR0)&MUSB_CSR0_TXPKTRDY)
				;	//                       
		}
	} else {
		csr = usb_readw(MUSB_IECSR + MUSB_TXCSR);
		if((count%data_ep_max_size) == 0)
			send_zero = true;
		while(count < data_len)
		{
			i=0;
			while((usb_readw(MUSB_IECSR+MUSB_TXCSR)&MUSB_TXCSR_TXPKTRDY))
				;	//                                                                              
			if(i >= time_out ||IS_DISCONNECT || IS_SUSPEND)
			{
				printk ("count = 0x%x, IS_DISCONNECT=%d, IS_SUSPEND=%d \n", count,IS_DISCONNECT, IS_SUSPEND);
				//                                                       
				com_opend = false;
				cdc_set_contr = false;
				/*
                      
                                      
                                       
                                         
    */
				//                                                                                                          
				//                                                       
			}

			if((data_len - count) < data_ep_max_size)
				write_bytes = data_len - count;
			else
				write_bytes = data_ep_max_size;

			usb_write_fifo(write_bytes,pbuffer + count,ep_num);

			count = count + write_bytes;
			csr |= MUSB_TXCSR_TXPKTRDY;	//                                      
			usb_writew(csr,MUSB_IECSR + MUSB_TXCSR);
			i = 0;
			while((usb_readw(MUSB_IECSR+MUSB_TXCSR)&MUSB_TXCSR_TXPKTRDY))
				;
			if(i >= time_out ||IS_DISCONNECT || IS_SUSPEND)
			{
				usb_writew(MUSB_RXCSR_FLUSHFIFO,MUSB_IECSR+MUSB_RXCSR);
				com_opend = false;
				cdc_set_contr = false;
				/*
                      
                                       
                                        
                                          
    */
				return count;
			}

		}
#if 0
		if(send_zero)
		{
			i = 0;
			csr = usb_readw(MUSB_IECSR + MUSB_TXCSR);
			csr |= MUSB_TXCSR_TXPKTRDY;	//                                      
			usb_writew(csr,MUSB_IECSR + MUSB_TXCSR);
			while((usb_readw(MUSB_IECSR+MUSB_TXCSR)&MUSB_TXCSR_TXPKTRDY)) ;
			if(i >= time_out ||IS_DISCONNECT || IS_SUSPEND)
			{
				usb_writew(MUSB_RXCSR_FLUSHFIFO,MUSB_IECSR+MUSB_RXCSR);
				com_opend = false;
				cdc_set_contr = false;
				/*
                      
                                      
                                       
                                         
    */
				return count;
			}
		}
#endif
		usb_writew(1<<data_ep_num,MUSB_INTRTX);	//                                             

	}
	return count;
}

static void ep0_hand_shake(void)
{
	u32 csr = 0;
	usb_writeb(0,MUSB_INDEX);
	csr = usb_readw(MUSB_IECSR+MUSB_CSR0);
	csr &= (~MUSB_CSR0_P_SENTSTALL);
	csr |= (MUSB_CSR0_P_DATAEND| MUSB_CSR0_P_SVDRXPKTRDY);
	usb_writew(csr,MUSB_IECSR + MUSB_CSR0);
}

static bool send_descriptor(struct usb_ctrlrequest* pudr)
{
	u8 *data_tx = NULL;
	u16 wLength = 0;
	u16 wtype = pudr->wValue;
	bool fRet = TRUE;
	u16 csr0;
	DBG(0,"get descriptor %x\n",wtype);
	switch (wtype >> 8) {
	case USB_DT_DEVICE:
		data_tx = (u8 *)&device_desc;
		wLength = sizeof(device_desc);
		break;
	case USB_DT_CONFIG:
		if(is_high_speed)
			data_tx = usb_hs_config_desc;
		else
			data_tx = usb_fs_config_desc;
		wLength = min(pudr->wLength, config_desc.wTotalLength);
		break;
	case USB_DT_STRING:
		fRet = FALSE;
		break;
	default:
		fRet = FALSE;
		break;
	}

	usb_writeb(0,MUSB_INDEX);
	//              
	csr0 = usb_readw(MUSB_IECSR+MUSB_CSR0);
	csr0 &= (~MUSB_CSR0_P_SENTSTALL);
	if(csr0 & MUSB_CSR0_RXPKTRDY)
	{
		csr0 |= MUSB_CSR0_P_SVDRXPKTRDY;
	}
	usb_writew(csr0,MUSB_IECSR+MUSB_CSR0);	//                                                      
	while(usb_readw(MUSB_IECSR+MUSB_CSR0)&MUSB_CSR0_RXPKTRDY)
		;
	if (fRet) {
		if(data_tx !=NULL) {

			send_data(0,data_tx,wLength);
		} else {
		}
	}
	return TRUE;
}
static void process_standard_request(struct usb_ctrlrequest* pudr)
{

	switch(pudr->bRequest)   {
	case USB_REQ_GET_STATUS:

		if (pudr->bRequestType == 0x80) {
			u8 device_status[2]={0,0};	//                                       
			send_data(0,device_status,2);
		} else if (pudr->bRequestType == 0xC0) {
			//                                                         
		} else if (pudr->bRequestType == 0x00) {
			//                                                         
		}

		break;

	case USB_REQ_CLEAR_FEATURE:

		break;

	case USB_REQ_SET_FEATURE:
		break;

	case USB_REQ_SET_ADDRESS:

		ep0_hand_shake();
		usb_writeb(0,MUSB_INDEX);
		while((usb_readw(MUSB_INTRTX)&0x1)==0)
			;
		usb_writeb((u8)pudr->wValue,MUSB_FADDR);

		break;

	case USB_REQ_GET_DESCRIPTOR:

		send_descriptor(pudr);
		break;

	case USB_REQ_SET_DESCRIPTOR:

		break;

	case USB_REQ_GET_CONFIGURATION:

		break;

	case USB_REQ_SET_CONFIGURATION:
		ep0_hand_shake();
		break;

	case USB_REQ_GET_INTERFACE:

		break;

	case USB_REQ_SET_INTERFACE:

		break;
	case USB_TYPE_CLASS:
		break;
	default:
		;
	}
}
static void process_class_request(struct usb_ctrlrequest* request)
{


	switch (request->bRequest)
	{
	case USB_CDC_REQ_SET_LINE_CODING:
		recv_data(0,(u8 *)&cdc_line,sizeof (struct usb_cdc_line_coding));
		break;
	case USB_CDC_REQ_GET_LINE_CODING:
		send_data(0,(u8 *)&cdc_line,sizeof (struct usb_cdc_line_coding));
		if(cdc_set_contr == true)
			com_opend = true;
		break;
	case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
		ep0_hand_shake();
		if(request->wValue & 1)
			cdc_set_contr = true;	//                                                                                         

		break;
	case USB_CDC_REQ_SEND_BREAK:	/*            */
		ep0_hand_shake();
		break;
	default:
		return;
	}
	return;
}


static void process_setup_packet(struct usb_ctrlrequest* pudr)
{

	if (((pudr->bRequestType) & USB_TYPE_MASK) == USB_TYPE_STANDARD) {
		process_standard_request(pudr);
	} else if(((pudr->bRequestType) & USB_TYPE_MASK) == USB_TYPE_CLASS) {
		process_class_request(pudr);
	}
}

static void usb_reset(void)
{
	u16 swrst = 0;

	//             
	usb_writeb(DATA_EP,MUSB_INDEX);

	usb_writew(BULK_P_SIZE,MUSB_IECSR + MUSB_RXMAXP);
	usb_writeb(6,MUSB_RXFIFOSZ);	//            
	usb_writew(0x8,MUSB_RXFIFOADD);	//                                                                         

	usb_writew(BULK_P_SIZE,MUSB_IECSR + MUSB_TXMAXP);
	usb_writeb(6,MUSB_TXFIFOSZ);	//            
	usb_writew(72,MUSB_TXFIFOADD);

	//          
	usb_writeb(0,MUSB_INDEX);
	usb_writew(MUSB_CSR0_FLUSHFIFO | MUSB_CSR0_P_WZC_BITS,MUSB_IECSR+MUSB_CSR0);	//               

	usb_writeb(NOTIFY_EP,MUSB_INDEX);

	usb_writew(64,MUSB_IECSR + MUSB_TXMAXP);
	usb_writeb(3,MUSB_TXFIFOSZ);	//            
	usb_writew(144,MUSB_TXFIFOADD);

	usb_writew(MUSB_TXCSR_FLUSHFIFO | MUSB_TXCSR_CLRDATATOG,MUSB_IECSR+MUSB_TXCSR);	//                     

	usb_writeb(DATA_EP,MUSB_INDEX);
	usb_writew(MUSB_RXCSR_FLUSHFIFO | MUSB_RXCSR_CLRDATATOG,MUSB_IECSR+MUSB_RXCSR);	//                      
	usb_writew(MUSB_TXCSR_FLUSHFIFO | MUSB_TXCSR_CLRDATATOG,MUSB_IECSR+MUSB_TXCSR);	//                      


	swrst = usb_readw(MUSB_SWRST);
	swrst |= (MUSB_SWRST_DISUSBRESET | MUSB_SWRST_SWRST);
	usb_writew(swrst, MUSB_SWRST);

	if(usb_readb(MUSB_POWER) & MUSB_POWER_HSMODE)
	{
		is_high_speed = true;
		data_ep_max_size = 64;
	} else {
		is_high_speed = false;
		data_ep_max_size = 512;
	}

	//                                                                         
	printk("usb is hight speed  %d %x\n",is_high_speed,usb_readb(MUSB_POWER));
}

static void config_usb_acm(void)
{

	u8 power = 0;
	int i = 0;
	u8 usb_comm_int = 0;
	u16 usb_tx_int = 0;

	u8 ep0_fifo;
	struct usb_ctrlrequest udr;
	usb_phy_recover();	//         
	//                   
	usb_writew(0,MUSB_INTRRXE);
	usb_writew(0,MUSB_INTRTXE);
	usb_writeb(0,MUSB_INTRUSBE);	//                      

	//                      

	power = usb_readb(MUSB_POWER);
	power |= MUSB_POWER_SOFTCONN | MUSB_POWER_HSENAB | MUSB_POWER_ENSUSPEND;
	usb_writeb(power, MUSB_POWER);
	while(1) {
		i++;
		mdelay(5);
		usb_comm_int = usb_readb(MUSB_INTRUSB);
		usb_writeb(usb_comm_int,MUSB_INTRUSB);
		if(usb_comm_int & MUSB_INTR_RESET) {
			usb_reset();
			break;
		}
	}

	i = 0;
	while(1) {
		u16 csr0 = 0;
		usb_tx_int = usb_readw(MUSB_INTRTX);
		usb_writew(usb_tx_int,MUSB_INTRTX);
		if((usb_tx_int & 0x1) != 0)	//                        
		{
			usb_writeb(0,MUSB_INDEX);
			csr0 = usb_readw(MUSB_IECSR+MUSB_CSR0);
			if(csr0 & MUSB_CSR0_RXPKTRDY) {
				//                                   
				ep0_fifo = usb_readb(MUSB_IECSR + MUSB_COUNT0);
				if(ep0_fifo != 8) {
					DBG(0,"serial::Error,setup Packet size!=8 =%d\r\n",ep0_fifo);
				}
				usb_read_fifo(8,(u8 *)&udr,0);
				process_setup_packet(&udr);
			}
		}
		if(com_opend== TRUE) {
			break;
		}
	}

}
/*                                                                         */
void format_and_send_string(const char *s,unsigned int count)
{
	int real_send_count =0;
	int si =0;
	char* buf_start = (char *)s;
	char* buf_end = (char *)s + count;
	for(si=0; si<count; si++)
	{
		//                                                                                                                         
		//                                   
		real_send_count++;
		if(buf_start[si] == '\n')
		{
			send_data(data_ep_num,buf_start,real_send_count);
			send_data(data_ep_num,"\r",1);
			buf_start = buf_start + si +1;
			real_send_count = 0;
		}
	}
	if(buf_start != NULL)	//                                                           
		send_data(data_ep_num,buf_start,buf_end-buf_start);
}

/*
                                                                              
         

                     
                                      

                         

                       
                        
                   
  
                
   
                      
                                                       
       
                          
                                   
                                                                                                            
                         
                    
                
                                   
          
             
                        
                                                                                   
                            
    

                        
                             
           
    

                              
                                          
              
                    
     
                                           
               
     
    
                              
                                                    
    
   
  
                          
 
*/

/*                                                                           */
static int usb_console_setup(struct console *co, char *options)
{
	usb_hs_config_desc = kmalloc(config_desc.wTotalLength,GFP_KERNEL);
	usb_fs_config_desc = kmalloc(config_desc.wTotalLength,GFP_KERNEL);
	if(usb_hs_config_desc == NULL || usb_fs_config_desc == NULL)
		return -1;
	copy_config_desc(usb_hs_config_desc,true);
	copy_config_desc(usb_fs_config_desc,false);
	return 0;	//                                      
}


/*
                                   
 
                  
*/
/*                                */
/*
                            
                             
                 
                           
              
               
  
*/


int usb_init(void)
{
//                                 
	config_usb_acm ();
	return 0;
}
unsigned int Serial_Recv (unsigned char *pData, unsigned int size, unsigned int timeout)
{
	recv_data (data_ep_num, pData, size);
	return size;
}
unsigned int Serial_Send (unsigned char *pData, unsigned int size)
{

	return send_data (data_ep_num, pData, size);
}
void USBDumpInit (void)
{
	usb_console_setup (NULL, NULL);
}
int usb_get_max_send_size (void)
{
	return 64*1024;
}

EXPORT_SYMBOL (usb_init);
EXPORT_SYMBOL (Serial_Recv);
EXPORT_SYMBOL (Serial_Send);
EXPORT_SYMBOL (USBDumpInit);
EXPORT_SYMBOL (usb_get_max_send_size);
