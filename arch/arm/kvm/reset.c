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
#include <linux/compiler.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kvm_host.h>
#include <linux/kvm.h>

#include <asm/unified.h>
#include <asm/ptrace.h>
#include <asm/cputype.h>
#include <asm/kvm_arm.h>
#include <asm/kvm_coproc.h>

/*                                                                             
                          
 */

static const int a15_max_cpu_idx = 3;

static struct kvm_regs a15_regs_reset = {
	.usr_regs.ARM_cpsr = SVC_MODE | PSR_A_BIT | PSR_I_BIT | PSR_F_BIT,
};


/*                                                                              
                          
 */

/* 
                                                                         
                          
  
                                                                          
                                                                   
 */
int kvm_reset_vcpu(struct kvm_vcpu *vcpu)
{
	struct kvm_regs *cpu_reset;

	switch (vcpu->arch.target) {
	case KVM_ARM_TARGET_CORTEX_A15:
		if (vcpu->vcpu_id > a15_max_cpu_idx)
			return -EINVAL;
		cpu_reset = &a15_regs_reset;
		vcpu->arch.midr = read_cpuid_id();
		break;
	default:
		return -ENODEV;
	}

	/*                      */
	memcpy(&vcpu->arch.regs, cpu_reset, sizeof(vcpu->arch.regs));

	/*                      */
	kvm_reset_coprocs(vcpu);

	return 0;
}
