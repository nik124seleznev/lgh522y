/*
 * NVRAM definitions and access functions.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef _UAPI_ASM_POWERPC_NVRAM_H
#define _UAPI_ASM_POWERPC_NVRAM_H

/*                                 */
#define NVRAM_SIG_SP	0x02	/*                   */
#define NVRAM_SIG_OF	0x50	/*                      */
#define NVRAM_SIG_FW	0x51	/*                  */
#define NVRAM_SIG_HW	0x52	/*                */
#define NVRAM_SIG_FLIP	0x5a	/*                        */
#define NVRAM_SIG_APPL	0x5f	/*                      */
#define NVRAM_SIG_SYS	0x70	/*                 */
#define NVRAM_SIG_CFG	0x71	/*             */
#define NVRAM_SIG_ELOG	0x72	/*           */
#define NVRAM_SIG_VEND	0x7e	/*                */
#define NVRAM_SIG_FREE	0x7f	/*            */
#define NVRAM_SIG_OS	0xa0	/*            */
#define NVRAM_SIG_PANIC	0xa1	/*                   */


/*                                */

enum {
	pmac_nvram_OF,		/*                         */
	pmac_nvram_XPRAM,	/*                       */
	pmac_nvram_NR		/*                               */
};


/*                       */
#define PMAC_XPRAM_MACHINE_LOC	0xe4
#define PMAC_XPRAM_SOUND_VOLUME	0x08

/*                                              */
struct pmac_machine_location {
	unsigned int	latitude;	/*                            */
	unsigned int	longitude;	/*                            */
	unsigned int	delta;		/*                          */
};

/*
                    
  
                                                             
                                                         
 */

#define OBSOLETE_PMAC_NVRAM_GET_OFFSET \
				_IOWR('p', 0x40, int)

#define IOC_NVRAM_GET_OFFSET	_IOWR('p', 0x42, int)	/*                            */
#define IOC_NVRAM_SYNC		_IO('p', 0x43)		/*                  */

#endif /*                           */
