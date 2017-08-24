/*
    ni_labpc.h

    Header for ni_labpc.c and ni_labpc_cs.c

    Copyright (C) 2003 Frank Mori Hess <fmhess@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef _NI_LABPC_H
#define _NI_LABPC_H

#define EEPROM_SIZE	256	/*                  */
#define NUM_AO_CHAN	2	/*                                         */

enum labpc_register_layout { labpc_plus_layout, labpc_1200_layout };
enum transfer_type { fifo_not_empty_transfer, fifo_half_full_transfer,
	isa_dma_transfer
};

struct labpc_boardinfo {
	const char *name;
	int device_id;		/*                                      */
	int ai_speed;		/*                                     */

	/*                                           */
	enum labpc_register_layout register_layout;
	int has_ao;		/*                               */
	const struct comedi_lrange *ai_range_table;
	const int *ai_range_code;

	/*                                                       */
	unsigned ai_scan_up:1;

	/*                                          */
	unsigned has_mmio:1;
};

struct labpc_private {
	struct mite_struct *mite;	/*                            */
	/*                                         */
	unsigned long long count;
	/*                                        */
	unsigned int ao_value[NUM_AO_CHAN];
	/*                                                      */
	unsigned int cmd1;
	unsigned int cmd2;
	unsigned int cmd3;
	unsigned int cmd4;
	unsigned int cmd5;
	unsigned int cmd6;
	/*                                            */
	unsigned int stat1;
	unsigned int stat2;
	/*
                                                                       
               
  */
	unsigned int divisor_a0;
	/*
                                                                        
  */
	unsigned int divisor_b0;
	/*
                                                                 
               
  */
	unsigned int divisor_b1;
	unsigned int dma_chan;	/*                     */
	u16 *dma_buffer;	/*                          */
	phys_addr_t dma_addr;
	/*                                             */
	unsigned int dma_transfer_size;
	/*                                      */
	enum transfer_type current_transfer;
	/*                                   */
	unsigned int eeprom_data[EEPROM_SIZE];
	/*                                     */
	unsigned int caldac[16];
	/*
                                                               
               
  */
	unsigned int (*read_byte) (unsigned long address);
	void (*write_byte) (unsigned int byte, unsigned long address);
};

int labpc_common_attach(struct comedi_device *dev,
			unsigned int irq, unsigned long isr_flags);
void labpc_common_detach(struct comedi_device *dev);

extern const int labpc_1200_ai_gain_bits[];
extern const struct comedi_lrange range_labpc_1200_ai;

#endif /*             */
