/*
 * Copyright (C) 2012 - Virtual Open Systems and Columbia University
 * Authors: Rusty Russell <rusty@rustcorp.com.au>
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
#include <linux/mm.h>
#include <linux/kvm_host.h>
#include <linux/uaccess.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_host.h>
#include <asm/kvm_emulate.h>
#include <asm/kvm_coproc.h>
#include <asm/cacheflush.h>
#include <asm/cputype.h>
#include <trace/events/kvm.h>
#include <asm/vfp.h>
#include "../vfp/vfpinstr.h"

#include "trace.h"
#include "coproc.h"


/*                                                                             
                         
                                                                             */

/*                                                                        */
static u32 cache_levels;

/*                                                          */
#define CSSELR_MAX 12

int kvm_handle_cp10_id(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	kvm_inject_undefined(vcpu);
	return 1;
}

int kvm_handle_cp_0_13_access(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	/*
                                                                      
                                                       
  */
	kvm_inject_undefined(vcpu);
	return 1;
}

int kvm_handle_cp14_load_store(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	kvm_inject_undefined(vcpu);
	return 1;
}

int kvm_handle_cp14_access(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	kvm_inject_undefined(vcpu);
	return 1;
}

/*                             */
static bool access_dcsw(struct kvm_vcpu *vcpu,
			const struct coproc_params *p,
			const struct coproc_reg *r)
{
	unsigned long val;
	int cpu;

	if (!p->is_write)
		return read_from_write_only(vcpu, p);

	cpu = get_cpu();

	cpumask_setall(&vcpu->arch.require_dcache_flush);
	cpumask_clear_cpu(cpu, &vcpu->arch.require_dcache_flush);

	/*                                                        */
	if (cpu != vcpu->arch.last_pcpu) {
		flush_cache_all();
		goto done;
	}

	val = *vcpu_reg(vcpu, p->Rt1);

	switch (p->CRm) {
	case 6:			/*                                          */
	case 14:		/*        */
		asm volatile("mcr p15, 0, %0, c7, c14, 2" : : "r" (val));
		break;

	case 10:		/*       */
		asm volatile("mcr p15, 0, %0, c7, c10, 2" : : "r" (val));
		break;
	}

done:
	put_cpu();

	return true;
}

/*
                                                                        
                                                                            
                                          
  
                                                                     
                                                                      
                                                                   
 */
static bool pm_fake(struct kvm_vcpu *vcpu,
		    const struct coproc_params *p,
		    const struct coproc_reg *r)
{
	if (p->is_write)
		return ignore_write(vcpu, p);
	else
		return read_zero(vcpu, p);
}

#define access_pmcr pm_fake
#define access_pmcntenset pm_fake
#define access_pmcntenclr pm_fake
#define access_pmovsr pm_fake
#define access_pmselr pm_fake
#define access_pmceid0 pm_fake
#define access_pmceid1 pm_fake
#define access_pmccntr pm_fake
#define access_pmxevtyper pm_fake
#define access_pmxevcntr pm_fake
#define access_pmuserenr pm_fake
#define access_pmintenset pm_fake
#define access_pmintenclr pm_fake

/*                            
                                                                           
                                                                              
                  
                                                                            
                                              
 */
