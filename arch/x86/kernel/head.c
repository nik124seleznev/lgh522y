#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/memblock.h>

#include <asm/setup.h>
#include <asm/bios_ebda.h>

/*
                                                           
                                                       
                                                           
                                                            
                                                               
                                                             
                                                              
                                            
  
                                                            
                                                             
                                                             
                                                          
                                                
 */

#define BIOS_LOWMEM_KILOBYTES	0x413
#define LOWMEM_CAP		0x9f000U	/*                  */
#define INSANE_CUTOFF		0x20000U	/*                         */

void __init reserve_ebda_region(void)
{
	unsigned int lowmem, ebda_addr;

	/*
                                                 
                                                  
                                                    
                                                  
                                                  
                                
  */
	if (paravirt_enabled())
		return;

	/*                                  */
	lowmem = *(unsigned short *)__va(BIOS_LOWMEM_KILOBYTES);
	lowmem <<= 10;

	/*                    */
	ebda_addr = get_bios_ebda();

	/*
                                                     
                                                           
                                     
  */

	/*                                                       */
	if (ebda_addr < INSANE_CUTOFF)
		ebda_addr = LOWMEM_CAP;

	/*                                                 */
	if (lowmem < INSANE_CUTOFF)
		lowmem = LOWMEM_CAP;

	/*                                                            */
	lowmem = min(lowmem, ebda_addr);
	lowmem = min(lowmem, LOWMEM_CAP); /*              */

	/*                                                    */
	memblock_reserve(lowmem, 0x100000 - lowmem);
}
