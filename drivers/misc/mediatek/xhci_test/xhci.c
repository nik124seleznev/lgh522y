/*
 * xHCI host controller driver
 *
 * Copyright (C) 2008 Intel Corp.
 *
 * Author: Sarah Sharp
 * Some code borrowed from the Linux EHCI driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/irq.h>
#include <linux/log2.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>

#include <linux/kernel.h>       /*          */
#include <linux/fs.h>           /*               */
#include <linux/errno.h>        /*             */
#include <linux/types.h>        /*        */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>        /*           */
#include <linux/seq_file.h>
#include <linux/cdev.h>
//                      
#include <asm/unaligned.h>
//                          
#include "xhci.h"
#include "mtk-test.h"
#include "mtk-test-lib.h"
#include "xhci-platform.c"
#include "mtk-usb-hcd.h"
#include "xhci-mtk-power.h"
#include "xhci-mtk-scheduler.h"

#if 1 //      
#include <mach/eint.h>
#include <linux/irq.h>
//                         
#endif

/*                                                                           */
static int link_quirk;

static void xhci_work(struct xhci_hcd *xhci){
	u32 temp;
	u64 temp_64;
	/*
                                            
                                                               
                                          
  */
	temp = xhci_readl(xhci, &xhci->op_regs->status);
	printk(KERN_ERR "[OTG_H][xhci_work] read status 0x%x\n", temp);
	
	temp |= STS_EINT;
	xhci_writel(xhci, temp, &xhci->op_regs->status);

	/*                           */
	temp = xhci_readl(xhci, &xhci->ir_set->irq_pending);
	printk(KERN_ERR "[OTG_H][xhci_work] read irq_pending 0x%x\n", temp);
	
	temp |= 0x3;
	xhci_writel(xhci, temp, &xhci->ir_set->irq_pending);

	/*                     */
	xhci_readl(xhci, &xhci->ir_set->irq_pending);

	while(mtktest_xhci_handle_event(xhci) > 0){}

	/*                                                                           */
	temp_64 = xhci_read_64(xhci, &xhci->ir_set->erst_dequeue);
	xhci_dbg(xhci, "Clear EHB bit (RW1C)");
	xhci_write_64(xhci, temp_64 | ERST_EHB, &xhci->ir_set->erst_dequeue);
	/*                                                 */
	xhci_readl(xhci, &xhci->ir_set->irq_pending);
}

irqreturn_t mtktest_xhci_mtk_irq(struct usb_hcd *hcd){
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	u32 temp, temp2;
	union xhci_trb *trb;
#if TEST_OTG
	u32 temp3;
#endif

	spin_lock(&xhci->lock);
#if TEST_OTG
	temp3 = readl(SSUSB_OTG_STS);
	printk(KERN_ERR "[OTG_H][IRQ] OTG_STS 0x%x\n", temp3);
#if 0
	if(temp3 & SSUSB_IDDIG){
		g_otg_iddig = 1;
	}
	else{
		g_otg_iddig = 0;
	}
#endif
#if 0
	if(temp3 & SSUSB_ATTACH_A_ROLE){
#if 0
		//                              
		temp = readl(SSUSB_U2_CTRL(0));
		temp = temp | SSUSB_U2_PORT_OTG_HOST_VBUSVALID_SEL;
		writel(temp, SSUSB_U2_CTRL(0));
#endif
		//                                                         
		mtktest_enableXhciAllPortPower(xhci);
		writel(SSUSB_ATTACH_A_ROLE_CLR, SSUSB_OTG_STS_CLR);
		spin_unlock(&xhci->lock);
		return IRQ_HANDLED;
	}
#endif
	if(temp3 & SSUSB_CHG_A_ROLE_A){
		g_otg_wait_con = true;
		g_otg_hnp_become_dev = false ;
		g_otg_hnp_become_host = true;
		mb() ;
		writel(SSUSB_CHG_A_ROLE_A_CLR, SSUSB_OTG_STS_CLR);
		//            
		printk(KERN_ERR "[OTG_H] going to set dma to host\n");
		
		
#if 0		
		writel(0x0f0f0f0f, 0xf00447bc);		
		while((readl(0xf00447c4) & 0x2000) == 0x2000){
		
		}
		printk(KERN_ERR "[OTG_H] can set dma to host\n");
#endif		
		temp = readl(SSUSB_U2_CTRL(0));
		temp = temp | SSUSB_U2_PORT_HOST_SEL;
		writel(temp, SSUSB_U2_CTRL(0));
		spin_unlock(&xhci->lock);
		return IRQ_HANDLED;
	}
	if(temp3 & SSUSB_CHG_B_ROLE_A){
		printk("[OTG_H] get SSUSB_CHG_B_ROLE_A\n");
		g_otg_hnp_become_dev = true;
		mb() ;
		writel(SSUSB_CHG_B_ROLE_A_CLR, SSUSB_OTG_STS_CLR);
		spin_unlock(&xhci->lock);
		return IRQ_HANDLED;
	}
	if(temp3 & SSUSB_SRP_REQ_INTR){
		//              
		//                       
		//      
		//                      
		printk("[OTG_H] get srp interrupt, just clear it\n");
		
		writel(SSUSB_SRP_REQ_INTR_CLR, SSUSB_OTG_STS_CLR);
		spin_unlock(&xhci->lock);
		return IRQ_HANDLED;
	}
#endif
	trb = xhci->event_ring->dequeue;

	temp = xhci_readl(xhci, &xhci->op_regs->status);
	temp2 = xhci_readl(xhci, &xhci->ir_set->irq_pending);
	
	//                                                                    

	//                                                                          
	
	if (!(temp & STS_EINT) && !ER_IRQ_PENDING(temp2)) {
		spin_unlock(&xhci->lock);
		return IRQ_NONE;
	}
	xhci_dbg(xhci, "--> interrupt in: op_reg(%08x), irq_pend(%08x)\n", temp, temp2);

	#if 0
	xhci_dbg(xhci, "Got interrupt\n");
	xhci_dbg(xhci, "op reg status = %08x\n", temp);
	xhci_dbg(xhci, "ir set irq_pending = %08x\n", temp2);
	xhci_dbg(xhci, "Event ring dequeue ptr:\n");
	xhci_dbg(xhci, "@%llx %08x %08x %08x %08x\n",
			(unsigned long long)mtktest_xhci_trb_virt_to_dma(xhci->event_ring->deq_seg, trb),
			lower_32_bits(trb->link.segment_ptr),
			upper_32_bits(trb->link.segment_ptr),
			(unsigned int) trb->link.intr_target,
			(unsigned int) trb->link.control);
	#endif
	if(g_intr_handled != -1){
		g_intr_handled++;
	}
	xhci_work(xhci);
	spin_unlock(&xhci->lock);
	xhci_dbg(xhci, "<-- interrupt out\n");
	return IRQ_HANDLED;
}


