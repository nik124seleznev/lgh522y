/*
 * r8a73a4 processor support
 *
 * Copyright (C) 2013  Renesas Solutions Corp.
 * Copyright (C) 2013  Magnus Damm
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <linux/irq.h>
#include <linux/irqchip.h>
#include <linux/kernel.h>
#include <linux/of_platform.h>
#include <linux/platform_data/irq-renesas-irqc.h>
#include <linux/serial_sci.h>
#include <mach/common.h>
#include <mach/irqs.h>
#include <mach/r8a73a4.h>
#include <asm/mach/arch.h>

static const struct resource pfc_resources[] = {
	DEFINE_RES_MEM(0xe6050000, 0x9000),
};

void __init r8a73a4_pinmux_init(void)
{
	platform_device_register_simple("pfc-r8a73a4", -1, pfc_resources,
					ARRAY_SIZE(pfc_resources));
}

#define SCIF_COMMON(scif_type, baseaddr, irq)			\
	.type		= scif_type,				\
	.mapbase	= baseaddr,				\
	.flags		= UPF_BOOT_AUTOCONF | UPF_IOREMAP,	\
	.scbrr_algo_id	= SCBRR_ALGO_4,				\
	.irqs		= SCIx_IRQ_MUXED(irq)

#define SCIFA_DATA(index, baseaddr, irq)		\
[index] = {						\
	SCIF_COMMON(PORT_SCIFA, baseaddr, irq),		\
	.scscr = SCSCR_RE | SCSCR_TE | SCSCR_CKE0,	\
}

#define SCIFB_DATA(index, baseaddr, irq)	\
[index] = {					\
	SCIF_COMMON(PORT_SCIFB, baseaddr, irq),	\
	.scscr = SCSCR_RE | SCSCR_TE,		\
}

enum { SCIFA0, SCIFA1, SCIFB0, SCIFB1, SCIFB2, SCIFB3 };

static const struct plat_sci_port scif[] = {
	SCIFA_DATA(SCIFA0, 0xe6c40000, gic_spi(144)), /*        */
	SCIFA_DATA(SCIFA1, 0xe6c50000, gic_spi(145)), /*        */
	SCIFB_DATA(SCIFB0, 0xe6c20000, gic_spi(148)), /*        */
	SCIFB_DATA(SCIFB1, 0xe6c30000, gic_spi(149)), /*        */
	SCIFB_DATA(SCIFB2, 0xe6ce0000, gic_spi(150)), /*        */
	SCIFB_DATA(SCIFB3, 0xe6cf0000, gic_spi(151)), /*        */
};

static inline void r8a73a4_register_scif(int idx)
{
	platform_device_register_data(&platform_bus, "sh-sci", idx, &scif[idx],
				      sizeof(struct plat_sci_port));
}

static const struct renesas_irqc_config irqc0_data = {
	.irq_base = irq_pin(0), /*               */
};

static const struct resource irqc0_resources[] = {
	DEFINE_RES_MEM(0xe61c0000, 0x200), /*                             */
	DEFINE_RES_IRQ(gic_spi(0)), /*      */
	DEFINE_RES_IRQ(gic_spi(1)), /*      */
	DEFINE_RES_IRQ(gic_spi(2)), /*      */
	DEFINE_RES_IRQ(gic_spi(3)), /*      */
	DEFINE_RES_IRQ(gic_spi(4)), /*      */
	DEFINE_RES_IRQ(gic_spi(5)), /*      */
	DEFINE_RES_IRQ(gic_spi(6)), /*      */
	DEFINE_RES_IRQ(gic_spi(7)), /*      */
	DEFINE_RES_IRQ(gic_spi(8)), /*      */
	DEFINE_RES_IRQ(gic_spi(9)), /*      */
	DEFINE_RES_IRQ(gic_spi(10)), /*       */
	DEFINE_RES_IRQ(gic_spi(11)), /*       */
	DEFINE_RES_IRQ(gic_spi(12)), /*       */
	DEFINE_RES_IRQ(gic_spi(13)), /*       */
	DEFINE_RES_IRQ(gic_spi(14)), /*       */
	DEFINE_RES_IRQ(gic_spi(15)), /*       */
	DEFINE_RES_IRQ(gic_spi(16)), /*       */
	DEFINE_RES_IRQ(gic_spi(17)), /*       */
	DEFINE_RES_IRQ(gic_spi(18)), /*       */
	DEFINE_RES_IRQ(gic_spi(19)), /*       */
	DEFINE_RES_IRQ(gic_spi(20)), /*       */
	DEFINE_RES_IRQ(gic_spi(21)), /*       */
	DEFINE_RES_IRQ(gic_spi(22)), /*       */
	DEFINE_RES_IRQ(gic_spi(23)), /*       */
	DEFINE_RES_IRQ(gic_spi(24)), /*       */
	DEFINE_RES_IRQ(gic_spi(25)), /*       */
	DEFINE_RES_IRQ(gic_spi(26)), /*       */
	DEFINE_RES_IRQ(gic_spi(27)), /*       */
	DEFINE_RES_IRQ(gic_spi(28)), /*       */
	DEFINE_RES_IRQ(gic_spi(29)), /*       */
	DEFINE_RES_IRQ(gic_spi(30)), /*       */
	DEFINE_RES_IRQ(gic_spi(31)), /*       */
};

