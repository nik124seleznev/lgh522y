#ifndef _UAPI_METAG_PTRACE_H
#define _UAPI_METAG_PTRACE_H

#ifndef __ASSEMBLY__

/*
                                                                             
 */

/*                      */

/*                                                     */
#define USER_GP_REGS_STATUS_CATCH_BIT		(1 << 22)
#define USER_GP_REGS_STATUS_CATCH_S		22
/*                                           */
#define USER_GP_REGS_STATUS_LSM_STEP_BITS	(0x7 << 8)
#define USER_GP_REGS_STATUS_LSM_STEP_S		8
/*                                                 */
#define USER_GP_REGS_STATUS_SCC_BIT		(1 << 4)
#define USER_GP_REGS_STATUS_SCC_S		4

/*                        */
/*                      */
#define USER_GP_REGS_STATUS_CF_Z_BIT		(1 << 3)
#define USER_GP_REGS_STATUS_CF_Z_S		3
/*                          */
#define USER_GP_REGS_STATUS_CF_N_BIT		(1 << 2)
#define USER_GP_REGS_STATUS_CF_N_S		2
/*                          */
#define USER_GP_REGS_STATUS_CF_V_BIT		(1 << 1)
#define USER_GP_REGS_STATUS_CF_V_S		1
/*                       */
#define USER_GP_REGS_STATUS_CF_C_BIT		(1 << 0)
#define USER_GP_REGS_STATUS_CF_C_S		0

/*                             */
/*                            */
#define USER_GP_REGS_STATUS_SCF_LZ_BIT		(1 << 3)
#define USER_GP_REGS_STATUS_SCF_LZ_S		3
/*                             */
#define USER_GP_REGS_STATUS_SCF_HZ_BIT		(1 << 2)
#define USER_GP_REGS_STATUS_SCF_HZ_S		2
/*                              */
#define USER_GP_REGS_STATUS_SCF_HC_BIT		(1 << 1)
#define USER_GP_REGS_STATUS_SCF_HC_S		1
/*                             */
#define USER_GP_REGS_STATUS_SCF_LC_BIT		(1 << 0)
#define USER_GP_REGS_STATUS_SCF_LC_S		0

/* 
                                                       
                                                                  
                                                                     
                    
                                                             
                                                
                                                                
                         
                                                                  
  
                                                                     
  
                                                                
  
                                         
 */
struct user_gp_regs {
	unsigned long dx[8][2];
	unsigned long ax[4][2];
	unsigned long pc;
	unsigned long status;
	unsigned long rpt;
	unsigned long bpobits;
	unsigned long mode;
	unsigned long _pad1;
};

/* 
                                                    
                                          
                                           
                                                                  
  
                                                                            
                                                                          
                         
  
                                                                  
 */
struct user_cb_regs {
	unsigned long flags;
	unsigned long addr;
	unsigned long long data;
};

/* 
                                                  
                                  
                                                                         
  
                                                                                
                                                                
  
                                                                   
 */
struct user_rp_state {
	unsigned long long entries[6];
	unsigned long mask;
};

#endif /*              */

#endif /*                      */
