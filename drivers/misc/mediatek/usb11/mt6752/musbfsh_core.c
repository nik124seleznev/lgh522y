/*
 * MUSB OTG driver core code
 *
 * Copyright 2005 Mentor Graphics Corporation
 * Copyright (C) 2005-2006 by Texas Instruments
 * Copyright (C) 2006-2007 Nokia Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
                                                               
  
                                                                   
                                                                  
                                                                     
                                         
  
                                                                   
                                                                  
                                                                    
                                        
  
  
                                                                    
                                                                      
                                                                       
                           
  
                                                                       
                                                         
  
                                                                      
                                                                      
                                                                    
                                                    
  
                                                                  
                                                                  
                                                                  
                                                                   
                         
  
                                                                       
  
                                                                      
                                                                    
                                                                        
                                                                         
  
                                                                    
                                                                   
                                                                   
                                                                       
                                                                
 */

/*
                                                     
                                             
                                                           
                                                           
                                            
  
                                                             
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/proc_fs.h>
#include <linux/wakelock.h>
#include <linux/ctype.h>
#include <linux/dma-mapping.h>

#ifdef	CONFIG_ARM
#include <mach/hardware.h>
#include <mach/memory.h>
#include <asm/mach-types.h>
#endif

#include "musbfsh_core.h"
#include "musbfsh_host.h"
#include "musbfsh_dma.h"
#include "musbfsh_hsdma.h"
#include "musbfsh_mt65xx.h"
#include "usb.h" 

#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_i2c.h>
#include <mach/mt_clkmgr.h>

static int __init musbfsh_dts_probe(struct platform_device *pdev);

static const struct of_device_id apusb11_of_ids[] = {
       { .compatible = "mediatek,ICUSB",},
       {},
};

static struct platform_driver musbfsh_dts_driver = {
	.driver = {
		.name       ="musb11_dts",
        .of_match_table = apusb11_of_ids,
	},
	.probe      = musbfsh_dts_probe,
};

#ifdef CONFIG_MTK_USBFSH
#include <mach/mt_pm_ldo.h>


extern irqreturn_t musbfsh_dma_controller_irq(int irq, void *private_data);


#define DRIVER_AUTHOR "Mentor Graphics, Texas Instruments, Nokia, Mediatek"
#define DRIVER_DESC "MT65xx USB1.1 Host Controller Driver"

#define MUSBFSH_VERSION "6.0"

#define DRIVER_INFO DRIVER_DESC ", v" MUSBFSH_VERSION

#define MUSBFSH_DRIVER_NAME "musbfsh_hdrc"
const char musbfsh_driver_name[] = MUSBFSH_DRIVER_NAME;

MODULE_DESCRIPTION(DRIVER_INFO);
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" MUSBFSH_DRIVER_NAME);

struct wake_lock musbfsh_suspend_lock;
struct musbfsh  *g_musbfsh;
static u64 usb11_dmamask = DMA_BIT_MASK(32);

#ifdef CONFIG_MTK_ICUSB_SUPPORT
void mt65xx_usb11_mac_phy_dump(void);
void mt65xx_usb11_phy_recover(void);
void mt65xx_usb11_phy_savecurrent(void);
void mt65xx_usb11_enable_clk_pll_pw(void);
void mt65xx_usb11_disable_clk_pll_pw(void);
//                                  
#ifdef CONFIG_PM
static int suspend_cnt = 0, resume_cnt = 0;
#endif
#endif

DEFINE_SPINLOCK(musbfs_io_lock);

static struct musbfsh *mtk_musbfsh;
int is_musbfsh_rh(struct usb_device *udev)
{
	struct usb_device *rhdev;
	struct usb_hcd	*hcd;

	if(!mtk_musbfsh){
		MYDBG("mtk_musbfsh is NULL!\n");
		return 0;
	}

	hcd = musbfsh_to_hcd(mtk_musbfsh);
	rhdev = hcd->self.root_hub;
	return (udev == rhdev)? 1 : 0;
}

#ifdef MTK_DT_USB_SUPPORT
extern void request_wakeup_md_timeout(unsigned int dev_id, unsigned int dev_sub_id);
#endif
void mt65xx_usb11_suspend_resume_test(void)
{
	u8	power;
	u32 frame_number;	

	MYDBG("begin\n");

	power = musbfsh_readb((unsigned char __iomem*)mtk_musbfsh->mregs, MUSBFSH_POWER);
	//                                 
	power |= MUSBFSH_POWER_ENSUSPEND;
	power |= MUSBFSH_POWER_SUSPENDM;
	musbfsh_writeb((unsigned char __iomem*)mtk_musbfsh->mregs, MUSBFSH_POWER, power);

	frame_number = musbfsh_readw((unsigned char __iomem*)mtk_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(12);
	MYDBG("suspend done\n");
	frame_number = musbfsh_readw((unsigned char __iomem*)mtk_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(1000);
	MYDBG("start resuming\n");

#ifdef MTK_DT_USB_SUPPORT
	MYDBG("request md wake up\n");
	request_wakeup_md_timeout(0, 0);		
#endif

	power = musbfsh_readb((unsigned char __iomem*)mtk_musbfsh->mregs, MUSBFSH_POWER);
	power &= ~MUSBFSH_POWER_SUSPENDM;
	power |= MUSBFSH_POWER_RESUME;
	musbfsh_writeb((unsigned char __iomem*)mtk_musbfsh->mregs, MUSBFSH_POWER, power);
	MYDBG("");

	mdelay(12);
	MYDBG("stop resuming\n");

	power = musbfsh_readb((unsigned char __iomem*)mtk_musbfsh->mregs, MUSBFSH_POWER);
	power &= ~MUSBFSH_POWER_RESUME;
	musbfsh_writeb((unsigned char __iomem*)mtk_musbfsh->mregs, MUSBFSH_POWER, power);
	
	frame_number = musbfsh_readw((unsigned char __iomem*)mtk_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(3);
	
	frame_number = musbfsh_readw((unsigned char __iomem*)mtk_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	MYDBG("end\n");

}

#ifdef MTK_DT_USB_SUPPORT
#include <linux/of.h>
#include <linux/of_irq.h>
#include <mach/eint.h> 
#include <mach/mt_gpio.h>
static int ext_usb_wkup_irq = 0;
#define USB_WAKE_TIME 5 //                                                                                                                 
struct wake_lock usb_resume_lock;

int musbfsh_eint_cnt = 0; 
int musbfsh_skip_phy = 1;	
int musbfsh_skip_clk = 0;
int musbfsh_skip_port_suspend = 0;
int musbfsh_skip_port_resume = 0;
int musbfsh_wake_lock_hold = 0;
	
void create_dsda_tmp_entry(void);

void release_usb11_wakelock(void)
{
	MYDBG("\n");
	if(musbfsh_wake_lock_hold){
		wake_unlock(&musbfsh_suspend_lock);	
	}	
}

irqreturn_t remote_wakeup_irq(unsigned irq, struct irq_desc *desc)
{
	musbfsh_eint_cnt++;
	MYDBG("cpuid:%d\n", smp_processor_id());	
#if 0
	u8		power, devctl;
	unsigned long flags;

	if(!mtk_musbfsh) {
		MYDBG("\n");//          
		return;
	}
	MYDBG("\n");//          
	spin_lock_irqsave(&mtk_musbfsh->lock, flags);
	
	devctl = musbfsh_readb(mtk_musbfsh->mregs, MUSBFSH_DEVCTL);
	if (devctl & MUSBFSH_DEVCTL_HM) {
		wake_lock_timeout(&usb_resume_lock, USB_WAKE_TIME * HZ);
		musbfsh_set_power(mtk_musbfsh, 1);
		power = musbfsh_readb(mtk_musbfsh->mregs, MUSBFSH_POWER);
		WARNING("Remote wakeup! power=0x%x\n", power);
		if(!(power & MUSBFSH_POWER_SUSPENDM)) {
			MYDBG("\n");//          
			goto out;
		}
		
		/*                                     
                               
   */
		power &= ~MUSBFSH_POWER_SUSPENDM;
		musbfsh_writeb(mtk_musbfsh->mregs, MUSBFSH_POWER,
				power | MUSBFSH_POWER_RESUME);
	
		mtk_musbfsh->port1_status |=
				(USB_PORT_STAT_C_SUSPEND << 16)
				| MUSBFSH_PORT_STAT_RESUME;
		mtk_musbfsh->rh_timer = jiffies
				+ msecs_to_jiffies(20);
	
		mtk_musbfsh->is_active = 1;
		usb_hcd_resume_root_hub(musbfsh_to_hcd(mtk_musbfsh));
	}
out:	
	spin_unlock_irqrestore(&mtk_musbfsh->lock, flags);
#endif

	//                                                   
	return IRQ_HANDLED;
}

void get_eint_ext_usb_wkup_irq(void){
	struct device_node *node;
	node = of_find_compatible_node(NULL, NULL, "mediatek, DT_EXT_MD_WK_UP_USB-eint");
	if(node){
		//                                        
		ext_usb_wkup_irq = irq_of_parse_and_map(node, 0);		
		if(!ext_usb_wkup_irq) {
			MYDBG("can't irq_of_parse_and_map!\n");
			return;
		}
		
	}
	else{
		MYDBG("can't find node\n");
	}
}

void register_eint_usb_p1_wakeup_callback(void)
{
	int ret;

	if(!ext_usb_wkup_irq){
		MYDBG("no ext_usb_wkup_irq\n");
		return;
	}

	//                              
	ret = request_irq(ext_usb_wkup_irq, (irq_handler_t)remote_wakeup_irq, IRQF_TRIGGER_FALLING, "DT_EXT_MD_WK_UP_USB-eint", NULL);	
	if(ret > 0){
		MYDBG("eint irq %d not available\n", ext_usb_wkup_irq);
		return;
	}

}
#endif


