/*
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Vineetg: March 2009 (Supporting 2 levels of Interrupts)
 *  Stack switching code can no longer reliably rely on the fact that
 *  if we are NOT in user mode, stack is switched to kernel mode.
 *  e.g. L2 IRQ interrupted a L1 ISR which had not yet completed
 *  it's prologue including stack switching from user mode
 *
 * Vineetg: Aug 28th 2008: Bug #94984
 *  -Zero Overhead Loop Context shd be cleared when entering IRQ/EXcp/Trap
 *   Normally CPU does this automatically, however when doing FAKE rtie,
 *   we also need to explicitly do this. The problem in macros
 *   FAKE_RET_FROM_EXCPN and FAKE_RET_FROM_EXCPN_LOCK_IRQ was that this bit
 *   was being "CLEARED" rather then "SET". Actually "SET" clears ZOL context
 *
 * Vineetg: May 5th 2008
 *  -Modified CALLEE_REG save/restore macros to handle the fact that
 *      r25 contains the kernel current task ptr
 *  - Defined Stack Switching Macro to be reused in all intr/excp hdlrs
 *  - Shaved off 11 instructions from RESTORE_ALL_INT1 by using the
 *      address Write back load ld.ab instead of seperate ld/add instn
 *
 * Amit Bhor, Sameer Dhavale: Codito Technologies 2004
 */

#ifndef __ASM_ARC_ENTRY_H
#define __ASM_ARC_ENTRY_H

#ifdef __ASSEMBLY__
#include <asm/unistd.h>		/*                            */
#include <asm/asm-offsets.h>
#include <asm/arcregs.h>
#include <asm/ptrace.h>
#include <asm/processor.h>	/*                   */
#include <asm/thread_info.h>	/*                 */

/*                                                 
  
                     
  
                                       
                                      
  
                                        
                                  
 */

/*                                                              
                                                               
                                                                      
                  
                                                               */
.macro  SAVE_CALLER_SAVED
	st.a    r0, [sp, -4]
	st.a    r1, [sp, -4]
	st.a    r2, [sp, -4]
	st.a    r3, [sp, -4]
	st.a    r4, [sp, -4]
	st.a    r5, [sp, -4]
	st.a    r6, [sp, -4]
	st.a    r7, [sp, -4]
	st.a    r8, [sp, -4]
	st.a    r9, [sp, -4]
	st.a    r10, [sp, -4]
	st.a    r11, [sp, -4]
	st.a    r12, [sp, -4]
.endm

/*                                                              
                                                     
                                                               */
.macro RESTORE_CALLER_SAVED
	ld.ab   r12, [sp, 4]
	ld.ab   r11, [sp, 4]
	ld.ab   r10, [sp, 4]
	ld.ab   r9, [sp, 4]
	ld.ab   r8, [sp, 4]
	ld.ab   r7, [sp, 4]
	ld.ab   r6, [sp, 4]
	ld.ab   r5, [sp, 4]
	ld.ab   r4, [sp, 4]
	ld.ab   r3, [sp, 4]
	ld.ab   r2, [sp, 4]
	ld.ab   r1, [sp, 4]
	ld.ab   r0, [sp, 4]
.endm


/*                                                              
                                                                    
                    
                                                    
                                                            
                                                          
                                                                         
                                                                    
                                                                          
                                                        
                                                               */
.macro SAVE_CALLEE_SAVED_USER
	st.a    r13, [sp, -4]
	st.a    r14, [sp, -4]
	st.a    r15, [sp, -4]
	st.a    r16, [sp, -4]
	st.a    r17, [sp, -4]
	st.a    r18, [sp, -4]
	st.a    r19, [sp, -4]
	st.a    r20, [sp, -4]
	st.a    r21, [sp, -4]
	st.a    r22, [sp, -4]
	st.a    r23, [sp, -4]
	st.a    r24, [sp, -4]

#ifdef CONFIG_ARC_CURR_IN_REG
	; Retrieve orig r25 and save it on stack
	ld      r12, [r25, TASK_THREAD + THREAD_USER_R25]
	st.a    r12, [sp, -4]
#else
	st.a    r25, [sp, -4]
#endif

	/*                                                                 */
	sub sp, sp, 4
.endm

/*                                                              
                                                                    
                                                                       
                                                                      
                                                   
                                                               */
.macro SAVE_CALLEE_SAVED_KERNEL
	st.a    r13, [sp, -4]
	st.a    r14, [sp, -4]
	st.a    r15, [sp, -4]
	st.a    r16, [sp, -4]
	st.a    r17, [sp, -4]
	st.a    r18, [sp, -4]
	st.a    r19, [sp, -4]
	st.a    r20, [sp, -4]
	st.a    r21, [sp, -4]
	st.a    r22, [sp, -4]
	st.a    r23, [sp, -4]
	st.a    r24, [sp, -4]
