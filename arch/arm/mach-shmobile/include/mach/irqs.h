#ifndef __ASM_MACH_IRQS_H
#define __ASM_MACH_IRQS_H

#include <linux/sh_intc.h>

/*     */
#define gic_spi(nr)		((nr) + 32)
#define gic_iid(nr)		(nr) /*                       */

/*       */
#define INTCS_VECT_BASE		0x3400
#define INTCS_VECT(n, vect)	INTC_VECT((n), INTCS_VECT_BASE + (vect))
#define intcs_evt2irq(evt)	evt2irq(INTCS_VECT_BASE + (evt))

/*                   */
#define IRQPIN_BASE		2000
#define irq_pin(nr)		((nr) + IRQPIN_BASE)

#endif /*                   */
