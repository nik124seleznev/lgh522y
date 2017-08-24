/*
 *
 *  Support for a cx23416 mpeg encoder via cx2388x host port.
 *  "blackbird" reference design.
 *
 *    (c) 2004 Jelle Foks <jelle@foks.us>
 *    (c) 2004 Gerd Knorr <kraxel@bytesex.org>
 *
 *    (c) 2005-2006 Mauro Carvalho Chehab <mchehab@infradead.org>
 *        - video_ioctl2 conversion
 *
 *  Includes parts from the ivtv driver <http://sourceforge.net/projects/ivtv/>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <media/v4l2-common.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-event.h>
#include <media/cx2341x.h>

#include "cx88.h"

MODULE_DESCRIPTION("driver for cx2388x/cx23416 based mpeg encoder cards");
MODULE_AUTHOR("Jelle Foks <jelle@foks.us>, Gerd Knorr <kraxel@bytesex.org> [SuSE Labs]");
MODULE_LICENSE("GPL");
MODULE_VERSION(CX88_VERSION);

static unsigned int mpegbufs = 32;
module_param(mpegbufs,int,0644);
MODULE_PARM_DESC(mpegbufs,"number of mpeg buffers, range 2-32");

static unsigned int debug;
module_param(debug,int,0644);
MODULE_PARM_DESC(debug,"enable debug messages [blackbird]");

#define dprintk(level, fmt, arg...) do {				      \
	if (debug + 1 > level)						      \
		printk(KERN_DEBUG "%s/2-bb: " fmt, dev->core->name , ## arg); \
} while(0)

/*                                                                    */

#define BLACKBIRD_FIRM_IMAGE_SIZE 376836

/*                                      */

#define IVTV_CMD_HW_BLOCKS_RST 0xFFFFFFFF

/*                       */
#define IVTV_API_STD_TIMEOUT 500

enum blackbird_capture_type {
	BLACKBIRD_MPEG_CAPTURE,
	BLACKBIRD_RAW_CAPTURE,
	BLACKBIRD_RAW_PASSTHRU_CAPTURE
};
enum blackbird_capture_bits {
	BLACKBIRD_RAW_BITS_NONE             = 0x00,
	BLACKBIRD_RAW_BITS_YUV_CAPTURE      = 0x01,
	BLACKBIRD_RAW_BITS_PCM_CAPTURE      = 0x02,
	BLACKBIRD_RAW_BITS_VBI_CAPTURE      = 0x04,
	BLACKBIRD_RAW_BITS_PASSTHRU_CAPTURE = 0x08,
	BLACKBIRD_RAW_BITS_TO_HOST_CAPTURE  = 0x10
};
enum blackbird_capture_end {
	BLACKBIRD_END_AT_GOP, /*                                      */
	BLACKBIRD_END_NOW, /*                          */
};
enum blackbird_framerate {
	BLACKBIRD_FRAMERATE_NTSC_30, /*             */
	BLACKBIRD_FRAMERATE_PAL_25   /*            */
};
enum blackbird_stream_port {
	BLACKBIRD_OUTPUT_PORT_MEMORY,
	BLACKBIRD_OUTPUT_PORT_STREAMING,
	BLACKBIRD_OUTPUT_PORT_SERIAL
};
enum blackbird_data_xfer_status {
	BLACKBIRD_MORE_BUFFERS_FOLLOW,
	BLACKBIRD_LAST_BUFFER,
};
enum blackbird_picture_mask {
	BLACKBIRD_PICTURE_MASK_NONE,
	BLACKBIRD_PICTURE_MASK_I_FRAMES,
	BLACKBIRD_PICTURE_MASK_I_P_FRAMES = 0x3,
	BLACKBIRD_PICTURE_MASK_ALL_FRAMES = 0x7,
};
enum blackbird_vbi_mode_bits {
	BLACKBIRD_VBI_BITS_SLICED,
	BLACKBIRD_VBI_BITS_RAW,
};
enum blackbird_vbi_insertion_bits {
	BLACKBIRD_VBI_BITS_INSERT_IN_XTENSION_USR_DATA,
	BLACKBIRD_VBI_BITS_INSERT_IN_PRIVATE_PACKETS = 0x1 << 1,
	BLACKBIRD_VBI_BITS_SEPARATE_STREAM = 0x2 << 1,
	BLACKBIRD_VBI_BITS_SEPARATE_STREAM_USR_DATA = 0x4 << 1,
	BLACKBIRD_VBI_BITS_SEPARATE_STREAM_PRV_DATA = 0x5 << 1,
};
enum blackbird_dma_unit {
	BLACKBIRD_DMA_BYTES,
	BLACKBIRD_DMA_FRAMES,
};
enum blackbird_dma_transfer_status_bits {
	BLACKBIRD_DMA_TRANSFER_BITS_DONE = 0x01,
	BLACKBIRD_DMA_TRANSFER_BITS_ERROR = 0x04,
	BLACKBIRD_DMA_TRANSFER_BITS_LL_ERROR = 0x10,
};
enum blackbird_pause {
	BLACKBIRD_PAUSE_ENCODING,
	BLACKBIRD_RESUME_ENCODING,
};
enum blackbird_copyright {
	BLACKBIRD_COPYRIGHT_OFF,
	BLACKBIRD_COPYRIGHT_ON,
};
enum blackbird_notification_type {
	BLACKBIRD_NOTIFICATION_REFRESH,
};
enum blackbird_notification_status {
	BLACKBIRD_NOTIFICATION_OFF,
	BLACKBIRD_NOTIFICATION_ON,
};
enum blackbird_notification_mailbox {
	BLACKBIRD_NOTIFICATION_NO_MAILBOX = -1,
};
enum blackbird_field1_lines {
	BLACKBIRD_FIELD1_SAA7114 = 0x00EF, /*     */
	BLACKBIRD_FIELD1_SAA7115 = 0x00F0, /*     */
	BLACKBIRD_FIELD1_MICRONAS = 0x0105, /*     */
};
enum blackbird_field2_lines {
	BLACKBIRD_FIELD2_SAA7114 = 0x00EF, /*     */
	BLACKBIRD_FIELD2_SAA7115 = 0x00F0, /*     */
	BLACKBIRD_FIELD2_MICRONAS = 0x0106, /*     */
};
enum blackbird_custom_data_type {
	BLACKBIRD_CUSTOM_EXTENSION_USR_DATA,
	BLACKBIRD_CUSTOM_PRIVATE_PACKET,
};
enum blackbird_mute {
	BLACKBIRD_UNMUTE,
	BLACKBIRD_MUTE,
};
enum blackbird_mute_video_mask {
	BLACKBIRD_MUTE_VIDEO_V_MASK = 0x0000FF00,
	BLACKBIRD_MUTE_VIDEO_U_MASK = 0x00FF0000,
	BLACKBIRD_MUTE_VIDEO_Y_MASK = 0xFF000000,
};
enum blackbird_mute_video_shift {
	BLACKBIRD_MUTE_VIDEO_V_SHIFT = 8,
	BLACKBIRD_MUTE_VIDEO_U_SHIFT = 16,
	BLACKBIRD_MUTE_VIDEO_Y_SHIFT = 24,
};

