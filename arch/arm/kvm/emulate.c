/*
 * Copyright (C) 2012 - Virtual Open Systems and Columbia University
 * Author: Christoffer Dall <c.dall@virtualopensystems.com>
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
#include <asm/kvm_arm.h>
#include <asm/kvm_emulate.h>
#include <asm/opcodes.h>
#include <trace/events/kvm.h>

#include "trace.h"

#define VCPU_NR_MODES		6
#define VCPU_REG_OFFSET_USR	0
#define VCPU_REG_OFFSET_FIQ	1
#define VCPU_REG_OFFSET_IRQ	2
#define VCPU_REG_OFFSET_SVC	3
#define VCPU_REG_OFFSET_ABT	4
#define VCPU_REG_OFFSET_UND	5
#define REG_OFFSET(_reg) \
	(offsetof(struct kvm_regs, _reg) / sizeof(u32))

#define USR_REG_OFFSET(_num) REG_OFFSET(usr_regs.uregs[_num])

static const unsigned long vcpu_reg_offsets[VCPU_NR_MODES][15] = {
	/*                   */
	[VCPU_REG_OFFSET_USR] = {
		USR_REG_OFFSET(0), USR_REG_OFFSET(1), USR_REG_OFFSET(2),
		USR_REG_OFFSET(3), USR_REG_OFFSET(4), USR_REG_OFFSET(5),
		USR_REG_OFFSET(6), USR_REG_OFFSET(7), USR_REG_OFFSET(8),
		USR_REG_OFFSET(9), USR_REG_OFFSET(10), USR_REG_OFFSET(11),
		USR_REG_OFFSET(12), USR_REG_OFFSET(13),	USR_REG_OFFSET(14),
	},

	/*               */
	[VCPU_REG_OFFSET_FIQ] = {
		USR_REG_OFFSET(0), USR_REG_OFFSET(1), USR_REG_OFFSET(2),
		USR_REG_OFFSET(3), USR_REG_OFFSET(4), USR_REG_OFFSET(5),
		USR_REG_OFFSET(6), USR_REG_OFFSET(7),
		REG_OFFSET(fiq_regs[0]), /*    */
		REG_OFFSET(fiq_regs[1]), /*    */
		REG_OFFSET(fiq_regs[2]), /*     */
		REG_OFFSET(fiq_regs[3]), /*     */
		REG_OFFSET(fiq_regs[4]), /*     */
		REG_OFFSET(fiq_regs[5]), /*     */
		REG_OFFSET(fiq_regs[6]), /*     */
	},

	/*               */
	[VCPU_REG_OFFSET_IRQ] = {
		USR_REG_OFFSET(0), USR_REG_OFFSET(1), USR_REG_OFFSET(2),
		USR_REG_OFFSET(3), USR_REG_OFFSET(4), USR_REG_OFFSET(5),
		USR_REG_OFFSET(6), USR_REG_OFFSET(7), USR_REG_OFFSET(8),
		USR_REG_OFFSET(9), USR_REG_OFFSET(10), USR_REG_OFFSET(11),
		USR_REG_OFFSET(12),
		REG_OFFSET(irq_regs[0]), /*     */
		REG_OFFSET(irq_regs[1]), /*     */
	},

	/*               */
	[VCPU_REG_OFFSET_SVC] = {
		USR_REG_OFFSET(0), USR_REG_OFFSET(1), USR_REG_OFFSET(2),
		USR_REG_OFFSET(3), USR_REG_OFFSET(4), USR_REG_OFFSET(5),
		USR_REG_OFFSET(6), USR_REG_OFFSET(7), USR_REG_OFFSET(8),
		USR_REG_OFFSET(9), USR_REG_OFFSET(10), USR_REG_OFFSET(11),
		USR_REG_OFFSET(12),
		REG_OFFSET(svc_regs[0]), /*     */
		REG_OFFSET(svc_regs[1]), /*     */
	},

	/*               */
	[VCPU_REG_OFFSET_ABT] = {
		USR_REG_OFFSET(0), USR_REG_OFFSET(1), USR_REG_OFFSET(2),
		USR_REG_OFFSET(3), USR_REG_OFFSET(4), USR_REG_OFFSET(5),
		USR_REG_OFFSET(6), USR_REG_OFFSET(7), USR_REG_OFFSET(8),
		USR_REG_OFFSET(9), USR_REG_OFFSET(10), USR_REG_OFFSET(11),
		USR_REG_OFFSET(12),
		REG_OFFSET(abt_regs[0]), /*     */
		REG_OFFSET(abt_regs[1]), /*     */
	},

	/*               */
	[VCPU_REG_OFFSET_UND] = {
		USR_REG_OFFSET(0), USR_REG_OFFSET(1), USR_REG_OFFSET(2),
		USR_REG_OFFSET(3), USR_REG_OFFSET(4), USR_REG_OFFSET(5),
		USR_REG_OFFSET(6), USR_REG_OFFSET(7), USR_REG_OFFSET(8),
		USR_REG_OFFSET(9), USR_REG_OFFSET(10), USR_REG_OFFSET(11),
		USR_REG_OFFSET(12),
		REG_OFFSET(und_regs[0]), /*     */
		REG_OFFSET(und_regs[1]), /*     */
	},
};

