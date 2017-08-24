/*
 *  The driver for the ForteMedia FM801 based soundcards
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>
 *
 *  Support FM only card by Andy Shevchenko <andy@smile.org.ua>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/tlv.h>
#include <sound/ac97_codec.h>
#include <sound/mpu401.h>
#include <sound/opl3.h>
#include <sound/initval.h>

#include <asm/io.h>

#ifdef CONFIG_SND_FM801_TEA575X_BOOL
#include <sound/tea575x-tuner.h>
#endif

MODULE_AUTHOR("Jaroslav Kysela <perex@perex.cz>");
MODULE_DESCRIPTION("ForteMedia FM801");
MODULE_LICENSE("GPL");
MODULE_SUPPORTED_DEVICE("{{ForteMedia,FM801},"
		"{Genius,SoundMaker Live 5.1}}");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/*             */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/*                  */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;	/*                  */
/*
                        
                            
                            
                           
                                                                          
                                                    
 */
static int tea575x_tuner[SNDRV_CARDS];
static int radio_nr[SNDRV_CARDS] = {[0 ... (SNDRV_CARDS - 1)] = -1};

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for the FM801 soundcard.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for the FM801 soundcard.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable FM801 soundcard.");
module_param_array(tea575x_tuner, int, NULL, 0444);
MODULE_PARM_DESC(tea575x_tuner, "TEA575x tuner access method (0 = auto, 1 = SF256-PCS, 2=SF256-PCP, 3=SF64-PCR, 8=disable, +16=tuner-only).");
module_param_array(radio_nr, int, NULL, 0444);
MODULE_PARM_DESC(radio_nr, "Radio device numbers");


#define TUNER_DISABLED		(1<<3)
#define TUNER_ONLY		(1<<4)
#define TUNER_TYPE_MASK		(~TUNER_ONLY & 0xFFFF)

/*
                    
 */

#define FM801_REG(chip, reg)	(chip->port + FM801_##reg)

#define FM801_PCM_VOL		0x00	/*                   */
#define FM801_FM_VOL		0x02	/*                  */
#define FM801_I2S_VOL		0x04	/*            */
#define FM801_REC_SRC		0x06	/*               */
#define FM801_PLY_CTRL		0x08	/*                  */
#define FM801_PLY_COUNT		0x0a	/*                */
#define FM801_PLY_BUF1		0x0c	/*                  */
#define FM801_PLY_BUF2		0x10	/*                    */
#define FM801_CAP_CTRL		0x14	/*                 */
#define FM801_CAP_COUNT		0x16	/*               */
#define FM801_CAP_BUF1		0x18	/*                  */
#define FM801_CAP_BUF2		0x1c	/*                   */
#define FM801_CODEC_CTRL	0x22	/*               */
#define FM801_I2S_MODE		0x24	/*                  */
#define FM801_VOLUME		0x26	/*                            */
#define FM801_I2C_CTRL		0x29	/*             */
#define FM801_AC97_CMD		0x2a	/*               */
#define FM801_AC97_DATA		0x2c	/*            */
#define FM801_MPU401_DATA	0x30	/*             */
#define FM801_MPU401_CMD	0x31	/*                */
#define FM801_GPIO_CTRL		0x52	/*                             */
#define FM801_GEN_CTRL		0x54	/*                 */
#define FM801_IRQ_MASK		0x56	/*                */
#define FM801_IRQ_STATUS	0x5a	/*                  */
#define FM801_OPL3_BANK0	0x68	/*                                 */
#define FM801_OPL3_DATA0	0x69	/*                   */
#define FM801_OPL3_BANK1	0x6a	/*                   */
#define FM801_OPL3_DATA1	0x6b	/*                   */
#define FM801_POWERDOWN		0x70	/*                           */

/*              */
#define FM801_AC97_READ		(1<<7)	/*                 */
#define FM801_AC97_VALID	(1<<8)	/*              */
#define FM801_AC97_BUSY		(1<<9)	/*        */
#define FM801_AC97_ADDR_SHIFT	10	/*                 */

/*                                           */
#define FM801_BUF1_LAST		(1<<1)
#define FM801_BUF2_LAST		(1<<2)
#define FM801_START		(1<<5)
#define FM801_PAUSE		(1<<6)
#define FM801_IMMED_STOP	(1<<7)
#define FM801_RATE_SHIFT	8
#define FM801_RATE_MASK		(15 << FM801_RATE_SHIFT)
#define FM801_CHANNELS_4	(1<<12)	/*               */
#define FM801_CHANNELS_6	(2<<12)	/*               */
#define FM801_CHANNELS_6MS	(3<<12)	/*               */
#define FM801_CHANNELS_MASK	(3<<12)
#define FM801_16BIT		(1<<14)
#define FM801_STEREO		(1<<15)

/*                 */
#define FM801_IRQ_PLAYBACK	(1<<8)
#define FM801_IRQ_CAPTURE	(1<<9)
#define FM801_IRQ_VOLUME	(1<<14)
#define FM801_IRQ_MPU		(1<<15)

/*                       */
#define FM801_GPIO_GP0		(1<<0)	/*            */
#define FM801_GPIO_GP1		(1<<1)
#define FM801_GPIO_GP2		(1<<2)
#define FM801_GPIO_GP3		(1<<3)
#define FM801_GPIO_GP(x)	(1<<(0+(x)))
#define FM801_GPIO_GD0		(1<<8)	/*                                  */
#define FM801_GPIO_GD1		(1<<9)
#define FM801_GPIO_GD2		(1<<10)
#define FM801_GPIO_GD3		(1<<11)
#define FM801_GPIO_GD(x)	(1<<(8+(x)))
#define FM801_GPIO_GS0		(1<<12)	/*                  */
#define FM801_GPIO_GS1		(1<<13)	/*             */
#define FM801_GPIO_GS2		(1<<14)	/*                            */
#define FM801_GPIO_GS3		(1<<15)
#define FM801_GPIO_GS(x)	(1<<(12+(x)))
	
/*

 */

struct fm801 {
	int irq;

	unsigned long port;	/*                 */
	unsigned int multichannel: 1,	/*                      */
		     secondary: 1;	/*                 */
	unsigned char secondary_addr;	/*                                */
	unsigned int tea575x_tuner;	/*                             */

	unsigned short ply_ctrl; /*                  */
	unsigned short cap_ctrl; /*                 */

	unsigned long ply_buffer;
	unsigned int ply_buf;
	unsigned int ply_count;
	unsigned int ply_size;
	unsigned int ply_pos;

	unsigned long cap_buffer;
	unsigned int cap_buf;
	unsigned int cap_count;
	unsigned int cap_size;
	unsigned int cap_pos;

	struct snd_ac97_bus *ac97_bus;
	struct snd_ac97 *ac97;
	struct snd_ac97 *ac97_sec;

