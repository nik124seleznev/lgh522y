/*
 *  Driver for Cirrus Logic CS4281 based PCI soundcard
 *  Copyright (c) by Jaroslav Kysela <perex@perex.cz>,
 *
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

#include <asm/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/gameport.h>
#include <linux/module.h>
#include <sound/core.h>
#include <sound/control.h>
#include <sound/pcm.h>
#include <sound/rawmidi.h>
#include <sound/ac97_codec.h>
#include <sound/tlv.h>
#include <sound/opl3.h>
#include <sound/initval.h>


MODULE_AUTHOR("Jaroslav Kysela <perex@perex.cz>");
MODULE_DESCRIPTION("Cirrus Logic CS4281");
MODULE_LICENSE("GPL");
MODULE_SUPPORTED_DEVICE("{{Cirrus Logic,CS4281}}");

static int index[SNDRV_CARDS] = SNDRV_DEFAULT_IDX;	/*             */
static char *id[SNDRV_CARDS] = SNDRV_DEFAULT_STR;	/*                  */
static bool enable[SNDRV_CARDS] = SNDRV_DEFAULT_ENABLE_PNP;	/*                 */
static bool dual_codec[SNDRV_CARDS];	/*            */

module_param_array(index, int, NULL, 0444);
MODULE_PARM_DESC(index, "Index value for CS4281 soundcard.");
module_param_array(id, charp, NULL, 0444);
MODULE_PARM_DESC(id, "ID string for CS4281 soundcard.");
module_param_array(enable, bool, NULL, 0444);
MODULE_PARM_DESC(enable, "Enable CS4281 soundcard.");
module_param_array(dual_codec, bool, NULL, 0444);
MODULE_PARM_DESC(dual_codec, "Secondary Codec ID (0 = disabled).");

/*
                    
 */

#define CS4281_BA0_SIZE		0x1000
#define CS4281_BA1_SIZE		0x10000

/*
                 
 */
#define BA0_HISR		0x0000	/*                                */
#define BA0_HISR_INTENA		(1<<31)	/*                               */
#define BA0_HISR_MIDI		(1<<22)	/*                     */
#define BA0_HISR_FIFOI		(1<<20)	/*                       */
#define BA0_HISR_DMAI		(1<<18)	/*                             */
#define BA0_HISR_FIFO(c)	(1<<(12+(c))) /*                        */
#define BA0_HISR_DMA(c)		(1<<(8+(c)))  /*                       */
#define BA0_HISR_GPPI		(1<<5)	/*                                      */
#define BA0_HISR_GPSI		(1<<4)	/*                                        */
#define BA0_HISR_GP3I		(1<<3)	/*                     */
#define BA0_HISR_GP1I		(1<<2)	/*                     */
#define BA0_HISR_VUPI		(1<<1)	/*                     */
#define BA0_HISR_VDNI		(1<<0)	/*                     */

#define BA0_HICR		0x0008	/*                                 */
#define BA0_HICR_CHGM		(1<<1)	/*                    */
#define BA0_HICR_IEV		(1<<0)	/*              */
#define BA0_HICR_EOI		(3<<0)	/*                          */

#define BA0_HIMR		0x000c	/*                              */
					/*                                   */

#define BA0_IIER		0x0010	/*                               */

#define BA0_HDSR0		0x00f0	/*                                   */
#define BA0_HDSR1		0x00f4	/*                                   */
#define BA0_HDSR2		0x00f8	/*                                   */
#define BA0_HDSR3		0x00fc	/*                                   */

#define BA0_HDSR_CH1P		(1<<25)	/*                   */
#define BA0_HDSR_CH2P		(1<<24)	/*                   */
#define BA0_HDSR_DHTC		(1<<17)	/*                         */
#define BA0_HDSR_DTC		(1<<16)	/*                    */
#define BA0_HDSR_DRUN		(1<<15)	/*             */
#define BA0_HDSR_RQ		(1<<7)	/*                 */

#define BA0_DCA0		0x0110	/*                                   */
#define BA0_DCC0		0x0114	/*                                 */
#define BA0_DBA0		0x0118	/*                                */
#define BA0_DBC0		0x011c	/*                              */
#define BA0_DCA1		0x0120	/*                                   */
#define BA0_DCC1		0x0124	/*                                 */
#define BA0_DBA1		0x0128	/*                                */
#define BA0_DBC1		0x012c	/*                              */
#define BA0_DCA2		0x0130	/*                                   */
#define BA0_DCC2		0x0134	/*                                 */
#define BA0_DBA2		0x0138	/*                                */
#define BA0_DBC2		0x013c	/*                              */
#define BA0_DCA3		0x0140	/*                                   */
#define BA0_DCC3		0x0144	/*                                 */
#define BA0_DBA3		0x0148	/*                                */
#define BA0_DBC3		0x014c	/*                              */
#define BA0_DMR0		0x0150	/*                        */
#define BA0_DCR0		0x0154	/*                           */
#define BA0_DMR1		0x0158	/*                        */
#define BA0_DCR1		0x015c	/*                           */
#define BA0_DMR2		0x0160	/*                        */
#define BA0_DCR2		0x0164	/*                           */
#define BA0_DMR3		0x0168	/*                        */
#define BA0_DCR3		0x016c	/*                           */

#define BA0_DMR_DMA		(1<<29)	/*                 */
#define BA0_DMR_POLL		(1<<28)	/*                  */
#define BA0_DMR_TBC		(1<<25)	/*                     */
#define BA0_DMR_CBC		(1<<24)	/*                                         */
#define BA0_DMR_SWAPC		(1<<22)	/*                          */
#define BA0_DMR_SIZE20		(1<<20)	/*                  */
#define BA0_DMR_USIGN		(1<<19)	/*          */
#define BA0_DMR_BEND		(1<<18)	/*            */
#define BA0_DMR_MONO		(1<<17)	/*      */
#define BA0_DMR_SIZE8		(1<<16)	/*                 */
#define BA0_DMR_TYPE_DEMAND	(0<<6)
#define BA0_DMR_TYPE_SINGLE	(1<<6)
#define BA0_DMR_TYPE_BLOCK	(2<<6)
#define BA0_DMR_TYPE_CASCADE	(3<<6)	/*               */
#define BA0_DMR_DEC		(1<<5)	/*                                       */
#define BA0_DMR_AUTO		(1<<4)	/*                 */
#define BA0_DMR_TR_VERIFY	(0<<2)	/*                 */
#define BA0_DMR_TR_WRITE	(1<<2)	/*                */
#define BA0_DMR_TR_READ		(2<<2)	/*               */

#define BA0_DCR_HTCIE		(1<<17)	/*                               */
#define BA0_DCR_TCIE		(1<<16)	/*                          */
#define BA0_DCR_MSK		(1<<0)	/*              */

#define BA0_FCR0		0x0180	/*                */
#define BA0_FCR1		0x0184	/*                */
#define BA0_FCR2		0x0188	/*                */
#define BA0_FCR3		0x018c	/*                */

#define BA0_FCR_FEN		(1<<31)	/*                 */
#define BA0_FCR_DACZ		(1<<30)	/*          */
#define BA0_FCR_PSH		(1<<29)	/*                      */
#define BA0_FCR_RS(x)		(((x)&0x1f)<<24) /*                    */
#define BA0_FCR_LS(x)		(((x)&0x1f)<<16) /*                   */
#define BA0_FCR_SZ(x)		(((x)&0x7f)<<8)	/*                               */
#define BA0_FCR_OF(x)		(((x)&0x7f)<<0)	/*                                   */

#define BA0_FPDR0		0x0190	/*                    */
#define BA0_FPDR1		0x0194	/*                    */
#define BA0_FPDR2		0x0198	/*                    */
#define BA0_FPDR3		0x019c	/*                    */

#define BA0_FCHS		0x020c	/*                     */
#define BA0_FCHS_RCO(x)		(1<<(7+(((x)&3)<<3))) /*                   */
#define BA0_FCHS_LCO(x)		(1<<(6+(((x)&3)<<3))) /*                  */
#define BA0_FCHS_MRP(x)		(1<<(5+(((x)&3)<<3))) /*                   */
#define BA0_FCHS_FE(x)		(1<<(4+(((x)&3)<<3))) /*            */
#define BA0_FCHS_FF(x)		(1<<(3+(((x)&3)<<3))) /*           */
#define BA0_FCHS_IOR(x)		(1<<(2+(((x)&3)<<3))) /*                       */
#define BA0_FCHS_RCI(x)		(1<<(1+(((x)&3)<<3))) /*                  */
#define BA0_FCHS_LCI(x)		(1<<(0+(((x)&3)<<3))) /*                 */

#define BA0_FSIC0		0x0210	/*                                     */
#define BA0_FSIC1		0x0214	/*                                     */
#define BA0_FSIC2		0x0218	/*                                     */
#define BA0_FSIC3		0x021c	/*                                     */

#define BA0_FSIC_FIC(x)		(((x)&0x7f)<<24) /*                      */
#define BA0_FSIC_FORIE		(1<<23) /*                               */
#define BA0_FSIC_FURIE		(1<<22) /*                                */
#define BA0_FSIC_FSCIE		(1<<16)	/*                                    */
#define BA0_FSIC_FSC(x)		(((x)&0x7f)<<8) /*                   */
#define BA0_FSIC_FOR		(1<<7)	/*              */
#define BA0_FSIC_FUR		(1<<6)	/*               */
#define BA0_FSIC_FSCR		(1<<0)	/*                           */

#define BA0_PMCS		0x0344	/*                                 */
#define BA0_CWPR		0x03e0	/*                             */

#define BA0_EPPMC		0x03e4	/*                                       */
#define BA0_EPPMC_FPDN		(1<<14) /*                 */

#define BA0_GPIOR		0x03e8	/*                             */

