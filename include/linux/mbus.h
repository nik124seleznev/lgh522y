/*
 * Marvell MBUS common definitions.
 *
 * Copyright (C) 2008 Marvell Semiconductor
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __LINUX_MBUS_H
#define __LINUX_MBUS_H

struct mbus_dram_target_info
{
	/*
                                                    
  */
	u8		mbus_dram_target_id;

	/*
                                                          
                                                       
                                                  
  */
	int		num_cs;
	struct mbus_dram_window {
		u8	cs_index;
		u8	mbus_attr;
		u32	base;
		u32	size;
	} cs[4];
};

/*                                             */
#define MVEBU_MBUS_PCI_IO  0x1
#define MVEBU_MBUS_PCI_MEM 0x2
#define MVEBU_MBUS_PCI_WA  0x3

/*
                                                                    
                           
 */
#define MVEBU_MBUS_NO_REMAP (0xffffffff)

/*                                    */
#define MVEBU_MBUS_MAX_WINNAME_SZ 32

/*
                                                                     
                                                                
 */
#ifdef CONFIG_PLAT_ORION
extern const struct mbus_dram_target_info *mv_mbus_dram_info(void);
#else
static inline const struct mbus_dram_target_info *mv_mbus_dram_info(void)
{
	return NULL;
}
#endif

int mvebu_mbus_add_window_remap_flags(const char *devname, phys_addr_t base,
				      size_t size, phys_addr_t remap,
				      unsigned int flags);
int mvebu_mbus_add_window(const char *devname, phys_addr_t base,
			  size_t size);
int mvebu_mbus_del_window(phys_addr_t base, size_t size);
int mvebu_mbus_init(const char *soc, phys_addr_t mbus_phys_base,
		    size_t mbus_size, phys_addr_t sdram_phys_base,
		    size_t sdram_size);

#endif /*                */
