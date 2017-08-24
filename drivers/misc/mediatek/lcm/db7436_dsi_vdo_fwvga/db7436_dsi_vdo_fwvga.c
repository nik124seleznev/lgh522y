/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifdef BUILD_LK
#include <string.h>
#include <mt_gpio.h>
#include <platform/mt_pmic.h>
//                         
#include <platform/boot_mode.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include <linux/string.h>
#include <mach/mt_gpio.h>
#include <mach/mt_pm_ldo.h>
#include <mach/board_lge.h>
#endif

#include "lcm_drv.h"
#include <cust_gpio_usage.h>

#ifndef BUILD_LK
#include <mach/mt_pwm.h> 
#endif

#if defined(BUILD_LK)
#define LCM_PRINT printf
#elif defined(BUILD_UBOOT)
#define LCM_PRINT printf
#else
#define LCM_PRINT printk
#endif

#if 0  /*                                                                      */
#define INIT_CODE_TUNNING_BY_SD_CARD

#if (!defined(BUILD_UBOOT) && !defined(BUILD_LK))  && defined(INIT_CODE_TUNNING_BY_SD_CARD)
#include <linux/fs.h>
#include <linux/slab.h>
#endif
#endif  /*                                                                      */

#if defined(CONFIG_LEDS_LM3632) || defined(CONFIG_BACKLIGHT_LM3632)
extern void chargepump_dsv_ctrl(int dsv_en);
#endif

//                                                                            
//                 
//                                                                            
//      
#define FRAME_WIDTH  			(480)
#define FRAME_HEIGHT 			(854)
//                   
#define PHYSICAL_WIDTH        (55)
#define PHYSICAL_HIGHT         (99)


#define LCM_ID       (0x40)
#define LCM_DSI_CMD_MODE		0

#define REGFLAG_DELAY 0xAB
#define REGFLAG_END_OF_TABLE 0xAA //                        

/*
                 
*/
#ifndef GPIO_DSV_AVDD_EN
#define GPIO_DSV_AVDD_EN (GPIO107 | 0x80000000)
#define GPIO_DSV_AVDD_EN_M_GPIO GPIO_MODE_00
#define GPIO_DSV_AVDD_EN_M_KROW GPIO_MODE_06
#define GPIO_DSV_AVDD_EN_M_PWM GPIO_MODE_05
#endif

#ifndef GPIO_DSV_AVEE_EN
#define GPIO_DSV_AVEE_EN (GPIO106 | 0x80000000)
#define GPIO_DSV_AVEE_EN_M_GPIO GPIO_MODE_00
#define GPIO_DSV_AVEE_EN_M_KROW GPIO_MODE_06
#define GPIO_DSV_AVEE_EN_M_PWM GPIO_MODE_05
#endif

#ifndef GPIO_LCM_PWR
#define GPIO_LCM_PWR         (GPIO75 | 0x80000000)
#define GPIO_LCM_PWR_M_GPIO   GPIO_MODE_00
#define GPIO_LCM_PWR_M_KCOL   GPIO_MODE_01
#endif

//                                                                            
//                 
//                                                                            

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))
#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))

//                                                                            
//                 
//                                                                            

#define dsi_set_cmdq_V3(para_tbl, size, force_update)   	lcm_util.dsi_set_cmdq_V3(para_tbl, size, force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

static unsigned int need_set_lcm_addr = 1;

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