	struct pci_dev *pci;
	struct snd_card *card;
	struct snd_pcm *pcm;
	struct snd_rawmidi *rmidi;
	struct snd_pcm_substream *playback_substream;
	struct snd_pcm_substream *capture_substream;
	unsigned int p_dma_size;
	unsigned int c_dma_size;

	spinlock_t reg_lock;
	struct snd_info_entry *proc_entry;

#ifdef CONFIG_SND_FM801_TEA575X_BOOL
	struct v4l2_device v4l2_dev;
	struct snd_tea575x tea;
#endif

#ifdef CONFIG_PM_SLEEP
	u16 saved_regs[0x20];
#endif
};

static DEFINE_PCI_DEVICE_TABLE(snd_fm801_ids) = {
	{ 0x1319, 0x0801, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_MULTIMEDIA_AUDIO << 8, 0xffff00, 0, },   /*       */
	{ 0x5213, 0x0510, PCI_ANY_ID, PCI_ANY_ID, PCI_CLASS_MULTIMEDIA_AUDIO << 8, 0xffff00, 0, },   /*                         */
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, snd_fm801_ids);

/*
                       
 */

static int snd_fm801_update_bits(struct fm801 *chip, unsigned short reg,
				 unsigned short mask, unsigned short value)
{
	int change;
	unsigned long flags;
	unsigned short old, new;

	spin_lock_irqsave(&chip->reg_lock, flags);
	old = inw(chip->port + reg);
	new = (old & ~mask) | value;
	change = old != new;
	if (change)
		outw(new, chip->port + reg);
	spin_unlock_irqrestore(&chip->reg_lock, flags);
	return change;
}

static void snd_fm801_codec_write(struct snd_ac97 *ac97,
				  unsigned short reg,
				  unsigned short val)
{
	struct fm801 *chip = ac97->private_data;
	int idx;

	/*
                                                  
  */
	for (idx = 0; idx < 100; idx++) {
		if (!(inw(FM801_REG(chip, AC97_CMD)) & FM801_AC97_BUSY))
			goto ok1;
		udelay(10);
	}
	snd_printk(KERN_ERR "AC'97 interface is busy (1)\n");
	return;

 ok1:
	/*                        */
	outw(val, FM801_REG(chip, AC97_DATA));
	outw(reg | (ac97->addr << FM801_AC97_ADDR_SHIFT), FM801_REG(chip, AC97_CMD));
	/*
                                                    
         */
	for (idx = 0; idx < 1000; idx++) {
		if (!(inw(FM801_REG(chip, AC97_CMD)) & FM801_AC97_BUSY))
			return;
		udelay(10);
	}
	snd_printk(KERN_ERR "AC'97 interface #%d is busy (2)\n", ac97->num);
}

static unsigned short snd_fm801_codec_read(struct snd_ac97 *ac97, unsigned short reg)
{
	struct fm801 *chip = ac97->private_data;
	int idx;

	/*
                                                  
  */
	for (idx = 0; idx < 100; idx++) {
		if (!(inw(FM801_REG(chip, AC97_CMD)) & FM801_AC97_BUSY))
			goto ok1;
		udelay(10);
	}
	snd_printk(KERN_ERR "AC'97 interface is busy (1)\n");
	return 0;

 ok1:
	/*              */
	outw(reg | (ac97->addr << FM801_AC97_ADDR_SHIFT) | FM801_AC97_READ,
	     FM801_REG(chip, AC97_CMD));
	for (idx = 0; idx < 100; idx++) {
		if (!(inw(FM801_REG(chip, AC97_CMD)) & FM801_AC97_BUSY))
			goto ok2;
		udelay(10);
	}
	snd_printk(KERN_ERR "AC'97 interface #%d is busy (2)\n", ac97->num);
	return 0;

 ok2:
	for (idx = 0; idx < 1000; idx++) {
		if (inw(FM801_REG(chip, AC97_CMD)) & FM801_AC97_VALID)
			goto ok3;
		udelay(10);
	}
	snd_printk(KERN_ERR "AC'97 interface #%d is not valid (2)\n", ac97->num);
	return 0;

 ok3:
	return inw(FM801_REG(chip, AC97_DATA));
}

static unsigned int rates[] = {
  5500,  8000,  9600, 11025,
  16000, 19200, 22050, 32000,
  38400, 44100, 48000
};

static struct snd_pcm_hw_constraint_list hw_constraints_rates = {
	.count = ARRAY_SIZE(rates),
	.list = rates,
	.mask = 0,
};

static unsigned int channels[] = {
  2, 4, 6
};

static struct snd_pcm_hw_constraint_list hw_constraints_channels = {
	.count = ARRAY_SIZE(channels),
	.list = channels,
	.mask = 0,
};

/*
                        
 */

static unsigned short snd_fm801_rate_bits(unsigned int rate)
{
	unsigned int idx;

	for (idx = 0; idx < ARRAY_SIZE(rates); idx++)
		if (rates[idx] == rate)
			return idx;
	snd_BUG();
	return ARRAY_SIZE(rates) - 1;
}

/*
            
 */

static int snd_fm801_playback_trigger(struct snd_pcm_substream *substream,
				      int cmd)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);

	spin_lock(&chip->reg_lock);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		chip->ply_ctrl &= ~(FM801_BUF1_LAST |
				     FM801_BUF2_LAST |
				     FM801_PAUSE);
		chip->ply_ctrl |= FM801_START |
				   FM801_IMMED_STOP;
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		chip->ply_ctrl &= ~(FM801_START | FM801_PAUSE);
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		chip->ply_ctrl |= FM801_PAUSE;
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	case SNDRV_PCM_TRIGGER_RESUME:
		chip->ply_ctrl &= ~FM801_PAUSE;
		break;
	default:
		spin_unlock(&chip->reg_lock);
		snd_BUG();
		return -EINVAL;
	}
	outw(chip->ply_ctrl, FM801_REG(chip, PLY_CTRL));
	spin_unlock(&chip->reg_lock);
	return 0;
}

static int snd_fm801_capture_trigger(struct snd_pcm_substream *substream,
				     int cmd)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);

	spin_lock(&chip->reg_lock);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		chip->cap_ctrl &= ~(FM801_BUF1_LAST |
				     FM801_BUF2_LAST |
				     FM801_PAUSE);
		chip->cap_ctrl |= FM801_START |
				   FM801_IMMED_STOP;
		break;
	case SNDRV_PCM_TRIGGER_STOP:
		chip->cap_ctrl &= ~(FM801_START | FM801_PAUSE);
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		chip->cap_ctrl |= FM801_PAUSE;
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
	case SNDRV_PCM_TRIGGER_RESUME:
		chip->cap_ctrl &= ~FM801_PAUSE;
		break;
	default:
		spin_unlock(&chip->reg_lock);
		snd_BUG();
		return -EINVAL;
	}
	outw(chip->cap_ctrl, FM801_REG(chip, CAP_CTRL));
	spin_unlock(&chip->reg_lock);
	return 0;
}

