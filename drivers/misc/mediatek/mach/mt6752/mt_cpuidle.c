#include <linux/kernel.h>
#include <linux/init.h>
#if defined (__KERNEL__)  //                     
#include <linux/jiffies.h>
#include <linux/export.h>
#include <linux/module.h>
#include <linux/cpuidle.h>
#include <linux/cpu_pm.h>
#include <linux/irqchip/arm-gic.h>

#include <asm/psci.h>
#include <asm/smp_scu.h>
#include <asm/cpuidle.h>
#endif //                      

#include <asm/proc-fns.h>
#include <asm/suspend.h>
#include <asm/tlbflush.h>
#include <asm/memory.h>
#include <asm/system.h>
#if !defined (__KERNEL__) //                    
#include "reg_base.H"
#include "mt_dormant.h"
#include "mt_cpuidle.h"
#include "smp.h"
#include "mt_spm.h"
#include "irq.h"
#include "sync_write.h"
//                       
#include "gic.h"
#else  //                                               
#include <asm/idmap.h>
#include <asm/irqflags.h>
#include <mach/mt_reg_base.h>
#include <mach/mt_dormant.h>
#include <mach/mt_cpuidle.h>
#include <mach/mt_spm.h>
#include <mach/mt_spm_idle.h>
#include <mach/smp.h>
#include <mach/mt_irq.h>
#include <mach/sync_write.h>
//                            
#include <mach/mt_boot.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <mach/mt_dbg.h>
#endif //                                               


/*                                
        
                                  */

#if defined (__KERNEL__)
//                                    
#if defined (CONFIG_OF)
#define MP0_DBGAPB_BASE (0xf0810000)
#define MP1_DBGAPB_BASE (0xf0C10000)
#define K2_MCUCFG_BASE (0xf0200000)      //           
#define CCI400_BASE     (0xf0390000)   //           
#define INFRACFG_AO_BASE        (0xf0001000) //           
#define GIC_CPU_BASE    (0xf0220000 + 0x2000)
#define GIC_DIST_BASE   (0xf0220000 + 0x1000)

#else
#define MP0_DBGAPB_BASE (0xf0810000)
#define MP1_DBGAPB_BASE (0xf0C10000)
#define K2_MCUCFG_BASE (0xf0200000)      //           
#define CCI400_BASE     (0xf0390000)   //           
#define INFRACFG_AO_BASE        (0xf0001000) //           
#define GIC_CPU_BASE    (0xf0220000 + 0x2000)
#define GIC_DIST_BASE   (0xf0220000 + 0x1000)
#endif //                       

#define DBGAPB_CORE_OFFSET (0x10000)
#else //                        
//                                    
#define MP0_DBGAPB_BASE (0x10810000)
#define MP1_DBGAPB_BASE (0x10C10000)
#define K2_MCUCFG_BASE (0x10200000)      //           
#define CCI400_BASE     (0x10390000)   //           
#define INFRACFG_AO_BASE        (0x10001000) //           

#define DBGAPB_CORE_OFFSET (0x10000)
typedef unsigned long long u64;
#define ____cacheline_aligned __attribute__((aligned(8)))
#define __weak __attribute__((weak))
#define __naked __attribute__((naked))
typedef enum {
    CHIP_SW_VER_01 = 0x0000,
    CHIP_SW_VER_02 = 0x0001
} CHIP_SW_VER;
#define local_fiq_enable() do {} while(0)
#endif //                        

#define MP0_CA7L_CACHE_CONFIG   (K2_MCUCFG_BASE + 0)
#define MP1_CA7L_CACHE_CONFIG   (K2_MCUCFG_BASE + 0x200)
#define L2RSTDISABLE 		(1 << 4)

#define MP0_AXI_CONFIG          (K2_MCUCFG_BASE + 0x2C) 
#define MP1_AXI_CONFIG          (K2_MCUCFG_BASE + 0x22C) 
#define ACINACTM                (1<<4)


#define CCI400_MP0_SNOOP_CTLR   (CCI400_BASE + 0x5000) //          
#define CCI400_MP1_SNOOP_CTLR   (CCI400_BASE + 0x4000) //          
#define CCI400_STATUS		(CCI400_BASE + 0x000C) //          
#define CCI400_CONTROL		(CCI400_BASE + 0x0000) //          

#define BOOTROM_PWR_CTRL        (INFRACFG_AO_BASE + 0x804) //          
#define BOOTROM_BOOT_ADDR       (INFRACFG_AO_BASE + 0x800) //          


//                                                                    
#define PD_SW_CG_EN 		(1 << 4)
	

#define reg_read(addr)          __raw_readl(IOMEM(addr))        //                                   
#define reg_write(addr, val)    mt_reg_sync_writel(val, addr)
#define _and(a, b) 	((a) & (b))
#define _or(a, b) 	((a) | (b))
#define _aor(a, b, c) 	_or(_and(a, b), (c))

#define smp() 	do {					\
		register unsigned long long t;                  \
		__asm__ __volatile__ (			\
			"isb \n\t"                              \
			"dsb \n\t"                              \
			"MRRC p15, 1 ,%Q0, %R0, c15 \n\t"	\
			"isb \n\t"                              \
			"dsb \n\t"                              \
			"ORR %Q0,%Q0,#(1<<6)     \n\t"          \
			"MCRR p15, 1 ,%Q0, %R0, c15 \n\t"	\
			"isb \n\t"			\
			"dsb \n\t"			\
			: "=r"(t)			\
			);				\
	} while(0)

#define amp() do {					\
		register unsigned long long t;                  \
		__asm__ __volatile__ (			\
			"clrex \n\t"			\
			"isb \n\t"                              \
			"dsb \n\t"                              \
			"MRRC p15, 1 ,%Q0, %R0, c15 \n\t"	\
			"isb \n\t"                              \
			"dsb \n\t"                              \
			"BIC %Q0,%Q0,#(1<<6) \n\t"              \
			"MCRR p15, 1 ,%Q0, %R0, c15 \n\t"	\
			"isb \n\t"			\
			"dsb \n\t"			\
			: "=r"(t)			\
			);				\
	} while(0)

#define read_cntpct()					\
	({						\
		unsigned long long cntpct;		\
		__asm__ __volatile__(			\
			"MRRC p15, 0, %Q0, %R0, c14\n"	\
			:"=r"(cntpct)			\
			:				\
			:"memory");			\
		cntpct;					\
	})


#define read_cntpctl()					\
	({						\
		unsigned int cntpctl;			\
		__asm__ __volatile__(			\
			"MRC p15, 0, %0, c14, c2, 1\n"  \
			:"=r"(cntpctl)			\
			:				\
			:"memory");			\
		cntpctl;				\
	})

#define write_cntpctl(cntpctl)				\
	do {						\
		__asm__ __volatile__(			\
			"MCR p15, 0, %0, c14, c2, 1\n"  \
			:				\
			:"r"(cntpctl));			\
	} while (0)


#define EP (1!=0)
#if (EP)
#define EP_IGNORE(exp) do{} while(0)
#define EP_ONLY(exp) ({exp;})
#else //        
#define EP_IGNORE(exp) ({exp;})
#define EP_ONLY(exp) do{} while(0)
#endif //        

/*                                
                
                                  */
#define CPU_DORMANT_LOG_WITH_NONE                           0
#define CPU_DORMANT_LOG_WITH_XLOG                           1
#define CPU_DORMANT_LOG_WITH_PRINTK                         2

#define CPU_DORMANT_LOG_PRINT CPU_DORMANT_LOG_WITH_NONE

#if (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_WITH_NONE)
#define CPU_DORMANT_INFO(fmt, args...)          do { } while(0)
#elif (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_WITH_XLOG)
#define CPU_DORMANT_INFO(fmt, args...)		do { xlog_printk(ANDROID_LOG_INFO, "Power/cpu_dormant", fmt, ##args); } while(0)
#elif (CPU_DORMANT_LOG_PRINT == CPU_DORMANT_LOG_WITH_PRINTK)
#define CPU_DORMANT_INFO(fmt, args...)		do { printk("[Power/cpu_dormant] "fmt, ##args); } while(0)
#endif

#define zlog(fmt, args...)		xlog_printk(ANDROID_LOG_INFO, "Power/cpu_dormant", fmt, ##args)

#define MT_DORMANT_DEBUG 

#if defined (MT_DORMANT_DEBUG)
#define SENTINEL_CHECK(data, p) BUG_ON((unsigned long)(p) > ((unsigned long)(&data) + sizeof(data)))
#else //                              
#define SENTINEL_CHECK(a, b) do{} while(0)
#endif //                              


/*                */
#define DEBUG_DORMANT_BYPASS (1==0)

#define TSLOG_ENABLE
#define TSLOG_ENABLE_2  //                                           
#if defined (TSLOG_ENABLE)
#define TSLOG(a, b) do { (a) = (b); } while(0)
#else 
#define TSLOG(a, b) do {} while(0)
#endif

/*                                
         
                                  */