static const struct renesas_irqc_config irqc1_data = {
	.irq_base = irq_pin(32), /*                */
};

static const struct resource irqc1_resources[] = {
	DEFINE_RES_MEM(0xe61c0200, 0x200), /*                             */
	DEFINE_RES_IRQ(gic_spi(32)), /*       */
	DEFINE_RES_IRQ(gic_spi(33)), /*       */
	DEFINE_RES_IRQ(gic_spi(34)), /*       */
	DEFINE_RES_IRQ(gic_spi(35)), /*       */
	DEFINE_RES_IRQ(gic_spi(36)), /*       */
	DEFINE_RES_IRQ(gic_spi(37)), /*       */
	DEFINE_RES_IRQ(gic_spi(38)), /*       */
	DEFINE_RES_IRQ(gic_spi(39)), /*       */
	DEFINE_RES_IRQ(gic_spi(40)), /*       */
	DEFINE_RES_IRQ(gic_spi(41)), /*       */
	DEFINE_RES_IRQ(gic_spi(42)), /*       */
	DEFINE_RES_IRQ(gic_spi(43)), /*       */
	DEFINE_RES_IRQ(gic_spi(44)), /*       */
	DEFINE_RES_IRQ(gic_spi(45)), /*       */
	DEFINE_RES_IRQ(gic_spi(46)), /*       */
	DEFINE_RES_IRQ(gic_spi(47)), /*       */
	DEFINE_RES_IRQ(gic_spi(48)), /*       */
	DEFINE_RES_IRQ(gic_spi(49)), /*       */
	DEFINE_RES_IRQ(gic_spi(50)), /*       */
	DEFINE_RES_IRQ(gic_spi(51)), /*       */
	DEFINE_RES_IRQ(gic_spi(52)), /*       */
	DEFINE_RES_IRQ(gic_spi(53)), /*       */
	DEFINE_RES_IRQ(gic_spi(54)), /*       */
	DEFINE_RES_IRQ(gic_spi(55)), /*       */
	DEFINE_RES_IRQ(gic_spi(56)), /*       */
	DEFINE_RES_IRQ(gic_spi(57)), /*       */
};

#define r8a73a4_register_irqc(idx)					\
	platform_device_register_resndata(&platform_bus, "renesas_irqc", \
					  idx, irqc##idx##_resources,	\
					  ARRAY_SIZE(irqc##idx##_resources), \
					  &irqc##idx##_data,		\
					  sizeof(struct renesas_irqc_config))

/*                      */
static const struct resource thermal0_resources[] = {
	DEFINE_RES_MEM(0xe61f0000, 0x14),
	DEFINE_RES_MEM(0xe61f0100, 0x38),
	DEFINE_RES_MEM(0xe61f0200, 0x38),
	DEFINE_RES_MEM(0xe61f0300, 0x38),
	DEFINE_RES_IRQ(gic_spi(69)),
};

#define r8a73a4_register_thermal()					\
	platform_device_register_simple("rcar_thermal", -1,		\
					thermal0_resources,		\
					ARRAY_SIZE(thermal0_resources))

void __init r8a73a4_add_standard_devices(void)
{
	r8a73a4_register_scif(SCIFA0);
	r8a73a4_register_scif(SCIFA1);
	r8a73a4_register_scif(SCIFB0);
	r8a73a4_register_scif(SCIFB1);
	r8a73a4_register_scif(SCIFB2);
	r8a73a4_register_scif(SCIFB3);
	r8a73a4_register_irqc(0);
	r8a73a4_register_irqc(1);
	r8a73a4_register_thermal();
}

#ifdef CONFIG_USE_OF
void __init r8a73a4_add_standard_devices_dt(void)
{
	of_platform_populate(NULL, of_default_bus_match_table, NULL, NULL);
}

static const char *r8a73a4_boards_compat_dt[] __initdata = {
	"renesas,r8a73a4",
	NULL,
};

DT_MACHINE_START(R8A73A4_DT, "Generic R8A73A4 (Flattened Device Tree)")
	.init_irq	= irqchip_init,
	.init_machine	= r8a73a4_add_standard_devices_dt,
	.init_time	= shmobile_timer_init,
	.dt_compat	= r8a73a4_boards_compat_dt,
MACHINE_END
#endif /*               */
