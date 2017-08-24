/*
* Copyright (C) 2011-2014 MediaTek Inc.
* 
* This program is free software: you can redistribute it and/or modify it under the terms of the 
* GNU General Public License version 2 as published by the Free Software Foundation.
* 
* This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
* without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <asm/uaccess.h>

#include <linux/device.h>
#include <linux/platform_device.h>

#include <mach/mt_typedefs.h>
#include <mach/sync_write.h>
#include <mach/mt_clkmgr.h>
#include <mach/mt_dcm.h>
#include <mach/mt_spm.h>
#include <mach/mt_spm_mtcmos.h>
#include <mach/mt_spm_sleep.h>
#include <mach/mt_freqhopping.h>
#include <mach/mt_gpufreq.h>
#include <linux/earlysuspend.h>

//                   
//               
//               
//                   
#define MUX_LOG
//                   
#define PLL_LOG

/*                                               
                                                 
                                                */

#define USING_XLOG

#ifdef USING_XLOG 
#include <linux/xlog.h>

#define TAG     "Power/clkmgr"

#define clk_err(fmt, args...)       \
    xlog_printk(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define clk_warn(fmt, args...)      \
    xlog_printk(ANDROID_LOG_WARN, TAG, fmt, ##args)
#define clk_info(fmt, args...)      \
    xlog_printk(ANDROID_LOG_INFO, TAG, fmt, ##args)
#define clk_dbg(fmt, args...)       \
    xlog_printk(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define clk_ver(fmt, args...)       \
    xlog_printk(ANDROID_LOG_VERBOSE, TAG, fmt, ##args)

#else

#define TAG     "[Power/clkmgr] "

#define clk_err(fmt, args...)       \
    printk(KERN_ERR TAG);           \
    printk(KERN_CONT fmt, ##args) 
#define clk_warn(fmt, args...)      \
    printk(KERN_WARNING TAG);       \
    printk(KERN_CONT fmt, ##args)
#define clk_info(fmt, args...)      \
    printk(KERN_NOTICE TAG);        \
    printk(KERN_CONT fmt, ##args)
#define clk_dbg(fmt, args...)       \
    printk(KERN_INFO TAG);          \
    printk(KERN_CONT fmt, ##args)
#define clk_ver(fmt, args...)       \
    printk(KERN_DEBUG TAG);         \
    printk(KERN_CONT fmt, ##args)

#endif



/*                                               
                                                 
                                                */

#define clk_readl(addr) \
    DRV_Reg32(addr)

#define clk_writel(addr, val)   \
    mt65xx_reg_sync_writel(val, addr)

#define clk_setl(addr, val) \
    mt65xx_reg_sync_writel(clk_readl(addr) | (val), addr)

#define clk_clrl(addr, val) \
    mt65xx_reg_sync_writel(clk_readl(addr) & ~(val), addr)



/*                                               
                                                 
                                                */

//                          

struct pll;
struct pll_ops {
    int (*get_state)(struct pll *pll);
    //                              
    void (*enable)(struct pll *pll);
    void (*disable)(struct pll *pll);
    void (*fsel)(struct pll *pll, unsigned int value);
    int (*dump_regs)(struct pll *pll, unsigned int *ptr);
    unsigned int (*vco_calc)(struct pll *pll);
    int (*hp_enable)(struct pll *pll);
    int (*hp_disable)(struct pll *pll);
};

struct pll {
    const char *name;
    int type;
    int mode;
    int feat;
    int state;
    unsigned int cnt;
    unsigned int en_mask;
    unsigned int base_addr;
    unsigned int pwr_addr;
    struct pll_ops *ops;
    unsigned int hp_id;
    int hp_switch;
#ifdef CONFIG_CLKMGR_STAT
    struct list_head head;
#endif
};


struct subsys;
struct subsys_ops {
    int (*enable)(struct subsys *sys);
    int (*disable)(struct subsys *sys);
    int (*get_state)(struct subsys *sys);
    int (*dump_regs)(struct subsys *sys, unsigned int *ptr);
};

struct subsys {
    const char *name;
    int type;
    int force_on;
    unsigned int cnt;
    unsigned int state;
    unsigned int default_sta;
    unsigned int sta_mask;  //                   
    unsigned int ctl_addr;
    //                           
    struct subsys_ops *ops;
    struct cg_grp *start;
    unsigned int nr_grps;
    struct clkmux *mux;
#ifdef CONFIG_CLKMGR_STAT
    struct list_head head;
#endif
};


struct clkmux;
struct clkmux_ops {
    void (*sel)(struct clkmux *mux, unsigned int clksrc);
    void (*enable)(struct clkmux *mux);
    void (*disable)(struct clkmux *mux);
};

struct clkmux {
    const char *name;
    unsigned int cnt;
    unsigned int base_addr;
    unsigned int sel_mask;
    unsigned int pdn_mask;
    unsigned int offset;
    unsigned int nr_inputs;
    struct clkmux_ops *ops;
    struct clkmux *parent;
    struct clkmux *siblings;
    struct pll *pll;
#ifdef CONFIG_CLKMGR_STAT
    struct list_head head;
#endif
};


struct cg_grp;
struct cg_grp_ops {
    int (*prepare)(struct cg_grp *grp);
    int (*finished)(struct cg_grp *grp);
    unsigned int (*get_state)(struct cg_grp *grp);
    int (*dump_regs)(struct cg_grp *grp, unsigned int *ptr);
};

struct cg_grp {
    const char *name;
    unsigned int set_addr;
    unsigned int clr_addr;
    unsigned int sta_addr;
    unsigned int mask;
    unsigned int state;
    struct cg_grp_ops *ops;
    struct subsys *sys;
};


struct cg_clk;
struct cg_clk_ops {
    int (*get_state)(struct cg_clk *clk);  
    int (*check_validity)(struct cg_clk *clk);//                     
    int (*enable)(struct cg_clk *clk);
    int (*disable)(struct cg_clk *clk);
};

struct cg_clk {
    int cnt;
    unsigned int state;
    unsigned int mask;
    int force_on;
    struct cg_clk_ops *ops;
    struct cg_grp *grp;
    struct clkmux *mux;
    struct cg_clk *parent;
#ifdef CONFIG_CLKMGR_STAT
    struct list_head head;
#endif
};


#ifdef CONFIG_CLKMGR_STAT
struct stat_node {
    struct list_head link;
    unsigned int cnt_on;
    unsigned int cnt_off;
    char name[0];
};
#endif



/*                                               
                                                 
                                                */

#define PWR_DOWN    0
#define PWR_ON      1

static int initialized = 0;

static struct pll plls[NR_PLLS];
static struct subsys syss[NR_SYSS];
static struct clkmux muxs[NR_MUXS];
static struct cg_grp grps[NR_GRPS];
static struct cg_clk clks[NR_CLKS];



/*                                               
                                                 
                                                */

static DEFINE_SPINLOCK(clock_lock);

#define clkmgr_lock(flags)  \
do {    \
    spin_lock_irqsave(&clock_lock, flags);  \
} while (0)

#define clkmgr_unlock(flags)  \
do {    \
    spin_unlock_irqrestore(&clock_lock, flags);  \
} while (0)

#define clkmgr_locked()  (spin_is_locked(&clock_lock))

int clkmgr_is_locked()
{
    return clkmgr_locked();
}
EXPORT_SYMBOL(clkmgr_is_locked);



/*                                               
                                                 
                                                */

#ifdef CONFIG_CLKMGR_STAT
void update_stat_locked(struct list_head *head, char *name, int op)
{
    struct list_head *pos = NULL;
    struct stat_node *node = NULL;
    int len = strlen(name);
    int new_node = 1;

    list_for_each(pos, head) {
        node = list_entry(pos, struct stat_node, link);
        if (!strncmp(node->name, name, len)) {
            new_node = 0;
            break;
        }    
    }    

    if (new_node) {
        node = NULL;
        node = kzalloc(sizeof(*node) + len + 1, GFP_ATOMIC);   
        if (!node) {
            clk_err("[%s]: malloc stat node for %s fail\n", __func__, name);
            return;
        } else {
            memcpy(node->name, name, len);
            list_add_tail(&node->link, head);
        }    
    }    

    if (op) {
        node->cnt_on++;
    } else {
        node->cnt_off++;
    }    
}
#endif



/*                                               
                                                 
                                                */

static int pll_enable_locked(struct pll *pll);
static int pll_disable_locked(struct pll *pll);

static int sys_enable_locked(struct subsys *sys);
static int sys_disable_locked(struct subsys *sys, int force_off);

static void mux_enable_locked(struct clkmux *mux);
static void mux_disable_locked(struct clkmux *mux);

static int clk_enable_locked(struct cg_clk *clk);
static int clk_disable_locked(struct cg_clk *clk);


static inline int pll_enable_internal(struct pll *pll, char *name)
{
    int err;
    err = pll_enable_locked(pll);
#ifdef CONFIG_CLKMGR_STAT
    update_stat_locked(&pll->head, name, 1);
#endif
    return err;
}

static inline int pll_disable_internal(struct pll *pll, char *name)
{
    int err;
    err = pll_disable_locked(pll);
#ifdef CONFIG_CLKMGR_STAT
    update_stat_locked(&pll->head, name, 0);
#endif
    return err;
}


static inline int subsys_enable_internal(struct subsys *sys, char *name)
{
    int err;
    err = sys_enable_locked(sys);
#ifdef CONFIG_CLKMGR_STAT
    //                                        
#endif
    return err;
}

static inline int subsys_disable_internal(struct subsys *sys, int force_off, char *name)
{
    int err;
    err = sys_disable_locked(sys, force_off);
#ifdef CONFIG_CLKMGR_STAT
    //                                        
#endif
    return err;
}


static inline void mux_enable_internal(struct clkmux *mux, char *name)
{
    mux_enable_locked(mux);
#ifdef CONFIG_CLKMGR_STAT
    update_stat_locked(&mux->head, name, 1);
#endif
}

static inline void mux_disable_internal(struct clkmux *mux, char *name)
{
    mux_disable_locked(mux);
#ifdef CONFIG_CLKMGR_STAT
    update_stat_locked(&mux->head, name, 0);
#endif
}


static inline int clk_enable_internal(struct cg_clk *clk, char *name)
{
    int err;
    err = clk_enable_locked(clk);
#ifdef CONFIG_CLKMGR_STAT
    update_stat_locked(&clk->head, name, 1);
#endif
    return err;
}

static inline int clk_disable_internal(struct cg_clk *clk, char *name)
{
    int err;
    err = clk_disable_locked(clk);
#ifdef CONFIG_CLKMGR_STAT
    update_stat_locked(&clk->head, name, 0);
#endif
    return err;
}



/*                                               
                                                 
                                                */

#define PLL_TYPE_SDM    0
#define PLL_TYPE_LC     1

#define HAVE_RST_BAR    (0x1 << 0)
#define HAVE_PLL_HP     (0x1 << 1)
#define HAVE_FIX_FRQ    (0x1 << 2) 
#define Others          (0x1 << 3)

//                                 
#define RST_BAR_MASK    0x1000000

static struct pll_ops arm_pll_ops;
static struct pll_ops sdm_pll_ops;
//                                 

static struct pll plls[NR_PLLS] = {
    {
        .name = __stringify(ARMPLL),
        .type = PLL_TYPE_SDM,
        .feat = HAVE_PLL_HP,
        .en_mask = 0x00000001,
        .base_addr = ARMPLL_CON0,
        .pwr_addr = ARMPLL_PWR_CON0,
        .ops = &arm_pll_ops,
        .hp_id = MT658X_FH_ARM_PLL,
        .hp_switch = 1,
    }, {
        .name = __stringify(MAINPLL),
        .type = PLL_TYPE_SDM,
        .feat = HAVE_PLL_HP | HAVE_RST_BAR,
        .en_mask = 0x78000001,
        .base_addr = MAINPLL_CON0,
        .pwr_addr = MAINPLL_PWR_CON0,
        .ops = &sdm_pll_ops,
        .hp_id = MT658X_FH_MAIN_PLL,
        .hp_switch = 1,
    }, {
        .name = __stringify(MSDCPLL),
        .type = PLL_TYPE_SDM,
        .feat = HAVE_PLL_HP,
        .en_mask = 0x00000001,
        .base_addr = MSDCPLL_CON0,
        .pwr_addr = MSDCPLL_PWR_CON0,
        .ops = &sdm_pll_ops,
        .hp_id = MT658X_FH_MSDC_PLL,
        .hp_switch = 1,
    }, {
        .name = __stringify(UNIVPLL),
        .type = PLL_TYPE_SDM,
        .feat = HAVE_RST_BAR | HAVE_FIX_FRQ,
        .en_mask = 0xFC000001,
        .base_addr = UNIVPLL_CON0,
        .pwr_addr = UNIVPLL_PWR_CON0,
        .ops = &sdm_pll_ops,
    }, {
        .name = __stringify(MMPLL),
        //                    
        .type = PLL_TYPE_SDM,
        .feat = HAVE_PLL_HP,
        .en_mask = 0x00000001,
        .base_addr = MMPLL_CON0,
        .pwr_addr = MMPLL_PWR_CON0,
        .ops = &sdm_pll_ops,
        .hp_id = MT658X_FH_MM_PLL,
        .hp_switch = 1,
    }, {
        .name = __stringify(VENCPLL),
        .type = PLL_TYPE_SDM,
        .feat = HAVE_PLL_HP,
        .en_mask = 0x00000001,
        .base_addr = VENCPLL_CON0,
        .pwr_addr = VENCPLL_PWR_CON0,
        .ops = &sdm_pll_ops,
        .hp_id = MT658X_FH_VENC_PLL,
        .hp_switch = 1,
    }
};

static struct pll *id_to_pll(unsigned int id)
{
    return id < NR_PLLS ? plls + id : NULL;
}

#define PLL_PWR_ON  (0x1 << 0)
#define PLL_ISO_EN  (0x1 << 1)

#define SDM_PLL_N_INFO_MASK 0x001FFFFF
#define UNIV_SDM_PLL_N_INFO_MASK 0x001fc000
#define SDM_PLL_N_INFO_CHG  0x80000000
//                                      
#define ARMPLL_POSDIV_MASK  0x07000000

#define PLL_FBKDIV_MASK     0x00007F00
#define PLL_FBKDIV_OFFSET   0x8

//                                  

static int pll_get_state_op(struct pll *pll)
{
    return clk_readl(pll->base_addr) & 0x1;
}

static void sdm_pll_enable_op(struct pll *pll)
{
#ifdef PLL_LOG	
    //                                                      
    clk_dbg("[%s]: pll->name=%s\n", __func__, pll->name);
#endif	
//                                                                       
//  
//                      
//                      
//  
#if 0
    if( pll->base_addr == VENCPLL_CON0)
    {
        clk_clrl(CLK_CFG_0, 0x00800000);	//                     
    }
#endif
    clk_setl(pll->pwr_addr, PLL_PWR_ON);
    udelay(2);
    clk_clrl(pll->pwr_addr, PLL_ISO_EN);
    
    clk_setl(pll->base_addr, pll->en_mask);
    udelay(20);

    if (pll->feat & HAVE_RST_BAR) {
        clk_setl(pll->base_addr, RST_BAR_MASK);
    }
#if 0
    if( pll->base_addr == VENCPLL_CON0)
    {
        clk_setl(CLK_CFG_0, 0x00800000);	//                      
    }
#endif    
    //               
//                                      
//  
//                                                    
//                                                                 
//         
//     
}

static void sdm_pll_disable_op(struct pll *pll)
{
#ifdef PLL_LOG		
    //                                                      
    clk_dbg("[%s]: pll->name=%s\n", __func__, pll->name);
#endif
//                                                                       
//   
//                      
//                      
//  

	//             
//                                      
//     
//                                                 
//                                                                                     
//         
//      
#if 0
    if( pll->base_addr == VENCPLL_CON0)
    {
        clk_clrl(CLK_CFG_0, 0x00800000);
    }
#endif
    if (pll->feat & HAVE_RST_BAR) {
        clk_clrl(pll->base_addr, RST_BAR_MASK);
    }

    clk_clrl(pll->base_addr, 0x1);

    clk_setl(pll->pwr_addr, PLL_ISO_EN);
    clk_clrl(pll->pwr_addr, PLL_PWR_ON);
#if 0    
    if( pll->base_addr == VENCPLL_CON0)
    {
        clk_setl(CLK_CFG_0, 0x00800000);
    }
#endif    
}

static void sdm_pll_fsel_op(struct pll *pll, unsigned int value)
{
    unsigned int ctrl_value;

    ctrl_value = clk_readl(pll->base_addr + 4);
    if(pll->base_addr == UNIVPLL_CON0)
    {
    	ctrl_value &= ~UNIV_SDM_PLL_N_INFO_MASK; 
    	ctrl_value |= value & UNIV_SDM_PLL_N_INFO_MASK;	
    }	
    else
    {
    	ctrl_value &= ~SDM_PLL_N_INFO_MASK; 
    	ctrl_value |= value & SDM_PLL_N_INFO_MASK;	
	}
    
    ctrl_value |= SDM_PLL_N_INFO_CHG; 

    clk_writel(pll->base_addr + 4, ctrl_value);
    udelay(20);
}

static int sdm_pll_dump_regs_op(struct pll *pll, unsigned int *ptr)
{
    *(ptr) = clk_readl(pll->base_addr); 
    *(++ptr) = clk_readl(pll->base_addr + 4); 
    *(++ptr) = clk_readl(pll->pwr_addr); 

    return 3;
}

static unsigned int pll_vcodivsel_map[2] = {1, 2};
static unsigned int pll_prediv_map[4] = {1, 2, 4, 4};
//                                                     
static unsigned int pll_n_info_map[14] = {
    13000000,  
     6500000,
     3250000,
     1625000,
      812500,
      406250,
      203125,
      101563,
       50782, 
       25391,
       12696,
        6348,
        3174,
        1587,
};

static unsigned int sdm_pll_vco_calc_op(struct pll *pll)
{
    int i;
    unsigned int mask;
    unsigned int vco_i = 0;
    unsigned int vco_f = 0;
    unsigned int vco = 0;

    volatile unsigned int con0 = clk_readl(pll->base_addr);
    volatile unsigned int con1 = clk_readl(pll->base_addr + 4);

    unsigned int vcodivsel = (con0 >> 19) & 0x1;    //        
    unsigned int prediv = (con0 >> 4) & 0x3;        //         
    unsigned int n_info_i = (con1 >> 14) & 0x7F;    //           
    unsigned int n_info_f = (con1 & 0x3FFF);        //          


    vcodivsel = pll_vcodivsel_map[vcodivsel]; 
    prediv = pll_prediv_map[prediv]; 

    vco_i = 26 * n_info_i;

    for (i = 0; i < 14; i++) {
        mask = 1U << (13 - i);
        if (n_info_f & mask) {
            vco_f += pll_n_info_map[i];     
            if (!(n_info_f & (~mask))) {
                break;
            }
        } 
    }

    vco_f = (vco_f + 1000000 / 2) / 1000000;

    vco = (vco_i + vco_f) * 1000 * vcodivsel / prediv;

#if 0 
    clk_info("[%s]%s: [0x%08x, 0x%08x] vco_i=%uMHz, vco_f=%uMHz, vco=%uKHz\n", 
            __func__, pll->name, con0, con1, vco_i, vco_f, vco);
#endif

    return vco;
}

static int sdm_pll_hp_enable_op(struct pll *pll)
{
    int err;
    unsigned int vco=0;

    if (!pll->hp_switch || (pll->state == PWR_DOWN)) {
        return 0;
    }
//                                   
    err = freqhopping_config(pll->hp_id, vco, 1);

    return err;
}

static int sdm_pll_hp_disable_op(struct pll *pll)
{
    int err;
    unsigned int vco=0;
    
    if (!pll->hp_switch || (pll->state == PWR_ON)) {
        return 0;
    }
//                                   
    err = freqhopping_config(pll->hp_id, vco, 0);

    return err;
}

static struct pll_ops sdm_pll_ops = {
    .get_state = pll_get_state_op,
    .enable = sdm_pll_enable_op,
    .disable = sdm_pll_disable_op,
    .fsel = sdm_pll_fsel_op,
    .dump_regs = sdm_pll_dump_regs_op,
    .vco_calc = sdm_pll_vco_calc_op,
    .hp_enable = sdm_pll_hp_enable_op,
    .hp_disable = sdm_pll_hp_disable_op,
};

static void arm_pll_fsel_op(struct pll *pll, unsigned int value)
{
    unsigned int ctrl_value;

    ctrl_value = clk_readl(pll->base_addr + 4);
    ctrl_value &= ~(SDM_PLL_N_INFO_MASK | ARMPLL_POSDIV_MASK); 
    ctrl_value |= value & (SDM_PLL_N_INFO_MASK | ARMPLL_POSDIV_MASK);
    ctrl_value |= SDM_PLL_N_INFO_CHG; 

    clk_writel(pll->base_addr + 4, ctrl_value);
    udelay(20);
}

static struct pll_ops arm_pll_ops = {
    .get_state = pll_get_state_op,
    .enable = sdm_pll_enable_op,
    .disable = sdm_pll_disable_op,
    .fsel = arm_pll_fsel_op,
    .dump_regs = sdm_pll_dump_regs_op,
    .vco_calc = sdm_pll_vco_calc_op,
    .hp_enable = sdm_pll_hp_enable_op,
    .hp_disable = sdm_pll_hp_disable_op,
};

static int get_pll_state_locked(struct pll *pll)
{
    if (likely(initialized)) { 
        return pll->state;
    } else {
        return pll->ops->get_state(pll);
    }
}

static int pll_enable_locked(struct pll *pll)
{
    pll->cnt++;

#ifdef PLL_LOG_TOP
    clk_info("[%s]: Start. pll->name=%s, pll->cnt=%d, pll->state=%d\n", __func__, pll->name, pll->cnt, pll->state);
#endif	

    if (pll->cnt > 1) {
        return 0;
    }

    if (pll->state == PWR_DOWN) {
        pll->ops->enable(pll); 
        pll->state = PWR_ON;
    }

    if (pll->ops->hp_enable) {
        pll->ops->hp_enable(pll);
    }
#ifdef PLL_LOG_TOP    
    clk_info("[%s]: End. pll->name=%s, pll->cnt=%d, pll->state=%d\n", __func__, pll->name, pll->cnt, pll->state);
#endif    
    return 0;
}

static int pll_disable_locked(struct pll *pll)
{
#ifdef PLL_LOG_TOP
    clk_info("[%s]: Start. pll->name=%s, pll->cnt=%d, pll->state=%d\n", __func__, pll->name, pll->cnt, pll->state);
#endif    

    BUG_ON(!pll->cnt);
    pll->cnt--;

#ifdef PLL_LOG_TOP
    clk_info("[%s]: Start. pll->name=%s, pll->cnt=%d, pll->state=%d\n", __func__, pll->name, pll->cnt, pll->state);
#endif    

    if (pll->cnt > 0) {
        return 0;
    }

    if (pll->state == PWR_ON) {
        pll->ops->disable(pll); 
        pll->state = PWR_DOWN;
    }

    if (pll->ops->hp_disable) {
        pll->ops->hp_disable(pll);
    }
#ifdef PLL_LOG_TOP    
    clk_info("[%s]: End. pll->name=%s, pll->cnt=%d, pll->state=%d\n", __func__, pll->name, pll->cnt, pll->state);
#endif    
    return 0;
}


static int pll_fsel_locked(struct pll *pll, unsigned int value)
{
    pll->ops->fsel(pll, value); 
    if (pll->ops->hp_enable) {
        pll->ops->hp_enable(pll);
    }
    return 0;
}

int pll_is_on(int id)
{
    int state;
    unsigned long flags;
    struct pll *pll = id_to_pll(id);    

    BUG_ON(!pll);

    clkmgr_lock(flags);
    state = get_pll_state_locked(pll);
    clkmgr_unlock(flags);

    return state;
}
EXPORT_SYMBOL(pll_is_on);

int enable_pll(int id, char *name)
{
    int err;
    unsigned long flags;
    struct pll *pll = id_to_pll(id);    
return 0;

    BUG_ON(!initialized);
    BUG_ON(!pll);
    BUG_ON(!name);
#ifdef PLL_LOG_TOP
    clk_info("[%s]: id=%d, name=%s\n", __func__, id, name);
#endif
    clkmgr_lock(flags);
    err = pll_enable_internal(pll, name);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(enable_pll);

int disable_pll(int id, char *name)
{
    int err;
    unsigned long flags;
    struct pll *pll = id_to_pll(id);    
return 0;

    BUG_ON(!initialized);
    BUG_ON(!pll);
    BUG_ON(!name);
#ifdef PLL_LOG_TOP
    clk_info("[%s]: id=%d, name=%s\n", __func__, id, name);
#endif
    clkmgr_lock(flags);
    err = pll_disable_internal(pll, name);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(disable_pll);


int enable_pll_spec(int id, char *name)
{
    int err;
    unsigned long flags;
    struct pll *pll = id_to_pll(id);    

    BUG_ON(!initialized);
    BUG_ON(!pll);
    BUG_ON(!name);
#ifdef PLL_LOG_TOP
    clk_info("[%s]: id=%d, name=%s\n", __func__, id, name);
#endif
    clkmgr_lock(flags);
    err = pll_enable_internal(pll, name);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(enable_pll_spec);

int disable_pll_spec(int id, char *name)
{
    int err;
    unsigned long flags;
    struct pll *pll = id_to_pll(id);    

    BUG_ON(!initialized);
    BUG_ON(!pll);
    BUG_ON(!name);
#ifdef PLL_LOG_TOP
    clk_info("[%s]: id=%d, name=%s\n", __func__, id, name);
#endif
    clkmgr_lock(flags);
    err = pll_disable_internal(pll, name);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(disable_pll_spec);

int pll_fsel(int id, unsigned int value)
{
    int err;
    unsigned long flags;
    struct pll *pll = id_to_pll(id);

    BUG_ON(!initialized);
    BUG_ON(!pll);

    clkmgr_lock(flags);
    err = pll_fsel_locked(pll, value);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(pll_fsel);


int pll_hp_switch_on(int id, int hp_on)
{
    int err = 0;
    unsigned long flags;
    int old_value;
    struct pll *pll = id_to_pll(id);

    BUG_ON(!initialized);
    BUG_ON(!pll);

    if (pll->type != PLL_TYPE_SDM) {
        err = -EINVAL;
        goto out;
    }

    clkmgr_lock(flags);
    old_value = pll->hp_switch;
    if (old_value == 0) {
        pll->hp_switch = 1;
        if (hp_on) {
            err = pll->ops->hp_enable(pll);
        }
    }
    clkmgr_unlock(flags);

#if 0
    clk_info("[%s]hp_switch(%d->%d), hp_on=%d\n", 
            __func__, old_value, pll->hp_switch, hp_on);
#endif

out:
    return err;
}
EXPORT_SYMBOL(pll_hp_switch_on);

int pll_hp_switch_off(int id, int hp_off)
{
    int err = 0;
    unsigned long flags;
    int old_value;
    struct pll *pll = id_to_pll(id);

    BUG_ON(!initialized);
    BUG_ON(!pll);

    if (pll->type != PLL_TYPE_SDM) {
        err = -EINVAL;
        goto out;
    }

    clkmgr_lock(flags);
    old_value = pll->hp_switch;
    if (old_value == 1) {
        if (hp_off) {
            err = pll->ops->hp_disable(pll);
        }
        pll->hp_switch = 0;
    }
    clkmgr_unlock(flags);

#if 0
    clk_info("[%s]hp_switch(%d->%d), hp_off=%d\n", 
            __func__, old_value, pll->hp_switch, hp_off);
#endif

out:
    return err;
}
EXPORT_SYMBOL(pll_hp_switch_off);


int pll_dump_regs(int id, unsigned int *ptr)
{
    struct pll *pll = id_to_pll(id);    

    BUG_ON(!initialized);
    BUG_ON(!pll);

    return pll->ops->dump_regs(pll, ptr);
}
EXPORT_SYMBOL(pll_dump_regs);

const char* pll_get_name(int id)
{
    struct pll *pll = id_to_pll(id);    

    BUG_ON(!initialized);
    BUG_ON(!pll);

    return pll->name;
}

void enable_clksq1(void)
{
    unsigned long flags;
return ;

    clkmgr_lock(flags);
    clk_setl(AP_PLL_CON0, 0x1 << 0);
    udelay(200);
    clk_setl(AP_PLL_CON0, 0x1 << 1);
    clkmgr_unlock(flags);
}
EXPORT_SYMBOL(enable_clksq1);

void disable_clksq1(void)
{
    unsigned long flags;
return ;

    clkmgr_lock(flags);
    clk_clrl(AP_PLL_CON0, 0x3 << 0);
    clkmgr_unlock(flags);
}
EXPORT_SYMBOL(disable_clksq1);

void clksq1_sw2hw(void)
{
    unsigned long flags;
return ;

    clkmgr_lock(flags);
    clk_clrl(AP_PLL_CON1, 0x3 << 0);
    clkmgr_unlock(flags);
}
EXPORT_SYMBOL(clksq1_sw2hw);

void clksq1_hw2sw(void)
{
    unsigned long flags;
return ;

    clkmgr_lock(flags);
    clk_setl(AP_PLL_CON1, 0x3 << 0);
    clkmgr_unlock(flags);
}
EXPORT_SYMBOL(clksq1_hw2sw);



/*                                               
                                                 
                                                */

#define SYS_TYPE_MODEM    0 
#define SYS_TYPE_MEDIA    1
#define SYS_TYPE_OTHER    2
#define SYS_TYPE_CONN     3

static struct subsys_ops md1_sys_ops;
static struct subsys_ops conn_sys_ops;
static struct subsys_ops dpy_sys_ops;
static struct subsys_ops dis_sys_ops;
static struct subsys_ops mfg_sys_ops;
static struct subsys_ops isp_sys_ops;
static struct subsys_ops ifr_sys_ops;
static struct subsys_ops vde_sys_ops;

static struct subsys syss[NR_SYSS] = {
    {
        .name = __stringify(SYS_MD1),
        .type = SYS_TYPE_MODEM,
        .default_sta = PWR_DOWN,
        .sta_mask = 1U << 0,
        .ctl_addr = SPM_MD_PWR_CON,
        .ops = &md1_sys_ops,
    }, {
        .name = __stringify(SYS_CONN),
        .type = SYS_TYPE_CONN,
        .default_sta = PWR_DOWN,
        .sta_mask = 1U << 1,
        .ctl_addr = SPM_CONN_PWR_CON,
        .ops = &conn_sys_ops,
    }, {
        .name = __stringify(SYS_DPY),
        .type = SYS_TYPE_OTHER,
        .default_sta = PWR_ON,
        .sta_mask = 1U << 2,
        .ctl_addr = SPM_DPY_PWR_CON,
        .ops = &dpy_sys_ops,
    }, {
        .name = __stringify(SYS_DIS),
        .type = SYS_TYPE_MEDIA,
        .default_sta = PWR_ON,
        .sta_mask = 1U << 3,
        .ctl_addr = SPM_DIS_PWR_CON,
        .ops = &dis_sys_ops,
        .start = &grps[CG_DISP0],
        .nr_grps = 2,
        .mux = &muxs[MT_MUX_MM],
    }, {
        .name = __stringify(SYS_MFG),
        .type = SYS_TYPE_MEDIA,
        .default_sta = PWR_ON,
        .sta_mask = 1U << 4,
        .ctl_addr = SPM_MFG_PWR_CON,
        .ops = &mfg_sys_ops,
        .start = &grps[CG_MFG],
        .nr_grps = 1,
        .mux = &muxs[MT_MUX_MFG],
    }, {
        .name = __stringify(SYS_ISP),
        .type = SYS_TYPE_MEDIA,
        .default_sta = PWR_ON,
        .sta_mask = 1U << 5,
        .ctl_addr = SPM_ISP_PWR_CON,
        .ops = &isp_sys_ops,
        .start = &grps[CG_IMAGE],
        .nr_grps = 1,
//                                
    }, {
        .name = __stringify(SYS_IFR),
        .type = SYS_TYPE_OTHER,
        .default_sta = PWR_ON,
        .sta_mask = 1U << 6,
        .ctl_addr = SPM_IFR_PWR_CON,
        .ops = &ifr_sys_ops,
    }, {
        .name = __stringify(SYS_VDE),
        .type = SYS_TYPE_MEDIA,
        .default_sta = PWR_ON,
        .sta_mask = 1U << 7,
        .ctl_addr = SPM_VDE_PWR_CON,
        .ops = &vde_sys_ops,
        .start = &grps[CG_VDEC0],
        .nr_grps = 2,
        .mux = &muxs[MT_MUX_VDEC],
    }
};


static void larb_backup(int larb_idx);
static void larb_restore(int larb_idx);



static struct subsys *id_to_sys(unsigned int id)
{
    return id < NR_SYSS ? syss + id : NULL;
}

static int md1_sys_enable_op(struct subsys *sys)
{
    int err; 
    err = spm_mtcmos_ctrl_mdsys1(STA_POWER_ON);
    return err;
}

static int md1_sys_disable_op(struct subsys *sys)
{
    int err;
    err = spm_mtcmos_ctrl_mdsys1(STA_POWER_DOWN);
    return err;
}

static int conn_sys_enable_op(struct subsys *sys)
{
    int err;
    err = spm_mtcmos_ctrl_connsys(STA_POWER_ON);
    return err;
}

static int conn_sys_disable_op(struct subsys *sys)
{
    int err;
    err = spm_mtcmos_ctrl_connsys(STA_POWER_DOWN);
    return err;
}

static int dpy_sys_enable_op(struct subsys *sys)
{
    int err;
    err = spm_mtcmos_ctrl_ddrphy(STA_POWER_ON);
    return err;
}

static int dpy_sys_disable_op(struct subsys *sys)
{
    int err;
    err = spm_mtcmos_ctrl_ddrphy(STA_POWER_DOWN);
    return err;
}

static int dis_sys_enable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif	

	//                    

    err = spm_mtcmos_ctrl_disp(STA_POWER_ON);
    larb_restore(MT_LARB_DISP);
    return err;
}

static int dis_sys_disable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif
	
	//                     

    larb_backup(MT_LARB_DISP);
    err = spm_mtcmos_ctrl_disp(STA_POWER_DOWN);
    return err;
}

static int mfg_sys_enable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif

	//                    
	
    err = spm_mtcmos_ctrl_mfg(STA_POWER_ON);
    
    return err;
}

static int mfg_sys_disable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif
	
	//                    

    err = spm_mtcmos_ctrl_mfg(STA_POWER_DOWN);
    
    return err;
}

static int isp_sys_enable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif
    err = spm_mtcmos_ctrl_isp(STA_POWER_ON);
    larb_restore(MT_LARB_IMG);
//                           
    return err;
}

static int isp_sys_disable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif
    larb_backup(MT_LARB_IMG);
//                          
    err = spm_mtcmos_ctrl_isp(STA_POWER_DOWN);
    return err;
}

static int ifr_sys_enable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif    
    err = spm_mtcmos_ctrl_infra(STA_POWER_ON);
    return err;
}

static int ifr_sys_disable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif    
    err = spm_mtcmos_ctrl_infra(STA_POWER_DOWN);
    return err;
}

static int vde_sys_enable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif    
    err = spm_mtcmos_ctrl_vdec(STA_POWER_ON);
    larb_restore(MT_LARB_VDEC);
    return err;
}

static int vde_sys_disable_op(struct subsys *sys)
{
    int err;
#ifdef SYS_LOG	
    clk_info("[%s]: sys->name=%s\n", __func__, sys->name);
#endif    
    larb_backup(MT_LARB_VDEC);
    err = spm_mtcmos_ctrl_vdec(STA_POWER_DOWN);
    return err;
}

static int sys_get_state_op(struct subsys *sys)
{
    unsigned int sta = clk_readl(SPM_PWR_STATUS);
    unsigned int sta_s = clk_readl(SPM_PWR_STATUS_S);

    return (sta & sys->sta_mask) && (sta_s & sys->sta_mask);
}

static int sys_dump_regs_op(struct subsys *sys, unsigned int *ptr)
{
    *(ptr) = clk_readl(sys->ctl_addr);
    return 1;
}

static struct subsys_ops md1_sys_ops = {
    .enable = md1_sys_enable_op,
    .disable = md1_sys_disable_op,
    .get_state = sys_get_state_op,
    .dump_regs = sys_dump_regs_op,
};

static struct subsys_ops conn_sys_ops = {
    .enable = conn_sys_enable_op,
    .disable = conn_sys_disable_op,
    .get_state = sys_get_state_op,
    .dump_regs = sys_dump_regs_op,
};

static struct subsys_ops dpy_sys_ops = {
    .enable = dpy_sys_enable_op,
    .disable = dpy_sys_disable_op,
    .get_state = sys_get_state_op,
    .dump_regs = sys_dump_regs_op,
};

static struct subsys_ops dis_sys_ops = {
    .enable = dis_sys_enable_op,
    .disable = dis_sys_disable_op,
    .get_state = sys_get_state_op,
    .dump_regs = sys_dump_regs_op,
};

static struct subsys_ops mfg_sys_ops = {
    .enable = mfg_sys_enable_op,
    .disable = mfg_sys_disable_op,
    .get_state = sys_get_state_op,
    .dump_regs = sys_dump_regs_op,
};

static struct subsys_ops isp_sys_ops = {
    .enable = isp_sys_enable_op,
    .disable = isp_sys_disable_op,
    .get_state = sys_get_state_op,
    .dump_regs = sys_dump_regs_op,
};

static struct subsys_ops ifr_sys_ops = {
    .enable = ifr_sys_enable_op,
    .disable = ifr_sys_disable_op,
    .get_state = sys_get_state_op,
    .dump_regs = sys_dump_regs_op,
};

static struct subsys_ops vde_sys_ops = {
    .enable = vde_sys_enable_op,
    .disable = vde_sys_disable_op,
    .get_state = sys_get_state_op,
    .dump_regs = sys_dump_regs_op,
};




static int get_sys_state_locked(struct subsys* sys)
{
    if (likely(initialized)) {
        return sys->state;
    } else {
        return sys->ops->get_state(sys);
    }
}

int subsys_is_on(int id)
{
    int state;
    unsigned long flags;
    struct subsys *sys = id_to_sys(id);    

    BUG_ON(!sys);

    clkmgr_lock(flags);
    state = get_sys_state_locked(sys);
    clkmgr_unlock(flags);

    return state;
}
EXPORT_SYMBOL(subsys_is_on);

//                         

static int sys_enable_locked(struct subsys *sys)
{
    int err;
    int local_state = sys->state; //                            

#ifdef STATE_CHECK_DEBUG
    int reg_state = sys->ops->get_state(sys);//                          
    BUG_ON(local_state != reg_state);
#endif

#ifdef SYS_LOG
    clk_info("[%s]: Start. sys->name=%s, sys->state=%d\n", __func__, sys->name, sys->state);
#endif

    if (local_state == PWR_ON) {
        return 0;
    }

    if (sys->mux) {
        mux_enable_internal(sys->mux, "sys");
    }

    //                 
//                                
//                 
//     

    err = sys->ops->enable(sys);
    WARN_ON(err);

    if (!err) {
        sys->state = PWR_ON;
    }
#ifdef SYS_LOG
    clk_info("[%s]: End. sys->name=%s, sys->state=%d\n", __func__, sys->name, sys->state);
#endif    
    return err;
}

static int sys_disable_locked(struct subsys *sys, int force_off)
{
    int err;
    int local_state = sys->state;//                            
    int i;
    struct cg_grp *grp;

#ifdef STATE_CHECK_DEBUG
    int reg_state = sys->ops->get_state(sys);//                          
    BUG_ON(local_state != reg_state);
#endif

#ifdef SYS_LOG
    clk_info("[%s]: Start. sys->name=%s, sys->state=%d, force_off=%d\n", __func__, sys->name, sys->state, force_off);
#endif
    if (!force_off) {
        //                         
        for (i = 0; i < sys->nr_grps; i++) {
            grp = sys->start + i;
            if (grp->state) {
                return 0;
            }
        }
    }

    if (local_state == PWR_DOWN) {
        return 0;
    }

    err = sys->ops->disable(sys);
    WARN_ON(err);

    if (!err) {
        sys->state = PWR_DOWN;
    }

    if (sys->mux) {
        mux_disable_internal(sys->mux, "sys");
    }
#ifdef SYS_LOG
    clk_info("[%s]: End. sys->name=%s, sys->state=%d, force_off=%d\n", __func__, sys->name, sys->state, force_off);
#endif    
    return err;
}

int enable_subsys(int id, char *name)
{
//          
    int err;
    unsigned long flags;
    struct subsys *sys = id_to_sys(id);    
return 0;

    BUG_ON(!initialized);
    BUG_ON(!sys);

    clkmgr_lock(flags);
    err = subsys_enable_internal(sys, name);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(enable_subsys);

int disable_subsys(int id, char *name)
{
//          
    int err;
    unsigned long flags;
    struct subsys *sys = id_to_sys(id);    
return 0;

    BUG_ON(!initialized);
    BUG_ON(!sys);

    clkmgr_lock(flags);
    err = subsys_disable_internal(sys, 0, name);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(disable_subsys);

int disable_subsys_force(int id, char *name)
{
    int err;
    unsigned long flags;
    struct subsys *sys = id_to_sys(id);    

    BUG_ON(!initialized);
    BUG_ON(!sys);

    clkmgr_lock(flags);
    err = subsys_disable_internal(sys, 1, name);
    clkmgr_unlock(flags);

    return err;
}

int subsys_dump_regs(int id, unsigned int *ptr)
{
//          
    struct subsys *sys = id_to_sys(id);    
return 0;

    BUG_ON(!initialized);
    BUG_ON(!sys);

    return sys->ops->dump_regs(sys, ptr);
}
EXPORT_SYMBOL(subsys_dump_regs);

const char* subsys_get_name(int id)
{
    struct subsys *sys = id_to_sys(id);    

    BUG_ON(!initialized);
    BUG_ON(!sys);

    return sys->name;
}

#define JIFFIES_PER_LOOP 10

int md_power_on(int id)
{
//          
    int err;
    unsigned long flags;
    struct subsys *sys = id_to_sys(id);

    BUG_ON(!initialized);
    BUG_ON(!sys);
    BUG_ON(sys->type != SYS_TYPE_MODEM);

    clkmgr_lock(flags);
    err = subsys_enable_internal(sys, "md");
    clkmgr_unlock(flags);

    WARN_ON(err);

    return err;
}
EXPORT_SYMBOL(md_power_on);

int md_power_off(int id, unsigned int timeout)
{
//          
    int err;
    int cnt;
    bool slept;
    unsigned long flags;
    struct subsys *sys = id_to_sys(id);

    BUG_ON(!initialized);
    BUG_ON(!sys);
    BUG_ON(sys->type != SYS_TYPE_MODEM);

    //                       
    //                              
    slept = spm_is_md_sleep();

    cnt = (timeout + JIFFIES_PER_LOOP - 1) / JIFFIES_PER_LOOP;

    while (!slept && cnt--) {
        msleep(MSEC_PER_SEC / JIFFIES_PER_LOOP);

        //                              
        slept = spm_is_md_sleep();

        if (slept) {
            break;
        }
    }
    
    clkmgr_lock(flags);
    err = subsys_disable_internal(sys, 0, "md");
    clkmgr_unlock(flags);

    WARN_ON(err);
    
    return !slept;

}
EXPORT_SYMBOL(md_power_off);

int conn_power_on(void)
{
    int err;
    unsigned long flags;
    struct subsys *sys = id_to_sys(SYS_CONN);

    BUG_ON(!initialized);
    BUG_ON(!sys);
    BUG_ON(sys->type != SYS_TYPE_CONN);

    clkmgr_lock(flags);
    err = subsys_enable_internal(sys, "conn");
    clkmgr_unlock(flags);

    WARN_ON(err);

    return err;
}
EXPORT_SYMBOL(conn_power_on);

int conn_power_off(void)
{
    int err;
    unsigned long flags;
    struct subsys *sys = id_to_sys(SYS_CONN);

    BUG_ON(!initialized);
    BUG_ON(!sys);
    BUG_ON(sys->type != SYS_TYPE_CONN);

    clkmgr_lock(flags);
    err = subsys_disable_internal(sys, 0, "conn");
    clkmgr_unlock(flags);

    WARN_ON(err);

    return err;
}
EXPORT_SYMBOL(conn_power_off);


static DEFINE_MUTEX(larb_monitor_lock);
static LIST_HEAD(larb_monitor_handlers);

void register_larb_monitor(struct larb_monitor *handler)
{
    struct list_head *pos;
//       

    mutex_lock(&larb_monitor_lock);
    list_for_each(pos, &larb_monitor_handlers) {
        struct larb_monitor *l;
        l = list_entry(pos, struct larb_monitor, link);
        if (l->level > handler->level)
            break;
    }
    list_add_tail(&handler->link, pos);
    mutex_unlock(&larb_monitor_lock);
}
EXPORT_SYMBOL(register_larb_monitor);


void unregister_larb_monitor(struct larb_monitor *handler)
{
//       

    mutex_lock(&larb_monitor_lock);
    list_del(&handler->link);
    mutex_unlock(&larb_monitor_lock);
}
EXPORT_SYMBOL(unregister_larb_monitor);

static void larb_clk_prepare(int larb_idx)
{
    switch (larb_idx) {
    case MT_LARB_DISP:
        /*         */
        clk_writel(DISP_CG_CLR0, 0x3);
        //                            
        break;
    case MT_LARB_VDEC:
        /*     */
        //                              
        clk_writel(LARB_CKEN_SET, 0x1);
        //                             
        break;
    case MT_LARB_IMG:
        /*     */
        //                              
        clk_writel(IMG_CG_CLR, 0x1);
        //                          
        break;
    default:
        BUG();
    }
}

static void larb_clk_finish(int larb_idx)
{
    switch (larb_idx) {
    case MT_LARB_DISP:
        /*         */
        clk_writel(DISP_CG_SET0, 0x3);
        //                            
        break;
    case MT_LARB_VDEC:
        /*     */
        clk_writel(LARB_CKEN_CLR, 0x1);
        //                              
        //                             
        break;
    case MT_LARB_IMG:
        /*     */
        clk_writel(IMG_CG_SET, 0x1);
        //                              
        //                          
        break;
    default:
        BUG();
    }
}

static void larb_backup(int larb_idx)
{
    struct larb_monitor *pos;

    //                                                               
    clk_dbg("[%s]: start to backup larb%d\n", __func__, larb_idx);
    
    larb_clk_prepare(larb_idx);

    list_for_each_entry(pos, &larb_monitor_handlers, link) {
        if (pos->backup != NULL)
        {	
//                                                      
            pos->backup(pos, larb_idx);
        }    
    }

    larb_clk_finish(larb_idx);
}

static void larb_restore(int larb_idx)
{
    struct larb_monitor *pos;

    //                                                                
    clk_dbg("[%s]: start to restore larb%d\n", __func__, larb_idx);
    
    larb_clk_prepare(larb_idx);

    list_for_each_entry(pos, &larb_monitor_handlers, link) {
        if (pos->restore != NULL)
        {	
//                                                       
            pos->restore(pos, larb_idx);
        }
    }

    larb_clk_finish(larb_idx);
}



/*                                               
                                                 
                                                */

static struct clkmux_ops clkmux_ops;
static struct clkmux_ops audio_clkmux_ops;

static struct clkmux muxs[NR_MUXS] = {
    {
        .name = __stringify(MUX_MM),
        .base_addr = CLK_CFG_0,
        .sel_mask = 0x07000000,
        .pdn_mask = 0x80000000,
        .offset = 24,
        .nr_inputs = 8,
        .ops = &clkmux_ops,
        .pll = &plls[VENCPLL],
    }, {
        .name = __stringify(MUX_CAMTG),
        .base_addr = CLK_CFG_1,
        .sel_mask = 0x07000000,
        .pdn_mask = 0x80000000,
        .offset = 24,
        .nr_inputs = 7,
        .ops = &clkmux_ops,
        .pll = &plls[UNIVPLL],
    }, {
        .name = __stringify(MUX_MFG),
        .base_addr = CLK_CFG_1,
        .sel_mask = 0x00070000,
        .pdn_mask = 0x00800000,
        .offset = 16,
        .nr_inputs = 8,
        .ops = &clkmux_ops,
//                                     
        .pll = &plls[MMPLL],
    }, {
        .name = __stringify(MUX_VDEC),
        .base_addr = CLK_CFG_1,
        .sel_mask = 0x00000F00,
        .pdn_mask = 0x00008000,
        .offset = 8,
        .nr_inputs = 9,
        .ops = &clkmux_ops,
//                                     
    }, {
        .name = __stringify(MUX_PWM),
        .base_addr = CLK_CFG_1,
        .sel_mask = 0x00000003,
        .pdn_mask = 0x00000080,
        .offset = 0,
        .nr_inputs = 4,
        .ops = &clkmux_ops,
        .pll = &plls[UNIVPLL],
    }, {
        .name = __stringify(MUX_MSDC30_0),
        .base_addr = CLK_CFG_2,
        .sel_mask = 0x07000000,
        .pdn_mask = 0x80000000,
        .offset = 24,
        .nr_inputs = 6,
        .ops = &clkmux_ops,
        .pll = &plls[MSDCPLL],
    }, {
        .name = __stringify(MUX_USB20),
        .base_addr = CLK_CFG_2,
        .sel_mask = 0x00070000,
        .pdn_mask = 0x00800000,
        .offset = 16,
        .nr_inputs = 3,
        .ops = &clkmux_ops,
        .pll = &plls[UNIVPLL],
    }, {
        .name = __stringify(MUX_SPI),
        .base_addr = CLK_CFG_2,
        .sel_mask = 0x00000700,
        .pdn_mask = 0x00008000,
        .offset = 8,
        .nr_inputs = 5,
        .ops = &clkmux_ops,
    }, {
        .name = __stringify(MUX_UART),
        .base_addr = CLK_CFG_2,
        .sel_mask = 0x00000001,
        .pdn_mask = 0x00000080,
        .offset = 0,
        .nr_inputs = 2,
        .ops = &clkmux_ops,
    }, {
        .name = __stringify(MT_MUX_AUDINTBUS),
        .base_addr = CLK_CFG_3,
        .sel_mask = 0x07000000,
        .pdn_mask = 0x80000000,
        .offset = 24,
        .nr_inputs = 6,
        .ops = &audio_clkmux_ops,
        .siblings = &muxs[MT_MUX_AUDIO],
    }, {
        .name = __stringify(MUX_AUDIO),
        .base_addr = CLK_CFG_3,
        .sel_mask = 0x00010000,
        .pdn_mask = 0x00800000,
        .offset = 16,
        .nr_inputs = 2,
        .ops = &audio_clkmux_ops,
    }, {
        .name = __stringify(MUX_MSDC30_2),
        .base_addr = CLK_CFG_3,
        .sel_mask = 0x00000700,
        .pdn_mask = 0x00008000,
        .offset = 8,
        .nr_inputs = 6,
        .ops = &clkmux_ops,
        .pll = &plls[MSDCPLL],
    }, {
        .name = __stringify(MUX_MSDC30_1),
        .base_addr = CLK_CFG_3,
        .sel_mask = 0x00000007,
        .pdn_mask = 0x00000080,
        .offset = 0,
        .nr_inputs = 6,
        .ops = &clkmux_ops,
        .pll = &plls[MSDCPLL],
    }
        
};


static struct clkmux *id_to_mux(unsigned int id)
{
    return id < NR_MUXS ? muxs + id : NULL;
}

static void clkmux_sel_op(struct clkmux *mux, unsigned clksrc)
{
    volatile unsigned int reg;
    
#ifdef MUX_LOG
    //                                                                          
    clk_dbg("[%s]: mux->name=%s, clksrc=%d\n", __func__, mux->name, clksrc); 
#endif

    reg = clk_readl(mux->base_addr);

    reg &= ~(mux->sel_mask);
    reg |= (clksrc << mux->offset) & mux->sel_mask;

    clk_writel(mux->base_addr, reg);
}

static void clkmux_enable_op(struct clkmux *mux)
{
#ifdef MUX_LOG
    //                                                      
    clk_dbg("[%s]: mux->name=%s\n", __func__, mux->name);
#endif    

	//                                                            
//                                     
//     
       //                                           
//                           
//      
    clk_clrl(mux->base_addr, mux->pdn_mask); 
}

static void clkmux_disable_op(struct clkmux *mux)
{
#ifdef MUX_LOG	
    //                                                       
    clk_dbg("[%s]: mux->name=%s\n", __func__, mux->name); 
#endif    

    //                                                            
//                                     
//      
		//                                           
//                           
//    
    clk_setl(mux->base_addr, mux->pdn_mask); 
}

static struct clkmux_ops clkmux_ops = {
    .sel = clkmux_sel_op,
    .enable = clkmux_enable_op, 
    .disable = clkmux_disable_op,
};

static void audio_clkmux_enable_op(struct clkmux *mux)
{
#ifdef MUX_LOG	
    //                                                       
    clk_dbg("[%s]: mux->name=%s\n", __func__, mux->name); 
#endif     
    clk_clrl(mux->base_addr, mux->pdn_mask); 
};

static struct clkmux_ops audio_clkmux_ops = {
    .sel = clkmux_sel_op,
    .enable = audio_clkmux_enable_op, 
    .disable = clkmux_disable_op,
};

static void clkmux_sel_locked(struct clkmux *mux, unsigned int clksrc)
{
    mux->ops->sel(mux, clksrc);
}

static void mux_enable_locked(struct clkmux *mux)
{
    mux->cnt++;

#ifdef MUX_LOG_TOP
    clk_info("[%s]: Start. mux->name=%s, mux->cnt=%d\n", __func__, mux->name, mux->cnt);
#endif	

    if (mux->cnt > 1) {
        return;
    }

    if (mux->pll) {
        pll_enable_internal(mux->pll, "mux");
    }

//                      
//                                                  
//     

    mux->ops->enable(mux);

    if (mux->siblings) {
        mux_enable_internal(mux->siblings, "mux_s");
    }
    
#ifdef MUX_LOG_TOP    
    clk_info("[%s]: End. mux->name=%s, mux->cnt=%d\n", __func__, mux->name, mux->cnt);
#endif    
}

static void mux_disable_locked(struct clkmux *mux)
{
#ifdef MUX_LOG_TOP
    clk_info("[%s]: Start. mux->name=%s, mux->cnt=%d\n", __func__, mux->name, mux->cnt);
#endif
	
    BUG_ON(!mux->cnt);

    mux->cnt--;
    
#ifdef MUX_LOG_TOP
    clk_info("[%s]: Start. mux->name=%s, mux->cnt=%d\n", __func__, mux->name, mux->cnt);
#endif
    
    if (mux->cnt > 0) {
        return;
    }

    mux->ops->disable(mux);

    if (mux->siblings) {
        mux_disable_internal(mux->siblings, "mux_s");
    }

//                      
//                                                     
//     

    if (mux->pll) {
        pll_disable_internal(mux->pll, "mux");
    }

#ifdef MUX_LOG_TOP
    clk_info("[%s]: End. mux->name=%s, mux->cnt=%d\n", __func__, mux->name, mux->cnt);
#endif    
}

int clkmux_sel(int id, unsigned int clksrc, char *name)
{
    unsigned long flags;
    struct clkmux *mux = id_to_mux(id);
//         

    BUG_ON(!initialized); 
    BUG_ON(!mux);
    BUG_ON(clksrc >= mux->nr_inputs);
    
    clkmgr_lock(flags); 
    clkmux_sel_locked(mux, clksrc);
    clkmgr_unlock(flags);

    return 0;
}
EXPORT_SYMBOL(clkmux_sel);

void enable_mux(int id, char *name)
{
    unsigned long flags;
    struct clkmux *mux = id_to_mux(id);

    BUG_ON(!initialized); 
    BUG_ON(!mux);
    BUG_ON(!name);
#ifdef MUX_LOG_TOP
    clk_info("[%s]: id=%d, name=%s\n", __func__, id, name);
//     
//                       
//                                                               
#endif    
    clkmgr_lock(flags);
    mux_enable_internal(mux, name);
    clkmgr_unlock(flags);

    return;
}
EXPORT_SYMBOL(enable_mux);

void disable_mux(int id, char *name)
{
    unsigned long flags;
    struct clkmux *mux = id_to_mux(id);

    BUG_ON(!initialized); 
    BUG_ON(!mux);
    BUG_ON(!name);
#ifdef MUX_LOG_TOP
    clk_info("[%s]: id=%d, name=%s\n", __func__, id, name);
//     
//                       
//                                                               
#endif
    clkmgr_lock(flags);
    mux_disable_internal(mux, name);
    clkmgr_unlock(flags);

    return;
}
EXPORT_SYMBOL(disable_mux);


/*
                               
                               
                               
                               
 
                              
                               
                                                               

                                                       
                               
 
                                    

                               
 
                              
                               

                                                           
                               
 
                                    

                         
                           
 
                         
 
                                 
*/


/*                                               
                                                 
                                                */

static struct cg_grp_ops general_cg_grp_ops;
static struct cg_grp_ops vdec_cg_grp_ops;
//                                         


static struct cg_grp grps[NR_GRPS] = {
    {
        .name = __stringify(CG_PERI),
        .set_addr = PERI_PDN0_SET,    //       
        .clr_addr = PERI_PDN0_CLR,    //      
        .sta_addr = PERI_PDN0_STA,
        .mask = 0x03FFFFFF,
        .ops = &general_cg_grp_ops,
    }, {
        .name = __stringify(CG_INFRA),
        .set_addr = INFRA_PDN_SET,    //       
        .clr_addr = INFRA_PDN_CLR,    //      
        .sta_addr = INFRA_PDN_STA,
        .mask = 0x0091BFE3,
        .ops = &general_cg_grp_ops,
    }, {
        .name = __stringify(CG_TOPCK),
        .set_addr = CLK_CFG_4,        //       
        .clr_addr = CLK_CFG_4,        //      
        .sta_addr = CLK_CFG_4,
        //                   
        .mask = 0x00000020,
        .ops = &general_cg_grp_ops,
    }, {
        .name = __stringify(CG_DISP0),
        .set_addr = DISP_CG_SET0,    //       
        .clr_addr = DISP_CG_CLR0,    //      
        .sta_addr = DISP_CG_CON0,
        .mask = 0x0007FFFF,
        .ops = &general_cg_grp_ops,
        .sys = &syss[SYS_DIS],
    }, {
        .name = __stringify(CG_DISP1),
        .set_addr = DISP_CG_SET1,    //       
        .clr_addr = DISP_CG_CLR1,    //      
        .sta_addr = DISP_CG_CON1,
        .mask = 0x0000000F,
        .ops = &general_cg_grp_ops,
        .sys = &syss[SYS_DIS],
    }, {
        .name = __stringify(CG_IMAGE),
        .set_addr = IMG_CG_SET,        //       
        .clr_addr = IMG_CG_CLR,        //      
        .sta_addr = IMG_CG_CON,
        .mask = 0x000003E1,
        .ops = &general_cg_grp_ops,
        .sys = &syss[SYS_ISP],
    }, {
        .name = __stringify(CG_MFG),
        .set_addr = MFG_CG_SET,        //       
        .clr_addr = MFG_CG_CLR,        //      
        .sta_addr = MFG_CG_CON,
        .mask = 0x00000001,
        .ops = &general_cg_grp_ops,
        .sys = &syss[SYS_MFG],
    }, {
        .name = __stringify(CG_AUDIO),
        .sta_addr = AUDIO_TOP_CON0,
        .mask = 0x00000044,
        .ops = &general_cg_grp_ops,
    }, {
        .name = __stringify(CG_VDEC0),
        .set_addr = VDEC_CKEN_CLR,    //       
        .clr_addr = VDEC_CKEN_SET,    //      
        .mask = 0x00000001,
        //                   
        .ops = &vdec_cg_grp_ops,
        .sys = &syss[SYS_VDE],
    }, {
        .name = __stringify(CG_VDEC1),
        .set_addr = LARB_CKEN_CLR,    //       
        .clr_addr = LARB_CKEN_SET,    //      
        .mask = 0x00000001,
        .ops = &vdec_cg_grp_ops,
        .sys = &syss[SYS_VDE],
    }
};

static struct cg_grp *id_to_grp(unsigned int id)
{
    return id < NR_GRPS ? grps + id : NULL;
}

static unsigned int general_grp_get_state_op(struct cg_grp *grp)
{
    volatile unsigned int val;
    struct subsys *sys = grp->sys;

    if (sys && !sys->state) {
        return 0;
    }

    val = clk_readl(grp->sta_addr);
    val = (~val) & (grp->mask); 
    return val;
}

static int general_grp_dump_regs_op(struct cg_grp *grp, unsigned int *ptr)
{
    *(ptr) = clk_readl(grp->sta_addr);
//                                                  
    
    return 1;
}

static struct cg_grp_ops general_cg_grp_ops = {
    .get_state = general_grp_get_state_op,
    .dump_regs = general_grp_dump_regs_op,
};

static unsigned int vdec_grp_get_state_op(struct cg_grp *grp)
{
    volatile unsigned int val = clk_readl(grp->set_addr);
    val &= grp->mask; 
    return val;
}

static int vdec_grp_dump_regs_op(struct cg_grp *grp, unsigned int *ptr)
{
    *(ptr) = clk_readl(grp->set_addr);
    *(++ptr) = clk_readl(grp->clr_addr);
    
    return 2;
}

static struct cg_grp_ops vdec_cg_grp_ops = {
    .get_state = vdec_grp_get_state_op,
    .dump_regs = vdec_grp_dump_regs_op,
};



/*                                               
                                                 
                                                */

static struct cg_clk_ops general_cg_clk_ops;
//                                        
static struct cg_clk_ops audio_cg_clk_ops;
static struct cg_clk_ops audsys_cg_clk_ops; //           
static struct cg_clk_ops vdec_cg_clk_ops;
//                                         

static struct cg_clk clks[NR_CLKS] = {
    [CG_PERI_FROM ... CG_PERI_TO] = {
        .cnt = 0,
        .ops = &general_cg_clk_ops,
        .grp = &grps[CG_PERI],
    },
    [CG_INFRA_FROM ... CG_INFRA_TO] = {
        .cnt = 0,
        .ops = &general_cg_clk_ops,
        .grp = &grps[CG_INFRA],
    },
    [CG_TOPCK_FROM ... CG_TOPCK_TO] = {
        .cnt = 0,
        .ops = &general_cg_clk_ops,
        .grp = &grps[CG_TOPCK],
    },
    [CG_DISP0_FROM ... CG_DISP0_TO] = {
        .cnt = 0,
        .ops = &general_cg_clk_ops,
        .grp = &grps[CG_DISP0],
    },
    [CG_DISP1_FROM ... CG_DISP1_TO] = {
        .cnt = 0,
        .ops = &general_cg_clk_ops,
        .grp = &grps[CG_DISP1],
    },
    [CG_IMAGE_FROM ... CG_IMAGE_TO] = {
        .cnt = 0,
        .ops = &general_cg_clk_ops,
        .grp = &grps[CG_IMAGE],
    },
    [CG_MFG_FROM ... CG_MFG_TO] = {
        .cnt = 0,
        .ops = &general_cg_clk_ops,
        .grp = &grps[CG_MFG],
    },
    [CG_AUDIO_FROM ... CG_AUDIO_TO] = {
        .cnt = 0,
        .ops = &audsys_cg_clk_ops,
        .grp = &grps[CG_AUDIO],
    },
    [CG_VDEC0_FROM ... CG_VDEC0_TO] = {
        .cnt = 0,
        .ops = &vdec_cg_clk_ops,
        .grp = &grps[CG_VDEC0],
    },
    [CG_VDEC1_FROM ... CG_VDEC1_TO] = {
        .cnt = 0,
        .ops = &vdec_cg_clk_ops,
        .grp = &grps[CG_VDEC1],
    },
};

static struct cg_clk *id_to_clk(unsigned int id)
{
    return id < NR_CLKS ? clks + id : NULL;
}

static int general_clk_get_state_op(struct cg_clk *clk)
{
    struct subsys *sys = clk->grp->sys;
    if (sys && !sys->state) {
        return PWR_DOWN; 
    }

    return (clk_readl(clk->grp->sta_addr) & (clk->mask)) ? PWR_DOWN : PWR_ON ;
}

static int general_clk_check_validity_op(struct cg_clk *clk)
{
    int valid = 0;
    if (clk->mask & clk->grp->mask) {
        valid = 1;
    }

    return valid;
}

static int general_clk_enable_op(struct cg_clk *clk)
{
#ifdef CLK_LOG
    clk_info("[%s]: clk->grp->name=%s, clk->mask=0x%x\n", __func__, clk->grp->name, clk->mask);
#endif    
    
    clk_writel(clk->grp->clr_addr, clk->mask);
    return 0;
}

static int general_clk_disable_op(struct cg_clk *clk)
{
#ifdef CLK_LOG	
    clk_info("[%s]: clk->grp->name=%s, clk->mask=0x%x\n", __func__, clk->grp->name, clk->mask);
#endif    

    clk_writel(clk->grp->set_addr, clk->mask);
    return 0;
}

static struct cg_clk_ops general_cg_clk_ops = {
    .get_state = general_clk_get_state_op,
    .check_validity = general_clk_check_validity_op,
    .enable = general_clk_enable_op,
    .disable = general_clk_disable_op,
};

static int audio_clk_enable_op(struct cg_clk *clk)
{
#ifdef CLK_LOG	
    clk_info("[%s]: clk->grp->name=%s, clk->mask=0x%x\n", __func__, clk->grp->name, clk->mask);
#endif
    clk_writel(clk->grp->clr_addr, clk->mask);
//                                                           
    return 0;
}

static int audio_clk_disable_op(struct cg_clk *clk)
{
#ifdef CLK_LOG	
    clk_info("[%s]: clk->grp->name=%s, clk->mask=0x%x\n", __func__, clk->grp->name, clk->mask);
#endif    	
//                                                           
    clk_writel(clk->grp->set_addr, clk->mask);
    return 0;
}

static struct cg_clk_ops audio_cg_clk_ops = {
    .get_state = general_clk_get_state_op,
    .check_validity = general_clk_check_validity_op,
    .enable = audio_clk_enable_op,
    .disable = audio_clk_disable_op,
};

/*
                                                
 
                                              
             
 

                                                 
 
                                              
             
 

                                           
                                          
                                                    
                                
                                  
  */

static int audsys_clk_enable_op(struct cg_clk *clk)
{
//                                                                                                         
    clk_clrl(clk->grp->sta_addr, clk->mask);
    return 0;
}

static int audsys_clk_disable_op(struct cg_clk *clk)
{
    clk_setl(clk->grp->sta_addr, clk->mask);
    return 0;
}

static struct cg_clk_ops audsys_cg_clk_ops = {
    .get_state = general_clk_get_state_op,
    .check_validity = general_clk_check_validity_op,
    .enable = audsys_clk_enable_op,
    .disable = audsys_clk_disable_op,
};

static int vdec_clk_get_state_op(struct cg_clk *clk)
{
    return (clk_readl(clk->grp->set_addr) & (clk->mask)) ? PWR_ON : PWR_DOWN;
}

static struct cg_clk_ops vdec_cg_clk_ops = {
    .get_state = vdec_clk_get_state_op,
    .check_validity = general_clk_check_validity_op,
    .enable = general_clk_enable_op,
    .disable = general_clk_disable_op,
};




static int power_prepare_locked(struct cg_grp *grp)
{
    int err = 0;
    if (grp->sys) {
        err = subsys_enable_internal(grp->sys, "clk");
    }
    return err;
}

static int power_finish_locked(struct cg_grp *grp)
{
    int err = 0;
    if (grp->sys) {
        err = subsys_disable_internal(grp->sys, 0, "clk");
    }
    return err;
}

static int clk_enable_locked(struct cg_clk *clk)
{
    struct cg_grp *grp = clk->grp;
    unsigned int local_state;
#ifdef STATE_CHECK_DEBUG
    unsigned int reg_state;
#endif
    int err;
    
    clk->cnt++;

#ifdef CLK_LOG
    clk_info("[%s]: Start. grp->name=%s, grp->state=0x%x, clk->mask=0x%x, clk->cnt=%d, clk->state=%d\n", __func__, grp->name, grp->state, clk->mask, clk->cnt, clk->state);
#endif

    if (clk->cnt > 1) {
        return 0;
    }

    local_state = clk->state;

#ifdef STATE_CHECK_DEBUG
    reg_state = grp->ops->get_state(grp, clk);
    //                                 
#endif

#if 1
    if (clk->mux) {
        mux_enable_internal(clk->mux, "clk");
    }
    	
    err = power_prepare_locked(grp);
    BUG_ON(err);
#endif

    if (clk->parent) {
        clk_enable_internal(clk->parent, "clk");
    }

    if (local_state == PWR_ON) {
        return 0;
    }

    clk->ops->enable(clk);

    clk->state = PWR_ON;
    grp->state |= clk->mask;
#ifdef CLK_LOG
    clk_info("[%s]: End. grp->name=%s, grp->state=0x%x, clk->mask=0x%x, clk->cnt=%d, clk->state=%d\n", __func__, grp->name, grp->state, clk->mask, clk->cnt, clk->state);
#endif
    
    return 0;
}

static int clk_disable_locked(struct cg_clk *clk)
{
    struct cg_grp *grp = clk->grp; 
    unsigned int local_state;
#ifdef STATE_CHECK_DEBUG
    unsigned int reg_state;
#endif
    int err;

#ifdef CLK_LOG
    clk_info("[%s]: Start. grp->name=%s, grp->state=0x%x, clk->mask=0x%x, clk->cnt=%d, clk->state=%d\n", __func__, grp->name, grp->state, clk->mask, clk->cnt, clk->state);
#endif

    BUG_ON(!clk->cnt);
    clk->cnt--;

#ifdef CLK_LOG
    clk_info("[%s]: Start. grp->name=%s, grp->state=0x%x, clk->mask=0x%x, clk->cnt=%d, clk->state=%d\n", __func__, grp->name, grp->state, clk->mask, clk->cnt, clk->state);
#endif

    if (clk->cnt > 0) {
        return 0;
    }

    local_state = clk->state;

#ifdef STATE_CHECK_DEBUG
    reg_state = grp->ops->get_state(grp, clk);
    //                                 
#endif

    if (local_state == PWR_DOWN) {
        return 0;
    }

    if (clk->force_on) {
        return 0;
    }

    clk->ops->disable(clk);

    clk->state = PWR_DOWN;
    grp->state &= ~(clk->mask);

    if (clk->parent) {
        clk_disable_internal(clk->parent, "clk");
    }

#if 1
    err = power_finish_locked(grp);
    BUG_ON(err);

    if (clk->mux) {
        mux_disable_internal(clk->mux, "clk");
    }
#endif

#ifdef CLK_LOG
    clk_info("[%s]: End. grp->name=%s, grp->state=0x%x, clk->mask=0x%x, clk->cnt=%d, clk->state=%d\n", __func__, grp->name, grp->state, clk->mask, clk->cnt, clk->state);
#endif

    return 0;
}

static int get_clk_state_locked(struct cg_clk *clk)
{
    if (likely(initialized)) { 
        return clk->state;
    } else {
        return clk->ops->get_state(clk);
    }
}

int mt_enable_clock(enum cg_clk_id id, char *name)
{
    int err;
    unsigned long flags;
    struct cg_clk *clk = id_to_clk(id);

    BUG_ON(!initialized);
    BUG_ON(!clk);
    BUG_ON(!clk->grp);
    BUG_ON(!clk->ops->check_validity(clk));
    BUG_ON(!name);
#ifdef CLK_LOG_TOP
    clk_info("[%s]: id=%d, names=%s\n", __func__, id, name);
#else
    if (id == MT_CG_DISP0_SMI_COMMON)
        clk_dbg("[%s]: id=%d, names=%s\n", __func__, id, name);
#endif

    clkmgr_lock(flags);
    err = clk_enable_internal(clk, name);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(mt_enable_clock);


int mt_disable_clock(enum cg_clk_id id, char *name)
{
    int err;
    unsigned long flags;
    struct cg_clk *clk = id_to_clk(id);

    BUG_ON(!initialized);
    BUG_ON(!clk);
    BUG_ON(!clk->grp);
    BUG_ON(!clk->ops->check_validity(clk));
    BUG_ON(!name);
#ifdef CLK_LOG_TOP
    clk_info("[%s]: id=%d, names=%s\n", __func__, id, name);
#else
    if (id == MT_CG_DISP0_SMI_COMMON)
        clk_dbg("[%s]: id=%d, names=%s\n", __func__, id, name);
#endif

    clkmgr_lock(flags);
    err = clk_disable_internal(clk, name);
    clkmgr_unlock(flags);

    return err;
}
EXPORT_SYMBOL(mt_disable_clock);

int enable_clock_ext_locked(int id, char *name)
{
//             
    int err;
    struct cg_clk *clk = id_to_clk(id);
return 0;

    BUG_ON(!initialized);
    BUG_ON(!clk);
    BUG_ON(!clk->grp);
    BUG_ON(!clk->ops->check_validity(clk));

    BUG_ON(!clkmgr_locked());
    err = clk_enable_internal(clk, name);

    return err;
}
EXPORT_SYMBOL(enable_clock_ext_locked);


int disable_clock_ext_locked(int id, char *name)
{
//             
    int err;
    struct cg_clk *clk = id_to_clk(id);
return 0;

    BUG_ON(!initialized);
    BUG_ON(!clk);
    BUG_ON(!clk->grp);
    BUG_ON(!clk->ops->check_validity(clk));

    BUG_ON(!clkmgr_locked());
    err = clk_disable_internal(clk, name);

    return err;
}
EXPORT_SYMBOL(disable_clock_ext_locked);

int clock_is_on(int id)
{
//             
    int state;
    unsigned long flags;
    struct cg_clk *clk = id_to_clk(id);    

    BUG_ON(!clk);
    BUG_ON(!clk->grp);
    BUG_ON(!clk->ops->check_validity(clk));

    clkmgr_lock(flags);
    state = get_clk_state_locked(clk);
    clkmgr_unlock(flags);

    return state;
}
EXPORT_SYMBOL(clock_is_on);


static void clk_set_force_on_locked(struct cg_clk *clk)
{
    clk->force_on = 1;
}

static void clk_clr_force_on_locked(struct cg_clk *clk)
{
    clk->force_on = 0;
}

void clk_set_force_on(int id)
{
    unsigned long flags;
    struct cg_clk *clk = id_to_clk(id);

    BUG_ON(!initialized);
    BUG_ON(!clk);
    BUG_ON(!clk->grp);
    BUG_ON(!clk->ops->check_validity(clk));

    clkmgr_lock(flags);
    clk_set_force_on_locked(clk);
    clkmgr_unlock(flags);
}
EXPORT_SYMBOL(clk_set_force_on);

void clk_clr_force_on(int id)
{
    unsigned long flags;
    struct cg_clk *clk = id_to_clk(id);
    
    BUG_ON(!initialized);
    BUG_ON(!clk);
    BUG_ON(!clk->grp);
    BUG_ON(!clk->ops->check_validity(clk));

    clkmgr_lock(flags);
    clk_clr_force_on_locked(clk);
    clkmgr_unlock(flags);
}
EXPORT_SYMBOL(clk_clr_force_on);

int clk_is_force_on(int id)
{
    struct cg_clk *clk = id_to_clk(id);

    BUG_ON(!initialized);
    BUG_ON(!clk);
    BUG_ON(!clk->grp);
    BUG_ON(!clk->ops->check_validity(clk));

    return clk->force_on;
}

int grp_dump_regs(int id, unsigned int *ptr)
{
    struct cg_grp *grp = id_to_grp(id);    

    //                     
    BUG_ON(!grp);
	
    return grp->ops->dump_regs(grp, ptr);
}
EXPORT_SYMBOL(grp_dump_regs);

const char* grp_get_name(int id)
{
    struct cg_grp *grp = id_to_grp(id);    

    //                     
    BUG_ON(!grp);

    return grp->name;
}

void print_grp_regs(void)
{
    int i;
    int cnt;
    unsigned int value[2];
    const char *name;

    for (i = 0; i < NR_GRPS; i++) {
        name = grp_get_name(i);
        cnt = grp_dump_regs(i, value);
        if (cnt == 1) {
            clk_info("[%02d][%-8s]=[0x%08x]\n", i, name, value[0]);
        } else {
            clk_info("[%02d][%-8s]=[0x%08x][0x%08x]\n", i, name, value[0], value[1]);
        }
    }
}



/*                                               
                                                 
                                                */

#if 0
static void subsys_all_force_on(void)
{
    if (test_spm_gpu_power_on()) {
        spm_mtcmos_ctrl_mfg(STA_POWER_ON);
    } else {
        clk_warn("[%s]: not force to turn on MFG\n", __func__);
    }

    spm_mtcmos_ctrl_vdec(STA_POWER_ON);
    spm_mtcmos_ctrl_venc(STA_POWER_ON);
}
#endif

static void cg_all_force_on(void)
{
    //                    
    //        
    clk_writel(INFRA_PDN_CLR, 0x91BFE3);
    //       
    clk_writel(PERI_PDN0_CLR, 0x3FFFF);
    //     
    clk_clrl(AUDIO_TOP_CON0, (0x1 << 6) | (0x1 << 2));
    //   
    clk_writel(MFG_CG_CLR, 0x1);
    //    
    clk_writel(DISP_CG_CLR0, 0x7FFFF);
    clk_writel(DISP_CG_CLR1, 0xF);
    //   
    clk_writel(IMG_CG_CLR, 0x3E1);
    //   
    clk_writel(VDEC_CKEN_SET, 0x1);
    clk_writel(LARB_CKEN_SET, 0x1);

}

static void cg_bootup_pdn(void)
{
    //     
    clk_writel(AUDIO_TOP_CON0, (0x1 << 6) | (0x1 << 2));

    //        
    //                                   
    clk_writel(INFRA_PDN_SET, 0xB0E0); //                 

    //       
    //                                     
    clk_writel(PERI_PDN0_SET, 0x2FEF7FD); //                       

    //   
    clk_writel(MFG_CG_SET, 0x1);

    //    
    //                                  
    clk_writel(DISP_CG_SET0, 0x37E0C);
    clk_writel(DISP_CG_SET1, 0xc);

    //   
    clk_writel(IMG_CG_SET, 0x3E1);    	//                               
    //   
    clk_writel(VDEC_CKEN_CLR, 0x1);
    clk_writel(LARB_CKEN_CLR, 0x1);
}


static void mt_subsys_init(void)
{
    int i;
    struct subsys *sys;

    for (i = 0; i < NR_SYSS; i++) {
        sys = &syss[i];
        sys->state = sys->ops->get_state(sys);
        if (sys->state != sys->default_sta) {
            clk_info("[%s]%s, change state: (%u->%u)\n", __func__, 
                    sys->name, sys->state, sys->default_sta);
            if (sys->default_sta == PWR_DOWN) {
                sys_disable_locked(sys, 1);
            } else {
                sys_enable_locked(sys);
            }
        }
#ifdef CONFIG_CLKMGR_STAT
        INIT_LIST_HEAD(&sys->head);
#endif
    }

#if 0
    //         
    syss[SYS_DIS].mux = &muxs[MT_MUX_DISP];
    syss[SYS_VEN].mux = &muxs[MT_MUX_VENC];
    syss[SYS_VDE].mux = &muxs[MT_MUX_VDEC];
#endif
}

static void mt_plls_init(void)
{
    int i;
    struct pll *pll;
    for (i = 0; i < NR_PLLS; i++) {
        pll = &plls[i];
        pll->state = pll->ops->get_state(pll);
        
//                                                                                         
        
#ifdef CONFIG_CLKMGR_STAT
        INIT_LIST_HEAD(&pll->head);
#endif
    }
    //                    
    //                    
    plls[MMPLL].cnt=1;
    plls[VENCPLL].cnt=1;
    
    
}

static void mt_plls_enable_hp(void)
{
    int i;
    struct pll *pll;
    for (i = 0; i < NR_PLLS; i++) {
        pll = &plls[i];
        if (pll->ops->hp_enable) {
            pll->ops->hp_enable(pll);
        }
    }
}


static void mt_muxs_init(void)
{
    int i;
    struct clkmux *mux;
//                                
/*
                                                  
                                                 
                                                 
                                                      
                                                  
                                                         
                                              
                                                          
*/
    clk_setl(CLK_CFG_2, 0x00008000);	

    for (i = 0; i < NR_MUXS; i++) {
        mux = &muxs[i];
#ifdef CONFIG_CLKMGR_STAT
        INIT_LIST_HEAD(&mux->head);
#endif
    }
    muxs[MT_MUX_MM].cnt = 1;
    //                           
    muxs[MT_MUX_MFG].cnt = 1;
    muxs[MT_MUX_VDEC].cnt = 1;
    //                         
    //                              
    //                           
    //                         
    //                          
    //                               
    //                           
    //                              
    //                              
   
/*
                              

                                                            

                             
             
                                                           
              
             
                                                           
              
            
                                              
              
     */

#if 0
    //         
    muxs[MT_MUX_AUDINTBUS].siblings = &muxs[MT_MUX_AUDIO];

    muxs[MT_MUX_MSDC0].pll = &plls[MSDCPLL];
    muxs[MT_MUX_MSDC1].pll = &plls[MSDCPLL];
    muxs[MT_MUX_MSDC2].pll = &plls[MSDCPLL];
    muxs[MT_MUX_MSDC3].pll = &plls[MSDCPLL];
    muxs[MT_MUX_MSDC4].pll = &plls[MSDCPLL];

    muxs[MT_MUX_MFG].pll = &plls[MMPLL];
    muxs[MT_MUX_VENC].pll = &plls[MMPLL];
    muxs[MT_MUX_HYD].pll = &plls[MMPLL];

    muxs[MT_MUX_JPG].pll = &plls[MAINPLL];
    muxs[MT_MUX_CAM].pll = &plls[MAINPLL];
    muxs[MT_MUX_VDEC].pll = &plls[MAINPLL];
#endif
}


static void mt_clks_init(void)
{
    int i, j;
    struct cg_grp *grp;
    struct cg_clk *clk;

/*  
                                                                    
                                                  
                                                        
*/
    
    for (i = 0; i < NR_GRPS; i++) {
        grp = &grps[i];
        grp->state = grp->ops->get_state(grp);
        
//                                                 
        
        for (j = 0; j < 32; j++) {
            if (grp->mask & (1U << j)) {
                clk = &clks[i * 32 + j];
                //               
                //             
                clk->mask = 1U << j;
                clk->state = clk->ops->get_state(clk); 
                //                                             
                
//                                                                                   
                
#ifdef CONFIG_CLKMGR_STAT
                INIT_LIST_HEAD(&clk->head);
#endif
            }
        }
    }
/*
                                                    
                                                

                                                            
                                                            

                                                          

                                                     
                                                     
                                                      
                                                      
                                                      
                                                      
                                                      
                                                    
                                                     
                                                     
                                                     
                                                     
                                                   

                                                       
                                                
                                                

                                                      
                                                      
                                                       
                                                       
                                                       

                                                          
*/
    clks[MT_CG_INFRA_AUDIO].ops = &audio_cg_clk_ops;
    
    clks[MT_CG_AUDIO_AFE].parent = &clks[MT_CG_INFRA_AUDIO];
    clks[MT_CG_AUDIO_I2S].parent = &clks[MT_CG_INFRA_AUDIO];

//                                                  
//                                                  
    
    clks[MT_CG_INFRA_AUDIO].mux = &muxs[MT_MUX_AUDINTBUS];
    
    clks[MT_CG_PERI_USB0].mux = &muxs[MT_MUX_USB20];
    clks[MT_CG_PERI_MSDC30_0].mux = &muxs[MT_MUX_MSDC30_0];
    clks[MT_CG_PERI_MSDC30_1].mux = &muxs[MT_MUX_MSDC30_1];
    clks[MT_CG_PERI_MSDC30_2].mux = &muxs[MT_MUX_MSDC30_2];
    
    clks[MT_CG_PERI_UART0].mux = &muxs[MT_MUX_UART];
    clks[MT_CG_PERI_UART1].mux = &muxs[MT_MUX_UART];
    clks[MT_CG_PERI_UART2].mux = &muxs[MT_MUX_UART];
    clks[MT_CG_PERI_UART3].mux = &muxs[MT_MUX_UART];
    clks[MT_CG_PERI_SPI0].mux = &muxs[MT_MUX_SPI];
    
    clks[MT_CG_IMAGE_SEN_TG].mux = &muxs[MT_MUX_CAMTG];
    clks[MT_CG_IMAGE_SEN_CAM].mux = &muxs[MT_MUX_CAMTG];
    
    clks[MT_CG_DISP0_DISP_BLS].mux = &muxs[MT_MUX_PWM];
    clks[MT_CG_DISP0_MDP_BLS_26M].mux = &muxs[MT_MUX_PWM];
    //                                                                  
    clk_set_force_on_locked(&clks[MT_CG_DISP0_SMI_LARB0]);
    clk_set_force_on_locked(&clks[MT_CG_DISP0_SMI_COMMON]);


}

#if 0
static void mt_md1_cg_init(void)
{
    clk_clrl(PERI_PDN_MD_MASK, 1U << 0);
    clk_writel(PERI_PDN0_MD1_SET, 0xFFFFFFFF);
}
#endif
#if 0
static void mt_md2_cg_init(void)
{
    clk_clrl(PERI_PDN_MD_MASK, 1U << 1);
    clk_writel(PERI_PDN0_MD2_SET, 0xFFFFFFFF);
}
#endif

int mt_clkmgr_bringup_init(void)
{
    BUG_ON(initialized);

    spm_mtcmos_ctrl_vdec(STA_POWER_ON);
    spm_mtcmos_ctrl_isp(STA_POWER_ON);
    spm_mtcmos_ctrl_mfg(STA_POWER_ON);	
    
    cg_all_force_on();
    cg_bootup_pdn();

    mt_plls_init();
    mt_subsys_init();
    mt_muxs_init();
    mt_clks_init();

    initialized = 1;
    mt_freqhopping_init(); 
    mt_freqhopping_pll_init();

    print_grp_regs();

    return 0;
}

void mt_clkmgr_init(void)
{
    unsigned long flags;
    BUG_ON(initialized);

    mt_plls_init();
    mt_subsys_init();
    mt_muxs_init();
    mt_clks_init();
    
//                     
    initialized = 1;

    mt_freqhopping_init(); 

    clkmgr_lock(flags);
    mt_freqhopping_pll_init(); 
    mt_plls_enable_hp();
    clkmgr_unlock(flags);
}



#ifdef CONFIG_MTK_MMC
extern void msdc_clk_status(int * status);
#else
void msdc_clk_status(int * status) { *status = 0; }
#endif

bool clkmgr_idle_can_enter(unsigned int *condition_mask, unsigned int *block_mask)
{
    int i,j;
    unsigned int sd_mask = 0;    
    unsigned int cg_mask = 0;

    msdc_clk_status(&sd_mask);
    if (sd_mask) {
        block_mask[CG_PERI] |= sd_mask;
        return false;
    }

    for (i = CG_PERI; i < NR_GRPS; i++) {
        cg_mask = grps[i].state & condition_mask[i]; 
        if (cg_mask) 
		{
			for (j = CG_PERI; j < NR_GRPS; j++) 
			{
				block_mask[j] = grps[j].state & condition_mask[j];
			}
			
            //                         
            return false;
        }
    }

    return true;
}

bool isp_vdec_on_off(void)
{
    unsigned int state;

    state = subsys_is_on(SYS_ISP);
    if(state==PWR_ON)
        return true;

    state= subsys_is_on(SYS_VDE);
    if(state==PWR_ON)
        return true;

    return false;	
}
EXPORT_SYMBOL(isp_vdec_on_off);
/*                                               
                                                 
                                                */

static int pll_test_read(struct seq_file *m, void *v)
{
    int i,j;
    int cnt;
    unsigned int value[3];
    const char *name;

    seq_printf(m, "********** pll register dump **********\n");
    for (i = 0; i < NR_PLLS; i++) {
        name = pll_get_name(i);
        cnt = pll_dump_regs(i, value);    
        for (j = 0; j < cnt; j++) {
            seq_printf(m, "[%d][%-7s reg%d]=[0x%08x]\n", i, name, j, value[j]); 
        }
    }

    seq_printf(m, "\n********** pll_test help **********\n");
    seq_printf(m, "enable  pll: echo enable  id [mod_name] > /proc/clkmgr/pll_test\n");
    seq_printf(m, "disable pll: echo disable id [mod_name] > /proc/clkmgr/pll_test\n");

    return 0;
}

static int pll_test_write(struct file *file, const char __user *buffer, 
                size_t count, loff_t *data)
{
    char desc[32]; 
    int len = 0;

    char cmd[10];
    char mod_name[10];
    int id;
    int err = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %s", cmd, &id, mod_name) == 3) { 
        if (!strcmp(cmd, "enable")) {
            err = enable_pll(id, mod_name);
        } else if (!strcmp(cmd, "disable")) {
            err = disable_pll(id, mod_name);
        }
    } else if (sscanf(desc, "%s %d", cmd, &id) == 2) { 
        if (!strcmp(cmd, "enable")) {
            err = enable_pll(id, "pll_test");
        } else if (!strcmp(cmd, "disable")) {
            err = disable_pll(id, "pll_test");
        }
    }

    clk_info("[%s]%s pll %d: result is %d\n", __func__, cmd, id, err);

    return count;
}

static int pll_fsel_read(struct seq_file *m, void *v)
{
    int i;
    int cnt;
    unsigned int value[3];
    const char *name;

    for (i = 0; i < NR_PLLS; i++) {
        name = pll_get_name(i);
        if (pll_is_on(i)) {
            cnt = pll_dump_regs(i, value);    
            if (cnt >= 2) {
                seq_printf(m, "[%d][%-7s]=[0x%08x%08x]\n", i, name, value[0], value[1]);
            } else {
                seq_printf(m, "[%d][%-7s]=[0x%08x]\n", i, name, value[0]);
            }
        } else {
            seq_printf(m, "[%d][%-7s]=[-1]\n", i, name);
        }
    }

    seq_printf(m, "\n********** pll_fsel help **********\n");
    seq_printf(m, "adjust pll frequency:  echo id freq > /proc/clkmgr/pll_fsel\n");

    return 0;
}

static int pll_fsel_write(struct file *file, const char __user *buffer, 
                size_t count, loff_t *data)
{
    char desc[32]; 
    int len = 0;

    int id;
    unsigned int value;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%d %x", &id, &value) == 2) { 
        pll_fsel(id, value);
    }

    return count;
}


#ifdef CONFIG_CLKMGR_STAT
static int pll_stat_read(struct seq_file *m, void *v)
{
    struct pll *pll;
    struct list_head *pos;
    struct stat_node *node;
    int i;

    seq_printf(m, "\n********** pll stat dump **********\n");
    for (i = 0; i < NR_PLLS; i++) {
        pll = id_to_pll(i);
        seq_printf(m, "[%d][%-7s]state=%u, cnt=%u", i, pll->name, 
                pll->state, pll->cnt);
        list_for_each(pos, &pll->head) {
            node = list_entry(pos, struct stat_node, link);
            seq_printf(m, "\t(%s,%u,%u)", node->name, node->cnt_on, node->cnt_off);
        }
        seq_printf(m, "\n");
    }

    seq_printf(m, "\n********** pll_dump help **********\n");

    return 0;
}
#endif


static int subsys_test_read(struct seq_file *m, void *v)
{
    int i;
    int state;
    unsigned int value=0, sta, sta_s;
    const char *name;

    sta = clk_readl(SPM_PWR_STATUS);
    sta_s = clk_readl(SPM_PWR_STATUS_S);

    seq_printf(m, "********** subsys register dump **********\n");
    for (i = 0; i < NR_SYSS; i++) {
        name = subsys_get_name(i);
        state = subsys_is_on(i);
        subsys_dump_regs(i, &value);    
        seq_printf(m, "[%d][%-7s]=[0x%08x], state(%u)\n", i, name, value, state);
    }
    seq_printf(m, "SPM_PWR_STATUS=0x%08x, SPM_PWR_STATUS_2ND=0x%08x\n", sta, sta_s);

    seq_printf(m, "\n********** subsys_test help **********\n");
    seq_printf(m, "enable subsys:  echo enable id > /proc/clkmgr/subsys_test\n");
    seq_printf(m, "disable subsys: echo disable id [force_off] > /proc/clkmgr/subsys_test\n");

    return 0;
}

static int subsys_test_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;

    char cmd[10];
    int id;
    int force_off;
    int err = 0;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %d", cmd, &id, &force_off) == 3) {
        if (!strcmp(cmd, "disable")) {
            err = disable_subsys_force(id, "test");
        }
    } else if (sscanf(desc, "%s %d", cmd, &id) == 2) { 
        if (!strcmp(cmd, "enable")) {
            err = enable_subsys(id, "test");
        } else if (!strcmp(cmd, "disable")) {
            err = disable_subsys(id, "test");
        }
    }

    clk_info("[%s]%s subsys %d: result is %d\n", __func__, cmd, id, err);

    return count;
}


#ifdef CONFIG_CLKMGR_STAT
static int subsys_stat_read(struct seq_file *m, void *v)
{
    struct subsys *sys;
    struct list_head *pos;
    struct stat_node *node;
    int i;

    seq_printf(m, "\n********** subsys stat dump **********\n");
    for (i = 0; i < NR_SYSS; i++) {
        sys = id_to_sys(i);
        seq_printf(m, "[%d][%-7s]state=%u", i, sys->name, sys->state);
        list_for_each(pos, &sys->head) {
            node = list_entry(pos, struct stat_node, link);
            seq_printf(m, "\t(%s,%u,%u)", node->name, node->cnt_on, node->cnt_off);
        }
        seq_printf(m, "\n");
    }

    seq_printf(m, "\n********** subsys_dump help **********\n");

    return 0;
}
#endif


static int mux_test_read(struct seq_file *m, void *v)
{
    seq_printf(m, "********** mux register dump *********\n");
    seq_printf(m, "[CLK_CFG_0]=0x%08x\n", clk_readl(CLK_CFG_0));
    seq_printf(m, "[CLK_CFG_1]=0x%08x\n", clk_readl(CLK_CFG_1));
    seq_printf(m, "[CLK_CFG_2]=0x%08x\n", clk_readl(CLK_CFG_2));
    seq_printf(m, "[CLK_CFG_3]=0x%08x\n", clk_readl(CLK_CFG_3));
    seq_printf(m, "[CLK_CFG_4]=0x%08x\n", clk_readl(CLK_CFG_4));
    seq_printf(m, "[CLK_CFG_8]=0x%08x\n", clk_readl(CLK_CFG_8));
    seq_printf(m, "[CLK_CFG_9]=0x%08x\n", clk_readl(CLK_CFG_9));
    seq_printf(m, "[CLK_CFG_10]=0x%08x\n", clk_readl(CLK_CFG_10));
    seq_printf(m, "[CLK_CFG_11]=0x%08x\n", clk_readl(CLK_CFG_11));

    seq_printf(m, "\n********** mux_test help *********\n");

    return 0;
}



#ifdef CONFIG_CLKMGR_STAT
static int mux_stat_read(struct seq_file *m, void *v)
{
    struct clkmux *mux;
    struct list_head *pos;
    struct stat_node *node;
    int i;

    seq_printf(m, "********** mux stat dump **********\n");
    for (i = 0; i < NR_MUXS; i++) {
        mux = id_to_mux(i);
#if 0
        seq_printf(m, "[%02d][%-14s]state=%u, cnt=%u", i, mux->name, 
                mux->state, mux->cnt);
#else
        seq_printf(m, "[%02d][%-14s]cnt=%u", i, mux->name, mux->cnt);
#endif
        list_for_each(pos, &mux->head) {
            node = list_entry(pos, struct stat_node, link);
            seq_printf(m, "\t(%s,%u,%u)", node->name, node->cnt_on, node->cnt_off);
        }
        seq_printf(m, "\n");
    }

    seq_printf(m, "\n********** mux_dump help **********\n");

    return 0;
}
#endif


static int clk_test_read(struct seq_file *m, void *v)
{
    int i;
    int cnt;
    unsigned int value[3];
    const char *name;

    seq_printf(m, "********** clk register dump **********\n");

    for (i = 0; i < NR_GRPS; i++) {
        name = grp_get_name(i);
        cnt = grp_dump_regs(i, value);
        if (cnt == 1) {
            seq_printf(m, "[%02d][%-8s]=[0x%08x]\n", i, name, value[0]);
        } else if (cnt == 2){
            seq_printf(m, "[%02d][%-8s]=[0x%08x][0x%08x]\n", i, name, value[0], value[1]);
        } else {
            seq_printf(m, "[%02d][%-8s]=[0x%08x][0x%08x][0x%08x]\n", i, name, value[0], value[1], value[2]);
        }    
    }
	
    seq_printf(m, "\n********** clk_test help **********\n");
    seq_printf(m, "enable  clk: echo enable  id [mod_name] > /proc/clkmgr/clk_test\n");
    seq_printf(m, "disable clk: echo disable id [mod_name] > /proc/clkmgr/clk_test\n");
    seq_printf(m, "read state:  echo id > /proc/clkmgr/clk_test\n");

    return 0;
}

static int clk_test_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;

    char cmd[10];
    char mod_name[10];
    int id;
    int err;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d %s", cmd, &id, mod_name) == 3) { 
        if (!strcmp(cmd, "enable")) {
            err = enable_clock(id, mod_name);
        } else if (!strcmp(cmd, "disable")) {
            err = disable_clock(id, mod_name);
        }
    } else if (sscanf(desc, "%s %d", cmd, &id) == 2) { 
        if (!strcmp(cmd, "enable")) {
            err = enable_clock(id, "pll_test");
        } else if (!strcmp(cmd, "disable")) {
            err = disable_clock(id, "pll_test");
        }
    } else if (sscanf(desc, "%d", &id) == 1) { 
        clk_info("clock %d is %s\n", id, clock_is_on(id) ? "on" : "off");
    }

    //                                                                    

    return count;
}


#ifdef CONFIG_CLKMGR_STAT
static int clk_stat_read(struct seq_file *m, void *v)
{
    struct cg_clk *clk;
    struct list_head *pos;
    struct stat_node *node;
    int i, grp, offset;
    int skip;

    seq_printf(m, "\n********** clk stat dump **********\n");
    for (i = 0; i < NR_CLKS; i++) {
        grp = i / 32;
        offset = i % 32;
        if (offset == 0) {
            seq_printf(m, "\n*****[%02d][%-8s]*****\n", grp, grp_get_name(grp));
        }

        clk = id_to_clk(i);
        if (!clk || !clk->grp || !clk->ops->check_validity(clk))
            continue;

        skip = (clk->cnt == 0) && (clk->state == 0) && list_empty(&clk->head);
        if (skip)
            continue;

        seq_printf(m, "[%02d]state=%u, cnt=%u", offset, clk->state, clk->cnt);
        list_for_each(pos, &clk->head) {
            node = list_entry(pos, struct stat_node, link);
            seq_printf(m, "\t(%s,%u,%u)", node->name, node->cnt_on, node->cnt_off);
        }
        seq_printf(m, "\n");
    }

    seq_printf(m, "\n********** clk_dump help **********\n");

    return 0;
}
#endif


static int clk_force_on_read(struct seq_file *m, void *v)
{
    int i;
    struct cg_clk *clk;

    seq_printf(m, "********** clk force on info dump **********\n");
    for (i = 0; i < NR_CLKS; i++) {
        clk = &clks[i];
        if (clk->force_on) {
            seq_printf(m, "clock %d (0x%08x @ %s) is force on\n", i, 
                    clk->mask, clk->grp->name);
        }
    }

    seq_printf(m, "\n********** clk_force_on help **********\n");
    seq_printf(m, "set clk force on: echo set id > /proc/clkmgr/clk_force_on\n");
    seq_printf(m, "clr clk force on: echo clr id > /proc/clkmgr/clk_force_on\n");

    return 0;
}

static int clk_force_on_write(struct file *file, const char *buffer,
                unsigned long count, void *data)
{
    char desc[32]; 
    int len = 0;

    char cmd[10];
    int id;
    
    len = (count < (sizeof(desc) - 1)) ? count : (sizeof(desc) - 1);
    if (copy_from_user(desc, buffer, len)) {
        return 0;
    }
    desc[len] = '\0';

    if (sscanf(desc, "%s %d", cmd, &id) == 2) { 
        if (!strcmp(cmd, "set")) {
            clk_set_force_on(id);
        } else if (!strcmp(cmd, "clr")) {
            clk_clr_force_on(id);
        }
    }

    return count;
}

/*
                                                                
                                                
 
                   
                

                                                                  
                                                                             

                        

                   
                  
                   
        
                

             
                                      
 

                                                                   
                                                
 
                   
                

                       
                          
    
                                                                    
                                            
                 
     
                     

                                           
                                    
                      
                                    
                                                                        
                                          
     

                 
 
*/

//            
static int proc_pll_test_open(struct inode *inode, struct file *file)
{
    return single_open(file, pll_test_read, NULL);
}
static const struct file_operations pll_test_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_pll_test_open, 
    .read  = seq_read,
    .write = pll_test_write,
};

//            
static int proc_pll_fsel_open(struct inode *inode, struct file *file)
{
    return single_open(file, pll_fsel_read, NULL);
}
static const struct file_operations pll_fsel_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_pll_fsel_open, 
    .read  = seq_read,
    .write = pll_fsel_write,
};

#ifdef CONFIG_CLKMGR_STAT
//            
static int proc_pll_stat_open(struct inode *inode, struct file *file)
{
    return single_open(file, pll_stat_read, NULL);
}
static const struct file_operations pll_stat_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_pll_stat_open, 
    .read  = seq_read,
};
#endif

