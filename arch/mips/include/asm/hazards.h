/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003, 04, 07 Ralf Baechle <ralf@linux-mips.org>
 * Copyright (C) MIPS Technologies, Inc.
 *   written by Ralf Baechle <ralf@linux-mips.org>
 */
#ifndef _ASM_HAZARDS_H
#define _ASM_HAZARDS_H

#include <linux/stringify.h>

#define ___ssnop							\
	sll	$0, $0, 1

#define ___ehb								\
	sll	$0, $0, 3

/*
              
 */
#if defined(CONFIG_CPU_MIPSR2) && !defined(CONFIG_CPU_CAVIUM_OCTEON)

/*
                                          
 */

#define __mtc0_tlbw_hazard						\
	___ehb

#define __tlbw_use_hazard						\
	___ehb

#define __tlb_probe_hazard						\
	___ehb

#define __irq_enable_hazard						\
	___ehb

#define __irq_disable_hazard						\
	___ehb

#define __back_to_back_c0_hazard					\
	___ehb

/*
                                                                        
                                                                       
                                                                          
                                                                      
                                                                          
                                         
 */
#define instruction_hazard()						\
do {									\
	unsigned long tmp;						\
									\
	__asm__ __volatile__(						\
	"	.set	mips64r2				\n"	\
	"	dla	%0, 1f					\n"	\
	"	jr.hb	%0					\n"	\
	"	.set	mips0					\n"	\
	"1:							\n"	\
	: "=r" (tmp));							\
} while (0)

#elif (defined(CONFIG_CPU_MIPSR1) && !defined(CONFIG_MIPS_ALCHEMY)) || \
	defined(CONFIG_CPU_BMIPS)

/*
                                                                             
                             
 */

#define __mtc0_tlbw_hazard						\
	___ssnop;							\
	___ssnop;							\
	___ehb

#define __tlbw_use_hazard						\
	___ssnop;							\
	___ssnop;							\
	___ssnop;							\
	___ehb

#define __tlb_probe_hazard						\
	___ssnop;							\
	___ssnop;							\
	___ssnop;							\
	___ehb

#define __irq_enable_hazard						\
	___ssnop;							\
	___ssnop;							\
	___ssnop;							\
	___ehb

#define __irq_disable_hazard						\
	___ssnop;							\
	___ssnop;							\
	___ssnop;							\
	___ehb

#define __back_to_back_c0_hazard					\
	___ssnop;							\
	___ssnop;							\
	___ssnop;							\
	___ehb

/*
                                                                        
                                                                       
                                                                          
                                                                      
                                                                          
                                         
 */
#define __instruction_hazard()						\
do {									\
	unsigned long tmp;						\
									\
	__asm__ __volatile__(						\
	"	.set	mips64r2				\n"	\
	"	dla	%0, 1f					\n"	\
	"	jr.hb	%0					\n"	\
	"	.set	mips0					\n"	\
	"1:							\n"	\
	: "=r" (tmp));							\
} while (0)

#define instruction_hazard()						\
do {									\
	if (cpu_has_mips_r2)						\
		__instruction_hazard();					\
} while (0)

#elif defined(CONFIG_MIPS_ALCHEMY) || defined(CONFIG_CPU_CAVIUM_OCTEON) || \
	defined(CONFIG_CPU_LOONGSON2) || defined(CONFIG_CPU_R10000) || \
	defined(CONFIG_CPU_R5500) || defined(CONFIG_CPU_XLR)

/*
                                                                               
 */

#define __mtc0_tlbw_hazard

#define __tlbw_use_hazard

#define __tlb_probe_hazard

#define __irq_enable_hazard

#define __irq_disable_hazard

#define __back_to_back_c0_hazard

#define instruction_hazard() do { } while (0)

#elif defined(CONFIG_CPU_SB1)

/*
                                         
 */
#define __mtc0_tlbw_hazard

#define __tlbw_use_hazard

#define __tlb_probe_hazard

#define __irq_enable_hazard