static int snd_fm801_hw_params(struct snd_pcm_substream *substream,
			       struct snd_pcm_hw_params *hw_params)
{
	return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
}

static int snd_fm801_hw_free(struct snd_pcm_substream *substream)
{
	return snd_pcm_lib_free_pages(substream);
}

static int snd_fm801_playback_prepare(struct snd_pcm_substream *substream)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;

	chip->ply_size = snd_pcm_lib_buffer_bytes(substream);
	chip->ply_count = snd_pcm_lib_period_bytes(substream);
	spin_lock_irq(&chip->reg_lock);
	chip->ply_ctrl &= ~(FM801_START | FM801_16BIT |
			     FM801_STEREO | FM801_RATE_MASK |
			     FM801_CHANNELS_MASK);
	if (snd_pcm_format_width(runtime->format) == 16)
		chip->ply_ctrl |= FM801_16BIT;
	if (runtime->channels > 1) {
		chip->ply_ctrl |= FM801_STEREO;
		if (runtime->channels == 4)
			chip->ply_ctrl |= FM801_CHANNELS_4;
		else if (runtime->channels == 6)
			chip->ply_ctrl |= FM801_CHANNELS_6;
	}
	chip->ply_ctrl |= snd_fm801_rate_bits(runtime->rate) << FM801_RATE_SHIFT;
	chip->ply_buf = 0;
	outw(chip->ply_ctrl, FM801_REG(chip, PLY_CTRL));
	outw(chip->ply_count - 1, FM801_REG(chip, PLY_COUNT));
	chip->ply_buffer = runtime->dma_addr;
	chip->ply_pos = 0;
	outl(chip->ply_buffer, FM801_REG(chip, PLY_BUF1));
	outl(chip->ply_buffer + (chip->ply_count % chip->ply_size), FM801_REG(chip, PLY_BUF2));
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}

static int snd_fm801_capture_prepare(struct snd_pcm_substream *substream)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;

	chip->cap_size = snd_pcm_lib_buffer_bytes(substream);
	chip->cap_count = snd_pcm_lib_period_bytes(substream);
	spin_lock_irq(&chip->reg_lock);
	chip->cap_ctrl &= ~(FM801_START | FM801_16BIT |
			     FM801_STEREO | FM801_RATE_MASK);
	if (snd_pcm_format_width(runtime->format) == 16)
		chip->cap_ctrl |= FM801_16BIT;
	if (runtime->channels > 1)
		chip->cap_ctrl |= FM801_STEREO;
	chip->cap_ctrl |= snd_fm801_rate_bits(runtime->rate) << FM801_RATE_SHIFT;
	chip->cap_buf = 0;
	outw(chip->cap_ctrl, FM801_REG(chip, CAP_CTRL));
	outw(chip->cap_count - 1, FM801_REG(chip, CAP_COUNT));
	chip->cap_buffer = runtime->dma_addr;
	chip->cap_pos = 0;
	outl(chip->cap_buffer, FM801_REG(chip, CAP_BUF1));
	outl(chip->cap_buffer + (chip->cap_count % chip->cap_size), FM801_REG(chip, CAP_BUF2));
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}

static snd_pcm_uframes_t snd_fm801_playback_pointer(struct snd_pcm_substream *substream)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);
	size_t ptr;

	if (!(chip->ply_ctrl & FM801_START))
		return 0;
	spin_lock(&chip->reg_lock);
	ptr = chip->ply_pos + (chip->ply_count - 1) - inw(FM801_REG(chip, PLY_COUNT));
	if (inw(FM801_REG(chip, IRQ_STATUS)) & FM801_IRQ_PLAYBACK) {
		ptr += chip->ply_count;
		ptr %= chip->ply_size;
	}
	spin_unlock(&chip->reg_lock);
	return bytes_to_frames(substream->runtime, ptr);
}

static snd_pcm_uframes_t snd_fm801_capture_pointer(struct snd_pcm_substream *substream)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);
	size_t ptr;

	if (!(chip->cap_ctrl & FM801_START))
		return 0;
	spin_lock(&chip->reg_lock);
	ptr = chip->cap_pos + (chip->cap_count - 1) - inw(FM801_REG(chip, CAP_COUNT));
	if (inw(FM801_REG(chip, IRQ_STATUS)) & FM801_IRQ_CAPTURE) {
		ptr += chip->cap_count;
		ptr %= chip->cap_size;
	}
	spin_unlock(&chip->reg_lock);
	return bytes_to_frames(substream->runtime, ptr);
}

static irqreturn_t snd_fm801_interrupt(int irq, void *dev_id)
{
	struct fm801 *chip = dev_id;
	unsigned short status;
	unsigned int tmp;

	status = inw(FM801_REG(chip, IRQ_STATUS));
	status &= FM801_IRQ_PLAYBACK|FM801_IRQ_CAPTURE|FM801_IRQ_MPU|FM801_IRQ_VOLUME;
	if (! status)
		return IRQ_NONE;
	/*           */
	outw(status, FM801_REG(chip, IRQ_STATUS));
	if (chip->pcm && (status & FM801_IRQ_PLAYBACK) && chip->playback_substream) {
		spin_lock(&chip->reg_lock);
		chip->ply_buf++;
		chip->ply_pos += chip->ply_count;
		chip->ply_pos %= chip->ply_size;
		tmp = chip->ply_pos + chip->ply_count;
		tmp %= chip->ply_size;
		outl(chip->ply_buffer + tmp,
				(chip->ply_buf & 1) ?
					FM801_REG(chip, PLY_BUF1) :
					FM801_REG(chip, PLY_BUF2));
		spin_unlock(&chip->reg_lock);
		snd_pcm_period_elapsed(chip->playback_substream);
	}
	if (chip->pcm && (status & FM801_IRQ_CAPTURE) && chip->capture_substream) {
		spin_lock(&chip->reg_lock);
		chip->cap_buf++;
		chip->cap_pos += chip->cap_count;
		chip->cap_pos %= chip->cap_size;
		tmp = chip->cap_pos + chip->cap_count;
		tmp %= chip->cap_size;
		outl(chip->cap_buffer + tmp,
				(chip->cap_buf & 1) ?
					FM801_REG(chip, CAP_BUF1) :
					FM801_REG(chip, CAP_BUF2));
		spin_unlock(&chip->reg_lock);
		snd_pcm_period_elapsed(chip->capture_substream);
	}
	if (chip->rmidi && (status & FM801_IRQ_MPU))
		snd_mpu401_uart_interrupt(irq, chip->rmidi->private_data);
	if (status & FM801_IRQ_VOLUME)
		;/*      */

	return IRQ_HANDLED;
}

