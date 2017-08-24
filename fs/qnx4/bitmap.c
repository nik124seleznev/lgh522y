/*
                                          
  
                  
  
                                       
  
            
  
                                                 
                                                   
                                                                         
                                                              
 */

#include <linux/buffer_head.h>
#include <linux/bitops.h>
#include "qnx4.h"

unsigned long qnx4_count_free_blocks(struct super_block *sb)
{
	int start = le32_to_cpu(qnx4_sb(sb)->BitMap->di_first_xtnt.xtnt_blk) - 1;
	int total = 0;
	int total_free = 0;
	int offset = 0;
	int size = le32_to_cpu(qnx4_sb(sb)->BitMap->di_size);
	struct buffer_head *bh;

	while (total < size) {
		int bytes = min(size - total, QNX4_BLOCK_SIZE);

		if ((bh = sb_bread(sb, start + offset)) == NULL) {
			printk(KERN_ERR "qnx4: I/O error in counting free blocks\n");
			break;
		}
		total_free += bytes * BITS_PER_BYTE -
				memweight(bh->b_data, bytes);
		brelse(bh);
		total += bytes;
		offset++;
	}

	return total_free;
}