static const struct coproc_reg cp15_regs[] = {
	/*                                 */
	{ CRn( 0), CRm( 0), Op1( 2), Op2( 0), is32,
			NULL, reset_unknown, c0_CSSELR },

	/*                                      */
	{ CRm64( 2), Op1( 0), is64, NULL, reset_unknown64, c2_TTBR0 },
	{ CRm64( 2), Op1( 1), is64, NULL, reset_unknown64, c2_TTBR1 },

	/*                                */
	{ CRn( 2), CRm( 0), Op1( 0), Op2( 2), is32,
			NULL, reset_val, c2_TTBCR, 0x00000000 },

	/*                               */
	{ CRn( 3), CRm( 0), Op1( 0), Op2( 0), is32,
			NULL, reset_unknown, c3_DACR },

	/*                                                */
	{ CRn( 5), CRm( 0), Op1( 0), Op2( 0), is32,
			NULL, reset_unknown, c5_DFSR },
	{ CRn( 5), CRm( 0), Op1( 0), Op2( 1), is32,
			NULL, reset_unknown, c5_IFSR },
	{ CRn( 5), CRm( 1), Op1( 0), Op2( 0), is32,
			NULL, reset_unknown, c5_ADFSR },
	{ CRn( 5), CRm( 1), Op1( 0), Op2( 1), is32,
			NULL, reset_unknown, c5_AIFSR },

	/*                                    */
	{ CRn( 6), CRm( 0), Op1( 0), Op2( 0), is32,
			NULL, reset_unknown, c6_DFAR },
	{ CRn( 6), CRm( 0), Op1( 0), Op2( 2), is32,
			NULL, reset_unknown, c6_IFAR },

	/*                            */
	{ CRm64( 7), Op1( 0), is64, NULL, reset_unknown64, c7_PAR },

	/*
                            
  */
	{ CRn( 7), CRm( 6), Op1( 0), Op2( 2), is32, access_dcsw},
	{ CRn( 7), CRm(10), Op1( 0), Op2( 2), is32, access_dcsw},
	{ CRn( 7), CRm(14), Op1( 0), Op2( 2), is32, access_dcsw},
	/*
                                             
  */
	{ CRn( 9), CRm(12), Op1( 0), Op2( 0), is32, access_pmcr},
	{ CRn( 9), CRm(12), Op1( 0), Op2( 1), is32, access_pmcntenset},
	{ CRn( 9), CRm(12), Op1( 0), Op2( 2), is32, access_pmcntenclr},
	{ CRn( 9), CRm(12), Op1( 0), Op2( 3), is32, access_pmovsr},
	{ CRn( 9), CRm(12), Op1( 0), Op2( 5), is32, access_pmselr},
	{ CRn( 9), CRm(12), Op1( 0), Op2( 6), is32, access_pmceid0},
	{ CRn( 9), CRm(12), Op1( 0), Op2( 7), is32, access_pmceid1},
	{ CRn( 9), CRm(13), Op1( 0), Op2( 0), is32, access_pmccntr},
	{ CRn( 9), CRm(13), Op1( 0), Op2( 1), is32, access_pmxevtyper},
	{ CRn( 9), CRm(13), Op1( 0), Op2( 2), is32, access_pmxevcntr},
	{ CRn( 9), CRm(14), Op1( 0), Op2( 0), is32, access_pmuserenr},
	{ CRn( 9), CRm(14), Op1( 0), Op2( 1), is32, access_pmintenset},
	{ CRn( 9), CRm(14), Op1( 0), Op2( 2), is32, access_pmintenclr},

	/*                                                      */
	{ CRn(10), CRm( 2), Op1( 0), Op2( 0), is32,
			NULL, reset_unknown, c10_PRRR},
	{ CRn(10), CRm( 2), Op1( 0), Op2( 1), is32,
			NULL, reset_unknown, c10_NMRR},

	/*                               */
	{ CRn(12), CRm( 0), Op1( 0), Op2( 0), is32,
			NULL, reset_val, c12_VBAR, 0x00000000 },

	/*                                                                */
	{ CRn(13), CRm( 0), Op1( 0), Op2( 1), is32,
			NULL, reset_val, c13_CID, 0x00000000 },
	{ CRn(13), CRm( 0), Op1( 0), Op2( 2), is32,
			NULL, reset_unknown, c13_TID_URW },
	{ CRn(13), CRm( 0), Op1( 0), Op2( 3), is32,
			NULL, reset_unknown, c13_TID_URO },
	{ CRn(13), CRm( 0), Op1( 0), Op2( 4), is32,
			NULL, reset_unknown, c13_TID_PRIV },

	/*                                  */
	{ CRn(14), CRm( 1), Op1( 0), Op2( 0), is32,
			NULL, reset_val, c14_CNTKCTL, 0x00000000 },
};

/*                                  */
static struct kvm_coproc_target_table *target_tables[KVM_ARM_NUM_TARGETS];

void kvm_register_target_coproc_table(struct kvm_coproc_target_table *table)
{
	target_tables[table->target] = table;
}

/*                                              */
static const struct coproc_reg *get_target_table(unsigned target, size_t *num)
{
	struct kvm_coproc_target_table *table;

	table = target_tables[target];
	*num = table->num;
	return table->table;
}

static const struct coproc_reg *find_reg(const struct coproc_params *params,
					 const struct coproc_reg table[],
					 unsigned int num)
{
	unsigned int i;

	for (i = 0; i < num; i++) {
		const struct coproc_reg *r = &table[i];

		if (params->is_64bit != r->is_64)
			continue;
		if (params->CRn != r->CRn)
			continue;
		if (params->CRm != r->CRm)
			continue;
		if (params->Op1 != r->Op1)
			continue;
		if (params->Op2 != r->Op2)
			continue;

		return r;
	}
	return NULL;
}

