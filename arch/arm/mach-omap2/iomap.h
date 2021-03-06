/*
 * IO mappings for OMAP2+
 *
 * IO definitions for TI OMAP processors and boards
 *
 * Copied from arch/arm/mach-sa1100/include/mach/io.h
 * Copyright (C) 1997-1999 Russell King
 *
 * Copyright (C) 2009-2012 Texas Instruments
 * Added OMAP4/5 support - Santosh Shilimkar <santosh.shilimkar@ti.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#define OMAP2_L3_IO_OFFSET	0x90000000
#define OMAP2_L3_IO_ADDRESS(pa)	IOMEM((pa) + OMAP2_L3_IO_OFFSET) /*    */

#define OMAP2_L4_IO_OFFSET	0xb2000000
#define OMAP2_L4_IO_ADDRESS(pa)	IOMEM((pa) + OMAP2_L4_IO_OFFSET) /*    */

#define OMAP4_L3_IO_OFFSET	0xb4000000
#define OMAP4_L3_IO_ADDRESS(pa)	IOMEM((pa) + OMAP4_L3_IO_OFFSET) /*    */

#define AM33XX_L4_WK_IO_OFFSET	0xb5000000
#define AM33XX_L4_WK_IO_ADDRESS(pa)	IOMEM((pa) + AM33XX_L4_WK_IO_OFFSET)

#define OMAP4_L3_PER_IO_OFFSET	0xb1100000
#define OMAP4_L3_PER_IO_ADDRESS(pa)	IOMEM((pa) + OMAP4_L3_PER_IO_OFFSET)

#define OMAP2_EMU_IO_OFFSET		0xaa800000	/*           */
#define OMAP2_EMU_IO_ADDRESS(pa)	IOMEM((pa) + OMAP2_EMU_IO_OFFSET)

/*
                                                                               
                            
                                                                               
 */

/*                                */
#define L3_24XX_PHYS	L3_24XX_BASE	/*                          */
#define L3_24XX_VIRT	(L3_24XX_PHYS + OMAP2_L3_IO_OFFSET)
#define L3_24XX_SIZE	SZ_1M		/*                                   */
#define L4_24XX_PHYS	L4_24XX_BASE	/*                           */
#define L4_24XX_VIRT	(L4_24XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_24XX_SIZE	SZ_1M		/*                                  */

#define L4_WK_243X_PHYS		L4_WK_243X_BASE	/*                           */
#define L4_WK_243X_VIRT		(L4_WK_243X_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_WK_243X_SIZE		SZ_1M
#define OMAP243X_GPMC_PHYS	OMAP243X_GPMC_BASE
#define OMAP243X_GPMC_VIRT	(OMAP243X_GPMC_PHYS + OMAP2_L3_IO_OFFSET)
						/*                           */
#define OMAP243X_GPMC_SIZE	SZ_1M
#define OMAP243X_SDRC_PHYS	OMAP243X_SDRC_BASE
						/*                           */
#define OMAP243X_SDRC_VIRT	(OMAP243X_SDRC_PHYS + OMAP2_L3_IO_OFFSET)
#define OMAP243X_SDRC_SIZE	SZ_1M
#define OMAP243X_SMS_PHYS	OMAP243X_SMS_BASE
						/*                           */
#define OMAP243X_SMS_VIRT	(OMAP243X_SMS_PHYS + OMAP2_L3_IO_OFFSET)
#define OMAP243X_SMS_SIZE	SZ_1M

/*          */
#define DSP_MEM_2420_PHYS	OMAP2420_DSP_MEM_BASE
						/*                           */
#define DSP_MEM_2420_VIRT	0xfc100000
#define DSP_MEM_2420_SIZE	0x28000
#define DSP_IPI_2420_PHYS	OMAP2420_DSP_IPI_BASE
						/*                           */
#define DSP_IPI_2420_VIRT	0xfc128000
#define DSP_IPI_2420_SIZE	SZ_4K
#define DSP_MMU_2420_PHYS	OMAP2420_DSP_MMU_BASE
						/*                           */
#define DSP_MMU_2420_VIRT	0xfc129000
#define DSP_MMU_2420_SIZE	SZ_4K

/*                                  */

/*
                                                                               
                            
                                                                               
 */