#define BA0_SPMC		0x03ec	/*                                                        */
#define BA0_SPMC_GIPPEN		(1<<15)	/*                            */
#define BA0_SPMC_GISPEN		(1<<14)	/*                              */
#define BA0_SPMC_EESPD		(1<<9)	/*                            */
#define BA0_SPMC_ASDI2E		(1<<8)	/*               */
#define BA0_SPMC_ASDO		(1<<7)	/*                               */
#define BA0_SPMC_WUP2		(1<<3)	/*                            */
#define BA0_SPMC_WUP1		(1<<2)	/*                          */
#define BA0_SPMC_ASYNC		(1<<1)	/*                              */
#define BA0_SPMC_RSTN		(1<<0)	/*            */

#define BA0_CFLR		0x03f0	/*                                              */
#define BA0_CFLR_DEFAULT	0x00000001 /*                                */
#define BA0_IISR		0x03f4	/*                      */
#define BA0_TMS			0x03f8	/*               */
#define BA0_SSVID		0x03fc	/*                       */

#define BA0_CLKCR1		0x0400	/*                          */
#define BA0_CLKCR1_CLKON	(1<<25)	/*           */
#define BA0_CLKCR1_DLLRDY	(1<<24)	/*           */
#define BA0_CLKCR1_DLLOS	(1<<6)	/*                   */
#define BA0_CLKCR1_SWCE		(1<<5)	/*              */
#define BA0_CLKCR1_DLLP		(1<<4)	/*             */
#define BA0_CLKCR1_DLLSS	(((x)&3)<<3) /*                   */

#define BA0_FRR			0x0410	/*                            */
#define BA0_SLT12O		0x041c	/*                                          */

#define BA0_SERMC		0x0420	/*                            */
#define BA0_SERMC_FCRN		(1<<27)	/*                       */
#define BA0_SERMC_ODSEN2	(1<<25)	/*                                 */
#define BA0_SERMC_ODSEN1	(1<<24)	/*                                 */
#define BA0_SERMC_SXLB		(1<<21)	/*                           */
#define BA0_SERMC_SLB		(1<<20)	/*                           */
#define BA0_SERMC_LOVF		(1<<19)	/*                                 */
#define BA0_SERMC_TCID(x)	(((x)&3)<<16) /*                           */
#define BA0_SERMC_PXLB		(5<<1)	/*                                */
#define BA0_SERMC_PLB		(4<<1)	/*                                */
#define BA0_SERMC_PTC		(7<<1)	/*                           */
#define BA0_SERMC_PTC_AC97	(1<<1)	/*           */
#define BA0_SERMC_MSPE		(1<<0)	/*                           */

#define BA0_SERC1		0x0428	/*                             */
#define BA0_SERC1_SO1F(x)	(((x)&7)>>1) /*                            */
#define BA0_SERC1_AC97		(1<<1)
#define BA0_SERC1_SO1EN		(1<<0)	/*                            */

#define BA0_SERC2		0x042c	/*                             */
#define BA0_SERC2_SI1F(x)	(((x)&7)>>1) /*                           */
#define BA0_SERC2_AC97		(1<<1)
#define BA0_SERC2_SI1EN		(1<<0)	/*                           */

#define BA0_SLT12M		0x045c	/*                                              */

#define BA0_ACCTL		0x0460	/*               */
#define BA0_ACCTL_TC		(1<<6)	/*              */
#define BA0_ACCTL_CRW		(1<<4)	/*                         */
#define BA0_ACCTL_DCV		(1<<3)	/*                       */
#define BA0_ACCTL_VFRM		(1<<2)	/*             */
#define BA0_ACCTL_ESYN		(1<<1)	/*             */

#define BA0_ACSTS		0x0464	/*              */
#define BA0_ACSTS_VSTS		(1<<1)	/*              */
#define BA0_ACSTS_CRDY		(1<<0)	/*             */

#define BA0_ACOSV		0x0468	/*                         */
#define BA0_ACOSV_SLV(x)	(1<<((x)-3))

#define BA0_ACCAD		0x046c	/*                       */
#define BA0_ACCDA		0x0470	/*                    */

#define BA0_ACISV		0x0474	/*                        */
#define BA0_ACISV_SLV(x)	(1<<((x)-3))

#define BA0_ACSAD		0x0478	/*                      */
#define BA0_ACSDA		0x047c	/*                   */
#define BA0_JSPT		0x0480	/*                       */
#define BA0_JSCTL		0x0484	/*                  */
#define BA0_JSC1		0x0488	/*                  */
#define BA0_JSC2		0x048c	/*                  */
#define BA0_JSIO		0x04a0

#define BA0_MIDCR		0x0490	/*              */
#define BA0_MIDCR_MRST		(1<<5)	/*                      */
#define BA0_MIDCR_MLB		(1<<4)	/*                       */
#define BA0_MIDCR_TIE		(1<<3)	/*                                 */
#define BA0_MIDCR_RIE		(1<<2)	/*                               */
#define BA0_MIDCR_RXE		(1<<1)	/*                     */
#define BA0_MIDCR_TXE		(1<<0)	/*                      */

#define BA0_MIDCMD		0x0494	/*                   */

#define BA0_MIDSR		0x0494	/*                  */
#define BA0_MIDSR_RDA		(1<<15)	/*                       */
#define BA0_MIDSR_TBE		(1<<14) /*                       */
#define BA0_MIDSR_RBE		(1<<7)	/*                      */
#define BA0_MIDSR_TBF		(1<<6)	/*                      */

#define BA0_MIDWP		0x0498	/*            */
#define BA0_MIDRP		0x049c	/*                */

#define BA0_AODSD1		0x04a8	/*                                                    */
#define BA0_AODSD1_NDS(x)	(1<<((x)-3))

#define BA0_AODSD2		0x04ac	/*                                                      */
#define BA0_AODSD2_NDS(x)	(1<<((x)-3))

#define BA0_CFGI		0x04b0	/*                                        */
#define BA0_SLT12M2		0x04dc	/*                                                  */
#define BA0_ACSTS2		0x04e4	/*                         */
#define BA0_ACISV2		0x04f4	/*                                   */
#define BA0_ACSAD2		0x04f8	/*                                 */
#define BA0_ACSDA2		0x04fc	/*                              */
#define BA0_FMSR		0x0730	/*                          */
#define BA0_B0AP		0x0730	/*                             */
#define BA0_FMDP		0x0734	/*              */
#define BA0_B1AP		0x0738	/*                        */
#define BA0_B1DP		0x073c	/*                     */

#define BA0_SSPM		0x0740	/*                               */
#define BA0_SSPM_MIXEN		(1<<6)	/*                                 */
#define BA0_SSPM_CSRCEN		(1<<5)	/*                                      */
#define BA0_SSPM_PSRCEN		(1<<4)	/*                                       */
#define BA0_SSPM_JSEN		(1<<3)	/*                 */
#define BA0_SSPM_ACLEN		(1<<2)	/*                                       */
#define BA0_SSPM_FMEN		(1<<1)	/*                           */

#define BA0_DACSR		0x0744	/*                                */
#define BA0_ADCSR		0x0748	/*                               */

#define BA0_SSCR		0x074c	/*                               */
#define BA0_SSCR_HVS1		(1<<23)	/*                                */
#define BA0_SSCR_MVCS		(1<<19)	/*                            */
#define BA0_SSCR_MVLD		(1<<18)	/*                                */
#define BA0_SSCR_MVAD		(1<<17)	/*                                     */
#define BA0_SSCR_MVMD		(1<<16)	/*                                */
#define BA0_SSCR_XLPSRC		(1<<8)	/*                            */
#define BA0_SSCR_LPSRC		(1<<7)	/*                   */
#define BA0_SSCR_CDTX		(1<<5)	/*                  */
#define BA0_SSCR_HVC		(1<<3)	/*                               */

#define BA0_FMLVC		0x0754	/*                                  */
#define BA0_FMRVC		0x0758	/*                                   */
#define BA0_SRCSA		0x075c	/*                      */
#define BA0_PPLVC		0x0760	/*                                  */
#define BA0_PPRVC		0x0764	/*                                   */
#define BA0_PASR		0x0768	/*                      */
#define BA0_CASR		0x076C	/*                     */

/*                                */
#define SRCSLOT_LEFT_PCM_PLAYBACK		0
#define SRCSLOT_RIGHT_PCM_PLAYBACK		1
#define SRCSLOT_PHONE_LINE_1_DAC		2
#define SRCSLOT_CENTER_PCM_PLAYBACK		3
#define SRCSLOT_LEFT_SURROUND_PCM_PLAYBACK	4
#define SRCSLOT_RIGHT_SURROUND_PCM_PLAYBACK	5
#define SRCSLOT_LFE_PCM_PLAYBACK		6
#define SRCSLOT_PHONE_LINE_2_DAC		7
#define SRCSLOT_HEADSET_DAC			8
#define SRCSLOT_LEFT_WT				29  /*                       */
#define SRCSLOT_RIGHT_WT			30  /*                       */

/*                               */
#define SRCSLOT_LEFT_PCM_RECORD			10
#define SRCSLOT_RIGHT_PCM_RECORD		11
#define SRCSLOT_PHONE_LINE_1_ADC		12
#define SRCSLOT_MIC_ADC				13
#define SRCSLOT_PHONE_LINE_2_ADC		17
#define SRCSLOT_HEADSET_ADC			18
#define SRCSLOT_SECONDARY_LEFT_PCM_RECORD	20
#define SRCSLOT_SECONDARY_RIGHT_PCM_RECORD	21
#define SRCSLOT_SECONDARY_PHONE_LINE_1_ADC	22
#define SRCSLOT_SECONDARY_MIC_ADC		23
#define SRCSLOT_SECONDARY_PHONE_LINE_2_ADC	27
#define SRCSLOT_SECONDARY_HEADSET_ADC		28

/*                              */
#define SRCSLOT_POWER_DOWN			31

/*            */
#define CS4281_MODE_OUTPUT		(1<<0)
#define CS4281_MODE_INPUT		(1<<1)

