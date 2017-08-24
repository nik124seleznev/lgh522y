/*
 * core.h
 *
 * copyright (c) 2011 Samsung Electronics Co., Ltd
 *              http://www.samsung.com
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 */

#ifndef __LINUX_MFD_SEC_CORE_H
#define __LINUX_MFD_SEC_CORE_H

#define NUM_IRQ_REGS	4

enum sec_device_type {
	S5M8751X,
	S5M8763X,
	S5M8767X,
	S2MPS11X,
};

/* 
                                                              
                                                                        
                                                                      
                                              
                                        
                                           
                              
                                                             
                                       
                                           
                                         
                                          
                                                  
 */
struct sec_pmic_dev {
	struct device *dev;
	struct sec_platform_data *pdata;
	struct regmap *regmap;
	struct i2c_client *i2c;
	struct i2c_client *rtc;
	struct mutex iolock;
	struct mutex irqlock;

	int device_type;
	int irq_base;
	int irq;
	struct regmap_irq_chip_data *irq_data;

	int ono;
	u8 irq_masks_cur[NUM_IRQ_REGS];
	u8 irq_masks_cache[NUM_IRQ_REGS];
	int type;
	bool wakeup;
};

int sec_irq_init(struct sec_pmic_dev *sec_pmic);
void sec_irq_exit(struct sec_pmic_dev *sec_pmic);
int sec_irq_resume(struct sec_pmic_dev *sec_pmic);

extern int sec_reg_read(struct sec_pmic_dev *sec_pmic, u8 reg, void *dest);
extern int sec_bulk_read(struct sec_pmic_dev *sec_pmic, u8 reg, int count, u8 *buf);
extern int sec_reg_write(struct sec_pmic_dev *sec_pmic, u8 reg, u8 value);
extern int sec_bulk_write(struct sec_pmic_dev *sec_pmic, u8 reg, int count, u8 *buf);
extern int sec_reg_update(struct sec_pmic_dev *sec_pmic, u8 reg, u8 val, u8 mask);

struct sec_platform_data {
	struct sec_regulator_data	*regulators;
	struct sec_opmode_data		*opmode;
	int				device_type;
	int				num_regulators;

	int				irq_base;
	int				(*cfg_pmic_irq)(void);

	int				ono;
	bool				wakeup;
	bool				buck_voltage_lock;

	int				buck_gpios[3];
	int				buck_ds[3];
	unsigned int			buck2_voltage[8];
	bool				buck2_gpiodvs;
	unsigned int			buck3_voltage[8];
	bool				buck3_gpiodvs;
	unsigned int			buck4_voltage[8];
	bool				buck4_gpiodvs;

	int				buck_set1;
	int				buck_set2;
	int				buck_set3;
	int				buck2_enable;
	int				buck3_enable;
	int				buck4_enable;
	int				buck_default_idx;
	int				buck2_default_idx;
	int				buck3_default_idx;
	int				buck4_default_idx;

	int                             buck_ramp_delay;

	int				buck2_ramp_delay;
	int				buck34_ramp_delay;
	int				buck5_ramp_delay;
	int				buck16_ramp_delay;
	int				buck7810_ramp_delay;
	int				buck9_ramp_delay;

	bool                            buck2_ramp_enable;
	bool                            buck3_ramp_enable;
	bool                            buck4_ramp_enable;
	bool				buck6_ramp_enable;

	int				buck2_init;
	int				buck3_init;
	int				buck4_init;
};

/* 
                                      
                    
                                                             
 */
struct sec_regulator_data {
	int				id;
	struct regulator_init_data	*initdata;
	struct device_node *reg_node;
};

/*
                                                  
                    
                                  
 */
struct sec_opmode_data {
	int id;
	unsigned int mode;
};

/*
                                   
                                      
                                    
                                                         
                                                         
                                      
                                      
 */

enum sec_opmode {
	SEC_OPMODE_OFF,
	SEC_OPMODE_ON,
	SEC_OPMODE_LOWPOWER,
	SEC_OPMODE_SUSPEND,
};

#endif /*                         */
