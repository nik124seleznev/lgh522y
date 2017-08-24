/*
 *
 * (C) COPYRIGHT ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * A copy of the licence is included with the program, and can also be obtained
 * from Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 *
 */





#include <linux/ump.h>

#include <stdio.h>
#include <stdlib.h>

/*
                                                                 
                                                                               
                                                
                                                                             
                                                           
                                                                            
                                                                                   
  
                                                                          
                                          
   */

static int display_ump_memory_information(ump_secure_id secure_id)
{
	const ump_dd_physical_block_64 * ump_blocks = NULL;
	ump_dd_handle ump_mem;
	uint64_t nr_blocks;
	int i;
	ump_alloc_flags flags;

	/*                                 */
	ump_mem = ump_dd_from_secure_id(secure_id);

	if (UMP_DD_INVALID_MEMORY_HANDLE == ump_mem)
	{
		/*                     */
		return -1;
	}

	/*                                                                                                                */

	ump_dd_phys_blocks_get_64(ump_mem, &nr_blocks, &ump_blocks);
	flags = ump_dd_allocation_flags_get(ump_mem);

	printf("UMP allocation with secure ID %u consists of %zd physical block(s):\n", secure_id, nr_blocks);

	for(i=0; i<nr_blocks; ++i)
	{
		printf("\tBlock %d: 0x%08zX size 0x%08zX\n", i, ump_blocks[i].addr, ump_blocks[i].size);
	}

	printf("and was allocated using the following bitflag combo:  0x%lX\n", flags);

	ump_dd_release(ump_mem);

	return 0;
}

