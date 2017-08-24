/******************************************************************************
*******************************************************************************
**
**  Copyright (C) Sistina Software, Inc.  1997-2003  All rights reserved.
**  Copyright (C) 2004-2011 Red Hat, Inc.  All rights reserved.
**
**  This copyrighted material is made available to anyone wishing to use,
**  modify, copy, or redistribute it subject to the terms and conditions
**  of the GNU General Public License v.2.
**
*******************************************************************************
******************************************************************************/

#ifndef _UAPI__DLM_DOT_H__
#define _UAPI__DLM_DOT_H__

/*
                                              
                                                
 */

/*                                */
#include <linux/dlmconstants.h>
#include <linux/types.h>

typedef void dlm_lockspace_t;

/*
                    
  
                                                                             
                                                                           
                                                                            
                                           
  
                                                                              
                                         
  
                                                                                
                                     
  
                                                                               
                                                           
                                                                           
  
                                                                      
                                      
  
                                   
                                                                
                                                
                                                  
                                      
                                                               
 */

#define DLM_SBF_DEMOTED		0x01
#define DLM_SBF_VALNOTVALID	0x02
#define DLM_SBF_ALTMODE		0x04

struct dlm_lksb {
	int 	 sb_status;
	__u32	 sb_lkid;
	char 	 sb_flags;
	char *	 sb_lvbptr;
};

/*                           */

#define DLM_LSFL_TIMEWARN	0x00000002
#define DLM_LSFL_FS     	0x00000004
#define DLM_LSFL_NEWEXCL     	0x00000008


#endif /*                    */
