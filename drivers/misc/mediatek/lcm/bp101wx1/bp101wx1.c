#ifndef BUILD_LK
#include <linux/string.h>
#endif
#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <debug.h>
#elif (defined BUILD_UBOOT)
#include <asm/arch/mt6577_gpio.h>
#else
#include <mach/mt_gpio.h>
#include <linux/xlog.h>
#include "mt8193_iic.h"
#endif
#include "lcm_drv.h"
#include "mt8193_lvds.h"

#define GPIO_DISP_LEVEL_SHIFT_EN     GPIO177
#define GPIO_DISP_3V3_EN             GPIO187
#define GPIO_DISP_VLED_EN            GPIO142
//                                                                            
//                 
//                                                                            

#define FRAME_WIDTH  (1280)
#define FRAME_HEIGHT (800)

#define HSYNC_PULSE_WIDTH 20 
#define HSYNC_BACK_PORCH  20
#define HSYNC_FRONT_PORCH 10
#define VSYNC_PULSE_WIDTH 6
#define VSYNC_BACK_PORCH  4
#define VSYNC_FRONT_PORCH 4

#define LCD_DATA_FORMAT LCD_DATA_FORMAT_VESA8BIT 

#define V_TOTAL (FRAME_HEIGHT + VSYNC_PULSE_WIDTH + VSYNC_BACK_PORCH + VSYNC_FRONT_PORCH)
#define H_TOTAL (FRAME_WIDTH + HSYNC_PULSE_WIDTH + HSYNC_BACK_PORCH + HSYNC_FRONT_PORCH)

#define H_START (HSYNC_PULSE_WIDTH + HSYNC_BACK_PORCH)
#define H_END (H_START + FRAME_WIDTH - 1)

#define V_START (VSYNC_PULSE_WIDTH + VSYNC_BACK_PORCH)
#define V_END (V_START + FRAME_HEIGHT - 1)

#define V_DELAY  0x0002  //           
#define H_DELAY  (FRAME_WIDTH + HSYNC_PULSE_WIDTH + HSYNC_BACK_PORCH + HSYNC_FRONT_PORCH - V_DELAY)

#ifdef BUILD_LK
#define MT8193_REG_WRITE(add, data) mt8193_reg_i2c_write(add, data)
#define MT8193_REG_READ(add) mt8193_reg_i2c_read(add)
#elif (defined BUILD_UBOOT)
    //                    
#else
extern int mt8193_i2c_write(u16 addr, u32 data);
extern int mt8193_i2c_read(u16 addr, u32 *data);

#define MT8193_REG_WRITE(add, data) mt8193_i2c_write(add, data)
#define MT8193_REG_READ(add) lcm_mt8193_i2c_read(add)
#endif
//                                                                            
//                 
//                                                                            

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (mt_set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


//                                                                            
//                 
//                                                                            

static __inline void send_ctrl_cmd(unsigned int cmd)
{

}

static __inline void send_data_cmd(unsigned int data)
{

}

static __inline void set_lcm_register(unsigned int regIndex,
                                      unsigned int regData)
{

}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
    mt_set_gpio_mode(GPIO, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO, (output>0)? GPIO_OUT_ONE: GPIO_OUT_ZERO);
}
//                                                                            
//                            
//                                                                            

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

#ifdef BUILD_LK
#define I2C_CH                2
#define MT8193_I2C_ADDR       0x3A
int mt8193_reg_i2c_write(u16 addr, u32 data)
{
    u8 buffer[8];
    u8 lens;
	U32 ret_code = 0;

    if(((addr >> 8) & 0xFF) >= 0x80) //                  
    {
        buffer[0] = (addr >> 8) & 0xFF;
        buffer[1] = (data >> 24) & 0xFF;
        buffer[2] = (data >> 16) & 0xFF;
        buffer[3] = (data >> 8) & 0xFF;
        buffer[4] = data & 0xFF;
        lens = 5;
    }
    else //                     
    {
        buffer[0] = (addr >> 8) & 0xFF;
        buffer[1] = addr & 0xFF;
        buffer[2] = (data >> 24) & 0xFF;
        buffer[3] = (data >> 16) & 0xFF;
        buffer[4] = (data >> 8) & 0xFF;
        buffer[5] = data & 0xFF;        
        lens = 6;
    }

	ret_code = mt_i2c_write(I2C_CH, MT8193_I2C_ADDR, buffer, lens, 0); //                  
    if (ret_code != 0)
    {
        printf("[LK/LCM] mt_i2c_write reg[0x%X] fail, Error code [0x%X] \n", addr, ret_code);
        return ret_code;
    }
	
	return 0;
}