#ifdef CONFIG_ARC_CURR_IN_REG
	sub     sp, sp, 8
#else
	st.a    r25, [sp, -4]
	sub     sp, sp, 4
#endif
.endm

/*                                                              
                               
                                                                         
                                         
  
        
                                                           
                                       
                                                              
                                                  
                                                               */
.macro RESTORE_CALLEE_SAVED_KERNEL


#ifdef CONFIG_ARC_CURR_IN_REG
	add     sp, sp, 8  /*                                                 */
#else
	add     sp, sp, 4   /*                                        */
	ld.ab   r25, [sp, 4]
#endif

	ld.ab   r24, [sp, 4]
	ld.ab   r23, [sp, 4]
	ld.ab   r22, [sp, 4]
	ld.ab   r21, [sp, 4]
	ld.ab   r20, [sp, 4]
	ld.ab   r19, [sp, 4]
	ld.ab   r18, [sp, 4]
	ld.ab   r17, [sp, 4]
	ld.ab   r16, [sp, 4]
	ld.ab   r15, [sp, 4]
	ld.ab   r14, [sp, 4]
	ld.ab   r13, [sp, 4]

.endm

/*                                                              
                             
                                                                             
                                        
                                                                         
                                                                             
                                                                            
                                                                              
                    
                                                               */
.macro RESTORE_CALLEE_SAVED_USER

	add     sp, sp, 4   /*                                        */

#ifdef CONFIG_ARC_CURR_IN_REG
	ld.ab   r12, [sp, 4]
	st      r12, [r25, TASK_THREAD + THREAD_USER_R25]
#else
	ld.ab   r25, [sp, 4]
#endif

	ld.ab   r24, [sp, 4]
	ld.ab   r23, [sp, 4]
	ld.ab   r22, [sp, 4]
	ld.ab   r21, [sp, 4]
	ld.ab   r20, [sp, 4]
	ld.ab   r19, [sp, 4]
	ld.ab   r18, [sp, 4]
	ld.ab   r17, [sp, 4]
	ld.ab   r16, [sp, 4]
	ld.ab   r15, [sp, 4]
	ld.ab   r14, [sp, 4]
	ld.ab   r13, [sp, 4]
.endm

/*                                                              
                                                                 
                                                               */
.macro DISCARD_CALLEE_SAVED_USER
	add     sp, sp, 14 * 4
.endm

/*                                                              
                                                              
                                                               */
.macro RESTORE_USER_R25
	ld  r25, [r25, TASK_THREAD + THREAD_USER_R25]
.endm

/*                                                             
                                                                
                                                               
                                          
                                                              */

.macro GET_TSK_STACK_BASE tsk, out

	/*                                                             */
	ld  \out, [\tsk, TASK_THREAD_INFO]

	/*                                                      */
	add2 \out, \out, (THREAD_SIZE - 4)/4   /*                 */

.endm

/*                                                              
                                                              
  
                                                        
                                                   
                                                              
               
                                                               */

.macro SWITCH_TO_KERNEL_STK

	/*                                                             */
	bbit1   r9, STATUS_U_BIT, 88f

	/*                                                                     
                                                                    
  */

#ifdef CONFIG_ARC_COMPACT_IRQ_LEVELS
	/*            
                                                                    
                          
                                                                  
                                                              
                                 
                                                                    
                 
                                                                      
                                                                        
  */
	brlo sp, VMALLOC_START, 88f

	/*               
                                                                   
                                                                    
                                      
                                                             
                              
                                                             
                                                                 
                                   
  */

#endif

	/*                                                       
                                                                  
                                 
  */
	b.d	77f

	st.a	sp, [sp, -12]	; Make room for orig_r0 and orig_r8

88: /*                                                             */

	GET_CURR_TASK_ON_CPU   r9

#ifdef CONFIG_ARC_CURR_IN_REG

	/*                                               
                                                        
                                    
  */
	st.as	r25, [r9, (TASK_THREAD + THREAD_USER_R25)/4]
	mov	r25, r9
#endif

	/*                                                         */
	GET_TSK_STACK_BASE  r9, r9

#ifdef PT_REGS_CANARY
	st	0xabcdabcd, [r9, 0]
#endif

	/*                                                 */
	st.a    sp, [r9, -12]	; Make room for orig_r0 and orig_r8

	/*         
                                                                     
                                                                       
                                                  
  */

	/*                                      */
	mov sp, r9

77: /*                                                              */

.endm

/*                                                            
                                                     
                                                   
                                                    
                                                               */

.macro FAKE_RET_FROM_EXCPN  reg

	ld  \reg, [sp, PT_status32]
	bic  \reg, \reg, (STATUS_U_MASK|STATUS_DE_MASK)
	bset \reg, \reg, STATUS_L_BIT
	sr  \reg, [erstatus]
	mov \reg, 55f
	sr  \reg, [eret]

	rtie
