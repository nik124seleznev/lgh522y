/*
 * core.h - DesignWare HS OTG Controller common declarations
 *
 * Copyright (C) 2004-2013 Synopsys, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any
 * later version.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __DWC2_CORE_H__
#define __DWC2_CORE_H__

#include <linux/usb/phy.h>
#include "hw.h"

#ifdef DWC2_LOG_WRITES
static inline void do_write(u32 value, void *addr)
{
	writel(value, addr);
	pr_info("INFO:: wrote %08x to %p\n", value, addr);
}

#undef writel
#define writel(v, a)	do_write(v, a)
#endif

/*                                          */
#define MAX_EPS_CHANNELS	16

struct dwc2_hsotg;
struct dwc2_host_chan;

/*               */
enum dwc2_lx_state {
	DWC2_L0,	/*          */
	DWC2_L1,	/*                 */
	DWC2_L2,	/*                   */
	DWC2_L3,	/*           */
};

/* 
                                                                
  
                                                                       
                                                                            
                                          
                                                          
                                             
                                               
                                                                                
                                                                                
                                                                      
                                  
                                                        
                                                                          
                                                                             
                                                                                
                                                            
                                        
                                                                   
                                                                            
                                                                             
                                                                      
                                                 
                                       
                                                                               
                                                                            
                                  
                                                                   
                                                   
                                                                             
                                                                               
                                                       
                                                                               
                                                                          
                                   
                                  
                                                                              
                                                                
                                                                               
                                                      
                                                   
                                                                                
                                                                        
                                                   
                                                                              
                                                                     
                                                   
                                                                     
                                                        
                                                                   
                                                
                                                                   
                                             
                                                                               
                                                                          
                                                                                
                                                                             
                                                                              
                                                                         
                                                                              
                                                                              
                                                                            
                              
                                             
                                                                               
                                                                                
                                
                                                                            
                                               
                                                                            
                                     
                                                                                
                                                         
                                                                            
                                                                                
                              
                                         
                                
                                                                        
                                                                
                                                                                
                                   
                                                                               
                               
                                                                               
                                                             
                                                                              
                                                                                
                                             
                                
                                
  
                                                                            
                                                                     
 */
struct dwc2_core_params {
	/*
                                                       
                        
  */
	int otg_cap;
	int otg_ver;
	int dma_enable;
	int dma_desc_enable;
	int speed;
	int enable_dynamic_fifo;
	int en_multiple_tx_fifo;
	int host_rx_fifo_size;
	int host_nperio_tx_fifo_size;
	int host_perio_tx_fifo_size;
	int max_transfer_size;
	int max_packet_count;
	int host_channels;
	int phy_type;
	int phy_utmi_width;
	int phy_ulpi_ddr;
	int phy_ulpi_ext_vbus;
	int i2c_enable;
	int ulpi_fs_ls;
	int host_support_fs_ls_low_power;
	int host_ls_low_power_phy_clk;
	int ts_dline;
	int reload_ctl;
	int ahb_single;
};

/* 
                                                                                
                         
  
                                                 
                                     
                                                                                
                                                                            
                                                                            
                                                                            
                                                                            
                                                                            
                                                  
                                                            
                                               
                                                             
                                                                           
                                                                             
                                                                      
                                   
                                                                        
                                                               
                                                                            
                                             
                                                                             
                                                                           
                                 
                                                                           
                                                    
                                                                  
                                                                           
                                                                             
                                                   
                                                                       
                                                                         
                                                   
                                                                    
                                             
                                                                             
                                                                         
                                                                                
                                                                            
                                                                          
                                                                               
                                                                              
                                                                               
                                                                              
                                                                              
                                                                                
                                                                              
                                            
                                                                               
                                                                              
                                                              
                                                                       
                                                           
                                                                            
                                                                            
                                                                      
                                                                         
                                   
                                                                         
                                                                      
                                                                                
                                                                                
                                                                   
                                                                               
                                                                    
                                                
                                                                              
                                                                           
                                                                              
                                              
                                                                                
                                                  
                                                                             
                                           
                                                                               
                                                    
                                                                               
                                                                         
                                                                              
                                                                                
                                                                           
                                 
                                                                          
                                                                            
                                                                        
                                 
                                                                                
                                           
                                                  
                                                                         
                                                              
                                                                             
                                                              
                                       
                                  
                                              
 */
