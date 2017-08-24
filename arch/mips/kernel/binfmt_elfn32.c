/*
 * Support for n32 Linux/MIPS ELF binaries.
 *
 * Copyright (C) 1999, 2001 Ralf Baechle
 * Copyright (C) 1999, 2001 Silicon Graphics, Inc.
 *
 * Heavily inspired by the 32-bit Sparc compat code which is
 * Copyright (C) 1995, 1996, 1997, 1998 David S. Miller (davem@redhat.com)
 * Copyright (C) 1995, 1996, 1997, 1998 Jakub Jelinek	(jj@ultra.linux.cz)
 */

#define ELF_ARCH		EM_MIPS
#define ELF_CLASS		ELFCLASS32
#ifdef __MIPSEB__
#define ELF_DATA		ELFDATA2MSB;
#else /*            */
#define ELF_DATA		ELFDATA2LSB;
#endif

/*                          */
#define ELF_NGREG	45
#define ELF_NFPREG	33

typedef unsigned long elf_greg_t;
typedef elf_greg_t elf_gregset_t[ELF_NGREG];

typedef double elf_fpreg_t;
typedef elf_fpreg_t elf_fpregset_t[ELF_NFPREG];

/*
                                                                             
 */
#define elf_check_arch(hdr)						\
({									\
	int __res = 1;							\
	struct elfhdr *__h = (hdr);					\
									\
	if (__h->e_machine != EM_MIPS)					\
		__res = 0;						\
	if (__h->e_ident[EI_CLASS] != ELFCLASS32)			\
		__res = 0;						\
	if (((__h->e_flags & EF_MIPS_ABI2) == 0) ||			\
	    ((__h->e_flags & EF_MIPS_ABI) != 0))			\
		__res = 0;						\
									\
	__res;								\
})

#define TASK32_SIZE		0x7fff8000UL
#undef ELF_ET_DYN_BASE
#define ELF_ET_DYN_BASE		(TASK32_SIZE / 3 * 2)

#include <asm/processor.h>
#include <linux/module.h>
#include <linux/elfcore.h>
#include <linux/compat.h>
#include <linux/math64.h>

#define elf_prstatus elf_prstatus32
struct elf_prstatus32
{
	struct elf_siginfo pr_info;	/*                             */
	short	pr_cursig;		/*                */
	unsigned int pr_sigpend;	/*                        */
	unsigned int pr_sighold;	/*                     */
	pid_t	pr_pid;
	pid_t	pr_ppid;
	pid_t	pr_pgrp;
	pid_t	pr_sid;
	struct compat_timeval pr_utime; /*           */
	struct compat_timeval pr_stime; /*             */
	struct compat_timeval pr_cutime;/*                      */
	struct compat_timeval pr_cstime;/*                        */
	elf_gregset_t pr_reg;	/*              */
	int pr_fpvalid;		/*                                        */
};

#define elf_prpsinfo elf_prpsinfo32
struct elf_prpsinfo32
{
	char	pr_state;	/*                       */
	char	pr_sname;	/*                   */
	char	pr_zomb;	/*        */
	char	pr_nice;	/*          */
	unsigned int pr_flag;	/*       */
	__kernel_uid_t	pr_uid;
	__kernel_gid_t	pr_gid;
	pid_t	pr_pid, pr_ppid, pr_pgrp, pr_sid;
	/*              */
	char	pr_fname[16];	/*                        */
	char	pr_psargs[ELF_PRARGSZ]; /*                          */
};

#define elf_caddr_t	u32
#define init_elf_binfmt init_elfn32_binfmt

#define jiffies_to_timeval jiffies_to_compat_timeval
static __inline__ void
jiffies_to_compat_timeval(unsigned long jiffies, struct compat_timeval *value)
{
	/*
                                                    
               
  */
	u64 nsec = (u64)jiffies * TICK_NSEC;
	u32 rem;
	value->tv_sec = div_u64_rem(nsec, NSEC_PER_SEC, &rem);
	value->tv_usec = rem / NSEC_PER_USEC;
}

#define ELF_CORE_EFLAGS EF_MIPS_ABI2

MODULE_DESCRIPTION("Binary format loader for compatibility with n32 Linux/MIPS binaries");
MODULE_AUTHOR("Ralf Baechle (ralf@linux-mips.org)");

#undef MODULE_DESCRIPTION
#undef MODULE_AUTHOR

#undef TASK_SIZE
#define TASK_SIZE TASK_SIZE32

#undef cputime_to_timeval
#define cputime_to_timeval cputime_to_compat_timeval
static __inline__ void
cputime_to_compat_timeval(const cputime_t cputime, struct compat_timeval *value)
{
	unsigned long jiffies = cputime_to_jiffies(cputime);

	value->tv_usec = (jiffies % HZ) * (1000000L / HZ);
	value->tv_sec = jiffies / HZ;
}

#include "../../../fs/binfmt_elf.c"