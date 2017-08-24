/* 
                     
                                   
 */

#ifndef __MT_CPUFREQ_H__
#define __MT_CPUFREQ_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __MT_CPUFREQ_C__
#define CPUFREQ_EXTERN
#else
#define CPUFREQ_EXTERN extern
#endif

/*                                                             */
//              
/*                                                             */

//                

//                 

//               

//                   


/*                                                             */
//                 
/*                                                             */


/*                                                             */
//                
/*                                                             */

enum mt_cpu_dvfs_id {
    //                        
    MT_CPU_DVFS_LITTLE,
    NR_MT_CPU_DVFS,
};

enum top_ckmuxsel {
    TOP_CKMUXSEL_CLKSQ   = 0, /*                  */
    TOP_CKMUXSEL_ARMPLL  = 1,
    TOP_CKMUXSEL_MAINPLL = 2,
    TOP_CKMUXSEL_UNIVPLL = 3,

    NR_TOP_CKMUXSEL,
} ;

/*
            
 */

/*       */
enum pmic_wrap_phase_id {
    PMIC_WRAP_PHASE_NORMAL,
    PMIC_WRAP_PHASE_SUSPEND,
    PMIC_WRAP_PHASE_DEEPIDLE,

    NR_PMIC_WRAP_PHASE,
};

/*             */
enum {
    IDX_NM_VSRAM,		/*   */ /*                        */
    IDX_NM_VPROC,		/*   */
    IDX_NM_VGPU,		/*   */
    IDX_NM_VCORE,		/*   */
    IDX_NM_VLTE,		/*   */

    NR_IDX_NM,
};
enum {
    IDX_SP_VPROC_PWR_ON,        /*   */ /*                         */
    IDX_SP_VPROC_SHUTDOWN,      /*   */
    IDX_SP_VSRAM_PWR_ON,        /*   */ 
    IDX_SP_VSRAM_SHUTDOWN,      /*   */
    IDX_SP_VCORE_NORMAL,        /*   */
    IDX_SP_VCORE_SLEEP,         /*   */
    IDX_SP_VCORE_TEMP1,         /*   */
    IDX_SP_VCORE_TEMP2,         /*   */

    NR_IDX_SP,
};
enum {
    IDX_DI_VCORE_TEMP1,		/*   */ /*                          */
    IDX_DI_VCORE_TEMP2, 	/*   */
    
    IDX_DI_VSRAM_NORMAL,	/*   */
    IDX_DI_VSRAM_SLEEP, 	/*   */
    IDX_DI_VPROC_NORMAL = IDX_DI_VSRAM_NORMAL,  /*   */
    IDX_DI_VPROC_SLEEP = IDX_DI_VSRAM_SLEEP,    /*   */
    
    IDX_DI_VCORE_NORMAL,	/*   */
    IDX_DI_VCORE_SLEEP,		/*   */
    IDX_DI_VSRAM_FAST_TRSN_DIS, /*   */
    IDX_DI_VSRAM_FAST_TRSN_EN,  /*   */

    NR_IDX_DI,
};


typedef void (*cpuVoltsampler_func)(enum mt_cpu_dvfs_id , unsigned int mv);
/*                                                             */
//                           
/*                                                             */


/*                                                             */
//                           
/*                                                             */

/*           */
CPUFREQ_EXTERN void mt_cpufreq_set_pmic_phase(enum pmic_wrap_phase_id phase);
CPUFREQ_EXTERN void mt_cpufreq_set_pmic_cmd(enum pmic_wrap_phase_id phase, int idx, unsigned int cmd_wdata);
CPUFREQ_EXTERN void mt_cpufreq_apply_pmic_cmd(int idx);

/*        */
CPUFREQ_EXTERN unsigned int mt_cpufreq_get_freq_by_idx(enum mt_cpu_dvfs_id id, int idx);
CPUFREQ_EXTERN int mt_cpufreq_update_volt(enum mt_cpu_dvfs_id id, unsigned int *volt_tbl, int nr_volt_tbl);
CPUFREQ_EXTERN void mt_cpufreq_restore_default_volt(enum mt_cpu_dvfs_id id);
CPUFREQ_EXTERN unsigned int mt_cpufreq_get_cur_volt(enum mt_cpu_dvfs_id id);
CPUFREQ_EXTERN void mt_cpufreq_enable_by_ptpod(enum mt_cpu_dvfs_id id);
CPUFREQ_EXTERN unsigned int mt_cpufreq_disable_by_ptpod(enum mt_cpu_dvfs_id id);

/*         */
CPUFREQ_EXTERN void mt_cpufreq_thermal_protect(unsigned int limited_power);

/*      */
CPUFREQ_EXTERN void mt_vcore_dvfs_disable_by_sdio(unsigned int type, bool disabled);
CPUFREQ_EXTERN void mt_vcore_dvfs_volt_set_by_sdio(unsigned int volt);
CPUFREQ_EXTERN unsigned int mt_vcore_dvfs_volt_get_by_sdio(void);

CPUFREQ_EXTERN unsigned int mt_get_cur_volt_vcore_ao(void);
//                                                            

/*         */
CPUFREQ_EXTERN int mt_cpufreq_state_set(int enabled);
CPUFREQ_EXTERN int mt_cpufreq_clock_switch(enum mt_cpu_dvfs_id id, enum top_ckmuxsel sel);
CPUFREQ_EXTERN enum top_ckmuxsel mt_cpufreq_get_clock_switch(enum mt_cpu_dvfs_id id);
CPUFREQ_EXTERN void mt_cpufreq_setvolt_registerCB(cpuVoltsampler_func pCB);
CPUFREQ_EXTERN bool mt_cpufreq_earlysuspend_status_get(void);

CPUFREQ_EXTERN void mt_cpufreq_set_ramp_down_count_const(enum mt_cpu_dvfs_id id, int count);

#ifndef __KERNEL__
CPUFREQ_EXTERN int mt_cpufreq_pdrv_probe(void);
CPUFREQ_EXTERN int mt_cpufreq_set_opp_volt(enum mt_cpu_dvfs_id id, int idx);
CPUFREQ_EXTERN int mt_cpufreq_set_freq(enum mt_cpu_dvfs_id id, int idx);
CPUFREQ_EXTERN unsigned int dvfs_get_cpu_freq(enum mt_cpu_dvfs_id id);
CPUFREQ_EXTERN void dvfs_set_cpu_freq_FH(enum mt_cpu_dvfs_id id, int freq);
CPUFREQ_EXTERN unsigned int cpu_frequency_output_slt(enum mt_cpu_dvfs_id id);
CPUFREQ_EXTERN void dvfs_set_cpu_volt(enum mt_cpu_dvfs_id id, int volt);
CPUFREQ_EXTERN void dvfs_set_gpu_volt(int pmic_val);
CPUFREQ_EXTERN void dvfs_set_vcore_ao_volt(int pmic_val);
//                                                          
CPUFREQ_EXTERN void dvfs_disable_by_ptpod(void);
CPUFREQ_EXTERN void dvfs_enable_by_ptpod(void);
#endif /*              */

#undef CPUFREQ_EXTERN

#ifdef __cplusplus
}
#endif

#endif //                 