//                               
#ifdef ORG_SUSPEND_RESUME_TEST		
#define USB_BASE                   0xF1200000
void mt65xx_usb20_suspend_resume_test(void)
{
	u8	power;
	u32 frame_number;	

	MYDBG("begin\n");

	power = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER);
	//                                 
	power |= MUSBFSH_POWER_ENSUSPEND;
	power |= MUSBFSH_POWER_SUSPENDM;
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER, power);

	frame_number = musbfsh_readw((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(12);
	MYDBG("suspend done\n");
	frame_number = musbfsh_readw((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(1000);
	MYDBG("start resuming\n");

	power = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER);
	power &= ~MUSBFSH_POWER_SUSPENDM;
	power |= MUSBFSH_POWER_RESUME;
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER, power);
	MYDBG("");

	mdelay(12);
	MYDBG("stop resuming\n");

	power = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER);
	power &= ~MUSBFSH_POWER_RESUME;
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER, power);
	
	frame_number = musbfsh_readw((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(3);
	
	frame_number = musbfsh_readw((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	MYDBG("end\n");

}


#endif

#ifdef CONFIG_MTK_ICUSB_SUPPORT 

struct my_attr skip_session_req_attr = {
	.attr.name = "skip_session_req",
	.attr.mode = 0644,
#ifdef MTK_ICUSB_SKIP_SESSION_REQ
	.value = 1
#else
	.value = 0
#endif
};

struct my_attr skip_enable_session_attr = {
	.attr.name = "skip_enable_session",
	.attr.mode = 0644,
#ifdef MTK_ICUSB_SKIP_ENABLE_SESSION
	.value = 1
#else
	.value = 0
#endif
};

struct my_attr skip_mac_init_attr = {
	.attr.name = "skip_mac_init",
	.attr.mode = 0644,
#ifdef MTK_ICUSB_SKIP_MAC_INIT
	.value = 1
#else
	.value = 0
#endif
};

struct my_attr hw_dbg_attr = {
	.attr.name = "hw_dbg",
	.attr.mode = 0644,
#ifdef MTK_ICUSB_HW_DBG
	.value = 1
#else
	.value = 0
#endif
};



static int usb11_enabled = 0;

unsigned char g_usb_clk_reg_before, g_usb_clk_reg_after;
unsigned char g_FS_EOF1_before, g_FS_EOF1_after;

void mt65xx_usb11_disable_clk_pll_pw(void);
void mt65xx_usb11_enable_clk_pll_pw(void);
void mt65xx_usb11_phy_poweron(void);
void set_usb_phy_voltage(enum PHY_VOLTAGE_TYPE phy_volt);
void create_ic_usb_cmd_proc_entry(void);
void mt65xx_usb11_mac_phy_babble_clear(struct musbfsh *musbfsh);
void mt65xx_usb11_mac_phy_babble_recover(struct musbfsh *musbfsh);
void mt65xx_usb11_mac_phy_dump(void);
int mt65xx_check_usb11_clk_status(void);
void mt65xx_usb11_mac_reset_and_phy_stress_set(void);			
void set_usb11_sts_disconnecting(void);						
void create_icusb_sysfs_attr(void);
int check_usb11_sts_disconnect_done(void);
void set_usb11_sts_connect(void);

#define USB_BASE                   0xF1200000
void mt65xx_usb20_suspend_resume_test(void)
{
	u8	power;
	u32 frame_number;	

	MYDBG("begin\n");

	power = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER);
	//                                 
	power |= MUSBFSH_POWER_ENSUSPEND;
	power |= MUSBFSH_POWER_SUSPENDM;
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER, power);

	frame_number = musbfsh_readw((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(12);
	MYDBG("suspend done\n");
	frame_number = musbfsh_readw((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(1000);
	MYDBG("start resuming\n");

	power = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER);
	power &= ~MUSBFSH_POWER_SUSPENDM;
	power |= MUSBFSH_POWER_RESUME;
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER, power);
	MYDBG("");

	mdelay(12);
	MYDBG("stop resuming\n");

	power = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER);
	power &= ~MUSBFSH_POWER_RESUME;
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_POWER, power);
	
	frame_number = musbfsh_readw((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	mdelay(3);
	
	frame_number = musbfsh_readw((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_FRAME); 
	MYDBG("frame_number : %d\n", frame_number);

	MYDBG("end\n");

}


void musbfsh_root_disc_procedure(void)
{
	MYDBG("");
	usb_hcd_resume_root_hub(musbfsh_to_hcd(g_musbfsh));
	musbfsh_root_disconnect(g_musbfsh);
}

void ic_usb_clk_chg(void)
{
	//                                
	g_usb_clk_reg_before = __raw_readb((void __iomem *)0xF000014C);
	g_usb_clk_reg_after = g_usb_clk_reg_before & (~0x1);  
	g_usb_clk_reg_after = g_usb_clk_reg_after | 0x2;  
	__raw_writeb(g_usb_clk_reg_after, (void __iomem *)0xF000014C);
}

void musbfsh_open_vsim_power(int on)
{
	if(on)
		hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_3100, "USB11-SIM");
	else
		hwPowerDown(MT6325_POWER_LDO_VSIM1, "USB11-SIM");	 	
	MYDBG("musbfsh_open_vsim_power = %d\n",on);	
}
void musbfsh_open_vsim_power1(int on)
{
	if(on)
		hwPowerOn(MT6325_POWER_LDO_VSIM1, VOL_3000, "USB11-SIM");
	else
		hwPowerDown(MT6325_POWER_LDO_VSIM1, "USB11-SIM");	 	
	MYDBG("musbfsh_open_vsim_power = %d\n",on);	
}

void musbfsh_start_session_pure(void )
{
	u8 devctl;
	MYDBG("");

	devctl = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_DEVCTL);
	devctl |= MUSBFSH_DEVCTL_SESSION;
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_DEVCTL, devctl);

	MYDBG("[IC-USB]start session PURE\n");
}
void musbfsh_init_phy_by_voltage(enum PHY_VOLTAGE_TYPE phy_volt)
{
	MYDBG("");
	if(!usb11_enabled)
	{
		usb11_enabled = 1;
		mt65xx_usb11_enable_clk_pll_pw();		
	}

	set_usb_phy_voltage(phy_volt);
	mt65xx_usb11_phy_poweron();

}
void musbfsh_start_session(void )
{
	u8 devctl;
		/*              */
	devctl = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs,MUSBFSH_DEVCTL);
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_DEVCTL, (devctl&(~MUSBFSH_DEVCTL_SESSION)));	
	
	USB11PHY_SET8(0x6c, 0x10);
	USB11PHY_CLR8(0x6c, 0x2E);
	USB11PHY_SET8(0x6d, 0x3E);
	mdelay(5);	
	devctl = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_DEVCTL);
	devctl |= MUSBFSH_DEVCTL_SESSION;
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_DEVCTL, devctl);

	USB11PHY_CLR8(0x6c, 0x10);
	USB11PHY_SET8(0x6c, 0x2c);
	USB11PHY_SET8(0x6d, 0x3e);

#ifdef MTK_ICUSB_TAKE_WAKE_LOCK
	MYDBG("[IC-USB]start session, wake_lock taken feature enalbed\n");
#else
	MYDBG("[IC-USB]start session, wake_lock taken feature disalbed\n");
#endif
}


void musbfsh_stop_session(void )
{
	u8 devctl;
	MYDBG("");
	devctl = musbfsh_readb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_DEVCTL);
	devctl &= ~(MUSBFSH_DEVCTL_SESSION);
	musbfsh_writeb((unsigned char __iomem*)g_musbfsh->mregs, MUSBFSH_DEVCTL, devctl);
	
	MYDBG("[IC_USB] stop session\n");
}

#if defined(MTK_ICUSB_BABBLE_RECOVER) || defined(MTK_SMARTBOOK_SUPPORT)
static struct timer_list babble_recover_timer;
static void babble_recover_func(unsigned long _musbfsh)
{
	int flags = 0;
	u8 devctl = 0;
	struct musbfsh *musbfsh = (struct musbfsh *) _musbfsh;

	WARNING("execute babble_recover_func!\n");

	/*
                                                   
                                                       
  */
	mdelay(20);

	spin_lock_irqsave(&musbfsh->lock, flags);

	mt65xx_usb11_mac_phy_babble_recover(musbfsh);

	mdelay(1);
	devctl |= MUSBFSH_DEVCTL_SESSION;
	musbfsh_writeb(musbfsh->mregs, MUSBFSH_DEVCTL, devctl);

	del_timer(&babble_recover_timer);

	spin_unlock_irqrestore(&musbfsh->lock, flags);
}
#endif


	

static int musbfsh_core_init1(struct musbfsh *musbfsh);
static void musbfsh_generic_disable(struct musbfsh *musbfsh);
void musbfsh_mac_init(void)
{
	struct timeval tv_begin, tv_end;
	do_gettimeofday(&tv_begin);

	MYDBG("");

	musbfsh_generic_disable(g_musbfsh);

	musbfsh_writeb(g_musbfsh->mregs,MUSBFSH_HSDMA_DMA_INTR_UNMASK_SET,0xff);

	musbfsh_core_init1(g_musbfsh);

	musbfsh_start(g_musbfsh);
	
	do_gettimeofday(&tv_end);
	MYDBG("time spent, sec : %d, usec : %d\n", (unsigned int)(tv_end.tv_sec - tv_begin.tv_sec), (unsigned int)(tv_end.tv_usec - tv_begin.tv_usec));

}

int is_usb11_enabled(void)
{
	return usb11_enabled;
}

void set_usb11_enabled(void)
{
	usb11_enabled = 1;
}
#endif


/*                                                                         */
#if 0 //               
#ifdef IC_USB
static ssize_t show_start(struct device *dev,struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "start session under IC-USB mode\n");
}
static ssize_t store_start(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	char *pvalue = NULL;
	unsigned int value = 0;
	size_t count = 0;
	u8 devctl = musbfsh_readb((unsigned char __iomem*)USB11_BASE, MUSBFSH_DEVCTL);
	value = simple_strtoul(buf,&pvalue,10);
	count = pvalue - buf;
	
	if (*pvalue && isspace(*pvalue))
		count++;
		
	if(count == size)
	{
		if(value) {			
		WARNING("[IC-USB]start session\n");
		devctl |= MUSBFSH_DEVCTL_SESSION; //                                        
		musbfsh_writeb((unsigned char __iomem*)USB11_BASE, MUSBFSH_DEVCTL, devctl);
		WARNING("[IC-USB]power on VSIM\n");
		hwPowerOn(MT65XX_POWER_LDO_VSIM, VOL_3000, "USB11-SIM");
		}
	}
	return size;
}

static DEVICE_ATTR(start, 0666, show_start, store_start);
#endif
#endif

/*                                                                         */

static inline struct musbfsh *dev_to_musbfsh(struct device *dev)
{
	return hcd_to_musbfsh(dev_get_drvdata(dev));
}


/*                                                                         */
//                              
/*
                          
 */
void musbfsh_write_fifo(struct musbfsh_hw_ep *hw_ep, u16 len, const u8 *src)
{
	void __iomem *fifo = hw_ep->fifo;
	prefetch((u8 *)src);

	INFO("%cX ep%d fifo %p count %d buf %p\n",
			'T', hw_ep->epnum, fifo, len, src);

	/*                                      */
	if (likely((0x01 & (unsigned long) src) == 0)) {
		u16	index = 0;

		/*                                           */
		if ((0x02 & (unsigned long) src) == 0) {
			if (len >= 4) {
//                                         
				iowrite32_rep(fifo, src + index, len >> 2);
				index += len & ~0x03;
			}
			if (len & 0x02) {
				musbfsh_writew(fifo, 0, *(u16 *)&src[index]);
				index += 2;
			}
		} else {
			if (len >= 2) {
//                                         
				iowrite16_rep(fifo, src + index, len >> 1);						
				index += len & ~0x01;
			}
		}
		if (len & 0x01)
			musbfsh_writeb(fifo, 0, src[index]);
	} else  {
		/*              */
//                          
		iowrite8_rep(fifo, src, len);		
	}
    
}

