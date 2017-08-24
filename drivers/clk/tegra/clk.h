	/*
 * Copyright (c) 2012, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TEGRA_CLK_H
#define __TEGRA_CLK_H

#include <linux/clk-provider.h>
#include <linux/clkdev.h>

/* 
                                                                  
  
                                                              
                                     
                              
 */
struct tegra_clk_sync_source {
	struct		clk_hw hw;
	unsigned long	rate;
	unsigned long	max_rate;
};

#define to_clk_sync_source(_hw)					\
	container_of(_hw, struct tegra_clk_sync_source, hw)

extern const struct clk_ops tegra_clk_sync_source_ops;
struct clk *tegra_clk_register_sync_source(const char *name,
		unsigned long fixed_rate, unsigned long max_rate);

/* 
                                                       
  
                                                               
                                    
                                  
                                         
                                         
                                                 
                       
  
         
                                                                               
                                                                                
                                                               
                                                                         
                                                                                
                                 
                                                                              
                                                                              
                           
 */
struct tegra_clk_frac_div {
	struct clk_hw	hw;
	void __iomem	*reg;
	u8		flags;
	u8		shift;
	u8		width;
	u8		frac_width;
	spinlock_t	*lock;
};

#define to_clk_frac_div(_hw) container_of(_hw, struct tegra_clk_frac_div, hw)

#define TEGRA_DIVIDER_ROUND_UP BIT(0)
#define TEGRA_DIVIDER_FIXED BIT(1)
#define TEGRA_DIVIDER_INT BIT(2)
#define TEGRA_DIVIDER_UART BIT(3)

extern const struct clk_ops tegra_clk_frac_div_ops;
struct clk *tegra_clk_register_divider(const char *name,
		const char *parent_name, void __iomem *reg,
		unsigned long flags, u8 clk_divider_flags, u8 shift, u8 width,
		u8 frac_width, spinlock_t *lock);

/*
             
  
                                                    
                                   
                                   
                                                      
                                                   
  
                                                     
 */

/* 
                                                       
  
                                       
                                                        
                         
                      
                     
                               
 */
struct tegra_clk_pll_freq_table {
	unsigned long	input_rate;
	unsigned long	output_rate;
	u16		n;
	u16		m;
	u8		p;
	u8		cpcon;
};

/* 
                                                 
  
                       
                                              
 */
struct pdiv_map {
	u8 pdiv;
	u8 hw_val;
};

/* 
                                         
  
                                        
                                        
                                          
                                          
                                    
                                    
                                   
                                   
                                   
                                                
                                                     
                                                     
 */
struct tegra_clk_pll_params {
	unsigned long	input_min;
	unsigned long	input_max;
	unsigned long	cf_min;
	unsigned long	cf_max;
	unsigned long	vco_min;
	unsigned long	vco_max;

	u32		base_reg;
	u32		misc_reg;
	u32		lock_reg;
	u32		lock_mask;
	u32		lock_enable_bit_idx;
	u32		iddq_reg;
	u32		iddq_bit_idx;
	u32		aux_reg;
	u32		dyn_ramp_reg;
	u32		ext_misc_reg[3];
	int		stepa_shift;
	int		stepb_shift;
	int		lock_delay;
	int		max_p;
	struct pdiv_map *pdiv_tohw;
};

/* 
                                         
  
                                                               
                                       
                                                       
                                                     
                          
                    
                                       
                       
                                                       
                                                       
                                                    
                                                    
                                                   
                                                   
  
         
                                                                
                                                                    
                                                                   
                                                              
                                                                   
                                                              
                                                                   
                                                              
                                                                    
                                                     
                                                                  
                                                                
                                                                   
                    
                                                       
                                                                        
                     
                                        
                                                                    
 */
struct tegra_clk_pll {
	struct clk_hw	hw;
	void __iomem	*clk_base;
	void __iomem	*pmc;
	u32		flags;
	unsigned long	fixed_rate;
	spinlock_t	*lock;
	u8		divn_shift;
	u8		divn_width;
	u8		divm_shift;
	u8		divm_width;
	u8		divp_shift;
	u8		divp_width;
	struct tegra_clk_pll_freq_table	*freq_table;
	struct tegra_clk_pll_params	*params;
};

