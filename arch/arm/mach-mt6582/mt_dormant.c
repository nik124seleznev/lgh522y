/*                                
         
                                 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <asm/system.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_dormant.h>
#include <mach/mt_spm.h>
#include <mach/mt_irq.h>
#include <mach/sync_write.h>

#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT)
#include <mach/mt_secure_api.h>
#endif

/*                                
       
                                 */
#define BOOTROM_PWR_CTRL        (INFRACFG_AO_BASE + 0x804)
#define BOOTROM_BOOT_ADDR       (INFRACFG_AO_BASE + 0x800)

#define CA7_CACHE_CONFIG        (MCUSYS_CFGREG_BASE + 0x0000)

#define reg_read(addr)          (*(volatile u32 *)(addr))
#define reg_write(addr, val)    mt65xx_reg_sync_writel(val, addr)

/*            */
#define HYP_TIMER_MULT  0xa	/*                                                             */
#define LCL_TIMER_FREQ  0x7f	/*                                     */
#define HYP_TIMER_IRQ   0x1a
#define LCL_TIMER_IRQ   0x1e
#define TIMER_ENABLE    0x1
#define TIMER_DISABLE   0x0
#define TIMER_MASK_IRQ  0x2
#define TIMER_IRQ_STAT  0x4

/*              */
#define	PMU_STATE0 0
#define	PMU_STATE1 1
#define	PMU_STATE2 2

#define MAX_CLUSTERS    2
#define MAX_CORES       8
#define MAX_CPUIFS      8
#define STACK_SIZE      96

#define REGS 32
#define PMCR_IDX       0
#define PMSELR_IDX     1
#define PMCNTENSET_IDX 2
#define PMCNTENCLR_IDX 3
#define PMCCNTR_IDX    4
#define PMOVSR_IDX     5
#define PMINTENSET_IDX 6
#define PMINTENCLR_IDX 7
#define PMXEVTYPE0_IDX 8
#define PMXEVCNT0_IDX  9
#define PMXEVTYPE1_IDX 10
#define PMXEVCNT1_IDX  11
#define PMXEVTYPE2_IDX 12
#define PMXEVCNT2_IDX  13
#define PMXEVTYPE3_IDX 14
#define PMXEVCNT3_IDX  15
unsigned int clusters_ctx[MAX_CLUSTERS][MAX_CORES][REGS];
unsigned int migration_ctx[MAX_CORES][REGS];
/*
                         
 */
static int pmu_mode = PMU_STATE0;

#define DIDR_VERSION_SHIFT 16
#define DIDR_VERSION_MASK  0xF
#define DIDR_VERSION_7_1   5
#define DIDR_BP_SHIFT      24
#define DIDR_BP_MASK       0xF
#define DIDR_WP_SHIFT      28
#define DIDR_WP_MASK       0xF
#define CLAIMCLR_CLEAR_ALL 0xff

#define DRAR_VALID_MASK   0x00000003
#define DSAR_VALID_MASK   0x00000003
#define DRAR_ADDRESS_MASK 0xFFFFF000
#define DSAR_ADDRESS_MASK 0xFFFFF000
#define OSLSR_OSLM_MASK   0x00000009
#define OSLAR_UNLOCKED    0x00000000
#define OSLAR_LOCKED      0xC5ACCE55
#define LAR_UNLOCKED      0xC5ACCE55
#define LAR_LOCKED        0x00000000
#define OSDLR_UNLOCKED    0x00000000
#define OSDLR_LOCKED      0x00000001

#define DBGREG_BP_VAL     0x0
#define DBGREG_WP_VAL     0x1
#define DBGREG_BP_CTRL    0x2
#define DBGREG_WP_CTRL    0x3
#define DBGREG_BP_XVAL    0x4

#if 0
/*                  */
#define MCU_BIU_CONTROL        (MCU_BIU_BASE)
#endif

/*                                
               
                                 */
#define CPU_DORMANT_LOG_NONE                                0
#define CPU_DORMANT_LOG_WITH_XLOG                           1
#define CPU_DORMANT_LOG_WITH_PRINTK                         2

#define CPU_DORMANT_LOG_PRINT                               CPU_DORMANT_LOG_WITH_PRINTK

#if (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_NONE)
#define CPU_DORMANT_INFO(fmt, args...)                    
#elif (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_WITH_XLOG)
#define CPU_DORMANT_INFO(fmt, args...)                      xlog_printk(ANDROID_LOG_INFO, "Power/cpu_dormant", fmt, ##args)
#elif (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_WITH_PRINTK)
#define CPU_DORMANT_INFO(fmt, args...)                      printk("[Power/cpu_dormant] "fmt, ##args)
#endif

/*                                
        
                                 */
typedef struct {
        unsigned vcr;
        unsigned eacr;
        unsigned claim;
        unsigned claimclr;
        unsigned dtrrx_e;
        unsigned dtrtx_e;
        unsigned dscr_e;
        unsigned wfar;
        unsigned bvr[16];
        unsigned bcr[16];
        unsigned wvr[16];
        unsigned wcr[16];
        unsigned bxvr[16];
} debug_context_t;              /*                               */

typedef struct ns_gic_cpu_context {
    unsigned int gic_cpu_if_regs[32];   /*                              */
    unsigned int gic_dist_if_pvt_regs[32];  /*                                      */
} gic_cpu_context;

typedef struct fault_regs {
    unsigned dfar;
    unsigned ifar;
    unsigned ifsr;
    unsigned dfsr;
    unsigned adfsr;
    unsigned aifsr;
} cp15_fault_regs;

typedef struct ns_banked_cp15_context {
    unsigned int cp15_misc_regs[2]; /*                              */
    unsigned int cp15_ctrl_regs[20];    /*                        */
    unsigned int cp15_mmu_regs[16]; /*                    */
    cp15_fault_regs ns_cp15_fault_regs; /*                             */
} banked_cp15_context;

typedef struct gen_tmr_ctx {
    unsigned cntfrq;
    unsigned long long cntvoff;
    unsigned cnthctl;
    unsigned cntkctl;
    unsigned long long cntp_cval;
    unsigned cntp_tval;
    unsigned cntp_ctl;
    unsigned long long cntv_cval;
    unsigned cntv_tval;
    unsigned cntv_ctl;
    unsigned long long cnthp_cval;
    unsigned cnthp_tval;
    unsigned cnthp_ctl;
} generic_timer_context;

typedef struct ns_cpu_context {
    unsigned int banked_cpu_regs[32];   /*                      */
    banked_cp15_context banked_cp15_regs;   /*                             */
    generic_timer_context cp15_timer_ctx;   /*                                                    */
    gic_cpu_context gic_cpu_ctx;    /*                                               */
    unsigned int endianess; /*                   */
    unsigned int vfp_regs[34];  /*                              */
    //                                                          
    unsigned int  dbg_data[32]; /*                          */
} cpu_context;

typedef struct ns_global_context {
    unsigned int gic_dist_if_regs[512]; /*                                                      */
    unsigned int generic_timer_regs[8]; /*                                                  */
} global_context;

/*
                                                                                  
                   
 */