//             
static struct LCM_setting_table lcm_initialization_setting[] = {
    {0xB7, 7, {0x00, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C}},
    {0xB6, 6, {0x06, 0x0A, 0x34, 0x23, 0x41, 0x0A}},
    {0xB8, 4, {0x00, 0x42, 0x12, 0xF7}},
    {0xB0, 1, {0x00}},
    {0xB5, 10, {0x43, 0xA0, 0x01, 0x12, 0x06, 0x00, 0x00, 0x00, 0x00, 0x48}},
    {0xB4, 6, {0x01, 0x09, 0x02, 0x02, 0x02, 0x02}},  //           
    {0xB3, 3, {0x8B, 0x7F, 0x30}},
    {0xB2, 2, {0x00, 0x03}},
    {0xB1, 4, {0xC6, 0x1E, 0x0F, 0x00}},
    {0xB9, 3, {0x00, 0x03, 0x04}},
    {0xBB, 1, {0x35}},
    {0x36, 1, {0x00}},
    {0xD9, 1, {0xA0}},
    {0xF8, 2, {0x00, 0x06}},
    {0xD7, 2, {0x00, 0xB6}},
    {0xBD, 2, {0x03, 0x23}},
    {0xC5, 3, {0x3F, 0x00, 0x50}},
    {0xC4, 1, {0x00}},
    {0xD0, 9, {0x20, 0x54, 0x53, 0x11, 0x00, 0x00, 0x42, 0x03, 0x02}},
    {0xD1, 9, {0x20, 0x54, 0x53, 0x11, 0x08, 0x00, 0x42, 0x03, 0x02}},
    {0xD2, 9, {0x20, 0x54, 0x53, 0x11, 0x00, 0x00, 0x42, 0x03, 0x02}},
    {0xD3, 9, {0x20, 0x54, 0x53, 0x11, 0x08, 0x00, 0x42, 0x03, 0x02}},
    {0xD4, 9, {0x20, 0x54, 0x53, 0x11, 0x00, 0x00, 0x40, 0x03, 0x02}},
    {0xD5, 9, {0x20, 0x54, 0x53, 0x11, 0x08, 0x00, 0x40, 0x03, 0x02}},
    {REGFLAG_END_OF_TABLE, 0x00, {}},
};

