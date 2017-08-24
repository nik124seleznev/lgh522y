/* Driver for Freecom USB/IDE adaptor
 *
 * Freecom v0.1:
 *
 * First release
 *
 * Current development and maintenance by:
 *   (C) 2000 David Brown <usb-storage@davidb.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * This driver was developed with information provided in FREECOM's USB
 * Programmers Reference Guide.  For further information contact Freecom
 * (http://www.freecom.de/)
 */

#include <linux/module.h>
#include <scsi/scsi.h>
#include <scsi/scsi_cmnd.h>

#include "usb.h"
#include "transport.h"
#include "protocol.h"
#include "debug.h"

MODULE_DESCRIPTION("Driver for Freecom USB/IDE adaptor");
MODULE_AUTHOR("David Brown <usb-storage@davidb.org>");
MODULE_LICENSE("GPL");

#ifdef CONFIG_USB_STORAGE_DEBUG
static void pdump(struct us_data *us, void *ibuffer, int length);
#endif

/*                   */
#define ERR_STAT		0x01
#define DRQ_STAT		0x08

/*                                                */
struct freecom_cb_wrap {
	u8    Type;		/*               */
	u8    Timeout;		/*                     */
	u8    Atapi[12];	/*                  */
	u8    Filler[50];	/*               */
};

struct freecom_xfer_wrap {
	u8    Type;		/*               */
	u8    Timeout;		/*                     */
	__le32   Count;		/*                              */
	u8    Pad[58];
} __attribute__ ((packed));

struct freecom_ide_out {
	u8    Type;		/*                      */
	u8    Pad;
	__le16   Value;		/*                 */
	u8    Pad2[60];
};

struct freecom_ide_in {
	u8    Type;		/*                      */
	u8    Pad[63];
};

struct freecom_status {
	u8    Status;
	u8    Reason;
	__le16   Count;
	u8    Pad[60];
};

/*                                                                     
             */
#define FCM_INT_STATUS		0x02 /*            */
#define FCM_STATUS_BUSY		0x80

/*                                                                     
                                 */
#define FCM_PACKET_ATAPI	0x21
#define FCM_PACKET_STATUS	0x20

/*                                                                   
                                                        */
#define FCM_PACKET_INPUT	0x81

/*                                 */
#define FCM_PACKET_OUTPUT	0x01

/*                                                                      
                              */
#define FCM_PACKET_IDE_WRITE	0x40
#define FCM_PACKET_IDE_READ	0xC0

/*                                                    */
#define FCM_PACKET_LENGTH		64
#define FCM_STATUS_PACKET_LENGTH	4

static int init_freecom(struct us_data *us);


/*
                       
 */
#define UNUSUAL_DEV(id_vendor, id_product, bcdDeviceMin, bcdDeviceMax, \
		    vendorName, productName, useProtocol, useTransport, \
		    initFunction, flags) \
{ USB_DEVICE_VER(id_vendor, id_product, bcdDeviceMin, bcdDeviceMax), \
  .driver_info = (flags) }

static struct usb_device_id freecom_usb_ids[] = {
#	include "unusual_freecom.h"
	{ }		/*                   */
};
MODULE_DEVICE_TABLE(usb, freecom_usb_ids);

#undef UNUSUAL_DEV

/*
                  
 */
#define UNUSUAL_DEV(idVendor, idProduct, bcdDeviceMin, bcdDeviceMax, \
		    vendor_name, product_name, use_protocol, use_transport, \
		    init_function, Flags) \
{ \
	.vendorName = vendor_name,	\
	.productName = product_name,	\
	.useProtocol = use_protocol,	\
	.useTransport = use_transport,	\
	.initFunction = init_function,	\
}

static struct us_unusual_dev freecom_unusual_dev_list[] = {
#	include "unusual_freecom.h"
	{ }		/*                   */
};

#undef UNUSUAL_DEV