#if 1 //      
static int mtk_idpin_irqnum;

#if 0
#define IDPIN_IN MT_EINT_POL_NEG
#define IDPIN_OUT MT_EINT_POL_POS
#else
enum usbif_idpin_state {
	IDPIN_IN,
	IDPIN_OUT,
};

#endif

static struct xhci_hcd	*mtk_xhci = NULL;
static bool mtk_id_nxt_state = IDPIN_IN;
#define IDDIG_EINT_PIN (16)

static bool mtktest_set_iddig_out_detect(){
#if 0
    mt_eint_set_polarity(IDDIG_EINT_PIN, MT_EINT_POL_POS);
	mt_eint_unmask(IDDIG_EINT_PIN);
#else
	irq_set_irq_type(mtk_idpin_irqnum, IRQF_TRIGGER_HIGH);
	//                             
#endif
}

static bool mtktest_set_iddig_in_detect(){
#if 0
    mt_eint_set_polarity(IDDIG_EINT_PIN, MT_EINT_POL_NEG);
	mt_eint_unmask(IDDIG_EINT_PIN);
#else
	irq_set_irq_type(mtk_idpin_irqnum, IRQF_TRIGGER_LOW);
	//                             
#endif
}
#if 0
static void mtktest_xhci_eint_iddig_isr(void){
#else
static irqreturn_t mtktest_xhci_eint_iddig_isr(int irqnum, void *data){
#endif
    bool cur_id_state = mtk_id_nxt_state;

    if(cur_id_state == IDPIN_IN){ //     
        /*                                             */
        //                  
        /*                                         */
        //                                         

        /*                                          */
        mtk_id_nxt_state = IDPIN_OUT;

        g_otg_iddig = 0 ;
        mb() ;

        /*                                    */
        mtktest_set_iddig_out_detect();

        //                                           
    }
    else{ /*           */
        /*                                          */
        //                                          
        /*                                                   */
        //                         
        /*                                         */
        mtk_id_nxt_state = IDPIN_IN;

		g_otg_iddig = 1 ;
		mb() ;
        /*                                   */
        mtktest_set_iddig_in_detect();

        //                                           
    }

    printk("[OTG_H] xhci switch resource to %s\n", (cur_id_state == IDPIN_IN)? "host": "device");
}

void mtktest_mtk_xhci_set(struct xhci_hcd *xhci){
    mtk_xhci = xhci;
}

void mtktest_mtk_xhci_eint_iddig_init(void){
#if 0
	mt_eint_set_sens(IDDIG_EINT_PIN, MT_LEVEL_SENSITIVE);
	mt_eint_set_hw_debounce(IDDIG_EINT_PIN,64);
#else
	int retval;
	
	mtk_idpin_irqnum = mt_gpio_to_irq(IDDIG_EINT_PIN);
	/*              */
	mt_gpio_set_debounce(IDDIG_EINT_PIN, 50);
#endif
	mtktest_set_iddig_in_detect();

#if 0	
	mt_eint_registration(IDDIG_EINT_PIN, EINTF_TRIGGER_LOW, mtktest_xhci_eint_iddig_isr, false);
#else
	retval =
	    request_irq(mtk_idpin_irqnum, mtktest_xhci_eint_iddig_isr, IRQF_TRIGGER_LOW, "usbif_iddig_eint",
			NULL);
#endif

	enable_irq(mtk_idpin_irqnum);
	
	g_otg_iddig = 1 ;
    
    printk("[OTG_H] XHCI test driver GPIO iddig setting done.\n");
}

void mtktest_mtk_xhci_eint_iddig_deinit(void){
#if 0
	mt_eint_registration(IDDIG_EINT_PIN, EINTF_TRIGGER_LOW, NULL, false);
#else
	disable_irq_nosync(mtk_idpin_irqnum);

	free_irq(mtk_idpin_irqnum, NULL);

#endif

    printk("[OTG_H] XHCI test driver GPIO iddig deinit done.\n");	
}


#endif

//                        

/*                                                        */
/*
                                                                 
                                          
                                           
                                                     
                                 
  
                                             
  
                                                                          
                                                                            
                                                                            
 */
static int handshake(struct xhci_hcd *xhci, void __iomem *ptr,
		      u32 mask, u32 done, int usec)
{
	u32	result;

	do {
		result = xhci_readl(xhci, ptr);
		if (result == ~(u32)0)		/*              */
			return -ENODEV;
		result &= mask;
		if (result == done)
			return 0;
		udelay(1);
		usec--;
	} while (usec > 0);
	return -ETIMEDOUT;
}

/*
                                                         
 */
void mtktest_xhci_quiesce(struct xhci_hcd *xhci)
{
	u32 halted;
	u32 cmd;
	u32 mask;

	mask = ~(XHCI_IRQS);
	halted = xhci_readl(xhci, &xhci->op_regs->status) & STS_HALT;
	if (!halted)
		mask &= ~CMD_RUN;

	cmd = xhci_readl(xhci, &xhci->op_regs->command);
	cmd &= mask;
	xhci_writel(xhci, cmd, &xhci->op_regs->command);
}

/*
                            
  
                                               
                                                                        
                                                                       
                                                                            
                                                      
 */
int mtktest_xhci_halt(struct xhci_hcd *xhci)
{
	xhci_dbg(xhci, "// Halt the HC\n");
	mtktest_xhci_quiesce(xhci);

	return handshake(xhci, &xhci->op_regs->status,
			STS_HALT, STS_HALT, XHCI_MAX_HALT_USEC);
}
/*
                                                       
 */
int xhci_start(struct xhci_hcd *xhci)
{
	u32 temp;
	int ret;

	temp = xhci_readl(xhci, &xhci->op_regs->command);
	temp |= (CMD_RUN);
	xhci_dbg(xhci, "// Turn on HC, cmd = 0x%x.\n",
			temp);
	xhci_writel(xhci, temp, &xhci->op_regs->command);

	/*
                                                                    
            
  */
	ret = handshake(xhci, &xhci->op_regs->status,
			STS_HALT, 0, XHCI_MAX_HALT_USEC);
	if (ret == -ETIMEDOUT)
		xhci_err(xhci, "[ERROR]Host took too long to start, "
				"waited %u microseconds.\n",
				XHCI_MAX_HALT_USEC);
	return ret;
}

/*
                                                                     
  
                                                                
                                                                         
                                 
 */
int mtktest_xhci_reset(struct xhci_hcd *xhci)
{
	u32 command;
	u32 state;
	int ret;

	state = xhci_readl(xhci, &xhci->op_regs->status);
	if ((state & STS_HALT) == 0) {
		xhci_warn(xhci, "Host controller not halted, aborting reset.\n");
		return 0;
	}

	xhci_dbg(xhci, "// Reset the HC\n");
	command = xhci_readl(xhci, &xhci->op_regs->command);
	command |= CMD_RESET;
	xhci_writel(xhci, command, &xhci->op_regs->command);
	/*                                                                  */
	xhci_to_hcd(xhci)->state = HC_STATE_HALT;

	ret = handshake(xhci, &xhci->op_regs->command,
			CMD_RESET, 0, 250 * 1000);
	if (ret)
		return ret;

	xhci_dbg(xhci, "Wait for controller to be ready for doorbell rings\n");
	/*
                                                                     
                                                                 
  */
	return handshake(xhci, &xhci->op_regs->status, STS_CNR, 0, 250 * 1000);
}

/*
                                                     
  
                                                                             
                                                                             
                      
 */
int mtktest_xhci_init(struct usb_hcd *hcd)
{
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	int retval = 0;

	xhci_dbg(xhci, "mtktest_xhci_init\n");
	spin_lock_init(&xhci->lock);
	if (link_quirk) {
		xhci_dbg(xhci, "QUIRK: Not clearing Link TRB chain bits.\n");
		xhci->quirks |= XHCI_LINK_TRB_QUIRK;
	} else {
		xhci_dbg(xhci, "xHCI doesn't need link TRB QUIRK\n");
	}
	retval = mtktest_xhci_mem_init(xhci, GFP_KERNEL);
	xhci_dbg(xhci, "Finished mtktest_xhci_init\n");

	return retval;
}

/*                                                                         */

/* 
                                                                                            
                                                                              
                                          
  
                                        
                                         
                                                                        
                                                                          
 */
unsigned int mtktest_xhci_get_endpoint_index(struct usb_endpoint_descriptor *desc)
{
	unsigned int index;
	if (usb_endpoint_xfer_control(desc))
		index = (unsigned int) (usb_endpoint_num(desc)*2);
	else
		index = (unsigned int) (usb_endpoint_num(desc)*2) +
			(usb_endpoint_dir_in(desc) ? 1 : 0) - 1;
	return index;
}

/*                                                                           
                                                                                
              
 */
unsigned int mtktest_xhci_get_endpoint_flag_from_index(unsigned int ep_index)
{
	return 1 << (ep_index + 1);
}


/*                                                                       
                                                                             
                                                                    
                                                        
                                                                         
 */
unsigned int mtktest_xhci_last_valid_endpoint(u32 added_ctxs)
{
	return fls(added_ctxs) - 1;
}

static void xhci_setup_input_ctx_for_config_ep(struct xhci_hcd *xhci,
		struct xhci_container_ctx *in_ctx,
		struct xhci_container_ctx *out_ctx,
		u32 add_flags, u32 drop_flags)
{
	struct xhci_input_control_ctx *ctrl_ctx;
	ctrl_ctx = mtktest_xhci_get_input_control_ctx(xhci, in_ctx);
	ctrl_ctx->add_flags = add_flags;
	ctrl_ctx->drop_flags = drop_flags;
	mtktest_xhci_slot_copy(xhci, in_ctx, out_ctx);
	ctrl_ctx->add_flags |= SLOT_FLAG;

	xhci_dbg(xhci, "Input Context:\n");
	mtktest_xhci_dbg_ctx(xhci, in_ctx, mtktest_xhci_last_valid_endpoint(add_flags));
}

void xhci_setup_input_ctx_for_quirk(struct xhci_hcd *xhci,
		unsigned int slot_id, unsigned int ep_index,
		struct xhci_dequeue_state *deq_state)
{
	struct xhci_container_ctx *in_ctx;
	struct xhci_ep_ctx *ep_ctx;
	u32 added_ctxs;
	dma_addr_t addr;

	mtktest_xhci_endpoint_copy(xhci, xhci->devs[slot_id]->in_ctx,
			xhci->devs[slot_id]->out_ctx, ep_index);
	in_ctx = xhci->devs[slot_id]->in_ctx;
	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, in_ctx, ep_index);
	addr = mtktest_xhci_trb_virt_to_dma(deq_state->new_deq_seg,
			deq_state->new_deq_ptr);
	if (addr == 0) {
		xhci_warn(xhci, "WARN Cannot submit config ep after "
				"reset ep command\n");
		xhci_warn(xhci, "WARN deq seg = %p, deq ptr = %p\n",
				deq_state->new_deq_seg,
				deq_state->new_deq_ptr);
		return;
	}
	ep_ctx->deq = addr | deq_state->new_cycle_state;

	added_ctxs = mtktest_xhci_get_endpoint_flag_from_index(ep_index);
	xhci_setup_input_ctx_for_config_ep(xhci, xhci->devs[slot_id]->in_ctx,
			xhci->devs[slot_id]->out_ctx, added_ctxs, added_ctxs);
}