static int emulate_cp15(struct kvm_vcpu *vcpu,
			const struct coproc_params *params)
{
	size_t num;
	const struct coproc_reg *table, *r;

	trace_kvm_emulate_cp15_imp(params->Op1, params->Rt1, params->CRn,
				   params->CRm, params->Op2, params->is_write);

	table = get_target_table(vcpu->arch.target, &num);

	/*                                            */
	r = find_reg(params, table, num);
	if (!r)
		r = find_reg(params, cp15_regs, ARRAY_SIZE(cp15_regs));

	if (likely(r)) {
		/*                                                         */
		BUG_ON(!r->access);

		if (likely(r->access(vcpu, params, r))) {
			/*                                         */
			kvm_skip_instr(vcpu, kvm_vcpu_trap_il_is32bit(vcpu));
			return 1;
		}
		/*                                               */
	} else {
		kvm_err("Unsupported guest CP15 access at: %08lx\n",
			*vcpu_pc(vcpu));
		print_cp_instr(params);
	}
	kvm_inject_undefined(vcpu);
	return 1;
}

/* 
                                                                        
                          
                            
 */
int kvm_handle_cp15_64(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	struct coproc_params params;

	params.CRm = (kvm_vcpu_get_hsr(vcpu) >> 1) & 0xf;
	params.Rt1 = (kvm_vcpu_get_hsr(vcpu) >> 5) & 0xf;
	params.is_write = ((kvm_vcpu_get_hsr(vcpu) & 1) == 0);
	params.is_64bit = true;

	params.Op1 = (kvm_vcpu_get_hsr(vcpu) >> 16) & 0xf;
	params.Op2 = 0;
	params.Rt2 = (kvm_vcpu_get_hsr(vcpu) >> 10) & 0xf;
	params.CRn = 0;

	return emulate_cp15(vcpu, &params);
}

static void reset_coproc_regs(struct kvm_vcpu *vcpu,
			      const struct coproc_reg *table, size_t num)
{
	unsigned long i;

	for (i = 0; i < num; i++)
		if (table[i].reset)
			table[i].reset(vcpu, &table[i]);
}

/* 
                                                                      
                          
                            
 */
int kvm_handle_cp15_32(struct kvm_vcpu *vcpu, struct kvm_run *run)
{
	struct coproc_params params;

	params.CRm = (kvm_vcpu_get_hsr(vcpu) >> 1) & 0xf;
	params.Rt1 = (kvm_vcpu_get_hsr(vcpu) >> 5) & 0xf;
	params.is_write = ((kvm_vcpu_get_hsr(vcpu) & 1) == 0);
	params.is_64bit = false;

	params.CRn = (kvm_vcpu_get_hsr(vcpu) >> 10) & 0xf;
	params.Op1 = (kvm_vcpu_get_hsr(vcpu) >> 14) & 0x7;
	params.Op2 = (kvm_vcpu_get_hsr(vcpu) >> 17) & 0x7;
	params.Rt2 = 0;

	return emulate_cp15(vcpu, &params);
}

/*                                                                             
                
                                                                             */

static bool index_to_params(u64 id, struct coproc_params *params)
{
	switch (id & KVM_REG_SIZE_MASK) {
	case KVM_REG_SIZE_U32:
		/*                                             */
		if (id & ~(KVM_REG_ARCH_MASK | KVM_REG_SIZE_MASK
			   | KVM_REG_ARM_COPROC_MASK
			   | KVM_REG_ARM_32_CRN_MASK
			   | KVM_REG_ARM_CRM_MASK
			   | KVM_REG_ARM_OPC1_MASK
			   | KVM_REG_ARM_32_OPC2_MASK))
			return false;

		params->is_64bit = false;
		params->CRn = ((id & KVM_REG_ARM_32_CRN_MASK)
			       >> KVM_REG_ARM_32_CRN_SHIFT);
		params->CRm = ((id & KVM_REG_ARM_CRM_MASK)
			       >> KVM_REG_ARM_CRM_SHIFT);
		params->Op1 = ((id & KVM_REG_ARM_OPC1_MASK)
			       >> KVM_REG_ARM_OPC1_SHIFT);
		params->Op2 = ((id & KVM_REG_ARM_32_OPC2_MASK)
			       >> KVM_REG_ARM_32_OPC2_SHIFT);
		return true;
	case KVM_REG_SIZE_U64:
		/*                                             */
		if (id & ~(KVM_REG_ARCH_MASK | KVM_REG_SIZE_MASK
			      | KVM_REG_ARM_COPROC_MASK
			      | KVM_REG_ARM_CRM_MASK
			      | KVM_REG_ARM_OPC1_MASK))
			return false;
		params->is_64bit = true;
		/*                                           */
		params->CRn = ((id & KVM_REG_ARM_CRM_MASK)
			       >> KVM_REG_ARM_CRM_SHIFT);
		params->Op1 = ((id & KVM_REG_ARM_OPC1_MASK)
			       >> KVM_REG_ARM_OPC1_SHIFT);
		params->Op2 = 0;
		params->CRm = 0;
		return true;
	default:
		return false;
	}
}