//               
static int proc_subsys_test_open(struct inode *inode, struct file *file)
{
    return single_open(file, subsys_test_read, NULL);
}
static const struct file_operations subsys_test_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_subsys_test_open, 
    .read  = seq_read,
    .write = subsys_test_write
};

#ifdef CONFIG_CLKMGR_STAT
//               
static int proc_subsys_stat_open(struct inode *inode, struct file *file)
{
    return single_open(file, subsys_stat_read, NULL);
}
static const struct file_operations subsys_stat_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_subsys_stat_open, 
    .read  = seq_read,
};
#endif

//            
static int proc_mux_test_open(struct inode *inode, struct file *file)
{
    return single_open(file, mux_test_read, NULL);
}
static const struct file_operations mux_test_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_mux_test_open, 
    .read  = seq_read,
};

#ifdef CONFIG_CLKMGR_STAT
//            
static int proc_mux_stat_open(struct inode *inode, struct file *file)
{
    return single_open(file, mux_stat_read, NULL);
}
static const struct file_operations mux_stat_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_mux_stat_open,
    .read  = seq_read,
};
#endif

//            
static int proc_clk_test_open(struct inode *inode, struct file *file)
{
    return single_open(file, clk_test_read, NULL);
}
static const struct file_operations clk_test_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_clk_test_open,
    .read  = seq_read,
    .write = clk_test_write,
};