/*               */
/*               */
#define JSPT_CAX                                0x00000001
#define JSPT_CAY                                0x00000002
#define JSPT_CBX                                0x00000004
#define JSPT_CBY                                0x00000008
#define JSPT_BA1                                0x00000010
#define JSPT_BA2                                0x00000020
#define JSPT_BB1                                0x00000040
#define JSPT_BB2                                0x00000080

/*                */
#define JSCTL_SP_MASK                           0x00000003
#define JSCTL_SP_SLOW                           0x00000000
#define JSCTL_SP_MEDIUM_SLOW                    0x00000001
#define JSCTL_SP_MEDIUM_FAST                    0x00000002
#define JSCTL_SP_FAST                           0x00000003
#define JSCTL_ARE                               0x00000004

/*                           */
#define JSC1_Y1V_MASK                           0x0000FFFF
#define JSC1_X1V_MASK                           0xFFFF0000
#define JSC1_Y1V_SHIFT                          0
#define JSC1_X1V_SHIFT                          16
#define JSC2_Y2V_MASK                           0x0000FFFF
#define JSC2_X2V_MASK                           0xFFFF0000
#define JSC2_Y2V_SHIFT                          0
#define JSC2_X2V_SHIFT                          16

/*         */
#define JSIO_DAX                                0x00000001
#define JSIO_DAY                                0x00000002
#define JSIO_DBX                                0x00000004
#define JSIO_DBY                                0x00000008
#define JSIO_AXOE                               0x00000010
#define JSIO_AYOE                               0x00000020
#define JSIO_BXOE                               0x00000040
#define JSIO_BYOE                               0x00000080

/*
  
 */

struct cs4281_dma {
	struct snd_pcm_substream *substream;
	unsigned int regDBA;		/*                        */
	unsigned int regDCA;		/*                        */
	unsigned int regDBC;		/*                        */
	unsigned int regDCC;		/*                        */
	unsigned int regDMR;		/*                        */
	unsigned int regDCR;		/*                        */
	unsigned int regHDSR;		/*                         */
	unsigned int regFCR;		/*                        */
	unsigned int regFSIC;		/*                         */
	unsigned int valDMR;		/*          */
	unsigned int valDCR;		/*             */
	unsigned int valFCR;		/*              */
	unsigned int fifo_offset;	/*                        */
	unsigned char left_slot;	/*                */
	unsigned char right_slot;	/*                 */
	int frag;			/*               */
};

#define SUSPEND_REGISTERS	20

struct cs4281 {
	int irq;

	void __iomem *ba0;		/*                              */
	void __iomem *ba1;		/*                              */
	unsigned long ba0_addr;
	unsigned long ba1_addr;

	int dual_codec;

	struct snd_ac97_bus *ac97_bus;
	struct snd_ac97 *ac97;
	struct snd_ac97 *ac97_secondary;

	struct pci_dev *pci;
	struct snd_card *card;
	struct snd_pcm *pcm;
	struct snd_rawmidi *rmidi;
	struct snd_rawmidi_substream *midi_input;
	struct snd_rawmidi_substream *midi_output;

	struct cs4281_dma dma[4];

	unsigned char src_left_play_slot;
	unsigned char src_right_play_slot;
	unsigned char src_left_rec_slot;
	unsigned char src_right_rec_slot;

	unsigned int spurious_dhtc_irq;
	unsigned int spurious_dtc_irq;

	spinlock_t reg_lock;
	unsigned int midcr;
	unsigned int uartm;

	struct gameport *gameport;

#ifdef CONFIG_PM_SLEEP
	u32 suspend_regs[SUSPEND_REGISTERS];
#endif

};

static irqreturn_t snd_cs4281_interrupt(int irq, void *dev_id);

static DEFINE_PCI_DEVICE_TABLE(snd_cs4281_ids) = {
	{ PCI_VDEVICE(CIRRUS, 0x6005), 0, },	/*        */
	{ 0, }
};

MODULE_DEVICE_TABLE(pci, snd_cs4281_ids);

/*
             
 */

#define CS4281_FIFO_SIZE	32

/*
                       
 */

static inline void snd_cs4281_pokeBA0(struct cs4281 *chip, unsigned long offset,
				      unsigned int val)
{
        writel(val, chip->ba0 + offset);
}

static inline unsigned int snd_cs4281_peekBA0(struct cs4281 *chip, unsigned long offset)
{
        return readl(chip->ba0 + offset);
}

static void snd_cs4281_ac97_write(struct snd_ac97 *ac97,
				  unsigned short reg, unsigned short val)
{
	/*
                                                                               
                                                                               
                                                                      
                                                                    
                                                  
  */
	struct cs4281 *chip = ac97->private_data;
	int count;

	/*
                                                               
                                                         
                                            
                                         
                                    
                                                
                              
                                   
                                        
                                                    
         */
	snd_cs4281_pokeBA0(chip, BA0_ACCAD, reg);
	snd_cs4281_pokeBA0(chip, BA0_ACCDA, val);
	snd_cs4281_pokeBA0(chip, BA0_ACCTL, BA0_ACCTL_DCV | BA0_ACCTL_VFRM |
				            BA0_ACCTL_ESYN | (ac97->num ? BA0_ACCTL_TC : 0));
	for (count = 0; count < 2000; count++) {
		/*
                                              
   */
		udelay(10);
		/*
                                                   
                                                             
   */
		if (!(snd_cs4281_peekBA0(chip, BA0_ACCTL) & BA0_ACCTL_DCV)) {
			return;
		}
	}
	snd_printk(KERN_ERR "AC'97 write problem, reg = 0x%x, val = 0x%x\n", reg, val);
}

static unsigned short snd_cs4281_ac97_read(struct snd_ac97 *ac97,
					   unsigned short reg)
{
	struct cs4281 *chip = ac97->private_data;
	int count;
	unsigned short result;
	//                                                              
	//                  
	volatile int ac97_num = ((volatile struct snd_ac97 *)ac97)->num;

	/*
                                                                               
                                                                                
                                                                      
                                                                    
                                                  
                                                           
  */

	snd_cs4281_peekBA0(chip, ac97_num ? BA0_ACSDA2 : BA0_ACSDA);

	/*
                                                               
                                                         
                                            
                                         
                                    
                                                
                           
                                   
                                        
                                                    
  */

	snd_cs4281_pokeBA0(chip, BA0_ACCAD, reg);
	snd_cs4281_pokeBA0(chip, BA0_ACCDA, 0);
	snd_cs4281_pokeBA0(chip, BA0_ACCTL, BA0_ACCTL_DCV | BA0_ACCTL_CRW |
					    BA0_ACCTL_VFRM | BA0_ACCTL_ESYN |
			   (ac97_num ? BA0_ACCTL_TC : 0));


	/*
                                
  */
	for (count = 0; count < 500; count++) {
		/*
                                              
    */
		udelay(10);
		/*
                                                  
                                                             
   */
		if (!(snd_cs4281_peekBA0(chip, BA0_ACCTL) & BA0_ACCTL_DCV))
			goto __ok1;
	}

	snd_printk(KERN_ERR "AC'97 read problem (ACCTL_DCV), reg = 0x%x\n", reg);
	result = 0xffff;
	goto __end;
	
      __ok1:
	/*
                                                
  */
	for (count = 0; count < 100; count++) {
		/*
                                    
                                    
                         
   */
		if (snd_cs4281_peekBA0(chip, ac97_num ? BA0_ACSTS2 : BA0_ACSTS) & BA0_ACSTS_VSTS)
			goto __ok2;
		udelay(10);
	}
	
	snd_printk(KERN_ERR "AC'97 read problem (ACSTS_VSTS), reg = 0x%x\n", reg);
	result = 0xffff;
	goto __end;

      __ok2:
	/*
                                                   
                                        
  */
	result = snd_cs4281_peekBA0(chip, ac97_num ? BA0_ACSDA2 : BA0_ACSDA);

      __end:
	return result;
}

/*
            
 */

static int snd_cs4281_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct cs4281_dma *dma = substream->runtime->private_data;
	struct cs4281 *chip = snd_pcm_substream_chip(substream);

	spin_lock(&chip->reg_lock);
	switch (cmd) {
	case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		dma->valDCR |= BA0_DCR_MSK;
		dma->valFCR |= BA0_FCR_FEN;
		break;
	case SNDRV_PCM_TRIGGER_PAUSE_RELEASE:
		dma->valDCR &= ~BA0_DCR_MSK;
		dma->valFCR &= ~BA0_FCR_FEN;
		break;
	case SNDRV_PCM_TRIGGER_START:
	case SNDRV_PCM_TRIGGER_RESUME:
		snd_cs4281_pokeBA0(chip, dma->regDMR, dma->valDMR & ~BA0_DMR_DMA);
		dma->valDMR |= BA0_DMR_DMA;
		dma->valDCR &= ~BA0_DCR_MSK;
		dma->valFCR |= BA0_FCR_FEN;
		break;
	case SNDRV_PCM_TRIGGER_STOP:
	case SNDRV_PCM_TRIGGER_SUSPEND:
		dma->valDMR &= ~(BA0_DMR_DMA|BA0_DMR_POLL);
		dma->valDCR |= BA0_DCR_MSK;
		dma->valFCR &= ~BA0_FCR_FEN;
		/*                                         */
		if (dma->regFCR != BA0_FCR0)
			dma->valFCR &= ~BA0_FCR_FEN;
		break;
	default:
		spin_unlock(&chip->reg_lock);
		return -EINVAL;
	}
	snd_cs4281_pokeBA0(chip, dma->regDMR, dma->valDMR);
	snd_cs4281_pokeBA0(chip, dma->regFCR, dma->valFCR);
	snd_cs4281_pokeBA0(chip, dma->regDCR, dma->valDCR);
	spin_unlock(&chip->reg_lock);
	return 0;
}

static unsigned int snd_cs4281_rate(unsigned int rate, unsigned int *real_rate)
{
	unsigned int val = ~0;
	
	if (real_rate)
		*real_rate = rate;
	/*                           */
	switch (rate) {
	case 8000:	return 5;
	case 11025:	return 4;
	case 16000:	return 3;
	case 22050:	return 2;
	case 44100:	return 1;
	case 48000:	return 0;
	default:
		goto __variable;
	}
      __variable:
	val = 1536000 / rate;
	if (real_rate)
		*real_rate = 1536000 / val;
	return val;
}

