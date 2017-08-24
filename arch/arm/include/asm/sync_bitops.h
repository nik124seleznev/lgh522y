#ifndef __ASM_SYNC_BITOPS_H__
#define __ASM_SYNC_BITOPS_H__

#include <asm/bitops.h>
#include <asm/system.h>

/*                                                                      
                                                                   
  
                                                                         
                                                                         
                                                                          
                                                                        
                                              
 */

#define sync_set_bit(nr, p)		_set_bit(nr, p)
#define sync_clear_bit(nr, p)		_clear_bit(nr, p)
#define sync_change_bit(nr, p)		_change_bit(nr, p)
#define sync_test_and_set_bit(nr, p)	_test_and_set_bit(nr, p)
#define sync_test_and_clear_bit(nr, p)	_test_and_clear_bit(nr, p)
#define sync_test_and_change_bit(nr, p)	_test_and_change_bit(nr, p)
#define sync_test_bit(nr, addr)		test_bit(nr, addr)
#define sync_cmpxchg			cmpxchg


#endif