/*                                 */
int mtktest_xhci_mtk_run(struct usb_hcd *hcd){
	printk("mtktest_xhci_mtk_run is called\n");
	u32 temp;
	u64 temp_64;
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);
	void (*doorbell)(struct xhci_hcd *) = NULL;

	hcd->uses_new_polling = 1;
//                  

	xhci_dbg(xhci, "mtktest_xhci_run\n");
#if 0	/*                          */
	/*                                 */
	ret = xhci_setup_msix(xhci);
	if (!ret)
		return ret;

	return -ENOSYS;
#endif
#ifdef CONFIG_USB_XHCI_HCD_DEBUGGING
	init_timer(&xhci->event_ring_timer);
	xhci->event_ring_timer.data = (unsigned long) xhci;
	xhci->event_ring_timer.function = xhci_event_ring_work;
	/*                     */
	xhci->event_ring_timer.expires = jiffies + POLL_TIMEOUT * HZ;
	xhci->zombie = 0;
	xhci_dbg(xhci, "Setting event ring polling timer\n");
	add_timer(&xhci->event_ring_timer);
#endif

	xhci_dbg(xhci, "Command ring memory map follows:\n");
	mtktest_xhci_debug_ring(xhci, xhci->cmd_ring);
	mtktest_xhci_dbg_ring_ptrs(xhci, xhci->cmd_ring);
	mtktest_xhci_dbg_cmd_ptrs(xhci);

	xhci_dbg(xhci, "ERST memory map follows:\n");
	mtktest_xhci_dbg_erst(xhci, &xhci->erst);
	xhci_dbg(xhci, "Event ring:\n");
	mtktest_xhci_debug_ring(xhci, xhci->event_ring);
	mtktest_xhci_dbg_ring_ptrs(xhci, xhci->event_ring);
	temp_64 = xhci_read_64(xhci, &xhci->ir_set->erst_dequeue);
	temp_64 &= ~ERST_PTR_MASK;
	xhci_dbg(xhci, "ERST deq = 64'h%0lx\n", (long unsigned int) temp_64);

	xhci_dbg(xhci, "// Set the interrupt modulation register\n");
	temp = xhci_readl(xhci, &xhci->ir_set->irq_control);
	temp &= ~ER_IRQ_INTERVAL_MASK;
	temp |= (u32) 160;
	xhci_writel(xhci, temp, &xhci->ir_set->irq_control);

	/*                                             */
	hcd->state = HC_STATE_RUNNING;
	temp = xhci_readl(xhci, &xhci->op_regs->command);
	temp |= (CMD_EIE);
	xhci_dbg(xhci, "// Enable interrupts, cmd = 0x%x.\n",
			temp);
	xhci_writel(xhci, temp, &xhci->op_regs->command);

	temp = xhci_readl(xhci, &xhci->ir_set->irq_pending);
	xhci_dbg(xhci, "// Enabling event ring interrupter %p by writing 0x%x to irq_pending\n",
			xhci->ir_set, (unsigned int) ER_IRQ_ENABLE(temp));
	xhci_writel(xhci, ER_IRQ_ENABLE(temp),
			&xhci->ir_set->irq_pending);
	mtktest_xhci_print_ir_set(xhci, xhci->ir_set, 0);

	if (NUM_TEST_NOOPS > 0)
		doorbell = mtktest_xhci_setup_one_noop(xhci);
