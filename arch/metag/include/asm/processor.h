/*
 * Copyright (C) 2005,2006,2007,2008 Imagination Technologies
 */

#ifndef __ASM_METAG_PROCESSOR_H
#define __ASM_METAG_PROCESSOR_H

#include <linux/atomic.h>

#include <asm/page.h>
#include <asm/ptrace.h>
#include <asm/metag_regs.h>

/*
                                                       
                                           
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l; })

/*                                        */
#define TASK_SIZE	PAGE_OFFSET
/*                                                                          */
#define STACK_TOP	(TASK_SIZE - PAGE_SIZE)
#define STACK_TOP_MAX	STACK_TOP
/*                                 */
#define STACK_SIZE_MAX	(1 << 28)	/*        */

/*                                                                 
                       
 */
#define TASK_UNMAPPED_BASE	META_MEMORY_BASE

typedef struct {
	unsigned long seg;
} mm_segment_t;

#ifdef CONFIG_METAG_FPU
struct meta_fpu_context {
	TBICTXEXTFPU fpstate;
	union {
		struct {
			TBICTXEXTBB4 fx8_15;
			TBICTXEXTFPACC fpacc;
		} fx8_15;
		struct {
			TBICTXEXTFPACC fpacc;
			TBICTXEXTBB4 unused;
		} nofx8_15;
	} extfpstate;
	bool needs_restore;
};
#else
struct meta_fpu_context {};
#endif

#ifdef CONFIG_METAG_DSP
struct meta_ext_context {
	struct {
		TBIEXTCTX ctx;
		TBICTXEXTBB8 bb8;
		TBIDUAL ax[TBICTXEXTAXX_BYTES / sizeof(TBIDUAL)];
		TBICTXEXTHL2 hl2;
		TBICTXEXTTDPR ext;
		TBICTXEXTRP6 rp;
	} regs;

	/*                            */
	void *ram[2];

	/*                                        */
	unsigned int ram_sz[2];
};
#else
struct meta_ext_context {};
#endif

struct thread_struct {
	PTBICTX kernel_context;
	/*                                          */
	unsigned int user_flags;
	struct meta_fpu_context *fpu_context;
	void __user *tls_ptr;
	unsigned short int_depth;
	unsigned short txdefr_failure;
	struct meta_ext_context *dsp_context;
};

#define INIT_THREAD  { \
	NULL,			/*                */	\
	0,			/*            */	\
	NULL,			/*             */	\
	NULL,			/*         */		\
	1,			/*                                */	\
	0,			/*                */	\
	NULL,			/*             */	\
}

/*                                                                            
       
  
                            

                                                
                                            
                                             
                                           
          
                                                      
          

 */
#define start_thread(regs, pc, usp) do {				   \
	unsigned int *argc = (unsigned int *) bprm->exec;		   \
	set_fs(USER_DS);						   \
	current->thread.int_depth = 1;					   \
	/*                                      */			   \
	regs->ctx.SaveMask = TBICTX_PRIV_BIT;				   \
	regs->ctx.CurrPC = pc;						   \
	regs->ctx.AX[0].U0 = usp;					   \
	regs->ctx.DX[3].U1 = *((int *)argc);			/*      */ \
	regs->ctx.DX[3].U0 = (int)((int *)argc + 1);		/*      */ \
	regs->ctx.DX[2].U1 = (int)((int *)argc +			   \
				   regs->ctx.DX[3].U1 + 2);	/*      */ \
	regs->ctx.DX[2].U0 = 0;				   /*           */ \
} while (0)

/*                                        */
struct task_struct;

/*                                      */
static inline void release_thread(struct task_struct *dead_task)
{
}

#define copy_segments(tsk, mm)		do { } while (0)
#define release_segments(mm)		do { } while (0)

extern void exit_thread(void);

/*
                                       
 */
#define	thread_saved_pc(tsk)	\
	((unsigned long)(tsk)->thread.kernel_context->CurrPC)
#define thread_saved_sp(tsk)	\
	((unsigned long)(tsk)->thread.kernel_context->AX[0].U0)
#define thread_saved_fp(tsk)	\
	((unsigned long)(tsk)->thread.kernel_context->AX[1].U0)

unsigned long get_wchan(struct task_struct *p);

#define	KSTK_EIP(tsk)	((tsk)->thread.kernel_context->CurrPC)
#define	KSTK_ESP(tsk)	((tsk)->thread.kernel_context->AX[0].U0)

#define user_stack_pointer(regs)        ((regs)->ctx.AX[0].U0)

#define cpu_relax()     barrier()

extern void setup_priv(void);

static inline unsigned int hard_processor_id(void)
{
	unsigned int id;

	asm volatile ("MOV	%0, TXENABLE\n"
		      "AND	%0, %0, %1\n"
		      "LSR	%0, %0, %2\n"
		      : "=&d" (id)
		      : "I" (TXENABLE_THREAD_BITS),
			"K" (TXENABLE_THREAD_S)
		      );

	return id;
}

#define OP3_EXIT	0

#define HALT_OK		0
#define HALT_PANIC	-1

/*
                                                                    
                                                                      
                                                                      
 */
static inline void hard_processor_halt(int exit_code)
{
	asm volatile ("MOV	D1Ar1, %0\n"
		      "MOV	D0Ar6, %1\n"
		      "MSETL	[A0StP],D0Ar6,D0Ar4,D0Ar2\n"
		      "1:\n"
		      "SWITCH	#0xC30006\n"
		      "B		1b\n"
		      : : "r" (exit_code), "K" (OP3_EXIT));
}

/*                                                                      */
extern void (*soc_restart)(char *cmd);
extern void (*soc_halt)(void);

extern void show_trace(struct task_struct *tsk, unsigned long *sp,
		       struct pt_regs *regs);

#endif