struct dwc2_hsotg {
	struct device *dev;
	void __iomem *regs;
	struct dwc2_core_params *core_params;
	u32 hwcfg1;
	u32 hwcfg2;
	u32 hwcfg3;
	u32 hwcfg4;
	u32 hptxfsiz;
	u32 snpsid;
	u16 total_fifo_size;
	u16 rx_fifo_size;
	u16 nperio_tx_fifo_size;
	enum usb_otg_state op_state;

	unsigned int queuing_high_bandwidth:1;
	unsigned int srp_success:1;

	struct workqueue_struct *wq_otg;
	struct work_struct wf_otg;
	struct timer_list wkp_timer;
	enum dwc2_lx_state lx_state;

	union dwc2_hcd_internal_flags {
		u32 d32;
		struct {
			unsigned port_connect_status_change:1;
			unsigned port_connect_status:1;
			unsigned port_reset_change:1;
			unsigned port_enable_change:1;
			unsigned port_suspend_change:1;
			unsigned port_over_current_change:1;
			unsigned port_l1_change:1;
			unsigned reserved:26;
		} b;
	} flags;

	struct list_head non_periodic_sched_inactive;
	struct list_head non_periodic_sched_active;
	struct list_head *non_periodic_qh_ptr;
	struct list_head periodic_sched_inactive;
	struct list_head periodic_sched_ready;
	struct list_head periodic_sched_assigned;
	struct list_head periodic_sched_queued;
	u16 periodic_usecs;
	u16 frame_number;
	u16 periodic_qh_count;

#ifdef CONFIG_USB_DWC2_TRACK_MISSED_SOFS
#define FRAME_NUM_ARRAY_SIZE 1000
	u16 last_frame_num;
	u16 *frame_num_array;
	u16 *last_frame_num_array;
	int frame_num_idx;
	int dumped_frame_num_array;
#endif

	struct list_head free_hc_list;
	int periodic_channels;
	int non_periodic_channels;
	struct dwc2_host_chan *hc_ptr_array[MAX_EPS_CHANNELS];
	u8 *status_buf;
	dma_addr_t status_buf_dma;
#define DWC2_HCD_STATUS_BUF_SIZE 64

	struct delayed_work start_work;
	struct delayed_work reset_work;
	spinlock_t lock;
	void *priv;
	u8 otg_port;
	u32 *frame_list;
	dma_addr_t frame_list_dma;

	/*                             */
#define DWC2_CORE_REV_2_71a	0x4f54271a
#define DWC2_CORE_REV_2_90a	0x4f54290a
#define DWC2_CORE_REV_2_92a	0x4f54292a
#define DWC2_CORE_REV_2_94a	0x4f54294a
#define DWC2_CORE_REV_3_00a	0x4f54300a

#ifdef DEBUG
	u32 frrem_samples;
	u64 frrem_accum;

	u32 hfnum_7_samples_a;
	u64 hfnum_7_frrem_accum_a;
	u32 hfnum_0_samples_a;
	u64 hfnum_0_frrem_accum_a;
	u32 hfnum_other_samples_a;
	u64 hfnum_other_frrem_accum_a;

	u32 hfnum_7_samples_b;
	u64 hfnum_7_frrem_accum_b;
	u32 hfnum_0_samples_b;
	u64 hfnum_0_frrem_accum_b;
	u32 hfnum_other_samples_b;
	u64 hfnum_other_frrem_accum_b;
#endif
};