static struct LCM_setting_table lcm_initialization_setting_ext[] = {
        {0xFE, 1, {0x00}},
	{0x11,	0,  {}},
	{REGFLAG_DELAY, 120, {}},    //           
	{0x29,	0,  {}},
       {REGFLAG_END_OF_TABLE, 0x00, {}},
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for(i = 0; i < count; i++) {
		unsigned cmd;

		cmd = table[i].cmd;

		switch (cmd) {
		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}
	LCM_PRINT("[LCD] push_table \n");
}
//                                                                            
//                            
//                                                                            
static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy((void*)&lcm_util, (void*)util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS * params)

{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;
	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

   params->physical_width=PHYSICAL_WIDTH;
   params->physical_height=PHYSICAL_HIGHT;

	//                    
	params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = SYNC_EVENT_VDO_MODE;//                    
#endif

	//    
	/*                      */
	params->dsi.LANE_NUM				= LCM_TWO_LANE;
	//                                                                

	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	//                                        
	params->dsi.packet_size = 256;
	//                   
	//                                           
	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active = 4;
    params->dsi.vertical_backporch = 12;
    params->dsi.vertical_frontporch = 50;//    
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active              = 8;
    params->dsi.horizontal_backporch                = 72;
    params->dsi.horizontal_frontporch               = 16;
    params->dsi.horizontal_active_pixel             = FRAME_WIDTH;

	//                     
	//                                                                                         
	//                                                         

	/*                                                             */
	//                                                                      
	//                                       
	//                                                    

	//                                                                      
	//                                  
	//                                      

	//                                                                                                            
	//                                       
	//                     
	//                                 
	//                                                      

    params->dsi.null_packet_en = FALSE;
    params->dsi.mixmode_enable = TRUE;
    params->dsi.pwm_fps = 60;
    params->dsi.mixmode_mipi_clock = 425; //        
    params->dsi.send_frame_enable = TRUE;    

	//                                 
	params->dsi.HS_TRAIL = 14;//  
	params->dsi.HS_ZERO = 6;//  
	params->dsi.HS_PRPR = 8;//  
	params->dsi.LPX = 6;//  
	params->dsi.TA_SACK = 1;
	params->dsi.TA_GET = 30;//   
	params->dsi.TA_SURE = 9;//  
	params->dsi.TA_GO = 24;//   
	params->dsi.CLK_TRAIL = 7;//  
	params->dsi.CLK_ZERO = 28;//   
	params->dsi.LPX_WAIT = 1;
	params->dsi.CONT_DET = 0;
	params->dsi.CLK_HS_PRPR = 8;//  
	//                     
	params->dsi.PLL_CLOCK = 425;

	LCM_PRINT("[LCD] lcm_get_params \n");

}

#if (!defined(BUILD_UBOOT) && !defined(BUILD_LK))  && defined(INIT_CODE_TUNNING_BY_SD_CARD)  /*                                                                      */

//                                                                    
//                                                                                            
#define LCD_INITIAL_TXT "/storage/external_SD/lcd.txt"  //                                   

struct LCM_setting_table lcm_initialization_setting_sd[200]={0};

long Filesize(char *name)
{
	int filesize=0;
	struct kstat stat;

	int error = vfs_stat(name, &stat);

       LCM_PRINT("[LCD] tunning by SD card : file size error %d \n",error);

	if(error != 0) return error;

	return (stat.size);
}

int lcd_init_read_from_sd(void)
{
   struct file *fd;
   //       
   long size;
   char *Buffer_2;
   char *Ori_Buffer;
   //                      
   int packet_type=0;
   int packet_cmd=0;
   int packet_cnt=0;
   int i=0;
   int j=0;
   int ret;

   size= Filesize(LCD_INITIAL_TXT);

   if(size < 0)
   {
	   LCM_PRINT("[LCD] : size error =%ld\n",size);
	   return size;
   }
   else
	   LCM_PRINT("[LCD] : size=%ld\n",size);

   //                                    

#if 1 //                                            
   Buffer_2 = kmalloc((size*sizeof(char)), GFP_KERNEL);
   //                                                    

   if(Buffer_2 == NULL)
   {
	   LCM_PRINT("[seosctest] Buffer null\n");
	   return 0;
   }
   else
	   Ori_Buffer=Buffer_2;
#endif

   if ( 0 < ( fd = filp_open(LCD_INITIAL_TXT, O_RDONLY,0)))
   {
	   fd->f_op->read(fd,Buffer_2, (size *sizeof(char)), &(fd->f_pos));

	   LCM_PRINT("[LCD] : %s\n", Buffer_2);
	   do
	   {
		   LCM_PRINT("[LCD] Line Cnt [%d] \n",i);
		   //                                            
		   sscanf(Buffer_2,"%x %d ",(unsigned int*)&lcm_initialization_setting_sd[i].cmd, (unsigned int*)&lcm_initialization_setting_sd[i].count);
		   LCM_PRINT("[LCD] Packet cmd[%x], Packet Cnt [%d] \n",(unsigned int)lcm_initialization_setting_sd[i].cmd,(unsigned int)lcm_initialization_setting_sd[i].count);

		   while(*(Buffer_2++) != 0x20); //               
		   while(*(Buffer_2++) != 0x20); //                    
		   
		   for( j = 0; j < lcm_initialization_setting_sd[i].count; j++ ){
			   sscanf(Buffer_2,"%x ",(unsigned int*)&lcm_initialization_setting_sd[i].para_list[j]);
			   while(*(Buffer_2++) != 0x20); //               
			   LCM_PRINT("[LCD] para_list[%d] = 0x%x \n",j, (unsigned int)lcm_initialization_setting_sd[i].para_list[j]);
		   }
 
		   while(*(Buffer_2++) != 0x0A); //                   
		   i++;
	   }
	   while(*(Buffer_2) != 0x23); //                        

	   ret = i;
	   LCM_PRINT("[LCD] Escape Loop- Parsing Finish !! \n");

	   kfree(Ori_Buffer);
	   filp_close(fd, NULL);
   }
   else
   {
	   LCM_PRINT( " [LCD] Open Fail.\n");
	   return -1;
   }

   return ret;
}
#endif /*              */

static void init_lcm_registers(void)
{
#if (!defined(BUILD_UBOOT) && !defined(BUILD_LK))  && defined(INIT_CODE_TUNNING_BY_SD_CARD)  /*                                                                      */
	 //          

	int line_cnt = 0;

	LCM_PRINT("[LCD] : [KERNEL] init_lcm_registers	\n ");

	if (Filesize(LCD_INITIAL_TXT) > 0)
	{
		line_cnt= lcd_init_read_from_sd();
		if (line_cnt > 0)
		{
			LCM_PRINT("[LCD] : SD card tunning & initial setting!!! \n ");
			push_table(lcm_initialization_setting_sd, sizeof(lcm_initialization_setting_sd) / sizeof(struct LCM_setting_table), 1);
		}
		else	{
			LCM_PRINT("[LCD] : Default initial setting \n ");
			push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
		}
	}
	else
	{
		LCM_PRINT("[LCD] : Default initial setting \n ");
		push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	}
#else
        push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	LCM_PRINT("[LCD] init_lcm_registers \n");
#endif  /*                                                                      */
}

static void init_lcm_registers_added(void)
{
        push_table(lcm_initialization_setting_ext, sizeof(lcm_initialization_setting_ext) / sizeof(struct LCM_setting_table), 1);

	LCM_PRINT("[LCD] init_lcm_registers_added \n");
}

static void init_lcm_registers_sleep(void)
{
	unsigned int data_array[2];

	data_array[0] = 0x00100500;	//       
	dsi_set_cmdq(data_array, 1, 1);
#if 1  /*                                                     */
        dsi_set_cmdq(data_array, 1, 1);  //                                               
#endif  /*                                                     */

        MDELAY(100);
        data_array[0] = 0x05FE1500;
	dsi_set_cmdq(data_array, 1, 1);
        data_array[0] = 0x00033902;
        data_array[1] = 0x000010D8;
	dsi_set_cmdq(data_array, 2, 1);
        LCM_PRINT("[LCD] init_lcm_registers_sleep \n");
}


/*                 */
static void ldo_1v8io_on(void)
{
#ifdef BUILD_UBOOT
	#error "not implemeted"
#elif defined(BUILD_LK)
	//                  
	upmu_set_rg_vcamd_vosel(3);
	upmu_set_rg_vcamd_en(1);
#else
	hwPowerOn(MT6323_POWER_LDO_VCAMD, VOL_1800, "1V8_LCD_VIO_MTK_S");
#endif
}

/*                       */
static void ldo_1v8io_off(void)
{
#ifdef BUILD_UBOOT
#error "not implemeted"
#elif defined(BUILD_LK)
	upmu_set_rg_vcamd_en(0);
#else
	hwPowerDown(MT6323_POWER_LDO_VCAMD, "1V8_LCD_VIO_MTK_S");
#endif
}

static void ldo_ext_3v0_on(void)
{
#ifdef BUILD_UBOOT
	#error "not implemeted"
#elif defined(BUILD_LK)
        mt_set_gpio_mode(GPIO_LCM_PWR, GPIO_LCM_PWR_M_GPIO);
        mt_set_gpio_pull_enable(GPIO_LCM_PWR, GPIO_PULL_ENABLE);
        mt_set_gpio_dir(GPIO_LCM_PWR, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCM_PWR, GPIO_OUT_ONE);
#else
        mt_set_gpio_mode(GPIO_LCM_PWR, GPIO_LCM_PWR_M_GPIO);
        mt_set_gpio_pull_enable(GPIO_LCM_PWR, GPIO_PULL_ENABLE);
        mt_set_gpio_dir(GPIO_LCM_PWR, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCM_PWR, GPIO_OUT_ONE);
#endif
}

static void ldo_ext_3v0_off(void)
{
#ifdef BUILD_UBOOT
	#error "not implemeted"
#elif defined(BUILD_LK)
	mt_set_gpio_mode(GPIO_LCM_PWR, GPIO_LCM_PWR_M_GPIO);
	mt_set_gpio_pull_enable(GPIO_LCM_PWR, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_LCM_PWR, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_PWR, GPIO_OUT_ZERO);
#else
	mt_set_gpio_mode(GPIO_LCM_PWR, GPIO_LCM_PWR_M_GPIO);
	mt_set_gpio_pull_enable(GPIO_LCM_PWR, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_LCM_PWR, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_PWR, GPIO_OUT_ZERO);
#endif
}

/*
                 
*/
static void ldo_p5m5_dsv_on(void)
{
    #if defined(CONFIG_LEDS_LM3632) || defined(CONFIG_BACKLIGHT_LM3632)
    chargepump_dsv_ctrl(TRUE);
    #else
	mt_set_gpio_mode(GPIO_DSV_AVDD_EN, GPIO_DSV_AVDD_EN_M_GPIO);
	mt_set_gpio_pull_enable(GPIO_DSV_AVDD_EN, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_DSV_AVDD_EN, GPIO_DIR_OUT);
	mt_set_gpio_mode(GPIO_DSV_AVEE_EN, GPIO_DSV_AVEE_EN_M_GPIO);
	mt_set_gpio_pull_enable(GPIO_DSV_AVEE_EN, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_DSV_AVEE_EN, GPIO_DIR_OUT);

	mt_set_gpio_out(GPIO_DSV_AVDD_EN, GPIO_OUT_ONE);
	MDELAY(1);
	mt_set_gpio_out(GPIO_DSV_AVEE_EN, GPIO_OUT_ONE);
    #endif
}

static void ldo_p5m5_dsv_off(void)
{
    #if defined(CONFIG_LEDS_LM3632) || defined(CONFIG_BACKLIGHT_LM3632)
    chargepump_dsv_ctrl(FALSE);
    #else
	mt_set_gpio_mode(GPIO_DSV_AVDD_EN, GPIO_DSV_AVDD_EN_M_GPIO);
	mt_set_gpio_pull_enable(GPIO_DSV_AVDD_EN, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_DSV_AVDD_EN, GPIO_DIR_OUT);
	mt_set_gpio_mode(GPIO_DSV_AVEE_EN, GPIO_DSV_AVEE_EN_M_GPIO);
	mt_set_gpio_pull_enable(GPIO_DSV_AVEE_EN, GPIO_PULL_ENABLE);
	mt_set_gpio_dir(GPIO_DSV_AVEE_EN, GPIO_DIR_OUT);

	mt_set_gpio_out(GPIO_DSV_AVDD_EN, GPIO_OUT_ZERO);
	MDELAY(1);
	mt_set_gpio_out(GPIO_DSV_AVEE_EN, GPIO_OUT_ZERO);
    #endif
}

static void lcm_init(void)
{
#if defined(BUILD_LK)
        SET_RESET_PIN(0);  //          
        ldo_p5m5_dsv_off();
        ldo_1v8io_off();
        ldo_ext_3v0_off();
        MDELAY(2);

        ldo_1v8io_on();  //        
        ldo_ext_3v0_on(); //             
        mt_set_gpio_mode(GPIO_LCD_PMODE_EN, GPIO_LCD_PMODE_EN_M_GPIO);
        mt_set_gpio_dir(GPIO_LCD_PMODE_EN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_LCD_PMODE_EN, GPIO_OUT_ZERO);  //              
        MDELAY(2);        
        mt_set_gpio_out(GPIO_LCD_PMODE_EN, GPIO_OUT_ONE);  //               
        MDELAY(5);
#else
        SET_RESET_PIN(0);
        ldo_1v8io_on();
        ldo_ext_3v0_on();
        MDELAY(10);
#endif
        SET_RESET_PIN(1);
        MDELAY(30);

        init_lcm_registers();
        MDELAY(20);
        ldo_p5m5_dsv_on();
        MDELAY(20);    
        init_lcm_registers_added(); //          

	need_set_lcm_addr = 1;

	LCM_PRINT("[LCD] lcm_init \n");
}

static void lcm_suspend(void)
{
	init_lcm_registers_sleep(); //            
	LCM_PRINT("[LCD] lcm_suspend \n");
}

static void lcm_suspend_power(void)
{
	//            
	ldo_p5m5_dsv_off(); //                   
	//           
	//             
	//                
	//                  
	LCM_PRINT("[LCD] lcm_suspend_power \n");
}

static void lcm_resume(void)
{
	lcm_init();
    need_set_lcm_addr = 1;
	LCM_PRINT("[LCD] lcm_resume \n");
}

static void lcm_esd_recover(void)
{
	lcm_suspend();
	lcm_resume();

	LCM_PRINT("[LCD] lcm_esd_recover \n");
}

static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	//                              
	if(need_set_lcm_addr)
	{
		data_array[0]= 0x00053902;
		data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
		data_array[2]= (x1_LSB);
		dsi_set_cmdq(data_array, 3, 1);

		data_array[0]= 0x00053902;
		data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
		data_array[2]= (y1_LSB);
		dsi_set_cmdq(data_array, 3, 1);
		need_set_lcm_addr = 0;
	}