typedef struct os_state {
    unsigned sctlr;
    unsigned dacr;
    unsigned ttbr0;
    unsigned nmrr;
    unsigned prrr;
} os_state;

/*
                                                                             
                         
 */
typedef struct core_context {
    /*
                                   
     */
    cpu_context ns_cpu_ctx;

} core_context;

/*
                                                                            
                 
 */
typedef struct cluster_context {
    core_context core[MAX_CORES];
    unsigned num_cores;
    global_context ns_cluster_ctx;
} cluster_context;

/*
                                                                             
 */
typedef struct system_context {
    cluster_context cluster;
    unsigned num_clusters;
    unsigned warm_reset;
} system_context;

typedef volatile struct {	/*                                                           */
	unsigned const didr;	/*                                                         */
	unsigned dscr_i;	/*                                                         */
	unsigned const dummy1[3];	/*                                                         */
	unsigned dtrrx_dtrtx_i;	/*                                                         */
	unsigned wfar;		/*                                                         */
	unsigned vcr;		/*                                                         */
	unsigned const dummy2;	/*                                                         */
	unsigned ecr;		/*                                                         */
	unsigned dsccr;		/*                                                         */
	unsigned dsmcr;		/*                                                         */
	unsigned const dummy3[20];	/*                                                         */
	unsigned dtrrx_e;	/*                                                         */
	unsigned itr_pcsr;	/*                                                         */
	unsigned dscr_e;	/*                                                         */
	unsigned dtrtx_e;	/*                                                         */
	unsigned drcr;		/*                                                         */
	unsigned eacr;		/*                                                         */
	unsigned const dummy4[2];	/*                                                         */
	unsigned pcsr;		/*                                                         */
	unsigned cidsr;		/*                                                         */
	unsigned vidsr;		/*                                                         */
	unsigned const dummy5[21];	/*                                                         */
	unsigned bvr[16];	/*                                                         */
	unsigned bcr[16];	/*                                                         */
	unsigned wvr[16];	/*                                                         */
	unsigned wcr[16];	/*                                                         */
	unsigned const dummy6[16];	/*                                                         */
	unsigned bxvr[16];	/*                                                         */
	unsigned const dummy7[32];	/*                                                         */
	unsigned oslar;		/*                                                         */
	unsigned const oslsr;	/*                                                         */
	unsigned ossrr;		/*                                                         */
	unsigned const dummy8;	/*                                                         */
	unsigned prcr;		/*                                                         */
	unsigned prsr;		/*                                                         */
	unsigned const dummy9[762];	/*                                                         */
	unsigned itctrl;	/*                                                         */
	unsigned const dummy10[39];	/*                                                         */
	unsigned claimset;	/*                                                         */
	unsigned claimclr;	/*                                                         */
	unsigned const dummy11[2];	/*                                                         */
	unsigned lar;		/*                                                         */
	unsigned const lsr;	/*                                                         */
	unsigned const authstatus;	/*                                                         */
	unsigned const dummy12;	/*                                                         */
	unsigned const devid2;	/*                                                         */
	unsigned const devid1;	/*                                                         */
	unsigned const devid;	/*                                                         */
	unsigned const devtype;	/*                                                         */
	unsigned const pid[8];	/*                                                         */
	unsigned const cid[4];	/*                                                         */
} debug_registers_t;

typedef struct {
	unsigned (*read) (void);
	void (*write) (unsigned);
} rw_ops;

typedef struct {
	rw_ops bvr;
	rw_ops bcr;
	rw_ops wvr;
	rw_ops wcr;
	rw_ops bxvr;
} dbgreg_rw_ops;



struct set_and_clear_regs
{
    volatile unsigned int set[32], clear[32];
};

typedef struct
{
    volatile unsigned int control;                      /*       */
    const unsigned int controller_type;
    const unsigned int implementer;
    const char padding1[116];
    volatile unsigned int security[32];                 /*       */
    struct set_and_clear_regs enable;                   /*       */
    struct set_and_clear_regs pending;                  /*       */
    struct set_and_clear_regs active;                   /*       */
    volatile unsigned int priority[256];                /*       */
    volatile unsigned int target[256];                  /*       */
    volatile unsigned int configuration[64];            /*       */
    const char padding3[512];                           /*       */
    volatile unsigned int software_interrupt;           /*       */
    volatile unsigned int sgi_clr_pending[4];           /*       */
    volatile unsigned int sgi_set_pending[4];           /*       */
    const char padding4[176];
    unsigned const int peripheral_id[4];                /*       */
    unsigned const int primecell_id[4];                 /*       */
} interrupt_distributor;



typedef struct
{
    volatile unsigned int control;                      /*      */
    volatile unsigned int priority_mask;                /*      */
    volatile unsigned int binary_point;                 /*      */
    volatile unsigned const int interrupt_ack;          /*      */
    volatile unsigned int end_of_interrupt;             /*      */
    volatile unsigned const int running_priority;       /*      */
    volatile unsigned const int highest_pending;        /*      */
    volatile unsigned int aliased_binary_point;         /*      */
    volatile unsigned const int aliased_interrupt_ack;  /*      */
    volatile unsigned int alias_end_of_interrupt;       /*      */
    volatile unsigned const int alias_highest_pending;  /*      */
} cpu_interface;

#if 0
typedef struct
{
    unsigned int mcu_biu_control; /*                          */
} mcu_biu_reg;
#endif

/*                                
        
                                 */
extern unsigned *copy_words(volatile unsigned *destination, volatile unsigned *source, unsigned num_words);

extern void cpu_wake_up(void);

extern void save_control_registers(unsigned *pointer, int is_secure);
extern void save_mmu(unsigned *pointer);
extern void save_mpu(unsigned *pointer);
extern void save_performance_monitors(unsigned *pointer);
extern void save_banked_registers(unsigned *pointer);
extern void save_cp15(unsigned *pointer);
extern void save_vfp(unsigned *pointer);
extern void save_generic_timer(unsigned *pointer, int is_hyp);
//                                             
extern void save_fault_status(unsigned *pointer);

extern void restore_control_registers(unsigned *pointer, int is_secure);
extern void mt_restore_control_registers(unsigned *pointer, int is_secure);
extern void restore_mmu(unsigned *pointer);
extern void restore_mpu(unsigned *pointer);
extern void restore_performance_monitors(unsigned *pointer);
extern void restore_banked_registers(unsigned *pointer);
extern void restore_cp15(unsigned *pointer);
extern void restore_vfp(unsigned *pointer);
extern void restore_generic_timer(unsigned *pointer, int is_hyp);
//                                                
extern void restore_fault_status(unsigned *pointer);

extern void write_cntp_ctl(unsigned);
extern void write_cntv_ctl(unsigned);
extern unsigned read_cpuid(void);
extern unsigned read_clusterid(void);
extern unsigned read_nsacr(void);
extern unsigned read_id_pfr1(void);