static void snd_cs4281_mode(struct cs4281 *chip, struct cs4281_dma *dma,
			    struct snd_pcm_runtime *runtime,
			    int capture, int src)
{
	int rec_mono;

	dma->valDMR = BA0_DMR_TYPE_SINGLE | BA0_DMR_AUTO |
		      (capture ? BA0_DMR_TR_WRITE : BA0_DMR_TR_READ);
	if (runtime->channels == 1)
		dma->valDMR |= BA0_DMR_MONO;
	if (snd_pcm_format_unsigned(runtime->format) > 0)
		dma->valDMR |= BA0_DMR_USIGN;
	if (snd_pcm_format_big_endian(runtime->format) > 0)
		dma->valDMR |= BA0_DMR_BEND;
	switch (snd_pcm_format_width(runtime->format)) {
	case 8: dma->valDMR |= BA0_DMR_SIZE8;
		if (runtime->channels == 1)
			dma->valDMR |= BA0_DMR_SWAPC;
		break;
	case 32: dma->valDMR |= BA0_DMR_SIZE20; break;
	}
	dma->frag = 0;	/*                */
	dma->valDCR = BA0_DCR_TCIE | BA0_DCR_MSK;
	if (runtime->buffer_size != runtime->period_size)
		dma->valDCR |= BA0_DCR_HTCIE;
	/*                */
	snd_cs4281_pokeBA0(chip, dma->regDBA, runtime->dma_addr);
	snd_cs4281_pokeBA0(chip, dma->regDBC, runtime->buffer_size - 1);
	rec_mono = (chip->dma[1].valDMR & BA0_DMR_MONO) == BA0_DMR_MONO;
	snd_cs4281_pokeBA0(chip, BA0_SRCSA, (chip->src_left_play_slot << 0) |
					    (chip->src_right_play_slot << 8) |
					    (chip->src_left_rec_slot << 16) |
					    ((rec_mono ? 31 : chip->src_right_rec_slot) << 24));
	if (!src)
		goto __skip_src;
	if (!capture) {
		if (dma->left_slot == chip->src_left_play_slot) {
			unsigned int val = snd_cs4281_rate(runtime->rate, NULL);
			snd_BUG_ON(dma->right_slot != chip->src_right_play_slot);
			snd_cs4281_pokeBA0(chip, BA0_DACSR, val);
		}
	} else {
		if (dma->left_slot == chip->src_left_rec_slot) {
			unsigned int val = snd_cs4281_rate(runtime->rate, NULL);
			snd_BUG_ON(dma->right_slot != chip->src_right_rec_slot);
			snd_cs4281_pokeBA0(chip, BA0_ADCSR, val);
		}
	}
      __skip_src:
	/*                                                                */
	if (dma->regFCR == BA0_FCR0)
		snd_cs4281_pokeBA0(chip, dma->regFCR, snd_cs4281_peekBA0(chip, dma->regFCR) & ~BA0_FCR_FEN);
	/*                 */
	dma->valFCR = BA0_FCR_LS(dma->left_slot) |
		      BA0_FCR_RS(capture && (dma->valDMR & BA0_DMR_MONO) ? 31 : dma->right_slot) |
		      BA0_FCR_SZ(CS4281_FIFO_SIZE) |
		      BA0_FCR_OF(dma->fifo_offset);
	snd_cs4281_pokeBA0(chip, dma->regFCR, dma->valFCR | (capture ? BA0_FCR_PSH : 0));
	/*                                     */
	if (dma->regFCR == BA0_FCR0)
		snd_cs4281_pokeBA0(chip, dma->regFCR, dma->valFCR | BA0_FCR_FEN);
	/*                                                  */
	snd_cs4281_pokeBA0(chip, dma->regFSIC, 0);
}

static int snd_cs4281_hw_params(struct snd_pcm_substream *substream,
				struct snd_pcm_hw_params *hw_params)
{
	return snd_pcm_lib_malloc_pages(substream, params_buffer_bytes(hw_params));
}

static int snd_cs4281_hw_free(struct snd_pcm_substream *substream)
{
	return snd_pcm_lib_free_pages(substream);
}

static int snd_cs4281_playback_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct cs4281_dma *dma = runtime->private_data;
	struct cs4281 *chip = snd_pcm_substream_chip(substream);

	spin_lock_irq(&chip->reg_lock);
	snd_cs4281_mode(chip, dma, runtime, 0, 1);
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}

static int snd_cs4281_capture_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct cs4281_dma *dma = runtime->private_data;
	struct cs4281 *chip = snd_pcm_substream_chip(substream);

	spin_lock_irq(&chip->reg_lock);
	snd_cs4281_mode(chip, dma, runtime, 1, 1);
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}

static snd_pcm_uframes_t snd_cs4281_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct cs4281_dma *dma = runtime->private_data;
	struct cs4281 *chip = snd_pcm_substream_chip(substream);

	/*
                                                                     
                                                                    
                 
 */
	return runtime->buffer_size -
	       snd_cs4281_peekBA0(chip, dma->regDCC) - 1;
}

static struct snd_pcm_hardware snd_cs4281_playback =
{
	.info =			SNDRV_PCM_INFO_MMAP |
				SNDRV_PCM_INFO_INTERLEAVED |
				SNDRV_PCM_INFO_MMAP_VALID |
				SNDRV_PCM_INFO_PAUSE |
				SNDRV_PCM_INFO_RESUME,
	.formats =		SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S8 |
				SNDRV_PCM_FMTBIT_U16_LE | SNDRV_PCM_FMTBIT_S16_LE |
				SNDRV_PCM_FMTBIT_U16_BE | SNDRV_PCM_FMTBIT_S16_BE |
				SNDRV_PCM_FMTBIT_U32_LE | SNDRV_PCM_FMTBIT_S32_LE |
				SNDRV_PCM_FMTBIT_U32_BE | SNDRV_PCM_FMTBIT_S32_BE,
	.rates =		SNDRV_PCM_RATE_CONTINUOUS | SNDRV_PCM_RATE_8000_48000,
	.rate_min =		4000,
	.rate_max =		48000,
	.channels_min =		1,
	.channels_max =		2,
	.buffer_bytes_max =	(512*1024),
	.period_bytes_min =	64,
	.period_bytes_max =	(512*1024),
	.periods_min =		1,
	.periods_max =		2,
	.fifo_size =		CS4281_FIFO_SIZE,
};

static struct snd_pcm_hardware snd_cs4281_capture =
{
	.info =			SNDRV_PCM_INFO_MMAP |
				SNDRV_PCM_INFO_INTERLEAVED |
				SNDRV_PCM_INFO_MMAP_VALID |
				SNDRV_PCM_INFO_PAUSE |
				SNDRV_PCM_INFO_RESUME,
	.formats =		SNDRV_PCM_FMTBIT_U8 | SNDRV_PCM_FMTBIT_S8 |
				SNDRV_PCM_FMTBIT_U16_LE | SNDRV_PCM_FMTBIT_S16_LE |
				SNDRV_PCM_FMTBIT_U16_BE | SNDRV_PCM_FMTBIT_S16_BE |
				SNDRV_PCM_FMTBIT_U32_LE | SNDRV_PCM_FMTBIT_S32_LE |
				SNDRV_PCM_FMTBIT_U32_BE | SNDRV_PCM_FMTBIT_S32_BE,
	.rates =		SNDRV_PCM_RATE_CONTINUOUS | SNDRV_PCM_RATE_8000_48000,
	.rate_min =		4000,
	.rate_max =		48000,
	.channels_min =		1,
	.channels_max =		2,
	.buffer_bytes_max =	(512*1024),
	.period_bytes_min =	64,
	.period_bytes_max =	(512*1024),
	.periods_min =		1,
	.periods_max =		2,
	.fifo_size =		CS4281_FIFO_SIZE,
};

static int snd_cs4281_playback_open(struct snd_pcm_substream *substream)
{
	struct cs4281 *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct cs4281_dma *dma;

	dma = &chip->dma[0];
	dma->substream = substream;
	dma->left_slot = 0;
	dma->right_slot = 1;
	runtime->private_data = dma;
	runtime->hw = snd_cs4281_playback;
	/*                                                      
                                                              
                       */
	snd_pcm_hw_constraint_msbits(runtime, 0, 32, 20);
	return 0;
}

static int snd_cs4281_capture_open(struct snd_pcm_substream *substream)
{
	struct cs4281 *chip = snd_pcm_substream_chip(substream);
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct cs4281_dma *dma;

	dma = &chip->dma[1];
	dma->substream = substream;
	dma->left_slot = 10;
	dma->right_slot = 11;
	runtime->private_data = dma;
	runtime->hw = snd_cs4281_capture;
	/*                                                      
                                                              
                       */
	snd_pcm_hw_constraint_msbits(runtime, 0, 32, 20);
	return 0;
}

static int snd_cs4281_playback_close(struct snd_pcm_substream *substream)
{
	struct cs4281_dma *dma = substream->runtime->private_data;

	dma->substream = NULL;
	return 0;
}

static int snd_cs4281_capture_close(struct snd_pcm_substream *substream)
{
	struct cs4281_dma *dma = substream->runtime->private_data;

	dma->substream = NULL;
	return 0;
}

static struct snd_pcm_ops snd_cs4281_playback_ops = {
	.open =		snd_cs4281_playback_open,
	.close =	snd_cs4281_playback_close,
	.ioctl =	snd_pcm_lib_ioctl,
	.hw_params =	snd_cs4281_hw_params,
	.hw_free =	snd_cs4281_hw_free,
	.prepare =	snd_cs4281_playback_prepare,
	.trigger =	snd_cs4281_trigger,
	.pointer =	snd_cs4281_pointer,
};

