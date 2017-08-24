/* Copyright (C) 2006-2013 B.A.T.M.A.N. contributors:
 *
 * Simon Wunderlich, Marek Lindner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA
 */

#include "main.h"
#include "bitarray.h"

#include <linux/bitops.h>

/*                                     */
static void batadv_bitmap_shift_left(unsigned long *seq_bits, int32_t n)
{
	if (n <= 0 || n >= BATADV_TQ_LOCAL_WINDOW_SIZE)
		return;

	bitmap_shift_left(seq_bits, seq_bits, n, BATADV_TQ_LOCAL_WINDOW_SIZE);
}


/*                                                                  
  
           
                                                      
                                          
 */
int batadv_bit_get_packet(void *priv, unsigned long *seq_bits,
			  int32_t seq_num_diff, int set_mark)
{
	struct batadv_priv *bat_priv = priv;

	/*                                                                    
                                             
  */
	if (seq_num_diff <= 0 && seq_num_diff > -BATADV_TQ_LOCAL_WINDOW_SIZE) {
		if (set_mark)
			batadv_set_bit(seq_bits, -seq_num_diff);
		return 0;
	}

	/*                                                              
                            
  */
	if (seq_num_diff > 0 && seq_num_diff < BATADV_TQ_LOCAL_WINDOW_SIZE) {
		batadv_bitmap_shift_left(seq_bits, seq_num_diff);

		if (set_mark)
			batadv_set_bit(seq_bits, 0);
		return 1;
	}

	/*                                                                 */
	if (seq_num_diff >= BATADV_TQ_LOCAL_WINDOW_SIZE &&
	    seq_num_diff < BATADV_EXPECTED_SEQNO_RANGE) {
		batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
			   "We missed a lot of packets (%i) !\n",
			   seq_num_diff - 1);
		bitmap_zero(seq_bits, BATADV_TQ_LOCAL_WINDOW_SIZE);
		if (set_mark)
			batadv_set_bit(seq_bits, 0);
		return 1;
	}

	/*                                                              
                                                               
                                                                 
                              
   
                                                
      
                                               
  */
	batadv_dbg(BATADV_DBG_BATMAN, bat_priv,
		   "Other host probably restarted!\n");

	bitmap_zero(seq_bits, BATADV_TQ_LOCAL_WINDOW_SIZE);
	if (set_mark)
		batadv_set_bit(seq_bits, 0);

	return 1;
}