static struct snd_pcm_hardware snd_fm801_playback =
{
	.info =			(SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED |
				 SNDRV_PCM_INFO_BLOCK_TRANSFER |
				 SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME |
				 SNDRV_PCM_INFO_MMAP_VALID),
	.formats =		SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S16_LE,
	.rates =		SNDRV_PCM_RATE_KNOT | SNDRV_PCM_RATE_8000_48000,
	.rate_min =		5500,
	.rate_max =		48000,
	.channels_min =		1,
	.channels_max =		2,
	.buffer_bytes_max =	(128*1024),
	.period_bytes_min =	64,
	.period_bytes_max =	(128*1024),
	.periods_min =		1,
	.periods_max =		1024,
	.fifo_size =		0,
};

static struct snd_pcm_hardware snd_fm801_capture =
{
	.info =			(SNDRV_PCM_INFO_MMAP | SNDRV_PCM_INFO_INTERLEAVED |
				 SNDRV_PCM_INFO_BLOCK_TRANSFER |
				 SNDRV_PCM_INFO_PAUSE | SNDRV_PCM_INFO_RESUME |
				 SNDRV_PCM_INFO_MMAP_VALID),
	.formats =		SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S16_LE,
	.rates =		SNDRV_PCM_RATE_KNOT | SNDRV_PCM_RATE_8000_48000,
	.rate_min =		5500,
	.rate_max =		48000,
	.channels_min =		1,
	.channels_max =		2,
	.buffer_bytes_max =	(128*1024),
	.period_bytes_min =	64,
	.period_bytes_max =	(128*1024),
	.periods_min =		1,
	.periods_max =		1024,
	.fifo_size =		0,
};

static int snd_fm801_playback_open(struct snd_pcm_substream *substream)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	chip->playback_substream = substream;
	runtime->hw = snd_fm801_playback;
	snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE,
				   &hw_constraints_rates);
	if (chip->multichannel) {
		runtime->hw.channels_max = 6;
		snd_pcm_hw_constraint_list(runtime, 0,
					   SNDRV_PCM_HW_PARAM_CHANNELS,
					   &hw_constraints_channels);
	}
	if ((err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS)) < 0)
		return err;
	return 0;
}

static int snd_fm801_capture_open(struct snd_pcm_substream *substream)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	int err;

	chip->capture_substream = substream;
	runtime->hw = snd_fm801_capture;
	snd_pcm_hw_constraint_list(runtime, 0, SNDRV_PCM_HW_PARAM_RATE,
				   &hw_constraints_rates);
	if ((err = snd_pcm_hw_constraint_integer(runtime, SNDRV_PCM_HW_PARAM_PERIODS)) < 0)
		return err;
	return 0;
}

static int snd_fm801_playback_close(struct snd_pcm_substream *substream)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);

	chip->playback_substream = NULL;
	return 0;
}

static int snd_fm801_capture_close(struct snd_pcm_substream *substream)
{
	struct fm801 *chip = snd_pcm_substream_chip(substream);

	chip->capture_substream = NULL;
	return 0;
}

static struct snd_pcm_ops snd_fm801_playback_ops = {
	.open =		snd_fm801_playback_open,
	.close =	snd_fm801_playback_close,
	.ioctl =	snd_pcm_lib_ioctl,
	.hw_params =	snd_fm801_hw_params,
	.hw_free =	snd_fm801_hw_free,
	.prepare =	snd_fm801_playback_prepare,
	.trigger =	snd_fm801_playback_trigger,
	.pointer =	snd_fm801_playback_pointer,
};

static struct snd_pcm_ops snd_fm801_capture_ops = {
	.open =		snd_fm801_capture_open,
	.close =	snd_fm801_capture_close,
	.ioctl =	snd_pcm_lib_ioctl,
	.hw_params =	snd_fm801_hw_params,
	.hw_free =	snd_fm801_hw_free,
	.prepare =	snd_fm801_capture_prepare,
	.trigger =	snd_fm801_capture_trigger,
	.pointer =	snd_fm801_capture_pointer,
};

static int snd_fm801_pcm(struct fm801 *chip, int device, struct snd_pcm **rpcm)
{
	struct snd_pcm *pcm;
	int err;

	if (rpcm)
		*rpcm = NULL;
	if ((err = snd_pcm_new(chip->card, "FM801", device, 1, 1, &pcm)) < 0)
		return err;

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_fm801_playback_ops);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_fm801_capture_ops);

	pcm->private_data = chip;
	pcm->info_flags = 0;
	strcpy(pcm->name, "FM801");
	chip->pcm = pcm;

	snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV,
					      snd_dma_pci_data(chip->pci),
					      chip->multichannel ? 128*1024 : 64*1024, 128*1024);

	err = snd_pcm_add_chmap_ctls(pcm, SNDRV_PCM_STREAM_PLAYBACK,
				     snd_pcm_alt_chmaps,
				     chip->multichannel ? 6 : 2, 0,
				     NULL);
	if (err < 0)
		return err;

	if (rpcm)
		*rpcm = pcm;
	return 0;
}

/*
                 
 */

#ifdef CONFIG_SND_FM801_TEA575X_BOOL

/*                      */
struct snd_fm801_tea575x_gpio {
	u8 data, clk, wren, most;
	char *name;
};

static struct snd_fm801_tea575x_gpio snd_fm801_tea575x_gpios[] = {
	{ .data = 1, .clk = 3, .wren = 2, .most = 0, .name = "SF256-PCS" },
	{ .data = 1, .clk = 0, .wren = 2, .most = 3, .name = "SF256-PCP" },
	{ .data = 2, .clk = 0, .wren = 1, .most = 3, .name = "SF64-PCR" },
};

#define get_tea575x_gpio(chip) \
	(&snd_fm801_tea575x_gpios[((chip)->tea575x_tuner & TUNER_TYPE_MASK) - 1])

static void snd_fm801_tea575x_set_pins(struct snd_tea575x *tea, u8 pins)
{
	struct fm801 *chip = tea->private_data;
	unsigned short reg = inw(FM801_REG(chip, GPIO_CTRL));
	struct snd_fm801_tea575x_gpio gpio = *get_tea575x_gpio(chip);

	reg &= ~(FM801_GPIO_GP(gpio.data) |
		 FM801_GPIO_GP(gpio.clk) |
		 FM801_GPIO_GP(gpio.wren));

	reg |= (pins & TEA575X_DATA) ? FM801_GPIO_GP(gpio.data) : 0;
	reg |= (pins & TEA575X_CLK)  ? FM801_GPIO_GP(gpio.clk) : 0;
	/*                          */
	reg |= (pins & TEA575X_WREN) ? 0 : FM801_GPIO_GP(gpio.wren);

	outw(reg, FM801_REG(chip, GPIO_CTRL));
}