static struct snd_pcm_ops snd_cs4281_capture_ops = {
	.open =		snd_cs4281_capture_open,
	.close =	snd_cs4281_capture_close,
	.ioctl =	snd_pcm_lib_ioctl,
	.hw_params =	snd_cs4281_hw_params,
	.hw_free =	snd_cs4281_hw_free,
	.prepare =	snd_cs4281_capture_prepare,
	.trigger =	snd_cs4281_trigger,
	.pointer =	snd_cs4281_pointer,
};

static int snd_cs4281_pcm(struct cs4281 *chip, int device,
			  struct snd_pcm **rpcm)
{
	struct snd_pcm *pcm;
	int err;

	if (rpcm)
		*rpcm = NULL;
	err = snd_pcm_new(chip->card, "CS4281", device, 1, 1, &pcm);
	if (err < 0)
		return err;

	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_PLAYBACK, &snd_cs4281_playback_ops);
	snd_pcm_set_ops(pcm, SNDRV_PCM_STREAM_CAPTURE, &snd_cs4281_capture_ops);

	pcm->private_data = chip;
	pcm->info_flags = 0;
	strcpy(pcm->name, "CS4281");
	chip->pcm = pcm;

	snd_pcm_lib_preallocate_pages_for_all(pcm, SNDRV_DMA_TYPE_DEV,
					      snd_dma_pci_data(chip->pci), 64*1024, 512*1024);

	if (rpcm)
		*rpcm = pcm;
	return 0;
}

/*
                 
 */

#define CS_VOL_MASK	0x1f

static int snd_cs4281_info_volume(struct snd_kcontrol *kcontrol,
				  struct snd_ctl_elem_info *uinfo)
{
	uinfo->type              = SNDRV_CTL_ELEM_TYPE_INTEGER;
	uinfo->count             = 2;
	uinfo->value.integer.min = 0;
	uinfo->value.integer.max = CS_VOL_MASK;
	return 0;
}
 
static int snd_cs4281_get_volume(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct cs4281 *chip = snd_kcontrol_chip(kcontrol);
	int regL = (kcontrol->private_value >> 16) & 0xffff;
	int regR = kcontrol->private_value & 0xffff;
	int volL, volR;

	volL = CS_VOL_MASK - (snd_cs4281_peekBA0(chip, regL) & CS_VOL_MASK);
	volR = CS_VOL_MASK - (snd_cs4281_peekBA0(chip, regR) & CS_VOL_MASK);

	ucontrol->value.integer.value[0] = volL;
	ucontrol->value.integer.value[1] = volR;
	return 0;
}

static int snd_cs4281_put_volume(struct snd_kcontrol *kcontrol,
				 struct snd_ctl_elem_value *ucontrol)
{
	struct cs4281 *chip = snd_kcontrol_chip(kcontrol);
	int change = 0;
	int regL = (kcontrol->private_value >> 16) & 0xffff;
	int regR = kcontrol->private_value & 0xffff;
	int volL, volR;

	volL = CS_VOL_MASK - (snd_cs4281_peekBA0(chip, regL) & CS_VOL_MASK);
	volR = CS_VOL_MASK - (snd_cs4281_peekBA0(chip, regR) & CS_VOL_MASK);

	if (ucontrol->value.integer.value[0] != volL) {
		volL = CS_VOL_MASK - (ucontrol->value.integer.value[0] & CS_VOL_MASK);
		snd_cs4281_pokeBA0(chip, regL, volL);
		change = 1;
	}
	if (ucontrol->value.integer.value[1] != volR) {
		volR = CS_VOL_MASK - (ucontrol->value.integer.value[1] & CS_VOL_MASK);
		snd_cs4281_pokeBA0(chip, regR, volR);
		change = 1;
	}
	return change;
}

static const DECLARE_TLV_DB_SCALE(db_scale_dsp, -4650, 150, 0);

static struct snd_kcontrol_new snd_cs4281_fm_vol = 
{
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "Synth Playback Volume",
	.info = snd_cs4281_info_volume, 
	.get = snd_cs4281_get_volume,
	.put = snd_cs4281_put_volume, 
	.private_value = ((BA0_FMLVC << 16) | BA0_FMRVC),
	.tlv = { .p = db_scale_dsp },
};

static struct snd_kcontrol_new snd_cs4281_pcm_vol = 
{
	.iface = SNDRV_CTL_ELEM_IFACE_MIXER,
	.name = "PCM Stream Playback Volume",
	.info = snd_cs4281_info_volume, 
	.get = snd_cs4281_get_volume,
	.put = snd_cs4281_put_volume, 
	.private_value = ((BA0_PPLVC << 16) | BA0_PPRVC),
	.tlv = { .p = db_scale_dsp },
};

static void snd_cs4281_mixer_free_ac97_bus(struct snd_ac97_bus *bus)
{
	struct cs4281 *chip = bus->private_data;
	chip->ac97_bus = NULL;
}

static void snd_cs4281_mixer_free_ac97(struct snd_ac97 *ac97)
{
	struct cs4281 *chip = ac97->private_data;
	if (ac97->num)
		chip->ac97_secondary = NULL;
	else
		chip->ac97 = NULL;
}

static int snd_cs4281_mixer(struct cs4281 *chip)
{
	struct snd_card *card = chip->card;
	struct snd_ac97_template ac97;
	int err;
	static struct snd_ac97_bus_ops ops = {
		.write = snd_cs4281_ac97_write,
		.read = snd_cs4281_ac97_read,
	};

	if ((err = snd_ac97_bus(card, 0, &ops, chip, &chip->ac97_bus)) < 0)
		return err;
	chip->ac97_bus->private_free = snd_cs4281_mixer_free_ac97_bus;

	memset(&ac97, 0, sizeof(ac97));
	ac97.private_data = chip;
	ac97.private_free = snd_cs4281_mixer_free_ac97;
	if ((err = snd_ac97_mixer(chip->ac97_bus, &ac97, &chip->ac97)) < 0)
		return err;
	if (chip->dual_codec) {
		ac97.num = 1;
		if ((err = snd_ac97_mixer(chip->ac97_bus, &ac97, &chip->ac97_secondary)) < 0)
			return err;
	}
	if ((err = snd_ctl_add(card, snd_ctl_new1(&snd_cs4281_fm_vol, chip))) < 0)
		return err;
	if ((err = snd_ctl_add(card, snd_ctl_new1(&snd_cs4281_pcm_vol, chip))) < 0)
		return err;
	return 0;
}


/*
                 
 */

static void snd_cs4281_proc_read(struct snd_info_entry *entry, 
				  struct snd_info_buffer *buffer)
{
	struct cs4281 *chip = entry->private_data;

	snd_iprintf(buffer, "Cirrus Logic CS4281\n\n");
	snd_iprintf(buffer, "Spurious half IRQs   : %u\n", chip->spurious_dhtc_irq);
	snd_iprintf(buffer, "Spurious end IRQs    : %u\n", chip->spurious_dtc_irq);
}

static ssize_t snd_cs4281_BA0_read(struct snd_info_entry *entry,
				   void *file_private_data,
				   struct file *file, char __user *buf,
				   size_t count, loff_t pos)
{
	struct cs4281 *chip = entry->private_data;
	
	if (copy_to_user_fromio(buf, chip->ba0 + pos, count))
		return -EFAULT;
	return count;
}

static ssize_t snd_cs4281_BA1_read(struct snd_info_entry *entry,
				   void *file_private_data,
				   struct file *file, char __user *buf,
				   size_t count, loff_t pos)
{
	struct cs4281 *chip = entry->private_data;
	
	if (copy_to_user_fromio(buf, chip->ba1 + pos, count))
		return -EFAULT;
	return count;
}

static struct snd_info_entry_ops snd_cs4281_proc_ops_BA0 = {
	.read = snd_cs4281_BA0_read,
};

static struct snd_info_entry_ops snd_cs4281_proc_ops_BA1 = {
	.read = snd_cs4281_BA1_read,
};

static void snd_cs4281_proc_init(struct cs4281 *chip)
{
	struct snd_info_entry *entry;

	if (! snd_card_proc_new(chip->card, "cs4281", &entry))
		snd_info_set_text_ops(entry, chip, snd_cs4281_proc_read);
	if (! snd_card_proc_new(chip->card, "cs4281_BA0", &entry)) {
		entry->content = SNDRV_INFO_CONTENT_DATA;
		entry->private_data = chip;
		entry->c.ops = &snd_cs4281_proc_ops_BA0;
		entry->size = CS4281_BA0_SIZE;
	}
	if (! snd_card_proc_new(chip->card, "cs4281_BA1", &entry)) {
		entry->content = SNDRV_INFO_CONTENT_DATA;
		entry->private_data = chip;
		entry->c.ops = &snd_cs4281_proc_ops_BA1;
		entry->size = CS4281_BA1_SIZE;
	}
}

/*
                   
 */

#if defined(CONFIG_GAMEPORT) || (defined(MODULE) && defined(CONFIG_GAMEPORT_MODULE))

static void snd_cs4281_gameport_trigger(struct gameport *gameport)
{
	struct cs4281 *chip = gameport_get_port_data(gameport);

	if (snd_BUG_ON(!chip))
		return;
	snd_cs4281_pokeBA0(chip, BA0_JSPT, 0xff);
}

static unsigned char snd_cs4281_gameport_read(struct gameport *gameport)
{
	struct cs4281 *chip = gameport_get_port_data(gameport);

	if (snd_BUG_ON(!chip))
		return 0;
	return snd_cs4281_peekBA0(chip, BA0_JSPT);
}