u32 mt8193_reg_i2c_read(u16 addr)
{
    
    u8 buffer[8] = {0};
    u8 lens;
    u32 ret_code = 0;
    u32 data;

    if(((addr >> 8) & 0xFF) >= 0x80) //                  
    {
        buffer[0] = (addr >> 8) & 0xFF;
        lens = 1;
    }
    else //                     
    {
        buffer[0] = ( addr >> 8 ) & 0xFF;
        buffer[1] = addr & 0xFF;     
        lens = 2;
    }

    ret_code = mt_i2c_write(I2C_CH, MT8193_I2C_ADDR, buffer, lens, 0);    //                     
    if (ret_code != 0)
        return ret_code;

    lens = 4;
    ret_code = mt_i2c_read(I2C_CH, MT8193_I2C_ADDR, buffer, lens, 0);
    if (ret_code != 0)
    {
        return ret_code;
    }
    
    data = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | (buffer[0]); //         

    return data;
    
}
#elif (defined BUILD_UBOOT)
    //                    
#else
u32 lcm_mt8193_i2c_read(u16 addr)
{
   u32 u4Reg = 0;
   u32 ret_code = 0;
	
   ret_code = mt8193_i2c_read(addr, &u4Reg);
   if (ret_code != 0)
   {
	   return ret_code;
   }

   return u4Reg;
}
#endif

static void lcm_mt8193_set_ckgen(void)
{
    u32 u4Reg = 0;
	
	u4Reg = MT8193_REG_READ(REG_PLL_GPANACFG0);
	u4Reg&=(~(0x7c000000));  //               
	u4Reg |= (RG_PLL1_FBDIV2 | RG_PLL1_PREDIV | RG_PLL1_RST_DLY | RG_PLL1_LF | RG_PLL1_MONCKEN | RG_PLL1_VODEN | RG_NFIPLL_EN);
    MT8193_REG_WRITE(REG_PLL_GPANACFG0, u4Reg);
	
    MT8193_REG_WRITE(RG_LVDSWRAP_CTRL1, (RG_DCXO_POR_MON_EN | RG_PLL1_DIV2));
	UDELAY(200);
		
    MT8193_REG_WRITE(REG_LVDS_ANACFG2, (RG_VPLL_BC | RG_VPLL_BIC | RG_VPLL_BIR | RG_VPLL_BP | RG_VPLL_BR));
	
	u4Reg = 0;
	u4Reg |= (RG_VPLL_DIV | RG_VPLL_DPIX_CKSEL | RG_LVDS_DELAY | RG_VPLL_MKVCO |RG_VPLL_POSTDIV_EN);
	MT8193_REG_WRITE(REG_LVDS_ANACFG3, u4Reg);
	UDELAY(200);
	u4Reg &= (~(RG_VPLL_POSTDIV_EN));
	MT8193_REG_WRITE(REG_LVDS_ANACFG3, u4Reg);	
}

static void lcm_mt8193_set_lvds_analog(void)
{
    MT8193_REG_WRITE(REG_LVDS_ANACFG4, (RG_BYPASS | RG_LVDS_BYPASS));
    MT8193_REG_WRITE(REG_LVDS_ANACFG0, (RG_LVDS_ATERM_EN | RG_LVDS_APSRC | RG_LVDS_ANSRC | RG_LVDS_ATVCM | RG_LVDS_ATVO));
    MT8193_REG_WRITE(REG_LVDS_ANACFG1, 0x00000000);
}

static void lcm_mt8193_ckgen_power_on(void)
{
    u32 u4Reg = 0;

	u4Reg = MT8193_REG_READ(REG_PLL_GPANACFG0);
	u4Reg |= RG_PLL1_EN;
	MT8193_REG_WRITE(REG_PLL_GPANACFG0, u4Reg);	
    UDELAY(200);
	
	u4Reg = MT8193_REG_READ(REG_LVDS_ANACFG2);
	u4Reg &= (~(RG_VPLL_BG_PD | RG_VPLL_BIAS_PD));
	MT8193_REG_WRITE(REG_LVDS_ANACFG2, u4Reg);	
    UDELAY(200);
	
	u4Reg = MT8193_REG_READ(REG_LVDS_ANACFG4);
	u4Reg &= (~(RG_VPLL_PD));
	MT8193_REG_WRITE(REG_LVDS_ANACFG4, u4Reg);
    UDELAY(200);
	u4Reg &= (~(RG_VPLL_RST));
	MT8193_REG_WRITE(REG_LVDS_ANACFG4, u4Reg);
	UDELAY(200);
}