#ifdef CONFIG_CLKMGR_STAT
//            
static int proc_clk_stat_open(struct inode *inode, struct file *file)
{
    return single_open(file, clk_stat_read, NULL);
}
static const struct file_operations clk_stat_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_clk_stat_open,
    .read  = seq_read,
};
#endif

//                
static int proc_clk_force_on_open(struct inode *inode, struct file *file)
{
    return single_open(file, clk_force_on_read, NULL);
}
static const struct file_operations clk_force_on_proc_fops = { 
    .owner = THIS_MODULE,
    .open  = proc_clk_force_on_open,
    .read  = seq_read,
    .write = clk_force_on_write,
};


void mt_clkmgr_debug_init(void)
{
    struct proc_dir_entry *entry;
    struct proc_dir_entry *clkmgr_dir;

    clkmgr_dir = proc_mkdir("clkmgr", NULL);
    if (!clkmgr_dir) {
        clk_err("[%s]: fail to mkdir /proc/clkmgr\n", __func__);
        return;
    }

    entry = proc_create("pll_test", S_IRUGO | S_IWUSR, clkmgr_dir, &pll_test_proc_fops);

    entry = proc_create("pll_fsel", S_IRUGO | S_IWUSR, clkmgr_dir, &pll_fsel_proc_fops);

#ifdef CONFIG_CLKMGR_STAT
    entry = proc_create("pll_stat", S_IRUGO, clkmgr_dir, &pll_stat_proc_fops);
#endif

    entry = proc_create("subsys_test", S_IRUGO | S_IWUSR, clkmgr_dir, &subsys_test_proc_fops);

#ifdef CONFIG_CLKMGR_STAT
    entry = proc_create("subsys_stat", S_IRUGO, clkmgr_dir, &subsys_stat_proc_fops);
#endif

    entry = proc_create("mux_test", S_IRUGO, clkmgr_dir, &mux_test_proc_fops);

#ifdef CONFIG_CLKMGR_STAT
    entry = proc_create("mux_stat", S_IRUGO, clkmgr_dir, &mux_stat_proc_fops);
#endif

    entry = proc_create("clk_test", S_IRUGO | S_IWUSR, clkmgr_dir, &clk_test_proc_fops);

#ifdef CONFIG_CLKMGR_STAT
    entry = proc_create("clk_stat", S_IRUGO, clkmgr_dir, &clk_stat_proc_fops);
#endif

    entry = proc_create("clk_force_on", S_IRUGO | S_IWUSR, clkmgr_dir, &clk_force_on_proc_fops);

//                                                                
//                
//                                            
//                                              
//     
}