extern unsigned read_dbg_osdlr(void);
extern unsigned read_dbg_drar(void);
extern unsigned read_dbg_dsar(void);
extern unsigned read_dbg_devid(void);
extern unsigned read_dbg_didr(void);
extern unsigned read_dbg_dtrrxext(void);
extern unsigned read_dbg_dtrtxext(void);
extern unsigned read_dbg_dscrext(void);
extern unsigned read_dbg_wfar(void);
extern unsigned read_dbg_vcr(void);
extern unsigned read_dbg_claimclr(void);
extern unsigned read_dbg_bvr0(void);
extern unsigned read_dbg_bvr1(void);
extern unsigned read_dbg_bvr2(void);
extern unsigned read_dbg_bvr3(void);
extern unsigned read_dbg_bvr4(void);
extern unsigned read_dbg_bvr5(void);
extern unsigned read_dbg_bvr6(void);
extern unsigned read_dbg_bvr7(void);
extern unsigned read_dbg_bvr8(void);
extern unsigned read_dbg_bvr9(void);
extern unsigned read_dbg_bvr10(void);
extern unsigned read_dbg_bvr11(void);
extern unsigned read_dbg_bvr12(void);
extern unsigned read_dbg_bvr13(void);
extern unsigned read_dbg_bvr14(void);
extern unsigned read_dbg_bvr15(void);
extern unsigned read_dbg_bcr0(void);
extern unsigned read_dbg_bcr1(void);
extern unsigned read_dbg_bcr2(void);
extern unsigned read_dbg_bcr3(void);
extern unsigned read_dbg_bcr4(void);
extern unsigned read_dbg_bcr5(void);
extern unsigned read_dbg_bcr6(void);
extern unsigned read_dbg_bcr7(void);
extern unsigned read_dbg_bcr8(void);
extern unsigned read_dbg_bcr9(void);
extern unsigned read_dbg_bcr10(void);
extern unsigned read_dbg_bcr11(void);
extern unsigned read_dbg_bcr12(void);
extern unsigned read_dbg_bcr13(void);
extern unsigned read_dbg_bcr14(void);
extern unsigned read_dbg_bcr15(void);
extern unsigned read_dbg_wvr0(void);
extern unsigned read_dbg_wvr1(void);
extern unsigned read_dbg_wvr2(void);
extern unsigned read_dbg_wvr3(void);
extern unsigned read_dbg_wvr4(void);
extern unsigned read_dbg_wvr5(void);
extern unsigned read_dbg_wvr6(void);
extern unsigned read_dbg_wvr7(void);
extern unsigned read_dbg_wvr8(void);
extern unsigned read_dbg_wvr9(void);
extern unsigned read_dbg_wvr10(void);
extern unsigned read_dbg_wvr11(void);
extern unsigned read_dbg_wvr12(void);
extern unsigned read_dbg_wvr13(void);
extern unsigned read_dbg_wvr14(void);
extern unsigned read_dbg_wvr15(void);
extern unsigned read_dbg_wcr0(void);
extern unsigned read_dbg_wcr1(void);
extern unsigned read_dbg_wcr2(void);
extern unsigned read_dbg_wcr3(void);
extern unsigned read_dbg_wcr4(void);
extern unsigned read_dbg_wcr5(void);
extern unsigned read_dbg_wcr6(void);
extern unsigned read_dbg_wcr7(void);
extern unsigned read_dbg_wcr8(void);
extern unsigned read_dbg_wcr9(void);
extern unsigned read_dbg_wcr10(void);
extern unsigned read_dbg_wcr11(void);
extern unsigned read_dbg_wcr12(void);
extern unsigned read_dbg_wcr13(void);
extern unsigned read_dbg_wcr14(void);
extern unsigned read_dbg_wcr15(void);
extern unsigned read_dbg_bxvr0(void);
extern unsigned read_dbg_bxvr1(void);
extern unsigned read_dbg_bxvr2(void);
extern unsigned read_dbg_bxvr3(void);
extern unsigned read_dbg_bxvr4(void);
extern unsigned read_dbg_bxvr5(void);
extern unsigned read_dbg_bxvr6(void);
extern unsigned read_dbg_bxvr7(void);
extern unsigned read_dbg_bxvr8(void);
extern unsigned read_dbg_bxvr9(void);
extern unsigned read_dbg_bxvr10(void);
extern unsigned read_dbg_bxvr11(void);
extern unsigned read_dbg_bxvr12(void);
extern unsigned read_dbg_bxvr13(void);
extern unsigned read_dbg_bxvr14(void);
extern unsigned read_dbg_bxvr15(void);

extern void write_dbg_osdlr(unsigned);
extern void write_dbg_oslar(unsigned);
extern void write_dbg_dtrrxext(unsigned);
extern void write_dbg_dtrtxext(unsigned);
extern void write_dbg_dscrext(unsigned);
extern void write_dbg_wfar(unsigned);
extern void write_dbg_vcr(unsigned);
extern void write_dbg_claimset(unsigned);
extern void write_dbg_bvr0(unsigned);
extern void write_dbg_bvr1(unsigned);
extern void write_dbg_bvr2(unsigned);
extern void write_dbg_bvr3(unsigned);
extern void write_dbg_bvr4(unsigned);
extern void write_dbg_bvr5(unsigned);
extern void write_dbg_bvr6(unsigned);
extern void write_dbg_bvr7(unsigned);
extern void write_dbg_bvr8(unsigned);
extern void write_dbg_bvr9(unsigned);
extern void write_dbg_bvr10(unsigned);
extern void write_dbg_bvr11(unsigned);
extern void write_dbg_bvr12(unsigned);
extern void write_dbg_bvr13(unsigned);
extern void write_dbg_bvr14(unsigned);
extern void write_dbg_bvr15(unsigned);
extern void write_dbg_bcr0(unsigned);
extern void write_dbg_bcr1(unsigned);
extern void write_dbg_bcr2(unsigned);
extern void write_dbg_bcr3(unsigned);
extern void write_dbg_bcr4(unsigned);
extern void write_dbg_bcr5(unsigned);
extern void write_dbg_bcr6(unsigned);
extern void write_dbg_bcr7(unsigned);
extern void write_dbg_bcr8(unsigned);
extern void write_dbg_bcr9(unsigned);
extern void write_dbg_bcr10(unsigned);
extern void write_dbg_bcr11(unsigned);
extern void write_dbg_bcr12(unsigned);
extern void write_dbg_bcr13(unsigned);
extern void write_dbg_bcr14(unsigned);
extern void write_dbg_bcr15(unsigned);
extern void write_dbg_wvr0(unsigned);
extern void write_dbg_wvr1(unsigned);
extern void write_dbg_wvr2(unsigned);
extern void write_dbg_wvr3(unsigned);
extern void write_dbg_wvr4(unsigned);
extern void write_dbg_wvr5(unsigned);
extern void write_dbg_wvr6(unsigned);
extern void write_dbg_wvr7(unsigned);
extern void write_dbg_wvr8(unsigned);
extern void write_dbg_wvr9(unsigned);
extern void write_dbg_wvr10(unsigned);
extern void write_dbg_wvr11(unsigned);
extern void write_dbg_wvr12(unsigned);
extern void write_dbg_wvr13(unsigned);
extern void write_dbg_wvr14(unsigned);
extern void write_dbg_wvr15(unsigned);
extern void write_dbg_wcr0(unsigned);
extern void write_dbg_wcr1(unsigned);
extern void write_dbg_wcr2(unsigned);
extern void write_dbg_wcr3(unsigned);
extern void write_dbg_wcr4(unsigned);
extern void write_dbg_wcr5(unsigned);
extern void write_dbg_wcr6(unsigned);
extern void write_dbg_wcr7(unsigned);
extern void write_dbg_wcr8(unsigned);
extern void write_dbg_wcr9(unsigned);
extern void write_dbg_wcr10(unsigned);
extern void write_dbg_wcr11(unsigned);
extern void write_dbg_wcr12(unsigned);
extern void write_dbg_wcr13(unsigned);
extern void write_dbg_wcr14(unsigned);
extern void write_dbg_wcr15(unsigned);
extern void write_dbg_bxvr0(unsigned);
extern void write_dbg_bxvr1(unsigned);
extern void write_dbg_bxvr2(unsigned);
extern void write_dbg_bxvr3(unsigned);
extern void write_dbg_bxvr4(unsigned);
extern void write_dbg_bxvr5(unsigned);
extern void write_dbg_bxvr6(unsigned);
extern void write_dbg_bxvr7(unsigned);
extern void write_dbg_bxvr8(unsigned);
extern void write_dbg_bxvr9(unsigned);
extern void write_dbg_bxvr10(unsigned);
extern void write_dbg_bxvr11(unsigned);
extern void write_dbg_bxvr12(unsigned);
extern void write_dbg_bxvr13(unsigned);
extern void write_dbg_bxvr14(unsigned);
extern void write_dbg_bxvr15(unsigned);

