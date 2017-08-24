#include <mach/mt_clkmgr.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/musb/mtk_musb.h>
#include <linux/musb/musb_core.h>
#include "usb20.h"

#include <mach/mt_gpio.h>

#define FRA (48)
#define PARA (28)

#ifdef CONFIG_MTK_UART_USB_SWITCH
extern u32 usb_port_mode_temp = 0;
#endif

extern void mtk_uart_usb_rx_sel(unsigned int uart_port, unsigned int enable);

#ifdef FPGA_PLATFORM
bool usb_enable_clock(bool enable)
{
	return true;
}

void usb_phy_poweron(void)
{
}

void usb_phy_savecurrent(void)
{
}

void usb_phy_recover(void)
{
}

//      
void Charger_Detect_Init(void)
{
}

void Charger_Detect_Release(void)
{
}

void usb_phy_context_save(void)
{
}

void usb_phy_context_restore(void)
{
}

#ifdef CONFIG_MTK_UART_USB_SWITCH
bool usb_phy_check_in_uart_mode()
{
	UINT8 usb_port_mode;
    usb_enable_clock(true);
    udelay(50);

	usb_port_mode = USB_PHY_Read_Register8(0x6B);
    usb_enable_clock(false);

	if ((usb_port_mode == 0x5C) ||
		(usb_port_mode == 0x5E))
		return true;
	else
		return false;
}

void usb_phy_switch_to_uart(void)
{

	if (usb_phy_check_in_uart_mode()) {
		return;
	}
	//            
	printk("%s, line %d: force to uart mode!!\n", __func__, __LINE__);
	//            

    usb_enable_clock(true);
	udelay(50);

	/*                            */
	USBPHY_CLR8(0x1a, 0x80);

	/*                      */
	USBPHY_SET8(0x68, 0x08);

	/*                    */
	USBPHY_SET8(0x6a, 0x04);

	/*                           */
	USBPHY_SET8(0x6B, 0x5C);

	/*                     */
	USBPHY_SET8(0x6E, 0x07);

	/*                              */
	USBPHY_SET8(0x22, 0x02);
    usb_enable_clock(false);
}

void usb_phy_switch_to_usb(void)
{
	usb_enable_clock(true);
    udelay(50);
	/*                     */
	USBPHY_WRITE8(0x6B, 0x00);
    usb_enable_clock(false);
	usb_phy_poweron();
	/*                                                      */
    usb_enable_clock(false);
}
#endif

#else

#ifdef CONFIG_MTK_UART_USB_SWITCH
static bool in_uart_mode = false;
#endif

static DEFINE_SPINLOCK(musb_reg_clock_lock);

static void enable_phy_clock(bool enable)
{
    /*                                      */
    #if 0
    if (enable) {
        writel(readl((void __iomem *)UNIVPLL_CON0)|(0x04000000),
			   (void __iomem *)UNIVPLL_CON0);
    } else {
        writel(readl((void __iomem *)UNIVPLL_CON0)&~(0x04000000),
			   (void __iomem *)UNIVPLL_CON0);
    }
    #endif
}

bool usb_enable_clock(bool enable)
{
	static int count = 0;
	bool res1 = TRUE;
    bool res2 = TRUE;
    bool res3 = TRUE;
	unsigned long flags;

	spin_lock_irqsave(&musb_reg_clock_lock, flags);

	if (enable && count == 0) {
		enable_phy_clock(true);
		res1 = enable_clock(MT_CG_INFRA_USB, "INFRA_USB");
        res2 = enable_clock(MT_CG_INFRA_USB_MCU, "INFRA_USB_MCU");
        res3 = enable_clock(MT_CG_INFRA_ICUSB, "INFRA_ICUSB");
	} else if (!enable && count == 1) {
        res2 = disable_clock(MT_CG_INFRA_USB_MCU, "INFRA_USB_MCU");
		res1 = disable_clock(MT_CG_INFRA_USB, "INFRA_USB");
        res3 = disable_clock(MT_CG_INFRA_ICUSB, "INFRA_ICUSB");
		enable_phy_clock(false);
	}

	if (enable)
		count++;
	else
		count = (count==0) ? 0 : (count-1);

	spin_unlock_irqrestore(&musb_reg_clock_lock, flags);

	printk(KERN_DEBUG "enable(%d), count(%d) res1=%d, res2=%d\n", enable, count, res1, res2);
	return 1;
}

