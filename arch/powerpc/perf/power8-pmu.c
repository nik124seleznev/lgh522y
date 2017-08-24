/*
 * Performance counter support for POWER8 processors.
 *
 * Copyright 2009 Paul Mackerras, IBM Corporation.
 * Copyright 2013 Michael Ellerman, IBM Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/perf_event.h>
#include <asm/firmware.h>


/*
                           
 */
#define PM_CYC				0x0001e
#define PM_GCT_NOSLOT_CYC		0x100f8
#define PM_CMPLU_STALL			0x4000a
#define PM_INST_CMPL			0x00002
#define PM_BRU_FIN			0x10068
#define PM_BR_MPRED_CMPL		0x400f6


/*
                                 
  
                                                                                  
                                                                                    
                                                                                  
                                                                          
                                                                          
  
                                                                                  
                                                                                    
                                                                                  
                                                              
                                                                    
                                                        
                                                        
                                                                 
        
                    
  
                                
  
                                  
                                  
  
                                                          
                         
                                                                 
                                                                 
                             
                                                                 
       
                                                 
  
                 
                            
  
                 
                                 
                                 
  
                            
                                                   
                                  
                                  
                                                 
                             
                                                 
                                   
                           
                            
  
           
                                    
                                              
                                               
  
 */

#define EVENT_THR_CMP_SHIFT	40	/*                     */
#define EVENT_THR_CMP_MASK	0x3ff
#define EVENT_THR_CTL_SHIFT	32	/*                                      */
#define EVENT_THR_CTL_MASK	0xffull
#define EVENT_THR_SEL_SHIFT	29	/*                        */
#define EVENT_THR_SEL_MASK	0x7
#define EVENT_THRESH_SHIFT	29	/*                    */
#define EVENT_THRESH_MASK	0x1fffffull
#define EVENT_SAMPLE_SHIFT	24	/*                             */
#define EVENT_SAMPLE_MASK	0x1f
#define EVENT_CACHE_SEL_SHIFT	20	/*                    */
#define EVENT_CACHE_SEL_MASK	0xf
#define EVENT_IS_L1		(4 << EVENT_CACHE_SEL_SHIFT)
#define EVENT_PMC_SHIFT		16	/*                      */
#define EVENT_PMC_MASK		0xf
#define EVENT_UNIT_SHIFT	12	/*      */
#define EVENT_UNIT_MASK		0xf
#define EVENT_COMBINE_SHIFT	11	/*             */
#define EVENT_COMBINE_MASK	0x1
#define EVENT_MARKED_SHIFT	8	/*            */
#define EVENT_MARKED_MASK	0x1
#define EVENT_IS_MARKED		(EVENT_MARKED_MASK << EVENT_MARKED_SHIFT)
#define EVENT_PSEL_MASK		0xff	/*               */

#define EVENT_VALID_MASK	\
	((EVENT_THRESH_MASK    << EVENT_THRESH_SHIFT)		|	\
	 (EVENT_SAMPLE_MASK    << EVENT_SAMPLE_SHIFT)		|	\
	 (EVENT_CACHE_SEL_MASK << EVENT_CACHE_SEL_SHIFT)	|	\
	 (EVENT_PMC_MASK       << EVENT_PMC_SHIFT)		|	\
	 (EVENT_UNIT_MASK      << EVENT_UNIT_SHIFT)		|	\
	 (EVENT_COMBINE_MASK   << EVENT_COMBINE_SHIFT)		|	\
	 (EVENT_MARKED_MASK    << EVENT_MARKED_SHIFT)		|	\
	  EVENT_PSEL_MASK)

/*                         */
#define	POWER8_MMCRA_IFM1		0x0000000040000000UL
#define	POWER8_MMCRA_IFM2		0x0000000080000000UL
#define	POWER8_MMCRA_IFM3		0x00000000C0000000UL

#define ONLY_PLM \
	(PERF_SAMPLE_BRANCH_USER        |\
	 PERF_SAMPLE_BRANCH_KERNEL      |\
	 PERF_SAMPLE_BRANCH_HV)

/*
                             
  
                                                                                  
                                                                                    
                                                                                  
                                                                                
                                                                                
  
                                                                                  
                                                                                    
                                                                                  
                                                 
                                                                                    
                                                                                
                                                 
  
                                                                                
                                                                       
  
                                    
 */