typedef struct cpu_context {
	unsigned int banked_regs[32];
	unsigned int pmu_data[20];
	unsigned int vfp_data[32*2+8]; 
	unsigned int timer_data[8]; /*                                                  */
	volatile u64 timestamp[5];
        unsigned int count, rst, abt, brk;
} core_context;

#define MAX_CORES (4)   //                    

typedef struct cluster_context {
	core_context core[MAX_CORES] ____cacheline_aligned;
	unsigned int dbg_data[40]; 
	int l2rstdisable;
	int l2rstdisable_rfcnt;
} cluster_context;


#define MAX_CLUSTER (2)
/*
                                                                             
 */
typedef struct system_context {
	cluster_context cluster[2];
        struct _data_poc {
                void (*cpu_resume_phys)(void); //                                         
                unsigned long l2ectlr, l2actlr;
                CHIP_SW_VER chip_ver;
                unsigned long *cpu_dormant_aee_rr_rec;
        } poc  ____cacheline_aligned; 
} system_context;


/*                                
         
                                  */
void __disable_dcache__inner_flush_dcache_L1__inner_clean_dcache_L2(void);
void __disable_dcache__inner_flush_dcache_L1__inner_flush_dcache_L2(void);
void __disable_dcache__inner_flush_dcache_L1(void);

extern unsigned long *aee_rr_rec_cpu_dormant(void);

extern unsigned int *mt_save_banked_registers(unsigned int *container);
extern void mt_restore_banked_registers(unsigned int *container);

extern void cpu_wake_up(void);
extern void mt_smp_set_boot_addr(u32 addr, int cpu);
extern void __disable_dcache(void);
extern void __enable_dcache(void);
extern void __disable_icache(void);
extern void __enable_icache(void);
extern void v7_flush_kern_dcache_louis(void);
extern void v7_flush_kern_dcache_all(void);
extern void cpu_resume(void);
extern void trace_start_dormant(void);

__weak unsigned int mt_get_chip_sw_ver(void) {return 0; }

#if 0
__weak bool spm_is_cpu_irq_occur(int spm_core_id)
{
	return 0;
}
#else

//                
//                
#define SPM_CORE_ID() core_idx()
#define SPM_IS_CPU_IRQ_OCCUR(core_id)                                   \
        ({                                                              \
                (!!(spm_read(SPM_SLEEP_WAKEUP_MISC) & ((0x101<<(core_id))))); \
        })
#endif

#if 0
#define PCLOG(cid) do {                                                 \
                __asm__ __volatile__ (                                  \
                        "str pc, [%0, %1] \n\t"                         \
                        ::"r"(dormant_data[0].poc.cpu_dormant_aee_rr_rec), "r"(cid*4) \
                        );                                              \
        } while(0)

#else
#define PCLOG(cid) do { } while(0)
__weak unsigned long *aee_rr_rec_cpu_dormant(void) 
{ 
        return (unsigned long *)0; 
}

#endif

#if defined (__CTP__)

#endif

/*                                
                  
                                  */
/*
                                                                   
                    
 */

system_context dormant_data[1];
volatile int debug_dormant_bypass=0;
unsigned long *aee_rr_rec_dmnt_phys;
unsigned long *aee_rr_rec_dmnt;
static int mt_dormant_initialized = 0;


/*                                
           
                                  */
//                                
#define read_isr()							\
	({								\
		register unsigned int ret;				\
		__asm__ __volatile__ ("mrc   p15, 0, %0, c12, c1, 0 \n\t" \
				      :"=r"(ret));			\
		ret;							\
	})

#define read_mpidr()							\
	({								\
		register unsigned int ret;				\
		__asm__ __volatile__ ("mrc   p15, 0, %0, c0, c0, 5 \n\t" \
				      :"=r"(ret));			\
		ret;							\
	})

//                               
#define read_midr()							\
	({								\
		register unsigned int ret;				\
		__asm__ __volatile__ ("mrc   p15, 0, %0, c0, c0, 0 \n\t" \
				      :"=r"(ret));			\
		ret;							\
	})


#define read_scuctlr()							\
	({								\
		register unsigned int ret;				\
		__asm__ __volatile__ ("mrc   p15, 1, %0, c9, c0, 4 \n\t" \
				      :"=r"(ret));			\
		ret;							\
	})


#define CA12_TYPEID 0x410FC0D0
#define CA17_TYPEID 0x410FC0E0
#define CA7_TYPEID 0x410FC070
#define CA7L_TYPEID 0x410FD030
#define CPU_TYPEID_MASK 0xfffffff0

//                                 
#define is_cpu_type(type)						\
	({								\
		((read_midr() & CPU_TYPEID_MASK) == type) ? 1 : 0;	\
	})
	
//                         
#define boot_cpu()					\
	({						\
		((read_mpidr() & 0xf0f) == 0) ? 1 : 0;	\
	})

//                       
#define cpu_id()				\
	({					\
		(read_mpidr() & 0x03);		\
	})

//                           
#define cluster_id()				\
	({					\
		((read_mpidr() >> 8) & 0x0f);	\
	})

#define core_idx()                                                      \
	({                                                              \
                int mpidr = read_mpidr();                               \
		((( mpidr & (0x0f << 8)) >> 6) | (mpidr & 0x03));	\
	})

inline int read_id(int *cpu_id, int *cluster_id)
{
	int mpidr = read_mpidr();
	
	*cpu_id = mpidr & 0x0f;
	*cluster_id = (mpidr >> 8) & 0x0f;

	return mpidr;
}

#define system_cluster(system, clusterid)	(&((system_context *)system)->cluster[clusterid])
#define cluster_core(cluster, cpuid)	(&((cluster_context *)cluster)->core[cpuid])

void *_get_data(int core_or_cluster)
{
	int cpuid, clusterid;
	cluster_context *cluster;
	core_context *core;
					  
	read_id(&cpuid, &clusterid);
	
	cluster = system_cluster(dormant_data, clusterid);
	if (core_or_cluster == 1) 
		return (void *)cluster;

	core = cluster_core(cluster, cpuid);
	return (void *)core;
}

#define GET_CORE_DATA() ((core_context *)_get_data(0))
#define GET_CLUSTER_DATA() ((cluster_context *)_get_data(1))
#define GET_SYSTEM_DATA() ((system_context *)dormant_data)


/*                  */
/*                  */
/*                  */
unsigned *save_vfp(unsigned int *container)
{

	__asm__ __volatile__ (
                ".fpu neon \n\t"
		"@ FPU state save/restore.	   \n\t"
		"@    Save configuration registers and enable. \n\t"
                
                "@ CPACR allows CP10 and CP11 access \n\t"
                "mrc    p15,0,r4,c1,c0,2    \n\t"
                "ORR    r2,r4,#0xF00000 \n\t"
                "mcr    p15,0,r2,c1,c0,2 \n\t"
                "isb    \n\t"
                "mrc    p15,0,r2,c1,c0,2 \n\t"
                "and    r2,r2,#0xF00000 \n\t"
                "cmp    r2,#0xF00000 \n\t"
                "beq    0f \n\t"
                "movs   r2, #0 \n\t"
                "b      2f \n\t"

                "0: \n\t"
                "@ Enable FPU access to save/restore the other registers. \n\t"
		"FMRX   r1,FPEXC          @ vmrs   r1,FPEXC \n\t"
		"ldr    r2,=0x40000000  \n\t"
		"orr    r2, r1         \n\t"
		"FMXR   FPEXC,r2           @ vmsr   FPEXC,r2 \n\t"

		"@ Store the VFP-D16 registers. \n\t"
		"vstm   %0!, {D0-D15} \n\t"

		"@ Check for Advanced SIMD/VFP-D32 support \n\t"
		"FMRX   r2,MVFR0           @ vmrs   r2,MVFR0 \n\t"
		"and    r2,r2,#0xF         @ extract the A_SIMD bitfield \n\t"
		"cmp    r2, #0x2 \n\t"

		"@ Store the Advanced SIMD/VFP-D32 additional registers. \n\t"
		"vstmiaeq   %0!, {D16-D31} \n\t"
                "addne  %0, %0, #128 \n\t"

		"FMRX   r2,FPSCR           @ vmrs   r2,FPSCR \n\t"

                "tst	r1, #0x80000000    @#FPEXC_EX  \n\t"
                "beq	3f \n\t"
                "FMRX	r3, FPINST	   @ FPINST (only if FPEXC.EX is set) \n\t"
                
                "tst	r1, #0x10000000    @FPEXC_FP2V, is there an FPINST2 to read? \n\t"
                "beq	3f \n\t"
                "FMRX	ip, FPINST2		@ FPINST2 if needed (and present) \n\t"
                
                "3: \n\t"
                "@ IMPLEMENTATION DEFINED: save any subarchitecture defined state \n\t"
		"@ NOTE: Dont change the order of the FPEXC and CPACR restores \n\t"
                "stm	%0!, {r1, r2, r3, ip} \n\t"

		"@ Restore the original En bit of FPU. \n\t"
		"FMXR   FPEXC, r1          @ vmsr   FPEXC,r1           \n\t"
        
                "@ Restore the original CPACR value. \n\t"
                "2:     \n\t"
                "mcr    p15,0,r4,c1,c0,2   \n\t"

		:"+r"(container)
                :
                :"r1", "r2", "r3", "r4", "ip");

		
        
	return container;
}