#if 0
	if (xhci->quirks & XHCI_NEC_HOST)
		mtktest_xhci_queue_address_device(xhci, 0, 0, 0,
				TRB_TYPE(TRB_NEC_GET_FW));
#endif
	if (xhci_start(xhci)) {
		mtktest_xhci_halt(xhci);
		return -ENODEV;
	}

	xhci_dbg(xhci, "// @%p = 0x%x\n", &xhci->op_regs->command, temp);
	if (doorbell)
		(*doorbell)(xhci);
#if 0
	if (xhci->quirks & XHCI_NEC_HOST)
		mtktest_xhci_ring_cmd_db(xhci);
#endif

#if 0 //      
	mtktest_mtk_xhci_set(xhci);
	mtktest_mtk_xhci_eint_iddig_init();
#else

	#if TEST_OTG
	mb() ;
	if(!g_otg_test){	
	#endif
		mtktest_enableXhciAllPortPower(xhci);
	#if TEST_OTG
	}
	#endif
#endif
	msleep(50);

	mtktest_disableAllClockPower();
	xhci_dbg(xhci, "Finished mtktest_xhci_run\n");
	return 0;
}

void mtktest_xhci_mtk_stop(struct usb_hcd *hcd){
	printk("mtktest_xhci_mtk_stop is called\n");
	u32 temp;
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);