static int
freecom_readdata (struct scsi_cmnd *srb, struct us_data *us,
		unsigned int ipipe, unsigned int opipe, int count)
{
	struct freecom_xfer_wrap *fxfr =
		(struct freecom_xfer_wrap *) us->iobuf;
	int result;

	fxfr->Type = FCM_PACKET_INPUT | 0x00;
	fxfr->Timeout = 0;    /*                              */
	fxfr->Count = cpu_to_le32 (count);
	memset (fxfr->Pad, 0, sizeof (fxfr->Pad));

	usb_stor_dbg(us, "Read data Freecom! (c=%d)\n", count);

	/*                             */
	result = usb_stor_bulk_transfer_buf (us, opipe, fxfr,
			FCM_PACKET_LENGTH, NULL);
	if (result != USB_STOR_XFER_GOOD) {
		usb_stor_dbg(us, "Freecom readdata transport error\n");
		return USB_STOR_TRANSPORT_ERROR;
	}

	/*                                 */
	usb_stor_dbg(us, "Start of read\n");
	result = usb_stor_bulk_srb(us, ipipe, srb);
	usb_stor_dbg(us, "freecom_readdata done!\n");

	if (result > USB_STOR_XFER_SHORT)
		return USB_STOR_TRANSPORT_ERROR;
	return USB_STOR_TRANSPORT_GOOD;
}

static int
freecom_writedata (struct scsi_cmnd *srb, struct us_data *us,
		int unsigned ipipe, unsigned int opipe, int count)
{
	struct freecom_xfer_wrap *fxfr =
		(struct freecom_xfer_wrap *) us->iobuf;
	int result;

	fxfr->Type = FCM_PACKET_OUTPUT | 0x00;
	fxfr->Timeout = 0;    /*                              */
	fxfr->Count = cpu_to_le32 (count);
	memset (fxfr->Pad, 0, sizeof (fxfr->Pad));

	usb_stor_dbg(us, "Write data Freecom! (c=%d)\n", count);

	/*                             */
	result = usb_stor_bulk_transfer_buf (us, opipe, fxfr,
			FCM_PACKET_LENGTH, NULL);
	if (result != USB_STOR_XFER_GOOD) {
		usb_stor_dbg(us, "Freecom writedata transport error\n");
		return USB_STOR_TRANSPORT_ERROR;
	}

	/*                                 */
	usb_stor_dbg(us, "Start of write\n");
	result = usb_stor_bulk_srb(us, opipe, srb);

	usb_stor_dbg(us, "freecom_writedata done!\n");
	if (result > USB_STOR_XFER_SHORT)
		return USB_STOR_TRANSPORT_ERROR;
	return USB_STOR_TRANSPORT_GOOD;
}

/*
                                             
  
 */
