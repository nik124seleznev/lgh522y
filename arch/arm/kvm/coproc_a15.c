/*
 * Copyright (C) 2012 - Virtual Open Systems and Columbia University
 * Authors: Rusty Russell <rusty@rustcorp.au>
 *          Christoffer Dall <c.dall@virtualopensystems.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <linux/kvm_host.h>
#include <asm/cputype.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_host.h>
#include <asm/kvm_emulate.h>
#include <asm/kvm_coproc.h>
#include <linux/init.h>

static void reset_mpidr(struct kvm_vcpu *vcpu, const struct coproc_reg *r)
{
	/*
                        
                                                            
                                                               
                                                             
                        
  */
	vcpu->arch.cp15[c0_MPIDR] = (read_cpuid_mpidr() & ~MPIDR_LEVEL_MASK)
		| (vcpu->vcpu_id & MPIDR_LEVEL_MASK);
}

#include "coproc.h"

/*                       */
static bool access_actlr(struct kvm_vcpu *vcpu,
			 const struct coproc_params *p,
			 const struct coproc_reg *r)
{
	if (p->is_write)
		return ignore_write(vcpu, p);

	*vcpu_reg(vcpu, p->Rt1) = vcpu->arch.cp15[c1_ACTLR];
	return true;
}

/*                      */
static bool access_cbar(struct kvm_vcpu *vcpu,
			const struct coproc_params *p,
			const struct coproc_reg *r)
{
	if (p->is_write)
		return write_to_read_only(vcpu, p);
	return read_zero(vcpu, p);
}

/*                         */
static bool access_l2ctlr(struct kvm_vcpu *vcpu,
			  const struct coproc_params *p,
			  const struct coproc_reg *r)
{
	if (p->is_write)
		return ignore_write(vcpu, p);

	*vcpu_reg(vcpu, p->Rt1) = vcpu->arch.cp15[c9_L2CTLR];
	return true;
}

static void reset_l2ctlr(struct kvm_vcpu *vcpu, const struct coproc_reg *r)
{
	u32 l2ctlr, ncores;

	asm volatile("mrc p15, 1, %0, c9, c0, 2\n" : "=r" (l2ctlr));
	l2ctlr &= ~(3 << 24);
	ncores = atomic_read(&vcpu->kvm->online_vcpus) - 1;
	l2ctlr |= (ncores & 3) << 24;

	vcpu->arch.cp15[c9_L2CTLR] = l2ctlr;
}

static void reset_actlr(struct kvm_vcpu *vcpu, const struct coproc_reg *r)
{
	u32 actlr;

	/*                                                              */
	asm volatile("mrc p15, 0, %0, c1, c0, 1\n" : "=r" (actlr));
	/*                                                          */
	if (atomic_read(&vcpu->kvm->online_vcpus) > 1)
		actlr |= 1U << 6;
	else
		actlr &= ~(1U << 6);

	vcpu->arch.cp15[c1_ACTLR] = actlr;
}

/*                                                                           */
static bool access_l2ectlr(struct kvm_vcpu *vcpu,
			   const struct coproc_params *p,
			   const struct coproc_reg *r)
{
	if (p->is_write)
		return ignore_write(vcpu, p);

	*vcpu_reg(vcpu, p->Rt1) = 0;
	return true;
}

/*
                               
                                                                           
                                                                              
                  
                                                                            
                                              
 */
static const struct coproc_reg a15_regs[] = {
	/*                                        */
	{ CRn( 0), CRm( 0), Op1( 0), Op2( 5), is32,
			NULL, reset_mpidr, c0_MPIDR },

	/*                                */
	{ CRn( 1), CRm( 0), Op1( 0), Op2( 0), is32,
			NULL, reset_val, c1_SCTLR, 0x00C50078 },
	/*                                */
	{ CRn( 1), CRm( 0), Op1( 0), Op2( 1), is32,
			access_actlr, reset_actlr, c1_ACTLR },
	/*                                */
	{ CRn( 1), CRm( 0), Op1( 0), Op2( 2), is32,
			NULL, reset_val, c1_CPACR, 0x00000000 },

	/*
                                              
  */
	{ CRn( 9), CRm( 0), Op1( 1), Op2( 2), is32,
			access_l2ctlr, reset_l2ctlr, c9_L2CTLR },
	{ CRn( 9), CRm( 0), Op1( 1), Op2( 3), is32, access_l2ectlr},

	/*                                          */
	{ CRn(15), CRm( 0), Op1( 4), Op2( 0), is32, access_cbar},
};

static struct kvm_coproc_target_table a15_target_table = {
	.target = KVM_ARM_TARGET_CORTEX_A15,
	.table = a15_regs,
	.num = ARRAY_SIZE(a15_regs),
};

static int __init coproc_a15_init(void)
{
	unsigned int i;

	for (i = 1; i < ARRAY_SIZE(a15_regs); i++)
		BUG_ON(cmp_reg(&a15_regs[i-1],
			       &a15_regs[i]) >= 0);

	kvm_register_target_coproc_table(&a15_target_table);
	return 0;
}
late_initcall(coproc_a15_init);