#ifdef TEST_OTG_IDDIG //      
	mtktest_disableXhciAllPortPower(xhci);
	mtktest_mtk_xhci_eint_iddig_deinit();

#endif

	spin_lock_irq(&xhci->lock);
	mtktest_xhci_halt(xhci);
	mtktest_xhci_reset(xhci);
	spin_unlock_irq(&xhci->lock);

#if 0	/*            */
	xhci_cleanup_msix(xhci);
#endif
#ifdef CONFIG_USB_XHCI_HCD_DEBUGGING
	/*                                                     */
	xhci->zombie = 1;
	del_timer_sync(&xhci->event_ring_timer);
#endif

	xhci_dbg(xhci, "// Disabling event ring interrupts\n");
	temp = xhci_readl(xhci, &xhci->op_regs->status);
	xhci_writel(xhci, temp & ~STS_EINT, &xhci->op_regs->status);
	temp = xhci_readl(xhci, &xhci->ir_set->irq_pending);
	xhci_writel(xhci, ER_IRQ_DISABLE(temp),
			&xhci->ir_set->irq_pending);
	mtktest_xhci_print_ir_set(xhci, xhci->ir_set, 0);

	xhci_dbg(xhci, "cleaning up memory\n");
	mtktest_xhci_mem_cleanup(xhci);
	xhci_dbg(xhci, "mtktest_xhci_stop completed - status = %x\n",
		    xhci_readl(xhci, &xhci->op_regs->status));
	//                  
}

void mtktest_xhci_mtk_shutdown(struct usb_hcd *hcd){
	printk("mtktest_xhci_mtk_shutdown is called\n");
	struct xhci_hcd *xhci = hcd_to_xhci(hcd);

	spin_lock_irq(&xhci->lock);
	mtktest_xhci_halt(xhci);
	spin_unlock_irq(&xhci->lock);

#if 0
	xhci_cleanup_msix(xhci);
#endif

	xhci_dbg(xhci, "mtktest_xhci_shutdown completed - status = %x\n",
		    xhci_readl(xhci, &xhci->op_regs->status));
}

int mtktest_xhci_mtk_urb_enqueue(struct usb_hcd *hcd, struct urb *urb, gfp_t mem_flags){
	printk("mtktest_xhci_mtk_urb_enqueue is called\n");
}

int mtktest_xhci_mtk_urb_dequeue(struct usb_hcd *hcd, struct urb *urb, int status){
	printk("mtktest_xhci_mtk_urb_dequeue is called\n");
}

int mtktest_xhci_mtk_alloc_dev(struct usb_hcd *hcd, struct usb_device *udev){
	printk("mtktest_xhci_mtk_alloc_dev is called\n");
}

void mtktest_xhci_mtk_free_dev(struct usb_hcd *hcd, struct usb_device *udev){
	printk("mtktest_xhci_mtk_free_dev is called\n");
}

int mtktest_xhci_mtk_alloc_streams(struct usb_hcd *hcd, struct usb_device *udev
		, struct usb_host_endpoint **eps, unsigned int num_eps,
		unsigned int num_streams, gfp_t mem_flags){
	printk("mtktest_xhci_mtk_alloc_streams is called\n");
}

int mtktest_xhci_mtk_free_streams(struct usb_hcd *hcd, struct usb_device *udev,
		struct usb_host_endpoint **eps, unsigned int num_eps,
		gfp_t mem_flags){
	printk("mtktest_xhci_mtk_free_streams is called\n");
}

