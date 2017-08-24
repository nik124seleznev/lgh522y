/*
 * CPU kernel entry/exit control
 *
 * Copyright (C) 2013 ARM Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <asm/cpu_ops.h>
#include <asm/smp_plat.h>
#include <linux/errno.h>
#include <linux/of.h>
#include <linux/string.h>

extern const struct cpu_operations smp_spin_table_ops;
extern const struct cpu_operations cpu_psci_ops;
#ifdef CONFIG_MTK_PSCI
extern const struct cpu_operations mt_cpu_psci_ops;
#endif

const struct cpu_operations *cpu_ops[NR_CPUS];

static const struct cpu_operations *supported_cpu_ops[] __initconst = {
#ifdef CONFIG_SMP
	&smp_spin_table_ops,
	&cpu_psci_ops,
#ifdef CONFIG_MTK_PSCI
	&mt_cpu_psci_ops,
#endif
#endif
	NULL,
};

static const struct cpu_operations * __init cpu_get_ops(const char *name)
{
	const struct cpu_operations **ops = supported_cpu_ops;

	while (*ops) {
		if (!strcmp(name, (*ops)->name))
			return *ops;

		ops++;
	}

	return NULL;
}

/*
                                                                            
 */
int __init cpu_read_ops(struct device_node *dn, int cpu)
{
	const char *enable_method = of_get_property(dn, "enable-method", NULL);
	if (!enable_method) {
		/*
                                                          
                                                                
   */
		if (cpu != 0)
			pr_err("%s: missing enable-method property\n",
				dn->full_name);
		return -ENOENT;
	}

	cpu_ops[cpu] = cpu_get_ops(enable_method);
	if (!cpu_ops[cpu]) {
		pr_err("%s: invalid enable-method property: %s\n",
		       dn->full_name, enable_method);
		return -EOPNOTSUPP;
	}

	return 0;
}

void __init cpu_read_bootcpu_ops(void)
{
	struct device_node *dn = NULL;
	u64 mpidr = cpu_logical_map(0);

	while ((dn = of_find_node_by_type(dn, "cpu"))) {
		u64 hwid;
		const __be32 *prop;

		prop = of_get_property(dn, "reg", NULL);
		if (!prop)
			continue;

		hwid = of_read_number(prop, of_n_addr_cells(dn));
		if (hwid == mpidr) {
			cpu_read_ops(dn, 0);
			of_node_put(dn);
			return;
		}
	}
}