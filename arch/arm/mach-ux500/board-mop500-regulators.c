/*
 * Copyright (C) ST-Ericsson SA 2010
 *
 * License Terms: GNU General Public License v2
 *
 * Authors: Sundar Iyer <sundar.iyer@stericsson.com>
 *          Bengt Jonsson <bengt.g.jonsson@stericsson.com>
 *          Daniel Willerud <daniel.willerud@stericsson.com>
 *
 * MOP500 board specific initialization for regulators
 */
#include <linux/kernel.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/ab8500.h>
#include "board-mop500-regulators.h"
#include "id.h"

static struct regulator_consumer_supply gpio_en_3v3_consumers[] = {
       REGULATOR_SUPPLY("vdd33a", "smsc911x.0"),
};

struct regulator_init_data gpio_en_3v3_regulator = {
       .constraints = {
               .name = "EN-3V3",
               .min_uV = 3300000,
               .max_uV = 3300000,
               .valid_ops_mask = REGULATOR_CHANGE_STATUS,
       },
       .num_consumer_supplies = ARRAY_SIZE(gpio_en_3v3_consumers),
       .consumer_supplies = gpio_en_3v3_consumers,
};

static struct regulator_consumer_supply sdi0_reg_consumers[] = {
        REGULATOR_SUPPLY("vqmmc", "sdi0"),
};

struct regulator_init_data sdi0_reg_init_data = {
        .constraints = {
                .min_uV         = 1800000,
                .max_uV         = 2900000,
                .valid_ops_mask = REGULATOR_CHANGE_VOLTAGE|REGULATOR_CHANGE_STATUS,
        },
        .num_consumer_supplies  = ARRAY_SIZE(sdi0_reg_consumers),
        .consumer_supplies      = sdi0_reg_consumers,
};

/*
                     
 */
static struct regulator_consumer_supply tps61052_vaudio_consumers[] = {
	/*
                                                                  
                                                                 
                                                                
                                     
  */
	REGULATOR_SUPPLY("vintdclassint", "ab8500-codec.0"),
};

struct regulator_init_data tps61052_regulator = {
	.constraints = {
		.name = "vaudio-hf",
		.min_uV = 4500000,
		.max_uV = 4500000,
		.valid_ops_mask = REGULATOR_CHANGE_STATUS,
	},
	.num_consumer_supplies = ARRAY_SIZE(tps61052_vaudio_consumers),
	.consumer_supplies = tps61052_vaudio_consumers,
};

static struct regulator_consumer_supply ab8500_vaux1_consumers[] = {
	/*                            */
	REGULATOR_SUPPLY("vddi", "mcde_disp_sony_acx424akp.0"),
	/*                                    */
	REGULATOR_SUPPLY("vdd1", "samsung_s6d16d0.0"),
	/*                           */
	REGULATOR_SUPPLY("vdd1", "samsung_s6d16d0.1"),
	/*                          */
	REGULATOR_SUPPLY("vcc", "gpio-keys.0"),
	/*                                */
	REGULATOR_SUPPLY("vcc", "2-0029"),
	/*                         */
	REGULATOR_SUPPLY("vdd", "2-0018"),
	/*                          */
	REGULATOR_SUPPLY("vdd", "2-0019"),
	/*                        */
	REGULATOR_SUPPLY("vdd", "2-001e"),
	/*                                  */
	REGULATOR_SUPPLY("avdd", "3-005c"),
	REGULATOR_SUPPLY("avdd", "3-005d"),
	/*                                   */
	REGULATOR_SUPPLY("vdd", "3-004b"),
	/*                           */
	REGULATOR_SUPPLY("vdd", "2-0068"),
	/*                             */
	REGULATOR_SUPPLY("vdd", "3-0029"),
	/*                        */
	REGULATOR_SUPPLY("vdd", "2-005c"),
	/*                                      */
	REGULATOR_SUPPLY("vcpin", "spi8.0"),
	/*               */
	REGULATOR_SUPPLY("vaux12v5", "mmio_camera"),
};

