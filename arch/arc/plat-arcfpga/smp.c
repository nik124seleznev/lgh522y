/*
 * ARC700 Simulation-only Extensions for SMP
 *
 * Copyright (C) 2004, 2007-2010, 2011-2012 Synopsys, Inc. (www.synopsys.com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  Vineet Gupta    - 2012 : split off arch common and plat specific SMP
 *  Rajeshwar Ranga - 2007 : Interrupt Distribution Unit API's
 */

#include <linux/smp.h>
#include <linux/irq.h>
#include <plat/irq.h>
#include <plat/smp.h>

static char smp_cpuinfo_buf[128];

/*
                                                                     
                                                        
                                                                     
 */

/*
                                   
 */
static void iss_model_smp_wakeup_cpu(int cpu, unsigned long pc)
{
	/*                    */
	write_aux_reg(ARC_AUX_XTL_REG_PARAM, pc);

	/*                                   */
	write_aux_reg(ARC_AUX_XTL_REG_CMD,
			(ARC_XTL_CMD_WRITE_PC | (cpu << 8)));

	/*                          */
	write_aux_reg(ARC_AUX_XTL_REG_CMD,
			(ARC_XTL_CMD_CLEAR_HALT | (cpu << 8)));

}

/*
                                                       
                                                             
                      
                            
                                          
 */
void iss_model_init_smp(unsigned int cpu)
{
	/*                                                        */
	if (NR_IRQS <= 16 || get_hw_config_num_irq() <= 16)
		panic("[arcfpga] IRQ system can't support IDU IPI\n");

	idu_disable();

	/*                                                               
                                                                       
                                  
                                                        
   
                                     
                                         
                                                                       
                                         
   
                                                                    
                                       
                                                                   */

	/*
                                                     
                                                                    
                                               
  */
	idu_irq_set_tgtcpu(cpu, 0x1 << cpu);

	/*                                                  */
	idu_irq_set_mode(cpu, 7, /*                                        */
			 IDU_IRQ_MODE_PULSE_TRIG);

	idu_enable();

	/*                                               */
	smp_ipi_irq_setup(cpu, IDU_INTERRUPT_0 + cpu);
}

static void iss_model_ipi_send(void *arg)
{
	struct cpumask *callmap = arg;
	unsigned int cpu;

	for_each_cpu(cpu, callmap)
		idu_irq_assert(cpu);
}

static void iss_model_ipi_clear(int cpu, int irq)
{
	idu_irq_clear(IDU_INTERRUPT_0 + cpu);
}

void iss_model_init_early_smp(void)
{
#define IS_AVAIL1(var, str)    ((var) ? str : "")

	struct bcr_mp mp;

	READ_BCR(ARC_REG_MP_BCR, mp);

	sprintf(smp_cpuinfo_buf, "Extn [ISS-SMP]: v%d, arch(%d) %s %s %s\n",
		mp.ver, mp.mp_arch, IS_AVAIL1(mp.scu, "SCU"),
		IS_AVAIL1(mp.idu, "IDU"), IS_AVAIL1(mp.sdu, "SDU"));

	plat_smp_ops.info = smp_cpuinfo_buf;

	plat_smp_ops.cpu_kick = iss_model_smp_wakeup_cpu;
	plat_smp_ops.ipi_send = iss_model_ipi_send;
	plat_smp_ops.ipi_clear = iss_model_ipi_clear;
}

/*
                                                                     
                                   
                                                                     
 */

/*                                 */
void idu_irq_set_mode(uint8_t irq, uint8_t dest_mode, uint8_t trig_mode)
{
	uint32_t par = IDU_IRQ_MODE_PARAM(dest_mode, trig_mode);

	IDU_SET_PARAM(par);
	IDU_SET_COMMAND(irq, IDU_IRQ_WMODE);
}

/*                                           */
void idu_irq_set_tgtcpu(uint8_t irq, uint32_t mask)
{
	IDU_SET_PARAM(mask);
	IDU_SET_COMMAND(irq, IDU_IRQ_WBITMASK);
}

/*                                                                */
bool idu_irq_get_ack(uint8_t irq)
{
	uint32_t val;

	IDU_SET_COMMAND(irq, IDU_IRQ_ACK);
	val = IDU_GET_PARAM();

	return val & (1 << irq);
}

/*
                                                            
                                                                 
                                                                 
                                
 */
bool idu_irq_get_pend(uint8_t irq)
{
	uint32_t val;

	IDU_SET_COMMAND(irq, IDU_IRQ_PEND);
	val = IDU_GET_PARAM();

	return val & (1 << irq);
}
