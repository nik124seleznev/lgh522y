/*
 * arch/xtensa/include/asm/initialize_mmu.h
 *
 * Initializes MMU:
 *
 *      For the new V3 MMU we remap the TLB from virtual == physical
 *      to the standard Linux mapping used in earlier MMU's.
 *
 *      The the MMU we also support a new configuration register that
 *      specifies how the S32C1I instruction operates with the cache
 *      controller.
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 *
 * Copyright (C) 2008 - 2012 Tensilica, Inc.
 *
 *   Marc Gauthier <marc@tensilica.com>
 *   Pete Delaney <piet@tensilica.com>
 */

#ifndef _XTENSA_INITIALIZE_MMU_H
#define _XTENSA_INITIALIZE_MMU_H

#include <asm/pgtable.h>
#include <asm/vectors.h>

#ifdef __ASSEMBLY__

#define XTENSA_HWVERSION_RC_2009_0 230000

	.macro	initialize_mmu

#if XCHAL_HAVE_S32C1I && (XCHAL_HW_MIN_VERSION >= XTENSA_HWVERSION_RC_2009_0)
/*
                                                                      
                                                   
 */
#if XCHAL_DCACHE_IS_COHERENT
	movi	a3, 0x25	/*                                      
                    
     */
#else
	movi	a3, 0x29	/*                                    
                                             
     */
#endif
	wsr	a3, atomctl
#endif  /*                     
                                                        
  */

#if defined(CONFIG_MMU) && XCHAL_HAVE_PTP_MMU && XCHAL_HAVE_SPANNING_WAY
/*
              
 */

#if !XCHAL_HAVE_VECBASE
# error "MMU v3 requires reloc vectors"
#endif

	movi	a1, 0
	_call0	1f
	_j	2f

	.align	4
1:	movi	a2, 0x10000000
	movi	a3, 0x18000000
	add	a2, a2, a0
9:	bgeu	a2, a3, 9b	/*                                 */

	/*                                                       */

	movi	a2, 0x40000006
	idtlb	a2
	iitlb	a2
	isync

	/*                                                                 
                                
  */
#define CA_BYPASS	(_PAGE_CA_BYPASS | _PAGE_HW_WRITE | _PAGE_HW_EXEC)
#define CA_WRITEBACK	(_PAGE_CA_WB     | _PAGE_HW_WRITE | _PAGE_HW_EXEC)

	srli	a3, a0, 27
	slli	a3, a3, 27
	addi	a3, a3, CA_BYPASS
	addi	a7, a2, -1
	wdtlb	a3, a7
	witlb	a3, a7
	isync

	slli	a4, a0, 5
	srli	a4, a4, 5
	addi	a5, a2, -6
	add	a4, a4, a5
	jx	a4

	/*                                                  
                                                                
  */
2:	movi	a4, 0x20000000
	add	a5, a2, a4
3:	idtlb	a5
	iitlb	a5
	add	a5, a5, a4
	bne	a5, a2, 3b

	/*                                             */
	movi	a6, 0x01000000
	wsr	a6, ITLBCFG
	wsr	a6, DTLBCFG
	isync

	movi	a5, 0xd0000005
	movi	a4, CA_WRITEBACK
	wdtlb	a4, a5
	witlb	a4, a5

	movi	a5, 0xd8000005
	movi	a4, CA_BYPASS
	wdtlb	a4, a5
	witlb	a4, a5

	movi	a5, 0xe0000006
	movi	a4, 0xf0000000 + CA_WRITEBACK
	wdtlb	a4, a5
	witlb	a4, a5

	movi	a5, 0xf0000006
	movi	a4, 0xf0000000 + CA_BYPASS
	wdtlb	a4, a5
	witlb	a4, a5

	isync

	/*                                      */
	movi	a4, 1f
	jx	a4

1:
	movi    a2, VECBASE_RESET_VADDR
	wsr	a2, vecbase

	/*                                   */
	idtlb	a7
	iitlb	a7
	isync

	movi	a0, 0
	wsr	a0, ptevaddr
	rsync

#endif /*                                             
                           */

	.endm

#endif /*            */

#endif /*                          */