static struct regulator_consumer_supply ab8500_vaux2_consumers[] = {
	/*                     */
	REGULATOR_SUPPLY("vmmc", "sdi4"),
	/*                    */
	REGULATOR_SUPPLY("vcc-N2158", "ab8500-codec.0"),
	/*                           */
	REGULATOR_SUPPLY("vcc-N2158", "ab8500-acc-det.0"),
	/*                      */
	REGULATOR_SUPPLY("vcc-N2158", "mcde_tv_ab8500.4"),
	/*                    */
	REGULATOR_SUPPLY("vcc-N2158", "av8100_hdmi.3"),
};

static struct regulator_consumer_supply ab8500_vaux3_consumers[] = {
	REGULATOR_SUPPLY("v-SD-STM", "stm"),
	/*                         */
	REGULATOR_SUPPLY("vmmc", "sdi0"),
};

static struct regulator_consumer_supply ab8505_vaux4_consumers[] = {
};

static struct regulator_consumer_supply ab8505_vaux5_consumers[] = {
};

static struct regulator_consumer_supply ab8505_vaux6_consumers[] = {
};

static struct regulator_consumer_supply ab8505_vaux8_consumers[] = {
	/*                           */
	REGULATOR_SUPPLY("v-aux8", NULL),
};

static struct regulator_consumer_supply ab8505_vadc_consumers[] = {
	/*                              */
	REGULATOR_SUPPLY("vddadc", "ab8500-gpadc.0"),
	/*                 */
	REGULATOR_SUPPLY("vddadc", "ab8500-charger.0"),
};

static struct regulator_consumer_supply ab8500_vtvout_consumers[] = {
	/*                    */
	REGULATOR_SUPPLY("vtvout", "ab8500-denc.0"),
	/*                              */
	REGULATOR_SUPPLY("vddadc", "ab8500-gpadc.0"),
	/*                 */
	REGULATOR_SUPPLY("vddadc", "ab8500-charger.0"),
	/*                      */
	REGULATOR_SUPPLY("vtvout", "mcde_tv_ab8500.4"),
};

static struct regulator_consumer_supply ab8500_vaud_consumers[] = {
	/*                                */
	REGULATOR_SUPPLY("vaud", "ab8500-codec.0"),
};

static struct regulator_consumer_supply ab8500_vamic1_consumers[] = {
	/*                                */
	REGULATOR_SUPPLY("vamic1", "ab8500-codec.0"),
};

static struct regulator_consumer_supply ab8500_vamic2_consumers[] = {
	/*                                */
	REGULATOR_SUPPLY("vamic2", "ab8500-codec.0"),
};

static struct regulator_consumer_supply ab8500_vdmic_consumers[] = {
	/*                                */
	REGULATOR_SUPPLY("vdmic", "ab8500-codec.0"),
};

static struct regulator_consumer_supply ab8500_vintcore_consumers[] = {
	/*                            */
	REGULATOR_SUPPLY("v-intcore", NULL),
	/*                 */
	REGULATOR_SUPPLY("vddulpivio18", "ab8500-usb.0"),
	/*                              */
	REGULATOR_SUPPLY("v-intcore", "abx500-clk.0"),
};

static struct regulator_consumer_supply ab8505_usb_consumers[] = {
	/*                               */
	REGULATOR_SUPPLY("v-ape", NULL),
};

static struct regulator_consumer_supply ab8500_vana_consumers[] = {
	/*            */
	REGULATOR_SUPPLY("vdddsi1v2", "mcde"),
	REGULATOR_SUPPLY("vdddsi1v2", "b2r2_core"),
	REGULATOR_SUPPLY("vdddsi1v2", "b2r2_1_core"),
	REGULATOR_SUPPLY("vdddsi1v2", "dsilink.0"),
	REGULATOR_SUPPLY("vdddsi1v2", "dsilink.1"),
	REGULATOR_SUPPLY("vdddsi1v2", "dsilink.2"),
	/*            */
	REGULATOR_SUPPLY("vddcsi1v2", "mmio_camera"),
};

