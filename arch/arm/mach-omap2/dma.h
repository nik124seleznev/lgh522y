/*
 *  OMAP2PLUS DMA channel definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __OMAP2PLUS_DMA_CHANNEL_H
#define __OMAP2PLUS_DMA_CHANNEL_H


/*                       */
#define OMAP24XX_DMA_NO_DEVICE		0
#define OMAP24XX_DMA_EXT_DMAREQ0	2	/*         */
#define OMAP24XX_DMA_EXT_DMAREQ1	3	/*         */
#define OMAP24XX_DMA_GPMC		4	/*         */
#define OMAP24XX_DMA_AES_TX		9	/*         */
#define OMAP24XX_DMA_AES_RX		10	/*         */
#define OMAP242X_DMA_EXT_DMAREQ2	14	/*          */
#define OMAP242X_DMA_EXT_DMAREQ3	15	/*          */
#define OMAP242X_DMA_EXT_DMAREQ4	16	/*          */
#define OMAP34XX_DMA_I2C3_TX		25	/*          */
#define OMAP34XX_DMA_I2C3_RX		26	/*          */
#define OMAP24XX_DMA_I2C1_TX		27	/*          */
#define OMAP24XX_DMA_I2C1_RX		28	/*          */
#define OMAP24XX_DMA_I2C2_TX		29	/*          */
#define OMAP24XX_DMA_I2C2_RX		30	/*          */
#define OMAP24XX_DMA_MMC2_TX		47	/*          */
#define OMAP24XX_DMA_MMC2_RX		48	/*          */
#define OMAP24XX_DMA_UART1_TX		49	/*          */
#define OMAP24XX_DMA_UART1_RX		50	/*          */
#define OMAP24XX_DMA_UART2_TX		51	/*          */
#define OMAP24XX_DMA_UART2_RX		52	/*          */
#define OMAP24XX_DMA_UART3_TX		53	/*          */
#define OMAP24XX_DMA_UART3_RX		54	/*          */
#define OMAP24XX_DMA_MMC1_TX		61	/*          */
#define OMAP24XX_DMA_MMC1_RX		62	/*          */
#define OMAP242X_DMA_EXT_DMAREQ5	64	/*          */
#define OMAP34XX_DMA_AES2_TX		65	/*          */
#define OMAP34XX_DMA_AES2_RX		66	/*          */
#define OMAP34XX_DMA_SHA1MD5_RX		69	/*          */

#define OMAP36XX_DMA_UART4_TX		81	/*          */
#define OMAP36XX_DMA_UART4_RX		82	/*          */

/*                 */
#define AM35XX_DMA_UART4_TX		54
#define AM35XX_DMA_UART4_RX		55

#endif /*                           */