	data_array[0]= 0x002c3909;
   dsi_set_cmdq(data_array, 1, 0);
	LCM_PRINT("[LCD] lcm_update \n");
}

static unsigned int lcm_compare_id(void)
{
		return 1;
}


#if defined(BUILD_LK) 	

static void lcm_set_pwm_for_mix(int enable)
{
    return;
}


#else
//                                                                      
#define GPIO_INCELL_DISP_TE_PWM         (GPIO90 | 0x80000000) //      

#define GPIO_INCELL_DISP_TE_M_GPIO   GPIO_MODE_00
#define GPIO_INCELL_DISP_TE_M_PWM   GPIO_MODE_02

static struct pwm_spec_config pwm_setting = {
//                          
        .pwm_no = PWM2, //      
		.mode = PWM_MODE_OLD,
		.clk_src = PWM_CLK_OLD_MODE_32K,
		.pmic_pad = false,
		.PWM_MODE_OLD_REGS.IDLE_VALUE = IDLE_FALSE,
		.PWM_MODE_OLD_REGS.GUARD_VALUE = 0, /*                                    */
		.PWM_MODE_OLD_REGS.GDURATION = 0,
		.PWM_MODE_OLD_REGS.WAVE_NUM = 0,                /*                                         */    

  #if defined(TARGET_MT6582_Y50)  /*                                                                                        */
                  //                             
                  .clk_div = CLK_DIV1,            
                  .PWM_MODE_OLD_REGS.DATA_WIDTH = 547,
                  .PWM_MODE_OLD_REGS.THRESH =547/2
  