#ifdef COOKED_MODE
static int snd_cs4281_gameport_cooked_read(struct gameport *gameport,
					   int *axes, int *buttons)
{
	struct cs4281 *chip = gameport_get_port_data(gameport);
	unsigned js1, js2, jst;
	
	if (snd_BUG_ON(!chip))
		return 0;

	js1 = snd_cs4281_peekBA0(chip, BA0_JSC1);
	js2 = snd_cs4281_peekBA0(chip, BA0_JSC2);
	jst = snd_cs4281_peekBA0(chip, BA0_JSPT);
	
	*buttons = (~jst >> 4) & 0x0F; 
	
	axes[0] = ((js1 & JSC1_Y1V_MASK) >> JSC1_Y1V_SHIFT) & 0xFFFF;
	axes[1] = ((js1 & JSC1_X1V_MASK) >> JSC1_X1V_SHIFT) & 0xFFFF;
	axes[2] = ((js2 & JSC2_Y2V_MASK) >> JSC2_Y2V_SHIFT) & 0xFFFF;
	axes[3] = ((js2 & JSC2_X2V_MASK) >> JSC2_X2V_SHIFT) & 0xFFFF;

	for (jst = 0; jst < 4; ++jst)
		if (axes[jst] == 0xFFFF) axes[jst] = -1;
	return 0;
}
#else
#define snd_cs4281_gameport_cooked_read	NULL
#endif

static int snd_cs4281_gameport_open(struct gameport *gameport, int mode)
{
	switch (mode) {
#ifdef COOKED_MODE
	case GAMEPORT_MODE_COOKED:
		return 0;
#endif
	case GAMEPORT_MODE_RAW:
		return 0;
	default:
		return -1;
	}
	return 0;
}

static int snd_cs4281_create_gameport(struct cs4281 *chip)
{
	struct gameport *gp;

	chip->gameport = gp = gameport_allocate_port();
	if (!gp) {
		printk(KERN_ERR "cs4281: cannot allocate memory for gameport\n");
		return -ENOMEM;
	}

	gameport_set_name(gp, "CS4281 Gameport");
	gameport_set_phys(gp, "pci%s/gameport0", pci_name(chip->pci));
	gameport_set_dev_parent(gp, &chip->pci->dev);
	gp->open = snd_cs4281_gameport_open;
	gp->read = snd_cs4281_gameport_read;
	gp->trigger = snd_cs4281_gameport_trigger;
	gp->cooked_read = snd_cs4281_gameport_cooked_read;
	gameport_set_port_data(gp, chip);

	snd_cs4281_pokeBA0(chip, BA0_JSIO, 0xFF); //  
	snd_cs4281_pokeBA0(chip, BA0_JSCTL, JSCTL_SP_MEDIUM_SLOW);

	gameport_register_port(gp);

	return 0;
}

static void snd_cs4281_free_gameport(struct cs4281 *chip)
{
	if (chip->gameport) {
		gameport_unregister_port(chip->gameport);
		chip->gameport = NULL;
	}
}
#else
static inline int snd_cs4281_create_gameport(struct cs4281 *chip) { return -ENOSYS; }
static inline void snd_cs4281_free_gameport(struct cs4281 *chip) { }
#endif /*                                                       */

static int snd_cs4281_free(struct cs4281 *chip)
{
	snd_cs4281_free_gameport(chip);

	if (chip->irq >= 0)
		synchronize_irq(chip->irq);

	/*                 */
	snd_cs4281_pokeBA0(chip, BA0_HIMR, 0x7fffffff);
	/*                           */
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, 0);
	/*                                                     */
	snd_cs4281_pokeBA0(chip, BA0_SSPM, 0);
	/*                          */
	pci_set_power_state(chip->pci, 3);

	if (chip->irq >= 0)
		free_irq(chip->irq, chip);
	if (chip->ba0)
		iounmap(chip->ba0);
	if (chip->ba1)
		iounmap(chip->ba1);
	pci_release_regions(chip->pci);
	pci_disable_device(chip->pci);

	kfree(chip);
	return 0;
}

static int snd_cs4281_dev_free(struct snd_device *device)
{
	struct cs4281 *chip = device->device_data;
	return snd_cs4281_free(chip);
}

static int snd_cs4281_chip_init(struct cs4281 *chip); /*               */

static int snd_cs4281_create(struct snd_card *card,
			     struct pci_dev *pci,
			     struct cs4281 **rchip,
			     int dual_codec)
{
	struct cs4281 *chip;
	unsigned int tmp;
	int err;
	static struct snd_device_ops ops = {
		.dev_free =	snd_cs4281_dev_free,
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
	pci_set_master(pci);
	if (dual_codec < 0 || dual_codec > 3) {
		snd_printk(KERN_ERR "invalid dual_codec option %d\n", dual_codec);
		dual_codec = 0;
	}
	chip->dual_codec = dual_codec;

	if ((err = pci_request_regions(pci, "CS4281")) < 0) {
		kfree(chip);
		pci_disable_device(pci);
		return err;
	}
	chip->ba0_addr = pci_resource_start(pci, 0);
	chip->ba1_addr = pci_resource_start(pci, 1);

	chip->ba0 = pci_ioremap_bar(pci, 0);
	chip->ba1 = pci_ioremap_bar(pci, 1);
	if (!chip->ba0 || !chip->ba1) {
		snd_cs4281_free(chip);
		return -ENOMEM;
	}
	
	if (request_irq(pci->irq, snd_cs4281_interrupt, IRQF_SHARED,
			KBUILD_MODNAME, chip)) {
		snd_printk(KERN_ERR "unable to grab IRQ %d\n", pci->irq);
		snd_cs4281_free(chip);
		return -ENOMEM;
	}
	chip->irq = pci->irq;

	tmp = snd_cs4281_chip_init(chip);
	if (tmp) {
		snd_cs4281_free(chip);
		return tmp;
	}

	if ((err = snd_device_new(card, SNDRV_DEV_LOWLEVEL, chip, &ops)) < 0) {
		snd_cs4281_free(chip);
		return err;
	}

	snd_cs4281_proc_init(chip);

	snd_card_set_dev(card, &pci->dev);

	*rchip = chip;
	return 0;
}

static int snd_cs4281_chip_init(struct cs4281 *chip)
{
	unsigned int tmp;
	unsigned long end_time;
	int retry_count = 2;

	/*                                                        */
	tmp = snd_cs4281_peekBA0(chip, BA0_EPPMC);
	if (tmp & BA0_EPPMC_FPDN)
		snd_cs4281_pokeBA0(chip, BA0_EPPMC, tmp & ~BA0_EPPMC_FPDN);

      __retry:
	tmp = snd_cs4281_peekBA0(chip, BA0_CFLR);
	if (tmp != BA0_CFLR_DEFAULT) {
		snd_cs4281_pokeBA0(chip, BA0_CFLR, BA0_CFLR_DEFAULT);
		tmp = snd_cs4281_peekBA0(chip, BA0_CFLR);
		if (tmp != BA0_CFLR_DEFAULT) {
			snd_printk(KERN_ERR "CFLR setup failed (0x%x)\n", tmp);
			return -EIO;
		}
	}

	/*                                               
                                                  
                                                      */	
	snd_cs4281_pokeBA0(chip, BA0_CWPR, 0x4281);
	
	if ((tmp = snd_cs4281_peekBA0(chip, BA0_SERC1)) != (BA0_SERC1_SO1EN | BA0_SERC1_AC97)) {
		snd_printk(KERN_ERR "SERC1 AC'97 check failed (0x%x)\n", tmp);
		return -EIO;
	}
	if ((tmp = snd_cs4281_peekBA0(chip, BA0_SERC2)) != (BA0_SERC2_SI1EN | BA0_SERC2_AC97)) {
		snd_printk(KERN_ERR "SERC2 AC'97 check failed (0x%x)\n", tmp);
		return -EIO;
	}

	/*                               */
	snd_cs4281_pokeBA0(chip, BA0_SSPM, BA0_SSPM_MIXEN | BA0_SSPM_CSRCEN |
				           BA0_SSPM_PSRCEN | BA0_SSPM_JSEN |
				           BA0_SSPM_ACLEN | BA0_SSPM_FMEN);

	/*                              */
 	/*                                                     
                                                                       
                                                                      
                                       */
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, 0);
	snd_cs4281_pokeBA0(chip, BA0_SERMC, 0);

        /*                                 
                                            */
	snd_cs4281_pokeBA0(chip, BA0_ACCTL, 0);
	udelay(50);
                
	/*                                                                      
                                                                         
                                                                         
                  */
	snd_cs4281_pokeBA0(chip, BA0_SPMC, 0);
	udelay(50);
	snd_cs4281_pokeBA0(chip, BA0_SPMC, BA0_SPMC_RSTN);
	msleep(50);

	if (chip->dual_codec)
		snd_cs4281_pokeBA0(chip, BA0_SPMC, BA0_SPMC_RSTN | BA0_SPMC_ASDI2E);

	/*
                                              
  */
	snd_cs4281_pokeBA0(chip, BA0_SERMC,
			   (chip->dual_codec ? BA0_SERMC_TCID(chip->dual_codec) : BA0_SERMC_TCID(1)) |
			   BA0_SERMC_PTC_AC97 | BA0_SERMC_MSPE);

	/*
                               
  */
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, BA0_CLKCR1_DLLP);
	msleep(50);
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, BA0_CLKCR1_SWCE | BA0_CLKCR1_DLLP);

	/*
                                                       
  */
	end_time = jiffies + HZ;
	do {
		/*
                                                                
                                 
   */
		if (snd_cs4281_peekBA0(chip, BA0_CLKCR1) & BA0_CLKCR1_DLLRDY)
			goto __ok0;
		schedule_timeout_uninterruptible(1);
	} while (time_after_eq(end_time, jiffies));

	snd_printk(KERN_ERR "DLLRDY not seen\n");
	return -EIO;

      __ok0:

	/*
                                                                      
                                                                    
            
  */
	snd_cs4281_pokeBA0(chip, BA0_ACCTL, BA0_ACCTL_ESYN);

	/*
                                                        
  */
	end_time = jiffies + HZ;
	do {
		/*
                                                                
                                 
   */
		if (snd_cs4281_peekBA0(chip, BA0_ACSTS) & BA0_ACSTS_CRDY)
			goto __ok1;
		schedule_timeout_uninterruptible(1);
	} while (time_after_eq(end_time, jiffies));

	snd_printk(KERN_ERR "never read codec ready from AC'97 (0x%x)\n", snd_cs4281_peekBA0(chip, BA0_ACSTS));
	return -EIO;

      __ok1:
	if (chip->dual_codec) {
		end_time = jiffies + HZ;
		do {
			if (snd_cs4281_peekBA0(chip, BA0_ACSTS2) & BA0_ACSTS_CRDY)
				goto __codec2_ok;
			schedule_timeout_uninterruptible(1);
		} while (time_after_eq(end_time, jiffies));
		snd_printk(KERN_INFO "secondary codec doesn't respond. disable it...\n");
		chip->dual_codec = 0;
	__codec2_ok: ;
	}

	/*
                                                                        
                       
  */

	snd_cs4281_pokeBA0(chip, BA0_ACCTL, BA0_ACCTL_VFRM | BA0_ACCTL_ESYN);

	/*
                                                                        
                                                      
  */

	end_time = jiffies + HZ;
	do {
		/*
                                                                 
                      
   */
                if ((snd_cs4281_peekBA0(chip, BA0_ACISV) & (BA0_ACISV_SLV(3) | BA0_ACISV_SLV(4))) == (BA0_ACISV_SLV(3) | BA0_ACISV_SLV(4)))
                        goto __ok2;
		schedule_timeout_uninterruptible(1);
	} while (time_after_eq(end_time, jiffies));

	if (--retry_count > 0)
		goto __retry;
	snd_printk(KERN_ERR "never read ISV3 and ISV4 from AC'97\n");
	return -EIO;

      __ok2:

	/*
                                                                        
                                                                   
  */
	snd_cs4281_pokeBA0(chip, BA0_ACOSV, BA0_ACOSV_SLV(3) | BA0_ACOSV_SLV(4));

	/*
                              
  */
	for (tmp = 0; tmp < 4; tmp++) {
		struct cs4281_dma *dma = &chip->dma[tmp];
		dma->regDBA = BA0_DBA0 + (tmp * 0x10);
		dma->regDCA = BA0_DCA0 + (tmp * 0x10);
		dma->regDBC = BA0_DBC0 + (tmp * 0x10);
		dma->regDCC = BA0_DCC0 + (tmp * 0x10);
		dma->regDMR = BA0_DMR0 + (tmp * 8);
		dma->regDCR = BA0_DCR0 + (tmp * 8);
		dma->regHDSR = BA0_HDSR0 + (tmp * 4);
		dma->regFCR = BA0_FCR0 + (tmp * 4);
		dma->regFSIC = BA0_FSIC0 + (tmp * 4);
		dma->fifo_offset = tmp * CS4281_FIFO_SIZE;
		snd_cs4281_pokeBA0(chip, dma->regFCR,
				   BA0_FCR_LS(31) |
				   BA0_FCR_RS(31) |
				   BA0_FCR_SZ(CS4281_FIFO_SIZE) |
				   BA0_FCR_OF(dma->fifo_offset));
	}

	chip->src_left_play_slot = 0;	/*                             */
	chip->src_right_play_slot = 1;	/*                              */
	chip->src_left_rec_slot = 10;	/*                           */
	chip->src_right_rec_slot = 11;	/*                            */

	/*                                             */
	chip->dma[0].valFCR = BA0_FCR_FEN | BA0_FCR_LS(0) |
		              BA0_FCR_RS(1) |
 	  	              BA0_FCR_SZ(CS4281_FIFO_SIZE) |
		              BA0_FCR_OF(chip->dma[0].fifo_offset);
	snd_cs4281_pokeBA0(chip, chip->dma[0].regFCR, chip->dma[0].valFCR);
	snd_cs4281_pokeBA0(chip, BA0_SRCSA, (chip->src_left_play_slot << 0) |
					    (chip->src_right_play_slot << 8) |
					    (chip->src_left_rec_slot << 16) |
					    (chip->src_right_rec_slot << 24));

	/*                           */
	snd_cs4281_pokeBA0(chip, BA0_PPLVC, 0);
	snd_cs4281_pokeBA0(chip, BA0_PPRVC, 0);

	/*             */
	snd_cs4281_pokeBA0(chip, BA0_HICR, BA0_HICR_EOI);
	/*                   */
	snd_cs4281_pokeBA0(chip, BA0_HIMR, 0x7fffffff & ~(
					BA0_HISR_MIDI |
					BA0_HISR_DMAI |
					BA0_HISR_DMA(0) |
					BA0_HISR_DMA(1) |
					BA0_HISR_DMA(2) |
					BA0_HISR_DMA(3)));
	synchronize_irq(chip->irq);

	return 0;
}