/*                     */
/*                     */
/*                     */
void restore_vfp(int *container)
{
	__asm__ __volatile__ (
                ".fpu neon \n\t"
		"@ FPU state save/restore. Obviously FPSID,MVFR0 and MVFR1 dont get \n\t"
		"@ serialized (RO). \n\t"
		"@ Modify CPACR to allow CP10 and CP11 access \n\t"
                "mrc    p15,0,r4,c1,c0,2 \n\t"
                "ORR    r2,r4,#0x00F00000  \n\t"
		"mcr    p15,0,r2,c1,c0,2 \n\t"
                "isb    \n\t"

		"@ Enable FPU access to save/restore the rest of registers. \n\t"
		"FMRX   r1,FPEXC          @ vmrs   r1,FPEXC \n\t"

		"ldr    r2,=0x40000000 \n\t"
                "orr    r2, r1 \n\t"
		"FMXR   FPEXC, r2        @ vmsr   FPEXC, r2 \n\t"

		"@ Restore the VFP-D16 registers. \n\t"
		"vldm   %0!, {D0-D15} \n\t"

		"@ Check for Advanced SIMD/VFP-D32 support \n\t"
		"FMRX   r2, MVFR0        @ vmrs   r2, MVFR0 \n\t"
		"and    r2,r2,#0xF       @ extract the A_SIMD bitfield \n\t"
		"cmp    r2, #0x2 \n\t"
        
		"@ Store the Advanced SIMD/VFP-D32 additional registers. \n\t"
		"vldmiaeq    %0!, {D16-D31} \n\t"
                "addne  %0, %0, #128 \n\t"

                "ldm	%0, {r1, r2, r3, ip} \n\t"

                "tst	r1, #0x80000000   @#FPEXC_EX  \n\t"
                "beq	3f \n\t"
                "FMXR	FPINST, r3	   @ FPINST (only if FPEXC.EX is set) \n\t"
                
                "tst	r1, #0x10000000  @ FPEXC_FP2V, is there an FPINST2 to read? \n\t"
                "beq	3f \n\t"
                "FMXR	FPINST2, ip		@ FPINST2 if needed (and present) \n\t"
                
                "3: \n\t"
                
                "@ Restore configuration registers and enable. \n\t"
                "@ Restore FPSCR _before_ FPEXC since FPEXC could disable FPU \n\t"
                "@ and make setting FPSCR unpredictable. \n\t"
		"FMXR    FPSCR,r2       @ vmsr    FPSCR,r2 \n\t"

		"FMXR    FPEXC,r1        @ vmsr    FPEXC,r1                 \n\t"

                "mcr     p15,0,r4,c1,c0,2 \n\t"

		:
		:"r"(container)
                :"r1", "r2", "r3", "r4", "ip" );
	return;
}



/*                                   */
/*                                   */
/*                                   */
unsigned *save_pmu_context(unsigned *container)
{
	__asm__ __volatile__ (
		"mrc    p15,0,r8,c9,c12,0    @ PMon: Control Register \n\t"
		"bic    r1,r8,#1 \n\t"
		"mcr    p15,0,r1,c9,c12,0    @ disable counter updates from here \n\t"
		"isb                         @ 0b0 => PMCR<0> \n\t"
		"mrc    p15,0,r9,c9,c12,5    @ PMon: Event Counter Selection Register \n\t"
		"mrc    p15,0,r10,c9,c12,1   @ PMon: Count Enable Set Reg \n\t"
		"stm    %0!, {r8-r10} \n\t"
		"mrc    p15,0,r8,c9,c12,2    @ PMon: Count Enable Clear Register \n\t"
		"mrc    p15,0,r9,c9,c13,0    @ PMon: Cycle Counter Register \n\t"
		"mrc    p15,0,r10,c9,c12,3   @ PMon: Overflow flag Status Register \n\t"
		"stm    %0!, {r8-r10} \n\t"
		"mrc    p15,0,r8,c9,c14,1    @ PMon: Interrupt Enable Set Registern \n\t"
		"mrc    p15,0,r9,c9,c14,2    @ PMon: Interrupt Enable Clear Register \n\t"
		"stm    %0!, {r8-r9} \n\t"
		"mrc    p15,0,r8,c9,c12,0    @ Read PMon Control Register \n\t"
		"ubfx   r9,r8,#11,#5         @ extract # of event counters, N \n\t"
		"tst    r9, r9 \n\t"
		"beq    1f \n\t"
        
		"mov    r8,#0 \n\t"
		"0:         \n\t"
		"mcr    p15,0,r8,c9,c12,5    @ PMon: select CounterN \n\t"
		"isb \n\t"
		"mrc    p15,0,r3,c9,c13,1    @ PMon: save Event Type Register \n\t"
		"mrc    p15,0,r4,c9,c13,2    @ PMon: save Event Counter Register \n\t"
		"stm    %0!, {r3,r4} \n\t"
		"add    r8,r8,#1             @ increment index \n\t"
		"cmp    r8,r9 \n\t"
		"bne    0b \n\t"
		"1: \n\t"
		: "+r"(container)
		:
		:"r1", "r3", "r4", "r8", "9", "r10");

	return container;
}

/*                                      */
/*                                      */
/*                                      */
void restore_pmu_context(int *container)
{
	__asm__ __volatile__ (
		"@ NOTE: all counters disabled by PMCR<0> == 0 on reset \n\t"
        
		"ldr    r8,[%0]                  @ r8 = PMCR \n\t"
		"add    r1,%0,#20                @ r1 now points to saved PMOVSR \n\t"
		"ldr    r9,[r1]                  @ r9 = PMOVSR \n\t"
        
		"mvn    r2,#0                    @ generate Register of all 1s \n\t"
		"mcr    p15,0,r2,c9,c14,2        @ disable all counter related interrupts \n\t"
		"mcr    p15,0,r2,c9,c12,3        @ clear all overflow flags \n\t"
		"isb \n\t"
        
		"ubfx   r12,r8,#11,#5            @ extract # of event counters, N (0-31) \n\t"
		"tst    r12, r12 \n\t"
		"beq    20f \n\t"
        
		"add    r1,%0,#32                @ r1 now points to the 1st saved event counter \n\t"

		"@@ Restore counters \n\t"
		"mov    r6,#0 \n\t"
		"10:     \n\t"
		"mcr    p15,0,r6,c9,c12,5        @ PMon: select CounterN \n\t"
		"isb \n\t"
		"ldm    r1!, {r3,r4}             @ Read saved data \n\t"
		"mcr    p15,0,r3,c9,c13,1        @ PMon: restore Event Type Register \n\t"
		"mcr    p15,0,r4,c9,c13,2        @ PMon: restore Event Counter Register \n\t"
		"add    r6,r6,#1                 @ increment index \n\t"
		"cmp    r6,r12 \n\t"
		"bne    10b \n\t"
        
		"20:     \n\t"
		"tst    r9, #0x80000000          @ check for cycle count overflow flag \n\t"
		"beq    40f \n\t"
		"mcr    p15,0,r2,c9,c13,0        @ set Cycle Counter to all 1s \n\t"
		"isb \n\t"
		"mov    r3, #1 \n\t"
		"mcr    p15,0,r3,c9,c12,0        @ set the PMCR global enable bit \n\t"
		"mov    r3, #0x80000000 \n\t"
		"mcr    p15,0,r3,c9,c12,1        @ enable the Cycle Counter \n\t"
		"isb \n\t"
        
		"30:     \n\t"
		"mrc    p15,0,r4,c9,c12,3        @ check cycle count overflow now set \n\t"
		"movs   r4,r4                    @ test bit<31> \n\t"
		"bpl    30b \n\t"
		"mcr    p15,0,r3,c9,c12,2        @ disable the Cycle Counter \n\t"
        
		"40:     \n\t"
		"mov    r1, #0 \n\t"
		"mcr    p15,0,r1,c9,c12,0        @ clear the PMCR global enable bit \n\t"
		"isb \n\t"
        
		"@@ Restore left regs but PMCR \n\t"
		"add    r1,%0,#4                 @ r1 now points to the PMSELR \n\t"
		"ldm    r1!,{r3,r4} \n\t"
		"mcr    p15,0,r3,c9,c12,5        @ PMon: Event Counter Selection Reg \n\t"
		"mcr    p15,0,r4,c9,c12,1        @ PMon: Count Enable Set Reg \n\t"
		"ldm    r1!, {r3,r4} \n\t"
		"mcr    p15,0,r4,c9,c13,0        @ PMon: Cycle Counter Register \n\t"
		"ldm    r1!,{r3,r4} \n\t"
		"mcr    p15,0,r3,c9,c14,2        @ PMon: Interrupt Enable Clear Reg \n\t"
		"mcr    p15,0,r4,c9,c14,1        @ PMon: Interrupt Enable Set Reg \n\t"
		"ldr    r3,[r1] \n\t"
		"isb \n\t"
		"ldr    %0,[%0] \n\t"
		"mcr    p15,0,%0,c9,c12,0        @ restore the PM Control Register \n\t"
		"isb \n\t"
		:
		: "r"(container)
		: "r1", "r2", "r3", "r4", "r5", "r6", "r8", "r9", "r10", "r12", "lr");
	return;
}