/*
                            
 */
void musbfsh_read_fifo(struct musbfsh_hw_ep *hw_ep, u16 len, u8 *dst)
{
	void __iomem *fifo = hw_ep->fifo;

	INFO( "%cX ep%d fifo %p count %d buf %p\n",
			'R', hw_ep->epnum, fifo, len, dst);

	/*                                       */
	if (likely((0x01 & (unsigned long) dst) == 0)) {
		u16	index = 0;

		/*                                                */
		if ((0x02 & (unsigned long) dst) == 0) {
			if (len >= 4) {
//                                
				ioread32_rep(fifo, dst, len >> 2);
				index = len & ~0x03;
			}
			if (len & 0x02) {
				*(u16 *)&dst[index] = musbfsh_readw(fifo, 0);
				index += 2;
			}
		} else {
			if (len >= 2) {
//                                
				ioread16_rep(fifo, dst, len >> 1);				
				index = len & ~0x01;
			}
		}
		if (len & 0x01)
			dst[index] = musbfsh_readb(fifo, 0);
	} else  {
		/*              */
		//                       
		ioread8_rep(fifo, dst, len);		
	}
}


/*                                                                         */

/*                                                   */
static const u8 musbfsh_test_packet[53] = {
	/*                                   */

	/*             */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	/*             */
	0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
	/*             */
	0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee,
	/*                   */
	0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	/*             */
	0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd,
	/*                  */
	0xfc, 0x7e, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0x7e

	/*                                */
};

void musbfsh_load_testpacket(struct musbfsh *musbfsh)
{
	void __iomem	*regs = musbfsh->endpoints[0].regs;

	musbfsh_ep_select(musbfsh->mregs, 0);//                     
	musbfsh_write_fifo(musbfsh->control_ep,
			sizeof(musbfsh_test_packet), musbfsh_test_packet);
	musbfsh_writew(regs, MUSBFSH_CSR0, MUSBFSH_CSR0_TXPKTRDY);
}

/*                                                                         */
/*
                                                               
                                                        
                                                                
                                                    
  
                               
                                   
                
               
 */

//                       
static irqreturn_t musbfsh_stage0_irq(struct musbfsh *musbfsh, u8 int_usb,
				u8 devctl, u8 power)
{
#if 0
	u8 intusbe = musbfsh_readb(musbfsh->mregs, MUSBFSH_INTRUSBE);
	MYDBG("devctl : %x, power : %x, int_usb : %x, intusbe : %x\n", devctl, power, int_usb, intusbe);
#endif

#ifdef MUSBFSH_DBG_REG
	int i;
	u32 addr[] = {0x60, 0x74, 0x620, 0x630};
	for(i=0 ; i < sizeof(addr)/sizeof(u32); i++)
	{
		WARNING("dbg addr : %x, val : %x\n", addr[i], musbfsh_readl(musbfsh->mregs, addr[i]));
	}
#endif

	irqreturn_t handled = IRQ_NONE;

	/*                                                      
                                                     
                                                         
  */
	if (int_usb & MUSBFSH_INTR_RESUME) {
		handled = IRQ_HANDLED;
		WARNING("RESUME!\n");

		 if (devctl & MUSBFSH_DEVCTL_HM) {
			void __iomem *mbase = musbfsh->mregs;
			/*                                     
                               
   */

			if (power & MUSBFSH_POWER_SUSPENDM) {
				/*          */
				musbfsh->int_usb &= ~MUSBFSH_INTR_SUSPEND;
				WARNING("Spurious SUSPENDM\n");
			}

			power &= ~MUSBFSH_POWER_SUSPENDM;
			musbfsh_writeb(mbase, MUSBFSH_POWER,
					power | MUSBFSH_POWER_RESUME);

			musbfsh->port1_status |=
					(USB_PORT_STAT_C_SUSPEND << 16)
					| MUSBFSH_PORT_STAT_RESUME;
			musbfsh->rh_timer = jiffies
					+ msecs_to_jiffies(20);

			musbfsh->is_active = 1;
			usb_hcd_resume_root_hub(musbfsh_to_hcd(musbfsh));
			}
		} 

	/*                                       */
	if (int_usb & MUSBFSH_INTR_SESSREQ) {
		/*                    */
		void __iomem *mbase = musbfsh->mregs;
		WARNING("SESSION_REQUEST\n");

		/*                                                       
                                                   
                                                      
                               
                           
                                                       
   */

#ifdef CONFIG_MTK_ICUSB_SUPPORT
		if(skip_session_req_attr.value)
		{
			MYDBG("SESSION_REQUEST SKIPPED FOR IC USB\n");
		}
		else
		{
			devctl |= MUSBFSH_DEVCTL_SESSION;
			musbfsh_writeb(mbase, MUSBFSH_DEVCTL, devctl);
			musbfsh->ep0_stage = MUSBFSH_EP0_START;
			musbfsh_set_vbus(musbfsh, 1);
		}
#else
		devctl |= MUSBFSH_DEVCTL_SESSION;
		musbfsh_writeb(mbase, MUSBFSH_DEVCTL, devctl);
		musbfsh->ep0_stage = MUSBFSH_EP0_START;
		musbfsh_set_vbus(musbfsh, 1);
#endif


		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSBFSH_INTR_VBUSERROR) {
		int	ignore = 0;

		/*                                                     
                                                          
                                                        
                                                            
                                           
    
                                                            
                                                            
                                                          
    
                                                          
                                                    
    
                                                         
                                             
   */
		if (musbfsh->vbuserr_retry) {
			void __iomem *mbase = musbfsh->mregs;

			musbfsh->vbuserr_retry--;
			ignore = 1;
			devctl |= MUSBFSH_DEVCTL_SESSION;
			musbfsh_writeb(mbase, MUSBFSH_DEVCTL, devctl);
		} else {
			musbfsh->port1_status |=
				USB_PORT_STAT_OVERCURRENT
				| (USB_PORT_STAT_C_OVERCURRENT << 16);
		}

		ERR("VBUS_ERROR (%02x, %s), retry #%d, port1_status 0x%08x\n",
				devctl,
				({ char *s;
				switch (devctl & MUSBFSH_DEVCTL_VBUS) {
				case 0 << MUSBFSH_DEVCTL_VBUS_SHIFT:
					s = "<SessEnd"; break;
				case 1 << MUSBFSH_DEVCTL_VBUS_SHIFT:
					s = "<AValid"; break;
				case 2 << MUSBFSH_DEVCTL_VBUS_SHIFT:
					s = "<VBusValid"; break;
				/*                                      */
				default:
					s = "VALID"; break;
				}; s; }),
				VBUSERR_RETRY_COUNT - musbfsh->vbuserr_retry,
				musbfsh->port1_status);

		/*                                                  */
		if (!ignore)
			musbfsh_set_vbus(musbfsh, 0);
		handled = IRQ_HANDLED;
	}

	if (int_usb & MUSBFSH_INTR_CONNECT) {
		struct usb_hcd *hcd = musbfsh_to_hcd(musbfsh);

#ifdef CONFIG_MTK_ICUSB_SUPPORT
		set_usb11_sts_connect();
#endif

#ifndef CONFIG_MTK_ICUSB_SUPPORT
#ifndef MTK_DT_USB_SUPPORT
	wake_lock(&musbfsh_suspend_lock);			
#endif
#endif

		handled = IRQ_HANDLED;
		musbfsh->is_active = 1;
		musbfsh->ep0_stage = MUSBFSH_EP0_START;
		musbfsh->port1_status &= ~(USB_PORT_STAT_LOW_SPEED
					|USB_PORT_STAT_HIGH_SPEED
					|USB_PORT_STAT_ENABLE
					);
		musbfsh->port1_status |= USB_PORT_STAT_CONNECTION
					|(USB_PORT_STAT_C_CONNECTION << 16);

		/*                                                      */
		if (devctl & MUSBFSH_DEVCTL_LSDEV)
			musbfsh->port1_status |= USB_PORT_STAT_LOW_SPEED;

		if (hcd->status_urb)
			usb_hcd_poll_rh_status(hcd);
		else
			usb_hcd_resume_root_hub(hcd);


#ifdef CONFIG_MTK_ICUSB_SUPPORT
#define OPSTATE_MODE 0x04
#define OSR_INTE 0x80
#define OSR_START 0x01

		if(hw_dbg_attr.value)
		{
			void __iomem *mbase = musbfsh->mregs;

			/*                    */
			u8 opr_csr = musbfsh_readb(mbase, 0x610);
			opr_csr |= OPSTATE_MODE;
			opr_csr |= OSR_INTE;
			opr_csr |= OSR_START;
			musbfsh_writeb(mbase, 0x610, opr_csr);
		}
		else
		{
			MYDBG("");
		}
#if 0
		WARNING("CONNECT ! devctl 0x%02x, g_usb_clk_reg_before : %x, g_usb_clk_reg_after : %x, 0xF000014C : %x, g_FS_EOF1_before : %x, g_FS_EOF1_after :%x\n", 
				devctl, g_usb_clk_reg_before, g_usb_clk_reg_after, __raw_readb(0xF000014C), g_FS_EOF1_before, g_FS_EOF1_after);
#endif
#endif
		mtk_musbfsh = musbfsh;
#ifdef MTK_DT_USB_SUPPORT
		wake_lock_init(&usb_resume_lock, WAKE_LOCK_SUSPEND, "USB11 wakelock"); //                                      
		register_eint_usb_p1_wakeup_callback();
#endif
		WARNING("CONNECT ! devctl 0x%02x\n", devctl);

	}

	if (int_usb & MUSBFSH_INTR_DISCONNECT) {
		WARNING("DISCONNECT !devctl %02x\n", devctl);
#ifdef CONFIG_MTK_ICUSB_SUPPORT
		if(!check_usb11_sts_disconnect_done()){
			set_usb11_sts_disconnecting();
		}
		mt65xx_usb11_mac_reset_and_phy_stress_set();			
#endif
		handled = IRQ_HANDLED;
		usb_hcd_resume_root_hub(musbfsh_to_hcd(musbfsh));
		musbfsh_root_disconnect(musbfsh);


#ifndef CONFIG_MTK_ICUSB_SUPPORT
#ifndef MTK_DT_USB_SUPPORT
		wake_unlock(&musbfsh_suspend_lock);		
#endif
#endif
	}

	/*                                                             
                                                          
  */
	if (int_usb & MUSBFSH_INTR_BABBLE) {
		handled = IRQ_HANDLED;
		/*
                                                
                                                        
                                                      
                                                     
                     
  */
		
#ifdef CONFIG_MTK_ICUSB_SUPPORT
#define OSR_START 0x01

		if(hw_dbg_attr.value)
		{
			void __iomem *mbase = musbfsh->mregs;
			int dbg_register[] = {0x640, 0x644, 0x648, 0x64C, 0x650, 0x654, 0x658, 0x65C};
			int i;
			u8 op_state; 

			/*                   */ 
			u8 opr_csr = musbfsh_readb(mbase, 0x610);
			opr_csr &= ~OSR_START;
			musbfsh_writeb(mbase, 0x610, opr_csr);

			udelay(100);

			/*             */
			WARNING("BABBLE!, dump dbg register\n"); 

			for (i=0; i<sizeof(dbg_register)/sizeof(int); i++) {
				printk("offset=%x, value=%x\n", (unsigned int)dbg_register[i], (unsigned int)musbfsh_readl(musbfsh->mregs, (unsigned)dbg_register[i]));
			}

			op_state = (u8)musbfsh_readb(mbase, (unsigned)0x620);
			WARNING("BABBLE!, op_state : %x\n", (unsigned int)op_state);
		}
		else
		{
			MYDBG("");
		}
#endif
#if defined(MTK_ICUSB_BABBLE_RECOVER) || defined(MTK_SMARTBOOK_SUPPORT)
		/*                                                 */
		mt65xx_usb11_mac_phy_babble_clear(musbfsh);

		/*                               */
		//                                                    

		WARNING("BABBLE! schedule babble_recover\n");

		usb_hcd_resume_root_hub(musbfsh_to_hcd(musbfsh));
		musbfsh_root_disconnect(musbfsh);

		mod_timer(&babble_recover_timer, jiffies + msecs_to_jiffies(5000));
#else
		if (devctl & (MUSBFSH_DEVCTL_FSDEV | MUSBFSH_DEVCTL_LSDEV)) {
			ERR("BABBLE devctl: %02x\n", devctl);
			//                                                       
		} else {
			ERR("Stopping host session -- babble, devctl : %x\n", devctl);
			musbfsh_writeb(musbfsh->mregs, MUSBFSH_DEVCTL, 0);							
		}			
#endif
	}
	
	return handled;
}