static u8 snd_fm801_tea575x_get_pins(struct snd_tea575x *tea)
{
	struct fm801 *chip = tea->private_data;
	unsigned short reg = inw(FM801_REG(chip, GPIO_CTRL));
	struct snd_fm801_tea575x_gpio gpio = *get_tea575x_gpio(chip);
	u8 ret;

	ret = 0;
	if (reg & FM801_GPIO_GP(gpio.data))
		ret |= TEA575X_DATA;
	if (reg & FM801_GPIO_GP(gpio.most))
		ret |= TEA575X_MOST;
	return ret;
}

static void snd_fm801_tea575x_set_direction(struct snd_tea575x *tea, bool output)
{
	struct fm801 *chip = tea->private_data;
	unsigned short reg = inw(FM801_REG(chip, GPIO_CTRL));
	struct snd_fm801_tea575x_gpio gpio = *get_tea575x_gpio(chip);

	/*                                         */
	reg |= FM801_GPIO_GS(gpio.data) |
	       FM801_GPIO_GS(gpio.wren) |
	       FM801_GPIO_GS(gpio.clk) |
	       FM801_GPIO_GS(gpio.most);
	if (output) {
		/*                                         */
		/*                            */
		reg &= ~(FM801_GPIO_GD(gpio.data) |
			 FM801_GPIO_GD(gpio.wren) |
			 FM801_GPIO_GD(gpio.clk) |
			 FM801_GPIO_GP(gpio.data) |
			 FM801_GPIO_GP(gpio.clk) |
			 FM801_GPIO_GP(gpio.wren));
	} else {
		/*                                             */
		reg |= FM801_GPIO_GD(gpio.data) |
		       FM801_GPIO_GD(gpio.most) |
		       FM801_GPIO_GP(gpio.data) |
		       FM801_GPIO_GP(gpio.most) |
		       FM801_GPIO_GP(gpio.wren);
		/*                                                      */
		/*                                          */
		reg &= ~(FM801_GPIO_GD(gpio.wren) |
			 FM801_GPIO_GD(gpio.clk) |
			 FM801_GPIO_GP(gpio.clk));
	}

	outw(reg, FM801_REG(chip, GPIO_CTRL));
}

static struct snd_tea575x_ops snd_fm801_tea_ops = {
	.set_pins = snd_fm801_tea575x_set_pins,
	.get_pins = snd_fm801_tea575x_get_pins,
	.set_direction = snd_fm801_tea575x_set_direction,
};
#endif

/*
                  
 */

#define FM801_SINGLE(xname, reg, shift, mask, invert) \
{ .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, .info = snd_fm801_info_single, \
  .get = snd_fm801_get_single, .put = snd_fm801_put_single, \
  .private_value = reg | (shift << 8) | (mask << 16) | (invert << 24) }

static int snd_fm801_info_single(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_info *uinfo)
{
	int mask = (kcontrol->private_value >> 16) & 0xff;

	uinfo->type = mask == 1 ? SNDRV_CTL_ELEM_TYPE_BOOLEAN : SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 1;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = mask;
	return 0;
}

static int snd_fm801_get_single(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct fm801 *chip = snd_kcontrol_chip(kcontrol);
	int reg = kcontrol->private_value & 0xff;
	int shift = (kcontrol->private_value >> 8) & 0xff;
	int mask = (kcontrol->private_value >> 16) & 0xff;
	int invert = (kcontrol->private_value >> 24) & 0xff;

	ucontrol->value.integer.value[0] = (inw(chip->port + reg) >> shift) & mask;
	if (invert)
		ucontrol->value.integer.value[0] = mask - ucontrol->value.integer.value[0];
	return 0;
}

static int snd_fm801_put_single(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct fm801 *chip = snd_kcontrol_chip(kcontrol);
	int reg = kcontrol->private_value & 0xff;
	int shift = (kcontrol->private_value >> 8) & 0xff;
	int mask = (kcontrol->private_value >> 16) & 0xff;
	int invert = (kcontrol->private_value >> 24) & 0xff;
	unsigned short val;

	val = (ucontrol->value.integer.value[0] & mask);
	if (invert)
		val = mask - val;
	return snd_fm801_update_bits(chip, reg, mask << shift, val << shift);
}

#define FM801_DOUBLE(xname, reg, shift_left, shift_right, mask, invert) \
{ .iface = SNDRV_CTL_ELEM_IFACE_MIXER, .name = xname, .info = snd_fm801_info_double, \
  .get = snd_fm801_get_double, .put = snd_fm801_put_double, \
  .private_value = reg | (shift_left << 8) | (shift_right << 12) | (mask << 16) | (invert << 24) }
#define FM801_DOUBLE_TLV(xname, reg, shift_left, shift_right, mask, invert, xtlv) \
{ .iface = SNDRV_CTL_ELEM_IFACE_MIXER, \
  .access = SNDRV_CTL_ELEM_ACCESS_READWRITE | SNDRV_CTL_ELEM_ACCESS_TLV_READ, \
  .name = xname, .info = snd_fm801_info_double, \
  .get = snd_fm801_get_double, .put = snd_fm801_put_double, \
  .private_value = reg | (shift_left << 8) | (shift_right << 12) | (mask << 16) | (invert << 24), \
  .tlv = { .p = (xtlv) } }

static int snd_fm801_info_double(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_info *uinfo)
{
	int mask = (kcontrol->private_value >> 16) & 0xff;

	uinfo->type = mask == 1 ? SNDRV_CTL_ELEM_TYPE_BOOLEAN : SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = mask;
	return 0;
}

static int snd_fm801_get_double(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct fm801 *chip = snd_kcontrol_chip(kcontrol);
        int reg = kcontrol->private_value & 0xff;
	int shift_left = (kcontrol->private_value >> 8) & 0x0f;
	int shift_right = (kcontrol->private_value >> 12) & 0x0f;
	int mask = (kcontrol->private_value >> 16) & 0xff;
	int invert = (kcontrol->private_value >> 24) & 0xff;

	spin_lock_irq(&chip->reg_lock);
	ucontrol->value.integer.value[0] = (inw(chip->port + reg) >> shift_left) & mask;
	ucontrol->value.integer.value[1] = (inw(chip->port + reg) >> shift_right) & mask;
	spin_unlock_irq(&chip->reg_lock);
	if (invert) {
		ucontrol->value.integer.value[0] = mask - ucontrol->value.integer.value[0];
		ucontrol->value.integer.value[1] = mask - ucontrol->value.integer.value[1];
	}
	return 0;
}

