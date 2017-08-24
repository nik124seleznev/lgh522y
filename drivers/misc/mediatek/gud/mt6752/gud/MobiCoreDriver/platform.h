/*
 * Copyright (c) 2013 TRUSTONIC LIMITED
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
/*
                                                        
                      
  
                                         
  
                                                 
                                       
 */
#ifndef _MC_DRV_PLATFORM_H_
#define _MC_DRV_PLATFORM_H_

/*                     */
#define MC_INTR_SSIQ 280

/*                            */
#define MC_MEM_TRACES

/*                                 */
#ifdef CONFIG_PM_RUNTIME
 #define MC_PM_RUNTIME
#endif

//                            
/*                                                */
#define CPU_IDS {0x0000, 0x0001, 0x0002, 0x0003, 0x0100, 0x0101, 0x0102, 0x0103}
#define COUNT_OF_CPUS 8

/*                               */
#define MC_FASTCALL_WORKER_THREAD

/*             */
#define LPAE_SUPPORT

/*                              */
#define MC_AARCH32_FC


#endif /*                     */
