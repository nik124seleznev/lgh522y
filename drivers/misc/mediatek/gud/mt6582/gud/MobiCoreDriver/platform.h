/*
 * Header file of MobiCore Driver Kernel Module Platform
 * specific structures
 *
 * Internal structures of the McDrvModule
 *
 * Header file the MobiCore Driver Kernel Module,
 * its internal structures and defines.
 *
 * <-- Copyright Giesecke & Devrient GmbH 2009-2012 -->
 * <-- Copyright Trustonic Limited 2013 -->
 */

#ifndef _MC_DRV_PLATFORM_H_
#define _MC_DRV_PLATFORM_H_

/*                     */
#define MC_INTR_SSIQ 80

/*                            */
#define MC_MEM_TRACES

/*                                 */
#ifdef CONFIG_PM_RUNTIME
 #define MC_PM_RUNTIME
#endif

#define TBASE_CORE_SWITCHER
/*                                                */
#define CPU_IDS {0x0000, 0x0001, 0x0002, 0x0003}
#define COUNT_OF_CPUS 4

/*                               */
#define MC_FASTCALL_WORKER_THREAD

#endif /*                     */