#if 1
/*                                                                                 */
/*                                                                                 */
/*                                                                                 */
/*                                              */
/*                                                                     */
/*                                                                                 */

unsigned *mt_save_generic_timer(unsigned int *container, int sw)
{
	__asm__ __volatile__ (
		" cmp    %1, #0  		  @ non-secure? \n\t"
		/*                                                             */
		/*                             */
		" mrc    p15,0,r2,c14,c2,1        @ read CNTP_CTL \n\t"
		" mrc    p15,0,r3,c14,c2,0        @ read CNTP_TVAL \n\t"
		" mrc    p15,0,r12,c14,c1,0       @ read CNTKCTL \n\t"
		" stm    %0!, {r2, r3, r12}  \n\t"
		: "+r"(container)
		: "r"(sw)
		:"r2", "r3", "r12");

	return container;
}

void mt_restore_generic_timer(int *container, int sw)
{
	__asm__ __volatile__ (
		" cmp    %1, #0 \n\t"
		/*                             */
		/*                                                              */
		" ldm    %0!, {r2, r3, r12} \n\t"
		" mcr    p15,0,r3,c14,c2,0        @ write CNTP_TVAL \n\t"
		" mcr    p15,0,r12,c14,c1,0       @ write CNTKCTL \n\t"
		" mcr    p15,0,r2,c14,c2,1        @ write CNTP_CTL \n\t"
		:
		:"r"(container), "r"(sw)
		:"r2", "r3", "r12");
	
	return;
}
#endif

void stop_generic_timer(void)
{
	/*
                                                 
                                                 
                                                  
                                            
  */
	write_cntpctl(read_cntpctl() & ~1);

	return;
}

void start_generic_timer(void)
{
	write_cntpctl(read_cntpctl() | 1);
	return;
}

//                        
#if defined (USE_NEW_GIC_DRV)

/*  
                                   
                                                  
                                                            
  */
#if !defined(CONFIG_CPU_PM) || !defined(CONFIG_ARM_GIC)

union gic_base {
	void __iomem *common_base;
	void __percpu __iomem **percpu_base;
};

struct gic_chip_data {
	union gic_base dist_base;
	union gic_base cpu_base;
	u32 saved_spi_enable[DIV_ROUND_UP(1020, 32)];
	u32 saved_spi_conf[DIV_ROUND_UP(1020, 16)];
	u32 saved_spi_target[DIV_ROUND_UP(1020, 4)];
	u32 __percpu *saved_ppi_enable;
	u32 __percpu *saved_ppi_conf;
	unsigned int gic_irqs;
};

#define gic_data_dist_base(d)	((d)->dist_base.common_base)
#define gic_data_cpu_base(d)	((d)->cpu_base.common_base)

#ifndef MAX_GIC_NR
#define MAX_GIC_NR	1
#endif

static struct gic_chip_data gic_data[MAX_GIC_NR] __read_mostly;


/*
                                                                              
                                                                            
                                                                         
                                                   
 */
static void gic_dist_save(void)
{
	unsigned int gic_irqs;
	void __iomem *dist_base;
	int i;

	gic_irqs = gic_data[0].gic_irqs;
	dist_base = gic_data_dist_base(&gic_data[0]);

	if (!dist_base)
		return;

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 16); i++)
		gic_data[0].saved_spi_conf[i] =
			readl_relaxed(dist_base + GIC_DIST_CONFIG + i * 4);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 4); i++)
		gic_data[0].saved_spi_target[i] =
			readl_relaxed(dist_base + GIC_DIST_TARGET + i * 4);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 32); i++)
		gic_data[0].saved_spi_enable[i] =
			readl_relaxed(dist_base + GIC_DIST_ENABLE_SET + i * 4);
}

/*
                                                                             
                                                                          
                                                                        
                                                                             
                                                                         
 */
static void gic_dist_restore(void)
{
	unsigned int gic_irqs;
	unsigned int i;
	void __iomem *dist_base;

	gic_irqs = gic_data[0].gic_irqs;
	dist_base = gic_data_dist_base(&gic_data[0]);

	if (!dist_base)
		return;

	writel_relaxed(0, dist_base + GIC_DIST_CTRL);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 16); i++)
		writel_relaxed(gic_data[0].saved_spi_conf[i],
			       dist_base + GIC_DIST_CONFIG + i * 4);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 4); i++)
		writel_relaxed(0xa0a0a0a0,
			       dist_base + GIC_DIST_PRI + i * 4);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 4); i++)
		writel_relaxed(gic_data[0].saved_spi_target[i],
			       dist_base + GIC_DIST_TARGET + i * 4);

	for (i = 0; i < DIV_ROUND_UP(gic_irqs, 32); i++)
		writel_relaxed(gic_data[0].saved_spi_enable[i],
			       dist_base + GIC_DIST_ENABLE_SET + i * 4);

	writel_relaxed(1, dist_base + GIC_DIST_CTRL);
}

static void gic_cpu_save(void)
{
	int i;
	u32 *ptr;
	void __iomem *dist_base;
	void __iomem *cpu_base;


	dist_base = gic_data_dist_base(&gic_data[0]);
	cpu_base = gic_data_cpu_base(&gic_data[0]);

	if (!dist_base || !cpu_base)
		return;

	ptr = __this_cpu_ptr(gic_data[0].saved_ppi_enable);
	for (i = 0; i < DIV_ROUND_UP(32, 32); i++)
		ptr[i] = readl_relaxed(dist_base + GIC_DIST_ENABLE_SET + i * 4);

	ptr = __this_cpu_ptr(gic_data[0].saved_ppi_conf);
	for (i = 0; i < DIV_ROUND_UP(32, 16); i++)
		ptr[i] = readl_relaxed(dist_base + GIC_DIST_CONFIG + i * 4);

}

static void gic_cpu_restore(void)
{
	int i;
	u32 *ptr;
	void __iomem *dist_base;
	void __iomem *cpu_base;

	dist_base = gic_data_dist_base(&gic_data[0]);
	cpu_base = gic_data_cpu_base(&gic_data[0]);

	if (!dist_base || !cpu_base)
		return;

	ptr = __this_cpu_ptr(gic_data[0].saved_ppi_enable);
	for (i = 0; i < DIV_ROUND_UP(32, 32); i++)
		writel_relaxed(ptr[i], dist_base + GIC_DIST_ENABLE_SET + i * 4);

	ptr = __this_cpu_ptr(gic_data[0].saved_ppi_conf);
	for (i = 0; i < DIV_ROUND_UP(32, 16); i++)
		writel_relaxed(ptr[i], dist_base + GIC_DIST_CONFIG + i * 4);

	for (i = 0; i < DIV_ROUND_UP(32, 4); i++)
		writel_relaxed(0xa0a0a0a0, dist_base + GIC_DIST_PRI + i * 4);

	writel_relaxed(0xf0, cpu_base + GIC_CPU_PRIMASK);
	writel_relaxed(1, cpu_base + GIC_CPU_CTRL);
}


static void __init gic_pm_init(struct gic_chip_data *gic)
{
	gic->saved_ppi_enable = __alloc_percpu(DIV_ROUND_UP(32, 32) * 4,
					       sizeof(u32));
	BUG_ON(!gic->saved_ppi_enable);

	gic->saved_ppi_conf = __alloc_percpu(DIV_ROUND_UP(32, 16) * 4,
					     sizeof(u32));
	BUG_ON(!gic->saved_ppi_conf);
}


static void __init mt_gic_init_bases(void)
{
	struct gic_chip_data *gic;
	unsigned int gic_irqs;

	gic = &gic_data[0];

	//                                           
	gic->dist_base.common_base = (void *)GIC_DIST_BASE; 
	gic->cpu_base.common_base = (void *)GIC_CPU_BASE;

	/*
                                               
                                                       
  */
	gic_irqs = readl_relaxed(gic_data_dist_base(gic) + GIC_DIST_CTR) & 0x1f;
	gic_irqs = (gic_irqs + 1) * 32;
	if (gic_irqs > 1020)
		gic_irqs = 1020;
	gic->gic_irqs = gic_irqs;

	gic_pm_init(gic);
}

#endif //                                                       
#else //                             

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
	const char padding3[256];                           /*       */
	volatile unsigned int non_security_access_control[64];       /*       */
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
	volatile unsigned int aliased_highest_pending;      /*      */
} cpu_interface;

extern unsigned *copy_words(volatile unsigned *destination, volatile unsigned *source, unsigned num_words);


typedef struct ns_gic_cpu_context {
	unsigned int gic_cpu_if_regs[32];   /*                              */
	unsigned int gic_dist_if_pvt_regs[32];  /*                                      */
	unsigned int gic_dist_if_regs[512]; /*                                                      */
} gic_cpu_context;

