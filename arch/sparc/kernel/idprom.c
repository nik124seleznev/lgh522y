/*
 * idprom.c: Routines to load the idprom into kernel addresses and
 *           interpret the data contained within.
 *
 * Copyright (C) 1995 David S. Miller (davem@caip.rutgers.edu)
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/export.h>

#include <asm/oplib.h>
#include <asm/idprom.h>

struct idprom *idprom;
EXPORT_SYMBOL(idprom);

static struct idprom idprom_buffer;

#ifdef CONFIG_SPARC32
#include <asm/machines.h>  /*                                      */

/*                                                                       
                                                                       
                                                                 
 */
static struct Sun_Machine_Models Sun_Machines[] = {
/*             */
{ .name = "Leon3 System-on-a-Chip",  .id_machtype = (M_LEON | M_LEON3_SOC) },
/*                        */
{ .name = "Sun4m SparcSystem600",    .id_machtype = (SM_SUN4M | SM_4M_SS60) },
{ .name = "Sun4m SparcStation10/20", .id_machtype = (SM_SUN4M | SM_4M_SS50) },
{ .name = "Sun4m SparcStation5",     .id_machtype = (SM_SUN4M | SM_4M_SS40) },
/*                                                                        */
{ .name = "Sun4M OBP based system",  .id_machtype = (SM_SUN4M_OBP | 0x0) } };

static void __init display_system_type(unsigned char machtype)
{
	char sysname[128];
	register int i;

	for (i = 0; i < ARRAY_SIZE(Sun_Machines); i++) {
		if (Sun_Machines[i].id_machtype == machtype) {
			if (machtype != (SM_SUN4M_OBP | 0x00) ||
			    prom_getproperty(prom_root_node, "banner-name",
					     sysname, sizeof(sysname)) <= 0)
				printk(KERN_WARNING "TYPE: %s\n",
				       Sun_Machines[i].name);
			else
				printk(KERN_WARNING "TYPE: %s\n", sysname);
			return;
		}
	}

	prom_printf("IDPROM: Warning, bogus id_machtype value, 0x%x\n", machtype);
}
#else
static void __init display_system_type(unsigned char machtype)
{
}
#endif
/*                                                        */
static unsigned char __init calc_idprom_cksum(struct idprom *idprom)
{
	unsigned char cksum, i, *ptr = (unsigned char *)idprom;

	for (i = cksum = 0; i <= 0x0E; i++)
		cksum ^= *ptr++;

	return cksum;
}

/*                                                                        */
void __init idprom_init(void)
{
	prom_get_idprom((char *) &idprom_buffer, sizeof(idprom_buffer));

	idprom = &idprom_buffer;

	if (idprom->id_format != 0x01)
		prom_printf("IDPROM: Warning, unknown format type!\n");

	if (idprom->id_cksum != calc_idprom_cksum(idprom))
		prom_printf("IDPROM: Warning, checksum failure (nvram=%x, calc=%x)!\n",
			    idprom->id_cksum, calc_idprom_cksum(idprom));

	display_system_type(idprom->id_machtype);

	printk(KERN_WARNING "Ethernet address: %pM\n", idprom->id_ethaddr);
}