/*
                                                           
*/
void musbfsh_start(struct musbfsh *musbfsh)
{
	void __iomem	*regs = musbfsh->mregs;//                       
	u8		devctl = musbfsh_readb(regs, MUSBFSH_DEVCTL);
	u8		power = musbfsh_readb(regs, MUSBFSH_POWER);
	int int_level1 = 0;
	WARNING("<== devctl 0x%02x\n", devctl);

#ifdef CONFIG_MTK_ICUSB_SUPPORT
#if 0
	//                                         
	unsigned char result = musbfsh_readb(regs, 0x74);
	result &= ~(0x40);
	musbfsh_writeb(regs, 0x74, result);

	//                                   
	g_FS_EOF1_before = musbfsh_readb(regs, 0x7D);
	musbfsh_writeb(regs, 0x7D, 0xB4);
	g_FS_EOF1_after = musbfsh_readb(regs, 0x7D);
#endif
#endif


	/*                                              */
	musbfsh_writew(regs, MUSBFSH_INTRTXE, musbfsh->epmask);
	musbfsh_writew(regs, MUSBFSH_INTRRXE, musbfsh->epmask & 0xfffe);

	musbfsh_writeb(regs, MUSBFSH_INTRUSBE, 0xf7);

	/*                           */
	musbfsh_writew(regs, USB11_L1INTM, 0x000f);
	int_level1 = musbfsh_readw(musbfsh->mregs, USB11_L1INTM);
	INFO("Level 1 Interrupt Mask 0x%x\n", int_level1);
	int_level1 = musbfsh_readw(musbfsh->mregs, USB11_L1INTP);
	INFO("Level 1 Interrupt Polarity 0x%x\n", int_level1);
	
	/*                          */
	musbfsh_writew(regs, MUSBFSH_INTRTX, 0xffff);
	musbfsh_writew(regs, MUSBFSH_INTRRX, 0xffff);
	musbfsh_writeb(regs, MUSBFSH_INTRUSB, 0xff);
	musbfsh_writeb(regs, MUSBFSH_HSDMA_INTR, 0xff);

	musbfsh->is_active = 0;
	musbfsh->is_multipoint = 1;

	/*                         */
#ifndef MTK_DT_USB_SUPPORT
	musbfsh_set_vbus(musbfsh, 1);
#endif
	musbfsh_platform_enable(musbfsh);

#if 0	//                
#ifndef IC_USB
	/*                                                      */
	devctl |= MUSBFSH_DEVCTL_SESSION; //                                        
	musbfsh_writeb(regs, MUSBFSH_DEVCTL, devctl);
#endif
#endif

#ifdef CONFIG_MTK_ICUSB_SUPPORT
	if(skip_enable_session_attr.value)
	{
		MYDBG("SESSION ENABLE SKIPPED FOR IC USB\n");
	}
	else
	{
		/*                                                      */
		devctl |= MUSBFSH_DEVCTL_SESSION; //                                        
		musbfsh_writeb(regs, MUSBFSH_DEVCTL, devctl);
	}
#else
	/*                                                      */
	devctl |= MUSBFSH_DEVCTL_SESSION; //                                        
	musbfsh_writeb(regs, MUSBFSH_DEVCTL, devctl);
#endif
	
	/*                              */
	power |= MUSBFSH_POWER_HSENAB;
	power |= MUSBFSH_POWER_SOFTCONN;
	//                                                                     
	/*                                                                                            
                                                                                 */
	power |= MUSBFSH_POWER_ENSUSPEND;
	musbfsh_writeb(regs, MUSBFSH_POWER, power);

#ifdef MUSBFSH_DBG_REG
	/*                         */
	u32 tmp = musbfsh_readl(regs, 0x74);
	WARNING("tmp :%x, 1\n", tmp);
	tmp &= ~(1<<6);
	WARNING("tmp :%x, 2\n", tmp);
	musbfsh_writeb(regs, 0x74, tmp);
	tmp = musbfsh_readl(regs, 0x74);
	WARNING("tmp :%x, 3\n", tmp);

	int i;
	u32 addr[] = {0x60, 0x74, 0x620, 0x630};
	for(i=0 ; i < sizeof(addr)/sizeof(u32); i++)
	{
		WARNING("dbg addr : %x, val : %x\n", addr[i], musbfsh_readl(regs, addr[i]));
	}
#endif


#ifndef CONFIG_MTK_ICUSB_SUPPORT
	devctl = musbfsh_readb(regs, MUSBFSH_DEVCTL);
	power = musbfsh_readb(regs, MUSBFSH_POWER);
	WARNING(" musb ready. devctl=0x%x, power=0x%x\n", devctl, power);
	mdelay(500); //    
#endif

}


static void musbfsh_generic_disable(struct musbfsh *musbfsh)
{
	void __iomem	*mbase = musbfsh->mregs;
	u16	temp;
	INFO("musbfsh_generic_disable++\r\n");
	/*                    */
	musbfsh_writeb(mbase, MUSBFSH_INTRUSBE, 0);
	musbfsh_writew(mbase, MUSBFSH_INTRTXE, 0);
	musbfsh_writew(mbase, MUSBFSH_INTRRXE, 0);

	/*     */
	musbfsh_writeb(mbase, MUSBFSH_DEVCTL, 0);

	/*                           */
	temp = musbfsh_readb(mbase, MUSBFSH_INTRUSB);
	temp = musbfsh_readw(mbase, MUSBFSH_INTRTX);
	temp = musbfsh_readw(mbase, MUSBFSH_INTRRX);
}


/*
                                                 
                              
                                     
                                       
                                                        
 */
void musbfsh_stop(struct musbfsh *musbfsh)
{
	/*                        */
	musbfsh_platform_disable(musbfsh);
	musbfsh_generic_disable(musbfsh);
	INFO( "HDRC disabled\n");

	/*      
                                                            
                                               
                                                                  
                                                                
          
  */
	musbfsh_platform_try_idle(musbfsh, 0);
}

static void musbfsh_shutdown(struct platform_device *pdev)
{
	struct musbfsh	*musbfsh = dev_to_musbfsh(&pdev->dev);
	unsigned long	flags;
    INFO("musbfsh_shutdown++\r\n");
	spin_lock_irqsave(&musbfsh->lock, flags);
	musbfsh_platform_disable(musbfsh);
	musbfsh_generic_disable(musbfsh);
	musbfsh_set_vbus(musbfsh,0);
	musbfsh_set_power(musbfsh, 0);
	spin_unlock_irqrestore(&musbfsh->lock, flags);

	/*                  */
}


/*                                                                         */
/*
                                                              
                                                          
 */

/*             */
#define MAXFIFOSIZE 8096

