/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
                                                                          
                                                                    
                                                                            
 */
#if defined(CONFIG_SMP) && (CONFIG_NR_CPUS > 2)
#include <asm-generic/mutex-dec.h>
#else
#include <asm-generic/mutex-xchg.h>
#endif
