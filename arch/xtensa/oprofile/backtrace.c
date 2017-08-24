/**
 * @file backtrace.c
 *
 * @remark Copyright 2008 Tensilica Inc.
 * @remark Read the file COPYING
 *
 */

#include <linux/oprofile.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/ptrace.h>
#include <asm/uaccess.h>
#include <asm/traps.h>

/*                                                      
                                        
 */
extern int common_exception_return;

/*                                                                  
                
 */
struct frame_start {
	unsigned long a0;
	unsigned long a1;
};

static void xtensa_backtrace_user(struct pt_regs *regs, unsigned int depth)
{
	unsigned long windowstart = regs->windowstart;
	unsigned long windowbase = regs->windowbase;
	unsigned long a0 = regs->areg[0];
	unsigned long a1 = regs->areg[1];
	unsigned long pc = MAKE_PC_FROM_RA(a0, regs->pc);
	int index;

	/*                                        */
	if (pc != 0 && pc <= TASK_SIZE)
		oprofile_add_trace(pc);
	else
		return;

	/*           
   
                                               
                                   
   
                         
  */

	/*          */
	/*                                                    
                                     
  */
	windowstart = (windowstart << WSBITS | windowstart) >> windowbase;

	/*                                               
                  
  */
	for (index = WSBITS - 1; (index > 0) && depth; depth--, index--)
		if (windowstart & (1 << index)) {
			/*                        
                                      
    */
			a0 = regs->areg[index * 4];
			a1 = regs->areg[index * 4 + 1];
			/*                            */
			pc = MAKE_PC_FROM_RA(a0, pc);

			/*                          */
			if (pc != 0 && pc <= TASK_SIZE)
				oprofile_add_trace(pc);
			else
				return;
		}

	/*         */
	/*                                                 
                           
  */
	if (depth > 0) {
		/*                             */
		/*                     */
		while (a0 != 0 && depth--) {

			struct frame_start frame_start;
			/*                                    
                                         
    */
			unsigned long *psp = (unsigned long *)a1;
			psp -= 4;

			/*                                      */
			if (!access_ok(VERIFY_READ, psp, sizeof(frame_start)))
				return;
			/*                                          */
			if (__copy_from_user_inatomic(&frame_start, psp,
						sizeof(frame_start)))
				return;

			a0 = frame_start.a0;
			a1 = frame_start.a1;
			pc = MAKE_PC_FROM_RA(a0, pc);

			if (pc != 0 && pc <= TASK_SIZE)
				oprofile_add_trace(pc);
			else
				return;
		}
	}
}

static void xtensa_backtrace_kernel(struct pt_regs *regs, unsigned int depth)
{
	unsigned long pc = regs->pc;
	unsigned long *psp;
	unsigned long sp_start, sp_end;
	unsigned long a0 = regs->areg[0];
	unsigned long a1 = regs->areg[1];

	sp_start = a1 & ~(THREAD_SIZE-1);
	sp_end = sp_start + THREAD_SIZE;

	/*                                               */
	spill_registers();

	/*                                                   
                                             
  */
	while (a1 > sp_start && a1 < sp_end && depth--) {
		pc = MAKE_PC_FROM_RA(a0, pc);

		/*                          */
		oprofile_add_trace(pc);
		if (pc == (unsigned long) &common_exception_return) {
			regs = (struct pt_regs *)a1;
			if (user_mode(regs)) {
				pc = regs->pc;
				if (pc != 0 && pc <= TASK_SIZE)
					oprofile_add_trace(pc);
				else
					return;
				return xtensa_backtrace_user(regs, depth);
			}
			a0 = regs->areg[0];
			a1 = regs->areg[1];
			continue;
		}

		psp = (unsigned long *)a1;

		a0 = *(psp - 4);
		a1 = *(psp - 3);

		if (a1 <= (unsigned long)psp)
			return;

	}
	return;
}

void xtensa_backtrace(struct pt_regs * const regs, unsigned int depth)
{
	if (user_mode(regs))
		xtensa_backtrace_user(regs, depth);
	else
		xtensa_backtrace_kernel(regs, depth);
}