/*           */
#define IVTV_REG_ENC_SDRAM_REFRESH (0x07F8 /*                 */)
#define IVTV_REG_ENC_SDRAM_PRECHARGE (0x07FC /*                 */)
#define IVTV_REG_SPU (0x9050 /*                 */)
#define IVTV_REG_HW_BLOCKS (0x9054 /*                 */)
#define IVTV_REG_VPU (0x9058 /*                 */)
#define IVTV_REG_APU (0xA064 /*                 */)

/*                                                                    */

static void host_setup(struct cx88_core *core)
{
	/*                          */
	cx_write(MO_GPHST_SOFT_RST, 1);
	udelay(100);
	cx_write(MO_GPHST_SOFT_RST, 0);
	udelay(100);

	/*                 */
	cx_write(MO_GPHST_WSC, 0x44444444U);
	cx_write(MO_GPHST_XFR, 0);
	cx_write(MO_GPHST_WDTH, 15);
	cx_write(MO_GPHST_HDSHK, 0);
	cx_write(MO_GPHST_MUX16, 0x44448888U);
	cx_write(MO_GPHST_MODE, 0);
}

/*                                                                    */

#define P1_MDATA0 0x390000
#define P1_MDATA1 0x390001
#define P1_MDATA2 0x390002
#define P1_MDATA3 0x390003
#define P1_MADDR2 0x390004
#define P1_MADDR1 0x390005
#define P1_MADDR0 0x390006
#define P1_RDATA0 0x390008
#define P1_RDATA1 0x390009
#define P1_RDATA2 0x39000A
#define P1_RDATA3 0x39000B
#define P1_RADDR0 0x39000C
#define P1_RADDR1 0x39000D
#define P1_RRDWR  0x39000E

static int wait_ready_gpio0_bit1(struct cx88_core *core, u32 state)
{
	unsigned long timeout = jiffies + msecs_to_jiffies(1);
	u32 gpio0,need;

	need = state ? 2 : 0;
	for (;;) {
		gpio0 = cx_read(MO_GP0_IO) & 2;
		if (need == gpio0)
			return 0;
		if (time_after(jiffies,timeout))
			return -1;
		udelay(1);
	}
}

static int memory_write(struct cx88_core *core, u32 address, u32 value)
{
	/*                                             */
	cx_writeb(P1_MDATA0, (unsigned int)value);
	cx_writeb(P1_MDATA1, (unsigned int)(value >> 8));
	cx_writeb(P1_MDATA2, (unsigned int)(value >> 16));
	cx_writeb(P1_MDATA3, (unsigned int)(value >> 24));
	cx_writeb(P1_MADDR2, (unsigned int)(address >> 16) | 0x40);
	cx_writeb(P1_MADDR1, (unsigned int)(address >> 8));
	cx_writeb(P1_MADDR0, (unsigned int)address);
	cx_read(P1_MDATA0);
	cx_read(P1_MADDR0);

	return wait_ready_gpio0_bit1(core,1);
}

static int memory_read(struct cx88_core *core, u32 address, u32 *value)
{
	int retval;
	u32 val;

	/*                                             */
	cx_writeb(P1_MADDR2, (unsigned int)(address >> 16) & ~0xC0);
	cx_writeb(P1_MADDR1, (unsigned int)(address >> 8));
	cx_writeb(P1_MADDR0, (unsigned int)address);
	cx_read(P1_MADDR0);

	retval = wait_ready_gpio0_bit1(core,1);

	cx_writeb(P1_MDATA3, 0);
	val     = (unsigned char)cx_read(P1_MDATA3) << 24;
	cx_writeb(P1_MDATA2, 0);
	val    |= (unsigned char)cx_read(P1_MDATA2) << 16;
	cx_writeb(P1_MDATA1, 0);
	val    |= (unsigned char)cx_read(P1_MDATA1) << 8;
	cx_writeb(P1_MDATA0, 0);
	val    |= (unsigned char)cx_read(P1_MDATA0);

	*value  = val;
	return retval;
}

static int register_write(struct cx88_core *core, u32 address, u32 value)
{
	cx_writeb(P1_RDATA0, (unsigned int)value);
	cx_writeb(P1_RDATA1, (unsigned int)(value >> 8));
	cx_writeb(P1_RDATA2, (unsigned int)(value >> 16));
	cx_writeb(P1_RDATA3, (unsigned int)(value >> 24));
	cx_writeb(P1_RADDR0, (unsigned int)address);
	cx_writeb(P1_RADDR1, (unsigned int)(address >> 8));
	cx_writeb(P1_RRDWR, 1);
	cx_read(P1_RDATA0);
	cx_read(P1_RADDR0);

	return wait_ready_gpio0_bit1(core,1);
}