int mtktest_xhci_mtk_add_endpoint(struct usb_hcd *hcd, struct usb_device *udev, struct usb_host_endpoint *ep){
	struct xhci_hcd *xhci;
	struct xhci_container_ctx *in_ctx, *out_ctx;
	unsigned int ep_index;
	struct xhci_ep_ctx *ep_ctx;
	struct xhci_slot_ctx *slot_ctx;
	struct xhci_input_control_ctx *ctrl_ctx;
	u32 added_ctxs;
	unsigned int last_ctx;
	u32 new_add_flags, new_drop_flags, new_slot_info;
	int ret = 0;
#if 0
	ret = xhci_check_args(hcd, udev, ep, 1, __func__);
	if (ret <= 0) {
		/*                                                     */
		ep->hcpriv = NULL;
		return ret;
	}
#endif
	xhci = hcd_to_xhci(hcd);

	added_ctxs = mtktest_xhci_get_endpoint_flag(&ep->desc);
	last_ctx = mtktest_xhci_last_valid_endpoint(added_ctxs);
	if (added_ctxs == SLOT_FLAG || added_ctxs == EP0_FLAG) {
		/*                                                            
                                                           
                
   */
		xhci_dbg(xhci, "xHCI %s - can't add slot or ep 0 %#x\n",
				__func__, added_ctxs);
		return 0;
	}

	if (!xhci->devs || !xhci->devs[udev->slot_id]) {
		xhci_warn(xhci, "xHCI %s called with unaddressed device\n",
				__func__);
		return -EINVAL;
	}

	in_ctx = xhci->devs[udev->slot_id]->in_ctx;
	out_ctx = xhci->devs[udev->slot_id]->out_ctx;
	ctrl_ctx = mtktest_xhci_get_input_control_ctx(xhci, in_ctx);
	ep_index = mtktest_xhci_get_endpoint_index(&ep->desc);
	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, out_ctx, ep_index);
	/*                                                      
                        
  */
	if (ctrl_ctx->add_flags & mtktest_xhci_get_endpoint_flag(&ep->desc)) {
		xhci_warn(xhci, "xHCI %s called with enabled ep %p\n",
				__func__, ep);
		return 0;
	}

	/*
                                                               
                                                              
                                                               
  */
	if (mtktest_xhci_endpoint_init(xhci, xhci->devs[udev->slot_id],
				udev, ep, GFP_NOIO) < 0) {
		dev_dbg(&udev->dev, "%s - could not initialize ep %#x\n",
				__func__, ep->desc.bEndpointAddress);
		return -ENOMEM;
	}

	ctrl_ctx->add_flags |= added_ctxs;
	new_add_flags = ctrl_ctx->add_flags;

	/*                                                                 
                                                                    
                                                                   
                                                                        
                     
  */
	new_drop_flags = ctrl_ctx->drop_flags;

	slot_ctx = mtktest_xhci_get_slot_ctx(xhci, in_ctx);
	/*                                                                   */
	if ((slot_ctx->dev_info & LAST_CTX_MASK) < LAST_CTX(last_ctx)) {
		slot_ctx->dev_info &= ~LAST_CTX_MASK;
		slot_ctx->dev_info |= LAST_CTX(last_ctx);
	}
	new_slot_info = slot_ctx->dev_info;

	/*                                            */
	ep->hcpriv = udev;

	xhci_dbg(xhci, "add ep 0x%x, slot id %d, new drop flags = %#x, new add flags = %#x\n",
			(unsigned int) ep->desc.bEndpointAddress,
			udev->slot_id,
			(unsigned int) new_drop_flags,
			(unsigned int) new_add_flags);
	xhci_dbg(xhci, "new slot context 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n"
		, slot_ctx->dev_info, slot_ctx->dev_info2, slot_ctx->tt_info, slot_ctx->dev_state
		, slot_ctx->reserved[0], slot_ctx->reserved[1], slot_ctx->reserved[2], slot_ctx->reserved[3]);
	return 0;
}

int mtktest_xhci_mtk_drop_endpoint(struct usb_hcd *hcd, struct usb_device *udev
		, struct usb_host_endpoint *ep){
	struct xhci_hcd *xhci;
	struct xhci_container_ctx *in_ctx, *out_ctx;
	struct xhci_input_control_ctx *ctrl_ctx;
	struct xhci_slot_ctx *slot_ctx;
	unsigned int last_ctx;
	unsigned int ep_index;
	struct xhci_ep_ctx *ep_ctx;
	u32 drop_flag;
	u32 new_add_flags, new_drop_flags, new_slot_info;
	int ret;

	xhci = hcd_to_xhci(hcd);
	if (xhci->xhc_state & XHCI_STATE_DYING)
		return -ENODEV;
	xhci_dbg(xhci, "%s called for udev %p\n", __func__, udev);
	drop_flag = mtktest_xhci_get_endpoint_flag(&ep->desc);
	if (drop_flag == SLOT_FLAG || drop_flag == EP0_FLAG) {
		xhci_dbg(xhci, "xHCI %s - can't drop slot or ep 0 %#x\n",
				__func__, drop_flag);
		return 0;
	}

	in_ctx = xhci->devs[udev->slot_id]->in_ctx;
	out_ctx = xhci->devs[udev->slot_id]->out_ctx;
	ctrl_ctx = mtktest_xhci_get_input_control_ctx(xhci, in_ctx);
	ep_index = mtktest_xhci_get_endpoint_index(&ep->desc);
	ep_ctx = mtktest_xhci_get_ep_ctx(xhci, out_ctx, ep_index);

	/*                                                  
                                                            
  */
	if ((le32_to_cpu(ep_ctx->ep_info) & EP_STATE_MASK) ==
	    EP_STATE_DISABLED ||
	    le32_to_cpu(ctrl_ctx->drop_flags) &
	    mtktest_xhci_get_endpoint_flag(&ep->desc)) {
		xhci_warn(xhci, "xHCI %s called with disabled ep %p\n",
				__func__, ep);
		return 0;
	}

	ctrl_ctx->drop_flags |= cpu_to_le32(drop_flag);
	new_drop_flags = le32_to_cpu(ctrl_ctx->drop_flags);

	ctrl_ctx->add_flags &= cpu_to_le32(~drop_flag);
	new_add_flags = le32_to_cpu(ctrl_ctx->add_flags);

	last_ctx = mtktest_xhci_last_valid_endpoint(le32_to_cpu(ctrl_ctx->add_flags));
	slot_ctx = mtktest_xhci_get_slot_ctx(xhci, in_ctx);
	/*                                                                    */
	if ((le32_to_cpu(slot_ctx->dev_info) & LAST_CTX_MASK) >
	    LAST_CTX(last_ctx)) {
		slot_ctx->dev_info &= cpu_to_le32(~LAST_CTX_MASK);
		slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(last_ctx));
	}
	new_slot_info = le32_to_cpu(slot_ctx->dev_info);

	mtktest_xhci_endpoint_zero(xhci, xhci->devs[udev->slot_id], ep);

	xhci_dbg(xhci, "drop ep 0x%x, slot id %d, new drop flags = %#x, new add flags = %#x, new slot info = %#x\n",
			(unsigned int) ep->desc.bEndpointAddress,
			udev->slot_id,
			(unsigned int) new_drop_flags,
			(unsigned int) new_add_flags,
			(unsigned int) new_slot_info);
	return 0;
}
void mtktest_xhci_cleanup_stalled_ring(struct xhci_hcd *xhci,
		struct usb_device *udev, unsigned int ep_index)
{
	struct xhci_dequeue_state deq_state;
	struct xhci_virt_ep *ep;