extern void __enable_cache(void);
extern void __disable_cache(void);
extern void __disable_dcache(void);

extern void inner_dcache_flush_L1(void);
extern void inner_dcache_flush_all(void);

extern void invalidate_unified_TLB_inner_shareable(void);

extern void __inner_inv_dcache_L1(void);
extern void __inner_inv_dcache_L2(void);

extern void __inner_clean_dcache_L1(void);
extern void __inner_clean_dcache_L2(void);
extern void __inner_clean_dcache_all(void);

extern void trace_stop_dormant(void);
extern void trace_start_dormant(void);

//                                          
extern void save_dbg_regs(unsigned int data[]);
extern void restore_dbg_regs(unsigned int data[]);

/*                                
                 
                                 */
volatile static int dormant_ret_flag[4] = {0,0,0,0};
volatile int power_state[4] = {STATUS_RUN,STATUS_RUN,STATUS_RUN,STATUS_RUN};
#if 0
mcu_biu_reg mcu_biu;
#endif

/*
                                                                   
                    
 */
system_context switcher_context;

dbgreg_rw_ops dbgreg_rw_handlers[] = {
	{
	 {read_dbg_bvr0, write_dbg_bvr0,},
	 {read_dbg_bcr0, write_dbg_bcr0,},
	 {read_dbg_wvr0, write_dbg_wvr0,},
	 {read_dbg_wcr0, write_dbg_wcr0,},
	 {read_dbg_bxvr0, write_dbg_bxvr0,},
	 },
	{
	 {read_dbg_bvr1, write_dbg_bvr1,},
	 {read_dbg_bcr1, write_dbg_bcr1,},
	 {read_dbg_wvr1, write_dbg_wvr1,},
	 {read_dbg_wcr1, write_dbg_wcr1,},
	 {read_dbg_bxvr1, write_dbg_bxvr1,},
	 },
	{
	 {read_dbg_bvr2, write_dbg_bvr2,},
	 {read_dbg_bcr2, write_dbg_bcr2,},
	 {read_dbg_wvr2, write_dbg_wvr2,},
	 {read_dbg_wcr2, write_dbg_wcr2,},
	 {read_dbg_bxvr2, write_dbg_bxvr2,},
	 },
	{
	 {read_dbg_bvr3, write_dbg_bvr3,},
	 {read_dbg_bcr3, write_dbg_bcr3,},
	 {read_dbg_wvr3, write_dbg_wvr3,},
	 {read_dbg_wcr3, write_dbg_wcr3,},
	 {read_dbg_bxvr3, write_dbg_bxvr3,},
	 },
	{
	 {read_dbg_bvr4, write_dbg_bvr4,},
	 {read_dbg_bcr4, write_dbg_bcr4,},
	 {read_dbg_wvr4, write_dbg_wvr4,},
	 {read_dbg_wcr4, write_dbg_wcr4,},
	 {read_dbg_bxvr4, write_dbg_bxvr4,},
	 },
	{
	 {read_dbg_bvr5, write_dbg_bvr5,},
	 {read_dbg_bcr5, write_dbg_bcr5,},
	 {read_dbg_wvr5, write_dbg_wvr5,},
	 {read_dbg_wcr5, write_dbg_wcr5,},
	 {read_dbg_bxvr5, write_dbg_bxvr5,},
	 },
	{
	 {read_dbg_bvr6, write_dbg_bvr6,},
	 {read_dbg_bcr6, write_dbg_bcr6,},
	 {read_dbg_wvr6, write_dbg_wvr6,},
	 {read_dbg_wcr6, write_dbg_wcr6,},
	 {read_dbg_bxvr6, write_dbg_bxvr6,},
	 },
	{
	 {read_dbg_bvr7, write_dbg_bvr7,},
	 {read_dbg_bcr7, write_dbg_bcr7,},
	 {read_dbg_wvr7, write_dbg_wvr7,},
	 {read_dbg_wcr7, write_dbg_wcr7,},
	 {read_dbg_bxvr7, write_dbg_bxvr7,},
	 },
	{
	 {read_dbg_bvr8, write_dbg_bvr8,},
	 {read_dbg_bcr8, write_dbg_bcr8,},
	 {read_dbg_wvr8, write_dbg_wvr8,},
	 {read_dbg_wcr8, write_dbg_wcr8,},
	 {read_dbg_bxvr8, write_dbg_bxvr8,},
	 },
	{
	 {read_dbg_bvr9, write_dbg_bvr9,},
	 {read_dbg_bcr9, write_dbg_bcr9,},
	 {read_dbg_wvr9, write_dbg_wvr9,},
	 {read_dbg_wcr9, write_dbg_wcr9,},
	 {read_dbg_bxvr9, write_dbg_bxvr9,},
	 },
	{
	 {read_dbg_bvr10, write_dbg_bvr10,},
	 {read_dbg_bcr10, write_dbg_bcr10,},
	 {read_dbg_wvr10, write_dbg_wvr10,},
	 {read_dbg_wcr10, write_dbg_wcr10,},
	 {read_dbg_bxvr10, write_dbg_bxvr10,},
	 },
	{
	 {read_dbg_bvr11, write_dbg_bvr11,},
	 {read_dbg_bcr11, write_dbg_bcr11,},
	 {read_dbg_wvr11, write_dbg_wvr11,},
	 {read_dbg_wcr11, write_dbg_wcr11,},
	 {read_dbg_bxvr11, write_dbg_bxvr11,},
	 },
	{
	 {read_dbg_bvr12, write_dbg_bvr12,},
	 {read_dbg_bcr12, write_dbg_bcr12,},
	 {read_dbg_wvr12, write_dbg_wvr12,},
	 {read_dbg_wcr12, write_dbg_wcr12,},
	 {read_dbg_bxvr12, write_dbg_bxvr12,},
	 },
	{
	 {read_dbg_bvr13, write_dbg_bvr13,},
	 {read_dbg_bcr13, write_dbg_bcr13,},
	 {read_dbg_wvr13, write_dbg_wvr13,},
	 {read_dbg_wcr13, write_dbg_wcr13,},
	 {read_dbg_bxvr13, write_dbg_bxvr13,},
	 },
	{
	 {read_dbg_bvr14, write_dbg_bvr14,},
	 {read_dbg_bcr14, write_dbg_bcr14,},
	 {read_dbg_wvr14, write_dbg_wvr14,},
	 {read_dbg_wcr14, write_dbg_wcr14,},
	 {read_dbg_bxvr14, write_dbg_bxvr14,},
	 },
	{
	 {read_dbg_bvr15, write_dbg_bvr15,},
	 {read_dbg_bcr15, write_dbg_bcr15,},
	 {read_dbg_wvr15, write_dbg_wvr15,},
	 {read_dbg_wcr15, write_dbg_wcr15,},
	 {read_dbg_bxvr15, write_dbg_bxvr15,},
	 },
};