/*
                                                                       
                   
 */
unsigned long *vcpu_reg(struct kvm_vcpu *vcpu, u8 reg_num)
{
	unsigned long *reg_array = (unsigned long *)&vcpu->arch.regs;
	unsigned long mode = *vcpu_cpsr(vcpu) & MODE_MASK;

	switch (mode) {
	case USR_MODE...SVC_MODE:
		mode &= ~MODE32_BIT; /*         */
		break;

	case ABT_MODE:
		mode = VCPU_REG_OFFSET_ABT;
		break;

	case UND_MODE:
		mode = VCPU_REG_OFFSET_UND;
		break;

	case SYSTEM_MODE:
		mode = VCPU_REG_OFFSET_USR;
		break;

	default:
		BUG();
	}

	return reg_array + vcpu_reg_offsets[mode][reg_num];
}

/*
                                                           
 */
unsigned long *vcpu_spsr(struct kvm_vcpu *vcpu)
{
	unsigned long mode = *vcpu_cpsr(vcpu) & MODE_MASK;
	switch (mode) {
	case SVC_MODE:
		return &vcpu->arch.regs.KVM_ARM_SVC_spsr;
	case ABT_MODE:
		return &vcpu->arch.regs.KVM_ARM_ABT_spsr;
	case UND_MODE:
		return &vcpu->arch.regs.KVM_ARM_UND_spsr;
	case IRQ_MODE:
		return &vcpu->arch.regs.KVM_ARM_IRQ_spsr;
	case FIQ_MODE:
		return &vcpu->arch.regs.KVM_ARM_FIQ_spsr;
	default:
		BUG();
	}
}

/*
                                                               
                                                                
            
 */
bool kvm_condition_valid(struct kvm_vcpu *vcpu)
{
	unsigned long cpsr, cond, insn;

	/*
                                                               
                                                            
                                      
  */
	BUG_ON(!kvm_vcpu_trap_get_class(vcpu));

	/*                                        */
	if (kvm_vcpu_get_hsr(vcpu) >> 30)
		return true;

	cpsr = *vcpu_cpsr(vcpu);

	/*                           */
	if ((kvm_vcpu_get_hsr(vcpu) & HSR_CV) >> HSR_CV_SHIFT)
		cond = (kvm_vcpu_get_hsr(vcpu) & HSR_COND) >> HSR_COND_SHIFT;
	else {
		/*                                                  */
		unsigned long it;

		it = ((cpsr >> 8) & 0xFC) | ((cpsr >> 25) & 0x3);

		/*                           */
		if (it == 0)
			return true;

		/*                                                     */
		cond = (it >> 4);
	}

	/*                                                  */
	insn = cond << 28;
	return arm_check_condition(insn, cpsr) != ARM_OPCODE_CONDTEST_FAIL;
}

/* 
                                                                          
                          
  
                                                                         
                                                                              
                                                                    
  
                                     
 */
static void kvm_adjust_itstate(struct kvm_vcpu *vcpu)
{
	unsigned long itbits, cond;
	unsigned long cpsr = *vcpu_cpsr(vcpu);
	bool is_arm = !(cpsr & PSR_T_BIT);

	BUG_ON(is_arm && (cpsr & PSR_IT_MASK));

	if (!(cpsr & PSR_IT_MASK))
		return;

	cond = (cpsr & 0xe000) >> 13;
	itbits = (cpsr & 0x1c00) >> (10 - 2);
	itbits |= (cpsr & (0x3 << 25)) >> 25;

	/*                                                    */
	if ((itbits & 0x7) == 0)
		itbits = cond = 0;
	else
		itbits = (itbits << 1) & 0x1f;

	cpsr &= ~PSR_IT_MASK;
	cpsr |= cond << 13;
	cpsr |= (itbits & 0x1c) << (10 - 2);
	cpsr |= (itbits & 0x3) << 25;
	*vcpu_cpsr(vcpu) = cpsr;
}

/* 
                                                                      
                          
 */
void kvm_skip_instr(struct kvm_vcpu *vcpu, bool is_wide_instr)
{
	bool is_thumb;

	is_thumb = !!(*vcpu_cpsr(vcpu) & PSR_T_BIT);
	if (is_thumb && !is_wide_instr)
		*vcpu_pc(vcpu) += 2;
	else
		*vcpu_pc(vcpu) += 4;
	kvm_adjust_itstate(vcpu);
}