static int register_read(struct cx88_core *core, u32 address, u32 *value)
{
	int retval;
	u32 val;

	cx_writeb(P1_RADDR0, (unsigned int)address);
	cx_writeb(P1_RADDR1, (unsigned int)(address >> 8));
	cx_writeb(P1_RRDWR, 0);
	cx_read(P1_RADDR0);

	retval  = wait_ready_gpio0_bit1(core,1);
	val     = (unsigned char)cx_read(P1_RDATA0);
	val    |= (unsigned char)cx_read(P1_RDATA1) << 8;
	val    |= (unsigned char)cx_read(P1_RDATA2) << 16;
	val    |= (unsigned char)cx_read(P1_RDATA3) << 24;

	*value  = val;
	return retval;
}

/*                                                                    */

static int blackbird_mbox_func(void *priv, u32 command, int in, int out, u32 data[CX2341X_MBOX_MAX_DATA])
{
	struct cx8802_dev *dev = priv;
	unsigned long timeout;
	u32 value, flag, retval;
	int i;

	dprintk(1,"%s: 0x%X\n", __func__, command);

	/*                                                               
                         */
	memory_read(dev->core, dev->mailbox - 4, &value);
	if (value != 0x12345678) {
		dprintk(0, "Firmware and/or mailbox pointer not initialized or corrupted\n");
		return -1;
	}

	memory_read(dev->core, dev->mailbox, &flag);
	if (flag) {
		dprintk(0, "ERROR: Mailbox appears to be in use (%x)\n", flag);
		return -1;
	}

	flag |= 1; /*                              */
	memory_write(dev->core, dev->mailbox, flag);

	/*                                                  */
	memory_write(dev->core, dev->mailbox + 1, command); /*              */
	memory_write(dev->core, dev->mailbox + 3, IVTV_API_STD_TIMEOUT); /*         */
	for (i = 0; i < in; i++) {
		memory_write(dev->core, dev->mailbox + 4 + i, data[i]);
		dprintk(1, "API Input %d = %d\n", i, data[i]);
	}
	for (; i < CX2341X_MBOX_MAX_DATA; i++)
		memory_write(dev->core, dev->mailbox + 4 + i, 0);

	flag |= 3; /*                             */
	memory_write(dev->core, dev->mailbox, flag);

	/*                                             */
	timeout = jiffies + msecs_to_jiffies(10);
	for (;;) {
		memory_read(dev->core, dev->mailbox, &flag);
		if (0 != (flag & 4))
			break;
		if (time_after(jiffies,timeout)) {
			dprintk(0, "ERROR: API Mailbox timeout\n");
			return -1;
		}
		udelay(10);
	}

	/*                    */
	for (i = 0; i < out; i++) {
		memory_read(dev->core, dev->mailbox + 4 + i, data + i);
		dprintk(1, "API Output %d = %d\n", i, data[i]);
	}

	memory_read(dev->core, dev->mailbox + 2, &retval);
	dprintk(1, "API result = %d\n",retval);

	flag = 0;
	memory_write(dev->core, dev->mailbox, flag);
	return retval;
}
/*                                                                    */

/*                                                                                      */
static int blackbird_api_cmd(struct cx8802_dev *dev, u32 command,
			     u32 inputcnt, u32 outputcnt, ...)
{
	u32 data[CX2341X_MBOX_MAX_DATA];
	va_list vargs;
	int i, err;

	va_start(vargs, outputcnt);

	for (i = 0; i < inputcnt; i++) {
		data[i] = va_arg(vargs, int);
	}
	err = blackbird_mbox_func(dev, command, inputcnt, outputcnt, data);
	for (i = 0; i < outputcnt; i++) {
		int *vptr = va_arg(vargs, int *);
		*vptr = data[i];
	}
	va_end(vargs);
	return err;
}

static int blackbird_find_mailbox(struct cx8802_dev *dev)
{
	u32 signature[4]={0x12345678, 0x34567812, 0x56781234, 0x78123456};
	int signaturecnt=0;
	u32 value;
	int i;

	for (i = 0; i < BLACKBIRD_FIRM_IMAGE_SIZE; i++) {
		memory_read(dev->core, i, &value);
		if (value == signature[signaturecnt])
			signaturecnt++;
		else
			signaturecnt = 0;
		if (4 == signaturecnt) {
			dprintk(1, "Mailbox signature found\n");
			return i+1;
		}
	}
	dprintk(0, "Mailbox signature values not found!\n");
	return -1;
}

