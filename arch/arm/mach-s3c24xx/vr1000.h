
/* arch/arm/mach-s3c2410/include/mach/vr1000-cpld.h
 *
 * Copyright (c) 2003 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * VR1000 - CPLD control constants
 * Machine VR1000 - IRQ Number definitions
 * Machine VR1000 - Memory map definitions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef __MACH_S3C24XX_VR1000_H
#define __MACH_S3C24XX_VR1000_H __FILE__

#define VR1000_CPLD_CTRL2_RAMWEN	(0x04)	/*                   */

/*                                    */

#define VR1000_IRQ_USBOC		IRQ_EINT19
#define VR1000_IRQ_IDE0			IRQ_EINT16
#define VR1000_IRQ_IDE1			IRQ_EINT17
#define VR1000_IRQ_SERIAL		IRQ_EINT12
#define VR1000_IRQ_DM9000A		IRQ_EINT10
#define VR1000_IRQ_DM9000N		IRQ_EINT9
#define VR1000_IRQ_SMALERT		IRQ_EINT8

/*     */

#define VR1000_IOADDR(x)		(S3C2410_ADDR((x) + 0x01300000))

/*                                                            */

#define VR1000_VA_CTRL1			VR1000_IOADDR(0x00000000) /*            */
#define VR1000_PA_CTRL1			(S3C2410_CS5 | 0x7800000)

#define VR1000_VA_CTRL2			VR1000_IOADDR(0x00100000) /*            */
#define VR1000_PA_CTRL2			(S3C2410_CS1 | 0x6000000)

#define VR1000_VA_CTRL3			VR1000_IOADDR(0x00200000) /*            */
#define VR1000_PA_CTRL3			(S3C2410_CS1 | 0x6800000)

#define VR1000_VA_CTRL4			VR1000_IOADDR(0x00300000) /*            */
#define VR1000_PA_CTRL4			(S3C2410_CS1 | 0x7000000)

/*                                                 */

#define VR1000_PA_PC104_IRQREQ		(S3C2410_CS5 | 0x6000000) /*            */
#define VR1000_VA_PC104_IRQREQ		VR1000_IOADDR(0x00400000)

#define VR1000_PA_PC104_IRQRAW		(S3C2410_CS5 | 0x6800000) /*            */
#define VR1000_VA_PC104_IRQRAW		VR1000_IOADDR(0x00500000)

#define VR1000_PA_PC104_IRQMASK		(S3C2410_CS5 | 0x7000000) /*            */
#define VR1000_VA_PC104_IRQMASK		VR1000_IOADDR(0x00600000)

/*
                                                              
                                                               
                             
  
                                          
  
                                        
                                        
                                       
                                        
  
                                             
  
                                                    
                                                        
                                                      
                                                          
                                                                    
                               
  
                                        
                      
                       
                      
                       
 */

#define VR1000_VA_MULTISPACE	(0xE0000000)

#define VR1000_VA_ISAIO		(VR1000_VA_MULTISPACE + 0x00000000)
#define VR1000_VA_ISAMEM	(VR1000_VA_MULTISPACE + 0x01000000)
#define VR1000_VA_IDEPRI	(VR1000_VA_MULTISPACE + 0x02000000)
#define VR1000_VA_IDEPRIAUX	(VR1000_VA_MULTISPACE + 0x02100000)
#define VR1000_VA_IDESEC	(VR1000_VA_MULTISPACE + 0x02200000)
#define VR1000_VA_IDESECAUX	(VR1000_VA_MULTISPACE + 0x02300000)
#define VR1000_VA_ASIXNET	(VR1000_VA_MULTISPACE + 0x02400000)
#define VR1000_VA_DM9000	(VR1000_VA_MULTISPACE + 0x02500000)
#define VR1000_VA_SUPERIO	(VR1000_VA_MULTISPACE + 0x02600000)

/*                                               */

#define VR1000_PA_IDEPRI	(0x02000000)
#define VR1000_PA_IDEPRIAUX	(0x02800000)
#define VR1000_PA_IDESEC	(0x03000000)
#define VR1000_PA_IDESECAUX	(0x03800000)
#define VR1000_PA_DM9000	(0x05000000)

#define VR1000_PA_SERIAL	(0x11800000)
#define VR1000_VA_SERIAL	(VR1000_IOADDR(0x00700000))

/*                                             */
#define VR1000_PA_SRAM		(S3C2410_CS1 | 0x05000000)

/*                                         */

#define VR1000_DM9000_CS	VR1000_VAM_CS4

#endif /*                         */