/*                                                            */
static const struct coproc_reg *index_to_coproc_reg(struct kvm_vcpu *vcpu,
						    u64 id)
{
	size_t num;
	const struct coproc_reg *table, *r;
	struct coproc_params params;

	/*                          */
	if ((id & KVM_REG_ARM_COPROC_MASK) >> KVM_REG_ARM_COPROC_SHIFT != 15)
		return NULL;

	if (!index_to_params(id, &params))
		return NULL;

	table = get_target_table(vcpu->arch.target, &num);
	r = find_reg(&params, table, num);
	if (!r)
		r = find_reg(&params, cp15_regs, ARRAY_SIZE(cp15_regs));

	/*                              */
	if (r && !r->reg)
		r = NULL;

	return r;
}

/*
                                                                        
                                                         
  
                                                                      
                                            
 */
/*                                                                   */
#define FUNCTION_FOR32(crn, crm, op1, op2, name)			\
	static void get_##name(struct kvm_vcpu *v,			\
			       const struct coproc_reg *r)		\
	{								\
		u32 val;						\
									\
		asm volatile("mrc p15, " __stringify(op1)		\
			     ", %0, c" __stringify(crn)			\
			     ", c" __stringify(crm)			\
			     ", " __stringify(op2) "\n" : "=r" (val));	\
		((struct coproc_reg *)r)->val = val;			\
	}

FUNCTION_FOR32(0, 0, 0, 0, MIDR)
FUNCTION_FOR32(0, 0, 0, 1, CTR)
FUNCTION_FOR32(0, 0, 0, 2, TCMTR)
FUNCTION_FOR32(0, 0, 0, 3, TLBTR)
FUNCTION_FOR32(0, 0, 0, 6, REVIDR)
FUNCTION_FOR32(0, 1, 0, 0, ID_PFR0)
FUNCTION_FOR32(0, 1, 0, 1, ID_PFR1)
FUNCTION_FOR32(0, 1, 0, 2, ID_DFR0)
FUNCTION_FOR32(0, 1, 0, 3, ID_AFR0)
FUNCTION_FOR32(0, 1, 0, 4, ID_MMFR0)
FUNCTION_FOR32(0, 1, 0, 5, ID_MMFR1)
FUNCTION_FOR32(0, 1, 0, 6, ID_MMFR2)
FUNCTION_FOR32(0, 1, 0, 7, ID_MMFR3)
FUNCTION_FOR32(0, 2, 0, 0, ID_ISAR0)
FUNCTION_FOR32(0, 2, 0, 1, ID_ISAR1)
FUNCTION_FOR32(0, 2, 0, 2, ID_ISAR2)
FUNCTION_FOR32(0, 2, 0, 3, ID_ISAR3)
FUNCTION_FOR32(0, 2, 0, 4, ID_ISAR4)
FUNCTION_FOR32(0, 2, 0, 5, ID_ISAR5)
FUNCTION_FOR32(0, 0, 1, 1, CLIDR)
FUNCTION_FOR32(0, 0, 1, 7, AIDR)

/*                                                         */
static struct coproc_reg invariant_cp15[] = {
	{ CRn( 0), CRm( 0), Op1( 0), Op2( 0), is32, NULL, get_MIDR },
	{ CRn( 0), CRm( 0), Op1( 0), Op2( 1), is32, NULL, get_CTR },
	{ CRn( 0), CRm( 0), Op1( 0), Op2( 2), is32, NULL, get_TCMTR },
	{ CRn( 0), CRm( 0), Op1( 0), Op2( 3), is32, NULL, get_TLBTR },
	{ CRn( 0), CRm( 0), Op1( 0), Op2( 6), is32, NULL, get_REVIDR },

	{ CRn( 0), CRm( 1), Op1( 0), Op2( 0), is32, NULL, get_ID_PFR0 },
	{ CRn( 0), CRm( 1), Op1( 0), Op2( 1), is32, NULL, get_ID_PFR1 },
	{ CRn( 0), CRm( 1), Op1( 0), Op2( 2), is32, NULL, get_ID_DFR0 },
	{ CRn( 0), CRm( 1), Op1( 0), Op2( 3), is32, NULL, get_ID_AFR0 },
	{ CRn( 0), CRm( 1), Op1( 0), Op2( 4), is32, NULL, get_ID_MMFR0 },
	{ CRn( 0), CRm( 1), Op1( 0), Op2( 5), is32, NULL, get_ID_MMFR1 },
	{ CRn( 0), CRm( 1), Op1( 0), Op2( 6), is32, NULL, get_ID_MMFR2 },
	{ CRn( 0), CRm( 1), Op1( 0), Op2( 7), is32, NULL, get_ID_MMFR3 },