/*                                          */
static struct ab8500_regulator_reg_init ab8500_reg_init[] = {
	/*
                                                           
                                                           
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUREQUESTCTRL2,       0xf0, 0x00),
	/*
                                                           
                                                           
                                                           
                                                           
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUREQUESTCTRL3,       0xff, 0x00),
	/*
                                                           
                                                               
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUREQUESTCTRL4,       0x07, 0x00),
	/*
                                       
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUSYSCLKREQ1HPVALID1, 0xe8, 0x00),
	/*
                                           
                                           
                                                        
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUSYSCLKREQ1HPVALID2, 0x70, 0x40),
	/*
                                       
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUHWHPREQ1VALID1,     0xe8, 0x00),
	/*
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUHWHPREQ1VALID2,     0x07, 0x00),
	/*
                                       
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUHWHPREQ2VALID1,     0xe8, 0x00),
	/*
                                       
                                       
                                                
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUHWHPREQ2VALID2,     0x07, 0x04),
	/*
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUSWHPREQVALID1,      0xa0, 0x00),
	/*
                                       
                                       
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUSWHPREQVALID2,      0x1f, 0x00),
	/*
                                                    
                                       
                                                    
                                       
                                                    
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUSYSCLKREQVALID1,    0xfe, 0x2a),
	/*
                                       
                                       
                                       
                                       
                                                    
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUSYSCLKREQVALID2,    0xfe, 0x20),
	/*
                                       
                                       
                                     
                                            
                                            
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUMISC1,              0xfe, 0x10),
	/*
                                       
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8500_VAUDIOSUPPLY,           0x1e, 0x00),
	/*
                                                             
                                                             
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUCTRL1VAMIC,         0x03, 0x00),
	/*
                                                               
                                        
  */
	INIT_REGULATOR_REGISTER(AB8500_VPLLVANAREGU,           0x0f, 0x02),
	/*
                                       
                                                     
  */
	INIT_REGULATOR_REGISTER(AB8500_VREFDDR,                0x03, 0x00),
	/*
                                       
                                        
                                                                
                                                                     
                                                                    
  */
	INIT_REGULATOR_REGISTER(AB8500_EXTSUPPLYREGU,          0xff, 0x13),
	/*
                                       
                                        
  */
	INIT_REGULATOR_REGISTER(AB8500_VAUX12REGU,             0x0f, 0x01),
	/*
                                        
  */
	INIT_REGULATOR_REGISTER(AB8500_VRF1VAUX3REGU,          0x03, 0x00),
	/*
                                    
  */
	INIT_REGULATOR_REGISTER(AB8500_VAUX1SEL,               0x0f, 0x0C),
	/*
                                    
  */
	INIT_REGULATOR_REGISTER(AB8500_VAUX2SEL,               0x0f, 0x0d),
	/*
                                     
  */
	INIT_REGULATOR_REGISTER(AB8500_VRF1VAUX3SEL,           0x07, 0x07),
	/*
                                               
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUCTRL2SPARE,         0x01, 0x00),
	/*
                                                   
                                                   
                                                   
                                                   
                                                   
                                                   
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUCTRLDISCH,          0xfc, 0x00),
	/*
                                                   
                                                                       
                                                   
  */
	INIT_REGULATOR_REGISTER(AB8500_REGUCTRLDISCH2,         0x16, 0x00),
};