static struct musbfsh_fifo_cfg __initdata epx_cfg[] = {
{ .hw_ep_num =  1, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  1, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  2, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  2, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  3, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  3, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  4, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  4, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  5, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =	5, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
};

static struct musbfsh_fifo_cfg __initdata ep0_cfg = {
	.style = FIFO_RXTX, .maxpacket = 64,
};

#ifdef CONFIG_MTK_ICUSB_SUPPORT
static struct musbfsh_fifo_cfg ep0_cfg1 = {
	.style = FIFO_RXTX, .maxpacket = 64,
};
static struct musbfsh_fifo_cfg epx_cfg1[] = {
{ .hw_ep_num =  1, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  1, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  2, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  2, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  3, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  3, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  4, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  4, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =  5, .style = FIFO_TX,   .maxpacket = 512, .mode = BUF_SINGLE},
{ .hw_ep_num =	5, .style = FIFO_RX,   .maxpacket = 512, .mode = BUF_SINGLE},
};
/*                                                                         */

static int 
fifo_setup1(struct musbfsh *musbfsh, struct musbfsh_hw_ep  *hw_ep,
		const struct musbfsh_fifo_cfg *cfg, u16 offset)
{
	void __iomem	*mbase = musbfsh->mregs;
	int	size = 0;
	u16	maxpacket = cfg->maxpacket;
	u16	c_off = offset >> 3;
	u8	c_size;//                                      
	INFO("musbfsh::fifo_setup++,hw_ep->epnum=%d,cfg->hw_ep_num=%d\r\n",hw_ep->epnum,cfg->hw_ep_num);
	/*                                                */

	size = ffs(max(maxpacket, (u16) 8)) - 1;
	maxpacket = 1 << size;

	c_size = size - 3;
	if (cfg->mode == BUF_DOUBLE) {
		if ((offset + (maxpacket << 1)) >
				MAXFIFOSIZE)
			return -EMSGSIZE;
		c_size |= MUSBFSH_FIFOSZ_DPB;
	} else {
		if ((offset + maxpacket) > MAXFIFOSIZE)
			return -EMSGSIZE;
	}

	/*                    */
	musbfsh_writeb(mbase, MUSBFSH_INDEX, hw_ep->epnum);
	/*                                                  
                                                   
  */
	 if (hw_ep->epnum == 1)
		musbfsh->bulk_ep = hw_ep;
	/*                                                          */
	switch (cfg->style) {
	case FIFO_TX:
		musbfsh_write_txfifosz(mbase, c_size);
		musbfsh_write_txfifoadd(mbase, c_off);
		hw_ep->tx_double_buffered = !!(c_size & MUSBFSH_FIFOSZ_DPB);
		hw_ep->max_packet_sz_tx = maxpacket;
		break;
	case FIFO_RX:
		musbfsh_write_rxfifosz(mbase, c_size);
		musbfsh_write_rxfifoadd(mbase, c_off);
		hw_ep->rx_double_buffered = !!(c_size & MUSBFSH_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;
		break;
	case FIFO_RXTX:
		musbfsh_write_txfifosz(mbase, c_size);
		musbfsh_write_txfifoadd(mbase, c_off);
		hw_ep->rx_double_buffered = !!(c_size & MUSBFSH_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;

		musbfsh_write_rxfifosz(mbase, c_size);
		musbfsh_write_rxfifoadd(mbase, c_off);
		hw_ep->tx_double_buffered = hw_ep->rx_double_buffered;
		hw_ep->max_packet_sz_tx = maxpacket;
		hw_ep->is_shared_fifo = true;
		break;
	}

	/*                                                        
                          
  */
	musbfsh->epmask |= (1 << hw_ep->epnum);

	return offset + (maxpacket << ((c_size & MUSBFSH_FIFOSZ_DPB) ? 1 : 0));
}
#endif
/*
                                                                 
                                              
  
                                                  
 */
static int __init
fifo_setup(struct musbfsh *musbfsh, struct musbfsh_hw_ep  *hw_ep,
		const struct musbfsh_fifo_cfg *cfg, u16 offset)
{
	void __iomem	*mbase = musbfsh->mregs;
	int	size = 0;
	u16	maxpacket = cfg->maxpacket;
	u16	c_off = offset >> 3;
	u8	c_size;//                                      
	INFO("musbfsh::fifo_setup++,hw_ep->epnum=%d,cfg->hw_ep_num=%d\r\n",hw_ep->epnum,cfg->hw_ep_num);
	/*                                                */

	size = ffs(max(maxpacket, (u16) 8)) - 1;
	maxpacket = 1 << size;

	c_size = size - 3;
	if (cfg->mode == BUF_DOUBLE) {
		if ((offset + (maxpacket << 1)) >
				MAXFIFOSIZE)
			return -EMSGSIZE;
		c_size |= MUSBFSH_FIFOSZ_DPB;
	} else {
		if ((offset + maxpacket) > MAXFIFOSIZE)
			return -EMSGSIZE;
	}

	/*                    */
	musbfsh_writeb(mbase, MUSBFSH_INDEX, hw_ep->epnum);
	/*                                                  
                                                   
  */
	 if (hw_ep->epnum == 1)
		musbfsh->bulk_ep = hw_ep;
	/*                                                          */
	switch (cfg->style) {
	case FIFO_TX:
		musbfsh_write_txfifosz(mbase, c_size);
		musbfsh_write_txfifoadd(mbase, c_off);
		hw_ep->tx_double_buffered = !!(c_size & MUSBFSH_FIFOSZ_DPB);
		hw_ep->max_packet_sz_tx = maxpacket;
		break;
	case FIFO_RX:
		musbfsh_write_rxfifosz(mbase, c_size);
		musbfsh_write_rxfifoadd(mbase, c_off);
		hw_ep->rx_double_buffered = !!(c_size & MUSBFSH_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;
		break;
	case FIFO_RXTX:
		musbfsh_write_txfifosz(mbase, c_size);
		musbfsh_write_txfifoadd(mbase, c_off);
		hw_ep->rx_double_buffered = !!(c_size & MUSBFSH_FIFOSZ_DPB);
		hw_ep->max_packet_sz_rx = maxpacket;

		musbfsh_write_rxfifosz(mbase, c_size);
		musbfsh_write_rxfifoadd(mbase, c_off);
		hw_ep->tx_double_buffered = hw_ep->rx_double_buffered;
		hw_ep->max_packet_sz_tx = maxpacket;
		hw_ep->is_shared_fifo = true;
		break;
	}

	/*                                                        
                          
  */
	musbfsh->epmask |= (1 << hw_ep->epnum);

	return offset + (maxpacket << ((c_size & MUSBFSH_FIFOSZ_DPB) ? 1 : 0));
}
#ifdef CONFIG_MTK_ICUSB_SUPPORT
static int ep_config_from_table1(struct musbfsh *musbfsh)
{
	const struct musbfsh_fifo_cfg	*cfg = NULL;
	unsigned		i = 0;
	unsigned		n = 0;
	int			offset;
	struct musbfsh_hw_ep	*hw_ep = musbfsh->endpoints;
	INFO("musbfsh::ep_config_from_table++\r\n");

	/*                       */
	musbfsh->config->fifo_cfg = epx_cfg1;
	musbfsh->config->fifo_cfg_size = sizeof(epx_cfg1)/sizeof(struct musbfsh_fifo_cfg);

	if (musbfsh->config->fifo_cfg) {
		cfg = musbfsh->config->fifo_cfg;
		n = musbfsh->config->fifo_cfg_size;
		INFO("musbfsh:fifo_cfg, n=%d\n",n);
		goto done;
	}

done:
	offset = fifo_setup1(musbfsh, hw_ep, &ep0_cfg1, 0);
	/*                    */

	/*                                                          
                                                                       
  */
	
	for (i = 0; i < n; i++) {
        u8 epn = cfg->hw_ep_num;
        
		if (epn >= MUSBFSH_C_NUM_EPS) {
			ERR("%s: invalid ep %d\n",musbfsh_driver_name, epn);
			return -EINVAL;
		}
		offset = fifo_setup1(musbfsh, hw_ep + epn, cfg++, offset);
		if (offset < 0) {
			ERR("%s: mem overrun, ep %d\n",musbfsh_driver_name, epn);
			return -EINVAL;
		}
        
		epn++;//           
		musbfsh->nr_endpoints = max(epn, musbfsh->nr_endpoints);
	}
	INFO("%s: %d/%d max ep, %d/%d memory\n",
			musbfsh_driver_name,
			n + 1, musbfsh->config->num_eps * 2 - 1,
			offset, MAXFIFOSIZE);
	
	if (!musbfsh->bulk_ep) {
		ERR("%s: missing bulk\n", musbfsh_driver_name);
		return -EINVAL;
	}
	return 0;
}
#endif

static int __init ep_config_from_table(struct musbfsh *musbfsh)
{
	const struct musbfsh_fifo_cfg	*cfg = NULL;
	unsigned		i = 0;
	unsigned		n = 0;
	int			offset;
	struct musbfsh_hw_ep	*hw_ep = musbfsh->endpoints;
	INFO("musbfsh::ep_config_from_table++\r\n");
	if (musbfsh->config->fifo_cfg) {
		cfg = musbfsh->config->fifo_cfg;
		n = musbfsh->config->fifo_cfg_size;
		INFO("musbfsh:fifo_cfg, n=%d\n",n);
		goto done;
	}

done:
	offset = fifo_setup(musbfsh, hw_ep, &ep0_cfg, 0);
	/*                    */

	/*                                                          
                                                                       
  */
	
	for (i = 0; i < n; i++) {
        u8 epn = cfg->hw_ep_num;
        
		if (epn >= MUSBFSH_C_NUM_EPS) {
			ERR("%s: invalid ep %d\n",musbfsh_driver_name, epn);
			return -EINVAL;
		}
		offset = fifo_setup(musbfsh, hw_ep + epn, cfg++, offset);
		if (offset < 0) {
			ERR("%s: mem overrun, ep %d\n",musbfsh_driver_name, epn);
			return -EINVAL;
		}
        
		epn++;//           
		musbfsh->nr_endpoints = max(epn, musbfsh->nr_endpoints);
	}
	INFO("%s: %d/%d max ep, %d/%d memory\n",
			musbfsh_driver_name,
			n + 1, musbfsh->config->num_eps * 2 - 1,
			offset, MAXFIFOSIZE);
	
	if (!musbfsh->bulk_ep) {
		ERR("%s: missing bulk\n", musbfsh_driver_name);
		return -EINVAL;
	}
	return 0;
}
#ifdef CONFIG_MTK_ICUSB_SUPPORT
static int musbfsh_core_init1(struct musbfsh *musbfsh)
{
	void __iomem	*mbase = musbfsh->mregs;
	int		status = 0;
	int		i;
    INFO("musbfsh_core_init\r\n");
	/*               */
	musbfsh_configure_ep0(musbfsh);

	/*                                 */
	musbfsh->nr_endpoints = 1;//                                         
	musbfsh->epmask = 1;
	
	status = ep_config_from_table1(musbfsh);
	if (status < 0)
		return status;

	/*                                        */
	for (i = 0; i < musbfsh->nr_endpoints; i++) {
		struct musbfsh_hw_ep	*hw_ep = musbfsh->endpoints + i;

		hw_ep->fifo = MUSBFSH_FIFO_OFFSET(i) + mbase;
		hw_ep->regs = MUSBFSH_EP_OFFSET(i, 0) + mbase;
		hw_ep->rx_reinit = 1;
		hw_ep->tx_reinit = 1;

		if (hw_ep->max_packet_sz_tx) {
			INFO("%s: hw_ep %d%s, %smax %d,and hw_ep->epnum=%d\n",
				musbfsh_driver_name, i,
				hw_ep->is_shared_fifo ? "shared" : "tx",
				hw_ep->tx_double_buffered
					? "doublebuffer, " : "",
				hw_ep->max_packet_sz_tx,hw_ep->epnum);
		}
		if (hw_ep->max_packet_sz_rx && !hw_ep->is_shared_fifo) {
			INFO("%s: hw_ep %d%s, %smax %d,and hw_ep->epnum=%d\n",
				musbfsh_driver_name, i,
				"rx",
				hw_ep->rx_double_buffered
					? "doublebuffer, " : "",
				hw_ep->max_packet_sz_rx,hw_ep->epnum);
		}
		if (!(hw_ep->max_packet_sz_tx || hw_ep->max_packet_sz_rx))
			INFO("hw_ep %d not configured\n", i);
	}
	return 0;
}
#endif


/*                                                            
                                                         
 */
static int __init musbfsh_core_init(struct musbfsh *musbfsh)
{
	void __iomem	*mbase = musbfsh->mregs;
	int		status = 0;
	int		i;
    INFO("musbfsh_core_init\r\n");
	/*               */
	musbfsh_configure_ep0(musbfsh);

	/*                                 */
	musbfsh->nr_endpoints = 1;//                                         
	musbfsh->epmask = 1;
	
	status = ep_config_from_table(musbfsh);
	if (status < 0)
		return status;

	/*                                        */
	for (i = 0; i < musbfsh->nr_endpoints; i++) {
		struct musbfsh_hw_ep	*hw_ep = musbfsh->endpoints + i;

		hw_ep->fifo = MUSBFSH_FIFO_OFFSET(i) + mbase;
		hw_ep->regs = MUSBFSH_EP_OFFSET(i, 0) + mbase;
		hw_ep->rx_reinit = 1;
		hw_ep->tx_reinit = 1;

		if (hw_ep->max_packet_sz_tx) {
			INFO("%s: hw_ep %d%s, %smax %d,and hw_ep->epnum=%d\n",
				musbfsh_driver_name, i,
				hw_ep->is_shared_fifo ? "shared" : "tx",
				hw_ep->tx_double_buffered
					? "doublebuffer, " : "",
				hw_ep->max_packet_sz_tx,hw_ep->epnum);
		}
		if (hw_ep->max_packet_sz_rx && !hw_ep->is_shared_fifo) {
			INFO("%s: hw_ep %d%s, %smax %d,and hw_ep->epnum=%d\n",
				musbfsh_driver_name, i,
				"rx",
				hw_ep->rx_double_buffered
					? "doublebuffer, " : "",
				hw_ep->max_packet_sz_rx,hw_ep->epnum);
		}
		if (!(hw_ep->max_packet_sz_tx || hw_ep->max_packet_sz_rx))
			INFO("hw_ep %d not configured\n", i);
	}
	return 0;
}

/*                                                                         */
void musbfsh_read_clear_generic_interrupt(struct musbfsh *musbfsh)
{
	musbfsh->int_usb = musbfsh_readb(musbfsh->mregs, MUSBFSH_INTRUSB);
	musbfsh->int_tx = musbfsh_readw(musbfsh->mregs, MUSBFSH_INTRTX);
	musbfsh->int_rx = musbfsh_readw(musbfsh->mregs, MUSBFSH_INTRRX);
	musbfsh->int_dma =  musbfsh_readb(musbfsh->mregs, MUSBFSH_HSDMA_INTR);
	INFO( "** musbfsh::IRQ! usb%04x tx%04x rx%04x dma%04x\n",
		musbfsh->int_usb, musbfsh->int_tx, musbfsh->int_rx, musbfsh->int_dma);
	/*                        */	
	musbfsh_writew(musbfsh->mregs, MUSBFSH_INTRTX, musbfsh->int_tx);
	musbfsh_writew(musbfsh->mregs, MUSBFSH_INTRRX, musbfsh->int_rx);
	musbfsh_writeb(musbfsh->mregs, MUSBFSH_INTRUSB, musbfsh->int_usb);
	musbfsh_writeb(musbfsh->mregs, MUSBFSH_HSDMA_INTR, musbfsh->int_dma);
}
void musbfsh_read_clear_generic_interrupt_tmp(struct musbfsh *musbfsh)
{
	int i = 0;

	while(i++ < 5){
		musbfsh->int_usb = musbfsh_readb(musbfsh->mregs, MUSBFSH_INTRUSB);
		musbfsh->int_tx = musbfsh_readw(musbfsh->mregs, MUSBFSH_INTRTX);
		musbfsh->int_rx = musbfsh_readw(musbfsh->mregs, MUSBFSH_INTRRX);
		musbfsh->int_dma =  musbfsh_readb(musbfsh->mregs, MUSBFSH_HSDMA_INTR);
		WARNING( "** musbfsh::IRQ! usb%04x tx%04x rx%04x dma%04x\n",
				musbfsh->int_usb, musbfsh->int_tx, musbfsh->int_rx, musbfsh->int_dma);
		/*                        */	
		musbfsh_writew(musbfsh->mregs, MUSBFSH_INTRTX, musbfsh->int_tx);
		musbfsh_writew(musbfsh->mregs, MUSBFSH_INTRRX, musbfsh->int_rx);
		musbfsh_writeb(musbfsh->mregs, MUSBFSH_INTRUSB, musbfsh->int_usb);
		musbfsh_writeb(musbfsh->mregs, MUSBFSH_HSDMA_INTR, musbfsh->int_dma);
	}
}
	
static irqreturn_t generic_interrupt(int irq, void *__hci)
{
	unsigned long	flags;
	irqreturn_t	retval = IRQ_NONE;
	struct musbfsh	*musbfsh = __hci;
	u16 int_level1 = 0;
	INFO("musbfsh:generic_interrupt++\r\n");
#ifdef CONFIG_MTK_ICUSB_SUPPORT
#if 0
	if(mt65xx_check_usb11_clk_status())
	{
		dump_stack();
		BUG();
	}
#endif	
#endif	
	spin_lock_irqsave(&musbfsh->lock, flags);

	musbfsh_read_clear_generic_interrupt(musbfsh);	
	int_level1 = musbfsh_readw(musbfsh->mregs, USB11_L1INTS);
	INFO("Level 1 Interrupt Status 0x%x\n", int_level1);
	
	if (musbfsh->int_usb || musbfsh->int_tx || musbfsh->int_rx)
		retval = musbfsh_interrupt(musbfsh);
	if(musbfsh->int_dma) 
		retval = musbfsh_dma_controller_irq(irq, musbfsh->musbfsh_dma_controller);
		
	spin_unlock_irqrestore(&musbfsh->lock, flags);
	return retval;
}

/*
                                                                          
                                                                           
                                                              
  
                                                         
 */
irqreturn_t musbfsh_interrupt(struct musbfsh *musbfsh)
{
	irqreturn_t	retval = IRQ_NONE;
	u8		devctl, power;
	int		ep_num;
	u32		reg;

	devctl = musbfsh_readb(musbfsh->mregs, MUSBFSH_DEVCTL);
	power = musbfsh_readb(musbfsh->mregs, MUSBFSH_POWER);

	INFO( "** musbfsh::devctl 0x%x power 0x%x\n", devctl, power);

	/*                                                          
                                           
  */
	if (musbfsh->int_usb)
	{
		retval |= musbfsh_stage0_irq(musbfsh, musbfsh->int_usb,
				devctl, power);
	}

	/*                                     */

	/*                         */
	if (musbfsh->int_tx & 1) {
		retval |= musbfsh_h_ep0_irq(musbfsh);
	}

	/*                      */
	reg = musbfsh->int_rx >> 1;
	ep_num = 1;
	while (reg) {
		if (reg & 1) {
			/*                                            */
			/*                                       */
			retval = IRQ_HANDLED;
			musbfsh_host_rx(musbfsh, ep_num);//               
		}

		reg >>= 1;
		ep_num++;
	}

	/*                      */
	reg = musbfsh->int_tx >> 1;
	ep_num = 1;
	while (reg) {
		if (reg & 1) {
			/*                                            */
			/*                                        */
			retval = IRQ_HANDLED;
			musbfsh_host_tx(musbfsh, ep_num);
		}
		reg >>= 1;
		ep_num++;
	}
	return retval;
}


#ifndef CONFIG_MUSBFSH_PIO_ONLY
static bool __initdata use_dma = 1;

/*                              */
module_param(use_dma, bool, 0);
MODULE_PARM_DESC(use_dma, "enable/disable use of DMA");

#else
#define use_dma			0
#endif

void musbfsh_dma_completion(struct musbfsh *musbfsh, u8 epnum, u8 transmit)
{
    INFO("musbfsh_dma_completion++\r\n");
	/*                                          */

	/*                 */
	if (transmit) 
		musbfsh_host_tx(musbfsh, epnum);
	else 
		musbfsh_host_rx(musbfsh, epnum);
}

/*                                                                           
               
 */

static struct musbfsh *__init
allocate_instance(struct device *dev,
		struct musbfsh_hdrc_config *config, void __iomem *mbase)
{
	struct musbfsh		*musbfsh;
	struct musbfsh_hw_ep	*ep;
	int			epnum;
	struct usb_hcd	*hcd;
	INFO("musbfsh::allocate_instance++\r\n");
	hcd = usb_create_hcd(&musbfsh_hc_driver, dev, dev_name(dev));
	if (!hcd)
		return NULL;
	/*                                                                  */

	musbfsh = hcd_to_musbfsh(hcd);
	INIT_LIST_HEAD(&musbfsh->control);
	INIT_LIST_HEAD(&musbfsh->in_bulk);
	INIT_LIST_HEAD(&musbfsh->out_bulk);

	hcd->uses_new_polling = 1;
	hcd->has_tt = 1;

	musbfsh->vbuserr_retry = VBUSERR_RETRY_COUNT;

	musbfsh->mregs = mbase;
	musbfsh->ctrl_base = mbase;
	musbfsh->nIrq = -ENODEV;//                                          
	musbfsh->config = config;
	BUG_ON(musbfsh->config->num_eps > MUSBFSH_C_NUM_EPS);
	for (epnum = 0, ep = musbfsh->endpoints;
			epnum < musbfsh->config->num_eps;
			epnum++, ep++) {
		ep->musbfsh = musbfsh;
		ep->epnum = epnum;
	}

	musbfsh->controller = dev;
	return musbfsh;
}
	
static void musbfsh_free(struct musbfsh *musbfsh)
{
	/*                                                              
                                                                   
                                                 
  */
    INFO("musbfsh_free++\r\n");
	if (musbfsh->nIrq >= 0) {
		if (musbfsh->irq_wake)
			disable_irq_wake(musbfsh->nIrq);
		free_irq(musbfsh->nIrq, musbfsh);
	}
	if (is_dma_capable() && musbfsh->dma_controller) {
		struct dma_controller	*c = musbfsh->dma_controller;

		(void) c->stop(c);
		musbfsh_dma_controller_destroy(c);
	}
	usb_put_hcd(musbfsh_to_hcd(musbfsh));
	kfree(musbfsh);
}

/*
                                                 
  
                                                  
             
                                                   
                                                  
 */
static int __init
musbfsh_init_controller(struct device *dev, int nIrq, void __iomem *ctrl,void __iomem *pbase)
{
	int			status;
	struct musbfsh		*musbfsh;
	struct musbfsh_hdrc_platform_data *plat = dev->platform_data;
	struct usb_hcd	*hcd;
	
#if 0
    struct musbfsh_hdrc_config		*config;
    struct device_node		*np = dev->of_node;
#endif
	
	WARNING("musbfsh::musbfsh_init_controller++\r\n");
	/*                                                          
                                                           
  */

#if 0
	plat = devm_kzalloc(dev, sizeof(*plat), GFP_KERNEL);
	if (!plat) {
		dev_err(dev, "failed to allocate musb platfrom data\n");
		goto fail0;
	}
		
	config = devm_kzalloc(dev, sizeof(*config), GFP_KERNEL);
	if (!config) {
		dev_err(dev, "failed to allocate musb hdrc config\n");
		goto fail0;
	}
		
	of_property_read_u32(np, "mode", (u32 *)&plat->mode);
		
//                                                                                          
	of_property_read_u32(np, "num_eps", (u32 *)&config->num_eps);
	config->multipoint = of_property_read_bool(np, "multipoint");
//                                                                          
//                                                                          
//                                                               
		
	plat->config	   = config;

#endif

	dev->platform_data = plat;
	if (!plat) {
		dev_dbg(dev, "no platform_data?\n");
		status = -ENODEV;
		goto fail0;
	}

	/*          */
	musbfsh = allocate_instance(dev, plat->config, ctrl);
	if (!musbfsh) {
		status = -ENOMEM;
		goto fail0;
	}
	spin_lock_init(&musbfsh->lock);
	musbfsh->phy_base = pbase;	
	musbfsh->board_mode = plat->mode;
	musbfsh->config->fifo_cfg = epx_cfg;
	musbfsh->config->fifo_cfg_size = sizeof(epx_cfg)/sizeof(struct musbfsh_fifo_cfg);
	/*                                  
                                                          
                                               
                                                                    
                         
                           
                                                               
   
                                                            
                                                             
                                                              
                                                          
  */
	musbfsh->isr = generic_interrupt;
	status = musbfsh_platform_init(musbfsh);
	if (status < 0){
        ERR("musbfsh_platform_init fail!status=%d",status);
        goto fail1;
    }

	if (!musbfsh->isr) {
		status = -ENODEV;
		goto fail2;
	}
	
#ifndef CONFIG_MUSBFSH_PIO_ONLY
	INFO("DMA mode\n");
	if (use_dma && dev->dma_mask) {
		struct dma_controller	*c;

		c = musbfsh_dma_controller_create(musbfsh, musbfsh->mregs);//                    
		musbfsh->dma_controller = c;
		if (c)
			(void) c->start(c);//                  
	}
#else
	INFO("PIO mode\n");
#endif

	/*                                                    */
	if (!is_dma_capable() || !musbfsh->dma_controller)
		dev->dma_mask = NULL;

	/*                                                       */
	musbfsh_platform_disable(musbfsh);//                                               
	
#ifdef CONFIG_MTK_ICUSB_SUPPORT
	if(skip_mac_init_attr.value)
	{
		MYDBG("skip musbfsh_generic_disable() and musbfsh_core_init()\n");
	}
	else
	{
		musbfsh_generic_disable(musbfsh);//                            

		/*                                                     */
		status = musbfsh_core_init(musbfsh);
		if (status < 0){
			ERR("musbfsh_core_init fail!");
			goto fail2;
		}
	}
#else
	musbfsh_generic_disable(musbfsh);//                            

	/*                                                     */
	status = musbfsh_core_init(musbfsh);
	if (status < 0){
		ERR("musbfsh_core_init fail!");
		goto fail2;
	}
#endif

	/*                   */
	if (request_irq(nIrq, musbfsh->isr, IRQF_TRIGGER_LOW, dev_name(dev), musbfsh)) { //                                                                      
		dev_err(dev, "musbfsh::request_irq %d failed!\n", nIrq);
		status = -ENODEV;
		goto fail2;
	}
	musbfsh->nIrq = nIrq; //                                            
/*                                      */
	if (enable_irq_wake(nIrq) == 0) { //                                                             
		musbfsh->irq_wake = 1;
		device_init_wakeup(dev, 1); //                                    
	} else {
		musbfsh->irq_wake = 0;
	}

	/*                            */
	hcd = musbfsh_to_hcd(musbfsh);
	hcd->power_budget = 2 * (plat->power ?plat->power : 250);//                                                          

	/*                                                    
                                                    
                                                    
  */
	status = usb_add_hcd(musbfsh_to_hcd(musbfsh), -1, 0);//           

	if (status < 0){
         ERR("musbfsh::usb_add_hcd fail!");
		goto fail2;
       }

	dev_info(dev, "USB controller at %p using %s, IRQ %d\n",
			ctrl,
			(is_dma_capable() && musbfsh->dma_controller)
			? "DMA" : "PIO",
			musbfsh->nIrq);

#if defined(MTK_ICUSB_BABBLE_RECOVER) || defined(MTK_SMARTBOOK_SUPPORT)
	init_timer(&babble_recover_timer);
	babble_recover_timer.function = babble_recover_func;
	babble_recover_timer.data = (unsigned long) musbfsh;
#endif

    g_musbfsh = musbfsh;
#ifdef CONFIG_MUSBFSH_DEBUG_FS 
	status = musbfsh_init_debugfs(musbfsh);
	if (status < 0)
		goto fail2;
	create_musbfsh_cmd_proc_entry();	
#endif

#ifdef CONFIG_MTK_ICUSB_SUPPORT
	set_usb_phy_voltage(VOL_50);
	mt65xx_usb11_phy_savecurrent();
	mt65xx_usb11_disable_clk_pll_pw();
	create_ic_usb_cmd_proc_entry();
	
#ifdef MTK_ICUSB_TAKE_WAKE_LOCK
	wake_lock(&musbfsh_suspend_lock);			
#else
	MYDBG("skip wake_lock(&musbfsh_suspend_lock)\n");
#endif

	MYDBG("end of %s(), build time : %s\n", __func__, __TIME__);

#endif
#ifdef MTK_DT_USB_SUPPORT
	get_eint_ext_usb_wkup_irq();

	/*                              */
	if(musbfsh_wake_lock_hold){
		wake_lock(&musbfsh_suspend_lock);		
		MYDBG("force wake_lock(&musbfsh_suspend_lock)\n");
	}
	create_dsda_tmp_entry();
#endif	


	return 0;


fail2:
	if (musbfsh->irq_wake)
		device_init_wakeup(dev, 0);
	musbfsh_platform_exit(musbfsh);

fail1:
	dev_err(musbfsh->controller,
		"musbfsh_init_controller failed with status %d\n", status);
	musbfsh_free(musbfsh);

fail0:

	return status;

}

/*                                                                         */

/*                                                                 
                                                          
 */

#ifndef CONFIG_MUSBFSH_PIO_ONLY
static u64	*orig_dma_mask;
#endif

static int __init musbfsh_probe(struct platform_device *pdev)
{
	struct device	*dev = &pdev->dev;
	int		status;
	int		irq;	
	unsigned char __iomem	*base;
	unsigned char __iomem	*pbase;	
    struct device_node *node = NULL;

	INFO("musbfsh_probe++\r\n");

#ifdef CONFIG_MTK_ICUSB_SUPPORT
	//                   
	//                 
#endif

#ifndef CONFIG_MUSBFSH_PIO_ONLY//         
	/*                        */
	#ifdef CONFIG_OF 
		dev->dma_mask = &usb11_dmamask;	
		dev->coherent_dma_mask = usb11_dmamask;	
	#endif
	orig_dma_mask = dev->dma_mask;

#endif

#ifdef CONFIG_OF
    node = of_find_compatible_node(NULL, NULL, "mediatek,ICUSB");

    if(node == NULL){
         pr_info("USB get node failed\n");
		 return -1;
    }
    base = of_iomap(node, 0);
    irq = irq_of_parse_and_map(node, 0);
    pbase = of_iomap(node, 1);
#else
	irq = MT_USB1_IRQ_ID;
	base  = (unsigned char __iomem*)USB11_BASE;
	pbase = (unsigned char __iomem*)USB11_PHY_ADDR;
#endif


	status = musbfsh_init_controller(dev, irq, base, pbase);//                                                                
	if (status < 0)
		ERR("musbfsh_probe failed with status %d\n", status);
	INFO("musbfsh_probe--\r\n");
#if 0 //               
#ifdef IC_USB
	device_create_file(dev, &dev_attr_start);
	WARNING("IC-USB is enabled\n");
#endif
#endif
	return status;
}

static int __exit musbfsh_remove(struct platform_device *pdev)
{
	struct musbfsh	*musbfsh = dev_to_musbfsh(&pdev->dev);
	void __iomem	*ctrl_base = musbfsh->ctrl_base;
	INFO("musbfsh_remove++\r\n");
	/*                           
                                          
                                                                      
                                                                
  */
#ifdef CONFIG_MUSBFSH_DEBUG_FS 
	musbfsh_exit_debugfs(musbfsh);	 
#endif	
	musbfsh_shutdown(pdev);
	if (musbfsh->board_mode == MUSBFSH_HOST)
		usb_remove_hcd(musbfsh_to_hcd(musbfsh));
	musbfsh_writeb(musbfsh->mregs, MUSBFSH_DEVCTL, 0);
	musbfsh_platform_exit(musbfsh);
	musbfsh_writeb(musbfsh->mregs, MUSBFSH_DEVCTL, 0);

	musbfsh_free(musbfsh);
	iounmap(ctrl_base);
	device_init_wakeup(&pdev->dev, 0);
#ifndef CONFIG_MUSBFSH_PIO_ONLY
	pdev->dev.dma_mask = orig_dma_mask;
#endif
	return 0;
}

#ifdef	CONFIG_PM

static void musbfsh_save_context(struct musbfsh *musbfsh)
{
	int i;
	void __iomem *musbfsh_base = musbfsh->mregs;
	void __iomem *epio;

	musbfsh->context.power = musbfsh_readb(musbfsh_base, MUSBFSH_POWER);
	musbfsh->context.intrtxe = musbfsh_readw(musbfsh_base, MUSBFSH_INTRTXE);
	musbfsh->context.intrrxe = musbfsh_readw(musbfsh_base, MUSBFSH_INTRRXE);
	musbfsh->context.intrusbe = musbfsh_readb(musbfsh_base, MUSBFSH_INTRUSBE);
	musbfsh->context.index = musbfsh_readb(musbfsh_base, MUSBFSH_INDEX);
	musbfsh->context.devctl = musbfsh_readb(musbfsh_base, MUSBFSH_DEVCTL);

	musbfsh->context.l1_int = musbfsh_readl(musbfsh_base, USB11_L1INTM);

	for (i = 0; i < MUSBFSH_C_NUM_EPS -1; ++i) {
		struct musbfsh_hw_ep	*hw_ep;

		hw_ep = &musbfsh->endpoints[i];
		if (!hw_ep)
			continue;

		epio = hw_ep->regs;
		if (!epio)
			continue;

		musbfsh_writeb(musbfsh_base, MUSBFSH_INDEX, i);
		musbfsh->context.index_regs[i].txmaxp =
			musbfsh_readw(epio, MUSBFSH_TXMAXP);
		musbfsh->context.index_regs[i].txcsr =
			musbfsh_readw(epio, MUSBFSH_TXCSR);
		musbfsh->context.index_regs[i].rxmaxp =
			musbfsh_readw(epio, MUSBFSH_RXMAXP);
		musbfsh->context.index_regs[i].rxcsr =
			musbfsh_readw(epio, MUSBFSH_RXCSR);

		if (musbfsh->dyn_fifo) {
			musbfsh->context.index_regs[i].txfifoadd =
					musbfsh_read_txfifoadd(musbfsh_base);
			musbfsh->context.index_regs[i].rxfifoadd =
					musbfsh_read_rxfifoadd(musbfsh_base);
			musbfsh->context.index_regs[i].txfifosz =
					musbfsh_read_txfifosz(musbfsh_base);
			musbfsh->context.index_regs[i].rxfifosz =
					musbfsh_read_rxfifosz(musbfsh_base);
		}
	}
}

static void musbfsh_restore_context(struct musbfsh *musbfsh)
{
	int i;
	void __iomem *musbfsh_base = musbfsh->mregs;
	void __iomem *epio;

	musbfsh_writeb(musbfsh_base, MUSBFSH_POWER, musbfsh->context.power);
	musbfsh_writew(musbfsh_base, MUSBFSH_INTRTXE, musbfsh->context.intrtxe);
	musbfsh_writew(musbfsh_base, MUSBFSH_INTRRXE, musbfsh->context.intrrxe);
	musbfsh_writeb(musbfsh_base, MUSBFSH_INTRUSBE, musbfsh->context.intrusbe);
	musbfsh_writeb(musbfsh_base, MUSBFSH_DEVCTL, musbfsh->context.devctl);

	for (i = 0; i < MUSBFSH_C_NUM_EPS-1; ++i) {
		struct musbfsh_hw_ep	*hw_ep;

		hw_ep = &musbfsh->endpoints[i];
		if (!hw_ep)
			continue;

		epio = hw_ep->regs;
		if (!epio)
			continue;

		musbfsh_writeb(musbfsh_base, MUSBFSH_INDEX, i);
		musbfsh_writew(epio, MUSBFSH_TXMAXP,
			musbfsh->context.index_regs[i].txmaxp);
		musbfsh_writew(epio, MUSBFSH_TXCSR,
			musbfsh->context.index_regs[i].txcsr);
		musbfsh_writew(epio, MUSBFSH_RXMAXP,
			musbfsh->context.index_regs[i].rxmaxp);
		musbfsh_writew(epio, MUSBFSH_RXCSR,
			musbfsh->context.index_regs[i].rxcsr);

		if (musbfsh->dyn_fifo) {
			musbfsh_write_txfifosz(musbfsh_base,
				musbfsh->context.index_regs[i].txfifosz);
			musbfsh_write_rxfifosz(musbfsh_base,
				musbfsh->context.index_regs[i].rxfifosz);
			musbfsh_write_txfifoadd(musbfsh_base,
				musbfsh->context.index_regs[i].txfifoadd);
			musbfsh_write_rxfifoadd(musbfsh_base,
				musbfsh->context.index_regs[i].rxfifoadd);
		}
	}

	musbfsh_writeb(musbfsh_base, MUSBFSH_INDEX, musbfsh->context.index);
	mb();
	/*                             
                                                      
  */
	musbfsh_writel(musbfsh->mregs, MUSBFSH_HSDMA_INTR, 0xFF | (0xFF << MUSBFSH_DMA_INTR_UNMASK_SET_OFFSET));
	//     
	musbfsh_writel(musbfsh_base, USB11_L1INTM, musbfsh->context.l1_int);
}



static int musbfsh_suspend(struct device *dev)
{
	
	struct platform_device *pdev = to_platform_device(dev);
	//                    
	struct musbfsh	*musbfsh = dev_to_musbfsh(&pdev->dev);
	MYDBG("");		
	//                                         

#ifdef CONFIG_MTK_ICUSB_SUPPORT
	if(!usb11_enabled)
	{
		MYDBG("usb11 is not enabled");
		return 0;
	}
#endif

	//                                         
#ifndef CONFIG_MTK_ICUSB_SUPPORT
	musbfsh_set_vbus(musbfsh,0);//            
#endif

#ifdef CONFIG_SMP
	disable_irq(musbfsh->nIrq); //                                                                                                                                            
	musbfsh_save_context(musbfsh);
	musbfsh_read_clear_generic_interrupt(musbfsh);	
	//                                                   
#endif

#ifdef CONFIG_MTK_ICUSB_SUPPORT
	//                            
	mt65xx_usb11_phy_savecurrent();			
	mt65xx_usb11_disable_clk_pll_pw();
	suspend_cnt++;
#else
	musbfsh_set_power(musbfsh, 0);						
#endif
	//                                              
	return 0;
}

static int musbfsh_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	//                    
	struct musbfsh	*musbfsh = dev_to_musbfsh(&pdev->dev);
	MYDBG("");		

#ifdef CONFIG_MTK_ICUSB_SUPPORT
	MYDBG("suspend_cnt : %d, resume_cnt :%d\n", suspend_cnt, resume_cnt);															
	if(!usb11_enabled)
	{
		MYDBG("usb11 is not enabled");
		return 0;
	}
#endif
	//                                         
#ifdef CONFIG_MTK_ICUSB_SUPPORT
	resume_cnt++;
	mt65xx_usb11_enable_clk_pll_pw();
	mt65xx_usb11_phy_recover();							
	//                            
#else
	musbfsh_set_vbus(musbfsh,1);//           
	musbfsh_set_power(musbfsh, 1);						
#endif


#ifdef CONFIG_SMP
	musbfsh_restore_context(musbfsh);
	enable_irq(musbfsh->nIrq);
#endif
	//                                              
	return 0;
}

static const struct dev_pm_ops musbfsh_dev_pm_ops = {
	.suspend	= musbfsh_suspend,
	.resume	= musbfsh_resume,
};

#define MUSBFSH_DEV_PM_OPS (&musbfsh_dev_pm_ops)
#else
#define	MUSBFSH_DEV_PM_OPS	NULL
#endif

static struct platform_driver musbfsh_driver = {
	.driver = {
		.name       ="musbfsh",	
		.bus		= &platform_bus_type,
		.owner		= THIS_MODULE,
		.pm		= MUSBFSH_DEV_PM_OPS,
	},
	.probe      = musbfsh_probe,
	.remove		= __exit_p(musbfsh_remove),
	.shutdown	= musbfsh_shutdown,
};

extern void musbfsh_hcd_release (struct device *dev);

static struct musbfsh_hdrc_config musbfsh_config_mt65xx = {
	.multipoint     = false,
	.dyn_fifo       = true,
	.soft_con       = true,
	.dma            = true,
	.num_eps        = 6,
	.dma_channels   = 4,
};

static struct musbfsh_hdrc_platform_data usb_data_mt65xx = {
	.mode           = 1,
	.config         = &musbfsh_config_mt65xx,
};


static struct platform_device mt_usb11_dev = {
	.name           = "musbfsh",
	.id             = -1,
	.dev = {
		.platform_data          = &usb_data_mt65xx,	
		.dma_mask               = &usb11_dmamask,
		.coherent_dma_mask      = DMA_BIT_MASK(32),
		.release		= musbfsh_hcd_release,
	},
};

static int __init musbfsh_dts_probe(struct platform_device *pdev)
{
  int retval;
	retval = platform_device_register(&mt_usb11_dev);
	if (retval != 0){
		ERR("musbfsh_dts_probe failed with status %d\n", retval);
	}
	return retval;	
}




/*                                                                         */

static int __init musbfsh_init(void)
{
	int retval;

	if (usb_disabled())//                             
		return 0;
	WARNING("MUSBFSH is enabled\n");
	wake_lock_init(&musbfsh_suspend_lock, WAKE_LOCK_SUSPEND, "usb11 host");

	retval = platform_driver_register(&musbfsh_driver);
	if (retval != 0){
		ERR("musbfsh_init with status %d\n", retval);
		return retval;
	}

	retval = platform_driver_register(&musbfsh_dts_driver);
	if (retval != 0){
		ERR("musbfsh_init with status %d\n", retval);
		platform_driver_unregister(&musbfsh_driver);
		return retval;
	}
	return 0;
	
}

/*                                                                   
                                                                
 */
module_init(musbfsh_init);

static void __exit musbfsh_cleanup(void)
{
	wake_lock_destroy(&musbfsh_suspend_lock);
	platform_driver_unregister(&musbfsh_driver);
	platform_driver_unregister(&musbfsh_dts_driver);
}
module_exit(musbfsh_cleanup);

int musbfsh_debug = 0;
module_param(musbfsh_debug, int, 0644);
#ifdef MTK_DT_USB_SUPPORT
module_param(musbfsh_eint_cnt, int, 0644);	
module_param(musbfsh_skip_phy, int, 0644);	
module_param(musbfsh_skip_clk, int, 0644);	
module_param(musbfsh_wake_lock_hold, int, 0644);	
module_param(musbfsh_skip_port_suspend, int, 0644);	
module_param(musbfsh_skip_port_resume, int, 0644);	
#endif

#else
	

static void usb11_phy_savecurrent(unsigned char __iomem	*pbase)
{
		//                                                                       
		USB11PHY_CLR8(pbase+0x96b, 0x04);
		USB11PHY_CLR8(pbase+0x96e, 0x01);
		USB11PHY_CLR8(pbase+0x921, 0x2);	
		USB11PHY_CLR8(pbase+0x921, 0x1);

		//               
		USB11PHY_SET8(pbase+0x968, 0x08);	
		//                            
		USB11PHY_SET8(pbase+0x96a, 0x04);
		//                                  
		USB11PHY_SET8(pbase+0x968, 0xc0);
		//                            
		USB11PHY_CLR8(pbase+0x968, 0x30);
		USB11PHY_SET8(pbase+0x968, 0x10);
		//                      
		USB11PHY_SET8(pbase+0x968, 0x04);
		//                           
		USB11PHY_CLR8(pbase+0x969, 0x3c);
		//                                                                       
		USB11PHY_SET8(pbase+0x96a, 0xba);

		//                            
		USB11PHY_CLR8(pbase+0x91a, 0x80);
		//                                 
		USB11PHY_CLR8(pbase+0x91a, 0x10);
		//                  
		udelay(800);
		//                             
		USB11PHY_SET8(pbase+0x963, 0x02);

		udelay(1);

		USB11PHY_CLR8(pbase+0x96C, 0x2C);
		USB11PHY_SET8(pbase+0x96C, 0x10);
		USB11PHY_CLR8(pbase+0x96D, 0x3C);
		
		//               
		USB11PHY_CLR8(pbase+0x968, 0x08);

		//               
		udelay(1);	
}


static int __init musbfsh_dts_probe(struct platform_device *pdev)
{
  unsigned char __iomem	*pbase;	  
	
#ifdef CONFIG_OF  
	if(pdev->dev.of_node == NULL){
	  pr_info("USB get node failed\n");
	  return -1;
	}
	pbase = of_iomap(pdev->dev.of_node, 1);
#else
	pbase = (unsigned char __iomem*)USB11_PHY_ADDR;
#endif
	enable_clock (MT_CG_INFRA_USB, "USB11");		
	enable_clock (MT_CG_INFRA_ICUSB, "USB11");
	enable_clock (MT_CG_INFRA_USB_MCU, "USB11");
	usb11_phy_savecurrent(pbase);
	disable_clock (MT_CG_INFRA_USB_MCU, "USB11");
	disable_clock (MT_CG_INFRA_ICUSB, "USB11");
	disable_clock (MT_CG_INFRA_USB, "USB11");	
	return 0;
}


/*                                                                         */

static int __init musbfsh_init(void)
{
	pr_info("MUSBFSH DUMMY Driver is enabled\n");
	return platform_driver_register(&musbfsh_dts_driver);
}

/*                                                                   
                                                                
 */
module_init(musbfsh_init);

static void __exit musbfsh_cleanup(void)
{
	platform_driver_unregister(&musbfsh_dts_driver);
}
module_exit(musbfsh_cleanup);

#endif