#define CNST_FAB_MATCH_VAL(v)	(((v) & EVENT_THR_CTL_MASK) << 56)
#define CNST_FAB_MATCH_MASK	CNST_FAB_MATCH_VAL(EVENT_THR_CTL_MASK)

/*                                                          */
#define CNST_THRESH_VAL(v)	(((v) & EVENT_THRESH_MASK) << 32)
#define CNST_THRESH_MASK	CNST_THRESH_VAL(EVENT_THRESH_MASK)

#define CNST_L1_QUAL_VAL(v)	(((v) & 3) << 22)
#define CNST_L1_QUAL_MASK	CNST_L1_QUAL_VAL(3)

#define CNST_SAMPLE_VAL(v)	(((v) & EVENT_SAMPLE_MASK) << 16)
#define CNST_SAMPLE_MASK	CNST_SAMPLE_VAL(EVENT_SAMPLE_MASK)

/*
                                                                               
                                                                               
                             
 */
#define CNST_NC_SHIFT		12
#define CNST_NC_VAL		(1 << CNST_NC_SHIFT)
#define CNST_NC_MASK		(8 << CNST_NC_SHIFT)
#define POWER8_TEST_ADDER	(3 << CNST_NC_SHIFT)

/*
                                                                           
                                                                           
                                                      
 */
#define CNST_PMC_SHIFT(pmc)	((pmc - 1) * 2)
#define CNST_PMC_VAL(pmc)	(1 << CNST_PMC_SHIFT(pmc))
#define CNST_PMC_MASK(pmc)	(2 << CNST_PMC_SHIFT(pmc))

/*                               */
#define POWER8_ADD_FIELDS	\
	CNST_PMC_VAL(1) | CNST_PMC_VAL(2) | CNST_PMC_VAL(3) | \
	CNST_PMC_VAL(4) | CNST_PMC_VAL(5) | CNST_PMC_VAL(6) | CNST_NC_VAL


/*                          */
#define MMCR1_UNIT_SHIFT(pmc)		(60 - (4 * ((pmc) - 1)))
#define MMCR1_COMBINE_SHIFT(pmc)	(35 - ((pmc) - 1))
#define MMCR1_PMCSEL_SHIFT(pmc)		(24 - (((pmc) - 1)) * 8)
#define MMCR1_FAB_SHIFT			36
#define MMCR1_DC_QUAL_SHIFT		47
#define MMCR1_IC_QUAL_SHIFT		46

/*                          */
#define MMCRA_SAMP_MODE_SHIFT		1
#define MMCRA_SAMP_ELIG_SHIFT		4
#define MMCRA_THR_CTL_SHIFT		8
#define MMCRA_THR_SEL_SHIFT		16
#define MMCRA_THR_CMP_SHIFT		32
#define MMCRA_SDAR_MODE_TLB		(1ull << 42)


static inline bool event_is_fab_match(u64 event)
{
	/*                                                           */
	event &= 0xff0fe;

	/*                                                 */
	return (event == 0x30056 || event == 0x4f052);
}

