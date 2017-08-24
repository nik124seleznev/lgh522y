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

#ifndef __ARM_KVM_ASM_H__
#define __ARM_KVM_ASM_H__

/*                                    */
#define c0_MPIDR	1	/*                            */
#define c0_CSSELR	2	/*                               */
#define c1_SCTLR	3	/*                         */
#define c1_ACTLR	4	/*                             */
#define c1_CPACR	5	/*                            */
#define c2_TTBR0	6	/*                                   */
#define c2_TTBR0_high	7	/*                   */
#define c2_TTBR1	8	/*                                   */
#define c2_TTBR1_high	9	/*                   */
#define c2_TTBCR	10	/*                                   */
#define c3_DACR		11	/*                                */
#define c5_DFSR		12	/*                            */
#define c5_IFSR		13	/*                                   */
#define c5_ADFSR	14	/*                              */
#define c5_AIFSR	15	/*                                      */
#define c6_DFAR		16	/*                             */
#define c6_IFAR		17	/*                                    */
#define c7_PAR		18	/*                           */
#define c7_PAR_high	19	/*                 */
#define c9_L2CTLR	20	/*                                */
#define c10_PRRR	21	/*                               */
#define c10_NMRR	22	/*                              */
#define c12_VBAR	23	/*                              */
#define c13_CID		24	/*                     */
#define c13_TID_URW	25	/*                     */
#define c13_TID_URO	26	/*                     */
#define c13_TID_PRIV	27	/*                       */
#define c14_CNTKCTL	28	/*                              */
#define NR_CP15_REGS	29	/*                                */

#define ARM_EXCEPTION_RESET	  0
#define ARM_EXCEPTION_UNDEFINED   1
#define ARM_EXCEPTION_SOFTWARE    2
#define ARM_EXCEPTION_PREF_ABORT  3
#define ARM_EXCEPTION_DATA_ABORT  4
#define ARM_EXCEPTION_IRQ	  5
#define ARM_EXCEPTION_FIQ	  6
#define ARM_EXCEPTION_HVC	  7

#ifndef __ASSEMBLY__
struct kvm;
struct kvm_vcpu;

extern char __kvm_hyp_init[];
extern char __kvm_hyp_init_end[];

extern char __kvm_hyp_exit[];
extern char __kvm_hyp_exit_end[];

extern char __kvm_hyp_vector[];

extern char __kvm_hyp_code_start[];
extern char __kvm_hyp_code_end[];

extern void __kvm_tlb_flush_vmid(struct kvm *kvm);

extern void __kvm_flush_vm_context(void);
extern void __kvm_tlb_flush_vmid_ipa(struct kvm *kvm, phys_addr_t ipa);

extern int __kvm_vcpu_run(struct kvm_vcpu *vcpu);
#endif

#endif /*                   */
