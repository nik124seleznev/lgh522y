#ifndef _LINUX_PERSONALITY_H
#define _LINUX_PERSONALITY_H

#include <uapi/linux/personality.h>


/*
                                              
 */

struct exec_domain;
struct pt_regs;

extern int		register_exec_domain(struct exec_domain *);
extern int		unregister_exec_domain(struct exec_domain *);
extern int		__set_personality(unsigned int);


/*
                                      
   
                                                           
                                                           
 */
typedef void (*handler_t)(int, struct pt_regs *);

struct exec_domain {
	const char		*name;		/*                        */
	handler_t		handler;	/*                      */
	unsigned char		pers_low;	/*                    */
	unsigned char		pers_high;	/*                     */
	unsigned long		*signal_map;	/*                */
	unsigned long		*signal_invmap;	/*                        */
	struct map_segment	*err_map;	/*               */
	struct map_segment	*socktype_map;	/*                     */
	struct map_segment	*sockopt_map;	/*                       */
	struct map_segment	*af_map;	/*                        */
	struct module		*module;	/*                           */
	struct exec_domain	*next;		/*                        */
};

/*
                                             
 */
#define personality(pers)	(pers & PER_MASK)


/*
                                                       
 */
#define set_personality(pers) \
	((current->personality == (pers)) ? 0 : __set_personality(pers))

#endif /*                      */