/*                   */
static struct regulator_init_data ab8500_regulators[AB8500_NUM_REGULATORS] = {
	/*                                */
	[AB8500_LDO_AUX1] = {
		.supply_regulator = "ab8500-ext-supply3",
		.constraints = {
			.name = "V-DISPLAY",
			.min_uV = 2800000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS,
			.boot_on = 1, /*                       */
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vaux1_consumers),
		.consumer_supplies = ab8500_vaux1_consumers,
	},
	/*                               */
	[AB8500_LDO_AUX2] = {
		.supply_regulator = "ab8500-ext-supply3",
		.constraints = {
			.name = "V-eMMC1",
			.min_uV = 1100000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vaux2_consumers),
		.consumer_supplies = ab8500_vaux2_consumers,
	},
	/*                                            */
	[AB8500_LDO_AUX3] = {
		.supply_regulator = "ab8500-ext-supply3",
		.constraints = {
			.name = "V-MMC-SD",
			.min_uV = 1100000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vaux3_consumers),
		.consumer_supplies = ab8500_vaux3_consumers,
	},
	/*                                    */
	[AB8500_LDO_TVOUT] = {
		.constraints = {
			.name = "V-TVOUT",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vtvout_consumers),
		.consumer_supplies = ab8500_vtvout_consumers,
	},
	/*                                      */
	[AB8500_LDO_AUDIO] = {
		.constraints = {
			.name = "V-AUD",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vaud_consumers),
		.consumer_supplies = ab8500_vaud_consumers,
	},
	/*                                 */
	[AB8500_LDO_ANAMIC1] = {
		.constraints = {
			.name = "V-AMIC1",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vamic1_consumers),
		.consumer_supplies = ab8500_vamic1_consumers,
	},
	/*                                                           */
	[AB8500_LDO_ANAMIC2] = {
		.constraints = {
			.name = "V-AMIC2",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vamic2_consumers),
		.consumer_supplies = ab8500_vamic2_consumers,
	},
	/*                              */
	[AB8500_LDO_DMIC] = {
		.constraints = {
			.name = "V-DMIC",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vdmic_consumers),
		.consumer_supplies = ab8500_vdmic_consumers,
	},
	/*                                        */
	[AB8500_LDO_INTCORE] = {
		.constraints = {
			.name = "V-INTCORE",
			.min_uV = 1250000,
			.max_uV = 1350000,
			.input_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE |
					  REGULATOR_CHANGE_DRMS,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vintcore_consumers),
		.consumer_supplies = ab8500_vintcore_consumers,
	},
	/*                                    */
	[AB8500_LDO_ANA] = {
		.constraints = {
			.name = "V-CSI-DSI",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vana_consumers),
		.consumer_supplies = ab8500_vana_consumers,
	},
};

/*                        */
static struct regulator_consumer_supply ab8500_ext_supply3_consumers[] = {
	/*                              */
	REGULATOR_SUPPLY("vinvsim", "sim-detect.0"),
};

/*                                                                        */
static struct ab8500_ext_regulator_cfg ab8500_ext_supply2 = {
	.hwreq = true,
};

/*
                             
 */
static struct regulator_init_data ab8500_ext_regulators[] = {
	/*                                    */
	[AB8500_EXT_SUPPLY1] = {
		.constraints = {
			.name = "ab8500-ext-supply1",
			.min_uV = 1800000,
			.max_uV = 1800000,
			.initial_mode = REGULATOR_MODE_IDLE,
			.boot_on = 1,
			.always_on = 1,
		},
	},
	/*                                                         */
	[AB8500_EXT_SUPPLY2] = {
		.constraints = {
			.name = "ab8500-ext-supply2",
			.min_uV = 1360000,
			.max_uV = 1360000,
		},
	},
	/*                                                       */
	[AB8500_EXT_SUPPLY3] = {
		.constraints = {
			.name = "ab8500-ext-supply3",
			.min_uV = 3400000,
			.max_uV = 3400000,
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
			.boot_on = 1,
		},
		.num_consumer_supplies =
			ARRAY_SIZE(ab8500_ext_supply3_consumers),
		.consumer_supplies = ab8500_ext_supply3_consumers,
	},
};