/*                                  
                  
                                   */
#ifdef CONFIG_HAS_EARLYSUSPEND
static void clkmgr_early_suspend(struct early_suspend *h)
{
    clk_info("[%s]: Dump Top MUX register, CLK_CFG_0=0x%x, CLK_CFG_1=0x%x, CLK_CFG_2=0x%x, CLK_CFG_3=0x%x\n", \
    __func__, clk_readl(CLK_CFG_0), clk_readl(CLK_CFG_1), clk_readl(CLK_CFG_2), clk_readl(CLK_CFG_3));
    
    return;
}
static void clkmgr_late_resume(struct early_suspend *h)
{
    clk_info("[%s]: Dump Top MUX register, CLK_CFG_0=0x%x, CLK_CFG_1=0x%x, CLK_CFG_2=0x%x, CLK_CFG_3=0x%x\n", \
    __func__, clk_readl(CLK_CFG_0), clk_readl(CLK_CFG_1), clk_readl(CLK_CFG_2), clk_readl(CLK_CFG_3));
    
    return;
}

static struct early_suspend mt_clkmgr_early_suspend_handler =
{
    .level = EARLY_SUSPEND_LEVEL_DISABLE_FB + 250,
    .suspend = clkmgr_early_suspend,
    .resume  = clkmgr_late_resume,
};
#endif //                              

