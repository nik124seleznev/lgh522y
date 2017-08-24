/*
 * Meta DA JTAG debugger control.
 *
 * Copyright 2012 Imagination Technologies Ltd.
 */

#ifndef _METAG_DA_H_
#define _METAG_DA_H_

#ifdef CONFIG_METAG_DA

#include <linux/init.h>
#include <linux/types.h>

extern bool _metag_da_present;

/* 
                                                               
  
                                                    
 */
static inline bool metag_da_enabled(void)
{
	return _metag_da_present;
}

/* 
                                                    
  
                                                             
  
                                           
 */
int __init metag_da_probe(void);

#else /*                  */

#define metag_da_enabled() false
#define metag_da_probe() do {} while (0)

#endif

#endif /*              */