static int snd_fm801_put_double(struct snd_kcontrol *kcontrol,
				struct snd_ctl_elem_value *ucontrol)
{
	struct fm801 *chip = snd_kcontrol_chip(kcontrol);
	int reg = kcontrol->private_value & 0xff;
	int shift_left = (kcontrol->private_value >> 8) & 0x0f;
	int shift_right = (kcontrol->private_value >> 12) & 0x0f;
	int mask = (kcontrol->private_value >> 16) & 0xff;
	int invert = (kcontrol->private_value >> 24) & 0xff;
	unsigned short val1, val2;
 
	val1 = ucontrol->value.integer.value[0] & mask;
	val2 = ucontrol->value.integer.value[1] & mask;
	if (invert) {
		val1 = mask - val1;
		val2 = mask - val2;
	}
	return snd_fm801_update_bits(chip, reg,
				     (mask << shift_left) | (mask << shift_right),
				     (val1 << shift_left ) | (val2 << shift_right));
}

static int snd_fm801_info_mux(struct snd_kcontrol *kcontrol,
			      struct snd_ctl_elem_info *uinfo)
{
	static char *texts[5] = {
		"AC97 Primary", "FM", "I2S", "PCM", "AC97 Secondary"
	};
 
	uinfo->type = SNDRV_CTL_ELEM_TYPE_ENUMERATED;
	uinfo->count = 1;
	uinfo->value.enumerated.items = 5;
	if (uinfo->value.enumerated.item > 4)
		uinfo->value.enumerated.item = 4;
	strcpy(uinfo->value.enumerated.name, texts[uinfo->value.enumerated.item]);
	return 0;
}

static int snd_fm801_get_mux(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct fm801 *chip = snd_kcontrol_chip(kcontrol);
        unsigned short val;
 
	val = inw(FM801_REG(chip, REC_SRC)) & 7;
	if (val > 4)
		val = 4;
        ucontrol->value.enumerated.item[0] = val;
        return 0;
}

static int snd_fm801_put_mux(struct snd_kcontrol *kcontrol,
			     struct snd_ctl_elem_value *ucontrol)
{
	struct fm801 *chip = snd_kcontrol_chip(kcontrol);
        unsigned short val;
 
        if ((val = ucontrol->value.enumerated.item[0]) > 4)
                return -EINVAL;
	return snd_fm801_update_bits(chip, FM801_REC_SRC, 7, val);
}

static const DECLARE_TLV_DB_SCALE(db_scale_dsp, -3450, 150, 0);

#define FM801_CONTROLS ARRAY_SIZE(snd_fm801_controls)

static struct snd_kcontrol_new snd_fm801_controls[] = {
FM801_DOUBLE_TLV("Wave Playback Volume", FM801_PCM_VOL, 0, 8, 31, 1,
		 db_scale_dsp),
FM801_SINGLE("Wave Playback Switch", FM801_PCM_VOL, 15, 1, 1),
FM801_DOUBLE_TLV("I2S Playback Volume", FM801_I2S_VOL, 0, 8, 31, 1,
		 db_scale_dsp),
FM801_SINGLE("I2S Playback Switch", FM801_I2S_VOL, 15, 1, 1),
FM801_DOUBLE_TLV("FM Playback Volume", FM801_FM_VOL, 0, 8, 31, 1,
		 db_scale_dsp),
FM801_SINGLE("FM Playback Switch", FM801_FM_VOL, 15, 1, 1),
{
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "Digital Capture Source",
	.info = snd_fm801_info_mux,
	.get = snd_fm801_get_mux,
	.put = snd_fm801_put_mux,
}
};

#define FM801_CONTROLS_MULTI ARRAY_SIZE(snd_fm801_controls_multi)

static struct snd_kcontrol_new snd_fm801_controls_multi[] = {
FM801_SINGLE("AC97 2ch->4ch Copy Switch", FM801_CODEC_CTRL, 7, 1, 0),
FM801_SINGLE("AC97 18-bit Switch", FM801_CODEC_CTRL, 10, 1, 0),
FM801_SINGLE(SNDRV_CTL_NAME_IEC958("",CAPTURE,SWITCH), FM801_I2S_MODE, 8, 1, 0),
FM801_SINGLE(SNDRV_CTL_NAME_IEC958("Raw Data ",PLAYBACK,SWITCH), FM801_I2S_MODE, 9, 1, 0),
FM801_SINGLE(SNDRV_CTL_NAME_IEC958("Raw Data ",CAPTURE,SWITCH), FM801_I2S_MODE, 10, 1, 0),
FM801_SINGLE(SNDRV_CTL_NAME_IEC958("",PLAYBACK,SWITCH), FM801_GEN_CTRL, 2, 1, 0),
};

static void snd_fm801_mixer_free_ac97_bus(struct snd_ac97_bus *bus)
{
	struct fm801 *chip = bus->private_data;
	chip->ac97_bus = NULL;
}

static void snd_fm801_mixer_free_ac97(struct snd_ac97 *ac97)
{
	struct fm801 *chip = ac97->private_data;
	if (ac97->num == 0) {
		chip->ac97 = NULL;
	} else {
		chip->ac97_sec = NULL;
	}
}

static int snd_fm801_mixer(struct fm801 *chip)
{
	struct snd_ac97_template ac97;
	unsigned int i;
	int err;
	static struct snd_ac97_bus_ops ops = {
		.write = snd_fm801_codec_write,
		.read = snd_fm801_codec_read,
	};

	if ((err = snd_ac97_bus(chip->card, 0, &ops, chip, &chip->ac97_bus)) < 0)
		return err;
	chip->ac97_bus->private_free = snd_fm801_mixer_free_ac97_bus;

	memset(&ac97, 0, sizeof(ac97));
	ac97.private_data = chip;
	ac97.private_free = snd_fm801_mixer_free_ac97;
	if ((err = snd_ac97_mixer(chip->ac97_bus, &ac97, &chip->ac97)) < 0)
		return err;
	if (chip->secondary) {
		ac97.num = 1;
		ac97.addr = chip->secondary_addr;
		if ((err = snd_ac97_mixer(chip->ac97_bus, &ac97, &chip->ac97_sec)) < 0)
			return err;
	}
	for (i = 0; i < FM801_CONTROLS; i++)
		snd_ctl_add(chip->card, snd_ctl_new1(&snd_fm801_controls[i], chip));
	if (chip->multichannel) {
		for (i = 0; i < FM801_CONTROLS_MULTI; i++)
			snd_ctl_add(chip->card, snd_ctl_new1(&snd_fm801_controls_multi[i], chip));
	}
	return 0;
}

/*
                           
 */