/*                                          */
static struct ab8500_regulator_reg_init ab8505_reg_init[] = {
	/*
                   
                     
                     
                     
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUREQUESTCTRL1,       0x00, 0x00),
	/*
                    
                   
                                                           
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUREQUESTCTRL2,       0x30, 0x00),
	/*
                                                           
                                                           
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUREQUESTCTRL3,       0xf0, 0x00),
	/*
                                                           
                                                               
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUREQUESTCTRL4,       0x07, 0x00),
	/*
                           
                           
                          
                                       
                         
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUSYSCLKREQ1HPVALID1, 0xe8, 0x00),
	/*
                           
                         
                        
                           
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUSYSCLKREQ1HPVALID2, 0x00, 0x00),
	/*
                       
                       
                      
                                       
                     
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUHWHPREQ1VALID1,     0xe8, 0x00),
	/*
                       
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUHWHPREQ1VALID2,     0x00, 0x00),
	/*
                       
                       
                      
                                       
                     
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUHWHPREQ2VALID1,     0xe8, 0x00),
	/*
                       
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUHWHPREQ2VALID2,     0x00, 0x00),
	/* 
                      
                    
                      
                      
                     
                    
                                       
                    
                                       
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUSWHPREQVALID1,      0xa0, 0x00),
	/*
                                       
                                       
                      
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUSWHPREQVALID2,      0x03, 0x00),
	/*
                                                    
                                       
                                                    
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUSYSCLKREQVALID1,    0x0e, 0x0a),
	/*
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUSYSCLKREQVALID2,    0x0e, 0x00),
	/*
                     
                      
                      
                          
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUVAUX4REQVALID,    0x00, 0x00),
	/*
                                       
                                       
                                     
                                            
                                            
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUMISC1,              0xfe, 0x10),
	/*
                                       
                                       
                                       
                                       
  */
	INIT_REGULATOR_REGISTER(AB8505_VAUDIOSUPPLY,           0x1e, 0x00),
	/*
                                                             
                                                             
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUCTRL1VAMIC,         0x03, 0x00),
	/*
              
                 
                  
                 
  */
	INIT_REGULATOR_REGISTER(AB8505_VSMPSAREGU,    0x00, 0x00),
	/*
              
                 
                  
                 
  */
	INIT_REGULATOR_REGISTER(AB8505_VSMPSBREGU,    0x00, 0x00),
	/*
             
                
                 
                
  */
	INIT_REGULATOR_REGISTER(AB8505_VSAFEREGU,    0x00, 0x00),
	/*
                                                               
                                        
  */
	INIT_REGULATOR_REGISTER(AB8505_VPLLVANAREGU,           0x0f, 0x02),
	/*
                                                                       
                                                                       
                                                                      
                                                                     
                                                                    
  */
	INIT_REGULATOR_REGISTER(AB8505_EXTSUPPLYREGU,          0xff, 0x30),
	/*
                                       
                                        
  */
	INIT_REGULATOR_REGISTER(AB8505_VAUX12REGU,             0x0f, 0x01),
	/*
                                        
  */
	INIT_REGULATOR_REGISTER(AB8505_VRF1VAUX3REGU,          0x03, 0x00),
	/*
              
  */
	INIT_REGULATOR_REGISTER(AB8505_VSMPSASEL1,    0x00, 0x00),
	/*
              
  */
	INIT_REGULATOR_REGISTER(AB8505_VSMPSASEL2,    0x00, 0x00),
	/*
              
  */
	INIT_REGULATOR_REGISTER(AB8505_VSMPSASEL3,    0x00, 0x00),
	/*
              
  */
	INIT_REGULATOR_REGISTER(AB8505_VSMPSBSEL1,    0x00, 0x00),
	/*
              
  */
	INIT_REGULATOR_REGISTER(AB8505_VSMPSBSEL2,    0x00, 0x00),
	/*
              
  */
	INIT_REGULATOR_REGISTER(AB8505_VSMPSBSEL3,    0x00, 0x00),
	/*
             
  */
	INIT_REGULATOR_REGISTER(AB8505_VSAFESEL1,    0x00, 0x00),
	/*
             
  */
	INIT_REGULATOR_REGISTER(AB8505_VSAFESEL2,    0x00, 0x00),
	/*
             
  */
	INIT_REGULATOR_REGISTER(AB8505_VSAFESEL3,    0x00, 0x00),
	/*
                                    
  */
	INIT_REGULATOR_REGISTER(AB8505_VAUX1SEL,               0x0f, 0x0C),
	/*
                                    
  */
	INIT_REGULATOR_REGISTER(AB8505_VAUX2SEL,               0x0f, 0x0d),
	/*
                                     
  */
	INIT_REGULATOR_REGISTER(AB8505_VRF1VAUX3SEL,           0x07, 0x07),
	/*
                    
  */
	INIT_REGULATOR_REGISTER(AB8505_VAUX4REQCTRL,    0x00, 0x00),
	/*
             
  */
	INIT_REGULATOR_REGISTER(AB8505_VAUX4REGU,    0x00, 0x00),
	/*
            
  */
	INIT_REGULATOR_REGISTER(AB8505_VAUX4SEL,    0x00, 0x00),
	/*
                                                   
                                                   
                                                   
                                                   
                                                   
                                                   
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUCTRLDISCH,          0xfc, 0x00),
	/*
                                                   
                                                                       
                                                   
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUCTRLDISCH2,         0x16, 0x00),
	/*
                                                   
  */
	INIT_REGULATOR_REGISTER(AB8505_REGUCTRLDISCH3,         0x01, 0x00),
	/*
            
           
            
              
                
                 
  */
	INIT_REGULATOR_REGISTER(AB8505_CTRLVAUX5,              0x00, 0x00),
	/*
            
           
            
                 
  */
	INIT_REGULATOR_REGISTER(AB8505_CTRLVAUX6,              0x00, 0x00),
};