static int freecom_transport(struct scsi_cmnd *srb, struct us_data *us)
{
	struct freecom_cb_wrap *fcb;
	struct freecom_status  *fst;
	unsigned int ipipe, opipe;		/*                     */
	int result;
	unsigned int partial;
	int length;

	fcb = (struct freecom_cb_wrap *) us->iobuf;
	fst = (struct freecom_status *) us->iobuf;

	usb_stor_dbg(us, "Freecom TRANSPORT STARTED\n");

	/*                                  */
	opipe = us->send_bulk_pipe;
	ipipe = us->recv_bulk_pipe;

	/*                                          */
	fcb->Type = FCM_PACKET_ATAPI | 0x00;
	fcb->Timeout = 0;
	memcpy (fcb->Atapi, srb->cmnd, 12);
	memset (fcb->Filler, 0, sizeof (fcb->Filler));

	US_DEBUG(pdump(us, srb->cmnd, 12));

	/*              */
	result = usb_stor_bulk_transfer_buf (us, opipe, fcb,
			FCM_PACKET_LENGTH, NULL);

	/*                                                                 
                                                                
                                */
	if (result != USB_STOR_XFER_GOOD) {
		usb_stor_dbg(us, "freecom transport error\n");
		return USB_STOR_TRANSPORT_ERROR;
	}

	/*                                                             
                                         */
	result = usb_stor_bulk_transfer_buf (us, ipipe, fst,
			FCM_STATUS_PACKET_LENGTH, &partial);
	usb_stor_dbg(us, "foo Status result %d %u\n", result, partial);
	if (result != USB_STOR_XFER_GOOD)
		return USB_STOR_TRANSPORT_ERROR;

	US_DEBUG(pdump(us, (void *)fst, partial));

	/*                                                                    
                                                                 
                                                                    
                                                             
   
                                                                       
                                                                   
  */
	while (fst->Status & FCM_STATUS_BUSY) {
		usb_stor_dbg(us, "20 second USB/ATAPI bridge TIMEOUT occurred!\n");
		usb_stor_dbg(us, "fst->Status is %x\n", fst->Status);

		/*                      */
		fcb->Type = FCM_PACKET_STATUS;
		fcb->Timeout = 0;
		memset (fcb->Atapi, 0, sizeof(fcb->Atapi));
		memset (fcb->Filler, 0, sizeof (fcb->Filler));

		/*              */
		result = usb_stor_bulk_transfer_buf (us, opipe, fcb,
				FCM_PACKET_LENGTH, NULL);

		/*                                                        
                                                         
                                                 
   */
		if (result != USB_STOR_XFER_GOOD) {
			usb_stor_dbg(us, "freecom transport error\n");
			return USB_STOR_TRANSPORT_ERROR;
		}

		/*              */
		result = usb_stor_bulk_transfer_buf (us, ipipe, fst,
				FCM_STATUS_PACKET_LENGTH, &partial);

		usb_stor_dbg(us, "bar Status result %d %u\n", result, partial);
		if (result != USB_STOR_XFER_GOOD)
			return USB_STOR_TRANSPORT_ERROR;

		US_DEBUG(pdump(us, (void *)fst, partial));
	}

	if (partial != 4)
		return USB_STOR_TRANSPORT_ERROR;
	if ((fst->Status & 1) != 0) {
		usb_stor_dbg(us, "operation failed\n");
		return USB_STOR_TRANSPORT_FAILED;
	}

	/*                                                       
                                                                   
                        */
	usb_stor_dbg(us, "Device indicates that it has %d bytes available\n",
		     le16_to_cpu(fst->Count));
	usb_stor_dbg(us, "SCSI requested %d\n", scsi_bufflen(srb));

	/*                                    */
	switch (srb->cmnd[0]) {
	case INQUIRY:
	case REQUEST_SENSE:	/*                                                            */
	case MODE_SENSE:
	case MODE_SENSE_10:
		length = le16_to_cpu(fst->Count);
		break;
	default:
		length = scsi_bufflen(srb);
	}

	/*                                  */
	if (length > scsi_bufflen(srb)) {
		length = scsi_bufflen(srb);
		usb_stor_dbg(us, "Truncating request to match buffer length: %d\n",
			     length);
	}

	/*                                                                 
             */

	switch (us->srb->sc_data_direction) {
	case DMA_FROM_DEVICE:
		/*                                  */
		if (!length)
			break;
		/*                                                    
                         */
		if ((fst->Status & DRQ_STAT) == 0 || (fst->Reason & 3) != 2) {
			usb_stor_dbg(us, "SCSI wants data, drive doesn't have any\n");
			return USB_STOR_TRANSPORT_FAILED;
		}
		result = freecom_readdata (srb, us, ipipe, opipe, length);
		if (result != USB_STOR_TRANSPORT_GOOD)
			return result;

		usb_stor_dbg(us, "Waiting for status\n");
		result = usb_stor_bulk_transfer_buf (us, ipipe, fst,
				FCM_PACKET_LENGTH, &partial);
		US_DEBUG(pdump(us, (void *)fst, partial));

		if (partial != 4 || result > USB_STOR_XFER_SHORT)
			return USB_STOR_TRANSPORT_ERROR;
		if ((fst->Status & ERR_STAT) != 0) {
			usb_stor_dbg(us, "operation failed\n");
			return USB_STOR_TRANSPORT_FAILED;
		}
		if ((fst->Reason & 3) != 3) {
			usb_stor_dbg(us, "Drive seems still hungry\n");
			return USB_STOR_TRANSPORT_FAILED;
		}
		usb_stor_dbg(us, "Transfer happy\n");
		break;

	case DMA_TO_DEVICE:
		/*                                   */
		if (!length)
			break;
		/*                                                        
                   */
		/*               */
		result = freecom_writedata (srb, us, ipipe, opipe, length);
		if (result != USB_STOR_TRANSPORT_GOOD)
			return result;

		usb_stor_dbg(us, "Waiting for status\n");
		result = usb_stor_bulk_transfer_buf (us, ipipe, fst,
				FCM_PACKET_LENGTH, &partial);

		if (partial != 4 || result > USB_STOR_XFER_SHORT)
			return USB_STOR_TRANSPORT_ERROR;
		if ((fst->Status & ERR_STAT) != 0) {
			usb_stor_dbg(us, "operation failed\n");
			return USB_STOR_TRANSPORT_FAILED;
		}
		if ((fst->Reason & 3) != 3) {
			usb_stor_dbg(us, "Drive seems still hungry\n");
			return USB_STOR_TRANSPORT_FAILED;
		}

		usb_stor_dbg(us, "Transfer happy\n");
		break;


	case DMA_NONE:
		/*                   */
		break;

	default:
		/*                                            */
		usb_stor_dbg(us, "freecom unimplemented direction: %d\n",
			     us->srb->sc_data_direction);
		/*                                                */
		return USB_STOR_TRANSPORT_FAILED;
		break;
	}

	return USB_STOR_TRANSPORT_GOOD;
}