#define to_clk_pll(_hw) container_of(_hw, struct tegra_clk_pll, hw)

#define TEGRA_PLL_USE_LOCK BIT(0)
#define TEGRA_PLL_HAS_CPCON BIT(1)
#define TEGRA_PLL_SET_LFCON BIT(2)
#define TEGRA_PLL_SET_DCCON BIT(3)
#define TEGRA_PLLU BIT(4)
#define TEGRA_PLLM BIT(5)
#define TEGRA_PLL_FIXED BIT(6)
#define TEGRA_PLLE_CONFIGURE BIT(7)
#define TEGRA_PLL_LOCK_MISC BIT(8)
#define TEGRA_PLL_BYPASS BIT(9)
#define TEGRA_PLL_HAS_LOCK_ENABLE BIT(10)

extern const struct clk_ops tegra_clk_pll_ops;
extern const struct clk_ops tegra_clk_plle_ops;
struct clk *tegra_clk_register_pll(const char *name, const char *parent_name,
		void __iomem *clk_base, void __iomem *pmc,
		unsigned long flags, unsigned long fixed_rate,
		struct tegra_clk_pll_params *pll_params, u32 pll_flags,
		struct tegra_clk_pll_freq_table *freq_table, spinlock_t *lock);

struct clk *tegra_clk_register_plle(const char *name, const char *parent_name,
		void __iomem *clk_base, void __iomem *pmc,
		unsigned long flags, unsigned long fixed_rate,
		struct tegra_clk_pll_params *pll_params, u32 pll_flags,
		struct tegra_clk_pll_freq_table *freq_table, spinlock_t *lock);

struct clk *tegra_clk_register_pllxc(const char *name, const char *parent_name,
			    void __iomem *clk_base, void __iomem *pmc,
			    unsigned long flags, unsigned long fixed_rate,
			    struct tegra_clk_pll_params *pll_params,
			    u32 pll_flags,
			    struct tegra_clk_pll_freq_table *freq_table,
			    spinlock_t *lock);

struct clk *tegra_clk_register_pllm(const char *name, const char *parent_name,
			   void __iomem *clk_base, void __iomem *pmc,
			   unsigned long flags, unsigned long fixed_rate,
			   struct tegra_clk_pll_params *pll_params,
			   u32 pll_flags,
			   struct tegra_clk_pll_freq_table *freq_table,
			   spinlock_t *lock);

struct clk *tegra_clk_register_pllc(const char *name, const char *parent_name,
			   void __iomem *clk_base, void __iomem *pmc,
			   unsigned long flags, unsigned long fixed_rate,
			   struct tegra_clk_pll_params *pll_params,
			   u32 pll_flags,
			   struct tegra_clk_pll_freq_table *freq_table,
			   spinlock_t *lock);

struct clk *tegra_clk_register_pllre(const char *name, const char *parent_name,
			   void __iomem *clk_base, void __iomem *pmc,
			   unsigned long flags, unsigned long fixed_rate,
			   struct tegra_clk_pll_params *pll_params,
			   u32 pll_flags,
			   struct tegra_clk_pll_freq_table *freq_table,
			   spinlock_t *lock, unsigned long parent_rate);

struct clk *tegra_clk_register_plle_tegra114(const char *name,
				const char *parent_name,
				void __iomem *clk_base, unsigned long flags,
				unsigned long fixed_rate,
				struct tegra_clk_pll_params *pll_params,
				struct tegra_clk_pll_freq_table *freq_table,
				spinlock_t *lock);

/* 
                                                    
  
                                                                
                                             
                                                  
                                         
                        
                                   
 */
struct tegra_clk_pll_out {
	struct clk_hw	hw;
	void __iomem	*reg;
	u8		enb_bit_idx;
	u8		rst_bit_idx;
	spinlock_t	*lock;
	u8		flags;
};

#define to_clk_pll_out(_hw) container_of(_hw, struct tegra_clk_pll_out, hw)

extern const struct clk_ops tegra_clk_pll_out_ops;
struct clk *tegra_clk_register_pll_out(const char *name,
		const char *parent_name, void __iomem *reg, u8 enb_bit_idx,
		u8 rst_bit_idx, unsigned long flags, u8 pll_div_flags,
		spinlock_t *lock);