static void lcm_mt8193_ckgen_power_off(void)
{
    u32 u4Reg = 0;
	
	u4Reg = MT8193_REG_READ(REG_LVDS_ANACFG4);
	u4Reg |= (RG_VPLL_RST);	
	MT8193_REG_WRITE(REG_LVDS_ANACFG4, u4Reg);
	UDELAY(200);
	u4Reg |= (RG_VPLL_PD);
	MT8193_REG_WRITE(REG_LVDS_ANACFG4, u4Reg);
	UDELAY(200);
		
	u4Reg = MT8193_REG_READ(REG_LVDS_ANACFG2);
	u4Reg |= (RG_VPLL_BG_PD | RG_VPLL_BIAS_PD);
	MT8193_REG_WRITE(REG_LVDS_ANACFG2, u4Reg);
	UDELAY(200);

	u4Reg = MT8193_REG_READ(REG_PLL_GPANACFG0);
	u4Reg &= (~(RG_PLL1_EN));
	MT8193_REG_WRITE(REG_PLL_GPANACFG0, u4Reg);	
    UDELAY(200);	
}

static void lcm_mt8193_lvds_analog_power_on(void)
{
    u32 u4Reg = 0;

	u4Reg = MT8193_REG_READ(REG_LVDS_ANACFG0);		
	u4Reg &= (~(RG_LVDS_APD | RG_LVDS_BIASA_PD));
	MT8193_REG_WRITE(REG_LVDS_ANACFG0, u4Reg);
	UDELAY(200);
}

static void lcm_mt8193_lvds_analog_power_off(void)
{
    u32 u4Reg = 0;

	u4Reg = MT8193_REG_READ(REG_LVDS_ANACFG0);
	u4Reg |= (RG_LVDS_APD | RG_LVDS_BIASA_PD);
	MT8193_REG_WRITE(REG_LVDS_ANACFG0, u4Reg);
	UDELAY(200);
}

static void lcm_mt8193_set_lvds_digital(void)
{
    MT8193_REG_WRITE(LVDS_CLK_CTRL, (RG_TEST_CK_EN | RG_RX_CK_EN | RG_TX_CK_EN));
	MT8193_REG_WRITE(LVDS_CH_SWAP, RG_SWAP_SEL);
	MT8193_REG_WRITE(LVDS_CLK_RESET, (RG_CTSCLK_RESET_B | RG_PCLK_RESET_B));	
}

static void lcm_mt8193_set_dgi0(void)
{	
	MT8193_REG_WRITE(DGI0_DATA_OUT_CTRL, DATA_OUT_SWAP);
	MT8193_REG_WRITE(DGI0_TG_CTRL00, PRGS_OUT);
	MT8193_REG_WRITE(DGI0_TG_CTRL02, ((V_TOTAL<<16) | H_TOTAL));
	MT8193_REG_WRITE(DGI0_TG_CTRL03, ((VSYNC_PULSE_WIDTH<<16) | HSYNC_PULSE_WIDTH));
	MT8193_REG_WRITE(DGI0_TG_CTRL05, ((H_START<<16) | H_END));	
	MT8193_REG_WRITE(DGI0_TG_CTRL06, ((V_START<<16) | V_END));
	MT8193_REG_WRITE(DGI0_TG_CTRL07, ((V_START<<16) | V_END));	
	MT8193_REG_WRITE(DGI0_TG_CTRL01, ((V_DELAY<<16) | H_DELAY));	
	MT8193_REG_WRITE(DGI0_TTL_ANAIF_CTRL, TTL_CLK_INV_ENABLE);
	MT8193_REG_WRITE(DGI0_TTL_ANAIF_CTRL1, PAD_TTL_EN_PP);
}

static void lcm_mt8193_anaif_clock_enable(void)
{
    MT8193_REG_WRITE(DGI0_ANAIF_CTRL0, DGI0_PAD_CLK_ENABLE);
}

