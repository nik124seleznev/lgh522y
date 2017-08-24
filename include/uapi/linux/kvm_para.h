#ifndef _UAPI__LINUX_KVM_PARA_H
#define _UAPI__LINUX_KVM_PARA_H

/*
                                                                        
                               
                                      
                           
                       
 */

/*                              */
#define KVM_ENOSYS		1000
#define KVM_EFAULT		EFAULT
#define KVM_E2BIG		E2BIG
#define KVM_EPERM		EPERM

#define KVM_HC_VAPIC_POLL_IRQ		1
#define KVM_HC_MMU_OP			2
#define KVM_HC_FEATURES			3
#define KVM_HC_PPC_MAP_MAGIC_PAGE	4

/*
                                       
 */
#include <asm/kvm_para.h>

#endif /*                         */
