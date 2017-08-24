#ifndef __MT_MEMORY_H__
#define __MT_MEMORY_H__

/*
                    
 */

#define PHYS_OFFSET 0x80000000

/*
                 
 */

/*                                      */
#define IO_VIRT_TO_PHYS(v) (0x10000000 | ((v) & 0x0fffffff))
#define IO_PHYS_TO_VIRT(p) (0xf0000000 | ((p) & 0x0fffffff))

#endif  /*                  */
