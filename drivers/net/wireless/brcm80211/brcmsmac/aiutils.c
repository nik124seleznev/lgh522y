/*
 * Copyright (c) 2010 Broadcom Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * File contents: support functions for PCI/PCIe
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/delay.h>

#include <defs.h>
#include <chipcommon.h>
#include <brcmu_utils.h>
#include <brcm_hw_ids.h>
#include <soc.h>
#include "types.h"
#include "pub.h"
#include "pmu.h"
#include "aiutils.h"

/*              */
 /*                        */
#define SCC_SS_MASK		0x00000007
 /*                             */
#define	SCC_SS_LPO		0x00000000
 /*                                 */
#define	SCC_SS_XTAL		0x00000001
 /*                             */
#define	SCC_SS_PCI		0x00000002
 /*                                 */
#define SCC_LF			0x00000200
 /*                                                     */
#define SCC_LP			0x00000400
 /*                                                                         */
#define SCC_FS			0x00000800
 /*                      
                                                                    
  */
#define SCC_IP			0x00001000
 /*                    
                                                              
  */
#define SCC_XC			0x00002000
 /*                                            */
#define SCC_XP			0x00004000
 /*                                        */
#define SCC_CD_MASK		0xffff0000
#define SCC_CD_SHIFT		16

/*                */
 /*                              */
#define	SYCC_IE			0x00000001
 /*                                */
#define	SYCC_AE			0x00000002
 /*            */
#define	SYCC_FP			0x00000004
 /*                                      */
#define	SYCC_AR			0x00000008
 /*          */
#define	SYCC_HR			0x00000010
 /*                                      */
#define SYCC_CD_MASK		0xffff0000
#define SYCC_CD_SHIFT		16

#define CST4329_SPROM_OTP_SEL_MASK	0x00000003
 /*                                           */
#define CST4329_DEFCIS_SEL		0
 /*                                     */
#define CST4329_SPROM_SEL		1
 /*                             */
#define CST4329_OTP_SEL			2
 /*                                       */
#define CST4329_OTP_PWRDN		3

#define CST4329_SPI_SDIO_MODE_MASK	0x00000004
#define CST4329_SPI_SDIO_MODE_SHIFT	2

/*                                               */
#define CCTRL43224_GPIO_TOGGLE          0x8000
 /*                      */
#define CCTRL_43224A0_12MA_LED_DRIVE    0x00F000F0
 /*                                       */
#define CCTRL_43224B0_12MA_LED_DRIVE    0xF0

/*                                              */
#define CST43236_SFLASH_MASK		0x00000040
#define CST43236_OTP_MASK		0x00000080
#define CST43236_HSIC_MASK		0x00000100	/*          */
#define CST43236_BP_CLK			0x00000200	/*            */
#define CST43236_BOOT_MASK		0x00001800
#define CST43236_BOOT_SHIFT		11
#define CST43236_BOOT_FROM_SRAM		0 /*                              */
#define CST43236_BOOT_FROM_ROM		1 /*               */
#define CST43236_BOOT_FROM_FLASH	2 /*                 */
#define CST43236_BOOT_FROM_INVALID	3

/*                                              */
 /*           */
#define CCTRL4331_BT_COEXIST		(1<<0)
 /*                                      */
#define CCTRL4331_SECI			(1<<1)
 /*           */
#define CCTRL4331_EXT_LNA		(1<<2)
 /*                     */
#define CCTRL4331_SPROM_GPIO13_15       (1<<3)
 /*                                    */
#define CCTRL4331_EXTPA_EN		(1<<4)
 /*                                        */
#define CCTRL4331_GPIOCLK_ON_SPROMCS	(1<<5)
 /*                                         */
#define CCTRL4331_PCIE_MDIO_ON_SPROMCS	(1<<6)
 /*                                               */
#define CCTRL4331_EXTPA_ON_GPIO2_5	(1<<7)
 /*                                            */
#define CCTRL4331_OVR_PIPEAUXCLKEN	(1<<8)
 /*                                            */
#define CCTRL4331_OVR_PIPEAUXPWRDOWN	(1<<9)
 /*                   */
#define CCTRL4331_PCIE_AUXCLKEN		(1<<10)
 /*                        */