static int wait_for_codec(struct fm801 *chip, unsigned int codec_id,
			  unsigned short reg, unsigned long waits)
{
	unsigned long timeout = jiffies + waits;

	outw(FM801_AC97_READ | (codec_id << FM801_AC97_ADDR_SHIFT) | reg,
	     FM801_REG(chip, AC97_CMD));
	udelay(5);
	do {
		if ((inw(FM801_REG(chip, AC97_CMD)) & (FM801_AC97_VALID|FM801_AC97_BUSY))
		    == FM801_AC97_VALID)
			return 0;
		schedule_timeout_uninterruptible(1);
	} while (time_after(timeout, jiffies));
	return -EIO;
}

static int snd_fm801_chip_init(struct fm801 *chip, int resume)
{
	unsigned short cmdw;

	if (chip->tea575x_tuner & TUNER_ONLY)
		goto __ac97_ok;

	/*                                     */
	outw((1<<5) | (1<<6), FM801_REG(chip, CODEC_CTRL));
	inw(FM801_REG(chip, CODEC_CTRL)); /*                    */
	udelay(100);
	outw(0, FM801_REG(chip, CODEC_CTRL));

	if (wait_for_codec(chip, 0, AC97_RESET, msecs_to_jiffies(750)) < 0)
		if (!resume) {
			snd_printk(KERN_INFO "Primary AC'97 codec not found, "
					    "assume SF64-PCR (tuner-only)\n");
			chip->tea575x_tuner = 3 | TUNER_ONLY;
			goto __ac97_ok;
		}

	if (chip->multichannel) {
		if (chip->secondary_addr) {
			wait_for_codec(chip, chip->secondary_addr,
				       AC97_VENDOR_ID1, msecs_to_jiffies(50));
		} else {
			/*                                 */
			/*                                        */
			int i;
			for (i = 3; i > 0; i--) {
				if (!wait_for_codec(chip, i, AC97_VENDOR_ID1,
						     msecs_to_jiffies(50))) {
					cmdw = inw(FM801_REG(chip, AC97_DATA));
					if (cmdw != 0xffff && cmdw != 0) {
						chip->secondary = 1;
						chip->secondary_addr = i;
						break;
					}
				}
			}
		}

		/*                                                                        */
		/*                        */
		wait_for_codec(chip, 0, AC97_VENDOR_ID1, msecs_to_jiffies(750));
	}

      __ac97_ok:

	/*             */
	outw(0x0808, FM801_REG(chip, PCM_VOL));
	outw(0x9f1f, FM801_REG(chip, FM_VOL));
	outw(0x8808, FM801_REG(chip, I2S_VOL));

	/*                        */
	outw(0x0003, FM801_REG(chip, I2S_MODE));

	/*                 */
	cmdw = inw(FM801_REG(chip, IRQ_MASK));
	if (chip->irq < 0)
		cmdw |= 0x00c3;		/*                                 */
	else
		cmdw &= ~0x0083;	/*                                */
	outw(cmdw, FM801_REG(chip, IRQ_MASK));

	/*                 */
	outw(FM801_IRQ_PLAYBACK|FM801_IRQ_CAPTURE|FM801_IRQ_MPU, FM801_REG(chip, IRQ_STATUS));

	return 0;
}


static int snd_fm801_free(struct fm801 *chip)
{
	unsigned short cmdw;

	if (chip->irq < 0)
		goto __end_hw;

	/*                                   */
	cmdw = inw(FM801_REG(chip, IRQ_MASK));
	cmdw |= 0x00c3;
	outw(cmdw, FM801_REG(chip, IRQ_MASK));

      __end_hw:
#ifdef CONFIG_SND_FM801_TEA575X_BOOL
	if (!(chip->tea575x_tuner & TUNER_DISABLED)) {
		snd_tea575x_exit(&chip->tea);
		v4l2_device_unregister(&chip->v4l2_dev);
	}
#endif
	if (chip->irq >= 0)
		free_irq(chip->irq, chip);
	pci_release_regions(chip->pci);
	pci_disable_device(chip->pci);

	kfree(chip);
	return 0;
}

static int snd_fm801_dev_free(struct snd_device *device)
{
	struct fm801 *chip = device->device_data;
	return snd_fm801_free(chip);
}

static int snd_fm801_create(struct snd_card *card,
			    struct pci_dev *pci,
			    int tea575x_tuner,
			    int radio_nr,
			    struct fm801 **rchip)
{
	struct fm801 *chip;
	int err;
	static struct snd_device_ops ops = {
		.dev_free =	snd_fm801_dev_free,
	};

	*rchip = NULL;
	if ((err = pci_enable_device(pci)) < 0)
		return err;
	chip = kzalloc(sizeof(*chip), GFP_KERNEL);
	if (chip == NULL) {
		pci_disable_device(pci);
		return -ENOMEM;
	}
	spin_lock_init(&chip->reg_lock);
	chip->card = card;
	chip->pci = pci;
	chip->irq = -1;
	chip->tea575x_tuner = tea575x_tuner;
	if ((err = pci_request_regions(pci, "FM801")) < 0) {
		kfree(chip);
		pci_disable_device(pci);
		return err;
	}
	chip->port = pci_resource_start(pci, 0);
	if ((tea575x_tuner & TUNER_ONLY) == 0) {
		if (request_irq(pci->irq, snd_fm801_interrupt, IRQF_SHARED,
				KBUILD_MODNAME, chip)) {
			snd_printk(KERN_ERR "unable to grab IRQ %d\n", chip->irq);
			snd_fm801_free(chip);
			return -EBUSY;
		}
		chip->irq = pci->irq;
		pci_set_master(pci);
	}

	if (pci->revision >= 0xb1)	/*          */
		chip->multichannel = 1;

	snd_fm801_chip_init(chip, 0);
	/*                                    */
	tea575x_tuner = chip->tea575x_tuner;

	if (chip->irq >= 0 && (tea575x_tuner & TUNER_ONLY)) {
		pci_clear_master(pci);
		free_irq(chip->irq, chip);
		chip->irq = -1;
	}

	if ((err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops)) < 0) {
		snd_fm801_free(chip);
		return err;
	}

	snd_card_set_dev(card, &pci->dev);