static void hs_slew_rate_cal(void){
  unsigned long data;
  unsigned long x;
  unsigned char value;
  unsigned long start_time, timeout;
  unsigned int timeout_flag = 0;
  //                                
  USBPHY_WRITE8(0x15,0x80);

  //              
  udelay(1);

  //                          
  USBPHY_WRITE8 (0xf00-0x800+0x11,0x01);
  //                      
  USBPHY_WRITE8 (0xf00-0x800+0x01,0x04);
  //                           
  USBPHY_SET8 (0xf00-0x800+0x03,0x01);

  //                              
  start_time = jiffies;
  timeout = jiffies + 3 * HZ;

  while(!(USBPHY_READ8(0xf00-0x800+0x10)&0x1)){
    if(time_after(jiffies, timeout)){
        timeout_flag = 1;
        break;
        }
    }

  //                  
  if(timeout_flag){
    printk("[USBPHY] Slew Rate Calibration: Timeout\n");
    value = 0x4;
    }
  else{
      data = USBPHY_READ32 (0xf00-0x800+0x0c);
      x = ((1024*FRA*PARA)/data);
      value = (unsigned char)(x/1000);
      if((x-value*1000)/100>=5)
        value += 1;
        printk("[USBPHY]slew calibration:FM_OUT =%lu,x=%lu,value=%d\n",data,x,value);
    }

  //                                      
  USBPHY_CLR8 (0xf00-0x800+0x03,0x01);//                       
  USBPHY_CLR8 (0xf00-0x800+0x11,0x01);//                      

  //     
  USBPHY_WRITE8(0x15,value<<4);

  //                                  
  USBPHY_CLR8(0x15,0x80);
}

#ifdef CONFIG_MTK_UART_USB_SWITCH
bool usb_phy_check_in_uart_mode()
{
	UINT8 usb_port_mode;

    usb_enable_clock(true);
    udelay(50);
	usb_port_mode = USBPHY_READ8(0x6B);
	//                  
    usb_enable_clock(false);

	if ((usb_port_mode == 0x5C) ||
		(usb_port_mode == 0x5E) || (usb_port_mode_temp == 1))
	{
        usb_port_mode_temp = 1;
		return true;
	}
	else
		return false;
}

void usb_phy_switch_to_uart(void)
{
	if (usb_phy_check_in_uart_mode()) {
		return;
	}
    usb_enable_clock(true);
	udelay(50);

	/*                            */
	USBPHY_CLR8(0x1a, 0x80);

	/*                      */
	USBPHY_SET8(0x68, 0x08);

	/*                    */
	USBPHY_SET8(0x6a, 0x04);

	/*                           */
	USBPHY_SET8(0x6B, 0x5C);

	/*                     */
	USBPHY_SET8(0x6E, 0x07);

	/*                              */
	USBPHY_SET8(0x22, 0x02);
    usb_enable_clock(false);

  /*                */  
  mtk_uart_usb_rx_sel(1, 1);
  usb_port_mode_temp = 1;
  printk("usb port value in uart function:%d\n", usb_port_mode_temp);
	/*                */
	//                                              
}


void usb_phy_switch_to_usb(void)
{
	/*                */
	//                                               
    mtk_uart_usb_rx_sel(1, 0);
    usb_enable_clock(true);
    udelay(50);
	/*                     */
	USBPHY_WRITE8(0x6B, 0x00);
    usb_enable_clock(false);
	usb_phy_poweron();
	/*                                                      */
    usb_enable_clock(false);

    usb_port_mode_temp = 0;
    printk("usb port value in usb function:%d\n", usb_port_mode_temp);
}
#endif

void usb_phy_poweron(void){

	#ifdef CONFIG_MTK_UART_USB_SWITCH
	if (usb_phy_check_in_uart_mode())
		return;
	#endif

    //                           
    usb_enable_clock(true);

    //                                           
    udelay(50);

    //                                                                       
    USBPHY_CLR8(0x6b, 0x04);
    USBPHY_CLR8(0x6e, 0x01);
    USBPHY_CLR8(0x21, 0x03);
    //                        

    //                              
    USBPHY_CLR8(0x1a, 0x80);

    //                                                        
    USBPHY_SET8(0x22, 0x04);
    USBPHY_CLR8(0x22, 0x03);

    /*          */
    USBPHY_SET8(0x20, 0x10);
    //                             
    USBPHY_CLR8(0x6a, 0x04);

    //                                        
    USBPHY_SET8(0x18, 0x06);

    //                        
    udelay(800);

    //                                            
    USBPHY_CLR8(0x6c, 0x10);
    USBPHY_SET8(0x6c, 0x2E);
    USBPHY_SET8(0x6d, 0x3E);

    printk("usb power on success\n");
}

#ifdef CONFIG_MTK_UART_USB_SWITCH
static bool skipDisableUartMode = false;
#endif

