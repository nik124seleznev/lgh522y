/*
 * ethtool.h: Defines for Linux ethtool.
 *
 * Copyright (C) 1998 David S. Miller (davem@redhat.com)
 * Copyright 2001 Jeff Garzik <jgarzik@pobox.com>
 * Portions Copyright 2001 Sun Microsystems (thockin@sun.com)
 * Portions Copyright 2002 Intel (eli.kupermann@intel.com,
 *                                christopher.leech@intel.com,
 *                                scott.feldman@intel.com)
 * Portions Copyright (C) Sun Microsystems 2008
 */
#ifndef _LINUX_ETHTOOL_H
#define _LINUX_ETHTOOL_H

#include <linux/compat.h>
#include <uapi/linux/ethtool.h>

#ifdef CONFIG_COMPAT

struct compat_ethtool_rx_flow_spec {
	u32		flow_type;
	union ethtool_flow_union h_u;
	struct ethtool_flow_ext h_ext;
	union ethtool_flow_union m_u;
	struct ethtool_flow_ext m_ext;
	compat_u64	ring_cookie;
	u32		location;
};

struct compat_ethtool_rxnfc {
	u32				cmd;
	u32				flow_type;
	compat_u64			data;
	struct compat_ethtool_rx_flow_spec fs;
	u32				rule_cnt;
	u32				rule_locs[0];
};

#endif /*               */

#include <linux/rculist.h>

extern int __ethtool_get_settings(struct net_device *dev,
				  struct ethtool_cmd *cmd);

/* 
                                                                           
                                                                    
                                                                
                                                                       
                    
                                                                         
                    
 */
enum ethtool_phys_id_state {
	ETHTOOL_ID_INACTIVE,
	ETHTOOL_ID_ACTIVE,
	ETHTOOL_ID_ON,
	ETHTOOL_ID_OFF
};

struct net_device;

/*                                                           */
u32 ethtool_op_get_link(struct net_device *dev);
int ethtool_op_get_ts_info(struct net_device *dev, struct ethtool_ts_info *eti);

/* 
                                                                              
                                                  
                                         
  
                                                                          
 */
static inline u32 ethtool_rxfh_indir_default(u32 index, u32 n_rx_rings)
{
	return index % n_rx_rings;
}

/* 
                                                  
                                                                     
                                                                
                                                                  
        
                                                                     
                                                    
                                                                       
                                                               
                                                                  
                                           
                                                          
                                  
                                                  
                                                                       
           
                                                                        
                                                            
                                          
                                                                       
           
                                                                         
                                                                     
                                 
                                                 
                                                                    
                                                                   
                                                               
                            
                                                
                                                                     
                                                                      
           
                                                                          
                      
                                                                          
                      
                                    
                                                                          
                                           
                                                                        
           
                                       
                                                                            
                                                                       
                                                               
                                                                     
                                                                         
                                                                        
                                                                  
                                                                      
                                                                 
                                                                         
                                                                      
                                                                     
                               
                                                                   
                                                             
                                         
                                                                       
                               
                                                                    
                                                              
                                                         
                                                                          
                      
                                                                       
                                                                    
                      
                                                                    
                      
                                                                  
                                         
                                                                   
                                                            
                      
                                                                            
                                                          
                                                                           
                                                           
                                         
                                                                           
                                                           
                                         
                                         
                                                                           
        
                                                                         
                               
                                 
                                                    
                                                                           
                                                                         
                            
                                                                         
                    
                                                                         
                                                             
                                                                   
  
                                                                    
                                                                   
                      
  
                                                                         
  
                                                                      
                                            
 */
struct ethtool_ops {
	int	(*get_settings)(struct net_device *, struct ethtool_cmd *);
	int	(*set_settings)(struct net_device *, struct ethtool_cmd *);
	void	(*get_drvinfo)(struct net_device *, struct ethtool_drvinfo *);
	int	(*get_regs_len)(struct net_device *);
	void	(*get_regs)(struct net_device *, struct ethtool_regs *, void *);
	void	(*get_wol)(struct net_device *, struct ethtool_wolinfo *);
	int	(*set_wol)(struct net_device *, struct ethtool_wolinfo *);
	u32	(*get_msglevel)(struct net_device *);
	void	(*set_msglevel)(struct net_device *, u32);
	int	(*nway_reset)(struct net_device *);
	u32	(*get_link)(struct net_device *);
	int	(*get_eeprom_len)(struct net_device *);
	int	(*get_eeprom)(struct net_device *,
			      struct ethtool_eeprom *, u8 *);
	int	(*set_eeprom)(struct net_device *,
			      struct ethtool_eeprom *, u8 *);
	int	(*get_coalesce)(struct net_device *, struct ethtool_coalesce *);
	int	(*set_coalesce)(struct net_device *, struct ethtool_coalesce *);
	void	(*get_ringparam)(struct net_device *,
				 struct ethtool_ringparam *);
	int	(*set_ringparam)(struct net_device *,
				 struct ethtool_ringparam *);
	void	(*get_pauseparam)(struct net_device *,
				  struct ethtool_pauseparam*);
	int	(*set_pauseparam)(struct net_device *,
				  struct ethtool_pauseparam*);
	void	(*self_test)(struct net_device *, struct ethtool_test *, u64 *);
	void	(*get_strings)(struct net_device *, u32 stringset, u8 *);
	int	(*set_phys_id)(struct net_device *, enum ethtool_phys_id_state);
	void	(*get_ethtool_stats)(struct net_device *,
				     struct ethtool_stats *, u64 *);
	int	(*begin)(struct net_device *);
	void	(*complete)(struct net_device *);
	u32	(*get_priv_flags)(struct net_device *);
	int	(*set_priv_flags)(struct net_device *, u32);
	int	(*get_sset_count)(struct net_device *, int);
	int	(*get_rxnfc)(struct net_device *,
			     struct ethtool_rxnfc *, u32 *rule_locs);
	int	(*set_rxnfc)(struct net_device *, struct ethtool_rxnfc *);
	int	(*flash_device)(struct net_device *, struct ethtool_flash *);
	int	(*reset)(struct net_device *, u32 *);
	u32	(*get_rxfh_indir_size)(struct net_device *);
	int	(*get_rxfh_indir)(struct net_device *, u32 *);
	int	(*set_rxfh_indir)(struct net_device *, const u32 *);
	void	(*get_channels)(struct net_device *, struct ethtool_channels *);
	int	(*set_channels)(struct net_device *, struct ethtool_channels *);
	int	(*get_dump_flag)(struct net_device *, struct ethtool_dump *);
	int	(*get_dump_data)(struct net_device *,
				 struct ethtool_dump *, void *);
	int	(*set_dump)(struct net_device *, struct ethtool_dump *);
	int	(*get_ts_info)(struct net_device *, struct ethtool_ts_info *);
	int     (*get_module_info)(struct net_device *,
				   struct ethtool_modinfo *);
	int     (*get_module_eeprom)(struct net_device *,
				     struct ethtool_eeprom *, u8 *);
	int	(*get_eee)(struct net_device *, struct ethtool_eee *);
	int	(*set_eee)(struct net_device *, struct ethtool_eee *);


};
#endif /*                  */
