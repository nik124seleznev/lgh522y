/*                                                                          */

/*
 *	m54xxpci.h -- ColdFire 547x and 548x PCI bus support
 *
 *	(C) Copyright 2011,  Greg Ungerer <gerg@uclinux.org>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of this archive
 * for more details.
 */

/*                                                                          */
#ifndef	M54XXPCI_H
#define	M54XXPCI_H
/*                                                                          */

/*
                                                                         
 */
#define	PCIIDR		(CONFIG_MBAR + 0xb00)	/*                      */
#define	PCISCR		(CONFIG_MBAR + 0xb04)	/*                    */
#define	PCICCRIR	(CONFIG_MBAR + 0xb08)	/*                    */
#define	PCICR1		(CONFIG_MBAR + 0xb0c)	/*                     */
#define	PCIBAR0		(CONFIG_MBAR + 0xb10)	/*                    */
#define	PCIBAR1		(CONFIG_MBAR + 0xb14)	/*                    */
#define	PCICCPR		(CONFIG_MBAR + 0xb28)	/*                         */
#define	PCISID		(CONFIG_MBAR + 0xb2c)	/*                   */
#define	PCIERBAR	(CONFIG_MBAR + 0xb30)	/*                   */
#define	PCICPR		(CONFIG_MBAR + 0xb34)	/*                          */
#define	PCICR2		(CONFIG_MBAR + 0xb3c)	/*                     */

#define	PCIGSCR		(CONFIG_MBAR + 0xb60)	/*                       */
#define	PCITBATR0	(CONFIG_MBAR + 0xb64)	/*                           */
#define	PCITBATR1	(CONFIG_MBAR + 0xb68)	/*                           */
#define	PCITCR		(CONFIG_MBAR + 0xb6c)	/*                */
#define	PCIIW0BTAR	(CONFIG_MBAR + 0xb70)	/*                    */
#define	PCIIW1BTAR	(CONFIG_MBAR + 0xb74)	/*                    */
#define	PCIIW2BTAR	(CONFIG_MBAR + 0xb78)	/*                    */
#define	PCIIWCR		(CONFIG_MBAR + 0xb80)	/*                         */
#define	PCIICR		(CONFIG_MBAR + 0xb84)	/*                   */
#define	PCIISR		(CONFIG_MBAR + 0xb88)	/*                  */
#define	PCICAR		(CONFIG_MBAR + 0xbf8)	/*                       */

#define	PCITPSR		(CONFIG_MBAR + 0x8400)	/*                */
#define	PCITSAR		(CONFIG_MBAR + 0x8404)	/*                  */
#define	PCITTCR		(CONFIG_MBAR + 0x8408)	/*                        */
#define	PCITER		(CONFIG_MBAR + 0x840c)	/*            */
#define	PCITNAR		(CONFIG_MBAR + 0x8410)	/*                 */
#define	PCITLWR		(CONFIG_MBAR + 0x8414)	/*              */
#define	PCITDCR		(CONFIG_MBAR + 0x8418)	/*                */
#define	PCITSR		(CONFIG_MBAR + 0x841c)	/*           */
#define	PCITFDR		(CONFIG_MBAR + 0x8440)	/*              */
#define	PCITFSR		(CONFIG_MBAR + 0x8444)	/*                */
#define	PCITFCR		(CONFIG_MBAR + 0x8448)	/*                 */
#define	PCITFAR		(CONFIG_MBAR + 0x844c)	/*               */
#define	PCITFRPR	(CONFIG_MBAR + 0x8450)	/*                      */
#define	PCITFWPR	(CONFIG_MBAR + 0x8454)	/*                       */

#define	PCIRPSR		(CONFIG_MBAR + 0x8480)	/*                */
#define	PCIRSAR		(CONFIG_MBAR + 0x8484)	/*                  */
#define	PCIRTCR		(CONFIG_MBAR + 0x8488)	/*                        */
#define	PCIRER		(CONFIG_MBAR + 0x848c)	/*            */
#define	PCIRNAR		(CONFIG_MBAR + 0x8490)	/*                 */
#define	PCIRDCR		(CONFIG_MBAR + 0x8498)	/*                */
#define	PCIRSR		(CONFIG_MBAR + 0x849c)	/*           */
#define	PCIRFDR		(CONFIG_MBAR + 0x84c0)	/*              */
#define	PCIRFSR		(CONFIG_MBAR + 0x84c4)	/*                */
#define	PCIRFCR		(CONFIG_MBAR + 0x84c8)	/*                 */
#define	PCIRFAR		(CONFIG_MBAR + 0x84cc)	/*               */
#define	PCIRFRPR	(CONFIG_MBAR + 0x84d0)	/*                      */
#define	PCIRFWPR	(CONFIG_MBAR + 0x84d4)	/*                       */

#define	PACR		(CONFIG_MBAR + 0xc00)	/*                     */
#define	PASR		(COFNIG_MBAR + 0xc04)	/*                    */

/*
                                                          
 */
#define	PCIGSCR_PE	0x20000000		/*                       */
#define	PCIGSCR_SE	0x10000000		/*                       */
#define	PCIGSCR_XCLKBIN	0x07000000		/*                */
#define	PCIGSCR_PEE	0x00002000		/*                          */
#define	PCIGSCR_SEE	0x00001000		/*                          */
#define	PCIGSCR_RESET	0x00000001		/*           */

/*
                                                                 
 */
#define	PCICAR_E	0x80000000		/*                     */
#define	PCICAR_BUSN	16			/*               */
#define	PCICAR_DEVFNN	8			/*                 */
#define	PCICAR_DWORDN	0			/*                 */

/*
                                                                    
                                                                     
 */
#define	WXBTAR(hostaddr, pciaddr, size)	\
			(((hostaddr) & 0xff000000) | \
			((((size) - 1) & 0xff000000) >> 8) | \
			(((pciaddr) & 0xff000000) >> 16))

#define	PCIIWCR_W0_MEM	0x00000000		/*                    */
#define	PCIIWCR_W0_IO	0x08000000		/*                */
#define	PCIIWCR_W0_MRD	0x00000000		/*                      */
#define	PCIIWCR_W0_MRDL	0x02000000		/*                           */
#define	PCIIWCR_W0_MRDM	0x04000000		/*                           */
#define	PCIIWCR_W0_E	0x01000000		/*                 */

#define	PCIIWCR_W1_MEM	0x00000000		/*                    */
#define	PCIIWCR_W1_IO	0x00080000		/*                */
#define	PCIIWCR_W1_MRD	0x00000000		/*                      */
#define	PCIIWCR_W1_MRDL	0x00020000		/*                           */
#define	PCIIWCR_W1_MRDM	0x00040000		/*                           */
#define	PCIIWCR_W1_E	0x00010000		/*                 */

/*
                                             
 */
#define	PCITBATR0_E	0x00000001		/*                 */
#define	PCITBATR1_E	0x00000001		/*                 */

/*
                                              
 */
#define	PACR_INTMPRI	0x00000001
#define	PACR_EXTMPRI(x)	(((x) & 0x1f) << 1)
#define	PACR_INTMINTE	0x00010000
#define	PACR_EXTMINTE(x) (((x) & 0x1f) << 17)
#define	PACR_PKMD	0x40000000
#define	PACR_DS		0x80000000

#define	PCICR1_CL(x)	((x) & 0xf)		/*                      */
#define	PCICR1_LT(x)	(((x) & 0xff) << 8)	/*                     */

/*                                                                          */
#endif	/*            */