/* 
                                                                         
  
                                    
                                        
                                         
                                   
                                                          
                                                            
 */
struct tegra_clk_periph_regs {
	u32 enb_reg;
	u32 enb_set_reg;
	u32 enb_clr_reg;
	u32 rst_reg;
	u32 rst_set_reg;
	u32 rst_clr_reg;
};

/* 
                                                       
  
                                         
                                                                
                                        
                                              
                                   
                          
                                                                 
  
         
                                                                        
                       
                                                                      
                                                                      
                   
                                                                      
                                                                       
                                          
                                                                     
 */
struct tegra_clk_periph_gate {
	u32			magic;
	struct clk_hw		hw;
	void __iomem		*clk_base;
	u8			flags;
	int			clk_num;
	int			*enable_refcnt;
	struct tegra_clk_periph_regs	*regs;
};

#define to_clk_periph_gate(_hw)					\
	container_of(_hw, struct tegra_clk_periph_gate, hw)

#define TEGRA_CLK_PERIPH_GATE_MAGIC 0x17760309

#define TEGRA_PERIPH_NO_RESET BIT(0)
#define TEGRA_PERIPH_MANUAL_RESET BIT(1)
#define TEGRA_PERIPH_ON_APB BIT(2)
#define TEGRA_PERIPH_WAR_1005168 BIT(3)

void tegra_periph_reset(struct tegra_clk_periph_gate *gate, bool assert);
extern const struct clk_ops tegra_clk_periph_gate_ops;
struct clk *tegra_clk_register_periph_gate(const char *name,
		const char *parent_name, u8 gate_flags, void __iomem *clk_base,
		unsigned long flags, int clk_num,
		struct tegra_clk_periph_regs *pregs, int *enable_refcnt);

/* 
                                       
  
                                        
                                                               
                  
                          
                    
                          
                              
                            
 */
struct tegra_clk_periph {
	u32			magic;
	struct clk_hw		hw;
	struct clk_mux		mux;
	struct tegra_clk_frac_div	divider;
	struct tegra_clk_periph_gate	gate;

	const struct clk_ops	*mux_ops;
	const struct clk_ops	*div_ops;
	const struct clk_ops	*gate_ops;
};

#define to_clk_periph(_hw) container_of(_hw, struct tegra_clk_periph, hw)

#define TEGRA_CLK_PERIPH_MAGIC 0x18221223

extern const struct clk_ops tegra_clk_periph_ops;
struct clk *tegra_clk_register_periph(const char *name,
		const char **parent_names, int num_parents,
		struct tegra_clk_periph *periph, void __iomem *clk_base,
		u32 offset, unsigned long flags);
struct clk *tegra_clk_register_periph_nodiv(const char *name,
		const char **parent_names, int num_parents,
		struct tegra_clk_periph *periph, void __iomem *clk_base,
		u32 offset);

#define TEGRA_CLK_PERIPH(_mux_shift, _mux_mask, _mux_flags,		\
			 _div_shift, _div_width, _div_frac_width,	\
			 _div_flags, _clk_num, _enb_refcnt, _regs,	\
			 _gate_flags, _table)				\
	{								\
		.mux = {						\
			.flags = _mux_flags,				\
			.shift = _mux_shift,				\
			.mask = _mux_mask,				\
			.table = _table,				\
		},							\
		.divider = {						\
			.flags = _div_flags,				\
			.shift = _div_shift,				\
			.width = _div_width,				\
			.frac_width = _div_frac_width,			\
		},							\
		.gate = {						\
			.flags = _gate_flags,				\
			.clk_num = _clk_num,				\
			.enable_refcnt = _enb_refcnt,			\
			.regs = _regs,					\
		},							\
		.mux_ops = &clk_mux_ops,				\
		.div_ops = &tegra_clk_frac_div_ops,			\
		.gate_ops = &tegra_clk_periph_gate_ops,			\
	}

struct tegra_periph_init_data {
	const char *name;
	int clk_id;
	const char **parent_names;
	int num_parents;
	struct tegra_clk_periph periph;
	u32 offset;
	const char *con_id;
	const char *dev_id;
	unsigned long flags;
};