#define CCTRL4331_PCIE_PIPE_PLLDOWN	(1<<11)
 /*                         */
#define CCTRL4331_BT_SHD0_ON_GPIO4	(1<<16)
 /*                         */
#define CCTRL4331_BT_SHD1_ON_GPIO5	(1<<17)

/*                                             */
 /*                            */
#define	CST4331_XTAL_FREQ		0x00000001
#define	CST4331_SPROM_PRESENT		0x00000002
#define	CST4331_OTP_PRESENT		0x00000004
#define	CST4331_LDO_RF			0x00000008
#define	CST4331_LDO_PAR			0x00000010

/*                                             */
#define	CST4319_SPI_CPULESSUSB		0x00000001
#define	CST4319_SPI_CLK_POL		0x00000002
#define	CST4319_SPI_CLK_PH		0x00000008
 /*                                */
#define	CST4319_SPROM_OTP_SEL_MASK	0x000000c0
#define	CST4319_SPROM_OTP_SEL_SHIFT	6
 /*                                    */
#define	CST4319_DEFCIS_SEL		0x00000000
 /*                              */
#define	CST4319_SPROM_SEL		0x00000040
 /*                            */
#define	CST4319_OTP_SEL			0x00000080
 /*                                */
#define	CST4319_OTP_PWRDN		0x000000c0
 /*                         */
#define	CST4319_SDIO_USB_MODE		0x00000100
#define	CST4319_REMAP_SEL_MASK		0x00000600
#define	CST4319_ILPDIV_EN		0x00000800
#define	CST4319_XTAL_PD_POL		0x00001000
#define	CST4319_LPO_SEL			0x00002000
#define	CST4319_RES_INIT_MODE		0x0000c000
 /*                                       */
#define	CST4319_PALDO_EXTPNP		0x00010000
#define	CST4319_CBUCK_MODE_MASK		0x00060000
#define CST4319_CBUCK_MODE_BURST	0x00020000
#define CST4319_CBUCK_MODE_LPBURST	0x00060000
#define	CST4319_RCAL_VALID		0x01000000
#define	CST4319_RCAL_VALUE_MASK		0x3e000000
#define	CST4319_RCAL_VALUE_SHIFT	25

/*                                             */
#define	CST4336_SPI_MODE_MASK		0x00000001
#define	CST4336_SPROM_PRESENT		0x00000002
#define	CST4336_OTP_PRESENT		0x00000004
#define	CST4336_ARMREMAP_0		0x00000008
#define	CST4336_ILPDIV_EN_MASK		0x00000010
#define	CST4336_ILPDIV_EN_SHIFT		4
#define	CST4336_XTAL_PD_POL_MASK	0x00000020
#define	CST4336_XTAL_PD_POL_SHIFT	5
#define	CST4336_LPO_SEL_MASK		0x00000040
#define	CST4336_LPO_SEL_SHIFT		6
#define	CST4336_RES_INIT_MODE_MASK	0x00000180
#define	CST4336_RES_INIT_MODE_SHIFT	7
#define	CST4336_CBUCK_MODE_MASK		0x00000600
#define	CST4336_CBUCK_MODE_SHIFT	9

/*                                             */
#define	CST4313_SPROM_PRESENT			1
#define	CST4313_OTP_PRESENT			2
#define	CST4313_SPROM_OTP_SEL_MASK		0x00000002
#define	CST4313_SPROM_OTP_SEL_SHIFT		0

/*                                              */
 /*                                    */
#define CCTRL_4313_12MA_LED_DRIVE    0x00000007

/*                  */
#define	MFGID_ARM		0x43b
#define	MFGID_BRCM		0x4bf
#define	MFGID_MIPS		0x4a7

/*                           */
#define	ER_EROMENTRY		0x000
#define	ER_REMAPCONTROL		0xe00
#define	ER_REMAPSELECT		0xe04
#define	ER_MASTERSELECT		0xe10
#define	ER_ITCR			0xf00
#define	ER_ITIP			0xf04

/*              */
#define	ER_TAG			0xe
#define	ER_TAG1			0x6
#define	ER_VALID		1
#define	ER_CI			0
#define	ER_MP			2
#define	ER_ADD			4
#define	ER_END			0xe
#define	ER_BAD			0xffffffff