55:
.endm

/*
                                       
 */
.macro GET_CURR_THR_INFO_FROM_SP  reg
	and \reg, sp, ~(THREAD_SIZE - 1)
.endm

/*
                                             
 */
.macro GET_CURR_THR_INFO_FLAGS  reg
	GET_CURR_THR_INFO_FROM_SP  \reg
	ld  \reg, [\reg, THREAD_INFO_FLAGS]
.endm

/*                                                              
                                                                    
                                                                      
                                                         
  
                                                                   
                                                             
                                                               */
.macro EXCPN_PROLOG_FREEUP_REG	reg
#ifdef CONFIG_SMP
	sr  \reg, [ARC_REG_SCRATCH_DATA0]
#else
	st  \reg, [@ex_saved_reg1]
#endif
.endm

.macro EXCPN_PROLOG_RESTORE_REG	reg
#ifdef CONFIG_SMP
	lr  \reg, [ARC_REG_SCRATCH_DATA0]
#else
	ld  \reg, [@ex_saved_reg1]
#endif
.endm

/*                                                              
                                                                        
                                                          
                                                                         
                                                                      
                  
                                                                        
                           
                                                               */
.macro SAVE_ALL_EXCEPTION   marker

	st      \marker, [sp, 8]	/*         */
	st      r0, [sp, 4]    /*                                    */

	/*                                            */
	EXCPN_PROLOG_RESTORE_REG  r9

	SAVE_CALLER_SAVED
	st.a    r26, [sp, -4]   /*    */
	st.a    fp, [sp, -4]
	st.a    blink, [sp, -4]
	lr	r9, [eret]
	st.a    r9, [sp, -4]
	lr	r9, [erstatus]
	st.a    r9, [sp, -4]
	st.a    lp_count, [sp, -4]
	lr	r9, [lp_end]
	st.a    r9, [sp, -4]
	lr	r9, [lp_start]
	st.a    r9, [sp, -4]
	lr	r9, [erbta]
	st.a    r9, [sp, -4]

#ifdef PT_REGS_CANARY
	mov   r9, 0xdeadbeef
	st    r9, [sp, -4]
#endif

	/*                                                             */
	sub sp, sp, 4
.endm

/*                                                              
                                   
                                                               */
.macro SAVE_ALL_SYS
	SAVE_ALL_EXCEPTION  orig_r8_IS_EXCPN
.endm

/*                                                              
                                  
                                                               */
.macro SAVE_ALL_TRAP
	/*
                           
                                                                     
                                                       
                                                                      
                                                                      
                                  
  */
	lsl  r9, r8, 16
	or   r9, r9, orig_r8_IS_SCALL

	SAVE_ALL_EXCEPTION  r9
.endm

/*                                                              
                                                          
                                                              
                            
  
        
  
                                                                          
                                                                          
                                    
                                                               */
.macro RESTORE_ALL_SYS

	add sp, sp, 4       /*                                               */

	ld.ab   r9, [sp, 4]
	sr	r9, [erbta]
	ld.ab   r9, [sp, 4]
	sr	r9, [lp_start]
	ld.ab   r9, [sp, 4]
	sr	r9, [lp_end]
	ld.ab   r9, [sp, 4]
	mov	lp_count, r9
	ld.ab   r9, [sp, 4]
	sr	r9, [erstatus]
	ld.ab   r9, [sp, 4]
	sr	r9, [eret]
	ld.ab   blink, [sp, 4]
	ld.ab   fp, [sp, 4]
	ld.ab   r26, [sp, 4]    /*    */
	RESTORE_CALLER_SAVED

	ld  sp, [sp] /*                     */
	/*                                           */
.endm


/*                                                              
                                                 
                                                               */
.macro SAVE_ALL_INT1

	/*                                              
                                                        
 */
#ifdef CONFIG_SMP
	lr  r9, [ARC_REG_SCRATCH_DATA0]
#else
	ld  r9, [@int1_saved_reg]
#endif

	/*                                                   */
	st      orig_r8_IS_IRQ1, [sp, 8]    /*            */
	st      0, [sp, 4]    /*                       */
	SAVE_CALLER_SAVED
	st.a    r26, [sp, -4]   /*    */
	st.a    fp, [sp, -4]
	st.a    blink, [sp, -4]
	st.a    ilink1, [sp, -4]
	lr	r9, [status32_l1]
	st.a    r9, [sp, -4]
	st.a    lp_count, [sp, -4]
	lr	r9, [lp_end]
	st.a    r9, [sp, -4]
	lr	r9, [lp_start]
	st.a    r9, [sp, -4]
	lr	r9, [bta_l1]
	st.a    r9, [sp, -4]