/*                                                                             
                                   
 */

static u32 exc_vector_base(struct kvm_vcpu *vcpu)
{
	u32 sctlr = vcpu->arch.cp15[c1_SCTLR];
	u32 vbar = vcpu->arch.cp15[c12_VBAR];

	if (sctlr & SCTLR_V)
		return 0xffff0000;
	else /*                                 */
		return vbar;
}

/* 
                                                                      
                                                     
  
                                                                           
                                                        
  
                                                       
 */
void kvm_inject_undefined(struct kvm_vcpu *vcpu)
{
	unsigned long new_lr_value;
	unsigned long new_spsr_value;
	unsigned long cpsr = *vcpu_cpsr(vcpu);
	u32 sctlr = vcpu->arch.cp15[c1_SCTLR];
	bool is_thumb = (cpsr & PSR_T_BIT);
	u32 vect_offset = 4;
	u32 return_offset = (is_thumb) ? 2 : 4;

	new_spsr_value = cpsr;
	new_lr_value = *vcpu_pc(vcpu) - return_offset;

	*vcpu_cpsr(vcpu) = (cpsr & ~MODE_MASK) | UND_MODE;
	*vcpu_cpsr(vcpu) |= PSR_I_BIT;
	*vcpu_cpsr(vcpu) &= ~(PSR_IT_MASK | PSR_J_BIT | PSR_E_BIT | PSR_T_BIT);

	if (sctlr & SCTLR_TE)
		*vcpu_cpsr(vcpu) |= PSR_T_BIT;
	if (sctlr & SCTLR_EE)
		*vcpu_cpsr(vcpu) |= PSR_E_BIT;

	/*                                            */
	*vcpu_spsr(vcpu) = cpsr;
	*vcpu_reg(vcpu, 14) = new_lr_value;

	/*                            */
	*vcpu_pc(vcpu) = exc_vector_base(vcpu) + vect_offset;
}

/*
                                                                         
              
 */
static void inject_abt(struct kvm_vcpu *vcpu, bool is_pabt, unsigned long addr)
{
	unsigned long new_lr_value;
	unsigned long new_spsr_value;
	unsigned long cpsr = *vcpu_cpsr(vcpu);
	u32 sctlr = vcpu->arch.cp15[c1_SCTLR];
	bool is_thumb = (cpsr & PSR_T_BIT);
	u32 vect_offset;
	u32 return_offset = (is_thumb) ? 4 : 0;
	bool is_lpae;

	new_spsr_value = cpsr;
	new_lr_value = *vcpu_pc(vcpu) + return_offset;

	*vcpu_cpsr(vcpu) = (cpsr & ~MODE_MASK) | ABT_MODE;
	*vcpu_cpsr(vcpu) |= PSR_I_BIT | PSR_A_BIT;
	*vcpu_cpsr(vcpu) &= ~(PSR_IT_MASK | PSR_J_BIT | PSR_E_BIT | PSR_T_BIT);

	if (sctlr & SCTLR_TE)
		*vcpu_cpsr(vcpu) |= PSR_T_BIT;
	if (sctlr & SCTLR_EE)
		*vcpu_cpsr(vcpu) |= PSR_E_BIT;

	/*                                            */
	*vcpu_spsr(vcpu) = cpsr;
	*vcpu_reg(vcpu, 14) = new_lr_value;

	if (is_pabt)
		vect_offset = 12;
	else
		vect_offset = 16;

	/*                            */
	*vcpu_pc(vcpu) = exc_vector_base(vcpu) + vect_offset;

	if (is_pabt) {
		/*                   */
		vcpu->arch.cp15[c6_IFAR] = addr;
		is_lpae = (vcpu->arch.cp15[c2_TTBCR] >> 31);
		/*                                                            */
		if (is_lpae)
			vcpu->arch.cp15[c5_IFSR] = 1 << 9 | 0x22;
		else
			vcpu->arch.cp15[c5_IFSR] = 2;
	} else { /*       */
		/*                   */
		vcpu->arch.cp15[c6_DFAR] = addr;
		is_lpae = (vcpu->arch.cp15[c2_TTBCR] >> 31);
		/*                                                            */
		if (is_lpae)
			vcpu->arch.cp15[c5_DFSR] = 1 << 9 | 0x22;
		else
			vcpu->arch.cp15[c5_DFSR] = 2;
	}

}

/* 
                                                       
                                                     
                                           
  
                                                                           
                                                        
 */
void kvm_inject_dabt(struct kvm_vcpu *vcpu, unsigned long addr)
{
	inject_abt(vcpu, false, addr);
}

/* 
                                                           
                                                     
                                           
  
                                                                           
                                                        
 */
void kvm_inject_pabt(struct kvm_vcpu *vcpu, unsigned long addr)
{
	inject_abt(vcpu, true, addr);
}
