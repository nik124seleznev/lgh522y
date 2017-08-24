/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 95, 96, 97, 98, 99, 2003 by Ralf Baechle
 * Copyright (C) 1996 by Paul M. Antoine
 * Copyright (C) 1999 Silicon Graphics
 * Copyright (C) 2000 MIPS Technologies, Inc.
 */
#include <asm/irqflags.h>
#include <asm/hazards.h>
#include <linux/compiler.h>
#include <linux/preempt.h>
#include <linux/export.h>
#include <linux/stringify.h>

#if !defined(CONFIG_CPU_MIPSR2) || defined(CONFIG_MIPS_MT_SMTC)

/*
                                                                   
                                                                     
         
                                                                       
                  
 */
/*
                                                 
  
                                                                  
                                                                     
                                             
  
                                                                  
  
                                                                     
 */
notrace void arch_local_irq_disable(void)
{
	preempt_disable();

	__asm__ __volatile__(
	"	.set	push						\n"
	"	.set	noat						\n"
#ifdef CONFIG_MIPS_MT_SMTC
	"	mfc0	$1, $2, 1					\n"
	"	ori	$1, 0x400					\n"
	"	.set	noreorder					\n"
	"	mtc0	$1, $2, 1					\n"
#elif defined(CONFIG_CPU_MIPSR2)
	/*                                    */
#else
	"	mfc0	$1,$12						\n"
	"	ori	$1,0x1f						\n"
	"	xori	$1,0x1f						\n"
	"	.set	noreorder					\n"
	"	mtc0	$1,$12						\n"
#endif
	"	" __stringify(__irq_disable_hazard) "			\n"
	"	.set	pop						\n"
	: /*            */
	: /*           */
	: "memory");

	preempt_enable();
}
EXPORT_SYMBOL(arch_local_irq_disable);


notrace unsigned long arch_local_irq_save(void)
{
	unsigned long flags;

	preempt_disable();

	__asm__ __volatile__(
	"	.set	push						\n"
	"	.set	reorder						\n"
	"	.set	noat						\n"
#ifdef CONFIG_MIPS_MT_SMTC
	"	mfc0	%[flags], $2, 1				\n"
	"	ori	$1, %[flags], 0x400				\n"
	"	.set	noreorder					\n"
	"	mtc0	$1, $2, 1					\n"
	"	andi	%[flags], %[flags], 0x400			\n"
#elif defined(CONFIG_CPU_MIPSR2)
	/*                                    */
#else
	"	mfc0	%[flags], $12					\n"
	"	ori	$1, %[flags], 0x1f				\n"
	"	xori	$1, 0x1f					\n"
	"	.set	noreorder					\n"
	"	mtc0	$1, $12						\n"
#endif
	"	" __stringify(__irq_disable_hazard) "			\n"
	"	.set	pop						\n"
	: [flags] "=r" (flags)
	: /*           */
	: "memory");

	preempt_enable();

	return flags;
}
EXPORT_SYMBOL(arch_local_irq_save);

notrace void arch_local_irq_restore(unsigned long flags)
{
	unsigned long __tmp1;

#ifdef CONFIG_MIPS_MT_SMTC
	/*
                                                       
                                                         
                       
  */
	if (unlikely(!(flags & 0x0400)))
		smtc_ipi_replay();
#endif
	preempt_disable();

	__asm__ __volatile__(
	"	.set	push						\n"
	"	.set	noreorder					\n"
	"	.set	noat						\n"
#ifdef CONFIG_MIPS_MT_SMTC
	"	mfc0	$1, $2, 1					\n"
	"	andi	%[flags], 0x400					\n"
	"	ori	$1, 0x400					\n"
	"	xori	$1, 0x400					\n"
	"	or	%[flags], $1					\n"
	"	mtc0	%[flags], $2, 1					\n"
#elif defined(CONFIG_CPU_MIPSR2) && defined(CONFIG_IRQ_CPU)
	/*                                    */
#elif defined(CONFIG_CPU_MIPSR2)
	/*                                    */
#else
	"	mfc0	$1, $12						\n"
	"	andi	%[flags], 1					\n"
	"	ori	$1, 0x1f					\n"
	"	xori	$1, 0x1f					\n"
	"	or	%[flags], $1					\n"
	"	mtc0	%[flags], $12					\n"
#endif
	"	" __stringify(__irq_disable_hazard) "			\n"
	"	.set	pop						\n"
	: [flags] "=r" (__tmp1)
	: "0" (flags)
	: "memory");

	preempt_enable();
}
EXPORT_SYMBOL(arch_local_irq_restore);


notrace void __arch_local_irq_restore(unsigned long flags)
{
	unsigned long __tmp1;

	preempt_disable();

	__asm__ __volatile__(
	"	.set	push						\n"
	"	.set	noreorder					\n"
	"	.set	noat						\n"
#ifdef CONFIG_MIPS_MT_SMTC
	"	mfc0	$1, $2, 1					\n"
	"	andi	%[flags], 0x400					\n"
	"	ori	$1, 0x400					\n"
	"	xori	$1, 0x400					\n"
	"	or	%[flags], $1					\n"
	"	mtc0	%[flags], $2, 1					\n"
#elif defined(CONFIG_CPU_MIPSR2) && defined(CONFIG_IRQ_CPU)
	/*                                    */
#elif defined(CONFIG_CPU_MIPSR2)
	/*                                    */
#else
	"	mfc0	$1, $12						\n"
	"	andi	%[flags], 1					\n"
	"	ori	$1, 0x1f					\n"
	"	xori	$1, 0x1f					\n"
	"	or	%[flags], $1					\n"
	"	mtc0	%[flags], $12					\n"
#endif
	"	" __stringify(__irq_disable_hazard) "			\n"
	"	.set	pop						\n"
	: [flags] "=r" (__tmp1)
	: "0" (flags)
	: "memory");

	preempt_enable();
}
EXPORT_SYMBOL(__arch_local_irq_restore);

#endif /*                                                             */
