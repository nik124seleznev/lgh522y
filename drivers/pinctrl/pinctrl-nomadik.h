#ifndef PINCTRL_PINCTRL_NOMADIK_H
#define PINCTRL_PINCTRL_NOMADIK_H

#include <linux/platform_data/pinctrl-nomadik.h>

/*                     */
#define PINCTRL_NMK_STN8815	0
#define PINCTRL_NMK_DB8500	1
#define PINCTRL_NMK_DB8540	2

#define PRCM_GPIOCR_ALTCX(pin_num,\
	altc1_used, altc1_ri, altc1_cb,\
	altc2_used, altc2_ri, altc2_cb,\
	altc3_used, altc3_ri, altc3_cb,\
	altc4_used, altc4_ri, altc4_cb)\
{\
	.pin = pin_num,\
	.altcx[PRCM_IDX_GPIOCR_ALTC1] = {\
		.used = altc1_used,\
		.reg_index = altc1_ri,\
		.control_bit = altc1_cb\
	},\
	.altcx[PRCM_IDX_GPIOCR_ALTC2] = {\
		.used = altc2_used,\
		.reg_index = altc2_ri,\
		.control_bit = altc2_cb\
	},\
	.altcx[PRCM_IDX_GPIOCR_ALTC3] = {\
		.used = altc3_used,\
		.reg_index = altc3_ri,\
		.control_bit = altc3_cb\
	},\
	.altcx[PRCM_IDX_GPIOCR_ALTC4] = {\
		.used = altc4_used,\
		.reg_index = altc4_ri,\
		.control_bit = altc4_cb\
	},\
}

/* 
                             
                                                     
 */
enum prcm_gpiocr_reg_index {
	PRCM_IDX_GPIOCR1,
	PRCM_IDX_GPIOCR2,
	PRCM_IDX_GPIOCR3
};
/* 
                               
                                                   
 */
enum prcm_gpiocr_altcx_index {
	PRCM_IDX_GPIOCR_ALTC1,
	PRCM_IDX_GPIOCR_ALTC2,
	PRCM_IDX_GPIOCR_ALTC3,
	PRCM_IDX_GPIOCR_ALTC4,
	PRCM_IDX_GPIOCR_ALTC_MAX,
};

/* 
                                                      
                                                 
                                                                      
                                                             
 */
struct prcm_gpiocr_altcx {
	bool used:1;
	u8 reg_index:2;
	u8 control_bit:5;
} __packed;

/* 
                                                          
                       
                                                    
 */
struct prcm_gpiocr_altcx_pin_desc {
	unsigned short pin;
	struct prcm_gpiocr_altcx altcx[PRCM_IDX_GPIOCR_ALTC_MAX];
};

/* 
                                                     
                                                             
                                                                 
                                              
 */
struct nmk_function {
	const char *name;
	const char * const *groups;
	unsigned ngroups;
};

/* 
                                                      
                                             
                                                                      
                                              
                                                                        
                                                      
                                                                    
                                          
 */
struct nmk_pingroup {
	const char *name;
	const unsigned int *pins;
	const unsigned npins;
	int altsetting;
};

/* 
                                                                             
                                                     
                                                           
                                                                  
                                                                 
                                                               
              
                                          
                                                   
                                                   
                                                                    
                                              
                                                                            
                                                     
                                                                         
 */
struct nmk_pinctrl_soc_data {
	struct pinctrl_gpio_range *gpio_ranges;
	unsigned gpio_num_ranges;
	const struct pinctrl_pin_desc *pins;
	unsigned npins;
	const struct nmk_function *functions;
	unsigned nfunctions;
	const struct nmk_pingroup *groups;
	unsigned ngroups;
	const struct prcm_gpiocr_altcx_pin_desc *altcx_pins;
	unsigned npins_altcx;
	const u16 *prcm_gpiocr_registers;
};

#ifdef CONFIG_PINCTRL_STN8815

void nmk_pinctrl_stn8815_init(const struct nmk_pinctrl_soc_data **soc);

#else

static inline void
nmk_pinctrl_stn8815_init(const struct nmk_pinctrl_soc_data **soc)
{
}

#endif

#ifdef CONFIG_PINCTRL_DB8500

void nmk_pinctrl_db8500_init(const struct nmk_pinctrl_soc_data **soc);

#else

static inline void
nmk_pinctrl_db8500_init(const struct nmk_pinctrl_soc_data **soc)
{
}

#endif

#ifdef CONFIG_PINCTRL_DB8540

void nmk_pinctrl_db8540_init(const struct nmk_pinctrl_soc_data **soc);

#else

static inline void
nmk_pinctrl_db8540_init(const struct nmk_pinctrl_soc_data **soc)
{
}

#endif

#endif /*                           */
