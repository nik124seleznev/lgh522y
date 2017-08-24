/*
 * Copyright (C) 2001-2003 Hewlett-Packard Co
 *               Stephane Eranian <eranian@hpl.hp.com>
 */
#ifndef _ASM_IA64_PERFMON_H
#define _ASM_IA64_PERFMON_H

#include <uapi/asm/perfmon.h>


extern long perfmonctl(int fd, int cmd, void *arg, int narg);

typedef struct {
	void (*handler)(int irq, void *arg, struct pt_regs *regs);
} pfm_intr_handler_desc_t;

extern void pfm_save_regs (struct task_struct *);
extern void pfm_load_regs (struct task_struct *);

extern void pfm_exit_thread(struct task_struct *);
extern int  pfm_use_debug_registers(struct task_struct *);
extern int  pfm_release_debug_registers(struct task_struct *);
extern void pfm_syst_wide_update_task(struct task_struct *, unsigned long info, int is_ctxswin);
extern void pfm_inherit(struct task_struct *task, struct pt_regs *regs);
extern void pfm_init_percpu(void);
extern void pfm_handle_work(void);
extern int  pfm_install_alt_pmu_interrupt(pfm_intr_handler_desc_t *h);
extern int  pfm_remove_alt_pmu_interrupt(pfm_intr_handler_desc_t *h);



/*
                           
 */
#define PFM_PMD_SHORT_RESET	0
#define PFM_PMD_LONG_RESET	1

typedef union {
	unsigned int val;
	struct {
		unsigned int notify_user:1;	/*                                 */
		unsigned int reset_ovfl_pmds:1;	/*                       */
		unsigned int block_task:1;	/*                                     */
		unsigned int mask_monitoring:1; /*                            */
		unsigned int reserved:28;	/*                */
	} bits;
} pfm_ovfl_ctrl_t;

typedef struct {
	unsigned char	ovfl_pmd;			/*                          */
	unsigned char   ovfl_notify;			/*                                               */
	unsigned short  active_set;			/*                                              */
	pfm_ovfl_ctrl_t ovfl_ctrl;			/*                                            */

	unsigned long   pmd_last_reset;			/*                                */
	unsigned long	smpl_pmds[4];			/*                                              */
	unsigned long   smpl_pmds_values[PMU_MAX_PMDS]; /*                                       */
	unsigned long   pmd_value;			/*                                 */
	unsigned long	pmd_eventid;			/*                             */
} pfm_ovfl_arg_t;


typedef struct {
	char		*fmt_name;
	pfm_uuid_t	fmt_uuid;
	size_t		fmt_arg_size;
	unsigned long	fmt_flags;

	int		(*fmt_validate)(struct task_struct *task, unsigned int flags, int cpu, void *arg);
	int		(*fmt_getsize)(struct task_struct *task, unsigned int flags, int cpu, void *arg, unsigned long *size);
	int 		(*fmt_init)(struct task_struct *task, void *buf, unsigned int flags, int cpu, void *arg);
	int		(*fmt_handler)(struct task_struct *task, void *buf, pfm_ovfl_arg_t *arg, struct pt_regs *regs, unsigned long stamp);
	int		(*fmt_restart)(struct task_struct *task, pfm_ovfl_ctrl_t *ctrl, void *buf, struct pt_regs *regs);
	int		(*fmt_restart_active)(struct task_struct *task, pfm_ovfl_ctrl_t *ctrl, void *buf, struct pt_regs *regs);
	int		(*fmt_exit)(struct task_struct *task, void *buf, struct pt_regs *regs);

	struct list_head fmt_list;
} pfm_buffer_fmt_t;

extern int pfm_register_buffer_fmt(pfm_buffer_fmt_t *fmt);
extern int pfm_unregister_buffer_fmt(pfm_uuid_t uuid);

/*
                                        
 */
extern int pfm_mod_read_pmds(struct task_struct *, void *req, unsigned int nreq, struct pt_regs *regs);
extern int pfm_mod_write_pmcs(struct task_struct *, void *req, unsigned int nreq, struct pt_regs *regs);
extern int pfm_mod_write_ibrs(struct task_struct *task, void *req, unsigned int nreq, struct pt_regs *regs);
extern int pfm_mod_write_dbrs(struct task_struct *task, void *req, unsigned int nreq, struct pt_regs *regs);

/*
                                                                  
 */
#define PFM_CPUINFO_SYST_WIDE	0x1	/*                                     */
#define PFM_CPUINFO_DCR_PP	0x2	/*                                            */
#define PFM_CPUINFO_EXCL_IDLE	0x4	/*                                                */

/*
                                                        
 */
typedef struct {
	int	debug;		/*                                  */
	int	debug_ovfl;	/*                                              */
	int	fastctxsw;	/*                                   */
	int	expert_mode;	/*                            */
} pfm_sysctl_t;
extern pfm_sysctl_t pfm_sysctl;


#endif /*                     */