#ifdef PT_REGS_CANARY
	mov   r9, 0xdeadbee1
	st    r9, [sp, -4]
#endif
	/*                                                             */
	sub sp, sp, 4
.endm

.macro SAVE_ALL_INT2

	/*                                                     
                                                           
                      
 */
	/*                                             */
	ld  r9, [@int2_saved_reg]

	/*                                                   */
	st      orig_r8_IS_IRQ2, [sp, 8]    /*            */
	st      0, [sp, 4]    /*                       */
	SAVE_CALLER_SAVED
	st.a    r26, [sp, -4]   /*    */
	st.a    fp, [sp, -4]
	st.a    blink, [sp, -4]
	st.a    ilink2, [sp, -4]
	lr	r9, [status32_l2]
	st.a    r9, [sp, -4]
	st.a    lp_count, [sp, -4]
	lr	r9, [lp_end]
	st.a    r9, [sp, -4]
	lr	r9, [lp_start]
	st.a    r9, [sp, -4]
	lr	r9, [bta_l2]
	st.a    r9, [sp, -4]

#ifdef PT_REGS_CANARY
	mov   r9, 0xdeadbee2
	st    r9, [sp, -4]
#endif

	/*                                                             */
	sub sp, sp, 4
.endm

/*                                                              
                                                    
  
        
  
                                                                          
                                                                          
                                    
                                                               */

.macro RESTORE_ALL_INT1
	add sp, sp, 4       /*                                               */

	ld.ab   r9, [sp, 4] /*                 */
	sr	r9, [bta_l1]
	ld.ab   r9, [sp, 4]
	sr	r9, [lp_start]
	ld.ab   r9, [sp, 4]
	sr	r9, [lp_end]
	ld.ab   r9, [sp, 4]
	mov	lp_count, r9
	ld.ab   r9, [sp, 4]
	sr	r9, [status32_l1]
	ld.ab   r9, [sp, 4]
	mov	ilink1, r9
	ld.ab   blink, [sp, 4]
	ld.ab   fp, [sp, 4]
	ld.ab   r26, [sp, 4]    /*    */
	RESTORE_CALLER_SAVED

	ld  sp, [sp] /*                     */
	/*                                           */
.endm

.macro RESTORE_ALL_INT2
	add sp, sp, 4       /*                                               */

	ld.ab   r9, [sp, 4]
	sr	r9, [bta_l2]
	ld.ab   r9, [sp, 4]
	sr	r9, [lp_start]
	ld.ab   r9, [sp, 4]
	sr	r9, [lp_end]
	ld.ab   r9, [sp, 4]
	mov	lp_count, r9
	ld.ab   r9, [sp, 4]
	sr	r9, [status32_l2]
	ld.ab   r9, [sp, 4]
	mov	ilink2, r9
	ld.ab   blink, [sp, 4]
	ld.ab   fp, [sp, 4]
	ld.ab   r26, [sp, 4]    /*    */
	RESTORE_CALLER_SAVED

	ld  sp, [sp] /*                     */
	/*                                           */

.endm


/*                         */
.macro  GET_CPU_ID  reg
	lr  \reg, [identity]
	lsr \reg, \reg, 8
	bmsk \reg, \reg, 7
.endm

#ifdef CONFIG_SMP

/*                                                 
                                                
                            
                                           
 */
.macro  GET_CURR_TASK_ON_CPU   reg
	GET_CPU_ID  \reg
	ld.as  \reg, [@_current_task, \reg]
.endm

/*                                                 
                                                    
                            
                                           
  
                                                                 
                                                       
                                                             
 */

.macro  SET_CURR_TASK_ON_CPU    tsk, tmp
	GET_CPU_ID  \tmp
	add2 \tmp, @_current_task, \tmp
	st   \tsk, [\tmp]
#ifdef CONFIG_ARC_CURR_IN_REG
	mov r25, \tsk
#endif

.endm


#else   /*                                       */

.macro  GET_CURR_TASK_ON_CPU    reg
	ld  \reg, [@_current_task]
.endm

.macro  SET_CURR_TASK_ON_CPU    tsk, tmp
	st  \tsk, [@_current_task]
#ifdef CONFIG_ARC_CURR_IN_REG
	mov r25, \tsk
#endif
.endm

#endif /*           */

/*                                                                   
                                                                   
                                                     
 */

#ifdef CONFIG_ARC_CURR_IN_REG

.macro GET_CURR_TASK_FIELD_PTR  off,  reg
	add \reg, r25, \off
.endm

#else

.macro GET_CURR_TASK_FIELD_PTR  off,  reg
	GET_CURR_TASK_ON_CPU  \reg
	add \reg, \reg, \off
.endm

#endif	/*                        */

#endif  /*              */

#endif  /*                   */
