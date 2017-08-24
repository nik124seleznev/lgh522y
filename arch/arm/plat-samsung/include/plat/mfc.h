/*
 * Copyright (C) 2011 Samsung Electronics Co.Ltd
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#ifndef __PLAT_SAMSUNG_MFC_H
#define __PLAT_SAMSUNG_MFC_H __FILE__

struct s5p_mfc_dt_meminfo {
	unsigned long	loff;
	unsigned long	lsize;
	unsigned long	roff;
	unsigned long	rsize;
	char		*compatible;
};

/* 
                                                                        
                                                        
                                                                
                                                       
                                                               
  
                                                                  
                                                                     
                   
 */
void __init s5p_mfc_reserve_mem(phys_addr_t rbase, unsigned int rsize,
				phys_addr_t lbase, unsigned int lsize);

int __init s5p_fdt_find_mfc_mem(unsigned long node, const char *uname,
				int depth, void *data);

#endif /*                      */