struct platform_device CLK_helper_device =
{
    .name = "CLK",
    .id   = -1,
    .dev  = {},
};

int CLK_pm_restore_noirq(struct device *device)
{
    struct subsys *sys;

    sys = &syss[SYS_DIS];
    sys->state = sys->ops->get_state(sys);

    muxs[MT_MUX_MM].cnt = 1;
    plls[VENCPLL].cnt=1;

    clk_set_force_on_locked(&clks[MT_CG_DISP0_SMI_LARB0]);
    clk_set_force_on_locked(&clks[MT_CG_DISP0_SMI_COMMON]);

    printk("CLK_pm_restore_noirq\n");

    return 0;
}

struct dev_pm_ops CLK_helper_pm_ops = {
    .restore_noirq = CLK_pm_restore_noirq,
};

static struct platform_driver CLK_helper_driver =
{
	.driver     = {
		.name	= "CLK",
#ifdef CONFIG_PM
        .pm     = &CLK_helper_pm_ops,
#endif
        .owner      = THIS_MODULE,
	},
	//                           
	//                             
};

static int mt_clkmgr_debug_bringup_init(void)
{
	int ret;
	
    mt_clkmgr_debug_init();

#ifdef CONFIG_HAS_EARLYSUSPEND    
    register_early_suspend(&mt_clkmgr_early_suspend_handler);
#endif

    ret = platform_device_register(&CLK_helper_device);
    if (ret) {
        printk("CLK_helper_device register fail(%d)\n", ret);
        return ret;
    }

    ret = platform_driver_register(&CLK_helper_driver);
    if (ret) {
        printk("CLK_helper_driver register fail(%d)\n", ret);
        return ret;
    }

    return 0;
}
static int __init mt_clkmgr_late_init(void)
{
	mt_enable_clock(MT_CG_MFG_G3D, "clkmgr");
	mt_disable_clock(MT_CG_MFG_G3D, "clkmgr");
	return 0;
}