struct regulator_init_data ab8505_regulators[AB8505_NUM_REGULATORS] = {
	/*                                */
	[AB8505_LDO_AUX1] = {
		.constraints = {
			.name = "V-DISPLAY",
			.min_uV = 2800000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS,
			.boot_on = 1, /*                       */
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vaux1_consumers),
		.consumer_supplies = ab8500_vaux1_consumers,
	},
	/*                               */
	[AB8505_LDO_AUX2] = {
		.constraints = {
			.name = "V-eMMC1",
			.min_uV = 1100000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vaux2_consumers),
		.consumer_supplies = ab8500_vaux2_consumers,
	},
	/*                                            */
	[AB8505_LDO_AUX3] = {
		.constraints = {
			.name = "V-MMC-SD",
			.min_uV = 1100000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vaux3_consumers),
		.consumer_supplies = ab8500_vaux3_consumers,
	},
	/*                                                                 */
	[AB8505_LDO_AUX4] = {
		.constraints = {
			.name = "V-NFC-SE",
			.min_uV = 1100000,
			.max_uV = 3300000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8505_vaux4_consumers),
		.consumer_supplies = ab8505_vaux4_consumers,
	},
	/*                                   */
	[AB8505_LDO_AUX5] = {
		.constraints = {
			.name = "V-AUX5",
			.min_uV = 1050000,
			.max_uV = 2790000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8505_vaux5_consumers),
		.consumer_supplies = ab8505_vaux5_consumers,
	},
	/*                                   */
	[AB8505_LDO_AUX6] = {
		.constraints = {
			.name = "V-AUX6",
			.min_uV = 1050000,
			.max_uV = 2790000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8505_vaux6_consumers),
		.consumer_supplies = ab8505_vaux6_consumers,
	},
	/*                           */
	[AB8505_LDO_ADC] = {
		.constraints = {
			.name = "V-ADC",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8505_vadc_consumers),
		.consumer_supplies = ab8505_vadc_consumers,
	},
	/*                                      */
	[AB8505_LDO_AUDIO] = {
		.constraints = {
			.name = "V-AUD",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vaud_consumers),
		.consumer_supplies = ab8500_vaud_consumers,
	},
	/*                                 */
	[AB8505_LDO_ANAMIC1] = {
		.constraints = {
			.name = "V-AMIC1",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vamic1_consumers),
		.consumer_supplies = ab8500_vamic1_consumers,
	},
	/*                                                           */
	[AB8505_LDO_ANAMIC2] = {
		.constraints = {
			.name = "V-AMIC2",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vamic2_consumers),
		.consumer_supplies = ab8500_vamic2_consumers,
	},
	/*                              */
	[AB8505_LDO_AUX8] = {
		.constraints = {
			.name = "V-AUX8",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8505_vaux8_consumers),
		.consumer_supplies = ab8505_vaux8_consumers,
	},
	/*                                        */
	[AB8505_LDO_INTCORE] = {
		.constraints = {
			.name = "V-INTCORE",
			.min_uV = 1250000,
			.max_uV = 1350000,
			.input_uV = 1800000,
			.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE |
					  REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE |
					  REGULATOR_CHANGE_DRMS,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vintcore_consumers),
		.consumer_supplies = ab8500_vintcore_consumers,
	},
	/*                    */
	[AB8505_LDO_USB] = {
		.constraints = {
			.name = "V-USB",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS |
					  REGULATOR_CHANGE_MODE,
			.valid_modes_mask = REGULATOR_MODE_NORMAL |
					    REGULATOR_MODE_IDLE,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8505_usb_consumers),
		.consumer_supplies = ab8505_usb_consumers,
	},
	/*                                    */
	[AB8505_LDO_ANA] = {
		.constraints = {
			.name = "V-CSI-DSI",
			.valid_ops_mask = REGULATOR_CHANGE_STATUS,
		},
		.num_consumer_supplies = ARRAY_SIZE(ab8500_vana_consumers),
		.consumer_supplies = ab8500_vana_consumers,
	},
};

