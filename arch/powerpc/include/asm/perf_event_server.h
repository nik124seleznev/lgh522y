/*
 * Performance event support - PowerPC classic/server specific definitions.
 *
 * Copyright 2008-2009 Paul Mackerras, IBM Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/types.h>
#include <asm/hw_irq.h>
#include <linux/device.h>

#define MAX_HWEVENTS		8
#define MAX_EVENT_ALTERNATIVES	8
#define MAX_LIMITED_HWCOUNTERS	2

/*
                                                             
                                                     
 */
struct power_pmu {
	const char	*name;
	int		n_counter;
	int		max_alternatives;
	unsigned long	add_fields;
	unsigned long	test_adder;
	int		(*compute_mmcr)(u64 events[], int n_ev,
				unsigned int hwc[], unsigned long mmcr[]);
	int		(*get_constraint)(u64 event_id, unsigned long *mskp,
				unsigned long *valp);
	int		(*get_alternatives)(u64 event_id, unsigned int flags,
				u64 alt[]);
	u64             (*bhrb_filter_map)(u64 branch_sample_type);
	void            (*config_bhrb)(u64 pmu_bhrb_filter);
	void		(*disable_pmc)(unsigned int pmc, unsigned long mmcr[]);
	int		(*limited_pmc_event)(u64 event_id);
	u32		flags;
	const struct attribute_group	**attr_groups;
	int		n_generic;
	int		*generic_events;
	int		(*cache_events)[PERF_COUNT_HW_CACHE_MAX]
			       [PERF_COUNT_HW_CACHE_OP_MAX]
			       [PERF_COUNT_HW_CACHE_RESULT_MAX];

	/*                         */
	int		bhrb_nr;
};

/*
                             
 */
#define PPMU_LIMITED_PMC5_6	0x00000001 /*                              */
#define PPMU_ALT_SIPR		0x00000002 /*                                 */
#define PPMU_NO_SIPR		0x00000004 /*                            */
#define PPMU_NO_CONT_SAMPLING	0x00000008 /*                        */
#define PPMU_SIAR_VALID		0x00000010 /*                              */
#define PPMU_HAS_SSLOT		0x00000020 /*                           */
#define PPMU_HAS_SIER		0x00000040 /*          */
#define PPMU_ARCH_207S		0x00000080 /*                            */

/*
                                         
 */
#define PPMU_LIMITED_PMC_OK	1	/*                               */
#define PPMU_LIMITED_PMC_REQD	2	/*                                   */
#define PPMU_ONLY_COUNT_RUN	4	/*                            */

extern int register_power_pmu(struct power_pmu *);

struct pt_regs;
extern unsigned long perf_misc_flags(struct pt_regs *regs);
extern unsigned long perf_instruction_pointer(struct pt_regs *regs);
extern unsigned long int read_bhrb(int n);

/*
                                                                      
                                   
 */
#ifdef CONFIG_PPC_PERF_CTRS
#define perf_misc_flags(regs)	perf_misc_flags(regs)
#endif

/*
                                                                      
                                                                          
                
  
                                                                     
                            
  
                                                                    
                                                                         
                                                                    
                                                                     
                                                                        
                                                                      
                
  
                                                                     
                                                                     
                                                                        
                                                                           
                                                                        
                                                                  
                                                               
                                                    
  
                                                                         
                                                                         
                                                                        
                                                                        
                                                                       
                                                                    
                                                                          
                        
  
                                                                            
                                                                        
 */

extern ssize_t power_events_sysfs_show(struct device *dev,
				struct device_attribute *attr, char *page);

/*
                                                      
  
                                                                       
                                                                     
                                                                       
                           
 */
#define	EVENT_VAR(_id, _suffix)		event_attr_##_id##_suffix
#define	EVENT_PTR(_id, _suffix)		&EVENT_VAR(_id, _suffix).attr.attr

#define	EVENT_ATTR(_name, _id, _suffix)					\
	PMU_EVENT_ATTR(_name, EVENT_VAR(_id, _suffix), PME_PM_##_id,	\
			power_events_sysfs_show)

#define	GENERIC_EVENT_ATTR(_name, _id)	EVENT_ATTR(_name, _id, _g)
#define	GENERIC_EVENT_PTR(_id)		EVENT_PTR(_id, _g)

#define	POWER_EVENT_ATTR(_name, _id)	EVENT_ATTR(PM_##_name, _id, _p)
#define	POWER_EVENT_PTR(_id)		EVENT_PTR(_id, _p)