module_init(mt_clkmgr_debug_bringup_init);
late_initcall(mt_clkmgr_late_init);

/*                               */
void CLKM_32K(bool flag)
{
    unsigned long flags;
    
    clkmgr_lock(flags);
    if(flag==true)
    {
        //          
        clk_setl(CLK_CFG_10, 0x00000002); //                                 
    }
    else
    {
        //           
        clk_clrl(CLK_CFG_10, 0x00000002);
    }
    clkmgr_unlock(flags);
    return;
}
EXPORT_SYMBOL(CLKM_32K);

int CLK_Monitor(enum ckmon_sel ckmon, enum monitor_clk_sel sel, int div)
{
    unsigned long flags;
    unsigned int ckmon_shift=0;
    unsigned int temp;
    
    if(div > 255)
    {
    	clk_info("CLK_Monitor error parameter\n");
    	return 1;
    }
    
    clkmgr_lock(flags);

    if(ckmon==1)
        ckmon_shift=0;
    else if(ckmon==2)
        ckmon_shift=8;
    else if(ckmon==3)
        ckmon_shift=16;

    temp = clk_readl(CLK_CFG_10);
    temp = temp & (~(0xf<<ckmon_shift));
    temp = temp | ((sel&0xf)<<ckmon_shift);
    clk_writel(CLK_CFG_10, temp);

    temp = clk_readl(CLK_CFG_11);
    temp = temp & (~(0xff<<ckmon_shift));
    temp = temp | ((div&0xff)<<ckmon_shift);
    clk_writel(CLK_CFG_11, temp);

    clk_info("CLK_Monitor Reg: CLK_CFG_10=0x%x, CLK_CFG_11=0x%x\n", clk_readl(CLK_CFG_10), clk_readl(CLK_CFG_11));

    clkmgr_unlock(flags);
    return 0;
}
EXPORT_SYMBOL(CLK_Monitor);