/*
 * Copyright 2012 Michael Ellerman, IBM Corporation.
 * Copyright 2012 Benjamin Herrenschmidt, IBM Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2, as
 * published by the Free Software Foundation.
 */

#ifndef _KVM_PPC_BOOK3S_XICS_H
#define _KVM_PPC_BOOK3S_XICS_H

/*
                                                                 
                                                              
                
 */
#define KVMPPC_XICS_MAX_ICS_ID	1023
#define KVMPPC_XICS_ICS_SHIFT	10
#define KVMPPC_XICS_IRQ_PER_ICS	(1 << KVMPPC_XICS_ICS_SHIFT)
#define KVMPPC_XICS_SRC_MASK	(KVMPPC_XICS_IRQ_PER_ICS - 1)

/*
                                                                
                                               
 */
#define KVMPPC_XICS_FIRST_IRQ	16
#define KVMPPC_XICS_NR_IRQS	((KVMPPC_XICS_MAX_ICS_ID + 1) * \
				 KVMPPC_XICS_IRQ_PER_ICS)

/*                                                  */
#define MASKED	0xff

/*                          */
struct ics_irq_state {
	u32 number;
	u32 server;
	u8  priority;
	u8  saved_priority;
	u8  resend;
	u8  masked_pending;
	u8  asserted; /*              */
	u8  exists;
};

/*                                                        */
union kvmppc_icp_state {
	unsigned long raw;
	struct {
		u8 out_ee:1;
		u8 need_resend:1;
		u8 cppr;
		u8 mfrr;
		u8 pending_pri;
		u32 xisr;
	};
};

/*                 */
#define ICP_RESEND_MAP_SIZE	(KVMPPC_XICS_MAX_ICS_ID / BITS_PER_LONG + 1)

struct kvmppc_icp {
	struct kvm_vcpu *vcpu;
	unsigned long server_num;
	union kvmppc_icp_state state;
	unsigned long resend_map[ICP_RESEND_MAP_SIZE];

	/*                                                           
                                      
  */
#define XICS_RM_KICK_VCPU	0x1
#define XICS_RM_CHECK_RESEND	0x2
#define XICS_RM_REJECT		0x4
	u32 rm_action;
	struct kvm_vcpu *rm_kick_target;
	u32  rm_reject;

	/*                           */
	union kvmppc_icp_state rm_dbgstate;
	struct kvm_vcpu *rm_dbgtgt;
};

struct kvmppc_ics {
	struct mutex lock;
	u16 icsid;
	struct ics_irq_state irq_state[KVMPPC_XICS_IRQ_PER_ICS];
};

struct kvmppc_xics {
	struct kvm *kvm;
	struct kvm_device *dev;
	struct dentry *dentry;
	u32 max_icsid;
	bool real_mode;
	bool real_mode_dbg;
	struct kvmppc_ics *ics[KVMPPC_XICS_MAX_ICS_ID + 1];
};

static inline struct kvmppc_icp *kvmppc_xics_find_server(struct kvm *kvm,
							 u32 nr)
{
	struct kvm_vcpu *vcpu = NULL;
	int i;

	kvm_for_each_vcpu(i, vcpu, kvm) {
		if (vcpu->arch.icp && nr == vcpu->arch.icp->server_num)
			return vcpu->arch.icp;
	}
	return NULL;
}

static inline struct kvmppc_ics *kvmppc_xics_find_ics(struct kvmppc_xics *xics,
						      u32 irq, u16 *source)
{
	u32 icsid = irq >> KVMPPC_XICS_ICS_SHIFT;
	u16 src = irq & KVMPPC_XICS_SRC_MASK;
	struct kvmppc_ics *ics;

	if (source)
		*source = src;
	if (icsid > KVMPPC_XICS_MAX_ICS_ID)
		return NULL;
	ics = xics->ics[icsid];
	if (!ics)
		return NULL;
	return ics;
}


#endif /*                        */