	{ CRn( 0), CRm( 2), Op1( 0), Op2( 0), is32, NULL, get_ID_ISAR0 },
	{ CRn( 0), CRm( 2), Op1( 0), Op2( 1), is32, NULL, get_ID_ISAR1 },
	{ CRn( 0), CRm( 2), Op1( 0), Op2( 2), is32, NULL, get_ID_ISAR2 },
	{ CRn( 0), CRm( 2), Op1( 0), Op2( 3), is32, NULL, get_ID_ISAR3 },
	{ CRn( 0), CRm( 2), Op1( 0), Op2( 4), is32, NULL, get_ID_ISAR4 },
	{ CRn( 0), CRm( 2), Op1( 0), Op2( 5), is32, NULL, get_ID_ISAR5 },

	{ CRn( 0), CRm( 0), Op1( 1), Op2( 1), is32, NULL, get_CLIDR },
	{ CRn( 0), CRm( 0), Op1( 1), Op2( 7), is32, NULL, get_AIDR },
};

static int reg_from_user(void *val, const void __user *uaddr, u64 id)
{
	/*                                               */
	if (copy_from_user(val, uaddr, KVM_REG_SIZE(id)) != 0)
		return -EFAULT;
	return 0;
}

static int reg_to_user(void __user *uaddr, const void *val, u64 id)
{
	/*                                               */
	if (copy_to_user(uaddr, val, KVM_REG_SIZE(id)) != 0)
		return -EFAULT;
	return 0;
}

static int get_invariant_cp15(u64 id, void __user *uaddr)
{
	struct coproc_params params;
	const struct coproc_reg *r;

	if (!index_to_params(id, &params))
		return -ENOENT;

	r = find_reg(&params, invariant_cp15, ARRAY_SIZE(invariant_cp15));
	if (!r)
		return -ENOENT;

	return reg_to_user(uaddr, &r->val, id);
}

static int set_invariant_cp15(u64 id, void __user *uaddr)
{
	struct coproc_params params;
	const struct coproc_reg *r;
	int err;
	u64 val = 0; /*                                           */

	if (!index_to_params(id, &params))
		return -ENOENT;
	r = find_reg(&params, invariant_cp15, ARRAY_SIZE(invariant_cp15));
	if (!r)
		return -ENOENT;

	err = reg_from_user(&val, uaddr, id);
	if (err)
		return err;

	/*                                                         */
	if (r->val != val)
		return -EINVAL;

	return 0;
}

static bool is_valid_cache(u32 val)
{
	u32 level, ctype;

	if (val >= CSSELR_MAX)
		return -ENOENT;

	/*                                                                */
        level = (val >> 1);
        ctype = (cache_levels >> (level * 3)) & 7;

	switch (ctype) {
	case 0: /*          */
		return false;
	case 1: /*                        */
		return (val & 1);
	case 2: /*                 */
	case 4: /*               */
		return !(val & 1);
	case 3: /*                                      */
		return true;
	default: /*                                              */
		return false;
	}
}

/*                                                        */
static u32 get_ccsidr(u32 csselr)
{
	u32 ccsidr;

	/*                                                  */
	local_irq_disable();
	/*                       */
	asm volatile("mcr p15, 2, %0, c0, c0, 0" : : "r" (csselr));
	isb();
	/*                           */
	asm volatile("mrc p15, 1, %0, c0, c0, 0" : "=r" (ccsidr));
	local_irq_enable();

	return ccsidr;
}

static int demux_c15_get(u64 id, void __user *uaddr)
{
	u32 val;
	u32 __user *uval = uaddr;

	/*                                   */
	if (id & ~(KVM_REG_ARCH_MASK|KVM_REG_SIZE_MASK|KVM_REG_ARM_COPROC_MASK
		   | ((1 << KVM_REG_ARM_COPROC_SHIFT)-1)))
		return -ENOENT;

	switch (id & KVM_REG_ARM_DEMUX_ID_MASK) {
	case KVM_REG_ARM_DEMUX_ID_CCSIDR:
		if (KVM_REG_SIZE(id) != 4)
			return -ENOENT;
		val = (id & KVM_REG_ARM_DEMUX_VAL_MASK)
			>> KVM_REG_ARM_DEMUX_VAL_SHIFT;
		if (!is_valid_cache(val))
			return -ENOENT;

		return put_user(get_ccsidr(val), uval);
	default:
		return -ENOENT;
	}
}

static int demux_c15_set(u64 id, void __user *uaddr)
{
	u32 val, newval;
	u32 __user *uval = uaddr;

	/*                                   */
	if (id & ~(KVM_REG_ARCH_MASK|KVM_REG_SIZE_MASK|KVM_REG_ARM_COPROC_MASK
		   | ((1 << KVM_REG_ARM_COPROC_SHIFT)-1)))
		return -ENOENT;

	switch (id & KVM_REG_ARM_DEMUX_ID_MASK) {
	case KVM_REG_ARM_DEMUX_ID_CCSIDR:
		if (KVM_REG_SIZE(id) != 4)
			return -ENOENT;
		val = (id & KVM_REG_ARM_DEMUX_VAL_MASK)
			>> KVM_REG_ARM_DEMUX_VAL_SHIFT;
		if (!is_valid_cache(val))
			return -ENOENT;

		if (get_user(newval, uval))
			return -EFAULT;

		/*                                              */
		if (newval != get_ccsidr(val))
			return -EINVAL;
		return 0;
	default:
		return -ENOENT;
	}
}

