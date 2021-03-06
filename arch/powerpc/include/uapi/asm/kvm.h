/*
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
 *
 * Copyright IBM Corp. 2007
 *
 * Authors: Hollis Blanchard <hollisb@us.ibm.com>
 */

#ifndef __LINUX_KVM_POWERPC_H
#define __LINUX_KVM_POWERPC_H

#include <linux/types.h>

/*                                                   */
#define __KVM_HAVE_SPAPR_TCE
#define __KVM_HAVE_PPC_SMT
#define __KVM_HAVE_IRQCHIP
#define __KVM_HAVE_IRQ_LINE

struct kvm_regs {
	__u64 pc;
	__u64 cr;
	__u64 ctr;
	__u64 lr;
	__u64 xer;
	__u64 msr;
	__u64 srr0;
	__u64 srr1;
	__u64 pid;

	__u64 sprg0;
	__u64 sprg1;
	__u64 sprg2;
	__u64 sprg3;
	__u64 sprg4;
	__u64 sprg5;
	__u64 sprg6;
	__u64 sprg7;

	__u64 gpr[32];
};

#define KVM_SREGS_E_IMPL_NONE	0
#define KVM_SREGS_E_IMPL_FSL	1

#define KVM_SREGS_E_FSL_PIDn	(1 << 0) /*           */

/*
                                                                      
                                                                        
                                                                         
                                                                      
                                       
  
                             
                                    
      
       
       
      
      
      
     
                  
 */
#define KVM_SREGS_E_BASE		(1 << 0)

/*
                                
  
      
           
        
       
 */
#define KVM_SREGS_E_ARCH206		(1 << 1)

/*
                                                         
                                             
 */
#define KVM_SREGS_E_64			(1 << 2)

#define KVM_SREGS_E_SPRG8		(1 << 3)
#define KVM_SREGS_E_MCIVPR		(1 << 4)

/*
                                                             
                                                  
 */
#define KVM_SREGS_E_IVOR		(1 << 5)

/*
                                            
                                   
 */
#define KVM_SREGS_E_ARCH206_MMU		(1 << 6)

/*                           */
#define KVM_SREGS_E_DEBUG		(1 << 7)

/*                                  */
#define KVM_SREGS_E_ED			(1 << 8)

/*                                                                */
#define KVM_SREGS_E_SPE			(1 << 9)

/*
                                        
                              
 */
#define KVM_SREGS_EXP			(1 << 10)

/*                                  */
#define KVM_SREGS_E_PD			(1 << 11)

/*                                                           */
#define KVM_SREGS_E_PC			(1 << 12)

/*                             */
#define KVM_SREGS_E_PT			(1 << 13)

/*                                                                   */
#define KVM_SREGS_E_PM			(1 << 14)

/*
                   
  
                                                              
                                                                
                                                                 
                                                                     
  
                                                                 
  
                                                                 
                                                                         
                                      
 */
#define KVM_SREGS_E_UPDATE_MCSR		(1 << 0)
#define KVM_SREGS_E_UPDATE_TSR		(1 << 1)
#define KVM_SREGS_E_UPDATE_DEC		(1 << 2)
#define KVM_SREGS_E_UPDATE_DBSR		(1 << 3)

/*
                                                                      
                         
  
                                                                      
                                                                           
                                                                        
                                                      
 */
struct kvm_sregs {
	__u32 pvr;
	union {
		struct {
			__u64 sdr1;
			struct {
				struct {
					__u64 slbe;
					__u64 slbv;
				} slb[64];
			} ppc64;
			struct {
				__u32 sr[16];
				__u64 ibat[8];
				__u64 dbat[8];
			} ppc32;
		} s;
		struct {
			union {
				struct { /*                      */
					__u32 features; /*                  */
					__u32 svr;
					__u64 mcar;
					__u32 hid0;

					/*                      */
					__u32 pid1, pid2;
				} fsl;
				__u8 pad[256];
			} impl;

			__u32 features; /*              */
			__u32 impl_id;	/*                   */
			__u32 update_special; /*                     */
			__u32 pir;	/*           */
			__u64 sprg8;
			__u64 sprg9;	/*      */
			__u64 csrr0;
			__u64 dsrr0;	/*      */
			__u64 mcsrr0;
			__u32 csrr1;
			__u32 dsrr1;	/*      */
			__u32 mcsrr1;
			__u32 esr;
			__u64 dear;
			__u64 ivpr;
			__u64 mcivpr;
			__u64 mcsr;	/*                         */

			__u32 tsr;	/*                        */
			__u32 tcr;
			__u32 decar;
			__u32 dec;	/*                        */

			/*
                                             
                                                   
     
                
    */
			__u64 tb;

			__u32 dbsr;	/*                         */
			__u32 dbcr[3];
			/*
                                                      
                                                     
                                                      
                        
    */
			__u32 iac[4];
			__u32 dac[2];
			__u32 dvc[2];
			__u8 num_iac;	/*           */
			__u8 num_dac;	/*           */
			__u8 num_dvc;	/*           */
			__u8 pad;

