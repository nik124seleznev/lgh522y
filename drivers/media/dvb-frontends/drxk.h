#ifndef _DRXK_H_
#define _DRXK_H_

#include <linux/kconfig.h>
#include <linux/types.h>
#include <linux/i2c.h>

/* 
                                                                  
  
                                  
                                                             
                       
                                                              
                                      
                                                              
                                                      
                                                                 
                                                      
                                                                      
                                                          
                                                                 
                                                                
                                                                            
                                            
                                                   
                                                
                                         
                                                    
                                              
                             
                                                                   
  
                                                                  
         
 */
struct drxk_config {
	u8	adr;
	bool	single_master;
	bool	no_i2c_bridge;
	bool	parallel_ts;
	bool	dynamic_clk;
	bool	enable_merr_cfg;
	bool	load_firmware_sync;

	bool	antenna_dvbt;
	u16	antenna_gpio;

	u8	mpeg_out_clk_strength;
	int	chunk_size;

	const char	*microcode_name;
	int		 qam_demod_parameter_count;
};

#if IS_ENABLED(CONFIG_DVB_DRXK)
extern struct dvb_frontend *drxk_attach(const struct drxk_config *config,
					struct i2c_adapter *i2c);
#else
static inline struct dvb_frontend *drxk_attach(const struct drxk_config *config,
					struct i2c_adapter *i2c)
{
        printk(KERN_WARNING "%s: driver disabled by Kconfig\n", __func__);
        return NULL;
}
#endif

#endif