#ifdef CONFIG_VFPv3
static const int vfp_sysregs[] = { KVM_REG_ARM_VFP_FPEXC,
				   KVM_REG_ARM_VFP_FPSCR,
				   KVM_REG_ARM_VFP_FPINST,
				   KVM_REG_ARM_VFP_FPINST2,
				   KVM_REG_ARM_VFP_MVFR0,
				   KVM_REG_ARM_VFP_MVFR1,
				   KVM_REG_ARM_VFP_FPSID };

static unsigned int num_fp_regs(void)
{
	if (((fmrx(MVFR0) & MVFR0_A_SIMD_MASK) >> MVFR0_A_SIMD_BIT) == 2)
		return 32;
	else
		return 16;
}

static unsigned int num_vfp_regs(void)
{
	/*                                */
	return num_fp_regs() + ARRAY_SIZE(vfp_sysregs);
}

static int copy_vfp_regids(u64 __user *uindices)
{
	unsigned int i;
	const u64 u32reg = KVM_REG_ARM | KVM_REG_SIZE_U32 | KVM_REG_ARM_VFP;
	const u64 u64reg = KVM_REG_ARM | KVM_REG_SIZE_U64 | KVM_REG_ARM_VFP;

	for (i = 0; i < num_fp_regs(); i++) {
		if (put_user((u64reg | KVM_REG_ARM_VFP_BASE_REG) + i,
			     uindices))
			return -EFAULT;
		uindices++;
	}

	for (i = 0; i < ARRAY_SIZE(vfp_sysregs); i++) {
		if (put_user(u32reg | vfp_sysregs[i], uindices))
			return -EFAULT;
		uindices++;
	}

	return num_vfp_regs();
}

static int vfp_get_reg(const struct kvm_vcpu *vcpu, u64 id, void __user *uaddr)
{
	u32 vfpid = (id & KVM_REG_ARM_VFP_MASK);
	u32 val;

	/*                                   */
	if (id & ~(KVM_REG_ARCH_MASK|KVM_REG_SIZE_MASK|KVM_REG_ARM_COPROC_MASK
		   | ((1 << KVM_REG_ARM_COPROC_SHIFT)-1)))
		return -ENOENT;

	if (vfpid < num_fp_regs()) {
		if (KVM_REG_SIZE(id) != 8)
			return -ENOENT;
		return reg_to_user(uaddr, &vcpu->arch.vfp_guest.fpregs[vfpid],
				   id);
	}

	/*                                      */
	if (KVM_REG_SIZE(id) != 4)
		return -ENOENT;

	switch (vfpid) {
	case KVM_REG_ARM_VFP_FPEXC:
		return reg_to_user(uaddr, &vcpu->arch.vfp_guest.fpexc, id);
	case KVM_REG_ARM_VFP_FPSCR:
		return reg_to_user(uaddr, &vcpu->arch.vfp_guest.fpscr, id);
	case KVM_REG_ARM_VFP_FPINST:
		return reg_to_user(uaddr, &vcpu->arch.vfp_guest.fpinst, id);
	case KVM_REG_ARM_VFP_FPINST2:
		return reg_to_user(uaddr, &vcpu->arch.vfp_guest.fpinst2, id);
	case KVM_REG_ARM_VFP_MVFR0:
		val = fmrx(MVFR0);
		return reg_to_user(uaddr, &val, id);
	case KVM_REG_ARM_VFP_MVFR1:
		val = fmrx(MVFR1);
		return reg_to_user(uaddr, &val, id);
	case KVM_REG_ARM_VFP_FPSID:
		val = fmrx(FPSID);
		return reg_to_user(uaddr, &val, id);
	default:
		return -ENOENT;
	}
}