gic_cpu_context gic_data[1];
#define gic_data_base() ((gic_cpu_context *)&gic_data[0])




/*
                                      
                             
 */
static void save_gic_interface(u32 *pointer, unsigned gic_interface_address)
{
	cpu_interface *ci = (cpu_interface *)gic_interface_address;

	pointer[0] = ci->control;
	pointer[1] = ci->priority_mask;
	pointer[2] = ci->binary_point;
	pointer[3] = ci->aliased_binary_point;

	pointer[4] = ci->aliased_highest_pending;

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

	/*                                                 */
	*pointer = id->non_security_access_control[0] & 0x0ffff;
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

		pointer = copy_words(pointer, id->non_security_access_control+1, num_spis / 16);
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

	ci->aliased_highest_pending = pointer[4];

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

	/*                                                   */
	id->non_security_access_control[0] = 
                (id->non_security_access_control[0] & 0x0ffff0000) | (*pointer) ;
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


#define _KP_IRQ_BIT_ID (194)
#define _CONN_WDT_IRQ_BIT_ID 268 
#define _LOWBATTERY_IRQ_BIT_ID 108
#define _MD2_WDT_IRQ_BIT_ID 207
#define _MD_WDT_IRQ_BIT_ID 264


static void restore_gic_spm_irq(unsigned gic_distributor_address)
{
	interrupt_distributor *id = (interrupt_distributor *)gic_distributor_address;
	unsigned int backup;
	int i, j;

	/*                                                                       */
        
	backup = id->control;
        id->control = 0;

        
        /*                                                                 */ 
        if (reg_read(SPM_SLEEP_ISR_RAW_STA) & WAKE_SRC_KP) {
                i = _KP_IRQ_BIT_ID / GIC_PRIVATE_SIGNALS;
                j = _KP_IRQ_BIT_ID % GIC_PRIVATE_SIGNALS;
                id->pending.set[i] |= (1 << j);
        }
        if (reg_read(SPM_SLEEP_ISR_RAW_STA) & WAKE_SRC_MD_WDT) {
                i = _MD_WDT_IRQ_BIT_ID / GIC_PRIVATE_SIGNALS;
                j = _MD_WDT_IRQ_BIT_ID % GIC_PRIVATE_SIGNALS;
                id->pending.set[i] |= (1 << j);
        }
        if (reg_read(SPM_SLEEP_ISR_RAW_STA) & WAKE_SRC_LOW_BAT) {
                i = _LOWBATTERY_IRQ_BIT_ID / GIC_PRIVATE_SIGNALS;
                j = _LOWBATTERY_IRQ_BIT_ID % GIC_PRIVATE_SIGNALS;
                id->pending.set[i] |= (1 << j);
        }
#if 0 //     
        if (reg_read(SPM_SLEEP_ISR_RAW_STA) & WAKE_SRC_WDT) {
                i = _WDT_IRQ_BIT_ID / GIC_PRIVATE_SIGNALS;
                j = _WDT_IRQ_BIT_ID % GIC_PRIVATE_SIGNALS;
                id->pending.set[i] |= (1 << j);
        }
#endif


	/*                                                                                           */
	/*                          */
	id->control = backup;
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
		pointer += num_spis / 32;

		copy_words(id->non_security_access_control + 1, pointer, num_spis / 16);
		pointer += num_spis / 16;


                restore_gic_spm_irq(gic_distributor_address);

	}

	/*                                                                                           */
	/*                          */
	id->control = *pointer;
}



static void gic_cpu_save(void)
{
	save_gic_interface(gic_data_base()->gic_cpu_if_regs, GIC_CPU_BASE);
	/*
         
                                                        
                                                     
                                                     
                                                       
                                      
  */
	/*                                             */
	save_gic_distributor_private(gic_data_base()->gic_dist_if_pvt_regs, GIC_DIST_BASE);
}

static void gic_dist_save(void)
{
	/*                                            */
	save_gic_distributor_shared(gic_data_base()->gic_dist_if_regs, GIC_DIST_BASE); 
}

static void gic_dist_restore(void)
{
	/*                             */
	restore_gic_distributor_shared(gic_data_base()->gic_dist_if_regs, GIC_DIST_BASE);
}

void gic_cpu_restore(void)
{
	/*                              */
	restore_gic_distributor_private(gic_data_base()->gic_dist_if_pvt_regs, GIC_DIST_BASE);
	/*                     */
	restore_gic_interface(gic_data_base()->gic_cpu_if_regs, GIC_CPU_BASE);
}

#endif //                             
/*                                                                      */

inline void cpu_ddvm(int set)
{
	__asm__ __volatile__ (
		"mrc p15, 0, r0, c1, c0, 1 	\n\t"
		"teq %0, #0 			\n\t"
		"orrne r0, #(1<<15) 		\n\t"
		"biceq r0, #(1<<15) 		\n\t"
		"mcr p15, 0, r0, c1, c0, 1 	\n\t"
		"dsb 				\n\t"
		:
		:"r"(set)
		:"r0", "cc");
}

static inline void cci_dvm_snoop_disable(void)
{
	register int val;
        int cluster = cluster_id();

	//                                                                 
	//             
	
	if (cluster == 0) {
		//                                                    
		val = reg_read(CCI400_MP0_SNOOP_CTLR);
		val = _and(val, ~0x03);
		reg_write(CCI400_MP0_SNOOP_CTLR,  val);
		while (reg_read(CCI400_STATUS) & 0x1);


		
		/*                                                
                                                                      
                                                                  
    */
		val = reg_read(MP0_AXI_CONFIG);
		val = _or(val, ACINACTM);
		reg_write(MP0_AXI_CONFIG,  val);
	}
#if 0 //                                                            
        else /*                   */ {
		//                                                     
		val = reg_read(CCI400_MP1_SNOOP_CTLR);
		val = _and(val, ~0x03);
		reg_write(CCI400_MP1_SNOOP_CTLR,  val);
		while (reg_read(CCI400_STATUS) & 0x1);
		
		//         
		val = reg_read(MP1_AXI_CONFIG);
		val = _or(val, ACINACTM);
		reg_write(MP1_AXI_CONFIG,  val);
	}
#endif //     

}

static inline void cci_dvm_snoop_enable(void)
{
	int val;
	if (cluster_id() == 0) {
		/*                                                
                                                                      
                                                                  
    */
		val = reg_read(MP0_AXI_CONFIG);
		val = _and(val, ~(ACINACTM));
		reg_write(MP0_AXI_CONFIG,  val);


		//                                                    
		val = reg_read(CCI400_MP0_SNOOP_CTLR);
		val = _or(val, 0x3);
		reg_write(CCI400_MP0_SNOOP_CTLR,  val);
		while (reg_read(CCI400_STATUS) & 0x1);
		
	}
#if 0 //                                                   
	else /*                        */ {
		//         
		val = reg_read(MP1_AXI_CONFIG);
		val = _and(val, ~(ACINACTM));
		reg_write(MP1_AXI_CONFIG,  val);

		//                                                     
		val = reg_read(CCI400_MP1_SNOOP_CTLR);
		val = _or(val, 0x3);
		reg_write(CCI400_MP1_SNOOP_CTLR,  val);
		while (reg_read(CCI400_STATUS) & 0x1);
		
	}
#endif //     

	//                                                                
//              


}

#if !defined (CONFIG_ARM_PSCI)

DEFINE_SPINLOCK(mp0_l2rstd_lock);
DEFINE_SPINLOCK(mp1_l2rstd_lock);

static inline void mp0_l2rstdisable(int flags)
{
        unsigned int read_back;
        int reg_val;

        spin_lock(&mp0_l2rstd_lock); //                    

        read_back = reg_read(MP0_CA7L_CACHE_CONFIG);
        reg_val = _aor(read_back, ~L2RSTDISABLE, IS_DORMANT_INNER_OFF(flags) ? 0: L2RSTDISABLE ); 

	reg_write(MP0_CA7L_CACHE_CONFIG, reg_val);
	
        if (GET_CLUSTER_DATA()->l2rstdisable_rfcnt++ == 0)
                GET_CLUSTER_DATA()->l2rstdisable = read_back & L2RSTDISABLE;

        spin_unlock(&mp0_l2rstd_lock);

}

static inline void mp1_l2rstdisable(int flags)
{
        unsigned int read_back;
        int reg_val;

        spin_lock(&mp1_l2rstd_lock); //                     
        
        read_back = reg_read(MP1_CA7L_CACHE_CONFIG);
        reg_val = _aor(read_back, ~L2RSTDISABLE, IS_DORMANT_INNER_OFF(flags) ? 0: L2RSTDISABLE);

	reg_write(MP1_CA7L_CACHE_CONFIG, reg_val);
	
        if (GET_CLUSTER_DATA()->l2rstdisable_rfcnt++ == 0)
                GET_CLUSTER_DATA()->l2rstdisable = read_back & L2RSTDISABLE;

        spin_unlock(&mp1_l2rstd_lock);
}


