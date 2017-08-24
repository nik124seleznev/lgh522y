#ifndef _PARISC_PDC_H
#define _PARISC_PDC_H

#include <uapi/asm/pdc.h>

#if !defined(__ASSEMBLY__)

extern int pdc_type;

/*                     */
#define PDC_TYPE_ILLEGAL	-1
#define PDC_TYPE_PAT		 0 /*                */
#define PDC_TYPE_SYSTEM_MAP	 1 /*                                     */
#define PDC_TYPE_SNAKE		 2 /*                            */

struct pdc_chassis_info {       /*                      */
	unsigned long actcnt;   /*                                 */
	unsigned long maxcnt;   /*                                                */
};

struct pdc_coproc_cfg {         /*                    */
        unsigned long ccr_functional;
        unsigned long ccr_present;
        unsigned long revision;
        unsigned long model;
};

struct pdc_model {		/*               */
	unsigned long hversion;
	unsigned long sversion;
	unsigned long hw_id;
	unsigned long boot_id;
	unsigned long sw_id;
	unsigned long sw_cap;
	unsigned long arch_rev;
	unsigned long pot_key;
	unsigned long curr_key;
};

struct pdc_cache_cf {		/*                             */
    unsigned long
#ifdef CONFIG_64BIT
		cc_padW:32,
#endif
		cc_alias: 4,	/*                                          */
		cc_block: 4,	/*                                    */
		cc_line	: 3,	/*                                                                         */
		cc_shift: 2,	/*                                 */
		cc_wt	: 1,	/*                              */
		cc_sh	: 2,	/*                                               */
		cc_cst  : 3,	/*                                            */
		cc_pad1 : 10,	/*          */
		cc_hv   : 3;	/*                    */
};

struct pdc_tlb_cf {		/*                           */
    unsigned long tc_pad0:12,	/*          */
#ifdef CONFIG_64BIT
		tc_padW:32,
#endif
		tc_sh	: 2,	/*                                           */
		tc_hv   : 1,	/*    */
		tc_page : 1,	/*                                            */
		tc_cst  : 3,	/*                                                     */
		tc_aid  : 5,	/*                                                   */
		tc_pad1 : 8;	/*                                          */
};

struct pdc_cache_info {		/*                                           */
	/*         */
	unsigned long	ic_size;	/*               */
	struct pdc_cache_cf ic_conf;	/*               */
	unsigned long	ic_base;	/*           */
	unsigned long	ic_stride;
	unsigned long	ic_count;
	unsigned long	ic_loop;
	/*         */
	unsigned long	dc_size;	/*               */
	struct pdc_cache_cf dc_conf;	/*               */
	unsigned long	dc_base;	/*           */
	unsigned long	dc_stride;
	unsigned long	dc_count;
	unsigned long	dc_loop;
	/*                 */
	unsigned long	it_size;	/*                            */
	struct pdc_tlb_cf it_conf;	/*                     */
	unsigned long	it_sp_base;
	unsigned long	it_sp_stride;
	unsigned long	it_sp_count;
	unsigned long	it_off_base;
	unsigned long	it_off_stride;
	unsigned long	it_off_count;
	unsigned long	it_loop;
	/*          */
	unsigned long	dt_size;	/*                            */
	struct pdc_tlb_cf dt_conf;	/*                     */
	unsigned long	dt_sp_base;
	unsigned long	dt_sp_stride;
	unsigned long	dt_sp_count;
	unsigned long	dt_off_base;
	unsigned long	dt_off_stride;
	unsigned long	dt_off_count;
	unsigned long	dt_loop;
};

#if 0
/*                                                                
                                        
 */
struct pdc_iodc {     /*          */
	unsigned char   hversion_model;
	unsigned char 	hversion;
	unsigned char 	spa;
	unsigned char 	type;
	unsigned int	sversion_rev:4;
	unsigned int	sversion_model:19;
	unsigned int	sversion_opt:8;
	unsigned char	rev;
	unsigned char	dep;
	unsigned char	features;
	unsigned char	pad1;
	unsigned int	checksum:16;
	unsigned int	length:16;
	unsigned int    pad[15];
} __attribute__((aligned(8))) ;
#endif

#ifndef CONFIG_PA20
/*                              */
struct pdc_btlb_info_range {
	__u8 res00;
	__u8 num_i;
	__u8 num_d;
	__u8 num_comb;
};

struct pdc_btlb_info {	/*                                        */
	unsigned int min_size;	/*                               */
	unsigned int max_size;	/*                               */
	struct pdc_btlb_info_range fixed_range_info;
	struct pdc_btlb_info_range variable_range_info;
};

#endif /*              */

#ifdef CONFIG_64BIT
struct pdc_memory_table_raddr { /*                                     */
	unsigned long entries_returned;
	unsigned long entries_total;
};

struct pdc_memory_table {       /*                                   */
	unsigned long paddr;
	unsigned int  pages;
	unsigned int  reserved;
};
#endif /*              */

struct pdc_system_map_mod_info { /*                            */
	unsigned long mod_addr;
	unsigned long mod_pgs;
	unsigned long add_addrs;
};

struct pdc_system_map_addr_info { /*                             */
	unsigned long mod_addr;
	unsigned long mod_pgs;
};

struct pdc_initiator { /*               */
	int host_id;
	int factor;
	int width;
	int mode;
};

struct hardware_path {
	char  flags;	/*                           */
	char  bc[6];	/*                                          */
			/*                                          */
	char  mod;	/*                                 */
};

/*
                                          
 */