/*                                
          
                                 */
#if 0
/*
                                     
                             
 */
static void save_mcu_biu_register(void)
{
    mcu_biu.mcu_biu_control = reg_read(MCU_BIU_CONTROL);
}

static void restore_mcu_biu_register(void)
{
    reg_write(MCU_BIU_CONTROL, mcu_biu.mcu_biu_control);
}
#endif

/*
                                      
                             
 */
static void save_gic_interface(u32 *pointer, unsigned gic_interface_address)
{
    cpu_interface *ci = (cpu_interface *)gic_interface_address;

    pointer[0] = ci->control;
    pointer[1] = ci->priority_mask;
    pointer[2] = ci->binary_point;
    pointer[3] = ci->aliased_binary_point;

    /*                           */

}

/*
                                                   
                                                                                          
                              
 */
static void save_gic_distributor_private(u32 *pointer, unsigned gic_distributor_address)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned int *ptr = 0x0;

    /*                            */
    *pointer = id->enable.set[0];
    ++pointer;
    /*                              */
    pointer = copy_words(pointer, id->priority, 8);
    /*                            */
    pointer = copy_words(pointer, id->target, 8);
    /*                                                               */
    *pointer = id->configuration[1];
    ++pointer;
    /*                              */
    *pointer = id->security[0];
    ++pointer;
#if 0
    /*
                                                           
                                                          
                                                           
                                                           
                                                          
                                 
                                                           
                                                      
     */
    /*                          */
    id->pending.clear[0] = 0xffffffff;
    id->active.clear[0] = 0xffffffff;
#endif
#if 1 
    /*                             */
    *pointer = id->pending.set[0];
    ++pointer;
#endif
    /*
                                                            
                                                    
     */
    ptr = pointer;
    copy_words(pointer, id->sgi_set_pending, 4);
    pointer += 8; 

    /*
                                                              
                                       
     */
    copy_words(id->sgi_clr_pending, ptr, 4);


    
}

/*
                                            
                                                                                     
                                                                                      
 */
static void save_gic_distributor_shared(u32 *pointer, unsigned gic_distributor_address)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned num_spis, *saved_pending;

    /*                                          */
    num_spis = 32 * (id->controller_type & 0x1f);

    /*                           */

    /*                                */
    if (num_spis) {
        pointer = copy_words(pointer, id->enable.set + 1, num_spis / 32);
        pointer = copy_words(pointer, id->priority + 8, num_spis / 4);
        pointer = copy_words(pointer, id->target + 8, num_spis / 4);
        pointer = copy_words(pointer, id->configuration + 2, num_spis / 16);
        pointer = copy_words(pointer, id->security + 1, num_spis / 32);
        saved_pending = pointer;
        pointer = copy_words(pointer, id->pending.set + 1, num_spis / 32);
    }

    /*                       */
    *pointer = id->control;
}
static void restore_gic_interface(u32 *pointer, unsigned gic_interface_address)
{
    cpu_interface *ci = (cpu_interface *)gic_interface_address;

    /*                           */

    ci->priority_mask = pointer[1];
    ci->binary_point = pointer[2];
    ci->aliased_binary_point = pointer[3];

    /*                               */
    ci->control = pointer[0];
}
static void restore_gic_distributor_private(u32 *pointer, unsigned gic_distributor_address)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned tmp;
    //                                         

    /*                                                                       */
    tmp = id->control;
    id->control = 0;
    /*                              */
    id->enable.set[0] = *pointer;
    ++pointer;
    /*                                 */
    copy_words(id->priority, pointer, 8);
    pointer += 8;
    /*                              */
    copy_words(id->target, pointer, 8);
    pointer += 8;
    /*                                                                 */
    id->configuration[1] = *pointer;
    ++pointer;
    /*                                */
    id->security[0] = *pointer;
    ++pointer;
#if 0
    /*
                                                                      
                   
     */
    id->active.clear[0] = 0xffffffff;
    id->pending.clear[0] = 0xffffffff;
#endif
#if 1
    /*                                */
    id->pending.set[0] = *pointer;
    ++pointer;
#endif
    /*
                           
     */
    copy_words(id->sgi_set_pending, pointer, 4);
    pointer += 4;

    id->control = tmp;
}
static void restore_gic_distributor_shared(u32 *pointer, unsigned gic_distributor_address)
{
    interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
    unsigned num_spis;
    int i, j;

    /*                                                                       */
    id->control = 0;

    /*                                          */
    num_spis = 32 * ((id->controller_type) & 0x1f);

    /*                           */

    /*                                   */
    if (num_spis) {
        copy_words(id->enable.set + 1, pointer, num_spis / 32);
        pointer += num_spis / 32;
        copy_words(id->priority + 8, pointer, num_spis / 4);
        pointer += num_spis / 4;
        copy_words(id->target + 8, pointer, num_spis / 4);
        pointer += num_spis / 4;
        copy_words(id->configuration + 2, pointer, num_spis / 16);
        pointer += num_spis / 16;
        copy_words(id->security + 1, pointer, num_spis / 32);
        pointer += num_spis / 32;
        copy_words(id->pending.set + 1, pointer, num_spis / 32);

        if (reg_read( IO_VIRT_TO_PHYS(SPM_SLEEP_ISR_RAW_STA) ) & WAKE_SRC_KP) {
            i = MT_KP_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = MT_KP_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }

        if (reg_read( IO_VIRT_TO_PHYS(SPM_SLEEP_ISR_RAW_STA) ) & WAKE_SRC_MD_WDT) {
            i = MD_WDT_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = MD_WDT_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }
        if (reg_read( IO_VIRT_TO_PHYS(SPM_SLEEP_ISR_RAW_STA) ) & WAKE_SRC_CONN_WDT) {
            i = CONN_WDT_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = CONN_WDT_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }
        if (reg_read( IO_VIRT_TO_PHYS(SPM_SLEEP_ISR_RAW_STA) ) & WAKE_SRC_TS) {
            i = TS_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = TS_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }
        if (reg_read( IO_VIRT_TO_PHYS(SPM_SLEEP_ISR_RAW_STA) ) & WAKE_SRC_LOW_BAT) {
            i = LOWBATTERY_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = LOWBATTERY_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }
        if (reg_read( IO_VIRT_TO_PHYS(SPM_SLEEP_ISR_RAW_STA) ) & WAKE_SRC_WDT) {
            i = MT_WDT_IRQ_ID / GIC_PRIVATE_SIGNALS;
            j = MT_WDT_IRQ_ID % GIC_PRIVATE_SIGNALS;
            id->pending.set[i] |= (1 << j);
        }

        pointer += num_spis / 32;
    }

    /*                                                                                           */
    /*                          */
    id->control = *pointer;
}