static inline void mp0_l2rstdisable_restore(int flags) 
{
        unsigned int read_back;
        int reg_val;

        spin_lock(&mp0_l2rstd_lock); //                     
        GET_CLUSTER_DATA()->l2rstdisable_rfcnt--;
        if (GET_CLUSTER_DATA()->l2rstdisable_rfcnt == 0) {
                read_back = reg_read(MP0_CA7L_CACHE_CONFIG);
                reg_val = _aor(read_back, ~L2RSTDISABLE, GET_CLUSTER_DATA()->l2rstdisable);  //

                reg_write(MP0_CA7L_CACHE_CONFIG, reg_val);
        }

        spin_unlock(&mp0_l2rstd_lock); //                     
}

static inline void mp1_l2rstdisable_restore(int flags)
{
        unsigned int read_back;
        int reg_val;

        spin_lock(&mp1_l2rstd_lock); //                     
        GET_CLUSTER_DATA()->l2rstdisable_rfcnt--;
        if (GET_CLUSTER_DATA()->l2rstdisable_rfcnt == 0) {
                read_back = reg_read(MP1_CA7L_CACHE_CONFIG);
                reg_val = _aor(read_back, ~L2RSTDISABLE, GET_CLUSTER_DATA()->l2rstdisable);

                reg_write(MP1_CA7L_CACHE_CONFIG, reg_val);
        }
        spin_unlock(&mp1_l2rstd_lock); //                     
}
#else //                              

static inline void mp0_l2rstdisable(int flags) {} 
static inline void mp1_l2rstdisable(int flags) {}
static inline void mp0_l2rstdisable_restore(int flags) {} 
static inline void mp1_l2rstdisable_restore(int flags) {}

#endif //                              


#if 0
static inline unsigned int ca7_delayip_clock_on(int on)
{
	unsigned int read_back = reg_read(CA7_MCU_CONFIG);
	int reg_val = _aor(read_back, ~PD_SW_CG_EN, (on) ? PD_SW_CG_EN : 0);
	reg_write(CA7_MCU_CONFIG, reg_val);
	
	return reg_val;
}
#endif


/*                       */
static void mt_cluster_save(int flags)
{
	/*                                     */
        /*                              */
	/*                                     */
	/*                             */
        /*                                     */

	if (cluster_id() == 0) {
		mp0_l2rstdisable(flags);
	} 
        else {
		mp1_l2rstdisable(flags);
	}
}

/*                       */
static void mt_cluster_restore(int flags)
{
	int cpuid, clusterid;


	/*                                   */
        /*                                 */
	/*                 */
        /*                                   */

	read_id(&cpuid, &clusterid);


	if (cluster_id() == 0) {
		mp0_l2rstdisable_restore(flags);
	} 
        else {
		mp1_l2rstdisable_restore(flags);
	}
}


#if 0
static inline unsigned int *mt_save_dbg_regs(unsigned int *p, unsigned long dbg_base)
{
        return p;
}

static inline void mt_restore_dbg_regs(unsigned int *p, unsigned long dbg_base)
{
        return;
}

static inline void mt_copy_dbg_regs(unsigned long dbg_base, int to, int from)
{
        return;
}
#endif

void mt_cpu_save(void)
{	
	core_context *core;
        cluster_context *cluster;
	unsigned int *ret;
        unsigned long dbg_base;
        unsigned int sleep_sta;
        int cpuid, clusterid;

	read_id(&cpuid, &clusterid);
	

	core = GET_CORE_DATA();

	ret = mt_save_generic_timer(core->timer_data, 0x0); 
	stop_generic_timer(); //                                                       
	
	SENTINEL_CHECK(core->timer_data, ret);

	ret = save_pmu_context(core->pmu_data);
	SENTINEL_CHECK(core->pmu_data, ret);


	ret = save_vfp(core->vfp_data);
	SENTINEL_CHECK(core->vfp_data, ret);

	/*        
                                                                                     
                                                                                             
                                               
                                                                 
          
                                                                
                                           
          */
        if (clusterid==0) {
                sleep_sta = (spm_read(SPM_SLEEP_TIMER_STA) >> 16) & 0x0f;
                dbg_base = MP0_DBGAPB_BASE;
        } else { 
                sleep_sta = (spm_read(SPM_SLEEP_TIMER_STA) >> 20) & 0x0f;
                dbg_base = MP1_DBGAPB_BASE;
        }
         
        if ((sleep_sta | (1<<cpuid)) == 0x0f) { //          
                cluster = GET_CLUSTER_DATA();
                ret = mt_save_dbg_regs(cluster->dbg_data, cpuid + (clusterid*4));
                SENTINEL_CHECK(cluster->dbg_data, ret);
        }
        else {
                /*              */
        }

        ret = mt_save_banked_registers(core->banked_regs);
	SENTINEL_CHECK(core->banked_regs, ret);
}

void mt_cpu_restore(void)
{
	core_context *core;
        cluster_context *cluster;
        unsigned long dbg_base;
        unsigned int sleep_sta;
        int cpuid, clusterid;

	read_id(&cpuid, &clusterid);
	

	core = GET_CORE_DATA();

        mt_restore_banked_registers(core->banked_regs);
	
	/*        
                                                                                     
                                                                                             
                                               
                                                                 
          
                                                                
                                           
          */
        if (clusterid == 0) {
                sleep_sta = (spm_read(SPM_SLEEP_TIMER_STA) >> 16) & 0x0f;
                dbg_base = MP0_DBGAPB_BASE;
        } else { 
                sleep_sta = (spm_read(SPM_SLEEP_TIMER_STA) >> 20) & 0x0f;
                dbg_base = MP1_DBGAPB_BASE;
        }
         
        sleep_sta = (sleep_sta | (1<<cpuid)); 

        if (sleep_sta == 0x0f) { //           
                cluster = GET_CLUSTER_DATA();
                mt_restore_dbg_regs(cluster->dbg_data, cpuid + (clusterid*4));
        }
        else {  //                              
                int any = __builtin_ffs(~sleep_sta) -1;
                mt_copy_dbg_regs(cpuid + (clusterid*4), any + (clusterid*4));
        }

        
	restore_vfp(core->vfp_data);
	
	restore_pmu_context(core->pmu_data);

	mt_restore_generic_timer(core->timer_data, 0x0);

}

void mt_platform_save_context(int flags)
{
	/*                      */
	mt_cpu_save();
	mt_cluster_save(flags);

	if (IS_DORMANT_GIC_OFF(flags)) {
		//                   
		gic_cpu_save();
		gic_dist_save();
	}

	/*                        */
	/*                    */

}


void mt_platform_restore_context(int flags)
{
	/*                       */
	/*                           */

	/*                         */
	mt_cluster_restore(flags);
	mt_cpu_restore();

	if (IS_DORMANT_GIC_OFF(flags)) {
		gic_dist_restore();
		gic_cpu_restore();
	}

}


/* 
                                                                              
  */