static void usb_phy_savecurrent_internal(void){

    //                                                                      

	#ifdef CONFIG_MTK_UART_USB_SWITCH
	if (!usb_phy_check_in_uart_mode()) {
		//                           
		usb_enable_clock(true);

		//                                           
		udelay(50);

		USBPHY_CLR8(0x6b, 0x04);
		USBPHY_CLR8(0x6e, 0x01);

		USBPHY_CLR8(0x21, 0x03);

		//                            
   //                         
    USBPHY_SET8(0x6a, 0x04);
    USBPHY_SET8(0x68, 0x08);
		usb_enable_clock(false);
	} else {
		if (skipDisableUartMode)
			skipDisableUartMode = false;
		else
			return;
	}
	#else
    USBPHY_CLR8(0x6b, 0x04);
    USBPHY_CLR8(0x6e, 0x01);
    USBPHY_CLR8(0x21, 0x03);

    //                            
   //                         
    USBPHY_SET8(0x6a, 0x04);
    USBPHY_SET8(0x68, 0x08);
	#endif

    //                                  
    USBPHY_SET8(0x68, 0xc0);
    //                            
    USBPHY_CLR8(0x68, 0x30);
    USBPHY_SET8(0x68, 0x10);
    //                      
	USBPHY_SET8(0x68, 0x04);
    //                           
	USBPHY_CLR8(0x69, 0x3c);
    //                                                                       
    USBPHY_SET8(0x6a, 0xba);

    //                            
    USBPHY_CLR8(0x1a, 0x80);
    //                                 
    USBPHY_CLR8(0x1a, 0x10);
    //                  
    udelay(800);
    //                             
    USBPHY_CLR8(0x68, 0x08);
    //                        

//                                                   
   //                                                                          
   //                                                                          
//                                                   

    udelay(1);
    //                          
   //                         
    //               
    udelay(1);

    //                        
   //                         
   //                         
   //                         
}

void usb_phy_savecurrent(void){

    usb_phy_savecurrent_internal();
    //                                  
    usb_enable_clock(false);
    printk("usb save current success\n");
}

void usb_phy_recover(void){

    //                                 
    usb_enable_clock(true);
    //                  
    udelay(50);

	#ifdef CONFIG_MTK_UART_USB_SWITCH
	if (!usb_phy_check_in_uart_mode()) {
    //                   
    //                    
    //                                        
    USBPHY_CLR8(0x1d, 0x10);

    //                         
    USBPHY_CLR8(0x6b, 0x04);
    //                      
    USBPHY_CLR8(0x6e, 0x1);
	//                            
    USBPHY_CLR8(0x6a, 0x04);
    USBPHY_CLR8(0x22, 0x02);

	skipDisableUartMode = false;
	} else {
		//                         
			return;
	}
	#else
    //                   
    //                    
    //                                        
    USBPHY_CLR8(0x1d, 0x10);

    //                         
    USBPHY_CLR8(0x6b, 0x04);
    //                      
    USBPHY_CLR8(0x6e, 0x1);
    //                         
    USBPHY_CLR8(0x6a, 0x04);

    USBPHY_CLR8(0x21, 0x03);
	#endif

    //                         
    USBPHY_CLR8(0x68, 0x40);
    //                         
    USBPHY_CLR8(0x68, 0x80);
    //                       
    USBPHY_CLR8(0x68, 0x30);
    //                      
    USBPHY_CLR8(0x68, 0x04);
    //                              
    USBPHY_CLR8(0x69, 0x3c);

   //                              
    USBPHY_CLR8(0x6a, 0x10);
    //                              
    USBPHY_CLR8(0x6a, 0x20);
    //                           
    USBPHY_CLR8(0x6a, 0x08);
    //                          
    USBPHY_CLR8(0x6a, 0x02);
    //                         
    USBPHY_CLR8(0x6a, 0x80);

    //                              
    USBPHY_CLR8(0x1a, 0x80);
    //                                   
    USBPHY_SET8(0x1a, 0x10);
    //                                       
    USBPHY_CLR8(0x18, 0x08);
    USBPHY_SET8(0x18, 0x06);

    //                    
    udelay(800);

    //                                  
    USBPHY_CLR8(0x6c, 0x10);
    USBPHY_SET8(0x6c, 0x2E);
    USBPHY_SET8(0x6d, 0x3E);

    #if defined(MTK_HDMI_SUPPORT)
    USBPHY_SET8(0x05, 0x05);
    USBPHY_SET8(0x05, 0x50);
    #endif
    hs_slew_rate_cal();

    printk("usb recovery success\n");
    return;
}

//      
void Charger_Detect_Init(void)
{
    //                            
    usb_enable_clock(true);
    //             
    udelay(50);
    /*                            */
    USBPHY_SET8(0x1a, 0x80);
    printk("Charger_Detect_Init\n");
}

void Charger_Detect_Release(void)
{
    /*                            */
    USBPHY_CLR8(0x1a, 0x80);
    udelay(1);
    //                                  
    usb_enable_clock(false);
    printk("Charger_Detect_Release\n");
}

void usb_phy_context_save(void)
{
#ifdef CONFIG_MTK_UART_USB_SWITCH
	in_uart_mode = usb_phy_check_in_uart_mode();
#endif
}

void usb_phy_context_restore(void)
{
#ifdef CONFIG_MTK_UART_USB_SWITCH
	if (in_uart_mode) {
		usb_phy_switch_to_uart();
	}
#endif
    usb_phy_savecurrent_internal();
}

#endif
