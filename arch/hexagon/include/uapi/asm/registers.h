/*
                                                    
 */


#ifndef _ASM_REGISTERS_H
#define _ASM_REGISTERS_H

#define SP r29

#ifndef __ASSEMBLY__

/*                                                 */

/*
                                                                 
                                          
 */

struct hvm_event_record {
	unsigned long vmel;     /*                                */
	unsigned long vmest;    /*                                      */
	unsigned long vmpsp;    /*                        */
	unsigned long vmbadva;  /*                                           */
};

struct pt_regs {
	long restart_r0;        /*                                   */
	long syscall_nr;        /*                           */
	union {
		struct {
			unsigned long usr;
			unsigned long preds;
		};
		long long int predsusr;
	};
	union {
		struct {
			unsigned long m0;
			unsigned long m1;
		};
		long long int m1m0;
	};
	union {
		struct {
			unsigned long sa1;
			unsigned long lc1;
		};
		long long int lc1sa1;
	};
	union {
		struct {
			unsigned long sa0;
			unsigned long lc0;
		};
		long long int lc0sa0;
	};
	union {
		struct {
			unsigned long ugp;
			unsigned long gp;
		};
		long long int gpugp;
	};
	union {
		struct {
			unsigned long cs0;
			unsigned long cs1;
		};
		long long int cs1cs0;
	};
	/*
                                                                     
                                                              
                       
                                            
 */
	union {
		struct {
			unsigned long r00;
			unsigned long r01;
		};
		long long int r0100;
	};
	union {
		struct {
			unsigned long r02;
			unsigned long r03;
		};
		long long int r0302;
	};
	union {
		struct {
			unsigned long r04;
			unsigned long r05;
		};
		long long int r0504;
	};
	union {
		struct {
			unsigned long r06;
			unsigned long r07;
		};
		long long int r0706;
	};
	union {
		struct {
			unsigned long r08;
			unsigned long r09;
		};
		long long int r0908;
	};
	union {
	       struct {
			unsigned long r10;
			unsigned long r11;
	       };
	       long long int r1110;
	};
	union {
	       struct {
			unsigned long r12;
			unsigned long r13;
	       };
	       long long int r1312;
	};
	union {
	       struct {
			unsigned long r14;
			unsigned long r15;
	       };
	       long long int r1514;
	};
	union {
		struct {
			unsigned long r16;
			unsigned long r17;
		};
		long long int r1716;
	};
	union {
		struct {
			unsigned long r18;
			unsigned long r19;
		};
		long long int r1918;
	};
	union {
		struct {
			unsigned long r20;
			unsigned long r21;
		};
		long long int r2120;
	};
	union {
		struct {
			unsigned long r22;
			unsigned long r23;
		};
		long long int r2322;
	};
	union {
		struct {
			unsigned long r24;
			unsigned long r25;
		};
		long long int r2524;
	};
	union {
		struct {
			unsigned long r26;
			unsigned long r27;
		};
		long long int r2726;
	};
	union {
		struct {
			unsigned long r28;
			unsigned long r29;
	       };
	       long long int r2928;
	};
	union {
		struct {
			unsigned long r30;
			unsigned long r31;
		};
		long long int r3130;
	};
	/*                                                                 */
	struct hvm_event_record hvmer;
};

/*                                            */

/*
                                                                     
                                                           
                                                                     
                                                                
                  
 */

#define pt_elr(regs) ((regs)->hvmer.vmel)
#define pt_set_elr(regs, val) ((regs)->hvmer.vmel = (val))
#define pt_cause(regs) ((regs)->hvmer.vmest & (HVM_VMEST_CAUSE_MSK))
#define user_mode(regs) \
	(((regs)->hvmer.vmest & (HVM_VMEST_UM_MSK << HVM_VMEST_UM_SFT)) != 0)
#define ints_enabled(regs) \
	(((regs)->hvmer.vmest & (HVM_VMEST_IE_MSK << HVM_VMEST_IE_SFT)) != 0)
#define pt_psp(regs) ((regs)->hvmer.vmpsp)
#define pt_badva(regs) ((regs)->hvmer.vmbadva)

#define pt_set_singlestep(regs) ((regs)->hvmer.vmest |= (1<<HVM_VMEST_SS_SFT))
#define pt_clr_singlestep(regs) ((regs)->hvmer.vmest &= ~(1<<HVM_VMEST_SS_SFT))

#define pt_set_rte_sp(regs, sp) do {\
	pt_psp(regs) = (regs)->SP = (sp);\
	} while (0)

#define pt_set_kmode(regs) \
	(regs)->hvmer.vmest = (HVM_VMEST_IE_MSK << HVM_VMEST_IE_SFT)

#define pt_set_usermode(regs) \
	(regs)->hvmer.vmest = (HVM_VMEST_UM_MSK << HVM_VMEST_UM_SFT) \
			    | (HVM_VMEST_IE_MSK << HVM_VMEST_IE_SFT)

#endif  /*                     */

#endif