#define disable_dcache_safe(inner_off, BARRIER_AFTER_COP)					\
	__asm__ __volatile__ (						\
		"MRC p15,0,r0,c1,c0,0 	\n\t"				\
		"dsb			\n\t"				\
		"BIC r0,r0,#4		\n\t"				\
		"MCR p15,0,r0,c1,c0,0	\n\t"				\
		"dsb			\n\t"				\
		"isb			\n\t"				\
		/*                                                                                                             */ \
		/*                                                                                                                                                             */ \
		/*                                             */ \
		/*                               */ \
		"MCR p15,0,r0,c8,c7,1	\n\t"				\
		"dsb			\n\t"				\
									\
		/*             */					\
		"mrc     p15, 1, r0, c0, c0, 1           @ read clidr \n\t" \
		"ands    r3, r0, #0x7000000              @ extract loc from clidr \n\t"	\
		"mov     r3, r3, lsr #23                 @ left align loc bit field \n\t" \
		"beq     L1_finished                        @ if loc is 0, then no need to clean \n\t" \
		"mov     r10, #0                         @ start clean at cache level 1 \n\t" \
		"L1_loop1: \n\t"					\
		"add     r2, r10, r10, lsr #1            @ work out 3x current cache level \n\t" \
		"mov     r1, r0, lsr r2                  @ extract cache type bits from clidr \n\t" \
		"and     r1, r1, #7                      @ mask of the bits for current cache only \n\t" \
		"cmp     r1, #2                          @ see what cache we have at this level \n\t" \
		"blt     L1_skip                            @ skip if no cache, or just i-cache \n\t" \
		"mcr     p15, 2, r10, c0, c0, 0          @ select current cache level in cssr \n\t" \
		"isb                                     @ isb to sych the new cssr&csidr \n\t"	\
		"mrc     p15, 1, r1, c0, c0, 0           @ read the new csidr \n\t" \
		"and     r2, r1, #7                      @ extract the length of the cache lines \n\t" \
		"add     r2, r2, #4                      @ add 4 (line length offset) \n\t" \
		/*                           */				\
		"mov	r4, #0x400 \n\t"				\
		"sub	r4, #1 \n\t"					\
		"ands    r4, r4, r1, lsr #3              @ find maximum number on the way size \n\t" \
		"clz     r5, r4                          @ find bit position of way size increment \n\t" \
		/*                            */			\
		"mov	r7, #0x8000 \n\t"				\
		"sub	r7, #1 \n\t"					\
		"ands    r7, r7, r1, lsr #13             @ extract max number of the index size \n\t" \
		"L1_loop2: \n\t"					\
		"mov     r9, r4                          @ create working copy of max way size \n\t" \
		"L1_loop3: \n\t"					\
		"orr     r6, r10, r9, lsl r5            @ factor way and cache number into r6 \n\t" \
		"orr     r6, r6, r7, lsl r2            @ factor index number into r6 \n\t" \
		"mcr     p15, 0, r6, c7, c14, 2         @ clean & invalidate by set/way \n\t" \
		/*                                                                   */	\
		/*                                                                       */ \
                BARRIER_AFTER_COP                                     \
		"subs    r9, r9, #1                      @ decrement the way \n\t" \
		"bge     L1_loop3 \n\t"					\
		"subs    r7, r7, #1                      @ decrement the index \n\t" \
		"bge     L1_loop2 \n\t"					\
		"L1_skip: \n\t"						\
		"@add     r10, r10, #2                    @ increment cache number \n\t" \
		"@cmp     r3, r10 \n\t"					\
		"@bgt     L1_loop1 \n\t"				\
		"L1_finished: \n\t"					\
		"mov     r10, #0                         @ swith back to cache level 0 \n\t" \
		"mcr     p15, 2, r10, c0, c0, 0          @ select current cache level in cssr \n\t" \
		"dsb \n\t"						\
		"isb \n\t"						\
									\
									\
		/*                   */					\
		"mrc     p15, 1, r0, c0, c0, 1           @ read clidr \n\t" \
		"isb \n\t"						\
		"ands    r3, r0, #0x7000000              @ extract loc from clidr \n\t"	\
		"mov     r3, r3, lsr #23                 @ left align loc bit field \n\t" \
		"beq     L2_cl_finished                        @ if loc is 0, then no need to clean \n\t" \
		"mov     r10, #2                         @ start clean at cache level 2 \n\t" \
		"L2_cl_loop1: \n\t"					\
		"add     r2, r10, r10, lsr #1            @ work out 3x current cache level \n\t" \
		"mov     r1, r0, lsr r2                  @ extract cache type bits from clidr \n\t" \
		"and     r1, r1, #7                      @ mask of the bits for current cache only \n\t" \
		"cmp     r1, #2                          @ see what cache we have at this level \n\t" \
		"blt     L2_cl_skip                            @ skip if no cache, or just i-cache \n\t" \
		"mcr     p15, 2, r10, c0, c0, 0          @ select current cache level in cssr \n\t" \
		"isb                                     @ isb to sych the new cssr&csidr \n\t"	\
		"mrc     p15, 1, r1, c0, c0, 0           @ read the new csidr \n\t" \
		"isb \n\t"						\
		"and     r2, r1, #7                      @ extract the length of the cache lines \n\t" \
		"add     r2, r2, #4                      @ add 4 (line length offset) \n\t" \
		/*                           */				\
		"mov	r4, #0x400 \n\t"				\
		"sub	r4, #1 \n\t"					\
		"ands    r4, r4, r1, lsr #3              @ find maximum number on the way size \n\t" \
		"clz     r5, r4                          @ find bit position of way size increment \n\t" \
		/*                            */			\
		"mov	r7, #0x8000 \n\t"				\
		"sub	r7, #1 \n\t"					\
		"ands    r7, r7, r1, lsr #13             @ extract max number of the index size \n\t" \
		"L2_cl_loop2: \n\t"					\
		"mov     r9, r4                          @ create working copy of max way size \n\t" \
		"L2_cl_loop3: \n\t"					\
		"orr     r6, r10, r9, lsl r5            @ factor way and cache number into r6 \n\t" \
		"orr     r6, r6, r7, lsl r2            @ factor index number into r6 \n\t" \
		"teq 	 %0, #0 							\n\t" \
		"mcreq     p15, 0, r6, c7, c10, 2         @ clean by set/way \n\t" \
		"mcrne     p15, 0, r6, c7, c14, 2         @ flush by set/way \n\t" \
                BARRIER_AFTER_COP                                     \
		"subs    r9, r9, #1                      @ decrement the way \n\t" \
		"bge     L2_cl_loop3 \n\t"				\
		"subs    r7, r7, #1                      @ decrement the index \n\t" \
		"bge     L2_cl_loop2 \n\t"				\
		"L2_cl_skip: \n\t"					\
		"@add     r10, r10, #2                    @ increment cache number \n\t" \
		"@cmp     r3, r10 \n\t"					\
		"@bgt     L2_cl_loop1 \n\t"				\
		"L2_cl_finished: \n\t"					\
		"mov     r10, #0                         @ swith back to cache level 0 \n\t" \
		"mcr     p15, 2, r10, c0, c0, 0          @ select current cache level in cssr \n\t" \
		"dsb \n\t"						\
		"isb \n\t"						\
		:							\
		: "r"(inner_off)					\
		:"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r9", "r10" );


#define _get_sp() ({ register void *SP asm("sp"); SP; })

int mt_cpu_dormant_reset(unsigned long flags)
{
	register int ret =1; //             

	int cpuid, clusterid;

#if defined(TSLOG_ENABLE)
        core_context *core = GET_CORE_DATA();
        TSLOG(core->timestamp[1], read_cntpct());
#endif

	read_id(&cpuid, &clusterid);

	disable_dcache_safe(!!IS_DORMANT_INNER_OFF(flags), "\n\t");

        /*          
                               
                                                        
                                                                       
                                                                                     
                              
           */
#if defined(TSLOG_ENABLE)
        __cpuc_flush_dcache_area(core->timestamp, sizeof(core->timestamp));
        TSLOG(core->timestamp[2], read_cntpct());
        dsb();
#endif
		
	
        PCLOG(clusterid*4 + cpuid);	
	if ((unlikely(IS_DORMANT_BREAK_CHECK(flags)) &&
             unlikely(SPM_IS_CPU_IRQ_OCCUR(SPM_CORE_ID()))))
        {
		ret =  2; //             
		goto _break0;
	}

        PCLOG(clusterid*4 + cpuid);
	if (unlikely(IS_DORMANT_SNOOP_OFF(flags))) {
		//                               
		cci_dvm_snoop_disable();
	}	

        PCLOG(clusterid*4 + cpuid);

	amp();  //              

	wfi(); //                                                  

#if 0    //             
        if (!(read_isr() & (0x7<<6))) {
                setup_mm_for_reboot();  //         
                cpu_reset((long)cpu_resume);

                BUG();
	}	
#endif
        

	smp(); //              

        PCLOG(clusterid*4 + cpuid);

	if (unlikely(IS_DORMANT_SNOOP_OFF(flags))) {
		cci_dvm_snoop_enable();
	}

_break0:

        PCLOG(clusterid*4 + cpuid);
        __enable_dcache();

        PCLOG(clusterid*4 + cpuid);

//        

	return ret; //             
}

#if defined (CONFIG_ARM_PSCI)
int mt_cpu_dormant_psci(unsigned long flags)
{
        int ret = 1;
        const struct psci_power_state pps = {
                .type = PSCI_POWER_STATE_TYPE_POWER_DOWN,
                .affinity_level = 2,
        };

        if (psci_ops.cpu_suspend) {

                ret = psci_ops.cpu_suspend(pps, virt_to_phys(cpu_resume));

                if (ret) { //     
                        ret = 1;
                }
        }

        return ret;
}
#endif //                             


static int mt_cpu_dormant_abort(unsigned long flags)
{
	int cpuid, clusterid;

	read_id(&cpuid, &clusterid);


	/*                              */
	if (cluster_id() == 0) {
		mp0_l2rstdisable_restore(flags);
	} 
        else {
		mp1_l2rstdisable_restore(flags);
	}

	//                     
	start_generic_timer();

        /*                                 */
	/*                          */
        /*        */
	/*                          */
		
	return 0; 
}

/*                                                                         */
/*                                            */
/*                              */
/*                                             */
/*                                                                     */
/*             */
/*                                                         */
/*                                           */
/*                             */
/*                                                                         */
#if !defined (__KERNEL__)
int cpu_pm_resume(void)
{
	return 0;
}

int cpu_pm_suspend(void)
{
	return 0;

}
#else //                         
int cpu_pm_resume(void) {  /*               */;  return 0; }
int cpu_pm_suspend(void) {  /*                */;  return 0; }

#endif //                         

#define NOMMU(a) (((a) & ~0xf0000000) | 0x10000000)
#define get_data_nommu(va)  \
        ({                                              \
                register int data;                      \
                register unsigned long pva = (unsigned long)(void *)(&(va)); \
                __asm__ __volatile__ (                  \
                        "adr r1, 1f 	\n\t"           \
                        "ldr r2, [r1]   \n\t"           \
                        "sub r2, %1, r2 \n\t"           \
                        "add r3, r1, r2	\n\t"           \
                        "ldr %0, [r3]  	\n\t"           \
                        "b 3f \n\t"                     \
                        "1: .long . \n\t"               \
                        "3: \n\t"                       \
                        :"=r"(data)                     \
                        : "r"(pva)                      \
                        : "r1", "r2", "r3");            \
                data;                                   \
        })