struct ab8500_regulator_platform_data ab8500_regulator_plat_data = {
	.reg_init               = ab8500_reg_init,
	.num_reg_init           = ARRAY_SIZE(ab8500_reg_init),
	.regulator              = ab8500_regulators,
	.num_regulator          = ARRAY_SIZE(ab8500_regulators),
	.ext_regulator          = ab8500_ext_regulators,
	.num_ext_regulator      = ARRAY_SIZE(ab8500_ext_regulators),
};

/*                                                                        */
struct ab8500_regulator_platform_data ab8505_regulator_plat_data = {
	.reg_init               = ab8505_reg_init,
	.num_reg_init           = ARRAY_SIZE(ab8505_reg_init),
	.regulator              = ab8505_regulators,
	.num_regulator          = ARRAY_SIZE(ab8505_regulators),
};

static void ab8500_modify_reg_init(int id, u8 mask, u8 value)
{
	int i;

	if (cpu_is_u8520()) {
		for (i = ARRAY_SIZE(ab8505_reg_init) - 1; i >= 0; i--) {
			if (ab8505_reg_init[i].id == id) {
				u8 initval = ab8505_reg_init[i].value;
				initval = (initval & ~mask) | (value & mask);
				ab8505_reg_init[i].value = initval;

				BUG_ON(mask & ~ab8505_reg_init[i].mask);
				return;
			}
		}
	} else {
		for (i = ARRAY_SIZE(ab8500_reg_init) - 1; i >= 0; i--) {
			if (ab8500_reg_init[i].id == id) {
				u8 initval = ab8500_reg_init[i].value;
				initval = (initval & ~mask) | (value & mask);
				ab8500_reg_init[i].value = initval;

				BUG_ON(mask & ~ab8500_reg_init[i].mask);
				return;
			}
		}
	}

	BUG_ON(1);
}

void mop500_regulator_init(void)
{
	struct regulator_init_data *regulator;

	/*
                                             
  */
	if (cpu_is_u8520()) {
		/*                            */
		ab8500_modify_reg_init(AB8505_VAUX12REGU, 0x0f, 0x05);
	}

	/*
                                                                
                         
  */
	if (cpu_is_u8500v20()) {
		/*                                                         */
		ab8500_modify_reg_init(AB8500_REGUREQUESTCTRL3, 0x01, 0x01);

		/*                                                      */
		ab8500_modify_reg_init(AB8500_REGUSYSCLKREQ1HPVALID2,
			0x20, 0x20);

		/*                                          */
		ab8500_modify_reg_init(AB8500_EXTSUPPLYREGU, 0x0c, 0x04);

		/*                                           */
		regulator = &ab8500_ext_regulators[AB8500_EXT_SUPPLY2];
		regulator->constraints.always_on = 1;

		/*                                */
		regulator = &ab8500_ext_regulators[AB8500_EXT_SUPPLY2];
		regulator->constraints.state_mem.disabled = 1;
		regulator->constraints.state_standby.disabled = 1;

		/*                                                 */
		regulator->driver_data = (void *)&ab8500_ext_supply2;
	}
}
