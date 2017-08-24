/*
 *   Copyright IBM Corp. 2011
 *   Author(s): Andreas Krebbel (krebbel@linux.vnet.ibm.com)
 *
 * @remark Copyright 2011 OProfile authors
 */

#ifndef OP_COUNTER_H
#define OP_COUNTER_H

struct op_counter_config {
	/*                                                 */
	/*                                                     */
	/*                                     */
	unsigned long kernel;
	unsigned long user;
};

extern struct op_counter_config counter_config;

#endif /*              */