/*                                */
#define L3_34XX_PHYS		L3_34XX_BASE	/*                           */
#define L3_34XX_VIRT		(L3_34XX_PHYS + OMAP2_L3_IO_OFFSET)
#define L3_34XX_SIZE		SZ_1M   /*                                   */

#define L4_34XX_PHYS		L4_34XX_BASE	/*                           */
#define L4_34XX_VIRT		(L4_34XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_34XX_SIZE		SZ_4M   /*                                  */

/*
                                                                               
                             
                                                                               
 */
#define L4_WK_AM33XX_PHYS	L4_WK_AM33XX_BASE
#define L4_WK_AM33XX_VIRT	(L4_WK_AM33XX_PHYS + AM33XX_L4_WK_IO_OFFSET)
#define L4_WK_AM33XX_SIZE	SZ_4M   /*                                  */

/*
                                      
                                              
 */

#define L4_PER_34XX_PHYS	L4_PER_34XX_BASE
						/*                           */
#define L4_PER_34XX_VIRT	(L4_PER_34XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_PER_34XX_SIZE	SZ_1M

#define L4_EMU_34XX_PHYS	L4_EMU_34XX_BASE
						/*                           */
#define L4_EMU_34XX_VIRT	(L4_EMU_34XX_PHYS + OMAP2_EMU_IO_OFFSET)
#define L4_EMU_34XX_SIZE	SZ_8M

#define OMAP34XX_GPMC_PHYS	OMAP34XX_GPMC_BASE
						/*                           */
#define OMAP34XX_GPMC_VIRT	(OMAP34XX_GPMC_PHYS + OMAP2_L3_IO_OFFSET)
#define OMAP34XX_GPMC_SIZE	SZ_1M

#define OMAP343X_SMS_PHYS	OMAP343X_SMS_BASE
						/*                           */
#define OMAP343X_SMS_VIRT	(OMAP343X_SMS_PHYS + OMAP2_L3_IO_OFFSET)
#define OMAP343X_SMS_SIZE	SZ_1M

#define OMAP343X_SDRC_PHYS	OMAP343X_SDRC_BASE
						/*                           */
#define OMAP343X_SDRC_VIRT	(OMAP343X_SDRC_PHYS + OMAP2_L3_IO_OFFSET)
#define OMAP343X_SDRC_SIZE	SZ_1M

/*                               */

/*
                                                                               
                            
                                                                               
 */

/*                                */
#define L3_44XX_PHYS		L3_44XX_BASE	/*                           */
#define L3_44XX_VIRT		(L3_44XX_PHYS + OMAP4_L3_IO_OFFSET)
#define L3_44XX_SIZE		SZ_1M

#define L4_44XX_PHYS		L4_44XX_BASE	/*                           */
#define L4_44XX_VIRT		(L4_44XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_44XX_SIZE		SZ_4M

#define L4_PER_44XX_PHYS	L4_PER_44XX_BASE
						/*                           */
#define L4_PER_44XX_VIRT	(L4_PER_44XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_PER_44XX_SIZE	SZ_4M

#define L4_ABE_44XX_PHYS	L4_ABE_44XX_BASE
						/*                           */
#define L4_ABE_44XX_VIRT	(L4_ABE_44XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_ABE_44XX_SIZE	SZ_1M
/*
                                                                               
                            
                                                                               
 */
#define L3_54XX_PHYS		L3_54XX_BASE	/*                           */
#define L3_54XX_VIRT		(L3_54XX_PHYS + OMAP4_L3_IO_OFFSET)
#define L3_54XX_SIZE		SZ_1M

#define L4_54XX_PHYS		L4_54XX_BASE	/*                           */
#define L4_54XX_VIRT		(L4_54XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_54XX_SIZE		SZ_4M

#define L4_WK_54XX_PHYS		L4_WK_54XX_BASE	/*                           */
#define L4_WK_54XX_VIRT		(L4_WK_54XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_WK_54XX_SIZE		SZ_2M

#define L4_PER_54XX_PHYS	L4_PER_54XX_BASE /*                           */
#define L4_PER_54XX_VIRT	(L4_PER_54XX_PHYS + OMAP2_L4_IO_OFFSET)
#define L4_PER_54XX_SIZE	SZ_4M