	xhci_dbg(xhci, "Cleaning up stalled endpoint ring\n");
	ep = &xhci->devs[udev->slot_id]->eps[ep_index];
	/*                                                       
                                                              
  */
	mtktest_xhci_find_new_dequeue_state(xhci, udev->slot_id,
			ep_index, ep->stopped_stream, ep->stopped_td,
			&deq_state);

	/*                                                                     
                                             
  */
	if (!(xhci->quirks & XHCI_RESET_EP_QUIRK)) {
		xhci_dbg(xhci, "Queueing new dequeue state\n");
		mtktest_xhci_queue_new_dequeue_state(xhci, udev->slot_id,
				ep_index, ep->stopped_stream, &deq_state);
	} else {
		/*                                                              
                               
                                                                
                 
   */
		xhci_dbg(xhci, "Setting up input context for "
				"configure endpoint command\n");
		xhci_setup_input_ctx_for_quirk(xhci, udev->slot_id,
				ep_index, &deq_state);
	}
}

void mtktest_xhci_zero_in_ctx(struct xhci_hcd *xhci, struct xhci_virt_device *virt_dev)
{
	struct xhci_input_control_ctx *ctrl_ctx;
	struct xhci_ep_ctx *ep_ctx;
	struct xhci_slot_ctx *slot_ctx;
	int i;

	/*                                                                
                                                               
                                                                   
                      
  */
	ctrl_ctx = mtktest_xhci_get_input_control_ctx(xhci, virt_dev->in_ctx);
	ctrl_ctx->drop_flags = 0;
	ctrl_ctx->add_flags = 0;
	slot_ctx = mtktest_xhci_get_slot_ctx(xhci, virt_dev->in_ctx);
	slot_ctx->dev_info &= cpu_to_le32(~LAST_CTX_MASK);
	/*                            */
	slot_ctx->dev_info |= cpu_to_le32(LAST_CTX(1));
	for (i = 1; i < 31; ++i) {
		ep_ctx = mtktest_xhci_get_ep_ctx(xhci, virt_dev->in_ctx, i);
		ep_ctx->ep_info = 0;
		ep_ctx->ep_info2 = 0;
		ep_ctx->deq = 0;
		ep_ctx->tx_info = 0;
	}
}


void mtktest_xhci_mtk_endpoint_reset(struct usb_hcd *hcd, struct usb_host_endpoint *ep){
	printk("mtktest_xhci_mtk_endpoint_reset is called\n");
}

int mtktest_xhci_mtk_check_bandwidth(struct usb_hcd *hcd, struct usb_device *udev){
	printk("mtktest_xhci_mtk_check_bandwidth is called\n");
}

void mtktest_xhci_mtk_reset_bandwidth(struct usb_hcd *hcd, struct usb_device *udev){
	printk("mtktest_xhci_mtk_reset_bandwidth is called\n");
}

int mtktest_xhci_mtk_address_device(struct usb_hcd *hcd, struct usb_device *udev){
	printk("mtktest_xhci_mtk_address_device is called\n");
}

int mtktest_xhci_mtk_update_hub_device(struct usb_hcd *hcd, struct usb_device *hdev,
			struct usb_tt *tt, gfp_t mem_flags){
	printk("mtktest_xhci_mtk_update_hub_device is called\n");
}

int mtktest_xhci_mtk_reset_device(struct usb_hcd *hcd, struct usb_device *udev){
	printk("mtktest_xhci_mtk_reset_device is called\n");
}

int mtktest_xhci_mtk_hub_control(struct usb_hcd *hcd, u16 typeReq, u16 wValue,
		u16 wIndex, char *buf, u16 wLength){
	printk("mtktest_xhci_mtk_hub_control is called\n");
	return 0;
}

int mtktest_xhci_mtk_hub_status_data(struct usb_hcd *hcd, char *buf){
	printk("mtktest_xhci_mtk_hub_status_data is called\n");
	return 0;
}

int mtktest_xhci_mtk_get_frame(struct usb_hcd *hcd){
	printk("mtktest_xhci_mtk_get_frame is called\n");
}
#if defined(CONFIG_MTK_LM_MODE)
#define MTK_XHCI_DMA_BIT_MASK DMA_BIT_MASK(64)
#else
#define MTK_XHCI_DMA_BIT_MASK DMA_BIT_MASK(32)
#endif

static u64 dummy_mask = MTK_XHCI_DMA_BIT_MASK;

static struct platform_device xhci_platform_dev = {
		.name = hcd_name,
		.id   = -1,
        .dev = {
//                                      
				.coherent_dma_mask = MTK_XHCI_DMA_BIT_MASK,
				.release = xhci_hcd_release,
        },
};

#if 0
#define U3_MAC_TX_FIFO_WAIT_EMPTY_ADDR	0xf0041144