/*                 */
#define	CIA_MFG_MASK		0xfff00000
#define	CIA_MFG_SHIFT		20
#define	CIA_CID_MASK		0x000fff00
#define	CIA_CID_SHIFT		8
#define	CIA_CCL_MASK		0x000000f0
#define	CIA_CCL_SHIFT		4

/*                 */
#define	CIB_REV_MASK		0xff000000
#define	CIB_REV_SHIFT		24
#define	CIB_NSW_MASK		0x00f80000
#define	CIB_NSW_SHIFT		19
#define	CIB_NMW_MASK		0x0007c000
#define	CIB_NMW_SHIFT		14
#define	CIB_NSP_MASK		0x00003e00
#define	CIB_NSP_SHIFT		9
#define	CIB_NMP_MASK		0x000001f0
#define	CIB_NMP_SHIFT		4

/*               */
#define	AD_ADDR_MASK		0xfffff000
#define	AD_SP_MASK		0x00000f00
#define	AD_SP_SHIFT		8
#define	AD_ST_MASK		0x000000c0
#define	AD_ST_SHIFT		6
#define	AD_ST_SLAVE		0x00000000
#define	AD_ST_BRIDGE		0x00000040
#define	AD_ST_SWRAP		0x00000080
#define	AD_ST_MWRAP		0x000000c0
#define	AD_SZ_MASK		0x00000030
#define	AD_SZ_SHIFT		4
#define	AD_SZ_4K		0x00000000
#define	AD_SZ_8K		0x00000010
#define	AD_SZ_16K		0x00000020
#define	AD_SZ_SZD		0x00000030
#define	AD_AG32			0x00000008
#define	AD_ADDR_ALIGN		0x00000fff
#define	AD_SZ_BASE		0x00001000	/*     */

/*               */
#define	SD_SZ_MASK		0xfffff000
#define	SD_SG32			0x00000008
#define	SD_SZ_ALIGN		0x00000fff

/*                                                     */
#define	PCI_CFG_GPIO_SCS	0x10
/*                                            */
#define PCI_CFG_GPIO_XTAL	0x40
/*                                             */
#define PCI_CFG_GPIO_PLL	0x80

/*                       */
#define PLL_DELAY		150	/*                 */
#define FREF_DELAY		200	/*                      */
#define	XTAL_ON_DELAY		1000	/*                           */

/*           */
#define	AIRC_RESET		1

#define	NOREV		-1	/*             */

/*                                  */
#define DEFAULT_GPIO_ONTIME	10	/*                 */
#define DEFAULT_GPIO_OFFTIME	90	/*                 */

/*                                                  */
#define	SRC_START		0x80000000
#define	SRC_BUSY		0x80000000
#define	SRC_OPCODE		0x60000000
#define	SRC_OP_READ		0x00000000
#define	SRC_OP_WRITE		0x20000000
#define	SRC_OP_WRDIS		0x40000000
#define	SRC_OP_WREN		0x60000000
#define	SRC_OTPSEL		0x00000010
#define	SRC_LOCK		0x00000008
#define	SRC_SIZE_MASK		0x00000006
#define	SRC_SIZE_1K		0x00000000
#define	SRC_SIZE_4K		0x00000002
#define	SRC_SIZE_16K		0x00000004
#define	SRC_SIZE_SHIFT		1
#define	SRC_PRESENT		0x00000001

/*                         */
#define GPIO_CTRL_EPA_EN_MASK 0x40

#define DEFAULT_GPIOTIMERVAL \
	((DEFAULT_GPIO_ONTIME << GPIO_ONTIME_SHIFT) | DEFAULT_GPIO_OFFTIME)

#define	BADIDX		(SI_MAXCORES + 1)

#define	IS_SIM(chippkg)	\
	((chippkg == HDLSIM_PKG_ID) || (chippkg == HWSIM_PKG_ID))

#define	GOODCOREADDR(x, b) \
	(((x) >= (b)) && ((x) < ((b) + SI_MAXCORES * SI_CORE_SIZE)) && \
		IS_ALIGNED((x), SI_CORE_SIZE))

