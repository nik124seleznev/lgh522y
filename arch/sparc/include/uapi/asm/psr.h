/*
 * psr.h: This file holds the macros for masking off various parts of
 *        the processor status register on the Sparc. This is valid
 *        for Version 8. On the V9 this is renamed to the PSTATE
 *        register and its members are accessed as fields like
 *        PSTATE.PRIV for the current CPU privilege level.
 *
 * Copyright (C) 1994 David S. Miller (davem@caip.rutgers.edu)
 */

#ifndef _UAPI__LINUX_SPARC_PSR_H
#define _UAPI__LINUX_SPARC_PSR_H

/*                                                    
  
                                                                            
                                                                            
                                                                            
                                                                            
 */
#define PSR_CWP     0x0000001f         /*                            */
#define PSR_ET      0x00000020         /*                            */
#define PSR_PS      0x00000040         /*                            */
#define PSR_S       0x00000080         /*                            */
#define PSR_PIL     0x00000f00         /*                            */
#define PSR_EF      0x00001000         /*                            */
#define PSR_EC      0x00002000         /*                            */
#define PSR_SYSCALL 0x00004000         /*                            */
#define PSR_LE      0x00008000         /*                            */
#define PSR_ICC     0x00f00000         /*                            */
#define PSR_C       0x00100000         /*                            */
#define PSR_V       0x00200000         /*                            */
#define PSR_Z       0x00400000         /*                            */
#define PSR_N       0x00800000         /*                            */
#define PSR_VERS    0x0f000000         /*                            */
#define PSR_IMPL    0xf0000000         /*                            */

#define PSR_VERS_SHIFT		24
#define PSR_IMPL_SHIFT		28
#define PSR_VERS_SHIFTED_MASK	0xf
#define PSR_IMPL_SHIFTED_MASK	0xf

#define PSR_IMPL_TI		0x4
#define PSR_IMPL_LEON		0xf


#endif /*                          */