#ifdef CONFIG_SND_FM801_TEA575X_BOOL
	err = v4l2_device_register(&pci->dev, &chip->v4l2_dev);
	if (err < 0) {
		snd_fm801_free(chip);
		return err;
	}
	chip->tea.v4l2_dev = &chip->v4l2_dev;
	chip->tea.radio_nr = radio_nr;
	chip->tea.private_data = chip;
	chip->tea.ops = &snd_fm801_tea_ops;
	sprintf(chip->tea.bus_info, "PCI:%s", pci_name(pci));
	if ((tea575x_tuner & TUNER_TYPE_MASK) > 0 &&
	    (tea575x_tuner & TUNER_TYPE_MASK) < 4) {
		if (snd_tea575x_init(&chip->tea, THIS_MODULE)) {
			snd_printk(KERN_ERR "TEA575x radio not found\n");
			snd_fm801_free(chip);
			return -ENODEV;
		}
	} else if ((tea575x_tuner & TUNER_TYPE_MASK) == 0) {
		/*                             */
		for (tea575x_tuner = 1; tea575x_tuner <= 3; tea575x_tuner++) {
			chip->tea575x_tuner = tea575x_tuner;
			if (!snd_tea575x_init(&chip->tea, THIS_MODULE)) {
				snd_printk(KERN_INFO "detected TEA575x radio type %s\n",
					   get_tea575x_gpio(chip)->name);
				break;
			}
		}
		if (tea575x_tuner == 4) {
			snd_printk(KERN_ERR "TEA575x radio not found\n");
			chip->tea575x_tuner = TUNER_DISABLED;
		}
	}
	if (!(chip->tea575x_tuner & TUNER_DISABLED)) {
		strlcpy(chip->tea.card, get_tea575x_gpio(chip)->name,
			sizeof(chip->tea.card));
	}
#endif

	*rchip = chip;
	return 0;
}

static int snd_card_fm801_probe(struct pci_dev *pci,
				const struct pci_device_id *pci_id)
{
	static int dev;
	struct snd_card *card;
	struct fm801 *chip;
	struct snd_opl3 *opl3;
	int err;

        if (dev >= SNDRV_CARDS)
                return -ENODEV;
	if (!enable[dev]) {
		dev++;
		return -ENOENT;
	}

	err = snd_card_create(index[dev], id[dev], THIS_MODULE, 0, &card);
	if (err < 0)
		return err;
	if ((err = snd_fm801_create(card, pci, tea575x_tuner[dev], radio_nr[dev], &chip)) < 0) {
		snd_card_free(card);
		return err;
	}
	card->private_data = chip;

	strcpy(card->driver, "FM801");
	strcpy(card->shortname, "ForteMedia FM801-");
	strcat(card->shortname, chip->multichannel ? "AU" : "AS");
	sprintf(card->longname, "%s at 0x%lx, irq %i",
		card->shortname, chip->port, chip->irq);

	if (chip->tea575x_tuner & TUNER_ONLY)
		goto __fm801_tuner_only;

	if ((err = snd_fm801_pcm(chip, 0, NULL)) < 0) {
		snd_card_free(card);
		return err;
	}
	if ((err = snd_fm801_mixer(chip)) < 0) {
		snd_card_free(card);
		return err;
	}
	if ((err = snd_mpu401_uart_new(card, 0, MPU401_HW_FM801,
				       FM801_REG(chip, MPU401_DATA),
				       MPU401_INFO_INTEGRATED |
				       MPU401_INFO_IRQ_HOOK,
				       -1, &chip->rmidi)) < 0) {
		snd_card_free(card);
		return err;
	}
	if ((err = snd_opl3_create(card, FM801_REG(chip, OPL3_BANK0),
				   FM801_REG(chip, OPL3_BANK1),
				   OPL3_HW_OPL3_FM801, 1, &opl3)) < 0) {
		snd_card_free(card);
		return err;
	}
	if ((err = snd_opl3_hwdep_new(opl3, 0, 1, NULL)) < 0) {
		snd_card_free(card);
		return err;
	}

      __fm801_tuner_only:
	if ((err = snd_card_register(card)) < 0) {
		snd_card_free(card);
		return err;
	}
	pci_set_drvdata(pci, card);
	dev++;
	return 0;
}

static void snd_card_fm801_remove(struct pci_dev *pci)
{
	snd_card_free(pci_get_drvdata(pci));
	pci_set_drvdata(pci, NULL);
}

#ifdef CONFIG_PM_SLEEP
static unsigned char saved_regs[] = {
	FM801_PCM_VOL, FM801_I2S_VOL, FM801_FM_VOL, FM801_REC_SRC,
	FM801_PLY_CTRL, FM801_PLY_COUNT, FM801_PLY_BUF1, FM801_PLY_BUF2,
	FM801_CAP_CTRL, FM801_CAP_COUNT, FM801_CAP_BUF1, FM801_CAP_BUF2,
	FM801_CODEC_CTRL, FM801_I2S_MODE, FM801_VOLUME, FM801_GEN_CTRL,
};

static int snd_fm801_suspend(struct device *dev)
{
	struct pci_dev *pci = to_pci_dev(dev);
	struct snd_card *card = dev_get_drvdata(dev);
	struct fm801 *chip = card->private_data;
	int i;

	snd_power_change_state(card, SNDRV_CTL_POWER_D3hot);
	snd_pcm_suspend_all(chip->pcm);
	snd_ac97_suspend(chip->ac97);
	snd_ac97_suspend(chip->ac97_sec);
	for (i = 0; i < ARRAY_SIZE(saved_regs); i++)
		chip->saved_regs[i] = inw(chip->port + saved_regs[i]);
	/*                        */

	pci_disable_device(pci);
	pci_save_state(pci);
	pci_set_power_state(pci, PCI_D3hot);
	return 0;
}

static int snd_fm801_resume(struct device *dev)
{
	struct pci_dev *pci = to_pci_dev(dev);
	struct snd_card *card = dev_get_drvdata(dev);
	struct fm801 *chip = card->private_data;
	int i;

	pci_set_power_state(pci, PCI_D0);
	pci_restore_state(pci);
	if (pci_enable_device(pci) < 0) {
		printk(KERN_ERR "fm801: pci_enable_device failed, "
		       "disabling device\n");
		snd_card_disconnect(card);
		return -EIO;
	}
	pci_set_master(pci);

	snd_fm801_chip_init(chip, 1);
	snd_ac97_resume(chip->ac97);
	snd_ac97_resume(chip->ac97_sec);
	for (i = 0; i < ARRAY_SIZE(saved_regs); i++)
		outw(chip->saved_regs[i], chip->port + saved_regs[i]);

	snd_power_change_state(card, SNDRV_CTL_POWER_D0);
	return 0;
}

static SIMPLE_DEV_PM_OPS(snd_fm801_pm, snd_fm801_suspend, snd_fm801_resume);
#define SND_FM801_PM_OPS	&snd_fm801_pm
#else
#define SND_FM801_PM_OPS	NULL
#endif /*                 */

static struct pci_driver fm801_driver = {
	.name = KBUILD_MODNAME,
	.id_table = snd_fm801_ids,
	.probe = snd_card_fm801_probe,
	.remove = snd_card_fm801_remove,
	.driver = {
		.pm = SND_FM801_PM_OPS,
	},
};

module_pci_driver(fm801_driver);