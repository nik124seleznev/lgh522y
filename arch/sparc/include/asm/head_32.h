#ifndef __SPARC_HEAD_H
#define __SPARC_HEAD_H

#define KERNBASE        0xf0000000  /*                                             */

#define WRITE_PAUSE      nop; nop; nop; /*                                     */

/*                            */

/*                     */
#define TRAP_ENTRY(type, label) \
	rd %psr, %l0; b label; rd %wim, %l3; nop;

/*                  */
#define SRMMU_TFAULT rd %psr, %l0; rd %wim, %l3; b srmmu_fault; mov 1, %l7;
#define SRMMU_DFAULT rd %psr, %l0; rd %wim, %l3; b srmmu_fault; mov 0, %l7;

/*                                        */
#define BAD_TRAP(num) \
        rd %psr, %l0; mov num, %l7; b bad_trap_handler; rd %wim, %l3;

/*                                                                          */
#define SKIP_TRAP(type, name) \
	jmpl %l2, %g0; rett %l2 + 4; nop; nop;

/*                                                                
                                                                     
                                                                      
                                                            
 */

/*                                       */
#define LINUX_SYSCALL_TRAP \
        sethi %hi(sys_call_table), %l7; \
        or %l7, %lo(sys_call_table), %l7; \
        b linux_sparc_syscall; \
        rd %psr, %l0;

#define BREAKPOINT_TRAP \
	b breakpoint_trap; \
	rd %psr,%l0; \
	nop; \
	nop;

#ifdef CONFIG_KGDB
#define KGDB_TRAP(num) \
	b kgdb_trap_low; \
	rd %psr,%l0; \
	nop; \
	nop;
#else
#define KGDB_TRAP(num) \
	BAD_TRAP(num)
#endif

/*                                                     */
#define GETCC_TRAP \
        b getcc_trap_handler; rd %psr, %l0; nop; nop;

/*                                                     */
#define SETCC_TRAP \
        b setcc_trap_handler; rd %psr, %l0; nop; nop;

/*                                         */
#define GETPSR_TRAP \
	rd %psr, %i0; jmp %l2; rett %l2 + 4; nop;

/*                                                                          
                                   
 */
#define TRAP_ENTRY_INTERRUPT(int_level) \
        mov int_level, %l7; rd %psr, %l0; b real_irq_entry; rd %wim, %l3;

/*                                                                    
                                 
 */
#define WINDOW_SPILL \
        rd %psr, %l0; rd %wim, %l3; b spill_window_entry; andcc %l0, PSR_PS, %g0;

#define WINDOW_FILL \
        rd %psr, %l0; rd %wim, %l3; b fill_window_entry; andcc %l0, PSR_PS, %g0;

#endif /*                */