static int vfp_set_reg(struct kvm_vcpu *vcpu, u64 id, const void __user *uaddr)
{
	u32 vfpid = (id & KVM_REG_ARM_VFP_MASK);
	u32 val;

	/*                                   */
	if (id & ~(KVM_REG_ARCH_MASK|KVM_REG_SIZE_MASK|KVM_REG_ARM_COPROC_MASK
		   | ((1 << KVM_REG_ARM_COPROC_SHIFT)-1)))
		return -ENOENT;

	if (vfpid < num_fp_regs()) {
		if (KVM_REG_SIZE(id) != 8)
			return -ENOENT;
		return reg_from_user(&vcpu->arch.vfp_guest.fpregs[vfpid],
				     uaddr, id);
	}

	/*                                      */
	if (KVM_REG_SIZE(id) != 4)
		return -ENOENT;

	switch (vfpid) {
	case KVM_REG_ARM_VFP_FPEXC:
		return reg_from_user(&vcpu->arch.vfp_guest.fpexc, uaddr, id);
	case KVM_REG_ARM_VFP_FPSCR:
		return reg_from_user(&vcpu->arch.vfp_guest.fpscr, uaddr, id);
	case KVM_REG_ARM_VFP_FPINST:
		return reg_from_user(&vcpu->arch.vfp_guest.fpinst, uaddr, id);
	case KVM_REG_ARM_VFP_FPINST2:
		return reg_from_user(&vcpu->arch.vfp_guest.fpinst2, uaddr, id);
	/*                      */
	case KVM_REG_ARM_VFP_MVFR0:
		if (reg_from_user(&val, uaddr, id))
			return -EFAULT;
		if (val != fmrx(MVFR0))
			return -EINVAL;
		return 0;
	case KVM_REG_ARM_VFP_MVFR1:
		if (reg_from_user(&val, uaddr, id))
			return -EFAULT;
		if (val != fmrx(MVFR1))
			return -EINVAL;
		return 0;
	case KVM_REG_ARM_VFP_FPSID:
		if (reg_from_user(&val, uaddr, id))
			return -EFAULT;
		if (val != fmrx(FPSID))
			return -EINVAL;
		return 0;
	default:
		return -ENOENT;
	}
}
#else /*               */
static unsigned int num_vfp_regs(void)
{
	return 0;
}

static int copy_vfp_regids(u64 __user *uindices)
{
	return 0;
}

static int vfp_get_reg(const struct kvm_vcpu *vcpu, u64 id, void __user *uaddr)
{
	return -ENOENT;
}

static int vfp_set_reg(struct kvm_vcpu *vcpu, u64 id, const void __user *uaddr)
{
	return -ENOENT;
}
#endif /*               */

int kvm_arm_coproc_get_reg(struct kvm_vcpu *vcpu, const struct kvm_one_reg *reg)
{
	const struct coproc_reg *r;
	void __user *uaddr = (void __user *)(long)reg->addr;

	if ((reg->id & KVM_REG_ARM_COPROC_MASK) == KVM_REG_ARM_DEMUX)
		return demux_c15_get(reg->id, uaddr);

	if ((reg->id & KVM_REG_ARM_COPROC_MASK) == KVM_REG_ARM_VFP)
		return vfp_get_reg(vcpu, reg->id, uaddr);

	r = index_to_coproc_reg(vcpu, reg->id);
	if (!r)
		return get_invariant_cp15(reg->id, uaddr);

	/*                                          */
	return reg_to_user(uaddr, &vcpu->arch.cp15[r->reg], reg->id);
}

int kvm_arm_coproc_set_reg(struct kvm_vcpu *vcpu, const struct kvm_one_reg *reg)
{
	const struct coproc_reg *r;
	void __user *uaddr = (void __user *)(long)reg->addr;

	if ((reg->id & KVM_REG_ARM_COPROC_MASK) == KVM_REG_ARM_DEMUX)
		return demux_c15_set(reg->id, uaddr);

	if ((reg->id & KVM_REG_ARM_COPROC_MASK) == KVM_REG_ARM_VFP)
		return vfp_set_reg(vcpu, reg->id, uaddr);

	r = index_to_coproc_reg(vcpu, reg->id);
	if (!r)
		return set_invariant_cp15(reg->id, uaddr);

	/*                                         */
	return reg_from_user(&vcpu->arch.cp15[r->reg], uaddr, reg->id);
}

static unsigned int num_demux_regs(void)
{
	unsigned int i, count = 0;

	for (i = 0; i < CSSELR_MAX; i++)
		if (is_valid_cache(i))
			count++;

	return count;
}

static int write_demux_regids(u64 __user *uindices)
{
	u64 val = KVM_REG_ARM | KVM_REG_SIZE_U32 | KVM_REG_ARM_DEMUX;
	unsigned int i;

	val |= KVM_REG_ARM_DEMUX_ID_CCSIDR;
	for (i = 0; i < CSSELR_MAX; i++) {
		if (!is_valid_cache(i))
			continue;
		if (put_user(val | i, uindices))
			return -EFAULT;
		uindices++;
	}
	return 0;
}