static void restore_bp_reg(debug_context_t *dbg, unsigned index, unsigned type)
{
    switch (type) {
    case DBGREG_WP_VAL:
        dbgreg_rw_handlers[index].wvr.write(dbg->wvr[index]);
        break;
    case DBGREG_WP_CTRL:
        dbgreg_rw_handlers[index].wcr.write(dbg->wcr[index]);
        break;
    case DBGREG_BP_XVAL:
        dbgreg_rw_handlers[index].bxvr.write(dbg->bxvr[index]);
        break;
    case DBGREG_BP_VAL:
        dbgreg_rw_handlers[index].bvr.write(dbg->bvr[index]);
        break;
    case DBGREG_BP_CTRL:
        dbgreg_rw_handlers[index].bcr.write(dbg->bcr[index]);
        break;
    default:
        break;
    }

    return;
}

static void save_bp_reg(debug_context_t *dbg, unsigned index, unsigned type)
{
    switch (type) {
    case DBGREG_WP_VAL:
        dbg->wvr[index] = dbgreg_rw_handlers[index].wvr.read();
        break;
    case DBGREG_WP_CTRL:
        dbg->wcr[index] = dbgreg_rw_handlers[index].wcr.read();
        break;
    case DBGREG_BP_XVAL:
        dbg->bxvr[index] = dbgreg_rw_handlers[index].bxvr.read();
        break;
    case DBGREG_BP_VAL:
        dbg->bvr[index] = dbgreg_rw_handlers[index].bvr.read();
        break;
    case DBGREG_BP_CTRL:
        dbg->bcr[index] = dbgreg_rw_handlers[index].bcr.read();
        break;
    default:
        break;
    }

    return;
}

static void sr_bp_context(debug_context_t *dbg, unsigned bp_type, unsigned op)
{
    unsigned num_bps, num_ctx_cmps, num_wps, didr;
    unsigned index = 0, max_index = 0;

    didr = read_dbg_didr();
    num_bps = (didr >> 24) & 0xf;
    num_ctx_cmps = (didr >> 20) & 0xf;
    num_wps = (didr >> 28) & 0xf;

    switch (bp_type) {
    case DBGREG_WP_VAL:
    case DBGREG_WP_CTRL:
        max_index = num_wps;
        break;
    case DBGREG_BP_XVAL:
        index = num_bps - num_ctx_cmps;
    case DBGREG_BP_VAL:
    case DBGREG_BP_CTRL:
        max_index = num_bps;
        break;
    default:
        break;
    }

    for (; index <= max_index; index++)
        if (op)
            save_bp_reg(dbg, index, bp_type);
        else
            restore_bp_reg(dbg, index, bp_type);
    return;
}

static void save_v71_debug_cp14(unsigned *context)
{
    debug_context_t *dbg = (void *) context;
    unsigned virtext_present;

    /*
                              
     */
    if (read_dbg_osdlr() == 0x1)
        write_dbg_osdlr(OSDLR_UNLOCKED);

    virtext_present = (read_dbg_devid() >> 16) & 0xf;

    /*
                                                                    
     */
    write_dbg_oslar(OSLAR_LOCKED);

    dbg->dtrrx_e = read_dbg_dtrrxext();
    dbg->dtrtx_e = read_dbg_dtrtxext();
    dbg->dscr_e = read_dbg_dscrext();
    dbg->wfar = read_dbg_wfar();
    dbg->vcr = read_dbg_vcr();
    dbg->claimclr = read_dbg_claimclr();

    if (virtext_present)
        sr_bp_context(dbg, DBGREG_BP_XVAL, 1);

    sr_bp_context(dbg, DBGREG_BP_VAL, 1);
    sr_bp_context(dbg, DBGREG_BP_CTRL, 1);
    sr_bp_context(dbg, DBGREG_WP_VAL, 1);
    sr_bp_context(dbg, DBGREG_WP_CTRL, 1);

    write_dbg_osdlr(OSDLR_LOCKED);

    return;
}

static void restore_v71_debug_cp14(unsigned *context)
{
    debug_context_t *dbg = (void *) context;
    unsigned virtext_present;

    /*
                              
     */
    if (read_dbg_osdlr() == 0x1)
        write_dbg_osdlr(OSDLR_UNLOCKED);

    virtext_present = (read_dbg_devid() >> 16) & 0xf;

    /*
                                                                    
     */
    write_dbg_oslar(OSLAR_LOCKED);

    write_dbg_dtrrxext(dbg->dtrrx_e);
    write_dbg_dtrtxext(dbg->dtrtx_e);
    write_dbg_dscrext(dbg->dscr_e);
    write_dbg_wfar(dbg->wfar);
    write_dbg_vcr(dbg->vcr);
    write_dbg_claimset(dbg->claimclr);

    if (virtext_present)
        sr_bp_context(dbg, DBGREG_BP_XVAL, 0);

    sr_bp_context(dbg, DBGREG_BP_VAL, 0);
    sr_bp_context(dbg, DBGREG_BP_CTRL, 0);
    sr_bp_context(dbg, DBGREG_WP_VAL, 0);
    sr_bp_context(dbg, DBGREG_WP_CTRL, 0);
    isb();

    /*
                                           
     */
    write_dbg_oslar(OSLAR_UNLOCKED);

    return;
}

debug_registers_t *read_debug_address(void)
{
    unsigned drar, dsar;

    drar = read_dbg_drar();
    dsar = read_dbg_dsar();

    if (!(drar & DRAR_VALID_MASK)
        || !(dsar & DSAR_VALID_MASK)) {
        return 0;   /*                                          */
    }

    return (debug_registers_t *) ((drar & DRAR_ADDRESS_MASK)
                      + (dsar & DSAR_ADDRESS_MASK));
}

/*
                                                  
                                                                   
                                                         
                                                                   
 */