static int init_freecom(struct us_data *us)
{
	int result;
	char *buffer = us->iobuf;

	/*                                                           
                                                                 
  */

	result = usb_stor_control_msg(us, us->recv_ctrl_pipe,
			0x4c, 0xc0, 0x4346, 0x0, buffer, 0x20, 3*HZ);
	buffer[32] = '\0';
	usb_stor_dbg(us, "String returned from FC init is: %s\n", buffer);

	/*                                                              
                                                                    
                                                              
                   
  */

	/*            */
	result = usb_stor_control_msg(us, us->send_ctrl_pipe,
			0x4d, 0x40, 0x24d8, 0x0, NULL, 0x0, 3*HZ);
	usb_stor_dbg(us, "result from activate reset is %d\n", result);

	/*            */
	mdelay(250);

	/*             */
	result = usb_stor_control_msg(us, us->send_ctrl_pipe,
			0x4d, 0x40, 0x24f8, 0x0, NULL, 0x0, 3*HZ);
	usb_stor_dbg(us, "result from clear reset is %d\n", result);

	/*                */
	mdelay(3 * 1000);

	return USB_STOR_TRANSPORT_GOOD;
}

static int usb_stor_freecom_reset(struct us_data *us)
{
	printk (KERN_CRIT "freecom reset called\n");

	/*                                    */
	return FAILED;
}

#ifdef CONFIG_USB_STORAGE_DEBUG
static void pdump(struct us_data *us, void *ibuffer, int length)
{
	static char line[80];
	int offset = 0;
	unsigned char *buffer = (unsigned char *) ibuffer;
	int i, j;
	int from, base;

	offset = 0;
	for (i = 0; i < length; i++) {
		if ((i & 15) == 0) {
			if (i > 0) {
				offset += sprintf (line+offset, " - ");
				for (j = i - 16; j < i; j++) {
					if (buffer[j] >= 32 && buffer[j] <= 126)
						line[offset++] = buffer[j];
					else
						line[offset++] = '.';
				}
				line[offset] = 0;
				usb_stor_dbg(us, "%s\n", line);
				offset = 0;
			}
			offset += sprintf (line+offset, "%08x:", i);
		} else if ((i & 7) == 0) {
			offset += sprintf (line+offset, " -");
		}
		offset += sprintf (line+offset, " %02x", buffer[i] & 0xff);
	}

	/*                               */
	from = (length - 1) % 16;
	base = ((length - 1) / 16) * 16;

	for (i = from + 1; i < 16; i++)
		offset += sprintf (line+offset, "   ");
	if (from < 8)
		offset += sprintf (line+offset, "  ");
	offset += sprintf (line+offset, " - ");

	for (i = 0; i <= from; i++) {
		if (buffer[base+i] >= 32 && buffer[base+i] <= 126)
			line[offset++] = buffer[base+i];
		else
			line[offset++] = '.';
	}
	line[offset] = 0;
	usb_stor_dbg(us, "%s\n", line);
	offset = 0;
}
#endif

static int freecom_probe(struct usb_interface *intf,
			 const struct usb_device_id *id)
{
	struct us_data *us;
	int result;

	result = usb_stor_probe1(&us, intf, id,
			(id - freecom_usb_ids) + freecom_unusual_dev_list);
	if (result)
		return result;

	us->transport_name = "Freecom";
	us->transport = freecom_transport;
	us->transport_reset = usb_stor_freecom_reset;
	us->max_lun = 0;

	result = usb_stor_probe2(us);
	return result;
}

static struct usb_driver freecom_driver = {
	.name =		"ums-freecom",
	.probe =	freecom_probe,
	.disconnect =	usb_stor_disconnect,
	.suspend =	usb_stor_suspend,
	.resume =	usb_stor_resume,
	.reset_resume =	usb_stor_reset_resume,
	.pre_reset =	usb_stor_pre_reset,
	.post_reset =	usb_stor_post_reset,
	.id_table =	freecom_usb_ids,
	.soft_unbind =	1,
	.no_dynamic_id = 1,
};

module_usb_driver(freecom_driver);