static u64 cp15_to_index(const struct coproc_reg *reg)
{
	u64 val = KVM_REG_ARM | (15 << KVM_REG_ARM_COPROC_SHIFT);
	if (reg->is_64) {
		val |= KVM_REG_SIZE_U64;
		val |= (reg->Op1 << KVM_REG_ARM_OPC1_SHIFT);
		/*
                                                            
                                                              
                                                           
                                                        
            
   */
		val |= (reg->CRn << KVM_REG_ARM_CRM_SHIFT);
	} else {
		val |= KVM_REG_SIZE_U32;
		val |= (reg->Op1 << KVM_REG_ARM_OPC1_SHIFT);
		val |= (reg->Op2 << KVM_REG_ARM_32_OPC2_SHIFT);
		val |= (reg->CRm << KVM_REG_ARM_CRM_SHIFT);
		val |= (reg->CRn << KVM_REG_ARM_32_CRN_SHIFT);
	}
	return val;
}

static bool copy_reg_to_user(const struct coproc_reg *reg, u64 __user **uind)
{
	if (!*uind)
		return true;

	if (put_user(cp15_to_index(reg), *uind))
		return false;

	(*uind)++;
	return true;
}

/*                                                    */
static int walk_cp15(struct kvm_vcpu *vcpu, u64 __user *uind)
{
	const struct coproc_reg *i1, *i2, *end1, *end2;
	unsigned int total = 0;
	size_t num;

	/*                                                                 */
	i1 = get_target_table(vcpu->arch.target, &num);
	end1 = i1 + num;
	i2 = cp15_regs;
	end2 = cp15_regs + ARRAY_SIZE(cp15_regs);

	BUG_ON(i1 == end1 || i2 == end2);

	/*                                                                */
	while (i1 || i2) {
		int cmp = cmp_reg(i1, i2);
		/*                                          */
		if (cmp <= 0) {
			/*                                          */
			if (i1->reg) {
				if (!copy_reg_to_user(i1, &uind))
					return -EFAULT;
				total++;
			}
		} else {
			/*                                          */
			if (i2->reg) {
				if (!copy_reg_to_user(i2, &uind))
					return -EFAULT;
				total++;
			}
		}

		if (cmp <= 0 && ++i1 == end1)
			i1 = NULL;
		if (cmp >= 0 && ++i2 == end2)
			i2 = NULL;
	}
	return total;
}

unsigned long kvm_arm_num_coproc_regs(struct kvm_vcpu *vcpu)
{
	return ARRAY_SIZE(invariant_cp15)
		+ num_demux_regs()
		+ num_vfp_regs()
		+ walk_cp15(vcpu, (u64 __user *)NULL);
}

int kvm_arm_copy_coproc_indices(struct kvm_vcpu *vcpu, u64 __user *uindices)
{
	unsigned int i;
	int err;

	/*                                                      */
	for (i = 0; i < ARRAY_SIZE(invariant_cp15); i++) {
		if (put_user(cp15_to_index(&invariant_cp15[i]), uindices))
			return -EFAULT;
		uindices++;
	}

	err = walk_cp15(vcpu, uindices);
	if (err < 0)
		return err;
	uindices += err;

	err = copy_vfp_regids(uindices);
	if (err < 0)
		return err;
	uindices += err;

	return write_demux_regids(uindices);
}

void kvm_coproc_table_init(void)
{
	unsigned int i;

	/*                                           */
	for (i = 1; i < ARRAY_SIZE(cp15_regs); i++)
		BUG_ON(cmp_reg(&cp15_regs[i-1], &cp15_regs[i]) >= 0);

	/*                                                            */
	for (i = 0; i < ARRAY_SIZE(invariant_cp15); i++)
		invariant_cp15[i].reset(NULL, &invariant_cp15[i]);

	/*
                                                              
   
                                                         
                                                           
                                                           
                                                             
                                                            
              
  */
	asm volatile("mrc p15, 1, %0, c0, c0, 1" : "=r" (cache_levels));
	for (i = 0; i < 7; i++)
		if (((cache_levels >> (i*3)) & 7) == 0)
			break;
	/*                        */
	cache_levels &= (1 << (i*3))-1;
}

/* 
                                                         
                          
  
                                                                          
                                                                    
 */
void kvm_reset_coprocs(struct kvm_vcpu *vcpu)
{
	size_t num;
	const struct coproc_reg *table;

	/*                                                                 */
	memset(vcpu->arch.cp15, 0x42, sizeof(vcpu->arch.cp15));

	/*                                                      */
	reset_coproc_regs(vcpu, cp15_regs, ARRAY_SIZE(cp15_regs));

	table = get_target_table(vcpu->arch.target, &num);
	reset_coproc_regs(vcpu, table, num);

	for (num = 1; num < NR_CP15_REGS; num++)
		if (vcpu->arch.cp15[num] == 0x42424242)
			panic("Didn't reset vcpu->arch.cp15[%zi]", num);
}
