/*
 * Windfarm PowerMac thermal control
 *
 * Copyright 2012 Benjamin Herrenschmidt, IBM Corp.
 *
 * Released under the term of the GNU GPL v2.
 */

#ifndef __WINDFARM_MPU_H
#define __WINDFARM_MPU_H

typedef unsigned short fu16;
typedef int fs32;
typedef short fs16;

/*                                                            
                                                             
 */
struct mpu_data
{
	u8	signature;		/*                    */
	u8	bytes_used;		/*                                     */
	u8	size;			/*                            */
	u8	version;		/*                       */
	u32	data_revision;		/*                         */
	u8	processor_bin_code[3];	/*                           */
	u8	bin_code_expansion;	/*                        */
	u8	processor_num;		/*                                   */
	u8	input_mul_bus_div;	/*                                           */
	u8	reserved1[2];		/*        */
	u32	input_clk_freq_high;	/*                                   */
	u8	cpu_nb_target_cycles;	/*            */
	u8	cpu_statlat;		/*            */
	u8	cpu_snooplat;		/*            */
	u8	cpu_snoopacc;		/*            */
	u8	nb_paamwin;		/*            */
	u8	nb_statlat;		/*            */
	u8	nb_snooplat;		/*            */
	u8	nb_snoopwin;		/*            */
	u8	api_bus_mode;		/*            */
	u8	reserved2[3];		/*        */
	u32	input_clk_freq_low;	/*                                  */
	u8	processor_card_slot;	/*                                   */
	u8	reserved3[2];		/*        */
	u8	padjmax;       		/*                                          */
	u8	ttarget;		/*                           */
	u8	tmax;			/*                        */
	u8	pmaxh;			/*                  */
	u8	tguardband;		/*                                            */
	fs32	pid_gp;			/*                              */
	fs32	pid_gr;			/*                       */
	fs32	pid_gd;			/*                            */
	fu16	voph;			/*                 */
	fu16	vopl;			/*                */
	fs16	nactual_die;		/*                    */
	fs16	nactual_heatsink;	/*                         */
	fs16	nactual_system;		/*                       */
	u16	calibration_flags;	/*                          */
	fu16	mdiode;			/*                                       */
	fs16	bdiode;			/*                               */
	fs32	theta_heat_sink;	/*                        */
	u16	rminn_intake_fan;	/*                           */
	u16	rmaxn_intake_fan;	/*                           */
	u16	rminn_exhaust_fan;	/*                            */
	u16	rmaxn_exhaust_fan;	/*                            */
	u8	processor_part_num[8];	/*                                               */
	u32	processor_lot_num;	/*                             */
	u8	orig_card_sernum[0x10];	/*                                    */
	u8	curr_card_sernum[0x10];	/*                                   */
	u8	mlb_sernum[0x18];	/*                          */
	u32	checksum1;		/*        */
	u32	checksum2;		/*        */	
}; /*                   */

static inline const struct mpu_data *wf_get_mpu(int cpu)
{
	struct device_node *np;
	char nodename[64];
	const void *data;
	int len;

	/*
                                                                 
                                                                
                                
  */
	sprintf(nodename, "/u3@0,f8000000/i2c@f8001000/cpuid@a%d", cpu ? 2 : 0);
	np = of_find_node_by_path(nodename);
	if (!np)
		return NULL;
	data = of_get_property(np, "cpuid", &len);	
	of_node_put(np);
	if (!data)
		return NULL;

	/*
                                                               
                                                              
                                                              
                                   
  */
	return data;
}

#endif /*                   */