struct aidmp {
	u32 oobselina30;	/*       */
	u32 oobselina74;	/*       */
	u32 PAD[6];
	u32 oobselinb30;	/*       */
	u32 oobselinb74;	/*       */
	u32 PAD[6];
	u32 oobselinc30;	/*       */
	u32 oobselinc74;	/*       */
	u32 PAD[6];
	u32 oobselind30;	/*       */
	u32 oobselind74;	/*       */
	u32 PAD[38];
	u32 oobselouta30;	/*       */
	u32 oobselouta74;	/*       */
	u32 PAD[6];
	u32 oobseloutb30;	/*       */
	u32 oobseloutb74;	/*       */
	u32 PAD[6];
	u32 oobseloutc30;	/*       */
	u32 oobseloutc74;	/*       */
	u32 PAD[6];
	u32 oobseloutd30;	/*       */
	u32 oobseloutd74;	/*       */
	u32 PAD[38];
	u32 oobsynca;	/*       */
	u32 oobseloutaen;	/*       */
	u32 PAD[6];
	u32 oobsyncb;	/*       */
	u32 oobseloutben;	/*       */
	u32 PAD[6];
	u32 oobsyncc;	/*       */
	u32 oobseloutcen;	/*       */
	u32 PAD[6];
	u32 oobsyncd;	/*       */
	u32 oobseloutden;	/*       */
	u32 PAD[38];
	u32 oobaextwidth;	/*       */
	u32 oobainwidth;	/*       */
	u32 oobaoutwidth;	/*       */
	u32 PAD[5];
	u32 oobbextwidth;	/*       */
	u32 oobbinwidth;	/*       */
	u32 oobboutwidth;	/*       */
	u32 PAD[5];
	u32 oobcextwidth;	/*       */
	u32 oobcinwidth;	/*       */
	u32 oobcoutwidth;	/*       */
	u32 PAD[5];
	u32 oobdextwidth;	/*       */
	u32 oobdinwidth;	/*       */
	u32 oobdoutwidth;	/*       */
	u32 PAD[37];
	u32 ioctrlset;	/*       */
	u32 ioctrlclear;	/*       */
	u32 ioctrl;		/*       */
	u32 PAD[61];
	u32 iostatus;	/*       */
	u32 PAD[127];
	u32 ioctrlwidth;	/*       */
	u32 iostatuswidth;	/*       */
	u32 PAD[62];
	u32 resetctrl;	/*       */
	u32 resetstatus;	/*       */
	u32 resetreadid;	/*       */
	u32 resetwriteid;	/*       */
	u32 PAD[60];
	u32 errlogctrl;	/*       */
	u32 errlogdone;	/*       */
	u32 errlogstatus;	/*       */
	u32 errlogaddrlo;	/*       */
	u32 errlogaddrhi;	/*       */
	u32 errlogid;	/*       */
	u32 errloguser;	/*       */
	u32 errlogflags;	/*       */
	u32 PAD[56];
	u32 intstatus;	/*       */
	u32 PAD[127];
	u32 config;		/*       */
	u32 PAD[63];
	u32 itcr;		/*       */
	u32 PAD[3];
	u32 itipooba;	/*       */
	u32 itipoobb;	/*       */
	u32 itipoobc;	/*       */
	u32 itipoobd;	/*       */
	u32 PAD[4];
	u32 itipoobaout;	/*       */
	u32 itipoobbout;	/*       */
	u32 itipoobcout;	/*       */
	u32 itipoobdout;	/*       */
	u32 PAD[4];
	u32 itopooba;	/*       */
	u32 itopoobb;	/*       */
	u32 itopoobc;	/*       */
	u32 itopoobd;	/*       */
	u32 PAD[4];
	u32 itopoobain;	/*       */
	u32 itopoobbin;	/*       */
	u32 itopoobcin;	/*       */
	u32 itopoobdin;	/*       */
	u32 PAD[4];
	u32 itopreset;	/*       */
	u32 PAD[15];
	u32 peripherialid4;	/*       */
	u32 peripherialid5;	/*       */
	u32 peripherialid6;	/*       */
	u32 peripherialid7;	/*       */
	u32 peripherialid0;	/*       */
	u32 peripherialid1;	/*       */
	u32 peripherialid2;	/*       */
	u32 peripherialid3;	/*       */
	u32 componentid0;	/*       */
	u32 componentid1;	/*       */
	u32 componentid2;	/*       */
	u32 componentid3;	/*       */
};