struct pdc_module_path {
	struct hardware_path path;
	unsigned int layers[6]; /*                                           */
};

#ifndef CONFIG_PA20
/*                                   */
struct pdc_memory_map {		/*                */
	unsigned long hpa;	/*                            */
	unsigned long more_pgs;	/*                              */
};
#endif

struct pdc_tod {
	unsigned long tod_sec; 
	unsigned long tod_usec;
};

/*                                                                   */

struct pdc_hpmc_pim_11 { /*         */
	__u32 gr[32];
	__u32 cr[32];
	__u32 sr[8];
	__u32 iasq_back;
	__u32 iaoq_back;
	__u32 check_type;
	__u32 cpu_state;
	__u32 rsvd1;
	__u32 cache_check;
	__u32 tlb_check;
	__u32 bus_check;
	__u32 assists_check;
	__u32 rsvd2;
	__u32 assist_state;
	__u32 responder_addr;
	__u32 requestor_addr;
	__u32 path_info;
	__u64 fr[32];
};

/*
                                                                    
  
                                                                      
                                                                        
  
                                                                  
                                                                       
                                                                       
        
 */

struct pdc_hpmc_pim_20 { /*         */
	__u64 gr[32];
	__u64 cr[32];
	__u64 sr[8];
	__u64 iasq_back;
	__u64 iaoq_back;
	__u32 check_type;
	__u32 cpu_state;
	__u32 cache_check;
	__u32 tlb_check;
	__u32 bus_check;
	__u32 assists_check;
	__u32 assist_state;
	__u32 path_info;
	__u64 responder_addr;
	__u64 requestor_addr;
	__u64 fr[32];
};

void pdc_console_init(void);	/*                  */
void pdc_console_restart(void);

void setup_pdc(void);		/*                */

/*                              */

int pdc_add_valid(unsigned long address);
int pdc_chassis_info(struct pdc_chassis_info *chassis_info, void *led_info, unsigned long len);
int pdc_chassis_disp(unsigned long disp);
int pdc_chassis_warn(unsigned long *warn);
int pdc_coproc_cfg(struct pdc_coproc_cfg *pdc_coproc_info);
int pdc_coproc_cfg_unlocked(struct pdc_coproc_cfg *pdc_coproc_info);
int pdc_iodc_read(unsigned long *actcnt, unsigned long hpa, unsigned int index,
		  void *iodc_data, unsigned int iodc_data_size);
int pdc_system_map_find_mods(struct pdc_system_map_mod_info *pdc_mod_info,
			     struct pdc_module_path *mod_path, long mod_index);
int pdc_system_map_find_addrs(struct pdc_system_map_addr_info *pdc_addr_info,
			      long mod_index, long addr_index);
int pdc_model_info(struct pdc_model *model);
int pdc_model_sysmodel(char *name);
int pdc_model_cpuid(unsigned long *cpu_id);
int pdc_model_versions(unsigned long *versions, int id);
int pdc_model_capabilities(unsigned long *capabilities);
int pdc_cache_info(struct pdc_cache_info *cache);
int pdc_spaceid_bits(unsigned long *space_bits);
#ifndef CONFIG_PA20
int pdc_btlb_info(struct pdc_btlb_info *btlb);
int pdc_mem_map_hpa(struct pdc_memory_map *r_addr, struct pdc_module_path *mod_path);
#endif /*              */
int pdc_lan_station_id(char *lan_addr, unsigned long net_hpa);

int pdc_stable_read(unsigned long staddr, void *memaddr, unsigned long count);
int pdc_stable_write(unsigned long staddr, void *memaddr, unsigned long count);
int pdc_stable_get_size(unsigned long *size);
int pdc_stable_verify_contents(void);
int pdc_stable_initialize(void);

int pdc_pci_irt_size(unsigned long *num_entries, unsigned long hpa);
int pdc_pci_irt(unsigned long num_entries, unsigned long hpa, void *tbl);

int pdc_get_initiator(struct hardware_path *, struct pdc_initiator *);
int pdc_tod_read(struct pdc_tod *tod);
int pdc_tod_set(unsigned long sec, unsigned long usec);

#ifdef CONFIG_64BIT
int pdc_mem_mem_table(struct pdc_memory_table_raddr *r_addr,
		struct pdc_memory_table *tbl, unsigned long entries);
#endif

void set_firmware_width(void);
void set_firmware_width_unlocked(void);
int pdc_do_firm_test_reset(unsigned long ftc_bitmap);
int pdc_do_reset(void);
int pdc_soft_power_info(unsigned long *power_reg);
int pdc_soft_power_button(int sw_control);
void pdc_io_reset(void);
void pdc_io_reset_devices(void);
int pdc_iodc_getc(void);
int pdc_iodc_print(const unsigned char *str, unsigned count);

void pdc_emergency_unlock(void);
int pdc_sti_call(unsigned long func, unsigned long flags,
                 unsigned long inptr, unsigned long outputr,
                 unsigned long glob_cfg);

static inline char * os_id_to_string(u16 os_id) {
	switch(os_id) {
	case OS_ID_NONE:	return "No OS";
	case OS_ID_HPUX:	return "HP-UX";
	case OS_ID_MPEXL:	return "MPE-iX";
	case OS_ID_OSF:		return "OSF";
	case OS_ID_HPRT:	return "HP-RT";
	case OS_ID_NOVEL:	return "Novell Netware";
	case OS_ID_LINUX:	return "Linux";
	default:	return "Unknown";
	}
}

#endif /*                        */
#endif /*               */