static int blackbird_load_firmware(struct cx8802_dev *dev)
{
	static const unsigned char magic[8] = {
		0xa7, 0x0d, 0x00, 0x00, 0x66, 0xbb, 0x55, 0xaa
	};
	const struct firmware *firmware;
	int i, retval = 0;
	u32 value = 0;
	u32 checksum = 0;
	u32 *dataptr;

	retval  = register_write(dev->core, IVTV_REG_VPU, 0xFFFFFFED);
	retval |= register_write(dev->core, IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
	retval |= register_write(dev->core, IVTV_REG_ENC_SDRAM_REFRESH, 0x80000640);
	retval |= register_write(dev->core, IVTV_REG_ENC_SDRAM_PRECHARGE, 0x1A);
	msleep(1);
	retval |= register_write(dev->core, IVTV_REG_APU, 0);

	if (retval < 0)
		dprintk(0, "Error with register_write\n");

	retval = request_firmware(&firmware, CX2341X_FIRM_ENC_FILENAME,
				  &dev->pci->dev);


	if (retval != 0) {
		dprintk(0, "ERROR: Hotplug firmware request failed (%s).\n",
			CX2341X_FIRM_ENC_FILENAME);
		dprintk(0, "Please fix your hotplug setup, the board will "
			"not work without firmware loaded!\n");
		return -1;
	}

	if (firmware->size != BLACKBIRD_FIRM_IMAGE_SIZE) {
		dprintk(0, "ERROR: Firmware size mismatch (have %zd, expected %d)\n",
			firmware->size, BLACKBIRD_FIRM_IMAGE_SIZE);
		release_firmware(firmware);
		return -1;
	}

	if (0 != memcmp(firmware->data, magic, 8)) {
		dprintk(0, "ERROR: Firmware magic mismatch, wrong file?\n");
		release_firmware(firmware);
		return -1;
	}

	/*                      */
	dprintk(1,"Loading firmware ...\n");
	dataptr = (u32*)firmware->data;
	for (i = 0; i < (firmware->size >> 2); i++) {
		value = le32_to_cpu(*dataptr);
		checksum += ~value;
		memory_write(dev->core, i, value);
		dataptr++;
	}

	/*                                       */
	for (i--; i >= 0; i--) {
		memory_read(dev->core, i, &value);
		checksum -= ~value;
	}
	if (checksum) {
		dprintk(0, "ERROR: Firmware load failed (checksum mismatch).\n");
		release_firmware(firmware);
		return -1;
	}
	release_firmware(firmware);
	dprintk(0, "Firmware upload successful.\n");

	retval |= register_write(dev->core, IVTV_REG_HW_BLOCKS, IVTV_CMD_HW_BLOCKS_RST);
	retval |= register_read(dev->core, IVTV_REG_SPU, &value);
	retval |= register_write(dev->core, IVTV_REG_SPU, value & 0xFFFFFFFE);
	msleep(1);

	retval |= register_read(dev->core, IVTV_REG_VPU, &value);
	retval |= register_write(dev->core, IVTV_REG_VPU, value & 0xFFFFFFE8);

	if (retval < 0)
		dprintk(0, "Error with register_write\n");
	return 0;
}

/* 
                                                 
                                                                                                                 
                                                                                                                 
                                                                                                                 
                                                                                                                
                                                                                                                
                                                                                                                
                                                                                                                
                                                                                                                
                                                                                                                 
                 
*/

static void blackbird_codec_settings(struct cx8802_dev *dev)
{
	/*                   */
	blackbird_api_cmd(dev, CX2341X_ENC_SET_FRAME_SIZE, 2, 0,
				dev->height, dev->width);

	dev->cxhdl.width = dev->width;
	dev->cxhdl.height = dev->height;
	cx2341x_handler_set_50hz(&dev->cxhdl, dev->core->tvnorm & V4L2_STD_625_50);
	cx2341x_handler_setup(&dev->cxhdl);
}

static int blackbird_initialize_codec(struct cx8802_dev *dev)
{
	struct cx88_core *core = dev->core;
	int version;
	int retval;

	dprintk(1,"Initialize codec\n");
	retval = blackbird_api_cmd(dev, CX2341X_ENC_PING_FW, 0, 0); /*      */
	if (retval < 0) {

		dev->mpeg_active = 0;

		/*                                                    */
		cx_write(MO_SRST_IO, 0); /*            */
		cx_write(MO_SRST_IO, 1); /*            */
		retval = blackbird_load_firmware(dev);
		if (retval < 0)
			return retval;

		retval = blackbird_find_mailbox(dev);
		if (retval < 0)
			return -1;

		dev->mailbox = retval;

		retval = blackbird_api_cmd(dev, CX2341X_ENC_PING_FW, 0, 0); /*      */
		if (retval < 0) {
			dprintk(0, "ERROR: Firmware ping failed!\n");
			return -1;
		}

		retval = blackbird_api_cmd(dev, CX2341X_ENC_GET_VERSION, 0, 1, &version);
		if (retval < 0) {
			dprintk(0, "ERROR: Firmware get encoder version failed!\n");
			return -1;
		}
		dprintk(0, "Firmware version is 0x%08x\n", version);
	}

	cx_write(MO_PINMUX_IO, 0x88); /*                                         */
	cx_clear(MO_INPUT_FORMAT, 0x100); /*                                   */
	cx_write(MO_VBOS_CONTROL, 0x84A00); /*                                        */
	cx_clear(MO_OUTPUT_FORMAT, 0x0008); /*                                              */

	blackbird_codec_settings(dev);

	blackbird_api_cmd(dev, CX2341X_ENC_SET_NUM_VSYNC_LINES, 2, 0,
			BLACKBIRD_FIELD1_SAA7115,
			BLACKBIRD_FIELD2_SAA7115
		);

	blackbird_api_cmd(dev, CX2341X_ENC_SET_PLACEHOLDER, 12, 0,
			BLACKBIRD_CUSTOM_EXTENSION_USR_DATA,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	return 0;
}

static int blackbird_start_codec(struct file *file, void *priv)
{
	struct cx8802_dev *dev  = ((struct cx8802_fh *)priv)->dev;
	struct cx88_core *core = dev->core;
	/*                                       */
	u32 reg;

	int i;
	int lastchange = -1;
	int lastval = 0;

	for (i = 0; (i < 10) && (i < (lastchange + 4)); i++) {
		reg = cx_read(AUD_STATUS);

		dprintk(1, "AUD_STATUS:%dL: 0x%x\n", i, reg);
		if ((reg & 0x0F) != lastval) {
			lastval = reg & 0x0F;
			lastchange = i;
		}
		msleep(100);
	}

	/*                     */
	cx_clear(AUD_VOL_CTL, (1 << 6));

	blackbird_api_cmd(dev, CX2341X_ENC_REFRESH_INPUT, 0, 0);

	/*                            */
	blackbird_api_cmd(dev, CX2341X_ENC_INITIALIZE_INPUT, 0, 0);

	cx2341x_handler_set_busy(&dev->cxhdl, 1);

	/*                                       */
	blackbird_api_cmd(dev, CX2341X_ENC_START_CAPTURE, 2, 0,
			BLACKBIRD_MPEG_CAPTURE,
			BLACKBIRD_RAW_BITS_NONE
		);

	dev->mpeg_active = 1;
	return 0;
}

static int blackbird_stop_codec(struct cx8802_dev *dev)
{
	blackbird_api_cmd(dev, CX2341X_ENC_STOP_CAPTURE, 3, 0,
			BLACKBIRD_END_NOW,
			BLACKBIRD_MPEG_CAPTURE,
			BLACKBIRD_RAW_BITS_NONE
		);

	cx2341x_handler_set_busy(&dev->cxhdl, 0);

	dev->mpeg_active = 0;
	return 0;
}

/*                                                                    */

static int bb_buf_setup(struct videobuf_queue *q,
			unsigned int *count, unsigned int *size)
{
	struct cx8802_fh *fh = q->priv_data;

	fh->dev->ts_packet_size  = 188 * 4; /*          */
	fh->dev->ts_packet_count = mpegbufs; /*          */

	*size = fh->dev->ts_packet_size * fh->dev->ts_packet_count;
	*count = fh->dev->ts_packet_count;
	return 0;
}

static int
bb_buf_prepare(struct videobuf_queue *q, struct videobuf_buffer *vb,
	       enum v4l2_field field)
{
	struct cx8802_fh *fh = q->priv_data;
	return cx8802_buf_prepare(q, fh->dev, (struct cx88_buffer*)vb, field);
}

static void
bb_buf_queue(struct videobuf_queue *q, struct videobuf_buffer *vb)
{
	struct cx8802_fh *fh = q->priv_data;
	cx8802_buf_queue(fh->dev, (struct cx88_buffer*)vb);
}

static void
bb_buf_release(struct videobuf_queue *q, struct videobuf_buffer *vb)
{
	cx88_free_buffer(q, (struct cx88_buffer*)vb);
}

static struct videobuf_queue_ops blackbird_qops = {
	.buf_setup    = bb_buf_setup,
	.buf_prepare  = bb_buf_prepare,
	.buf_queue    = bb_buf_queue,
	.buf_release  = bb_buf_release,
};

/*                                                                    */

static int vidioc_querycap(struct file *file, void  *priv,
					struct v4l2_capability *cap)
{
	struct cx8802_dev *dev  = ((struct cx8802_fh *)priv)->dev;
	struct cx88_core  *core = dev->core;

	strcpy(cap->driver, "cx88_blackbird");
	sprintf(cap->bus_info, "PCI:%s", pci_name(dev->pci));
	cx88_querycap(file, core, cap);
	return 0;
}

static int vidioc_enum_fmt_vid_cap (struct file *file, void  *priv,
					struct v4l2_fmtdesc *f)
{
	if (f->index != 0)
		return -EINVAL;

	strlcpy(f->description, "MPEG", sizeof(f->description));
	f->pixelformat = V4L2_PIX_FMT_MPEG;
	f->flags = V4L2_FMT_FLAG_COMPRESSED;
	return 0;
}

static int vidioc_g_fmt_vid_cap (struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct cx8802_fh  *fh   = priv;
	struct cx8802_dev *dev  = fh->dev;

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    = 188 * 4 * mpegbufs; /*                 */
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;
	f->fmt.pix.width        = dev->width;
	f->fmt.pix.height       = dev->height;
	f->fmt.pix.field        = fh->mpegq.field;
	dprintk(1, "VIDIOC_G_FMT: w: %d, h: %d, f: %d\n",
		dev->width, dev->height, fh->mpegq.field );
	return 0;
}

static int vidioc_try_fmt_vid_cap (struct file *file, void *priv,
			struct v4l2_format *f)
{
	struct cx8802_fh  *fh   = priv;
	struct cx8802_dev *dev  = fh->dev;

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    = 188 * 4 * mpegbufs; /*                 */
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;
	dprintk(1, "VIDIOC_TRY_FMT: w: %d, h: %d, f: %d\n",
		dev->width, dev->height, fh->mpegq.field );
	return 0;
}

static int vidioc_s_fmt_vid_cap (struct file *file, void *priv,
					struct v4l2_format *f)
{
	struct cx8802_fh  *fh   = priv;
	struct cx8802_dev *dev  = fh->dev;
	struct cx88_core  *core = dev->core;

	f->fmt.pix.pixelformat  = V4L2_PIX_FMT_MPEG;
	f->fmt.pix.bytesperline = 0;
	f->fmt.pix.sizeimage    = 188 * 4 * mpegbufs; /*                 */
	f->fmt.pix.colorspace   = V4L2_COLORSPACE_SMPTE170M;
	dev->width              = f->fmt.pix.width;
	dev->height             = f->fmt.pix.height;
	fh->mpegq.field         = f->fmt.pix.field;
	cx88_set_scale(core, f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.field);
	blackbird_api_cmd(dev, CX2341X_ENC_SET_FRAME_SIZE, 2, 0,
				f->fmt.pix.height, f->fmt.pix.width);
	dprintk(1, "VIDIOC_S_FMT: w: %d, h: %d, f: %d\n",
		f->fmt.pix.width, f->fmt.pix.height, f->fmt.pix.field );
	return 0;
}

static int vidioc_reqbufs (struct file *file, void *priv, struct v4l2_requestbuffers *p)
{
	struct cx8802_fh  *fh   = priv;
	return (videobuf_reqbufs(&fh->mpegq, p));
}

static int vidioc_querybuf (struct file *file, void *priv, struct v4l2_buffer *p)
{
	struct cx8802_fh  *fh   = priv;
	return (videobuf_querybuf(&fh->mpegq, p));
}

static int vidioc_qbuf (struct file *file, void *priv, struct v4l2_buffer *p)
{
	struct cx8802_fh  *fh   = priv;
	return (videobuf_qbuf(&fh->mpegq, p));
}

static int vidioc_dqbuf (struct file *file, void *priv, struct v4l2_buffer *p)
{
	struct cx8802_fh  *fh   = priv;
	return (videobuf_dqbuf(&fh->mpegq, p,
				file->f_flags & O_NONBLOCK));
}

static int vidioc_streamon(struct file *file, void *priv, enum v4l2_buf_type i)
{
	struct cx8802_fh  *fh   = priv;
	struct cx8802_dev *dev  = fh->dev;

	if (!dev->mpeg_active)
		blackbird_start_codec(file, fh);
	return videobuf_streamon(&fh->mpegq);
}

static int vidioc_streamoff(struct file *file, void *priv, enum v4l2_buf_type i)
{
	struct cx8802_fh  *fh   = priv;
	struct cx8802_dev *dev  = fh->dev;

	if (dev->mpeg_active)
		blackbird_stop_codec(dev);
	return videobuf_streamoff(&fh->mpegq);
}

static int vidioc_s_frequency (struct file *file, void *priv,
				const struct v4l2_frequency *f)
{
	struct cx8802_fh  *fh   = priv;
	struct cx8802_dev *dev  = fh->dev;
	struct cx88_core  *core = dev->core;

	if (unlikely(UNSET == core->board.tuner_type))
		return -EINVAL;
	if (unlikely(f->tuner != 0))
		return -EINVAL;
	if (dev->mpeg_active)
		blackbird_stop_codec(dev);

	cx88_set_freq (core,f);
	blackbird_initialize_codec(dev);
	cx88_set_scale(dev->core, dev->width, dev->height,
			fh->mpegq.field);
	return 0;
}

static int vidioc_log_status (struct file *file, void *priv)
{
	struct cx8802_dev *dev  = ((struct cx8802_fh *)priv)->dev;
	struct cx88_core  *core = dev->core;
	char name[32 + 2];

	snprintf(name, sizeof(name), "%s/2", core->name);
	call_all(core, core, log_status);
	v4l2_ctrl_handler_log_status(&dev->cxhdl.hdl, name);
	return 0;
}

static int vidioc_enum_input (struct file *file, void *priv,
				struct v4l2_input *i)
{
	struct cx88_core  *core = ((struct cx8802_fh *)priv)->dev->core;
	return cx88_enum_input (core,i);
}

static int vidioc_g_frequency (struct file *file, void *priv,
				struct v4l2_frequency *f)
{
	struct cx8802_fh  *fh   = priv;
	struct cx88_core  *core = fh->dev->core;

	if (unlikely(UNSET == core->board.tuner_type))
		return -EINVAL;
	if (unlikely(f->tuner != 0))
		return -EINVAL;

	f->frequency = core->freq;
	call_all(core, tuner, g_frequency, f);

	return 0;
}

static int vidioc_g_input (struct file *file, void *priv, unsigned int *i)
{
	struct cx88_core  *core = ((struct cx8802_fh *)priv)->dev->core;

	*i = core->input;
	return 0;
}

static int vidioc_s_input (struct file *file, void *priv, unsigned int i)
{
	struct cx88_core  *core = ((struct cx8802_fh *)priv)->dev->core;

	if (i >= 4)
		return -EINVAL;
	if (0 == INPUT(i).type)
		return -EINVAL;

	mutex_lock(&core->lock);
	cx88_newstation(core);
	cx88_video_mux(core,i);
	mutex_unlock(&core->lock);
	return 0;
}

static int vidioc_g_tuner (struct file *file, void *priv,
				struct v4l2_tuner *t)
{
	struct cx88_core  *core = ((struct cx8802_fh *)priv)->dev->core;
	u32 reg;

	if (unlikely(UNSET == core->board.tuner_type))
		return -EINVAL;
	if (0 != t->index)
		return -EINVAL;

	strcpy(t->name, "Television");
	t->capability = V4L2_TUNER_CAP_NORM;
	t->rangehigh  = 0xffffffffUL;
	call_all(core, tuner, g_tuner, t);

	cx88_get_stereo(core ,t);
	reg = cx_read(MO_DEVICE_STATUS);
	t->signal = (reg & (1<<5)) ? 0xffff : 0x0000;
	return 0;
}

static int vidioc_s_tuner (struct file *file, void *priv,
				const struct v4l2_tuner *t)
{
	struct cx88_core  *core = ((struct cx8802_fh *)priv)->dev->core;

	if (UNSET == core->board.tuner_type)
		return -EINVAL;
	if (0 != t->index)
		return -EINVAL;

	cx88_set_stereo(core, t->audmode, 1);
	return 0;
}

static int vidioc_g_std(struct file *file, void *priv, v4l2_std_id *tvnorm)
{
	struct cx88_core *core = ((struct cx8802_fh *)priv)->dev->core;

	*tvnorm = core->tvnorm;
	return 0;
}

static int vidioc_s_std(struct file *file, void *priv, v4l2_std_id id)
{
	struct cx88_core  *core = ((struct cx8802_fh *)priv)->dev->core;

	mutex_lock(&core->lock);
	cx88_set_tvnorm(core, id);
	mutex_unlock(&core->lock);
	return 0;
}

/*                                        */

static int mpeg_open(struct file *file)
{
	struct video_device *vdev = video_devdata(file);
	struct cx8802_dev *dev = video_drvdata(file);
	struct cx8802_fh *fh;
	struct cx8802_driver *drv = NULL;
	int err;

	dprintk( 1, "%s\n", __func__);

	mutex_lock(&dev->core->lock);

	/*                                       */
	drv = cx8802_get_driver(dev, CX88_MPEG_BLACKBIRD);
	if (!drv) {
		dprintk(1, "%s: blackbird driver is not loaded\n", __func__);
		mutex_unlock(&dev->core->lock);
		return -ENODEV;
	}

	err = drv->request_acquire(drv);
	if (err != 0) {
		dprintk(1,"%s: Unable to acquire hardware, %d\n", __func__, err);
		mutex_unlock(&dev->core->lock);
		return err;
	}

	if (!dev->core->mpeg_users && blackbird_initialize_codec(dev) < 0) {
		drv->request_release(drv);
		mutex_unlock(&dev->core->lock);
		return -EINVAL;
	}
	dprintk(1, "open dev=%s\n", video_device_node_name(vdev));

	/*                                           */
	fh = kzalloc(sizeof(*fh),GFP_KERNEL);
	if (NULL == fh) {
		drv->request_release(drv);
		mutex_unlock(&dev->core->lock);
		return -ENOMEM;
	}
	v4l2_fh_init(&fh->fh, vdev);
	file->private_data = fh;
	fh->dev      = dev;

	videobuf_queue_sg_init(&fh->mpegq, &blackbird_qops,
			    &dev->pci->dev, &dev->slock,
			    V4L2_BUF_TYPE_VIDEO_CAPTURE,
			    V4L2_FIELD_INTERLACED,
			    sizeof(struct cx88_buffer),
			    fh, NULL);

	/*                                           */
	cx88_set_scale(dev->core, dev->width, dev->height,
			fh->mpegq.field);

	dev->core->mpeg_users++;
	mutex_unlock(&dev->core->lock);
	v4l2_fh_add(&fh->fh);
	return 0;
}

static int mpeg_release(struct file *file)
{
	struct cx8802_fh  *fh  = file->private_data;
	struct cx8802_dev *dev = fh->dev;
	struct cx8802_driver *drv = NULL;

	mutex_lock(&dev->core->lock);

	if (dev->mpeg_active && dev->core->mpeg_users == 1)
		blackbird_stop_codec(dev);

	cx8802_cancel_buffers(fh->dev);
	/*                   */
	videobuf_stop(&fh->mpegq);

	videobuf_mmap_free(&fh->mpegq);

	v4l2_fh_del(&fh->fh);
	v4l2_fh_exit(&fh->fh);
	file->private_data = NULL;
	kfree(fh);

	/*                                   */
	drv = cx8802_get_driver(dev, CX88_MPEG_BLACKBIRD);
	WARN_ON(!drv);
	if (drv)
		drv->request_release(drv);

	dev->core->mpeg_users--;

	mutex_unlock(&dev->core->lock);

	return 0;
}

static ssize_t
mpeg_read(struct file *file, char __user *data, size_t count, loff_t *ppos)
{
	struct cx8802_fh *fh = file->private_data;
	struct cx8802_dev *dev = fh->dev;

	if (!dev->mpeg_active)
		blackbird_start_codec(file, fh);

	return videobuf_read_stream(&fh->mpegq, data, count, ppos, 0,
				    file->f_flags & O_NONBLOCK);
}

static unsigned int
mpeg_poll(struct file *file, struct poll_table_struct *wait)
{
	unsigned long req_events = poll_requested_events(wait);
	struct cx8802_fh *fh = file->private_data;
	struct cx8802_dev *dev = fh->dev;

	if (!dev->mpeg_active && (req_events & (POLLIN | POLLRDNORM)))
		blackbird_start_codec(file, fh);

	return v4l2_ctrl_poll(file, wait) | videobuf_poll_stream(file, &fh->mpegq, wait);
}

static int
mpeg_mmap(struct file *file, struct vm_area_struct * vma)
{
	struct cx8802_fh *fh = file->private_data;

	return videobuf_mmap_mapper(&fh->mpegq, vma);
}

static const struct v4l2_file_operations mpeg_fops =
{
	.owner	       = THIS_MODULE,
	.open	       = mpeg_open,
	.release       = mpeg_release,
	.read	       = mpeg_read,
	.poll          = mpeg_poll,
	.mmap	       = mpeg_mmap,
	.unlocked_ioctl = video_ioctl2,
};

static const struct v4l2_ioctl_ops mpeg_ioctl_ops = {
	.vidioc_querycap      = vidioc_querycap,
	.vidioc_enum_fmt_vid_cap  = vidioc_enum_fmt_vid_cap,
	.vidioc_g_fmt_vid_cap     = vidioc_g_fmt_vid_cap,
	.vidioc_try_fmt_vid_cap   = vidioc_try_fmt_vid_cap,
	.vidioc_s_fmt_vid_cap     = vidioc_s_fmt_vid_cap,
	.vidioc_reqbufs       = vidioc_reqbufs,
	.vidioc_querybuf      = vidioc_querybuf,
	.vidioc_qbuf          = vidioc_qbuf,
	.vidioc_dqbuf         = vidioc_dqbuf,
	.vidioc_streamon      = vidioc_streamon,
	.vidioc_streamoff     = vidioc_streamoff,
	.vidioc_s_frequency   = vidioc_s_frequency,
	.vidioc_log_status    = vidioc_log_status,
	.vidioc_enum_input    = vidioc_enum_input,
	.vidioc_g_frequency   = vidioc_g_frequency,
	.vidioc_g_input       = vidioc_g_input,
	.vidioc_s_input       = vidioc_s_input,
	.vidioc_g_tuner       = vidioc_g_tuner,
	.vidioc_s_tuner       = vidioc_s_tuner,
	.vidioc_g_std         = vidioc_g_std,
	.vidioc_s_std         = vidioc_s_std,
	.vidioc_subscribe_event      = v4l2_ctrl_subscribe_event,
	.vidioc_unsubscribe_event    = v4l2_event_unsubscribe,
};

static struct video_device cx8802_mpeg_template = {
	.name                 = "cx8802",
	.fops                 = &mpeg_fops,
	.ioctl_ops 	      = &mpeg_ioctl_ops,
	.tvnorms              = CX88_NORMS,
};

/*                                                                    */

/*                                                                 */
static int cx8802_blackbird_advise_acquire(struct cx8802_driver *drv)
{
	struct cx88_core *core = drv->core;
	int err = 0;

	switch (core->boardnr) {
	case CX88_BOARD_HAUPPAUGE_HVR1300:
		/*                                                                        
                                                              
                                                      
                                                            
   */
		/*                                            */
		cx_set(MO_GP0_IO, 0x00000080);
		udelay(1000);
		cx_clear(MO_GP0_IO, 0x00000080);
		udelay(50);
		cx_set(MO_GP0_IO, 0x00000080);
		udelay(1000);
		/*                            */
		cx_set(MO_GP0_IO, 0x00000004);
		udelay(1000);
		break;
	default:
		err = -ENODEV;
	}
	return err;
}

/*                                                                         */
static int cx8802_blackbird_advise_release(struct cx8802_driver *drv)
{
	struct cx88_core *core = drv->core;
	int err = 0;

	switch (core->boardnr) {
	case CX88_BOARD_HAUPPAUGE_HVR1300:
		/*                                     */
		break;
	default:
		err = -ENODEV;
	}
	return err;
}

static void blackbird_unregister_video(struct cx8802_dev *dev)
{
	if (dev->mpeg_dev) {
		if (video_is_registered(dev->mpeg_dev))
			video_unregister_device(dev->mpeg_dev);
		else
			video_device_release(dev->mpeg_dev);
		dev->mpeg_dev = NULL;
	}
}

static int blackbird_register_video(struct cx8802_dev *dev)
{
	int err;

	dev->mpeg_dev = cx88_vdev_init(dev->core,dev->pci,
				       &cx8802_mpeg_template,"mpeg");
	dev->mpeg_dev->ctrl_handler = &dev->cxhdl.hdl;
	video_set_drvdata(dev->mpeg_dev, dev);
	err = video_register_device(dev->mpeg_dev,VFL_TYPE_GRABBER, -1);
	if (err < 0) {
		printk(KERN_INFO "%s/2: can't register mpeg device\n",
		       dev->core->name);
		return err;
	}
	printk(KERN_INFO "%s/2: registered device %s [mpeg]\n",
	       dev->core->name, video_device_node_name(dev->mpeg_dev));
	return 0;
}

/*                                                             */

static int cx8802_blackbird_probe(struct cx8802_driver *drv)
{
	struct cx88_core *core = drv->core;
	struct cx8802_dev *dev = core->dvbdev;
	int err;

	dprintk( 1, "%s\n", __func__);
	dprintk( 1, " ->being probed by Card=%d Name=%s, PCI %02x:%02x\n",
		core->boardnr,
		core->name,
		core->pci_bus,
		core->pci_slot);

	err = -ENODEV;
	if (!(core->board.mpeg & CX88_MPEG_BLACKBIRD))
		goto fail_core;

	dev->width = 720;
	if (core->tvnorm & V4L2_STD_525_60) {
		dev->height = 480;
	} else {
		dev->height = 576;
	}
	dev->cxhdl.port = CX2341X_PORT_STREAMING;
	dev->cxhdl.width = dev->width;
	dev->cxhdl.height = dev->height;
	dev->cxhdl.func = blackbird_mbox_func;
	dev->cxhdl.priv = dev;
	err = cx2341x_handler_init(&dev->cxhdl, 36);
	if (err)
		goto fail_core;
	v4l2_ctrl_add_handler(&dev->cxhdl.hdl, &core->video_hdl, NULL);

	/*                 */
	printk("%s/2: cx23416 based mpeg encoder (blackbird reference design)\n",
	       core->name);
	host_setup(dev->core);

	blackbird_initialize_codec(dev);

	/*                                        */
//                     
	cx88_set_tvnorm(core,core->tvnorm);
	cx88_video_mux(core,0);
	cx2341x_handler_set_50hz(&dev->cxhdl, dev->height == 576);
	cx2341x_handler_setup(&dev->cxhdl);
	blackbird_register_video(dev);

	return 0;

 fail_core:
	return err;
}

static int cx8802_blackbird_remove(struct cx8802_driver *drv)
{
	struct cx88_core *core = drv->core;
	struct cx8802_dev *dev = core->dvbdev;

	/*           */
	blackbird_unregister_video(drv->core->dvbdev);
	v4l2_ctrl_handler_free(&dev->cxhdl.hdl);

	return 0;
}

static struct cx8802_driver cx8802_blackbird_driver = {
	.type_id	= CX88_MPEG_BLACKBIRD,
	.hw_access	= CX8802_DRVCTL_SHARED,
	.probe		= cx8802_blackbird_probe,
	.remove		= cx8802_blackbird_remove,
	.advise_acquire	= cx8802_blackbird_advise_acquire,
	.advise_release	= cx8802_blackbird_advise_release,
};

static int __init blackbird_init(void)
{
	printk(KERN_INFO "cx2388x blackbird driver version %s loaded\n",
	       CX88_VERSION);
	return cx8802_register_driver(&cx8802_blackbird_driver);
}

static void __exit blackbird_fini(void)
{
	cx8802_unregister_driver(&cx8802_blackbird_driver);
}

module_init(blackbird_init);
module_exit(blackbird_fini);

module_param_named(video_debug,cx8802_mpeg_template.debug, int, 0644);
MODULE_PARM_DESC(debug,"enable debug messages [video]");