static void save_v7_debug_mmapped(unsigned *context)
{
    debug_registers_t *dbg = (void *)read_debug_address();
    debug_context_t *ctx = (void *)context;
    unsigned v71, num_bps, num_wps, i;
    unsigned didr, virtext_present = (read_id_pfr1() >> 12) & 0xf;

    if (!dbg) {
        return;
    }

    didr = dbg->didr;
    /*
                                             
     */
    v71 =
        (((didr >> DIDR_VERSION_SHIFT) & DIDR_VERSION_MASK) ==
         DIDR_VERSION_7_1);

    /*
                                 
     */
    ctx->vcr = dbg->vcr;
    ctx->dscr_e = dbg->dscr_e;
    ctx->claim = dbg->claimclr;

    if (v71) {
        ctx->eacr = dbg->eacr;
    }

    num_bps = 1 + ((didr >> DIDR_BP_SHIFT) & DIDR_BP_MASK);
    for (i = 0; i < num_bps; ++i) {
        ctx->bvr[i] = dbg->bvr[i];
        ctx->bcr[i] = dbg->bcr[i];
        if (virtext_present)
            ctx->bxvr[i] = dbg->bxvr[i];
    }

    num_wps = 1 + ((didr >> DIDR_WP_SHIFT) & DIDR_WP_MASK);
    for (i = 0; i < num_wps; ++i) {
        ctx->wvr[i] = dbg->wvr[i];
        ctx->wcr[i] = dbg->wcr[i];
    }

    /*
                                                                              
                                                          
     */
    if (v71) {
        write_dbg_osdlr(OSDLR_LOCKED);
    }
}

static void restore_v7_debug_mmapped(unsigned *context)
{
    debug_registers_t *dbg = (void *)read_debug_address();
    debug_context_t *ctx = (void *)context;
    unsigned v71, num_bps, num_wps, i;
    unsigned didr, virtext_present = (read_id_pfr1() >> 12) & 0xf;;

    if (!dbg) {
        return;
    }

    didr = dbg->didr;
    /*
                                             
     */
    v71 =
        (((didr >> DIDR_VERSION_SHIFT) & DIDR_VERSION_MASK) ==
         DIDR_VERSION_7_1);

    /*                                  */
    dbg->lar = LAR_UNLOCKED;
    /*
                                                                               
                                                                          
     */
    if (v71) {
        write_dbg_osdlr(OSDLR_UNLOCKED);
    }

    /*
                                      
     */
    dbg->vcr = ctx->vcr;
    dbg->claimclr = CLAIMCLR_CLEAR_ALL;
    dbg->claimset = ctx->claim;

    if (v71) {
        dbg->eacr = ctx->eacr;
    }

    num_bps = 1 + ((didr >> DIDR_BP_SHIFT) & DIDR_BP_MASK);
    for (i = 0; i < num_bps; ++i) {
        dbg->bvr[i] = ctx->bvr[i];
        dbg->bcr[i] = ctx->bcr[i];
        if (virtext_present)
            dbg->bxvr[i] = ctx->bxvr[i];
    }

    num_wps = 1 + ((didr >> DIDR_WP_SHIFT) & DIDR_WP_MASK);
    for (i = 0; i < num_wps; ++i) {
        dbg->wvr[i] = ctx->wvr[i];
        dbg->wcr[i] = ctx->wcr[i];
    }

    /*                                */
    if (!v71) {
        (dbg->prsr);
    }

    /*                 */
    dbg->dscr_e = ctx->dscr_e;

    /*                                   */
    dbg->lar = LAR_LOCKED;
}

void save_v7_debug(unsigned *context)
{
    unsigned v71 = 0, didr = read_dbg_didr();

    v71 = (((didr >> DIDR_VERSION_SHIFT) & DIDR_VERSION_MASK) ==
           DIDR_VERSION_7_1);

    /*
                                                               
                                                      
     */
    if (v71)
        save_v71_debug_cp14(context);
    else
        save_v7_debug_mmapped(context);

    return;
}

void restore_v7_debug(unsigned *context)
{
    unsigned v71 = 0, didr = read_dbg_didr();

    v71 = (((didr >> DIDR_VERSION_SHIFT) & DIDR_VERSION_MASK) ==
           DIDR_VERSION_7_1);

    /*
                                                                  
                                                      
     */
    if (v71)
        restore_v71_debug_cp14(context);
    else
        restore_v7_debug_mmapped(context);

    return;
}

void stop_generic_timer(generic_timer_context * ctr_ctx)
{
    /*
                                                    
                                                    
                                                     
                                               
     */
    write_cntp_ctl(TIMER_MASK_IRQ);

#if 0 //                      
    /*
                                                     
                                                     
                                                   
                                                     
                                                      
     */
    if (!hyp_timer_trigger) {
        if (ctr_ctx->cntp_ctl & TIMER_IRQ_STAT)
            gic_enable_int(LCL_TIMER_IRQ);
    }
#endif    

    return;
}

void save_pmu_context(unsigned cluster_id, unsigned cpu_id)
{
    switch (pmu_mode) {
    case PMU_STATE1:
        save_performance_monitors(migration_ctx[cpu_id]);
        break;
    case PMU_STATE2:
        save_performance_monitors(clusters_ctx[cluster_id][cpu_id]);
        break;
    case PMU_STATE0:
    default:
        break;
    };
}

void restore_pmu_context(unsigned cluster_id, unsigned cpu_id)
{
    switch (pmu_mode) {
    case PMU_STATE1:
        restore_performance_monitors(migration_ctx[cpu_id]);
        break;
    case PMU_STATE2:
        restore_performance_monitors(clusters_ctx[cluster_id][cpu_id]);
        break;
    case PMU_STATE0:
    default:
        break;
    };
}

static void platform_save_context(void)
{
    unsigned cpu_id = read_cpuid(), cluster_id = read_clusterid();
    cpu_context *ns_cpu_ctx = &switcher_context.cluster.core[cpu_id].ns_cpu_ctx;    
    unsigned *gp_context = ns_cpu_ctx->banked_cpu_regs;
    unsigned *vfp_context = ns_cpu_ctx->vfp_regs;
    //                                                        
    unsigned int *dbg_ctx = ns_cpu_ctx->dbg_data;
    banked_cp15_context *cp15_context = &ns_cpu_ctx->banked_cp15_regs;
    generic_timer_context *cp15_timer_ctx = &ns_cpu_ctx->cp15_timer_ctx;
    cp15_fault_regs *fault_ctx = &cp15_context->ns_cp15_fault_regs;
    gic_cpu_context *gic_pvt_context = &ns_cpu_ctx->gic_cpu_ctx;
    global_context *gbl_context = &switcher_context.cluster.ns_cluster_ctx;

    //                                                    

    /*
                                                        
     */
    save_generic_timer((unsigned *)cp15_timer_ctx, 0x0);
    stop_generic_timer(cp15_timer_ctx);

    /*
                                           
                                                
                        
     */
    save_pmu_context(cluster_id, cpu_id);
    //                                                  
    save_cp15(cp15_context->cp15_misc_regs);
    save_control_registers(cp15_context->cp15_ctrl_regs, 0x0);
    save_mmu(cp15_context->cp15_mmu_regs);
    save_fault_status((unsigned *)fault_ctx);

    save_vfp(vfp_context);

#if 1 //                 
    if(cpu_id==0)
    {
        /*                                       */
        save_gic_interface(gic_pvt_context->gic_cpu_if_regs,GIC_CPU_BASE);
        /*
                
                                                               
                                                            
                                                            
                                                              
                                             
         */
        /*                                             */
        save_gic_distributor_private(gic_pvt_context->gic_dist_if_pvt_regs,GIC_DIST_BASE);
        /*                                            */
        save_gic_distributor_shared(gbl_context->gic_dist_if_regs,GIC_DIST_BASE); 
    }
#endif    

    //                                         
    //                                          
    save_dbg_regs(dbg_ctx);

#if 0
    save_mcu_biu_register();
#endif

    if (power_state[cpu_id] == STATUS_DORMANT) {
        /*                                  */
        reg_write(CA7_CACHE_CONFIG, reg_read(CA7_CACHE_CONFIG) | (1U << 4));
    }

    dormant_ret_flag[cpu_id] = 0;

    save_banked_registers(gp_context);
}

