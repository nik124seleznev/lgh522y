/*
 * INET		An implementation of the TCP/IP protocol suite for the LINUX
 *		operating system.  INET is implemented using the  BSD Socket
 *		interface as the means of communication with the user level.
 *
 *		Holds initial configuration information for devices.
 *
 * Version:	@(#)Space.c	1.0.7	08/12/93
 *
 * Authors:	Ross Biro
 *		Fred N. van Kempen, <waltje@uWalt.NL.Mugnet.ORG>
 *		Donald J. Becker, <becker@scyld.com>
 *
 * Changelog:
 *		Stephen Hemminger (09/2003)
 *		- get rid of pre-linked dev list, dynamic device allocation
 *		Paul Gortmaker (03/2002)
 *		- struct init cleanup, enable multiple ISA autoprobes.
 *		Arnaldo Carvalho de Melo <acme@conectiva.com.br> - 09/1999
 *		- fix sbni: s/device/net_device/
 *		Paul Gortmaker (06/98):
 *		 - sort probes in a sane way, make sure all (safe) probes
 *		   get run once & failed autoprobes don't autoprobe again.
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/netlink.h>

/*                                                                        
                                                 
   */

extern struct net_device *hp100_probe(int unit);
extern struct net_device *ultra_probe(int unit);
extern struct net_device *wd_probe(int unit);
extern struct net_device *ne_probe(int unit);
extern struct net_device *fmv18x_probe(int unit);
extern struct net_device *i82596_probe(int unit);
extern struct net_device *ni65_probe(int unit);
extern struct net_device *sonic_probe(int unit);
extern struct net_device *smc_init(int unit);
extern struct net_device *atarilance_probe(int unit);
extern struct net_device *sun3lance_probe(int unit);
extern struct net_device *sun3_82586_probe(int unit);
extern struct net_device *apne_probe(int unit);
extern struct net_device *cs89x0_probe(int unit);
extern struct net_device *mvme147lance_probe(int unit);
extern struct net_device *tc515_probe(int unit);
extern struct net_device *lance_probe(int unit);
extern struct net_device *mac8390_probe(int unit);
extern struct net_device *mac89x0_probe(int unit);
extern struct net_device *cops_probe(int unit);
extern struct net_device *ltpc_probe(void);

/*                        */
extern int iph5526_probe(struct net_device *dev);

/*               */
extern int sbni_probe(int unit);

struct devprobe2 {
	struct net_device *(*probe)(int unit);
	int status;	/*                                  */
};

static int __init probe_list2(int unit, struct devprobe2 *p, int autoprobe)
{
	struct net_device *dev;
	for (; p->probe; p++) {
		if (autoprobe && p->status)
			continue;
		dev = p->probe(unit);
		if (!IS_ERR(dev))
			return 0;
		if (autoprobe)
			p->status = PTR_ERR(dev);
	}
	return -ENODEV;
}

/*
                                                                     
                                                     
 */
static struct devprobe2 isa_probes[] __initdata = {
#if defined(CONFIG_HP100) && defined(CONFIG_ISA)	/*           */
	{hp100_probe, 0},
#endif
#ifdef CONFIG_3C515
	{tc515_probe, 0},
#endif
#ifdef CONFIG_ULTRA
	{ultra_probe, 0},
#endif
#ifdef CONFIG_WD80x3
	{wd_probe, 0},
#endif
#if defined(CONFIG_NE2000) || \
    defined(CONFIG_NE_H8300)  /*                                  */
	{ne_probe, 0},
#endif
#ifdef CONFIG_LANCE		/*                                     */
	{lance_probe, 0},
#endif
#ifdef CONFIG_SMC9194
	{smc_init, 0},
#endif
#ifdef CONFIG_CS89x0
#ifndef CONFIG_CS89x0_PLATFORM
 	{cs89x0_probe, 0},
#endif
#endif
#if defined(CONFIG_MVME16x_NET) || defined(CONFIG_BVME6000_NET)	/*              */
	{i82596_probe, 0},
#endif
#ifdef CONFIG_NI65
	{ni65_probe, 0},
#endif
	{NULL, 0},
};

static struct devprobe2 m68k_probes[] __initdata = {
#ifdef CONFIG_ATARILANCE	/*                                   */
	{atarilance_probe, 0},
#endif
#ifdef CONFIG_SUN3LANCE         /*                         */
	{sun3lance_probe, 0},
#endif
#ifdef CONFIG_SUN3_82586        /*                               */
	{sun3_82586_probe, 0},
#endif
#ifdef CONFIG_APNE		/*                     */
	{apne_probe, 0},
#endif
#ifdef CONFIG_MVME147_NET	/*                           */
	{mvme147lance_probe, 0},
#endif
#ifdef CONFIG_MAC8390           /*                          */
	{mac8390_probe, 0},
#endif
#ifdef CONFIG_MAC89x0
 	{mac89x0_probe, 0},
#endif
	{NULL, 0},
};

/*
                                                                
                                                                  
 */

static void __init ethif_probe2(int unit)
{
	unsigned long base_addr = netdev_boot_base("eth", unit);

	if (base_addr == 1)
		return;

	(void)(	probe_list2(unit, m68k_probes, base_addr == 0) &&
		probe_list2(unit, isa_probes, base_addr == 0));
}

/*                                                       */
static int __init net_olddevs_init(void)
{
	int num;

#ifdef CONFIG_SBNI
	for (num = 0; num < 8; ++num)
		sbni_probe(num);
#endif
	for (num = 0; num < 8; ++num)
		ethif_probe2(num);

#ifdef CONFIG_COPS
	cops_probe(0);
	cops_probe(1);
	cops_probe(2);
#endif
#ifdef CONFIG_LTPC
	ltpc_probe();
#endif

	return 0;
}

device_initcall(net_olddevs_init);