/*                                    */
enum dwc2_halt_status {
	DWC2_HC_XFER_NO_HALT_STATUS,
	DWC2_HC_XFER_COMPLETE,
	DWC2_HC_XFER_URB_COMPLETE,
	DWC2_HC_XFER_ACK,
	DWC2_HC_XFER_NAK,
	DWC2_HC_XFER_NYET,
	DWC2_HC_XFER_STALL,
	DWC2_HC_XFER_XACT_ERR,
	DWC2_HC_XFER_FRAME_OVERRUN,
	DWC2_HC_XFER_BABBLE_ERR,
	DWC2_HC_XFER_DATA_TOGGLE_ERR,
	DWC2_HC_XFER_AHB_ERR,
	DWC2_HC_XFER_PERIODIC_INCOMPLETE,
	DWC2_HC_XFER_URB_DEQUEUE,
};

/*
                                                                              
                             
 */
extern void dwc2_core_host_init(struct dwc2_hsotg *hsotg);

/*
                       
                                                                          
        
 */
extern void dwc2_hc_init(struct dwc2_hsotg *hsotg, struct dwc2_host_chan *chan);
extern void dwc2_hc_halt(struct dwc2_hsotg *hsotg, struct dwc2_host_chan *chan,
			 enum dwc2_halt_status halt_status);
extern void dwc2_hc_cleanup(struct dwc2_hsotg *hsotg,
			    struct dwc2_host_chan *chan);
extern void dwc2_hc_start_transfer(struct dwc2_hsotg *hsotg,
				   struct dwc2_host_chan *chan);
extern void dwc2_hc_start_transfer_ddma(struct dwc2_hsotg *hsotg,
					struct dwc2_host_chan *chan);
extern int dwc2_hc_continue_transfer(struct dwc2_hsotg *hsotg,
				     struct dwc2_host_chan *chan);
extern void dwc2_hc_do_ping(struct dwc2_hsotg *hsotg,
			    struct dwc2_host_chan *chan);
extern void dwc2_enable_host_interrupts(struct dwc2_hsotg *hsotg);
extern void dwc2_disable_host_interrupts(struct dwc2_hsotg *hsotg);

extern u32 dwc2_calc_frame_interval(struct dwc2_hsotg *hsotg);
extern int dwc2_check_core_status(struct dwc2_hsotg *hsotg);

/*
                         
                                                                            
                       
 */
extern void dwc2_read_packet(struct dwc2_hsotg *hsotg, u8 *dest, u16 bytes);
extern void dwc2_flush_tx_fifo(struct dwc2_hsotg *hsotg, const int num);
extern void dwc2_flush_rx_fifo(struct dwc2_hsotg *hsotg);

extern int dwc2_core_init(struct dwc2_hsotg *hsotg, bool select_phy, int irq);
extern void dwc2_enable_global_interrupts(struct dwc2_hsotg *hcd);
extern void dwc2_disable_global_interrupts(struct dwc2_hsotg *hcd);

/*                                                             */
extern irqreturn_t dwc2_handle_common_intr(int irq, void *dev);

/*                     */

/*
                                                                
                                                            
                                    
                       
                         
 */
extern int dwc2_set_param_otg_cap(struct dwc2_hsotg *hsotg, int val);
#define DWC2_CAP_PARAM_HNP_SRP_CAPABLE		0
#define DWC2_CAP_PARAM_SRP_ONLY_CAPABLE		1
#define DWC2_CAP_PARAM_NO_HNP_SRP_CAPABLE	2

/*
                                                                    
                                                                 
                                  
            
                                  
 */
extern int dwc2_set_param_dma_enable(struct dwc2_hsotg *hsotg, int val);

/*
                                                    
                                                           
                                                             
                                                     
                  
                                            
 */
extern int dwc2_set_param_dma_desc_enable(struct dwc2_hsotg *hsotg, int val);

/*
                                                                    
                                                                   
                                                                      
                   
                           
                 
 */
extern int dwc2_set_param_speed(struct dwc2_hsotg *hsotg, int val);
#define DWC2_SPEED_PARAM_HIGH	0
#define DWC2_SPEED_PARAM_FULL	1

/*
                                                              
                                                    
  
                                             
                             
 */
