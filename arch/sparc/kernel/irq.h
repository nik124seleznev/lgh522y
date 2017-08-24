#include <linux/platform_device.h>

#include <asm/cpu_type.h>

struct irq_bucket {
        struct irq_bucket *next;
        unsigned int real_irq;
        unsigned int irq;
        unsigned int pil;
};

#define SUN4M_HARD_INT(x)       (0x000000001 << (x))
#define SUN4M_SOFT_INT(x)       (0x000010000 << (x))

#define SUN4D_MAX_BOARD 10
#define SUN4D_MAX_IRQ ((SUN4D_MAX_BOARD + 2) << 5)

/*                                                 
                                                  
                            
 */
extern struct irq_bucket *irq_map[SUN4D_MAX_IRQ];


/*                                 */

/*                                                      */
struct sun4m_irq_percpu {
	u32	pending;
	u32	clear;
	u32	set;
};

/*                                                */
struct sun4m_irq_global {
	u32	pending;
	u32	mask;
	u32	mask_clear;
	u32	mask_set;
	u32	interrupt_target;
};

extern struct sun4m_irq_percpu __iomem *sun4m_irq_percpu[SUN4M_NCPUS];
extern struct sun4m_irq_global __iomem *sun4m_irq_global;

/*                                                                      */
#define FEAT_L10_CLOCKSOURCE (1 << 0) /*                                    */
#define FEAT_L10_CLOCKEVENT  (1 << 1) /*                                   */
#define FEAT_L14_ONESHOT     (1 << 2) /*                                  */

/*
                                  
                                                 
                                     
 */
struct sparc_config {
	void (*init_timers)(void);
	unsigned int (*build_device_irq)(struct platform_device *op,
	                                 unsigned int real_irq);

	/*                                                */
	int features;

	/*                                       */
	int clock_rate;

	/*                                   */
	unsigned int cs_period;

	/*                                          */
	unsigned int (*get_cycles_offset)(void);

	void (*clear_clock_irq)(void);
	void (*load_profile_irq)(int cpu, unsigned int limit);
};
extern struct sparc_config sparc_config;

unsigned int irq_alloc(unsigned int real_irq, unsigned int pil);
void irq_link(unsigned int irq);
void irq_unlink(unsigned int irq);
void handler_irq(unsigned int pil, struct pt_regs *regs);

unsigned long leon_get_irqmask(unsigned int irq);

#ifdef CONFIG_SMP

/*                                                                   */
#define SUN4D_IPI_IRQ 13

extern void sun4d_ipi_interrupt(void);

#endif