static int power8_get_constraint(u64 event, unsigned long *maskp, unsigned long *valp)
{
	unsigned int unit, pmc, cache;
	unsigned long mask, value;

	mask = value = 0;

	if (event & ~EVENT_VALID_MASK)
		return -1;

	pmc   = (event >> EVENT_PMC_SHIFT)       & EVENT_PMC_MASK;
	unit  = (event >> EVENT_UNIT_SHIFT)      & EVENT_UNIT_MASK;
	cache = (event >> EVENT_CACHE_SEL_SHIFT) & EVENT_CACHE_SEL_MASK;

	if (pmc) {
		if (pmc > 6)
			return -1;

		mask  |= CNST_PMC_MASK(pmc);
		value |= CNST_PMC_VAL(pmc);

		if (pmc >= 5 && event != 0x500fa && event != 0x600f4)
			return -1;
	}

	if (pmc <= 4) {
		/*
                                                                     
                                                                  
                                                                   
                                                           
   */
		mask  |= CNST_NC_MASK;
		value |= CNST_NC_VAL;
	}

	if (unit >= 6 && unit <= 9) {
		/*
                                                          
                                                                
                                                                 
                                                             
                                                               
                                   
   */
		if (cache)
			return -1;

	} else if (event & EVENT_IS_L1) {
		mask  |= CNST_L1_QUAL_MASK;
		value |= CNST_L1_QUAL_VAL(cache);
	}

	if (event & EVENT_IS_MARKED) {
		mask  |= CNST_SAMPLE_MASK;
		value |= CNST_SAMPLE_VAL(event >> EVENT_SAMPLE_SHIFT);
	}

	/*
                                                                       
                                                            
  */
	if (event_is_fab_match(event)) {
		mask  |= CNST_FAB_MATCH_MASK;
		value |= CNST_FAB_MATCH_VAL(event >> EVENT_THR_CTL_SHIFT);
	} else {
		/*
                                                               
                                                            
   */
		unsigned int cmp, exp;

		cmp = (event >> EVENT_THR_CMP_SHIFT) & EVENT_THR_CMP_MASK;
		exp = cmp >> 7;

		if (exp && (cmp & 0x60) == 0)
			return -1;

		mask  |= CNST_THRESH_MASK;
		value |= CNST_THRESH_VAL(event >> EVENT_THRESH_SHIFT);
	}

	*maskp = mask;
	*valp = value;

	return 0;
}

static int power8_compute_mmcr(u64 event[], int n_ev,
			       unsigned int hwc[], unsigned long mmcr[])
{
	unsigned long mmcra, mmcr1, unit, combine, psel, cache, val;
	unsigned int pmc, pmc_inuse;
	int i;

	pmc_inuse = 0;

	/*                                  */
	for (i = 0; i < n_ev; ++i) {
		pmc = (event[i] >> EVENT_PMC_SHIFT) & EVENT_PMC_MASK;
		if (pmc)
			pmc_inuse |= 1 << pmc;
	}

	/*                                                     */
	mmcra = MMCRA_SDAR_MODE_TLB;
	mmcr1 = 0;

	/*                                                */
	for (i = 0; i < n_ev; ++i) {
		pmc     = (event[i] >> EVENT_PMC_SHIFT) & EVENT_PMC_MASK;
		unit    = (event[i] >> EVENT_UNIT_SHIFT) & EVENT_UNIT_MASK;
		combine = (event[i] >> EVENT_COMBINE_SHIFT) & EVENT_COMBINE_MASK;
		psel    =  event[i] & EVENT_PSEL_MASK;

		if (!pmc) {
			for (pmc = 1; pmc <= 4; ++pmc) {
				if (!(pmc_inuse & (1 << pmc)))
					break;
			}

			pmc_inuse |= 1 << pmc;
		}

		if (pmc <= 4) {
			mmcr1 |= unit << MMCR1_UNIT_SHIFT(pmc);
			mmcr1 |= combine << MMCR1_COMBINE_SHIFT(pmc);
			mmcr1 |= psel << MMCR1_PMCSEL_SHIFT(pmc);
		}

		if (event[i] & EVENT_IS_L1) {
			cache = event[i] >> EVENT_CACHE_SEL_SHIFT;
			mmcr1 |= (cache & 1) << MMCR1_IC_QUAL_SHIFT;
			cache >>= 1;
			mmcr1 |= (cache & 1) << MMCR1_DC_QUAL_SHIFT;
		}

		if (event[i] & EVENT_IS_MARKED) {
			mmcra |= MMCRA_SAMPLE_ENABLE;

			val = (event[i] >> EVENT_SAMPLE_SHIFT) & EVENT_SAMPLE_MASK;
			if (val) {
				mmcra |= (val &  3) << MMCRA_SAMP_MODE_SHIFT;
				mmcra |= (val >> 2) << MMCRA_SAMP_ELIG_SHIFT;
			}
		}

		/*
                                                       
                                                     
   */
		if (event_is_fab_match(event[i])) {
			mmcr1 |= ((event[i] >> EVENT_THR_CTL_SHIFT) &
				  EVENT_THR_CTL_MASK) << MMCR1_FAB_SHIFT;
		} else {
			val = (event[i] >> EVENT_THR_CTL_SHIFT) & EVENT_THR_CTL_MASK;
			mmcra |= val << MMCRA_THR_CTL_SHIFT;
			val = (event[i] >> EVENT_THR_SEL_SHIFT) & EVENT_THR_SEL_MASK;
			mmcra |= val << MMCRA_THR_SEL_SHIFT;
			val = (event[i] >> EVENT_THR_CMP_SHIFT) & EVENT_THR_CMP_MASK;
			mmcra |= val << MMCRA_THR_CMP_SHIFT;
		}

		hwc[i] = pmc - 1;
	}

	/*                     */
	mmcr[0] = 0;

	/*                      */
	if (pmc_inuse & 2)
		mmcr[0] = MMCR0_PMC1CE;

	if (pmc_inuse & 0x7c)
		mmcr[0] |= MMCR0_PMCjCE;

	/*                                            */
	if (!(pmc_inuse & 0x60))
		mmcr[0] |= MMCR0_FC56;

	mmcr[1] = mmcr1;
	mmcr[2] = mmcra;

	return 0;
}