extern int dwc2_set_param_host_support_fs_ls_low_power(struct dwc2_hsotg *hsotg,
						       int val);

/*
                                                                     
                                                                      
                                                                    
                                           
  
             
            
 */
extern int dwc2_set_param_host_ls_low_power_phy_clk(struct dwc2_hsotg *hsotg,
						    int val);
#define DWC2_HOST_LS_LOW_POWER_PHY_CLK_PARAM_48MHZ	0
#define DWC2_HOST_LS_LOW_POWER_PHY_CLK_PARAM_6MHZ	1

/*
                                  
                                          
 */
extern int dwc2_set_param_enable_dynamic_fifo(struct dwc2_hsotg *hsotg,
					      int val);

/*
                                                                  
                          
                             
 */
extern int dwc2_set_param_host_rx_fifo_size(struct dwc2_hsotg *hsotg, int val);

/*
                                                                  
                                                   
                            
 */
extern int dwc2_set_param_host_nperio_tx_fifo_size(struct dwc2_hsotg *hsotg,
						   int val);

/*
                                                                   
                          
                            
 */
extern int dwc2_set_param_host_perio_tx_fifo_size(struct dwc2_hsotg *hsotg,
						  int val);

/*
                                                
                                   
 */
extern int dwc2_set_param_max_transfer_size(struct dwc2_hsotg *hsotg, int val);

/*
                                               
                           
 */
extern int dwc2_set_param_max_packet_count(struct dwc2_hsotg *hsotg, int val);

/*
                                               
                       
                                                                       
 */
extern int dwc2_set_param_host_channels(struct dwc2_hsotg *hsotg, int val);

/*
                                                                     
                                          
  
                     
                      
           
 */
extern int dwc2_set_param_phy_type(struct dwc2_hsotg *hsotg, int val);
#define DWC2_PHY_TYPE_PARAM_FS		0
#define DWC2_PHY_TYPE_PARAM_UTMI	1
#define DWC2_PHY_TYPE_PARAM_ULPI	2

/*
                                                    
                                                          
                                                            
                                                         
                                                              
                                                     
                                                
  
                            
 */
extern int dwc2_set_param_phy_utmi_width(struct dwc2_hsotg *hsotg, int val);

/*
                                                          
                                                              
        
  
                                                           
                
                                                           
      
 */
extern int dwc2_set_param_phy_ulpi_ddr(struct dwc2_hsotg *hsotg, int val);

/*
                                                              
                                  
 */
extern int dwc2_set_param_phy_ulpi_ext_vbus(struct dwc2_hsotg *hsotg, int val);
#define DWC2_PHY_ULPI_INTERNAL_VBUS	0
#define DWC2_PHY_ULPI_EXTERNAL_VBUS	1

/*
                                                                     
                                                  
                   
          
 */
extern int dwc2_set_param_i2c_enable(struct dwc2_hsotg *hsotg, int val);

extern int dwc2_set_param_ulpi_fs_ls(struct dwc2_hsotg *hsotg, int val);

extern int dwc2_set_param_ts_dline(struct dwc2_hsotg *hsotg, int val);

/*
                                                 
                                                       
         
          
 */
extern int dwc2_set_param_en_multiple_tx_fifo(struct dwc2_hsotg *hsotg,
					      int val);

extern int dwc2_set_param_reload_ctl(struct dwc2_hsotg *hsotg, int val);

extern int dwc2_set_param_ahb_single(struct dwc2_hsotg *hsotg, int val);

extern int dwc2_set_param_otg_ver(struct dwc2_hsotg *hsotg, int val);

/*
                                
 */
extern void dwc2_dump_dev_registers(struct dwc2_hsotg *hsotg);
extern void dwc2_dump_host_registers(struct dwc2_hsotg *hsotg);
extern void dwc2_dump_global_registers(struct dwc2_hsotg *hsotg);

/*
                                         
 */
extern u16 dwc2_get_otg_version(struct dwc2_hsotg *hsotg);

#endif /*                 */