static bool
ai_buscore_setup(struct si_info *sii, struct bcma_device *cc)
{
	/*                          */
	if (cc->bus->nr_cores == 0)
		return false;

	/*                    */
	sii->pub.ccrev = cc->id.rev;

	/*                           */
	sii->chipst = bcma_read32(cc, CHIPCREGOFFS(chipstatus));

	/*                            */
	sii->pub.cccaps = bcma_read32(cc, CHIPCREGOFFS(capabilities));

	/*                      */
	if (ai_get_cccaps(&sii->pub) & CC_CAP_PMU) {
		sii->pub.pmucaps = bcma_read32(cc,
					       CHIPCREGOFFS(pmucapabilities));
		sii->pub.pmurev = sii->pub.pmucaps & PCAP_REV_MASK;
	}

	return true;
}

static struct si_info *ai_doattach(struct si_info *sii,
				   struct bcma_bus *pbus)
{
	struct si_pub *sih = &sii->pub;
	struct bcma_device *cc;

	sii->icbus = pbus;
	sii->pcibus = pbus->host_pci;

	/*                           */
	cc = pbus->drv_cc.core;

	sih->chip = pbus->chipinfo.id;
	sih->chiprev = pbus->chipinfo.rev;
	sih->chippkg = pbus->chipinfo.pkg;
	sih->boardvendor = pbus->boardinfo.vendor;
	sih->boardtype = pbus->boardinfo.type;

	if (!ai_buscore_setup(sii, cc))
		goto exit;

	/*                               */
	bcma_write32(cc, CHIPCREGOFFS(gpiopullup), 0);
	bcma_write32(cc, CHIPCREGOFFS(gpiopulldown), 0);

	/*                              */
	if (ai_get_cccaps(sih) & CC_CAP_PMU) {
		(void)si_pmu_measure_alpclk(sih);
	}

	return sii;

 exit:

	return NULL;
}

/*
                                          
 */
struct si_pub *
ai_attach(struct bcma_bus *pbus)
{
	struct si_info *sii;

	/*                      */
	sii = kzalloc(sizeof(struct si_info), GFP_ATOMIC);
	if (sii == NULL)
		return NULL;

	if (ai_doattach(sii, pbus) == NULL) {
		kfree(sii);
		return NULL;
	}

	return (struct si_pub *) sii;
}

/*                                  */
void ai_detach(struct si_pub *sih)
{
	struct si_info *sii;

	sii = container_of(sih, struct si_info, pub);

	if (sii == NULL)
		return;

	kfree(sii);
}

/*
                                        
 */
uint ai_cc_reg(struct si_pub *sih, uint regoff, u32 mask, u32 val)
{
	struct bcma_device *cc;
	u32 w;
	struct si_info *sii;

	sii = container_of(sih, struct si_info, pub);
	cc = sii->icbus->drv_cc.core;

	/*              */
	if (mask || val)
		bcma_maskset32(cc, regoff, ~mask, val);

	/*          */
	w = bcma_read32(cc, regoff);

	return w;
}

/*                                                  */
static uint ai_slowclk_src(struct si_pub *sih, struct bcma_device *cc)
{
	return SCC_SS_XTAL;
}

/*
                                                 
                                                                  
*/
static uint ai_slowclk_freq(struct si_pub *sih, bool max_freq,
			    struct bcma_device *cc)
{
	uint div;

	/*                            */
	div = bcma_read32(cc, CHIPCREGOFFS(system_clk_ctl));
	div = 4 * ((div >> SYCC_CD_SHIFT) + 1);
	return max_freq ? XTALMAXFREQ : (XTALMINFREQ / div);
}