                  //                            
                 //                                 
                 //                                              
                 //                                          
  #else
                  /*                                                             */
               .clk_div = CLK_DIV4,            
               .PWM_MODE_OLD_REGS.DATA_WIDTH = 135,//                       
               .PWM_MODE_OLD_REGS.THRESH =135/2,   
#endif  /*                                                                                        */               
};


static void set_enable_te_framesync(void)
{
	LCM_PRINT("=============mt_pmic_pwm2_test===============\n");
	mt_set_gpio_mode(GPIO_INCELL_DISP_TE_PWM,GPIO_INCELL_DISP_TE_M_PWM); 

	LCM_PRINT("PWM: clk_div = %x, clk_src = %x, pwm_no = %x\n", pwm_setting.clk_div, pwm_setting.clk_src, pwm_setting.pwm_no);
	pwm_set_spec_config(&pwm_setting);

}

void lcm_set_fps(int fps)
{
    unsigned int width = 32 * 1024 / (4 * fps) - 1;
    
	LCM_PRINT("DSI_set_fps_for_PWM fps (%d), widht (%d)\n", fps, width);
    
    pwm_setting.PWM_MODE_OLD_REGS.DATA_WIDTH = width;
    pwm_setting.PWM_MODE_OLD_REGS.THRESH = width/2;
}

