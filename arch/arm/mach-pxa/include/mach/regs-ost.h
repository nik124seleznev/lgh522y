#ifndef __ASM_MACH_REGS_OST_H
#define __ASM_MACH_REGS_OST_H

#include <mach/hardware.h>

/*
                             
 */

#define OSMR0		io_p2v(0x40A00000)  /* */
#define OSMR1		io_p2v(0x40A00004)  /* */
#define OSMR2		io_p2v(0x40A00008)  /* */
#define OSMR3		io_p2v(0x40A0000C)  /* */
#define OSMR4		io_p2v(0x40A00080)  /* */
#define OSCR		io_p2v(0x40A00010)  /*                           */
#define OSCR4		io_p2v(0x40A00040)  /*                           */
#define OMCR4		io_p2v(0x40A000C0)  /* */
#define OSSR		io_p2v(0x40A00014)  /*                          */
#define OWER		io_p2v(0x40A00018)  /*                                   */
#define OIER		io_p2v(0x40A0001C)  /*                                    */

#define OSSR_M3		(1 << 3)	/*                        */
#define OSSR_M2		(1 << 2)	/*                        */
#define OSSR_M1		(1 << 1)	/*                        */
#define OSSR_M0		(1 << 0)	/*                        */

#define OWER_WME	(1 << 0)	/*                       */

#define OIER_E3		(1 << 3)	/*                            */
#define OIER_E2		(1 << 2)	/*                            */
#define OIER_E1		(1 << 1)	/*                            */
#define OIER_E0		(1 << 0)	/*                            */

#endif /*                       */