#define TEGRA_INIT_DATA_TABLE(_name, _con_id, _dev_id, _parent_names, _offset,\
			_mux_shift, _mux_mask, _mux_flags, _div_shift,	\
			_div_width, _div_frac_width, _div_flags, _regs,	\
			_clk_num, _enb_refcnt, _gate_flags, _clk_id, _table,\
			_flags) \
	{								\
		.name = _name,						\
		.clk_id = _clk_id,					\
		.parent_names = _parent_names,				\
		.num_parents = ARRAY_SIZE(_parent_names),		\
		.periph = TEGRA_CLK_PERIPH(_mux_shift, _mux_mask,	\
					   _mux_flags, _div_shift,	\
					   _div_width, _div_frac_width,	\
					   _div_flags, _clk_num,	\
					   _enb_refcnt, _regs,		\
					   _gate_flags, _table),	\
		.offset = _offset,					\
		.con_id = _con_id,					\
		.dev_id = _dev_id,					\
		.flags = _flags						\
	}

#define TEGRA_INIT_DATA(_name, _con_id, _dev_id, _parent_names, _offset,\
			_mux_shift, _mux_width, _mux_flags, _div_shift,	\
			_div_width, _div_frac_width, _div_flags, _regs,	\
			_clk_num, _enb_refcnt, _gate_flags, _clk_id)	\
	TEGRA_INIT_DATA_TABLE(_name, _con_id, _dev_id, _parent_names, _offset,\
			_mux_shift, BIT(_mux_width) - 1, _mux_flags,	\
			_div_shift, _div_width, _div_frac_width, _div_flags, \
			_regs, _clk_num, _enb_refcnt, _gate_flags, _clk_id,\
			NULL, 0)

/* 
                                     
  
                                                               
                                         
                                             
                                  
                                           
                                             
                       
  
         
                                                                           
                                     
 */
struct tegra_clk_super_mux {
	struct clk_hw	hw;
	void __iomem	*reg;
	u8		width;
	u8		flags;
	u8		div2_index;
	u8		pllx_index;
	spinlock_t	*lock;
};

#define to_clk_super_mux(_hw) container_of(_hw, struct tegra_clk_super_mux, hw)

#define TEGRA_DIVIDER_2 BIT(0)

extern const struct clk_ops tegra_clk_super_ops;
struct clk *tegra_clk_register_super_mux(const char *name,
		const char **parent_names, u8 num_parents,
		unsigned long flags, void __iomem *reg, u8 clk_super_flags,
		u8 width, u8 pllx_index, u8 div2_index, spinlock_t *lock);

/* 
                                                     
                                                         
                                                                   
                     
                         
 */
struct tegra_clk_init_table {
	unsigned int	clk_id;
	unsigned int	parent_id;
	unsigned long	rate;
	int		state;
};

/* 
                                          
                                                         
                                                
 */
struct tegra_clk_duplicate {
	int			clk_id;
	struct clk_lookup	lookup;
};

#define TEGRA_CLK_DUPLICATE(_clk_id, _dev, _con) \
	{					\
		.clk_id = _clk_id,		\
		.lookup = {			\
			.dev_id = _dev,		\
			.con_id = _con,		\
		},				\
	}

void tegra_init_from_table(struct tegra_clk_init_table *tbl,
		struct clk *clks[], int clk_max);

void tegra_init_dup_clks(struct tegra_clk_duplicate *dup_list,
		struct clk *clks[], int clk_max);

#ifdef CONFIG_ARCH_TEGRA_2x_SOC
void tegra20_clock_init(struct device_node *np);
#else
static inline void tegra20_clock_init(struct device_node *np) {}
#endif /*                          */

#ifdef CONFIG_ARCH_TEGRA_3x_SOC
void tegra30_clock_init(struct device_node *np);
#else
static inline void tegra30_clock_init(struct device_node *np) {}
#endif /*                          */

#ifdef CONFIG_ARCH_TEGRA_114_SOC
void tegra114_clock_init(struct device_node *np);
#else
static inline void tegra114_clock_init(struct device_node *np) {}
#endif /*                          */

typedef void (*tegra_clk_apply_init_table_func)(void);
extern tegra_clk_apply_init_table_func tegra_clk_apply_init_table;

#endif /*             */