static void lcm_set_pwm_for_mix(int enable)
{
    LCM_PARAMS params;
    
    lcm_get_params(&params);
    if (params.dsi.pwm_fps == 0)
    {
        LCM_PRINT("Please set PWM fps \n");
        return;
    }
    
    if (enable)
    {
#if defined(TARGET_MT6582_Y50)  /*                                                                               */
        //                                
#else
        lcm_set_fps(params.dsi.pwm_fps);
#endif  /*                                                                               */
        set_enable_te_framesync();
    }
    else
    {
    	mt_pwm_disable(pwm_setting.pwm_no, pwm_setting.pmic_pad);
    }
	LCM_PRINT("[LCD] lcm_set_pwm (%d)\n", enable);
    
    return;
}

#endif

//                                                                            
//                      
//                                                                            
LCM_DRIVER db7436_dsi_vdo_fwvga_drv = {
	.name = "db7436_dsi_vdo_fwvga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.suspend_power = lcm_suspend_power,
	.resume = lcm_resume,
	.compare_id = lcm_compare_id,
	.update = lcm_update,
#if (!defined(BUILD_UBOOT) && !defined(BUILD_LK))
	.esd_recover = lcm_esd_recover,
#endif
    .set_pwm_for_mix = lcm_set_pwm_for_mix,
};
