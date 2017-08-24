/*
 * Copyright (C) 2008 Imagination Technologies Ltd.
 * Licensed under the GPL
 *
 * Dynamic ftrace support.
 */

#include <linux/ftrace.h>
#include <linux/io.h>
#include <linux/uaccess.h>

#include <asm/cacheflush.h>

#define D04_MOVT_TEMPLATE	0x02200005
#define D04_CALL_TEMPLATE	0xAC200005
#define D1RTP_MOVT_TEMPLATE	0x03200005
#define D1RTP_CALL_TEMPLATE	0xAC200006

static const unsigned long NOP[2] = {0xa0fffffe, 0xa0fffffe};
static unsigned long movt_and_call_insn[2];

static unsigned char *ftrace_nop_replace(void)
{
	return (char *)&NOP[0];
}

static unsigned char *ftrace_call_replace(unsigned long pc, unsigned long addr)
{
	unsigned long hi16, low16;

	hi16 = (addr & 0xffff0000) >> 13;
	low16 = (addr & 0x0000ffff) << 3;

	/*
                                                   
                                                         
                                                                    
                                                               
  */
	if (pc == (unsigned long)&ftrace_call) {
		writel(D1RTP_MOVT_TEMPLATE | hi16, &movt_and_call_insn[0]);
		writel(D1RTP_CALL_TEMPLATE | low16, &movt_and_call_insn[1]);
	} else {
		writel(D04_MOVT_TEMPLATE | hi16, &movt_and_call_insn[0]);
		writel(D04_CALL_TEMPLATE | low16, &movt_and_call_insn[1]);
	}

	return (unsigned char *)&movt_and_call_insn[0];
}

static int ftrace_modify_code(unsigned long pc, unsigned char *old_code,
			      unsigned char *new_code)
{
	unsigned char replaced[MCOUNT_INSN_SIZE];

	/*
                                             
                                                              
                              
   
                                                    
                  
  */

	/*                                 */
	if (probe_kernel_read(replaced, (void *)pc, MCOUNT_INSN_SIZE))
		return -EFAULT;

	/*                                         */
	if (memcmp(replaced, old_code, MCOUNT_INSN_SIZE) != 0)
		return -EINVAL;

	/*                                    */
	if (probe_kernel_write((void *)pc, new_code, MCOUNT_INSN_SIZE))
		return -EPERM;

	flush_icache_range(pc, pc + MCOUNT_INSN_SIZE);

	return 0;
}

int ftrace_update_ftrace_func(ftrace_func_t func)
{
	int ret;
	unsigned long pc;
	unsigned char old[MCOUNT_INSN_SIZE], *new;

	pc = (unsigned long)&ftrace_call;
	memcpy(old, &ftrace_call, MCOUNT_INSN_SIZE);
	new = ftrace_call_replace(pc, (unsigned long)func);
	ret = ftrace_modify_code(pc, old, new);

	return ret;
}

int ftrace_make_nop(struct module *mod,
		    struct dyn_ftrace *rec, unsigned long addr)
{
	unsigned char *new, *old;
	unsigned long ip = rec->ip;

	old = ftrace_call_replace(ip, addr);
	new = ftrace_nop_replace();

	return ftrace_modify_code(ip, old, new);
}

int ftrace_make_call(struct dyn_ftrace *rec, unsigned long addr)
{
	unsigned char *new, *old;
	unsigned long ip = rec->ip;

	old = ftrace_nop_replace();
	new = ftrace_call_replace(ip, addr);

	return ftrace_modify_code(ip, old, new);
}

/*                        */
int __init ftrace_dyn_arch_init(void)
{
	return 0;
}
