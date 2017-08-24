/* written by Philipp Rumpf, Copyright (C) 1999 SuSE GmbH Nuernberg
** Copyright (C) 2000 Grant Grundler, Hewlett-Packard
*/
#ifndef _UAPI_PARISC_PTRACE_H
#define _UAPI_PARISC_PTRACE_H


#include <linux/types.h>

/*                                                             
                              
  
                                                              
                                                                      
                          
 */

struct pt_regs {
	unsigned long gr[32];	/*                 */
	__u64 fr[32];
	unsigned long sr[ 8];
	unsigned long iasq[2];
	unsigned long iaoq[2];
	unsigned long cr27;
	unsigned long pad0;     /*                          */
	unsigned long orig_r28;
	unsigned long ksp;
	unsigned long kpc;
	unsigned long sar;	/*      */
	unsigned long iir;	/*      */
	unsigned long isr;	/*      */
	unsigned long ior;	/*      */
	unsigned long ipsw;	/*      */
};

/*
                                                                     
                                                                   
  
                                                                    
                                                                   
                                        
 */
#define PTRACE_SINGLEBLOCK	12	/*                                    */


#endif /*                       */
