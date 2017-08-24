#ifndef _SPARC_CONTREGS_H
#define _SPARC_CONTREGS_H

/* contregs.h:  Addresses of registers in the ASI_CONTROL alternate address
 *              space. These are for the mmu's context register, etc.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 */

/*                                                                           */
#define AC_M_PCR      0x0000        /*                                       */
#define AC_M_CTPR     0x0100        /*                                       */
#define AC_M_CXR      0x0200        /*                                       */
#define AC_M_SFSR     0x0300        /*                                       */
#define AC_M_SFAR     0x0400        /*                                       */
#define AC_M_AFSR     0x0500        /*                                       */
#define AC_M_AFAR     0x0600        /*                                       */
#define AC_M_RESET    0x0700        /*                                       */
#define AC_M_RPR      0x1000        /*                                       */
#define AC_M_TSUTRCR  0x1000        /*                                       */
#define AC_M_IAPTP    0x1100        /*                                       */
#define AC_M_DAPTP    0x1200        /*                                       */
#define AC_M_ITR      0x1300        /*                                       */
#define AC_M_TRCR     0x1400        /*                                       */
#define AC_M_SFSRX    0x1300        /*                                       */
#define AC_M_SFARX    0x1400        /*                                       */
#define AC_M_RPR1     0x1500        /*                                       */
#define AC_M_IAPTP1   0x1600        /*                                       */
#define AC_M_DAPTP1   0x1700        /*                                       */

#endif /*                   */