/*
                
 */

static void snd_cs4281_midi_reset(struct cs4281 *chip)
{
	snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr | BA0_MIDCR_MRST);
	udelay(100);
	snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
}

static int snd_cs4281_midi_input_open(struct snd_rawmidi_substream *substream)
{
	struct cs4281 *chip = substream->rmidi->private_data;

	spin_lock_irq(&chip->reg_lock);
 	chip->midcr |= BA0_MIDCR_RXE;
	chip->midi_input = substream;
	if (!(chip->uartm & CS4281_MODE_OUTPUT)) {
		snd_cs4281_midi_reset(chip);
	} else {
		snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
	}
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}

static int snd_cs4281_midi_input_close(struct snd_rawmidi_substream *substream)
{
	struct cs4281 *chip = substream->rmidi->private_data;

	spin_lock_irq(&chip->reg_lock);
	chip->midcr &= ~(BA0_MIDCR_RXE | BA0_MIDCR_RIE);
	chip->midi_input = NULL;
	if (!(chip->uartm & CS4281_MODE_OUTPUT)) {
		snd_cs4281_midi_reset(chip);
	} else {
		snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
	}
	chip->uartm &= ~CS4281_MODE_INPUT;
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}

static int snd_cs4281_midi_output_open(struct snd_rawmidi_substream *substream)
{
	struct cs4281 *chip = substream->rmidi->private_data;

	spin_lock_irq(&chip->reg_lock);
	chip->uartm |= CS4281_MODE_OUTPUT;
	chip->midcr |= BA0_MIDCR_TXE;
	chip->midi_output = substream;
	if (!(chip->uartm & CS4281_MODE_INPUT)) {
		snd_cs4281_midi_reset(chip);
	} else {
		snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
	}
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}

static int snd_cs4281_midi_output_close(struct snd_rawmidi_substream *substream)
{
	struct cs4281 *chip = substream->rmidi->private_data;

	spin_lock_irq(&chip->reg_lock);
	chip->midcr &= ~(BA0_MIDCR_TXE | BA0_MIDCR_TIE);
	chip->midi_output = NULL;
	if (!(chip->uartm & CS4281_MODE_INPUT)) {
		snd_cs4281_midi_reset(chip);
	} else {
		snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
	}
	chip->uartm &= ~CS4281_MODE_OUTPUT;
	spin_unlock_irq(&chip->reg_lock);
	return 0;
}

static void snd_cs4281_midi_input_trigger(struct snd_rawmidi_substream *substream, int up)
{
	unsigned long flags;
	struct cs4281 *chip = substream->rmidi->private_data;

	spin_lock_irqsave(&chip->reg_lock, flags);
	if (up) {
		if ((chip->midcr & BA0_MIDCR_RIE) == 0) {
			chip->midcr |= BA0_MIDCR_RIE;
			snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
		}
	} else {
		if (chip->midcr & BA0_MIDCR_RIE) {
			chip->midcr &= ~BA0_MIDCR_RIE;
			snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
		}
	}
	spin_unlock_irqrestore(&chip->reg_lock, flags);
}

static void snd_cs4281_midi_output_trigger(struct snd_rawmidi_substream *substream, int up)
{
	unsigned long flags;
	struct cs4281 *chip = substream->rmidi->private_data;
	unsigned char byte;

	spin_lock_irqsave(&chip->reg_lock, flags);
	if (up) {
		if ((chip->midcr & BA0_MIDCR_TIE) == 0) {
			chip->midcr |= BA0_MIDCR_TIE;
			/*                                                                          */
			while ((chip->midcr & BA0_MIDCR_TIE) &&
			       (snd_cs4281_peekBA0(chip, BA0_MIDSR) & BA0_MIDSR_TBF) == 0) {
				if (snd_rawmidi_transmit(substream, &byte, 1) != 1) {
					chip->midcr &= ~BA0_MIDCR_TIE;
				} else {
					snd_cs4281_pokeBA0(chip, BA0_MIDWP, byte);
				}
			}
			snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
		}
	} else {
		if (chip->midcr & BA0_MIDCR_TIE) {
			chip->midcr &= ~BA0_MIDCR_TIE;
			snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
		}
	}
	spin_unlock_irqrestore(&chip->reg_lock, flags);
}

static struct snd_rawmidi_ops snd_cs4281_midi_output =
{
	.open =		snd_cs4281_midi_output_open,
	.close =	snd_cs4281_midi_output_close,
	.trigger =	snd_cs4281_midi_output_trigger,
};

static struct snd_rawmidi_ops snd_cs4281_midi_input =
{
	.open = 	snd_cs4281_midi_input_open,
	.close =	snd_cs4281_midi_input_close,
	.trigger =	snd_cs4281_midi_input_trigger,
};

static int snd_cs4281_midi(struct cs4281 *chip, int device,
			   struct snd_rawmidi **rrawmidi)
{
	struct snd_rawmidi *rmidi;
	int err;

	if (rrawmidi)
		*rrawmidi = NULL;
	if ((err = snd_rawmidi_new(chip->card, "CS4281", device, 1, 1, &rmidi)) < 0)
		return err;
	strcpy(rmidi->name, "CS4281");
	snd_rawmidi_set_ops(rmidi, SNDRV_RAWMIDI_STREAM_OUTPUT, &snd_cs4281_midi_output);
	snd_rawmidi_set_ops(rmidi, SNDRV_RAWMIDI_STREAM_INPUT, &snd_cs4281_midi_input);
	rmidi->info_flags |= SNDRV_RAWMIDI_INFO_OUTPUT | SNDRV_RAWMIDI_INFO_INPUT | SNDRV_RAWMIDI_INFO_DUPLEX;
	rmidi->private_data = chip;
	chip->rmidi = rmidi;
	if (rrawmidi)
		*rrawmidi = rmidi;
	return 0;
}