#define MAX_ALT	2

/*                                           */
static const unsigned int event_alternatives[][MAX_ALT] = {
	{ 0x10134, 0x301e2 },		/*                */
	{ 0x10138, 0x40138 },		/*                 */
	{ 0x18082, 0x3e05e },		/*               */
	{ 0x1d14e, 0x401e8 },		/*                         */
	{ 0x1e054, 0x4000a },		/*                */
	{ 0x20036, 0x40036 },		/*             */
	{ 0x200f2, 0x300f2 },		/*              */
	{ 0x200f4, 0x600f4 },		/*            */
	{ 0x2013c, 0x3012e },		/*                   */
	{ 0x3e054, 0x400f0 },		/*               */
	{ 0x400fa, 0x500fa },		/*                  */
};

/*
                                                         
                                                       
 */
static int find_alternative(u64 event)
{
	int i, j;

	for (i = 0; i < ARRAY_SIZE(event_alternatives); ++i) {
		if (event < event_alternatives[i][0])
			break;

		for (j = 0; j < MAX_ALT && event_alternatives[i][j]; ++j)
			if (event == event_alternatives[i][j])
				return i;
	}

	return -1;
}

static int power8_get_alternatives(u64 event, unsigned int flags, u64 alt[])
{
	int i, j, num_alt = 0;
	u64 alt_event;

	alt[num_alt++] = event;

	i = find_alternative(event);
	if (i >= 0) {
		/*                                                       */
		for (j = 0; j < MAX_ALT; ++j) {
			alt_event = event_alternatives[i][j];
			if (alt_event && alt_event != event)
				alt[num_alt++] = alt_event;
		}
	}

	if (flags & PPMU_ONLY_COUNT_RUN) {
		/*
                                                                 
                                                      
   */
		j = num_alt;
		for (i = 0; i < num_alt; ++i) {
			switch (alt[i]) {
			case 0x1e:	/*        */
				alt[j++] = 0x600f4;	/*            */
				break;
			case 0x600f4:	/*            */
				alt[j++] = 0x1e;
				break;
			case 0x2:	/*             */
				alt[j++] = 0x500fa;	/*                  */
				break;
			case 0x500fa:	/*                  */
				alt[j++] = 0x2;	/*             */
				break;
			}
		}
		num_alt = j;
	}

	return num_alt;
}

static void power8_disable_pmc(unsigned int pmc, unsigned long mmcr[])
{
	if (pmc <= 3)
		mmcr[1] &= ~(0xffUL << MMCR1_PMCSEL_SHIFT(pmc + 1));
}

PMU_FORMAT_ATTR(event,		"config:0-49");
PMU_FORMAT_ATTR(pmcxsel,	"config:0-7");
PMU_FORMAT_ATTR(mark,		"config:8");
PMU_FORMAT_ATTR(combine,	"config:11");
PMU_FORMAT_ATTR(unit,		"config:12-15");
PMU_FORMAT_ATTR(pmc,		"config:16-19");
PMU_FORMAT_ATTR(cache_sel,	"config:20-23");
PMU_FORMAT_ATTR(sample_mode,	"config:24-28");
PMU_FORMAT_ATTR(thresh_sel,	"config:29-31");
PMU_FORMAT_ATTR(thresh_stop,	"config:32-35");
PMU_FORMAT_ATTR(thresh_start,	"config:36-39");
PMU_FORMAT_ATTR(thresh_cmp,	"config:40-49");

