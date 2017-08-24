/*
 * Copyright (C) 2001-2003 Hewlett-Packard Co
 *               Stephane Eranian <eranian@hpl.hp.com>
 */

#ifndef _UAPI_ASM_IA64_PERFMON_H
#define _UAPI_ASM_IA64_PERFMON_H

/*
                                               
 */
#define PFM_WRITE_PMCS		0x01
#define PFM_WRITE_PMDS		0x02
#define PFM_READ_PMDS		0x03
#define PFM_STOP		0x04
#define PFM_START		0x05
#define PFM_ENABLE		0x06 /*          */
#define PFM_DISABLE		0x07 /*          */
#define PFM_CREATE_CONTEXT	0x08
#define PFM_DESTROY_CONTEXT	0x09 /*                      */
#define PFM_RESTART		0x0a
#define PFM_PROTECT_CONTEXT	0x0b /*          */
#define PFM_GET_FEATURES	0x0c
#define PFM_DEBUG		0x0d
#define PFM_UNPROTECT_CONTEXT	0x0e /*          */
#define PFM_GET_PMC_RESET_VAL	0x0f
#define PFM_LOAD_CONTEXT	0x10
#define PFM_UNLOAD_CONTEXT	0x11

/*
                                                                       
 */
#define PFM_WRITE_IBRS		0x20
#define PFM_WRITE_DBRS		0x21

/*
                
 */
#define PFM_FL_NOTIFY_BLOCK    	 0x01	/*                                        */
#define PFM_FL_SYSTEM_WIDE	 0x02	/*                              */
#define PFM_FL_OVFL_NO_MSG	 0x80   /*                                                    */

/*
                  
 */
#define PFM_SETFL_EXCL_IDLE      0x01   /*                                                      */

/*
            
 */
#define PFM_REGFL_OVFL_NOTIFY	0x1	/*                               */
#define PFM_REGFL_RANDOM	0x2	/*                               */

/*
                                                  
  
                                                                                
                                                                      
 */
#define PFM_REG_RETFL_NOTAVAIL	(1UL<<31) /*                                                  */
#define PFM_REG_RETFL_EINVAL	(1UL<<30) /*                                  */
#define PFM_REG_RETFL_MASK	(PFM_REG_RETFL_NOTAVAIL|PFM_REG_RETFL_EINVAL)

#define PFM_REG_HAS_ERROR(flag)	(((flag) & PFM_REG_RETFL_MASK) != 0)

typedef unsigned char pfm_uuid_t[16];	/*                                        */

/*
                                             
 */
typedef struct {
	pfm_uuid_t     ctx_smpl_buf_id;	 /*                                        */
	unsigned long  ctx_flags;	 /*               */
	unsigned short ctx_nextra_sets;	 /*                                               */
	unsigned short ctx_reserved1;	 /*                */
	int	       ctx_fd;		 /*                                               */
	void	       *ctx_smpl_vaddr;	 /*                                                         */
	unsigned long  ctx_reserved2[11];/*                */
} pfarg_context_t;

/*
                                                    
 */
typedef struct {
	unsigned int	reg_num;	   /*                */
	unsigned short	reg_set;	   /*                             */
	unsigned short	reg_reserved1;	   /*                */

	unsigned long	reg_value;	   /*                       */
	unsigned long	reg_flags;	   /*                                         */

	unsigned long	reg_long_reset;	   /*                                          */
	unsigned long	reg_short_reset;   /*                              */

	unsigned long	reg_reset_pmds[4]; /*                                           */
	unsigned long	reg_random_seed;   /*                                       */
	unsigned long	reg_random_mask;   /*                                    */
	unsigned long   reg_last_reset_val;/*                              */

	unsigned long	reg_smpl_pmds[4];  /*                                            */
	unsigned long	reg_smpl_eventid;  /*                                  */

	unsigned long   reg_reserved2[3];   /*                */
} pfarg_reg_t;

typedef struct {
	unsigned int	dbreg_num;		/*                      */
	unsigned short	dbreg_set;		/*                             */
	unsigned short	dbreg_reserved1;	/*                */
	unsigned long	dbreg_value;		/*                          */
	unsigned long	dbreg_flags;		/*                     */
	unsigned long	dbreg_reserved2[1];	/*                */
} pfarg_dbreg_t;

typedef struct {
	unsigned int	ft_version;	/*                                      */
	unsigned int	ft_reserved;	/*                         */
	unsigned long	reserved[4];	/*                */
} pfarg_features_t;

typedef struct {
	pid_t		load_pid;	   /*                                  */
	unsigned short	load_set;	   /*                         */
	unsigned short	load_reserved1;	   /*                */
	unsigned long	load_reserved2[3]; /*                */
} pfarg_load_t;

typedef struct {
	int		msg_type;		/*                        */
	int		msg_ctx_fd;		/*                        */
	unsigned long	msg_ovfl_pmds[4];	/*                       */
	unsigned short  msg_active_set;		/*                                    */
	unsigned short  msg_reserved1;		/*                */
	unsigned int    msg_reserved2;		/*                */
	unsigned long	msg_tstamp;		/*                       */
} pfm_ovfl_msg_t;

typedef struct {
	int		msg_type;		/*                        */
	int		msg_ctx_fd;		/*                        */
	unsigned long	msg_tstamp;		/*                 */
} pfm_end_msg_t;

typedef struct {
	int		msg_type;		/*                     */
	int		msg_ctx_fd;		/*                                   */
	unsigned long	msg_tstamp;		/*                 */
} pfm_gen_msg_t;

#define PFM_MSG_OVFL	1	/*                      */
#define PFM_MSG_END	2	/*                                          */

typedef union {
	pfm_ovfl_msg_t	pfm_ovfl_msg;
	pfm_end_msg_t	pfm_end_msg;
	pfm_gen_msg_t	pfm_gen_msg;
} pfm_msg_t;

/*
                                                                                         
 */
#define PFM_VERSION_MAJ		 2U
#define PFM_VERSION_MIN		 0U
#define PFM_VERSION		 (((PFM_VERSION_MAJ&0xffff)<<16)|(PFM_VERSION_MIN & 0xffff))
#define PFM_VERSION_MAJOR(x)	 (((x)>>16) & 0xffff)
#define PFM_VERSION_MINOR(x)	 ((x) & 0xffff)


/*
                                        
 */
#define PMU_FIRST_COUNTER	4	/*                                  */
#define PMU_MAX_PMCS		256	/*                                             */
#define PMU_MAX_PMDS		256	/*                                             */


#endif /*                          */
