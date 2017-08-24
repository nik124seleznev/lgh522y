#ifndef _ASM_X86_ALTERNATIVE_H
#define _ASM_X86_ALTERNATIVE_H

#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/stringify.h>
#include <asm/asm.h>

/*
                                       
  
                                                           
                                                         
  
                                                             
                                                                     
                                                                     
                                               
  
                                                                     
                                                                     
                                                                     
                                      
  
                                                              
                                                                      
                                                           
 */

#ifdef CONFIG_SMP
#define LOCK_PREFIX_HERE \
		".pushsection .smp_locks,\"a\"\n"	\
		".balign 4\n"				\
		".long 671f - .\n" /*        */		\
		".popsection\n"				\
		"671:"

#define LOCK_PREFIX LOCK_PREFIX_HERE "\n\tlock; "

#else /*              */
#define LOCK_PREFIX_HERE ""
#define LOCK_PREFIX ""
#endif

struct alt_instr {
	s32 instr_offset;	/*                      */
	s32 repl_offset;	/*                                   */
	u16 cpuid;		/*                               */
	u8  instrlen;		/*                                */
	u8  replacementlen;	/*                                        */
};

extern void alternative_instructions(void);
extern void apply_alternatives(struct alt_instr *start, struct alt_instr *end);

struct module;

#ifdef CONFIG_SMP
extern void alternatives_smp_module_add(struct module *mod, char *name,
					void *locks, void *locks_end,
					void *text, void *text_end);
extern void alternatives_smp_module_del(struct module *mod);
extern void alternatives_enable_smp(void);
extern int alternatives_text_reserved(void *start, void *end);
extern bool skip_smp_alternatives;
#else
static inline void alternatives_smp_module_add(struct module *mod, char *name,
					       void *locks, void *locks_end,
					       void *text, void *text_end) {}
static inline void alternatives_smp_module_del(struct module *mod) {}
static inline void alternatives_enable_smp(void) {}
static inline int alternatives_text_reserved(void *start, void *end)
{
	return 0;
}
#endif	/*            */

#define OLDINSTR(oldinstr)	"661:\n\t" oldinstr "\n662:\n"

#define b_replacement(number)	"663"#number
#define e_replacement(number)	"664"#number

#define alt_slen "662b-661b"
#define alt_rlen(number) e_replacement(number)"f-"b_replacement(number)"f"

#define ALTINSTR_ENTRY(feature, number)					      \
	" .long 661b - .\n"				/*                 */ \
	" .long " b_replacement(number)"f - .\n"	/*                 */ \
	" .word " __stringify(feature) "\n"		/*                 */ \
	" .byte " alt_slen "\n"				/*                 */ \
	" .byte " alt_rlen(number) "\n"			/*                 */

#define DISCARD_ENTRY(number)				/*              */    \
	" .byte 0xff + (" alt_rlen(number) ") - (" alt_slen ")\n"

#define ALTINSTR_REPLACEMENT(newinstr, feature, number)	/*             */     \
	b_replacement(number)":\n\t" newinstr "\n" e_replacement(number) ":\n\t"

/*                                 */
#define ALTERNATIVE(oldinstr, newinstr, feature)			\
	OLDINSTR(oldinstr)						\
	".pushsection .altinstructions,\"a\"\n"				\
	ALTINSTR_ENTRY(feature, 1)					\
	".popsection\n"							\
	".pushsection .discard,\"aw\",@progbits\n"			\
	DISCARD_ENTRY(1)						\
	".popsection\n"							\
	".pushsection .altinstr_replacement, \"ax\"\n"			\
	ALTINSTR_REPLACEMENT(newinstr, feature, 1)			\
	".popsection"

#define ALTERNATIVE_2(oldinstr, newinstr1, feature1, newinstr2, feature2)\
	OLDINSTR(oldinstr)						\
	".pushsection .altinstructions,\"a\"\n"				\
	ALTINSTR_ENTRY(feature1, 1)					\
	ALTINSTR_ENTRY(feature2, 2)					\
	".popsection\n"							\
	".pushsection .discard,\"aw\",@progbits\n"			\
	DISCARD_ENTRY(1)						\
	DISCARD_ENTRY(2)						\
	".popsection\n"							\
	".pushsection .altinstr_replacement, \"ax\"\n"			\
	ALTINSTR_REPLACEMENT(newinstr1, feature1, 1)			\
	ALTINSTR_REPLACEMENT(newinstr2, feature2, 2)			\
	".popsection"

/*
                                                                     
                       
 */
#include <asm/cpufeature.h>

/*
                                                                    
  
                                                                   
           
  
                                                                    
                                        
  
                                                          
                                       
 */
#define alternative(oldinstr, newinstr, feature)			\
	asm volatile (ALTERNATIVE(oldinstr, newinstr, feature) : : : "memory")

/*
                                          
  
                
                          
                                 
                                                                     
                                                               
                                                         
                                                          
 */
#define alternative_input(oldinstr, newinstr, feature, input...)	\
	asm volatile (ALTERNATIVE(oldinstr, newinstr, feature)		\
		: : "i" (0), ## input)

/*                                                           */
#define alternative_io(oldinstr, newinstr, feature, output, input...)	\
	asm volatile (ALTERNATIVE(oldinstr, newinstr, feature)		\
		: output : "i" (0), ## input)

/*                                                                        */
#define alternative_call(oldfunc, newfunc, feature, output, input...)	\
	asm volatile (ALTERNATIVE("call %P[old]", "call %P[new]", feature) \
		: output : [old] "i" (oldfunc), [new] "i" (newfunc), ## input)

/*
                                                                              
                                          
                                                     
                                   
 */
#define alternative_call_2(oldfunc, newfunc1, feature1, newfunc2, feature2,   \
			   output, input...)				      \
	asm volatile (ALTERNATIVE_2("call %P[old]", "call %P[new1]", feature1,\
		"call %P[new2]", feature2)				      \
		: output : [old] "i" (oldfunc), [new1] "i" (newfunc1),	      \
		[new2] "i" (newfunc2), ## input)

/*
                                                               
                    
 */
#define ASM_OUTPUT2(a...) a

/*
                                                       
                                
 */
#define ASM_NO_INPUT_CLOBBER(clbr...) "i" (0) : clbr

struct paravirt_patch_site;
#ifdef CONFIG_PARAVIRT
void apply_paravirt(struct paravirt_patch_site *start,
		    struct paravirt_patch_site *end);
#else
static inline void apply_paravirt(struct paravirt_patch_site *start,
				  struct paravirt_patch_site *end)
{}
#define __parainstructions	NULL
#define __parainstructions_end	NULL
#endif

extern void *text_poke_early(void *addr, const void *opcode, size_t len);

/*
                                                                       
                                             
                                                                                
                                           
  
           
                                                                                
                                                                               
                                                                          
                                                           
                                                                         
                                                                    
                                                  
                                                                                
                                            
 */
struct text_poke_param {
	void *addr;
	const void *opcode;
	size_t len;
};

extern void *text_poke(void *addr, const void *opcode, size_t len);
extern void *text_poke_smp(void *addr, const void *opcode, size_t len);
extern void text_poke_smp_batch(struct text_poke_param *params, int n);

#endif /*                        */