static void lcm_mt8193_anaif_clock_disable(void)
{
    MT8193_REG_WRITE(DGI0_ANAIF_CTRL0, DGI0_PAD_CLK_DISABLE);
}

static void lcm_mt8193_dgi0_clock_enable(void)
{
    MT8193_REG_WRITE(DGI0_CLK_RST_CTRL, (DGI0_CLK_OUT_ENABLE | DGI0_CLK_IN_INV_ENABLE | DGI0_CLK_IN_ENABLE));
}

static void lcm_mt8193_dgi0_clock_disable(void)
{
    MT8193_REG_WRITE(DGI0_CLK_RST_CTRL, DGI0_CLK_OUT_DISABLE);
}

static void lcm_mt8193_dgi0_fifo_write_disable(void)
{
    MT8193_REG_WRITE(DGI0_DEC_CTRL, 0x0);
}

static void lcm_mt8193_dgi0_fifo_write_enable(void)
{
    MT8193_REG_WRITE(DGI0_DEC_CTRL, FIFO_WRITE_EN);
}

static void lcm_mt8193_sw_reset(void)
{
    MT8193_REG_WRITE(DGI0_FIFO_CTRL, (SW_RST | FIFO_RESET_ON | RD_START));
	UDELAY(200);
	MT8193_REG_WRITE(DGI0_FIFO_CTRL, (FIFO_RESET_ON | RD_START));
}

static void lcm_mt8193_lvds_power_off(void)
{
    MT8193_REG_WRITE(REG_LVDS_PWR_CTRL, 0x00000006);
	UDELAY(200);
	MT8193_REG_WRITE(REG_LVDS_PWR_CTRL, 0x00000007);
	UDELAY(200);
    MT8193_REG_WRITE(REG_LVDS_PWR_RST_B, 0x00000000);
	UDELAY(200);
	MT8193_REG_WRITE(REG_LVDS_PWR_CTRL, 0x00000005);
}

static void lcm_mt8193_lvds_power_on(void)
{
    MT8193_REG_WRITE(REG_LVDS_PWR_CTRL, 0x00000007);
	UDELAY(200);
	MT8193_REG_WRITE(REG_LVDS_PWR_RST_B, 0x00000001);
	UDELAY(200);
    MT8193_REG_WRITE(REG_LVDS_PWR_CTRL, 0x00000006);
	UDELAY(200);
	MT8193_REG_WRITE(REG_LVDS_PWR_CTRL, 0x00000002);
}

static void lcm_mt8193_lvds_top_clock_disable(void)
{
    MT8193_REG_WRITE(LVDS_CLK_CTRL, 0x0);
}

static void lcm_mt8193_lvds_top_clock_enable(void)
{
    MT8193_REG_WRITE(LVDS_CLK_CTRL, (RG_TEST_CK_EN | RG_RX_CK_EN | RG_TX_CK_EN));
}

static void lcm_mt8193_lvds_out_disable(void)
{
    MT8193_REG_WRITE(LVDS_OUTPUT_CTRL, 0x0);
}

static void lcm_mt8193_lvds_out_enable(void)
{
	MT8193_REG_WRITE(LVDS_OUTPUT_CTRL, (RG_LVDSRX_FIFO_EN | RG_SYNC_TRIG_MODE | RG_OUT_FIFO_EN | RG_LVDS_E));
}

static void lcm_mt8193_enable_output(void)
{
	lcm_mt8193_lvds_power_on();		
	lcm_mt8193_anaif_clock_enable();
	lcm_mt8193_set_ckgen();
	lcm_mt8193_ckgen_power_on();
	lcm_mt8193_dgi0_clock_enable();
	lcm_mt8193_dgi0_fifo_write_disable();
	lcm_mt8193_set_dgi0();
	lcm_mt8193_dgi0_fifo_write_enable();
	lcm_mt8193_lvds_top_clock_enable();
	lcm_mt8193_lvds_out_disable();
	lcm_mt8193_set_lvds_digital();
	lcm_mt8193_lvds_analog_power_on();
	lcm_mt8193_set_lvds_analog();
	MDELAY(1);
	lcm_mt8193_lvds_out_enable();	
	lcm_mt8193_sw_reset();  
}