/*                                          
                                                      
                             
          
                                        
   
           
                                                                             
      
  */
__naked void cpu_resume_wrapper(void)
{
	register int val;
	register int val1;

	/*                                   */
        /*                                 */
	/*                                 */
	/*                          */
        /*                                   */

        
        /*                           
                                                                          
                                                                        
                                                                          
                                 
          
          */
                val = get_data_nommu(dormant_data[0].poc.l2ectlr);
                val1 = get_data_nommu(dormant_data[0].poc.l2actlr);
                if (val) {
                        __asm__ __volatile__ (
                                "isb; dsb \n\t"
                                "MCR p15, 1, %0, c9, c0, 3   @ write L2ECTLR \n\t"
                                "mcr p15, 1, %1, c15, c0, 0  @ write L2ACTLR \n\t" 
                                "dsb; isb \n\t"
                                :
                                :"r"(val), "r"(val1)
                                :);
                }

        //                  
        val = reg_read(NOMMU(MP0_AXI_CONFIG));
        val = _and(val, ~(ACINACTM));
        reg_write(NOMMU(MP0_AXI_CONFIG),  val);


        //                                                     
        val = reg_read(NOMMU(CCI400_MP0_SNOOP_CTLR));
        val = _or(val, 0x3);
        reg_write(NOMMU(CCI400_MP0_SNOOP_CTLR),  val);
        while (reg_read(NOMMU(CCI400_STATUS)) & 0x1);



        //             
	//                     
	__asm__ __volatile__ (
		"adr r0, 1f 	\n\t"
		"ldr r2, [r0] \n\t"
                "sub r2, %0, r2 \n\t"
		"add r3, r0, r2	\n\t"
		"ldr r3, [r3]  	\n\t"
		"bx r3		\n\t"
		"1: 		\n\t"
		".long  . \n\t"
                :
                : "r" (&(dormant_data[0].poc.cpu_resume_phys))
                : "r0", "r2", "r3");
}


int mt_cpu_dormant(unsigned long flags)
{
	int ret;
	int cpuid, clusterid;
        static unsigned int dormant_count;
	core_context *core = GET_CORE_DATA();

        if (mt_dormant_initialized == 0)
                return MT_CPU_DORMANT_BYPASS;


	read_id(&cpuid, &clusterid);

        dormant_count++;
#if defined(TSLOG_ENABLE)
        if ((core->count & 0x01ff) == 0) 
                CPU_DORMANT_INFO("dormant(%d) flags:%x start (cluster/cpu:%d/%d) !\n", 
                                 dormant_count, flags, clusterid, cpuid);
#endif

	//                           
	if (DEBUG_DORMANT_BYPASS || debug_dormant_bypass == 1)
		return MT_CPU_DORMANT_BYPASS;

        TSLOG(core->timestamp[0], read_cntpct());
        core->count++;

	BUG_ON(!irqs_disabled());



	//                    
	if (cpu_pm_suspend()) {
		ret = MT_CPU_DORMANT_BREAK_V(CPU_PM_BREAK);
		goto _back;
	}

	//              
	if (IS_DORMANT_BREAK_CHECK(flags) && 
	    SPM_IS_CPU_IRQ_OCCUR(SPM_CORE_ID())) {
		ret = MT_CPU_DORMANT_BREAK_V(IRQ_PENDING_1);
		goto _back;
	}

	//
	mt_platform_save_context(flags);


	//              
	if (IS_DORMANT_BREAK_CHECK(flags) && 
	    SPM_IS_CPU_IRQ_OCCUR(SPM_CORE_ID())) {
		mt_cpu_dormant_abort(flags);
		ret = MT_CPU_DORMANT_BREAK_V(IRQ_PENDING_2);
		goto _back;
	}

	//                                               
#if !defined (CONFIG_ARM_PSCI)        
	/*                  */
	if (unlikely(IS_DORMANT_SNOOP_OFF(flags))) {
		/*                 
                                                                                   
    */
		dormant_data[0].poc.cpu_resume_phys = (void (*)(void))(long)virt_to_phys(cpu_resume);
		mt_smp_set_boot_addr(virt_to_phys(cpu_resume_wrapper), clusterid*4+cpuid);
	}
	else {
		mt_smp_set_boot_addr(virt_to_phys(cpu_resume), clusterid*4+cpuid);
	}

	ret = cpu_suspend(flags, mt_cpu_dormant_reset);
#else //                                     
	ret = cpu_suspend(flags, mt_cpu_dormant_psci);
#endif //                                     

//                                           
        TSLOG(core->timestamp[3], read_cntpct());
		
	switch (ret) {
	case 0: //                        
		mt_platform_restore_context(flags);
#ifdef CONFIG_MTK_ETM
                trace_start_dormant();
#endif    
                core->rst++;
		ret = MT_CPU_DORMANT_RESET;
		break;

	case 1: //                         
		mt_cpu_dormant_abort(flags);
                core->abt++;
		ret = MT_CPU_DORMANT_ABORT;
		break;
		
	default: //                                               
	case 2:
		mt_cpu_dormant_abort(flags);
                core->brk++;
		ret = MT_CPU_DORMANT_BREAK_V(IRQ_PENDING_3);
		break;
	}

        cpu_pm_resume();
        local_fiq_enable();  /*                                                  */

_back:

        TSLOG(core->timestamp[4], read_cntpct());

#if defined(TSLOG_ENABLE)
        if (MT_CPU_DORMANT_BREAK & ret)
                CPU_DORMANT_INFO("dormant BREAK(%x) !! \n\t", ret);
        if (MT_CPU_DORMANT_ABORT & ret)
                CPU_DORMANT_INFO("dormant ABORT(%x) !! \n\t", ret);

        if ((_IS_DORMANT_SET(flags, DORMANT_CPUSYS_OFF))
            || (core->count & 0x01ff) == 0) {
                CPU_DORMANT_INFO("dormant(flags:%x) (ret:%x) (core:%d/%d) cnt:%d, rst:%d, abt:%d, brk:%d\n",
			 flags, ret, clusterid, cpuid, 
                                 core->count, core->rst, core->abt, core->brk);
	CPU_DORMANT_INFO("dormant timing: %llu, %llu, %llu, %llu, %llu\n",
                                 core->timestamp[0],
                                 core->timestamp[1],
                                 core->timestamp[2],
                                 core->timestamp[3],
                                 core->timestamp[4]);
        }       
#endif

	return ret & 0x0ff;

}

#if defined(TSLOG_ENABLE)
void mt_cpu_dormant_timestamp(int core_idx)
{
        
}
#endif

int mt_cpu_dormant_init(void)
{
	int cpuid, clusterid;
	read_id(&cpuid, &clusterid);
        
        if (mt_dormant_initialized == 1)
                return 0;
        
        aee_rr_rec_dmnt = dormant_data[0].poc.cpu_dormant_aee_rr_rec = aee_rr_rec_cpu_dormant();
        aee_rr_rec_dmnt_phys = (unsigned long *)(u32)virt_to_phys(aee_rr_rec_dmnt);

	//                                             
	reg_write(BOOTROM_PWR_CTRL, reg_read(BOOTROM_PWR_CTRL) | 0x80000000);

	//                  
        dormant_data[0].poc.chip_ver = mt_get_chip_sw_ver();

        if (1) { 
                __asm__ __volatile__(
                        "isb; dsb \n\t"
                        "mrc p15, 1, %0, c9, c0, 3 \n\t" 
                        "mrc p15, 1, %1, c15, c0, 0 \n\t" 
                        "isb \n\t"
                        "dsb \n\t"
                        : "=r" (dormant_data[0].poc.l2ectlr), "=r"(dormant_data[0].poc.l2actlr)
                        ::"memory");
                        
                zlog("dormant init (cluster/cpu:%d/%d), l2ectlr(%lx) l2actlr(%lx)!\n", 
                                 clusterid, cpuid, 
                                 dormant_data[0].poc.l2ectlr,
                                 dormant_data[0].poc.l2actlr);
        }       

#if defined (USE_NEW_GIC_DRV)
#if !defined(CONFIG_CPU_PM) || !defined(CONFIG_ARM_GIC)
	mt_gic_init_bases();
#endif //                                                       
#endif //                             

        mt_dormant_initialized = 1;

        return 0;
}

int mt_cpu_dormant_ready(void)
{
        extern void __iomem *cpuxgpt_regs;
        
        return (mt_dormant_initialized == 1); //                       
}


late_initcall(mt_cpu_dormant_init);

#if defined (MT_DORMANT_UT)
volatile int mt_cpu_dormant_test_mode = (
	CPU_MCDI_MODE | 
	0
	);


int mt_cpu_dormant_test(void)
{
	return mt_cpu_dormant(mt_cpu_dormant_test_mode);
}

void mt_cpu_dormant_init_test(void)
{

	mt_cpu_dormant_init();

	arm_pm_idle = mt_cpu_dormant_test;

}


#endif //                           


