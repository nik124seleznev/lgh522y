#ifndef _LINUX_BUG_H
#define _LINUX_BUG_H

#include <asm/bug.h>
#include <linux/compiler.h>

enum bug_trap_type {
	BUG_TRAP_TYPE_NONE = 0,
	BUG_TRAP_TYPE_WARN = 1,
	BUG_TRAP_TYPE_BUG = 2,
};

struct pt_regs;

#ifdef __CHECKER__
#define BUILD_BUG_ON_NOT_POWER_OF_2(n) (0)
#define BUILD_BUG_ON_ZERO(e) (0)
#define BUILD_BUG_ON_NULL(e) ((void*)0)
#define BUILD_BUG_ON_INVALID(e) (0)
#define BUILD_BUG_ON_MSG(cond, msg) (0)
#define BUILD_BUG_ON(condition) (0)
#define BUILD_BUG() (0)
#else /*             */

/*                                                                        */
#define BUILD_BUG_ON_NOT_POWER_OF_2(n)			\
	BUILD_BUG_ON((n) == 0 || (((n) & ((n) - 1)) != 0))

/*                                                                   
                                                                     
                                                                        
                      */
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int:-!!(e); }))
#define BUILD_BUG_ON_NULL(e) ((void *)sizeof(struct { int:-!!(e); }))

/*
                                                                           
                                                                            
                    
 */
#define BUILD_BUG_ON_INVALID(e) ((void)(sizeof((__force long)(e))))

/* 
                                                                          
                        
                                                                     
  
                                    
 */
#define BUILD_BUG_ON_MSG(cond, msg) compiletime_assert(!(cond), msg)

/* 
                                                       
                                                                     
  
                                                                          
                                                                              
                                
  
                                                                               
                                                                             
                                                                      
                                                                             
                                                                               
                                                                          
                                                                      
                                                                             
              
 */
#ifndef __OPTIMIZE__
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#else
#define BUILD_BUG_ON(condition) \
	BUILD_BUG_ON_MSG(condition, "BUILD_BUG_ON failed: " #condition)
#endif

/* 
                                     
  
                                                                     
                                                          
                     
 */
#define BUILD_BUG() BUILD_BUG_ON_MSG(1, "BUILD_BUG failed")

#endif	/*             */

#ifdef CONFIG_GENERIC_BUG
#include <asm-generic/bug.h>

static inline int is_warning_bug(const struct bug_entry *bug)
{
	return bug->flags & BUGFLAG_WARNING;
}

const struct bug_entry *find_bug(unsigned long bugaddr);

enum bug_trap_type report_bug(unsigned long bug_addr, struct pt_regs *regs);

/*                                       */
int is_valid_bugaddr(unsigned long addr);

#else	/*                     */

static inline enum bug_trap_type report_bug(unsigned long bug_addr,
					    struct pt_regs *regs)
{
	return BUG_TRAP_TYPE_BUG;
}

#endif	/*                    */
#endif	/*              */