static struct attribute *power8_pmu_format_attr[] = {
	&format_attr_event.attr,
	&format_attr_pmcxsel.attr,
	&format_attr_mark.attr,
	&format_attr_combine.attr,
	&format_attr_unit.attr,
	&format_attr_pmc.attr,
	&format_attr_cache_sel.attr,
	&format_attr_sample_mode.attr,
	&format_attr_thresh_sel.attr,
	&format_attr_thresh_stop.attr,
	&format_attr_thresh_start.attr,
	&format_attr_thresh_cmp.attr,
	NULL,
};

struct attribute_group power8_pmu_format_group = {
	.name = "format",
	.attrs = power8_pmu_format_attr,
};

static const struct attribute_group *power8_pmu_attr_groups[] = {
	&power8_pmu_format_group,
	NULL,
};

static int power8_generic_events[] = {
	[PERF_COUNT_HW_CPU_CYCLES] =			PM_CYC,
	[PERF_COUNT_HW_STALLED_CYCLES_FRONTEND] =	PM_GCT_NOSLOT_CYC,
	[PERF_COUNT_HW_STALLED_CYCLES_BACKEND] =	PM_CMPLU_STALL,
	[PERF_COUNT_HW_INSTRUCTIONS] =			PM_INST_CMPL,
	[PERF_COUNT_HW_BRANCH_INSTRUCTIONS] =		PM_BRU_FIN,
	[PERF_COUNT_HW_BRANCH_MISSES] =			PM_BR_MPRED_CMPL,
};

static u64 power8_bhrb_filter_map(u64 branch_sample_type)
{
	u64 pmu_bhrb_filter = 0;
	u64 br_privilege = branch_sample_type & ONLY_PLM;

	/*                                                           
                                                              
                                                                  
                                                                 
                                                                 
                                                                
                                                              
  */
	if ((br_privilege != 7) && (br_privilege != 0))
		return -1;

	/*                            */
	if (branch_sample_type & PERF_SAMPLE_BRANCH_ANY)
		return pmu_bhrb_filter;

	/*                                                     */
	if (branch_sample_type & PERF_SAMPLE_BRANCH_ANY_RETURN)
		return -1;

	if (branch_sample_type & PERF_SAMPLE_BRANCH_IND_CALL)
		return -1;

	if (branch_sample_type & PERF_SAMPLE_BRANCH_ANY_CALL) {
		pmu_bhrb_filter |= POWER8_MMCRA_IFM1;
		return pmu_bhrb_filter;
	}

	/*                                 */
	return -1;
}

static void power8_config_bhrb(u64 pmu_bhrb_filter)
{
	/*                           */
	mtspr(SPRN_MMCRA, (mfspr(SPRN_MMCRA) | pmu_bhrb_filter));
}

static struct power_pmu power8_pmu = {
	.name			= "POWER8",
	.n_counter		= 6,
	.max_alternatives	= MAX_ALT + 1,
	.add_fields		= POWER8_ADD_FIELDS,
	.test_adder		= POWER8_TEST_ADDER,
	.compute_mmcr		= power8_compute_mmcr,
	.config_bhrb		= power8_config_bhrb,
	.bhrb_filter_map	= power8_bhrb_filter_map,
	.get_constraint		= power8_get_constraint,
	.get_alternatives	= power8_get_alternatives,
	.disable_pmc		= power8_disable_pmc,
	.flags			= PPMU_HAS_SSLOT | PPMU_HAS_SIER | PPMU_ARCH_207S,
	.n_generic		= ARRAY_SIZE(power8_generic_events),
	.generic_events		= power8_generic_events,
	.attr_groups		= power8_pmu_attr_groups,
	.bhrb_nr		= 32,
};

static int __init init_power8_pmu(void)
{
	if (!cur_cpu_spec->oprofile_cpu_type ||
	    strcmp(cur_cpu_spec->oprofile_cpu_type, "ppc64/power8"))
		return -ENODEV;

	return register_power_pmu(&power8_pmu);
}
early_initcall(init_power8_pmu);