			__u32 epr;	/*     */
			__u32 vrsave;	/*               */
			__u32 epcr;	/*                */

			__u32 mas0;
			__u32 mas1;
			__u64 mas2;
			__u64 mas7_3;
			__u32 mas4;
			__u32 mas6;

			__u32 ivor_low[16]; /*          */
			__u32 ivor_high[18]; /*                              */

			__u32 mmucfg;	/*           */
			__u32 eptcfg;	/*                 */
			__u32 tlbcfg[4];/*           */
			__u32 tlbps[4]; /*           */

			__u32 eplc, epsc; /*      */
		} e;
		__u8 pad[1020];
	} u;
};

struct kvm_fpu {
	__u64 fpr[32];
};

struct kvm_debug_exit_arch {
};

/*                         */
struct kvm_guest_debug_arch {
	struct {
		/*                                   */
		__u64 addr;
		/*
                                                        
                                                    
   */
#define KVMPPC_DEBUG_NONE		0x0
#define KVMPPC_DEBUG_BREAKPOINT		(1UL << 1)
#define KVMPPC_DEBUG_WATCH_WRITE	(1UL << 2)
#define KVMPPC_DEBUG_WATCH_READ		(1UL << 3)
		__u32 type;
		__u32 reserved;
	} bp[16];
};

/*                       */
/*
                                                                            
                                                                             
                                                                        
 */
#define KVM_GUESTDBG_USE_SW_BP		0x00010000
#define KVM_GUESTDBG_USE_HW_BP		0x00020000

/*                                    */
struct kvm_sync_regs {
};

#define KVM_INTERRUPT_SET	-1U
#define KVM_INTERRUPT_UNSET	-2U
#define KVM_INTERRUPT_SET_LEVEL	-3U

#define KVM_CPU_440		1
#define KVM_CPU_E500V2		2
#define KVM_CPU_3S_32		3
#define KVM_CPU_3S_64		4
#define KVM_CPU_E500MC		5

/*                       */
struct kvm_create_spapr_tce {
	__u64 liobn;
	__u32 window_size;
};

/*                      */
struct kvm_allocate_rma {
	__u64 rma_size;
};

/*                      */
struct kvm_rtas_token_args {
	char name[120];
	__u64 token;	/*                                        */
};

struct kvm_book3e_206_tlb_entry {
	__u32 mas8;
	__u32 mas1;
	__u64 mas2;
	__u64 mas7_3;
};

struct kvm_book3e_206_tlb_params {
	/*
                                                                  
   
                                                                     
         
                                     
                                                                    
                                                
                                                 
                                
                                                          
                                                                   
                                   
                                                                  
                                  
   
                                                                    
   
                                                               
                                                                     
                
  */
	__u32 tlb_sizes[4];
	__u32 tlb_ways[4];
	__u32 reserved[8];
};

/*                         */
struct kvm_get_htab_fd {
	__u64	flags;
	__u64	start_index;
	__u64	reserved[2];
};

/*                                  */
#define KVM_GET_HTAB_BOLTED_ONLY	((__u64)0x1)
#define KVM_GET_HTAB_WRITE		((__u64)0x2)

/*
                                                               
                                                               
                                                                   
                                                               
                                                                 
                       
 */
struct kvm_get_htab_header {
	__u32	index;
	__u16	n_valid;
	__u16	n_invalid;
};

/*                                          */
#define KVM_REG_PPC_ICP_STATE	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x8c)

#define  KVM_REG_PPC_ICP_CPPR_SHIFT	56	/*                       */
#define  KVM_REG_PPC_ICP_CPPR_MASK	0xff
#define  KVM_REG_PPC_ICP_XISR_SHIFT	32	/*                        */
#define  KVM_REG_PPC_ICP_XISR_MASK	0xffffff
#define  KVM_REG_PPC_ICP_MFRR_SHIFT	24	/*                      */
#define  KVM_REG_PPC_ICP_MFRR_MASK	0xff
#define  KVM_REG_PPC_ICP_PPRI_SHIFT	16	/*                      */
#define  KVM_REG_PPC_ICP_PPRI_MASK	0xff

/*                                          */
#define KVM_DEV_MPIC_GRP_MISC		1
#define   KVM_DEV_MPIC_BASE_ADDR	0	/*        */

#define KVM_DEV_MPIC_GRP_REGISTER	2	/*        */
#define KVM_DEV_MPIC_GRP_IRQ_ACTIVE	3	/*        */