static void platform_restore_context(void)
{
    unsigned cpu_id = read_cpuid();
    unsigned cluster_id = read_clusterid();
    
    cpu_context *ns_cpu_ctx = (cpu_context *) __pa(&switcher_context.cluster.core[cpu_id].ns_cpu_ctx);    
    global_context *gbl_context = (global_context *) __pa(&switcher_context.cluster.ns_cluster_ctx);            
    unsigned *vfp_context = ns_cpu_ctx->vfp_regs;
    //                                                        
    //                                                                    
    banked_cp15_context *cp15_context = &ns_cpu_ctx->banked_cp15_regs;
    cp15_fault_regs *fault_ctx = &cp15_context->ns_cp15_fault_regs;
    gic_cpu_context *gic_pvt_context = &ns_cpu_ctx->gic_cpu_ctx;

    cpu_context *ns_cpu_ctx_v = &switcher_context.cluster.core[cpu_id].ns_cpu_ctx;
    unsigned *gp_context_v = ns_cpu_ctx_v->banked_cpu_regs;
    banked_cp15_context *cp15_context_v = &ns_cpu_ctx_v->banked_cp15_regs;
    generic_timer_context *cp15_timer_ctx_v = &ns_cpu_ctx_v->cp15_timer_ctx;
    unsigned int *dbg_ctx = ns_cpu_ctx_v->dbg_data;    

#if 1 //                 
    if(cpu_id==0)
    {
        /*                             */
        restore_gic_distributor_shared(gbl_context->gic_dist_if_regs,IO_VIRT_TO_PHYS(GIC_DIST_BASE));
        /*                              */
        restore_gic_distributor_private(gic_pvt_context->gic_dist_if_pvt_regs,IO_VIRT_TO_PHYS(GIC_DIST_BASE));
        /*                     */
        restore_gic_interface(gic_pvt_context->gic_cpu_if_regs,IO_VIRT_TO_PHYS(GIC_CPU_BASE));
    }
#endif
    
    isb();
    dsb();
    restore_vfp(vfp_context);
    /*
                           
                                                   
                                              
                                               
     */
    restore_fault_status((unsigned *)fault_ctx);
    
    restore_mmu(cp15_context->cp15_mmu_regs);

    invalidate_unified_TLB_inner_shareable();    
    
    mt_restore_control_registers(cp15_context->cp15_ctrl_regs, 0x0);
    isb();
    dsb();

    //                                                                    
    
    restore_cp15(cp15_context_v->cp15_misc_regs);    
  
    restore_pmu_context(cluster_id, cpu_id);

    restore_generic_timer((unsigned *)cp15_timer_ctx_v, 0x0);
    
    //                                            
    restore_dbg_regs(dbg_ctx);

#if 0
    restore_mcu_biu_register();
#endif

    if (power_state[cpu_id] == STATUS_DORMANT) {
        /*                                 */
        reg_write(CA7_CACHE_CONFIG, reg_read(CA7_CACHE_CONFIG) & ~(1U << 4));
    }
#if 0 /*                     */
    trace_stop_dormant();
    trace_start_dormant();
#endif    
    dormant_ret_flag[cpu_id] = 1;

    power_state[cpu_id] = STATUS_RUN;

    restore_banked_registers(gp_context_v);    
}

int cpu_power_down(int mode)
{
    unsigned cpu_id;

    void (*restore_ptr)(void);
    restore_ptr = cpu_wake_up;
       
    cpu_id = read_cpuid();

#if !defined(CONFIG_TRUSTONIC_TEE_SUPPORT)
	reg_write(BOOTROM_BOOT_ADDR, __pa(restore_ptr));
#else //                                          
    mt_secure_call(MC_FC_SLEEP, __pa(restore_ptr), cpu_id, 0);
#endif //                                          

    power_state[cpu_id] = mode;
    
#if 1   
    __disable_dcache();

    dsb();
    __asm__ __volatile__("MCR p15,0,r0,c8,c7,1");
    dsb();

    if (power_state[cpu_id] == STATUS_DORMANT)         
    {
        /*                                       */
        inner_dcache_flush_L1();

        /*                                       */
        __inner_clean_dcache_L2();
    }
    else
    {
        /*                                                          */            
        inner_dcache_flush_all();
    }

    /*                             */
    __asm__ __volatile__("clrex");
#endif    
    
    platform_save_context();

    return dormant_ret_flag[cpu_id];
}

void cpu_start_restore(void)
{
    platform_restore_context();
}

void cpu_dormant_init(void)
{
    //                                             
    reg_write(BOOTROM_PWR_CTRL, reg_read(BOOTROM_PWR_CTRL) | 0x80000000);
}

void cpu_check_dormant_abort(void)
{
    unsigned cpu_id;
    cpu_id = read_cpuid();

    __enable_cache();

    if (power_state[cpu_id] != STATUS_RUN)
    {
        //                
        unsigned cluster_id;
        cpu_context *ns_cpu_ctx;
        generic_timer_context *cp15_timer_ctx;

        //                 
#if defined(CONFIG_TRUSTONIC_TEE_SUPPORT)
        //                                                                           
        if (cpu_id == 0)
        {
	    //                                                        
            mt_secure_call(MC_FC_SLEEP_CANCELLED, 0, 0, 0);
        }
#endif

        //                      
        cluster_id = read_clusterid();
        ns_cpu_ctx = &switcher_context.cluster.core[cpu_id].ns_cpu_ctx;
        cp15_timer_ctx = &ns_cpu_ctx->cp15_timer_ctx;

        //                                      
        restore_generic_timer((unsigned *)cp15_timer_ctx, 0x0);
        restore_pmu_context(cluster_id, cpu_id);

        if (power_state[cpu_id] == STATUS_DORMANT) {
            /*                                 */
            reg_write(CA7_CACHE_CONFIG, reg_read(CA7_CACHE_CONFIG) & ~(1U << 4));
        }
       
        power_state[cpu_id] = STATUS_RUN;
    }
	else {
                /*            */
    }
		
}

MODULE_AUTHOR("Wan-Ching Huang <marc.huang@mediatek.com>");
MODULE_DESCRIPTION("MT658x Dormant/Shutdown Mode Driver $Revision: #1 $");

