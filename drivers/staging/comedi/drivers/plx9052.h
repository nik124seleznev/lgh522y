/*
    comedi/drivers/plx9052.h
    Definitions for the PLX-9052 PCI interface chip

    Copyright (C) 2002 MEV Ltd. <http://www.mev.co.uk/>

    COMEDI - Linux Control and Measurement Device Interface
    Copyright (C) 2000 David A. Schleef <ds@schleef.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef _PLX9052_H_
#define _PLX9052_H_

/*
                                             
 */
#define PLX9052_INTCSR			0x4c
#define PLX9052_INTCSR_LI1ENAB		(1 << 0)  /*             */
#define PLX9052_INTCSR_LI1POL		(1 << 1)  /*                 */
#define PLX9052_INTCSR_LI1STAT		(1 << 2)  /*            */
#define PLX9052_INTCSR_LI2ENAB		(1 << 3)  /*             */
#define PLX9052_INTCSR_LI2POL		(1 << 4)  /*                 */
#define PLX9052_INTCSR_LI2STAT		(1 << 5)  /*            */
#define PLX9052_INTCSR_PCIENAB		(1 << 6)  /*                */
#define PLX9052_INTCSR_SOFTINT		(1 << 7)  /*                   */
#define PLX9052_INTCSR_LI1SEL		(1 << 8)  /*          */
#define PLX9052_INTCSR_LI2SEL		(1 << 9)  /*          */
#define PLX9052_INTCSR_LI1CLRINT	(1 << 10) /*               */
#define PLX9052_INTCSR_LI2CLRINT	(1 << 11) /*               */
#define PLX9052_INTCSR_ISAMODE		(1 << 12) /*                    */

/*
                                                              
                                  
 */
#define PLX9052_CNTRL			0x50
#define PLX9052_CNTRL_WAITO		(1 << 0)  /*                       */
#define PLX9052_CNTRL_UIO0_DIR		(1 << 1)  /*                */
#define PLX9052_CNTRL_UIO0_DATA		(1 << 2)  /*           */
#define PLX9052_CNTRL_LLOCKO		(1 << 3)  /*                        */
#define PLX9052_CNTRL_UIO1_DIR		(1 << 4)  /*                */
#define PLX9052_CNTRL_UIO1_DATA		(1 << 5)  /*           */
#define PLX9052_CNTRL_CS2		(1 << 6)  /*                     */
#define PLX9052_CNTRL_UIO2_DIR		(1 << 7)  /*                */
#define PLX9052_CNTRL_UIO2_DATA		(1 << 8)  /*           */
#define PLX9052_CNTRL_CS3		(1 << 9)  /*                     */
#define PLX9052_CNTRL_UIO3_DIR		(1 << 10) /*                */
#define PLX9052_CNTRL_UIO3_DATA		(1 << 11) /*           */
#define PLX9052_CNTRL_PCIBAR01		(0 << 12) /*                         */
#define PLX9052_CNTRL_PCIBAR0		(1 << 12) /*                  */
#define PLX9052_CNTRL_PCIBAR1		(2 << 12) /*                  */
#define PLX9052_CNTRL_PCI2_1_FEATURES	(1 << 14) /*                           */
#define PLX9052_CNTRL_PCI_R_W_FLUSH	(1 << 15) /*                         */
#define PLX9052_CNTRL_PCI_R_NO_FLUSH	(1 << 16) /*                    */
#define PLX9052_CNTRL_PCI_R_NO_WRITE	(1 << 17) /*                    */
#define PLX9052_CNTRL_PCI_W_RELEASE	(1 << 18) /*                        */
#define PLX9052_CNTRL_RETRY_CLKS(x)	(((x) & 0xf) << 19) /*                  */
#define PLX9052_CNTRL_LOCK_ENAB		(1 << 23) /*                    */
#define PLX9052_CNTRL_EEPROM_MASK	(0x1f << 24) /*             */
#define PLX9052_CNTRL_EEPROM_CLK	(1 << 24) /*              */
#define PLX9052_CNTRL_EEPROM_CS		(1 << 25) /*                    */
#define PLX9052_CNTRL_EEPROM_DOUT	(1 << 26) /*                  */
#define PLX9052_CNTRL_EEPROM_DIN	(1 << 27) /*                 */
#define PLX9052_CNTRL_EEPROM_PRESENT	(1 << 28) /*                */
#define PLX9052_CNTRL_RELOAD_CFG	(1 << 29) /*                      */
#define PLX9052_CNTRL_PCI_RESET		(1 << 30) /*                   */
#define PLX9052_CNTRL_MASK_REV		(1 << 31) /*               */

#endif /*             */