static void
ai_clkctl_setdelay(struct si_pub *sih, struct bcma_device *cc)
{
	uint slowmaxfreq, pll_delay, slowclk;
	uint pll_on_delay, fref_sel_delay;

	pll_delay = PLL_DELAY;

	/*
                                                     
                                                     
                                              
  */

	slowclk = ai_slowclk_src(sih, cc);
	if (slowclk != SCC_SS_XTAL)
		pll_delay += XTAL_ON_DELAY;

	/*                                                          */
	slowmaxfreq =
	    ai_slowclk_freq(sih, false, cc);

	pll_on_delay = ((slowmaxfreq * pll_delay) + 999999) / 1000000;
	fref_sel_delay = ((slowmaxfreq * FREF_DELAY) + 999999) / 1000000;

	bcma_write32(cc, CHIPCREGOFFS(pll_on_delay), pll_on_delay);
	bcma_write32(cc, CHIPCREGOFFS(fref_sel_delay), fref_sel_delay);
}

/*                                          */
void ai_clkctl_init(struct si_pub *sih)
{
	struct si_info *sii = container_of(sih, struct si_info, pub);
	struct bcma_device *cc;

	if (!(ai_get_cccaps(sih) & CC_CAP_PWR_CTL))
		return;

	cc = sii->icbus->drv_cc.core;
	if (cc == NULL)
		return;

	/*                                    */
	bcma_maskset32(cc, CHIPCREGOFFS(system_clk_ctl), SYCC_CD_MASK,
		       (ILP_DIV_1MHZ << SYCC_CD_SHIFT));

	ai_clkctl_setdelay(sih, cc);
}

/*
                                               
                                       
 */
u16 ai_clkctl_fast_pwrup_delay(struct si_pub *sih)
{
	struct si_info *sii;
	struct bcma_device *cc;
	uint slowminfreq;
	u16 fpdelay;

	sii = container_of(sih, struct si_info, pub);
	if (ai_get_cccaps(sih) & CC_CAP_PMU) {
		fpdelay = si_pmu_fast_pwrup_delay(sih);
		return fpdelay;
	}

	if (!(ai_get_cccaps(sih) & CC_CAP_PWR_CTL))
		return 0;

	fpdelay = 0;
	cc = sii->icbus->drv_cc.core;
	if (cc) {
		slowminfreq = ai_slowclk_freq(sih, false, cc);
		fpdelay = (((bcma_read32(cc, CHIPCREGOFFS(pll_on_delay)) + 2)
			    * 1000000) + (slowminfreq - 1)) / slowminfreq;
	}
	return fpdelay;
}

/*
                                                     
  
                                                                  
                                               
                                                       
                                                            
 */
bool ai_clkctl_cc(struct si_pub *sih, enum bcma_clkmode mode)
{
	struct si_info *sii;
	struct bcma_device *cc;

	sii = container_of(sih, struct si_info, pub);

	cc = sii->icbus->drv_cc.core;
	bcma_core_set_clockmode(cc, mode);
	return mode == BCMA_CLKMODE_FAST;
}

void ai_pci_up(struct si_pub *sih)
{
	struct si_info *sii;

	sii = container_of(sih, struct si_info, pub);

	if (sii->icbus->hosttype == BCMA_HOSTTYPE_PCI)
		bcma_core_pci_extend_L1timer(&sii->icbus->drv_pci[0], true);
}

/*                                                       */
void ai_pci_down(struct si_pub *sih)
{
	struct si_info *sii;

	sii = container_of(sih, struct si_info, pub);

	if (sii->icbus->hosttype == BCMA_HOSTTYPE_PCI)
		bcma_core_pci_extend_L1timer(&sii->icbus->drv_pci[0], false);
}

/*                                 */
void ai_epa_4313war(struct si_pub *sih)
{
	struct si_info *sii = container_of(sih, struct si_info, pub);
	struct bcma_device *cc;

	cc = sii->icbus->drv_cc.core;

	/*         */
	bcma_set32(cc, CHIPCREGOFFS(gpiocontrol), GPIO_CTRL_EPA_EN_MASK);
}

/*                                */
bool ai_deviceremoved(struct si_pub *sih)
{
	u32 w = 0;
	struct si_info *sii;

	sii = container_of(sih, struct si_info, pub);

	if (sii->icbus->hosttype != BCMA_HOSTTYPE_PCI)
		return false;

	pci_read_config_dword(sii->pcibus, PCI_VENDOR_ID, &w);
	if ((w & 0xFFFF) != PCI_VENDOR_ID_BROADCOM)
		return true;

	return false;
}