void setMacFIFOWaitEmptyValue(){
        __u32 __iomem *mac_tx_fifo_wait_empty_addr;
        u32 mac_tx_fifo_wait_empty_value;
        mac_tx_fifo_wait_empty_addr = U3_MAC_TX_FIFO_WAIT_EMPTY_ADDR;
        mac_tx_fifo_wait_empty_value = 0x5;
        writel(mac_tx_fifo_wait_empty_value, mac_tx_fifo_wait_empty_addr);
}
#endif


//                                                                                       
void mtktest_setInitialReg(){
	__u32 __iomem *addr;
	u32 temp;
	int num_u3_port;

	num_u3_port = SSUSB_U3_PORT_NUM(readl((void __iomem *)SSUSB_IP_CAP));

	printk("[OTG_H] mtktest_setInitialReg , num_u3_port = %d\n", num_u3_port);
	//                                          
	if(num_u3_port ){
		//                             
		addr = SSUSB_U3_MAC_BASE+U3_UX_EXIT_LFPS_TIMING_PAR;
		temp = readl(addr);
		temp &= ~(0xff << U3_RX_UX_EXIT_LFPS_REF_OFFSET);
		temp |= (U3_RX_UX_EXIT_LFPS_REF << U3_RX_UX_EXIT_LFPS_REF_OFFSET);
		writel(temp, addr);
		addr = SSUSB_U3_MAC_BASE+U3_REF_CK_PAR;
		temp = readl(addr);
		temp &= ~(0xff);
		temp |= U3_REF_CK_VAL;
		writel(temp, addr);

		//          
		addr = SSUSB_U3_SYS_BASE+U3_TIMING_PULSE_CTRL;
		temp = readl(addr);
		temp &= ~(0xff);
		temp |= CNT_1US_VALUE;
		writel(temp, addr);
	}

	addr = SSUSB_U2_SYS_BASE+USB20_TIMING_PARAMETER;
	temp &= ~(0xff);
	temp |= TIME_VALUE_1US;
	writel(temp, addr);

	//                                                   
	if(num_u3_port ){
		//                   
		addr = SSUSB_U3_SYS_BASE+LINK_PM_TIMER;
		temp = readl(addr);
		temp &= ~(0xf);
		temp |= PM_LC_TIMEOUT_VALUE;
		writel(temp, addr);
	}

	u3phy_init();
}

void mtktest_setLatchSel(){
	__u32 __iomem *latch_sel_addr;
	u32 latch_sel_value;

	if(g_num_u3_port <= 0)
		return;

	latch_sel_addr = U3_PIPE_LATCH_SEL_ADD;
	latch_sel_value = ((U3_PIPE_LATCH_TX)<<2) | (U3_PIPE_LATCH_RX);
	writel(latch_sel_value, latch_sel_addr);
}

void mtktest_reinitIP(){
	__u32 __iomem *ip_reset_addr;
	u32 ip_reset_value;

	//               
	//                                               
	//           
	//                               
	//            
	
	//                                               
	mtktest_enableAllClockPower();
	/*                     */
	mtktest_setLatchSel();
	mtktest_mtk_xhci_scheduler_init();
	
}

int mtk_xhci_hcd_init(void)
{
	int retval = 0;
	__u32 __iomem *ip_reset_addr;
	u32 ip_reset_value;
	struct platform_device *pPlatformDev;

	printk(KERN_ERR "Module Init start!\n");
	//               
	mtktest_reinitIP();

	retval = platform_driver_register(&xhci_versatile_driver);
	if (retval < 0)
	{
		printk(KERN_ERR "Problem registering platform driver.");
		return retval;
	}

	pPlatformDev = &xhci_platform_dev;
	memset(pPlatformDev, 0, sizeof(struct platform_device));
	pPlatformDev->name = hcd_name;
	pPlatformDev->id = -1;
    pPlatformDev->dev.coherent_dma_mask = MTK_XHCI_DMA_BIT_MASK;
    pPlatformDev->dev.release = xhci_hcd_release;
	retval = platform_device_register(&xhci_platform_dev);
	if (retval < 0)
    {
        platform_driver_unregister (&xhci_versatile_driver);
    }
	printk(KERN_ERR "Module Init success!\n");
	//                        

	mtktest_mtk_xhci_eint_iddig_init() ;
	/*
                                                                      
                                             
  */
	BUILD_BUG_ON(sizeof(struct xhci_doorbell_array) != 256*32/8);
	BUILD_BUG_ON(sizeof(struct xhci_slot_ctx) != 8*32/8);
	BUILD_BUG_ON(sizeof(struct xhci_ep_ctx) != 8*32/8);
	/*                                               
                                               
  */
	BUILD_BUG_ON(sizeof(struct xhci_stream_ctx) != 4*32/8);
	BUILD_BUG_ON(sizeof(union xhci_trb) != 4*32/8);
	BUILD_BUG_ON(sizeof(struct xhci_erst_entry) != 4*32/8);
	BUILD_BUG_ON(sizeof(struct xhci_cap_regs) != 7*32/8);
	BUILD_BUG_ON(sizeof(struct xhci_intr_reg) != 8*32/8);
	/*                                                              */
	BUILD_BUG_ON(sizeof(struct xhci_run_regs) != (8+8*128)*32/8);
	BUILD_BUG_ON(sizeof(struct xhci_doorbell_array) != 256*32/8);
	return 0;
}

void mtk_xhci_hcd_cleanup(void)
{
	//                      
	uint32_t nCount;
    uint32_t i;
    struct platform_device *pPlatformDev;

	mtktest_mtk_xhci_eint_iddig_deinit() ;
	
	platform_device_unregister(&xhci_platform_dev);
	platform_driver_unregister(&xhci_versatile_driver);
}

