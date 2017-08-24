/*
                                                                      
                                             
  
                                     
 */

#ifndef __ASM_BOOTX_H__
#define __ASM_BOOTX_H__

#include <uapi/asm/bootx.h>

/*                                                                      
                                                                        
                                                                         
                
 */
#define BOOTX_COLORTABLE_SIZE    (256UL*3UL*2UL)

/*                                                                    
                                                                       
                           
 */
struct bootx_dt_prop {
	u32	name;
	int	length;
	u32	value;
	u32	next;
};

struct bootx_dt_node {
	u32	unused0;
	u32	unused1;
	u32	phandle;	/*                      */
	u32	unused2;
	u32	unused3;
	u32	unused4;
	u32	unused5;
	u32	full_name;
	u32	properties;
	u32	parent;
	u32	child;
	u32	sibling;
	u32	next;
	u32	allnext;
};

extern void bootx_init(unsigned long r4, unsigned long phys);

#endif