#define __irq_disable_hazard						\
	___ssnop;							\
	___ssnop;							\
	___ssnop

#define __back_to_back_c0_hazard

#define instruction_hazard() do { } while (0)

#else

/*
                                                                             
                                               
  
                                                                         
                                                                            
                                                                      
              
 */
#define __mtc0_tlbw_hazard						\
	nop;								\
	nop

#define __tlbw_use_hazard						\
	nop;								\
	nop;								\
	nop

#define __tlb_probe_hazard						\
	nop;								\
	nop;								\
	nop

#define __irq_enable_hazard						\
	___ssnop;							\
	___ssnop;							\
	___ssnop

#define __irq_disable_hazard						\
	nop;								\
	nop;								\
	nop

#define __back_to_back_c0_hazard					\
	___ssnop;							\
	___ssnop;							\
	___ssnop

#define instruction_hazard() do { } while (0)

#endif


/*             */

#if defined(CONFIG_CPU_SB1)

#define __enable_fpu_hazard						\
	.set	push;							\
	.set	mips64;							\
	.set	noreorder;						\
	___ssnop;							\
	bnezl	$0, .+4;						\
	___ssnop;							\
	.set	pop

#define __disable_fpu_hazard

#elif defined(CONFIG_CPU_MIPSR2)

#define __enable_fpu_hazard						\
	___ehb

#define __disable_fpu_hazard						\
	___ehb

#else

#define __enable_fpu_hazard						\
	nop;								\
	nop;								\
	nop;								\
	nop

#define __disable_fpu_hazard						\
	___ehb

#endif

#ifdef __ASSEMBLY__

#define _ssnop ___ssnop
#define	_ehb ___ehb
#define mtc0_tlbw_hazard __mtc0_tlbw_hazard
#define tlbw_use_hazard __tlbw_use_hazard
#define tlb_probe_hazard __tlb_probe_hazard
#define irq_enable_hazard __irq_enable_hazard
#define irq_disable_hazard __irq_disable_hazard
#define back_to_back_c0_hazard __back_to_back_c0_hazard
#define enable_fpu_hazard __enable_fpu_hazard
#define disable_fpu_hazard __disable_fpu_hazard

#else

#define _ssnop()							\
do {									\
	__asm__ __volatile__(						\
	__stringify(___ssnop)						\
	);								\
} while (0)

#define	_ehb()								\
do {									\
	__asm__ __volatile__(						\
	__stringify(___ehb)						\
	);								\
} while (0)


#define mtc0_tlbw_hazard()						\
do {									\
	__asm__ __volatile__(						\
	__stringify(__mtc0_tlbw_hazard)					\
	);								\
} while (0)


#define tlbw_use_hazard()						\
do {									\
	__asm__ __volatile__(						\
	__stringify(__tlbw_use_hazard)					\
	);								\
} while (0)


#define tlb_probe_hazard()						\
do {									\
	__asm__ __volatile__(						\
	__stringify(__tlb_probe_hazard)					\
	);								\
} while (0)


#define irq_enable_hazard()						\
do {									\
	__asm__ __volatile__(						\
	__stringify(__irq_enable_hazard)				\
	);								\
} while (0)


#define irq_disable_hazard()						\
do {									\
	__asm__ __volatile__(						\
	__stringify(__irq_disable_hazard)				\
	);								\
} while (0)


#define back_to_back_c0_hazard() 					\
do {									\
	__asm__ __volatile__(						\
	__stringify(__back_to_back_c0_hazard)				\
	);								\
} while (0)


#define enable_fpu_hazard()						\
do {									\
	__asm__ __volatile__(						\
	__stringify(__enable_fpu_hazard)				\
	);								\
} while (0)


#define disable_fpu_hazard()						\
do {									\
	__asm__ __volatile__(						\
	__stringify(__disable_fpu_hazard)				\
	);								\
} while (0)

/*
                                                                            
 */
extern void mips_ihb(void);

#endif /*               */

#endif /*                */