/*                                     */
#define KVM_REG_PPC_HIOR	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x1)
#define KVM_REG_PPC_IAC1	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x2)
#define KVM_REG_PPC_IAC2	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x3)
#define KVM_REG_PPC_IAC3	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x4)
#define KVM_REG_PPC_IAC4	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x5)
#define KVM_REG_PPC_DAC1	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x6)
#define KVM_REG_PPC_DAC2	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x7)
#define KVM_REG_PPC_DABR	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x8)
#define KVM_REG_PPC_DSCR	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x9)
#define KVM_REG_PPC_PURR	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0xa)
#define KVM_REG_PPC_SPURR	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0xb)
#define KVM_REG_PPC_DAR		(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0xc)
#define KVM_REG_PPC_DSISR	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0xd)
#define KVM_REG_PPC_AMR		(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0xe)
#define KVM_REG_PPC_UAMOR	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0xf)

#define KVM_REG_PPC_MMCR0	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x10)
#define KVM_REG_PPC_MMCR1	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x11)
#define KVM_REG_PPC_MMCRA	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x12)

#define KVM_REG_PPC_PMC1	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x18)
#define KVM_REG_PPC_PMC2	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x19)
#define KVM_REG_PPC_PMC3	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x1a)
#define KVM_REG_PPC_PMC4	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x1b)
#define KVM_REG_PPC_PMC5	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x1c)
#define KVM_REG_PPC_PMC6	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x1d)
#define KVM_REG_PPC_PMC7	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x1e)
#define KVM_REG_PPC_PMC8	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x1f)

/*                             */
#define KVM_REG_PPC_FPR0	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x20)
#define KVM_REG_PPC_FPR(n)	(KVM_REG_PPC_FPR0 + (n))
#define KVM_REG_PPC_FPR31	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x3f)

/*                                 */
#define KVM_REG_PPC_VR0		(KVM_REG_PPC | KVM_REG_SIZE_U128 | 0x40)
#define KVM_REG_PPC_VR(n)	(KVM_REG_PPC_VR0 + (n))
#define KVM_REG_PPC_VR31	(KVM_REG_PPC | KVM_REG_SIZE_U128 | 0x5f)

/*                                      */
/*                                        */
#define KVM_REG_PPC_VSR0	(KVM_REG_PPC | KVM_REG_SIZE_U128 | 0x60)
#define KVM_REG_PPC_VSR(n)	(KVM_REG_PPC_VSR0 + (n))
#define KVM_REG_PPC_VSR31	(KVM_REG_PPC | KVM_REG_SIZE_U128 | 0x7f)

/*                                        */
#define KVM_REG_PPC_FPSCR	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x80)
#define KVM_REG_PPC_VSCR	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x81)

/*                         */
/*                                                                 */
#define KVM_REG_PPC_VPA_ADDR	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x82)
#define KVM_REG_PPC_VPA_SLB	(KVM_REG_PPC | KVM_REG_SIZE_U128 | 0x83)
#define KVM_REG_PPC_VPA_DTL	(KVM_REG_PPC | KVM_REG_SIZE_U128 | 0x84)

#define KVM_REG_PPC_EPCR	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x85)
#define KVM_REG_PPC_EPR		(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x86)

/*                                          */
#define KVM_REG_PPC_OR_TSR	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x87)
#define KVM_REG_PPC_CLEAR_TSR	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x88)
#define KVM_REG_PPC_TCR		(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x89)
#define KVM_REG_PPC_TSR		(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x8a)

/*                                                        */
#define KVM_REG_PPC_DEBUG_INST	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x8b)

/*               */
#define KVM_REG_PPC_MAS0	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x8c)
#define KVM_REG_PPC_MAS1	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x8d)
#define KVM_REG_PPC_MAS2	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x8e)
#define KVM_REG_PPC_MAS7_3	(KVM_REG_PPC | KVM_REG_SIZE_U64 | 0x8f)
#define KVM_REG_PPC_MAS4	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x90)
#define KVM_REG_PPC_MAS6	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x91)
#define KVM_REG_PPC_MMUCFG	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x92)
/*
                                                                             
                       
 */
#define KVM_REG_PPC_TLB0CFG	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x93)
#define KVM_REG_PPC_TLB1CFG	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x94)
#define KVM_REG_PPC_TLB2CFG	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x95)
#define KVM_REG_PPC_TLB3CFG	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x96)
#define KVM_REG_PPC_TLB0PS	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x97)
#define KVM_REG_PPC_TLB1PS	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x98)
#define KVM_REG_PPC_TLB2PS	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x99)
#define KVM_REG_PPC_TLB3PS	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x9a)
#define KVM_REG_PPC_EPTCFG	(KVM_REG_PPC | KVM_REG_SIZE_U32 | 0x9b)

/*                                                   */
#define KVM_DEV_XICS_GRP_SOURCES	1	/*                          */

/*                                          */
#define  KVM_XICS_DESTINATION_SHIFT	0
#define  KVM_XICS_DESTINATION_MASK	0xffffffffULL
#define  KVM_XICS_PRIORITY_SHIFT	32
#define  KVM_XICS_PRIORITY_MASK		0xff
#define  KVM_XICS_LEVEL_SENSITIVE	(1ULL << 40)
#define  KVM_XICS_MASKED		(1ULL << 41)
#define  KVM_XICS_PENDING		(1ULL << 42)

#endif /*                       */