/*
                     
 */

static irqreturn_t snd_cs4281_interrupt(int irq, void *dev_id)
{
	struct cs4281 *chip = dev_id;
	unsigned int status, dma, val;
	struct cs4281_dma *cdma;

	if (chip == NULL)
		return IRQ_NONE;
	status = snd_cs4281_peekBA0(chip, BA0_HISR);
	if ((status & 0x7fffffff) == 0) {
		snd_cs4281_pokeBA0(chip, BA0_HICR, BA0_HICR_EOI);
		return IRQ_NONE;
	}

	if (status & (BA0_HISR_DMA(0)|BA0_HISR_DMA(1)|BA0_HISR_DMA(2)|BA0_HISR_DMA(3))) {
		for (dma = 0; dma < 4; dma++)
			if (status & BA0_HISR_DMA(dma)) {
				cdma = &chip->dma[dma];
				spin_lock(&chip->reg_lock);
				/*             */
				val = snd_cs4281_peekBA0(chip, cdma->regHDSR);
				/*                                           */
				/*                                       */
				cdma->frag++;
				if ((val & BA0_HDSR_DHTC) && !(cdma->frag & 1)) {
					cdma->frag--;
					chip->spurious_dhtc_irq++;
					spin_unlock(&chip->reg_lock);
					continue;
				}
				if ((val & BA0_HDSR_DTC) && (cdma->frag & 1)) {
					cdma->frag--;
					chip->spurious_dtc_irq++;
					spin_unlock(&chip->reg_lock);
					continue;
				}
				spin_unlock(&chip->reg_lock);
				snd_pcm_period_elapsed(cdma->substream);
			}
	}

	if ((status & BA0_HISR_MIDI) && chip->rmidi) {
		unsigned char c;
		
		spin_lock(&chip->reg_lock);
		while ((snd_cs4281_peekBA0(chip, BA0_MIDSR) & BA0_MIDSR_RBE) == 0) {
			c = snd_cs4281_peekBA0(chip, BA0_MIDRP);
			if ((chip->midcr & BA0_MIDCR_RIE) == 0)
				continue;
			snd_rawmidi_receive(chip->midi_input, &c, 1);
		}
		while ((snd_cs4281_peekBA0(chip, BA0_MIDSR) & BA0_MIDSR_TBF) == 0) {
			if ((chip->midcr & BA0_MIDCR_TIE) == 0)
				break;
			if (snd_rawmidi_transmit(chip->midi_output, &c, 1) != 1) {
				chip->midcr &= ~BA0_MIDCR_TIE;
				snd_cs4281_pokeBA0(chip, BA0_MIDCR, chip->midcr);
				break;
			}
			snd_cs4281_pokeBA0(chip, BA0_MIDWP, c);
		}
		spin_unlock(&chip->reg_lock);
	}

	/*                                             */
	snd_cs4281_pokeBA0(chip, BA0_HICR, BA0_HICR_EOI);

	return IRQ_HANDLED;
}


/*
               
 */
static void snd_cs4281_opl3_command(struct snd_opl3 *opl3, unsigned short cmd,
				    unsigned char val)
{
	unsigned long flags;
	struct cs4281 *chip = opl3->private_data;
	void __iomem *port;

	if (cmd & OPL3_RIGHT)
		port = chip->ba0 + BA0_B1AP; /*            */
	else
		port = chip->ba0 + BA0_B0AP; /*           */

	spin_lock_irqsave(&opl3->reg_lock, flags);

	writel((unsigned int)cmd, port);
	udelay(10);

	writel((unsigned int)val, port + 4);
	udelay(30);

	spin_unlock_irqrestore(&opl3->reg_lock, flags);
}

static int snd_cs4281_probe(struct pci_dev *pci,
			    const struct pci_device_id *pci_id)
{
	static int dev;
	struct snd_card *card;
	struct cs4281 *chip;
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

	if ((err = snd_cs4281_create(card, pci, &chip, dual_codec[dev])) < 0) {
		snd_card_free(card);
		return err;
	}
	card->private_data = chip;

	if ((err = snd_cs4281_mixer(chip)) < 0) {
		snd_card_free(card);
		return err;
	}
	if ((err = snd_cs4281_pcm(chip, 0, NULL)) < 0) {
		snd_card_free(card);
		return err;
	}
	if ((err = snd_cs4281_midi(chip, 0, NULL)) < 0) {
		snd_card_free(card);
		return err;
	}
	if ((err = snd_opl3_new(card, OPL3_HW_OPL3_CS4281, &opl3)) < 0) {
		snd_card_free(card);
		return err;
	}
	opl3->private_data = chip;
	opl3->command = snd_cs4281_opl3_command;
	snd_opl3_init(opl3);
	if ((err = snd_opl3_hwdep_new(opl3, 0, 1, NULL)) < 0) {
		snd_card_free(card);
		return err;
	}
	snd_cs4281_create_gameport(chip);
	strcpy(card->driver, "CS4281");
	strcpy(card->shortname, "Cirrus Logic CS4281");
	sprintf(card->longname, "%s at 0x%lx, irq %d",
		card->shortname,
		chip->ba0_addr,
		chip->irq);

	if ((err = snd_card_register(card)) < 0) {
		snd_card_free(card);
		return err;
	}

	pci_set_drvdata(pci, card);
	dev++;
	return 0;
}

static void snd_cs4281_remove(struct pci_dev *pci)
{
	snd_card_free(pci_get_drvdata(pci));
	pci_set_drvdata(pci, NULL);
}

/*
                   
 */
#ifdef CONFIG_PM_SLEEP

static int saved_regs[SUSPEND_REGISTERS] = {
	BA0_JSCTL,
	BA0_GPIOR,
	BA0_SSCR,
	BA0_MIDCR,
	BA0_SRCSA,
	BA0_PASR,
	BA0_CASR,
	BA0_DACSR,
	BA0_ADCSR,
	BA0_FMLVC,
	BA0_FMRVC,
	BA0_PPLVC,
	BA0_PPRVC,
};

#define CLKCR1_CKRA                             0x00010000L

static int cs4281_suspend(struct device *dev)
{
	struct pci_dev *pci = to_pci_dev(dev);
	struct snd_card *card = dev_get_drvdata(dev);
	struct cs4281 *chip = card->private_data;
	u32 ulCLK;
	unsigned int i;

	snd_power_change_state(card, SNDRV_CTL_POWER_D3hot);
	snd_pcm_suspend_all(chip->pcm);

	snd_ac97_suspend(chip->ac97);
	snd_ac97_suspend(chip->ac97_secondary);

	ulCLK = snd_cs4281_peekBA0(chip, BA0_CLKCR1);
	ulCLK |= CLKCR1_CKRA;
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, ulCLK);

	/*                     */
	snd_cs4281_pokeBA0(chip, BA0_HICR, BA0_HICR_CHGM);

	/*                               */
	for (i = 0; i < ARRAY_SIZE(saved_regs); i++)
		if (saved_regs[i])
			chip->suspend_regs[i] = snd_cs4281_peekBA0(chip, saved_regs[i]);

	/*                            */
	snd_cs4281_pokeBA0(chip, BA0_SERMC, 0);

	/*                                  */
	snd_cs4281_pokeBA0(chip, BA0_SSPM, 0);

	/*          */
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, 0);

	/*              */
	snd_cs4281_pokeBA0(chip, BA0_SPMC, 0);

	ulCLK = snd_cs4281_peekBA0(chip, BA0_CLKCR1);
	ulCLK &= ~CLKCR1_CKRA;
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, ulCLK);

	pci_disable_device(pci);
	pci_save_state(pci);
	pci_set_power_state(pci, PCI_D3hot);
	return 0;
}

static int cs4281_resume(struct device *dev)
{
	struct pci_dev *pci = to_pci_dev(dev);
	struct snd_card *card = dev_get_drvdata(dev);
	struct cs4281 *chip = card->private_data;
	unsigned int i;
	u32 ulCLK;

	pci_set_power_state(pci, PCI_D0);
	pci_restore_state(pci);
	if (pci_enable_device(pci) < 0) {
		printk(KERN_ERR "cs4281: pci_enable_device failed, "
		       "disabling device\n");
		snd_card_disconnect(card);
		return -EIO;
	}
	pci_set_master(pci);

	ulCLK = snd_cs4281_peekBA0(chip, BA0_CLKCR1);
	ulCLK |= CLKCR1_CKRA;
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, ulCLK);

	snd_cs4281_chip_init(chip);

	/*                              */
	for (i = 0; i < ARRAY_SIZE(saved_regs); i++)
		if (saved_regs[i])
			snd_cs4281_pokeBA0(chip, saved_regs[i], chip->suspend_regs[i]);

	snd_ac97_resume(chip->ac97);
	snd_ac97_resume(chip->ac97_secondary);

	ulCLK = snd_cs4281_peekBA0(chip, BA0_CLKCR1);
	ulCLK &= ~CLKCR1_CKRA;
	snd_cs4281_pokeBA0(chip, BA0_CLKCR1, ulCLK);

	snd_power_change_state(card, SNDRV_CTL_POWER_D0);
	return 0;
}

static SIMPLE_DEV_PM_OPS(cs4281_pm, cs4281_suspend, cs4281_resume);
#define CS4281_PM_OPS	&cs4281_pm
#else
#define CS4281_PM_OPS	NULL
#endif /*                 */

static struct pci_driver cs4281_driver = {
	.name = KBUILD_MODNAME,
	.id_table = snd_cs4281_ids,
	.probe = snd_cs4281_probe,
	.remove = snd_cs4281_remove,
	.driver = {
		.pm = CS4281_PM_OPS,
	},
};
	
module_pci_driver(cs4281_driver);