static void lcm_mt8193_disable_output(void)
{
	lcm_mt8193_lvds_analog_power_off();		
	lcm_mt8193_lvds_out_disable();
	lcm_mt8193_lvds_top_clock_disable();
	lcm_mt8193_dgi0_fifo_write_disable();
    lcm_mt8193_dgi0_clock_disable();
	lcm_mt8193_ckgen_power_off();	
    lcm_mt8193_anaif_clock_disable();
    lcm_mt8193_lvds_power_off(); 
}

static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type   = LCM_TYPE_DPI;
    params->ctrl   = LCM_CTRL_SERIAL_DBI;
    params->width  = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    params->io_select_mode = 0;	

    /*                              */    
    params->dpi.mipi_pll_clk_ref  = 0;
    params->dpi.mipi_pll_clk_div1 = 0x80000081;  //             
    params->dpi.mipi_pll_clk_div2 = 0x800a0000;
    params->dpi.dpi_clk_div       = 2;           //                
    params->dpi.dpi_clk_duty      = 1;

    params->dpi.clk_pol           = LCM_POLARITY_FALLING;
    params->dpi.de_pol            = LCM_POLARITY_RISING;
    params->dpi.vsync_pol         = LCM_POLARITY_FALLING;
    params->dpi.hsync_pol         = LCM_POLARITY_FALLING;

    params->dpi.hsync_pulse_width = HSYNC_PULSE_WIDTH;
    params->dpi.hsync_back_porch  = HSYNC_BACK_PORCH;
    params->dpi.hsync_front_porch = HSYNC_FRONT_PORCH;
    params->dpi.vsync_pulse_width = VSYNC_PULSE_WIDTH;
    params->dpi.vsync_back_porch  = VSYNC_BACK_PORCH;
    params->dpi.vsync_front_porch = VSYNC_FRONT_PORCH;
    
    params->dpi.i2x_en = 1;
    
    params->dpi.format            = LCM_DPI_FORMAT_RGB888;   //                 
    params->dpi.rgb_order         = LCM_COLOR_ORDER_RGB;
    params->dpi.is_serial_output  = 0;

    params->dpi.intermediat_buffer_num = 0;

    params->dpi.io_driving_current = LCM_DRIVING_CURRENT_2MA;
}


static void lcm_init(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_init()  \n");
#elif (defined BUILD_UBOOT)
    //                    
#else
    printk("[LCM] lcm_init() enter\n");

#endif
}


static void lcm_suspend(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_suspend() enter\n");

	lcm_set_gpio_output(GPIO_DISP_VLED_EN, 0);
	MDELAY(1);
	lcm_set_gpio_output(GPIO_DISP_3V3_EN, 0);
	MDELAY(5);
	lcm_set_gpio_output(GPIO_DISP_LEVEL_SHIFT_EN, 0);
	MDELAY(10);
	
    lcm_mt8193_disable_output();	
#elif (defined BUILD_UBOOT)
		//                    
#else
    printk("[LCM] lcm_suspend() enter\n");

	lcm_set_gpio_output(GPIO_DISP_VLED_EN, 0);
	MDELAY(1);
	lcm_set_gpio_output(GPIO_DISP_3V3_EN, 0);
	MDELAY(5);
	lcm_set_gpio_output(GPIO_DISP_LEVEL_SHIFT_EN, 0);
	MDELAY(10);

	lcm_mt8193_disable_output();
#endif
}


static void lcm_resume(void)
{
#ifdef BUILD_LK
	printf("[LK/LCM] lcm_resume() enter\n");
	
	lcm_set_gpio_output(GPIO_DISP_LEVEL_SHIFT_EN, 1);
	MDELAY(1);
	lcm_set_gpio_output(GPIO_DISP_3V3_EN, 1);	

	lcm_mt8193_enable_output();	

	MDELAY(200);
	lcm_set_gpio_output(GPIO_DISP_VLED_EN, 1);	
#elif (defined BUILD_UBOOT)
		//                    
#else
	printk("[LCM] lcm_resume() enter\n");

	lcm_set_gpio_output(GPIO_DISP_LEVEL_SHIFT_EN, 1);
	MDELAY(1);
	lcm_set_gpio_output(GPIO_DISP_3V3_EN, 1);

	lcm_mt8193_enable_output();

	MDELAY(200);
	lcm_set_gpio_output(GPIO_DISP_VLED_EN, 1);
#endif

}

LCM_DRIVER bp101wx1_lcm_drv = 
{
    .name		    = "bp101wx1",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
};

