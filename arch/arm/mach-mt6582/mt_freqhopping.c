/*
 * Copyright (C) 2011 MediaTek, Inc.
 *
 * Author: Holmes Chiou <holmes.chiou@mediatek.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/sched_clock.h>
#include <linux/vmalloc.h>
#include <linux/dma-mapping.h>
#include <board-custom.h>
#include <linux/seq_file.h>

#include "mach/mt_freqhopping.h"
#include "mach/mt_fhreg.h"
#include "mach/mt_clkmgr.h"
#include "mach/mt_typedefs.h"
#include "mach/mt_gpio.h"
#include "mach/mt_gpufreq.h"
#include "mach/mt_cpufreq.h"
#include "mach/emi_bwl.h"
#include "mach/sync_write.h"
#include "mach/mt_sleep.h"

#include <mach/mt_freqhopping_drv.h>

//                            
//                                                                                                       
//                             

#define MT_FH_CLK_GEN 		0

#define USER_DEFINE_SETTING_ID 	1

static DEFINE_SPINLOCK(freqhopping_lock);


//                     
static unsigned int 	g_curr_dramc=266; //                                              

//           
static unsigned int 	g_pll_mode;

#define PERCENT_TO_DDSLMT(dDS, pERCENT_M10)  ((dDS * pERCENT_M10 >> 5) / 100)
#if MT_FH_CLK_GEN
static unsigned int 	g_curr_clkgen=MT658X_FH_PLL_TOTAL_NUM+1; //                                   
#endif 

static unsigned char 	g_mempll_fh_table[8];

static unsigned int	g_initialize=0;

static unsigned int	g_clk_en=0;

#ifndef PER_PROJECT_FH_SETTING	

#define LOW_DRAMC_DDS		0x000E55EC
#define LOW_DRAMC_INT		57 //   
#define LOW_DRAMC_FRACTION	5612 //   
#define LOW_DRAMC		200//     
#define LOW_DRAMC_FREQ		200000

//                                                         
static  fh_pll_t g_fh_pll[MT658X_FH_PLL_TOTAL_NUM] = { //                                  
	{FH_FH_DISABLE,        FH_PLL_ENABLE   , 0, 0       , 0},  //                            
#if (MAINPLL_SSC)
	{FH_FH_ENABLE_SSC,     FH_PLL_ENABLE   , 0, 1612000 , 0}, //                            
#else	
	{FH_FH_DISABLE,     FH_PLL_ENABLE   , 0, 1612000 , 0}, //                            
#endif
#if (MEMPLL_SSC)
	{FH_FH_ENABLE_SSC,     FH_PLL_ENABLE   , 0, 266000  , 0},  //                           
#else
	{FH_FH_DISABLE,     FH_PLL_ENABLE   , 0, 266000  , 0},  //                           
#endif
	{FH_FH_ENABLE_SSC,     FH_PLL_ENABLE   , 0, 1599000 , 0},  //                           
	{FH_FH_ENABLE_SSC,     FH_PLL_ENABLE   , 0, 1188000 , 0},  //                          
	{FH_FH_ENABLE_SSC,     FH_PLL_ENABLE  , 0, 1200000 , 0}    //                           
};



//      
static const struct freqhopping_ssc mt_ssc_armpll_setting[MT_SSC_NR_PREDEFINE_SETTING]= {
	{0,0,0,0,0,0},//             
	{0,0xFF,0xFF,0xFF,0xFF,0xFF},//                 
	{0,0,0,0,0,0} //   
};

static const struct freqhopping_ssc mt_ssc_mainpll_setting[MT_SSC_NR_PREDEFINE_SETTING]= {
	{0,0,0,0,0,0},//             
	{0,0xFF,0xFF,0xFF,0xFF,0xFF},//                 
#if 0	
	{1612000 ,2 ,7 ,0, 4, 0xA8000},//      
#else
	{1612000 ,0 ,9 ,0, 8, 0xA8000},//      
#endif	
	{0,0,0,0,0,0} //   
};

static const struct freqhopping_ssc mt_ssc_mempll_setting[MT_SSC_NR_PREDEFINE_SETTING]= {
	{0,0,0,0,0,0},//             
	{0,0xFF,0xFF,0xFF,0xFF,0xFF},//                 
	{266000 ,2 ,7 ,0, 4, 0x0014030c},//                                                                                              
	{200000 ,2 ,7 ,0, 4, 0x000f04b0},//                                       
	{0,0,0,0,0,0} //   
};

static const struct freqhopping_ssc mt_ssc_msdcpll_setting[MT_SSC_NR_PREDEFINE_SETTING]= {
	{0,0,0,0,0,0},//             
	{0,0xFF,0xFF,0xFF,0xFF,0xFF},//                 
	{1599000, 0, 9, 0, 8, 0xF6275}, //                                                      
	{0,0,0,0,0,0} //   

};
static const struct freqhopping_ssc mt_ssc_mmpll_setting[MT_SSC_NR_PREDEFINE_SETTING]= {
	{0,0,0,0,0,0},//             
	{0,0xFF,0xFF,0xFF,0xFF,0xFF},//                 
	{1188000 ,0 ,9 ,0, 8, 0x134000}, //                                                          
	{0,0,0,0,0,0} //   
};
static const struct freqhopping_ssc mt_ssc_vencpll_setting[MT_SSC_NR_PREDEFINE_SETTING]= {
	{0,0,0,0,0,0},//             
	{0,0xFF,0xFF,0xFF,0xFF,0xFF},//                 
	{1200000, 0, 9, 0, 4, 0xB6000}, //                                                      
	{0,0,0,0,0,0} //   
};

static struct freqhopping_ssc mt_ssc_fhpll_userdefined[MT_FHPLL_MAX]= {
	{0,1,1,2,2,0}, //      
	{0,1,1,2,2,0}, //       
	{0,1,1,2,2,0}, //      
	{0,1,1,2,2,0}, //       
	{0,1,1,2,2,0}, //     
	{0,1,1,2,2,0}  //       
};

#else //                      

PER_PROJECT_FH_SETTING

#endif	//                      

unsigned int mt_get_emi_freq(void);

#define PLL_STATUS_ENABLE 1
#define PLL_STATUS_DISABLE 0

static void mt_fh_hal_default_conf(void)
{
	FH_MSG("EN: %s",__func__);
#if (MEMPLL_SSC)
	freqhopping_config(MT658X_FH_MEM_PLL, 266000, true); //                 
#endif
	freqhopping_config(MT658X_FH_MM_PLL, 1188000, true); //               
	freqhopping_config(MT658X_FH_VENC_PLL, 1200000, true); //               
#if (MAINPLL_SSC)	
	freqhopping_config(MT658X_FH_MAIN_PLL, 1612000, true); //               
#endif	
	freqhopping_config(MT658X_FH_MSDC_PLL, 1599000, true); //               
}


static void mt_fh_hal_switch_register_to_FHCTL_control(enum FH_PLL_ID pll_id, int i_control)
{
	if (pll_id==MT658X_FH_ARM_PLL)  //      
		{
			fh_set_field(PLL_HP_CON0, (0x1U<<0), i_control);
		}
	else if (pll_id==MT658X_FH_MAIN_PLL) //      
		{
			fh_set_field(PLL_HP_CON0, (0x1U<<1), i_control);
		}
	else if (pll_id==MT658X_FH_MEM_PLL) //      
		{
			fh_set_field(PLL_HP_CON0, (0x1U<<3), i_control);
		}
	else if (pll_id==MT658X_FH_MSDC_PLL) //      
		{
			fh_set_field(PLL_HP_CON0, (0x1U<<4), i_control);
		}
	else if (pll_id==MT658X_FH_MM_PLL) //      
		{
			fh_set_field(PLL_HP_CON0, (0x1U<<2), i_control);
		}
	else if (pll_id==MT658X_FH_VENC_PLL) //      
		{
			fh_set_field(PLL_HP_CON0, (0x1U<<5), i_control);
		}
}
static int mt_fh_hal_sync_ncpo_to_FHCTL_DDS(enum FH_PLL_ID pll_id)
{
	if (pll_id==MT658X_FH_ARM_PLL)  //      
		{
			fh_write32(REG_FHCTL0_DDS, (fh_read32(ARMPLL_CON1)&0x1FFFFF)|(1U<<31));
		}
	else if (pll_id==MT658X_FH_MAIN_PLL) //      
		{
			fh_write32(REG_FHCTL1_DDS, (fh_read32(MAINPLL_CON1)&0x1FFFFF)|(1U<<31));
		}
	else if (pll_id==MT658X_FH_MEM_PLL) //      
		{
			fh_write32(REG_FHCTL2_DDS, ((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )&0x1FFFFF)|(1U<<31));
		}
	else if (pll_id==MT658X_FH_MSDC_PLL) //      
		{
			fh_write32(REG_FHCTL3_DDS, (fh_read32(MSDCPLL_CON1)&0x1FFFFF)|(1U<<31));
		}
	else if (pll_id==MT658X_FH_MM_PLL) //      
		{
			fh_write32(REG_FHCTL4_DDS, (fh_read32(MMPLL_CON1)&0x1FFFFF)|(1U<<31));
		}
	else if (pll_id==MT658X_FH_VENC_PLL) //      
		{
			fh_write32(REG_FHCTL5_DDS, (fh_read32(VENCPLL_CON1)&0x1FFFFF)|(1U<<31));
		}
	else
		{
			FH_MSG("Incorrect PLL");
			return 0;
		}
	return 1;
}
static void update_fhctl_status(const int pll_id, const int enable)
{
        int i = 0 ;
        int enabled_num = 0 ;
        static unsigned int pll_status[] = {
                PLL_STATUS_DISABLE, //      
                PLL_STATUS_DISABLE, //       
                PLL_STATUS_DISABLE, //      
                PLL_STATUS_DISABLE, //       
                PLL_STATUS_DISABLE, //      
                PLL_STATUS_DISABLE  //       
        } ;

        //                                                                                                   
        //                                                           
        if(pll_status[pll_id] == enable) {
                //                            
                return ;
        }

        pll_status[pll_id] = enable ;

        for(i = MT658X_FH_MINIMUMM_PLL ; i <= MT658X_FH_MAXIMUMM_PLL ; i++) {

                if(pll_status[i] == PLL_STATUS_ENABLE) {
                        //                                
                        enabled_num++ ;
                }
                /*      
                                                         
                 */
        }

        //                                

        if((g_clk_en == 0)&&(enabled_num >= 1)) {
                //                                                                       
	//                                                                             
                g_clk_en = 1;                
        }
        else if((g_clk_en == 1)&&(enabled_num == 0)) {
                //                                                                        
	//                                                                              
                g_clk_en = 0;
        }
}



static int __mt_enable_freqhopping(unsigned int pll_id,const struct freqhopping_ssc* ssc_setting)
{
	//                       
	unsigned long 	flags=0;

	//                    
	FH_MSG("EN: %s:: %x u: %x df: %d dt: %d dds:%x",__func__ ,ssc_setting->lowbnd
			   			      ,ssc_setting->upbnd
			   			      ,ssc_setting->df
			   			      ,ssc_setting->dt
			   			      ,ssc_setting->dds);
	
	update_fhctl_status(pll_id, PLL_STATUS_ENABLE) ;
        mb() ;


	//                                
	//                                            

	g_fh_pll[pll_id].fh_status = FH_FH_ENABLE_SSC;		


	//                                           
	//                                                                 
	//                                         
	//                                                                         
	//                                    
	//           
	//                                                 
	//         
	//       
	
	local_irq_save(flags);	

	//                                                          
	//                 
	fh_set_field(REG_FHCTL0_CFG+(0x14*pll_id),FH_SFSTRX_DYS,ssc_setting->df);
	fh_set_field(REG_FHCTL0_CFG+(0x14*pll_id),FH_SFSTRX_DTS,ssc_setting->dt);

	//                                                                                                     
	//                                                                              

		
	//                                                                      
	//                                                                                          
	
	mt_fh_hal_sync_ncpo_to_FHCTL_DDS(pll_id);
	//                                                                                          
	
	fh_write32(REG_FHCTL0_UPDNLMT+(pll_id*0x14), (PERCENT_TO_DDSLMT((fh_read32(REG_FHCTL0_DDS+(pll_id*0x14))&0x1FFFFF),ssc_setting->lowbnd) << 16));
	//                                                                               
	//                                                                                                         

	mt_fh_hal_switch_register_to_FHCTL_control(pll_id, 1);
	//                                                
	//                                   

	mb();
	
	fh_set_field(REG_FHCTL0_CFG+(0x14*pll_id),FH_FRDDSX_EN,1);
	fh_set_field(REG_FHCTL0_CFG+(0x14*pll_id),FH_FHCTLX_EN,1);
	//                                
	//                                                 

	local_irq_restore(flags);

	//               
	return 0;
}

static int __mt_disable_freqhopping(unsigned int pll_id,const struct freqhopping_ssc* ssc_setting)
{
	unsigned long 	flags=0;
//                        
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;


	//                      
	
	//                                
	//                                            

	local_irq_save(flags);
	//                           
	fh_set_field(REG_FHCTL0_CFG+(0x14*pll_id),FH_FRDDSX_EN,0);
	fh_set_field(REG_FHCTL0_CFG+(0x14*pll_id),FH_FHCTLX_EN,0);
	mb();
	local_irq_restore(flags);


	if(pll_id == MT658X_FH_MEM_PLL){ //          
		unsigned int i=0;
		
		pll_dds =  (DRV_Reg32(DDRPHY_BASE+0x624)) >> 11 ;
		fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
		
		FH_MSG(">p:f< %x:%x",pll_dds,fh_dds);
		
		while((pll_dds != fh_dds) && ( i < 100)){

			if(unlikely(i > 100)){
				FH_BUG_ON(i > 100);
				break;
			}
				
			udelay(10);
			fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
			i++;
		}	
		
		FH_MSG("<p:f:i> %x:%x:%d",pll_dds,fh_dds,i);
	}
	else{
		//                                               
		//                   
	}
	
		
	local_irq_save(flags);
	mt_fh_hal_switch_register_to_FHCTL_control(pll_id, 0);
	//                                                 
	//                                     
	
	g_fh_pll[pll_id].fh_status = FH_FH_DISABLE;
	local_irq_restore(flags);
	
	//                                
	//                                                 

	mb() ;
        update_fhctl_status(pll_id, PLL_STATUS_DISABLE) ;
	

	//               

	return 0;
}


//                                                                       
static noinline int __freq_to_index(enum FH_PLL_ID pll_id,int freq) 
{
	unsigned int retVal = 0;
	unsigned int i=2; //                                                
	
	//                                                             
	//                                          
	
	switch(pll_id) {

		//                                                           
		case MT658X_FH_ARM_PLL:
		while(i < MT_SSC_NR_PREDEFINE_SETTING){
			if(freq == mt_ssc_armpll_setting[i].freq){
				retVal = i;
				break;
			}
			i++;
		}
		break;

		
		case MT658X_FH_MAIN_PLL:
		while(i < MT_SSC_NR_PREDEFINE_SETTING){
			if(freq == mt_ssc_mainpll_setting[i].freq){
				retVal = i;
				break;
			}
			i++;
		}
		break;
		
		case MT658X_FH_MEM_PLL:
		while(i < MT_SSC_NR_PREDEFINE_SETTING){
			if(freq == mt_ssc_mempll_setting[i].freq){
				retVal = i;
				break;
			}
			i++;
		}
		break;

		case MT658X_FH_MSDC_PLL:
		while(i < MT_SSC_NR_PREDEFINE_SETTING){
			if(freq == mt_ssc_msdcpll_setting[i].freq){
				retVal = i;
				break;
			}
			i++;
		}
		break;

		case MT658X_FH_MM_PLL:
		while(i < MT_SSC_NR_PREDEFINE_SETTING){
			if(freq == mt_ssc_mmpll_setting[i].freq){
				retVal = i;
				break;
			}
			i++;
		}
		break;

		case MT658X_FH_VENC_PLL:
		while(i < MT_SSC_NR_PREDEFINE_SETTING){
			if(freq == mt_ssc_vencpll_setting[i].freq){
				retVal = i;
				break;
			}
			i++;
		}
		break;
		
		case MT658X_FH_PLL_TOTAL_NUM:
		FH_MSG("Error MT658X_FH_PLL_TOTAL_NUM!");			
		break;
		

	};

	return retVal;	
}

static int __freqhopping_ctrl(struct freqhopping_ioctl* fh_ctl,bool enable)
{
	const struct freqhopping_ssc* 	pSSC_setting=NULL;
	unsigned int  			ssc_setting_id=0;
	int				retVal=1;
	
	//                                                   
	//                                                                                        

	//                                                  
	FH_BUG_ON(fh_ctl->pll_id>MT658X_FH_MAXIMUMM_PLL);
	FH_BUG_ON(fh_ctl->pll_id<MT658X_FH_MINIMUMM_PLL);	
	
	if (fh_ctl->pll_id==MT658X_FH_MAIN_PLL)
	{
		g_fh_pll[fh_ctl->pll_id].curr_freq = 1612000;
	}
	else if (fh_ctl->pll_id==MT658X_FH_MEM_PLL)
	{
		g_fh_pll[fh_ctl->pll_id].curr_freq = 266000;
	}
	else if (fh_ctl->pll_id==MT658X_FH_MSDC_PLL)
	{
		g_fh_pll[fh_ctl->pll_id].curr_freq = 1599000;
	}
	else if (fh_ctl->pll_id==MT658X_FH_MM_PLL)
	{
		g_fh_pll[fh_ctl->pll_id].curr_freq = 1188000;
	}
	else if (fh_ctl->pll_id==MT658X_FH_VENC_PLL)
	{
		g_fh_pll[fh_ctl->pll_id].curr_freq = 1200000;
	}
	else if (fh_ctl->pll_id==MT658X_FH_ARM_PLL)
	{
		g_fh_pll[fh_ctl->pll_id].curr_freq = 0;
	}
	else
	{
		FH_BUG_ON("Incorrect pll_id.");
	}

	if((enable == true) && (g_fh_pll[fh_ctl->pll_id].fh_status == FH_FH_ENABLE_SSC)  ){ 
		
		//                                 
	
		//                   
		
		//                                      
		__mt_disable_freqhopping(fh_ctl->pll_id,pSSC_setting);
	}
	else if((enable == false) && (g_fh_pll[fh_ctl->pll_id].fh_status == FH_FH_DISABLE)){
		
		//                                                            
		
		//                     
		retVal = 0;
		goto Exit;		
	}
		
	
	//                                                                          
	//                                                                           
	
	if( enable == true) { //                                     

		if(g_fh_pll[fh_ctl->pll_id].pll_status == FH_PLL_DISABLE) {
			//                     
			
			//                                                  
			g_fh_pll[fh_ctl->pll_id].fh_status = FH_FH_ENABLE_SSC;
			retVal = 0;
			goto Exit;
		} 
		else {
			//                    
			if(g_fh_pll[fh_ctl->pll_id].user_defined == true){
				FH_MSG("use u-def");

				pSSC_setting = &mt_ssc_fhpll_userdefined[fh_ctl->pll_id];
				g_fh_pll[fh_ctl->pll_id].setting_id = USER_DEFINE_SETTING_ID; 
			} 
			else {
				//                     
				
				if( g_fh_pll[fh_ctl->pll_id].curr_freq != 0 ){
					ssc_setting_id = g_fh_pll[fh_ctl->pll_id].setting_id = __freq_to_index(fh_ctl->pll_id, g_fh_pll[fh_ctl->pll_id].curr_freq);
				}
				else{
					ssc_setting_id = 0;
				}
					
				
				//                                
				if(ssc_setting_id == 0){
					FH_MSG("!!! No corresponding setting found !!!");
					
					//                      
					__mt_disable_freqhopping(fh_ctl->pll_id,pSSC_setting);
					goto Exit;
				}
				
				switch(fh_ctl->pll_id) {
					case MT658X_FH_MAIN_PLL:
						FH_BUG_ON(ssc_setting_id > (sizeof(mt_ssc_mainpll_setting)/sizeof(struct freqhopping_ssc)) );
						pSSC_setting = &mt_ssc_mainpll_setting[ssc_setting_id];
						break;
					case MT658X_FH_ARM_PLL:
						FH_BUG_ON(ssc_setting_id > (sizeof(mt_ssc_armpll_setting)/sizeof(struct freqhopping_ssc)));
						pSSC_setting = &mt_ssc_armpll_setting[ssc_setting_id];
						break;
					case MT658X_FH_MSDC_PLL:
						FH_BUG_ON(ssc_setting_id > (sizeof(mt_ssc_msdcpll_setting)/sizeof(struct freqhopping_ssc)));
						pSSC_setting = &mt_ssc_msdcpll_setting[ssc_setting_id];
						break;
					case MT658X_FH_MM_PLL:
						FH_BUG_ON(ssc_setting_id > (sizeof(mt_ssc_mmpll_setting)/sizeof(struct freqhopping_ssc)));
						pSSC_setting = &mt_ssc_mmpll_setting[ssc_setting_id];
						break;
					case MT658X_FH_VENC_PLL:
						FH_BUG_ON(ssc_setting_id > (sizeof(mt_ssc_vencpll_setting)/sizeof(struct freqhopping_ssc)));
						pSSC_setting = &mt_ssc_vencpll_setting[ssc_setting_id];
						break;
					case MT658X_FH_MEM_PLL:
						FH_BUG_ON(ssc_setting_id > (sizeof(mt_ssc_mempll_setting)/sizeof(struct freqhopping_ssc)));
						pSSC_setting = &mt_ssc_mempll_setting[ssc_setting_id];
						break;
				}
			}//            

			if(pSSC_setting == NULL){
				FH_MSG("!!! pSSC_setting is NULL !!!");
				//                      
				__mt_disable_freqhopping(fh_ctl->pll_id,pSSC_setting);
				goto Exit;
			}
			
			if( 0 == __mt_enable_freqhopping(fh_ctl->pll_id,pSSC_setting)) {
				retVal = 0;
				FH_MSG("en ok");
			}
			else{
				FH_MSG("__mt_enable_freqhopping fail.");
			}
		}
		
	}
	else{ //                                     
		if( 0 == __mt_disable_freqhopping(fh_ctl->pll_id,pSSC_setting)) {
			retVal = 0;
			FH_MSG("dis ok");
		}
		else{
			FH_MSG("__mt_disable_freqhopping fail.");
		}
	}

Exit:
			
	//               
	return retVal;
}





//                              
static int mt_h2oc_mempll(void)
{
	unsigned long 	flags;
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;
	unsigned int	i=0;

	FH_MSG("EN: %s:%d",__func__,g_curr_dramc);

	/*                                                                     
                                            */

	if( (get_ddr_type() != LPDDR2) && (get_ddr_type() != LPDDR3) ) {
		FH_MSG("Not LPDDR*");
		return -1;
	}

	if(g_curr_dramc != 266){
		FH_MSG("g_curr_dramc != 266)");
		return -1;	
	}

	FH_MSG("dds: %x",(DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ));

	if((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ) != 0x131a2d){
		FH_BUG_ON((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ) != 0x131a2d);
		FH_MSG("DDS != 0x131a2d");
		return -1;
	}
	
	//                                                    
	spin_lock_irqsave(&freqhopping_lock, flags);

	//                   
	__mt_disable_freqhopping(2, NULL);
	
	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC){//                       
		
		//                       
		fh_write32(REG_FHCTL2_CFG, 0x70700000);
	
		//            
		
		pll_dds =  (DRV_Reg32(DDRPHY_BASE+0x624)) >> 11 ;
		fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
		
		FH_MSG(">p:f< %x:%x",pll_dds,fh_dds);
		
		while((pll_dds != fh_dds) && ( i < 100)){
	
			if(unlikely(i > 100)){
				FH_BUG_ON(i > 100);
				break;
			}
				
			udelay(10);
			fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
			i++;
		}	
		
		FH_MSG("<p:f:i> %x:%x:%d",pll_dds,fh_dds,i);
	}

	FH_MSG("=> 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	//                                            

	//                                             
	//                                             
	//                                             
	//                                                             
	fh_write32(REG_FHSRAM_CON, (0x1 << 9 | 0x80) );

	//                                                                             
	//                      
	//                          
	fh_write32(REG_FHSRAM_WR, ((0x54 << 14) | 0x0A8F) );
	
	//                                                 
	//                                                 
	//                                                 
	//                        
	fh_write32(REG_FHCTL_CFG, (1 << 8));

	//                            
	//                                                            
	fh_write32(REG_FHDMA_2G1BASE,  __pa(g_mempll_fh_table));

	//               
	fh_write32(REG_FHCTL2_DDS, (DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )|(1U<<31));

	mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL, 1);
        //                                                                
       
       	mb();

	//                       
	//                              
	//                                   
	//                                                             
	fh_write32(REG_FHCTL2_CFG, ( (0 << 24) | (7 << 28) | 5));
	
	//                                                 
	//                                        
	//                                                 
	fh_write32(REG_FHCTL_2G1_CH, 0);

	g_curr_dramc = 293;

	//                                               

	FH_MSG("-MON- 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	//                                            

	fh_write32( DDRPHY_BASE+0x624, (fh_read32(DDRPHY_BASE+0x624) & 0x7FF) ); //          
	fh_write32( DDRPHY_BASE+0x624, ( fh_read32(DDRPHY_BASE+0x624) |(((0x54 << 14) | 0x0A8F ) << 11 )) ); //        
	
	//               
	fh_write32(REG_FHCTL2_DDS, (DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )|(1U<<31));
	
	//                       
	fh_write32(REG_FHCTL2_CFG, 0x70700000);

	//                                               

	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC){
		FH_MSG("to SSC");
		
	}
	else{
		//                      
		FH_MSG("to APM");
			mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL, 0);
        	//                                                                 
        }

	FH_MSG("+MON+ 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	g_fh_pll[MT658X_FH_MEM_PLL].curr_freq 	= 293000;
	g_fh_pll[MT658X_FH_MEM_PLL].pll_status 	= FH_PLL_ENABLE;

	//                                                     
	spin_unlock_irqrestore(&freqhopping_lock, flags);

	freqhopping_config(MT658X_FH_MEM_PLL, 293000, true); //            

	return 0;
}

//                              
static int mt_oc2h_mempll(void)  
{
	unsigned long 	flags;
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;
	unsigned int	i=0;
	
	FH_MSG("EN: %s:%d",__func__,g_curr_dramc);
	
	/*                                                                     
                                            */

	if( (get_ddr_type() != LPDDR2) && (get_ddr_type() != LPDDR3) ) {
		FH_MSG("Not LPDDR*");
		return -1;
	}
	
	if(g_curr_dramc != 293){
		FH_MSG("g_curr_dramc != 293)");
		return -1;	
	}
	
	FH_MSG("dds: %x",(DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ));
	
	if((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ) != 0x150A8F){
		FH_BUG_ON((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ) != 0x150A8F);
		FH_MSG("DDS != 0x150A8F");
		return 0;			
	}

	//                                                   
	spin_lock_irqsave(&freqhopping_lock, flags);
	
	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC){//                       
		
		//                       
		fh_write32(REG_FHCTL2_CFG, 0x70700000);
	
		//            
		
		pll_dds =  (DRV_Reg32(DDRPHY_BASE+0x624)) >> 11 ;
		fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
		
		FH_MSG(">p:f< %x:%x",pll_dds,fh_dds);
		
		while((pll_dds != fh_dds) && ( i < 100)){
	
			if(unlikely(i > 100)){
				FH_BUG_ON(i > 100);
				break;
			}
				
			udelay(10);
			fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
			i++;
		}	
		
		FH_MSG("<p:f:i> %x:%x:%d",pll_dds,fh_dds,i);
	}


	FH_MSG("=> 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	//                                            
	
	//                                             
	//                                             
	//                                             
	//                                                             
	fh_write32(REG_FHSRAM_CON, (0x1 << 9 | 0x80) );

	//                                                                            
	//                      
	//                          
	fh_write32(REG_FHSRAM_WR, ((0x4C << 14) | 0x1A2D) );
	
	
	//                                                 
	//                                                 
	//                                                 
	//                        
	fh_write32(REG_FHCTL_CFG, (1 << 8));

	//                            
	//                                                            
	fh_write32(REG_FHDMA_2G1BASE,  __pa(g_mempll_fh_table));

	//               
	fh_write32(REG_FHCTL2_DDS, (DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )|(1U<<31));
	
		mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL, 1);
        //                                                                
	
	mb();

	//                       
	//                              
	//                                   
	//                                                             
	fh_write32(REG_FHCTL2_CFG, ( (0 << 24) | (7 << 28) | 5));
	
	//                                                 
	//                                        
	//                                                 
	fh_write32(REG_FHCTL_2G1_CH, 0);

	g_curr_dramc = 266;

	//                                               
	
	FH_MSG("-MON- 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	//                                            
	fh_write32( DDRPHY_BASE+0x624, (fh_read32(DDRPHY_BASE+0x624) & 0x7FF) ); //          
	fh_write32( DDRPHY_BASE+0x624, ( fh_read32(DDRPHY_BASE+0x624) |(((0x4C << 14) | 0x1A2D ) << 11 )) ); //        
	
	//               
	fh_write32(REG_FHCTL2_DDS, (DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )|(1U<<31));

	//                       
	fh_write32(REG_FHCTL2_CFG, 0x70700000);
	//                                               
	
	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC){
		FH_MSG("to SSC");
		
	}
	else{
		//                      
		FH_MSG("to APM");
			mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL, 0);
        	//                                                                 
	}

	FH_MSG("+MON+ 0x%x",DRV_Reg32(REG_FHCTL2_MON));
	g_fh_pll[MT658X_FH_MEM_PLL].curr_freq 	= 266000;
	g_fh_pll[MT658X_FH_MEM_PLL].pll_status 	= FH_PLL_ENABLE;

	//                                                     
	spin_unlock_irqrestore(&freqhopping_lock, flags);

	freqhopping_config(MT658X_FH_MEM_PLL, 266000, true); //            
	
	return 0;
}

//                              
static int mt_fh_hal_l2h_mempll(void)  //                                
{
	unsigned long 	flags;
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;
	unsigned int	i=0;
	
	FH_MSG("EN: %s:%d",__func__,g_curr_dramc);
	
	/*                                                                     
                                            */

	if( (get_ddr_type() != LPDDR2) && (get_ddr_type() != LPDDR3) ) {
		FH_MSG("Not LPDDR*");
		return -1;
	}
	
	if(g_curr_dramc == 266){
		return -1;	
	}
	
	FH_MSG("dds: %x",(DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ));
	
	if((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ) != LOW_DRAMC_DDS){
		FH_BUG_ON((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ) != LOW_DRAMC_DDS);
		FH_MSG("DDS != 0x%X",LOW_DRAMC_DDS);
		return 0;			
	}

	//                                                   
	spin_lock_irqsave(&freqhopping_lock, flags);
	
	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC){//                       
		
		//                       
		fh_write32(REG_FHCTL2_CFG, 0x70700000);
	
		//            
		
		pll_dds =  (DRV_Reg32(DDRPHY_BASE+0x624)) >> 11 ;
		fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
		
		FH_MSG(">p:f< %x:%x",pll_dds,fh_dds);
		
		while((pll_dds != fh_dds) && ( i < 100)){
	
			if(unlikely(i > 100)){
				FH_BUG_ON(i > 100);
				break;
			}
				
			udelay(10);
			fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
			i++;
		}	
		
		FH_MSG("<p:f:i> %x:%x:%d",pll_dds,fh_dds,i);
	}


	FH_MSG("=> 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	//                                            
	
	//                                             
	//                                             
	//                                             
	//                                                             
	fh_write32(REG_FHSRAM_CON, (0x1 << 9 | 0x80) );

	//                                                                            
	//                      
	//                          
	fh_write32(REG_FHSRAM_WR, ((0x4C << 14) | 0x1A2D) );
	
	
	//                                                 
	//                                                 
	//                                                 
	//                        
	fh_write32(REG_FHCTL_CFG, (1 << 8));

	//                            
	//                                                            
	fh_write32(REG_FHDMA_2G1BASE,  __pa(g_mempll_fh_table));

	//               
	fh_write32(REG_FHCTL2_DDS, (DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )|(1U<<31));
		mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL, 1);
        //                                                                
	
	mb();

	//                       
	//                              
	//                                   
	//                                                             
	fh_write32(REG_FHCTL2_CFG, ( (0 << 24) | (7 << 28) | 5));
	
	//                                                 
	//                                        
	//                                                 
	fh_write32(REG_FHCTL_2G1_CH, 0);

	g_curr_dramc = 266;

	//                                               
	
	FH_MSG("-MON- 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	//                                            
	fh_write32( DDRPHY_BASE+0x624, (fh_read32(DDRPHY_BASE+0x624) & 0x7FF) ); //          
	fh_write32( DDRPHY_BASE+0x624, ( fh_read32(DDRPHY_BASE+0x624) |(((0x4C << 14) | 0x1A2D ) << 11 )) ); //        
	
	//               
	fh_write32(REG_FHCTL2_DDS, (DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )|(1U<<31));

	//                       
	fh_write32(REG_FHCTL2_CFG, 0x70700000);
	//                                               
	
	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC){
		FH_MSG("to SSC");
		
	}
	else{
		//                      
		FH_MSG("to APM");
			mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL, 0);
        	//                                                                 
	}

	FH_MSG("+MON+ 0x%x",DRV_Reg32(REG_FHCTL2_MON));
	g_fh_pll[MT658X_FH_MEM_PLL].curr_freq 	= 266000;
	g_fh_pll[MT658X_FH_MEM_PLL].pll_status 	= FH_PLL_ENABLE;

	//                                                     
	spin_unlock_irqrestore(&freqhopping_lock, flags);

	freqhopping_config(MT658X_FH_MEM_PLL, 266000, true); //            
	
	return 0;
}

//                              
static int mt_fh_hal_h2l_mempll(void)  //                                
{
	unsigned long 	flags;
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;
	unsigned int	i=0;
	
	FH_MSG("EN: %s:%d",__func__,g_curr_dramc);
	
	/*                                                                     
                                            */

	if( (get_ddr_type() != LPDDR2) && (get_ddr_type() != LPDDR3) ) {
		FH_MSG("Not LPDDR*");
		return -1;
	}

	if(g_curr_dramc == LOW_DRAMC){
		return -1;	
	}

	FH_MSG("dds: %x",(DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ));

	if((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ) != 0x131a2d){
		FH_BUG_ON((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ) != 0x131a2d);
		FH_MSG("DDS != 0x131a2d");
		return 0;			
	}
	
	//                                                   
	spin_lock_irqsave(&freqhopping_lock, flags);

	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC){//                       
		
		//                       
		fh_write32(REG_FHCTL2_CFG, 0x70700000);
	
		//            
		
		pll_dds =  (DRV_Reg32(DDRPHY_BASE+0x624)) >> 11 ;
		fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
		
		FH_MSG(">p:f< %x:%x",pll_dds,fh_dds);
		
		while((pll_dds != fh_dds) && ( i < 100)){
	
			if(unlikely(i > 100)){
				FH_BUG_ON(i > 100);
				break;
			}
				
			udelay(10);
			fh_dds  =  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
			i++;
		}	
		
		FH_MSG("<p:f:i> %x:%x:%d",pll_dds,fh_dds,i);
	}

	FH_MSG("=> 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	//                                            

	//                                             
	//                                             
	//                                             
	//                                                             
	fh_write32(REG_FHSRAM_CON, (0x1 << 9 | 0x80) );

	//             
	//                                                                             
	//                      
	//                         
	fh_write32(REG_FHSRAM_WR, LOW_DRAMC_DDS);
	
	
	//                                                 
	//                                                 
	//                                                 
	//                        
	fh_write32(REG_FHCTL_CFG, (1 << 8));

	//                            
	//                                                            
	fh_write32(REG_FHDMA_2G1BASE,  __pa(g_mempll_fh_table));

	//               
	fh_write32(REG_FHCTL2_DDS, (DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )|(1U<<31));
		mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL, 1);
        //                                                                
        
       	mb();

	//                       
	//                              
	//                                   
	//                                                             
	fh_write32(REG_FHCTL2_CFG, ( (0 << 24) | (7 << 28) | 5));
	
	//                                                 
	//                                        
	//                                                 
	fh_write32(REG_FHCTL_2G1_CH, 0);

	g_curr_dramc = LOW_DRAMC;

	//                                               

	FH_MSG("-MON- 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	//                                            

	fh_write32( DDRPHY_BASE+0x624, (fh_read32(DDRPHY_BASE+0x624) & 0x7FF) ); //          
	fh_write32( DDRPHY_BASE+0x624, ( fh_read32(DDRPHY_BASE+0x624) |(((LOW_DRAMC_INT << 14) | LOW_DRAMC_FRACTION ) << 11 )) ); //        
	
	//               
	fh_write32(REG_FHCTL2_DDS, (DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )|(1U<<31));
	
	//                       
	fh_write32(REG_FHCTL2_CFG, 0x70700000);

	//                                               

	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC){
		FH_MSG("to SSC");
		
	}
	else{
		//                      
		FH_MSG("to APM");
			mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL, 0);
        	//                                                                 
        }

	FH_MSG("+MON+ 0x%x",DRV_Reg32(REG_FHCTL2_MON));

	g_fh_pll[MT658X_FH_MEM_PLL].curr_freq 	= LOW_DRAMC_FREQ;
	g_fh_pll[MT658X_FH_MEM_PLL].pll_status 	= FH_PLL_ENABLE;

	//                                                     
	spin_unlock_irqrestore(&freqhopping_lock, flags);

	freqhopping_config(MT658X_FH_MEM_PLL, LOW_DRAMC_FREQ, true); //            

	return 0;
}

static int mt_fh_hal_dvfs(enum FH_PLL_ID pll_id, unsigned int dds_value)
{
	unsigned int	fh_dds=0;
	unsigned int	i=0;
	unsigned long 	flags=0;
	unsigned int    ilog =0;
	FH_MSG("EN: %s:",__func__);

	local_irq_save(flags);
	
	//                            
	if (!(mt_fh_hal_sync_ncpo_to_FHCTL_DDS(pll_id)))
		return 0;
	if(ilog)
	{
		FH_MSG("1. sync ncpo to DDS of FHCTL");
		FH_MSG("FHCTL%d_DDS: 0x%08x", pll_id, (fh_read32(REG_FHCTL0_DDS+(pll_id*0x14))&0x1FFFFF));
	}
	
	//                                  
	fh_set_field((REG_FHCTL0_CFG+(pll_id*0x14)), FH_SFSTRX_EN, 1);  //                
    fh_set_field((REG_FHCTL0_CFG+(pll_id*0x14)), FH_FHCTLX_EN, 1);  //                      
    
	if(ilog)
	{
	    FH_MSG("2. enable DVFS and Hopping control");
	}
	//                            
	mt_fh_hal_switch_register_to_FHCTL_control(pll_id,1);
	mb();
	
	if(ilog)
	{
		FH_MSG("3. switch to hopping control");
		FH_MSG("PLL_HP_CON0: 0x%08x",(fh_read32(PLL_HP_CON0)&0x3F));
	}	

	//              
	fh_write32((REG_FHCTL0_DVFS+(pll_id*0x14)), (dds_value)|(1U<<31));  //       
	
	if(ilog)
	{
		FH_MSG("4. set DFS DDS");
		FH_MSG("FHCTL%d_DDS: 0x%08x", pll_id, (fh_read32(REG_FHCTL0_DDS+(pll_id*0x14))&0x1FFFFF));
		FH_MSG("FHCTL%d_DVFS: 0x%08x", pll_id, (fh_read32(REG_FHCTL0_DVFS+(pll_id*0x14))&0x1FFFFF));
	}
	//                             
	fh_dds	=  (DRV_Reg32(REG_FHCTL0_MON+(pll_id*0x14))) & 0x1FFFFF;
	while((dds_value != fh_dds) && ( i < 100)){
		
				if(unlikely(i > 100)){
					FH_BUG_ON(i > 100);
					break;
				}
					
				udelay(10);
				fh_dds	=  (DRV_Reg32(REG_FHCTL0_MON+(pll_id*0x14))) & 0x1FFFFF;
				i++;
			}	
	
	if(ilog)
	{
		FH_MSG("4.1 ensure jump to target DDS");
		FH_MSG("i: %d",i);
	}			

	//                     
	
	if(ilog)
	{
		FH_MSG("5. write back to ncpo");
	}
	if (pll_id==MT658X_FH_ARM_PLL)  //      
		{
			fh_write32(ARMPLL_CON1, (fh_read32(REG_FHCTL0_DVFS)&0x1FFFFF)|(fh_read32(ARMPLL_CON1)&0xFFE00000)|(1U<<31));
			
			if(ilog)
			{
				FH_MSG("ARMPLL_CON1: 0x%08x",(fh_read32(ARMPLL_CON1)&0x1FFFFF));
			}
		}
	else if (pll_id==MT658X_FH_MAIN_PLL) //      
		{
			fh_write32(MAINPLL_CON1, (fh_read32(REG_FHCTL1_DVFS)&0x1FFFFF)|(fh_read32(MAINPLL_CON1)&0xFFE00000)|(1U<<31));
			
			if(ilog)
			{
				FH_MSG("MAINPLL_CON1: 0x%08x",(fh_read32(MAINPLL_CON1)&0x1FFFFF));
			}
		}
	else if (pll_id==MT658X_FH_MEM_PLL) //      
		{
			fh_write32((DDRPHY_BASE+0x624), ((fh_read32(REG_FHCTL2_DVFS)&0x1FFFFF)<<11)|((DRV_Reg32(DDRPHY_BASE+0x624))&0x7FF));
			
			if(ilog)
			{
				FH_MSG("(DDRPHY_BASE+0x624): 0x%08x   >>11: 0x%08x",(fh_read32(DDRPHY_BASE+0x624)),(fh_read32(DDRPHY_BASE+0x624)>>11));
			}
			if ((fh_read32(DDRPHY_BASE+0x624))&0x1)
				fh_write32((DDRPHY_BASE+0x624),((fh_read32(DDRPHY_BASE+0x624))&0xFFFFFFFE));
			else
				fh_write32((DDRPHY_BASE+0x624),((fh_read32(DDRPHY_BASE+0x624))|0x1));
			
			if(ilog)
			{
				FH_MSG("(DDRPHY_BASE+0x624): 0x%08x   >>11: 0x%08x",(fh_read32(DDRPHY_BASE+0x624)),(fh_read32(DDRPHY_BASE+0x624)>>11));	
			}
		}
	else if (pll_id==MT658X_FH_MSDC_PLL) //      
		{
			fh_write32(MSDCPLL_CON1, (fh_read32(REG_FHCTL3_DVFS)&0x1FFFFF)|(fh_read32(MSDCPLL_CON1)&0xFFE00000)|(1U<<31));
			
			if(ilog)
			{
				FH_MSG("MSDCPLL_CON1: 0x%08x",(fh_read32(MSDCPLL_CON1)&0x1FFFFF));
			}
		}
	else if (pll_id==MT658X_FH_MM_PLL) //      
		{
			fh_write32(MMPLL_CON1, (fh_read32(REG_FHCTL4_DVFS)&0x1FFFFF)|(fh_read32(MMPLL_CON1)&0xFFE00000)|(1U<<31));
			
			if(ilog)
			{
				FH_MSG("MMPLL_CON1: 0x%08x",(fh_read32(MMPLL_CON1)&0x1FFFFF));
			}
		}
	else if (pll_id==MT658X_FH_VENC_PLL) //      
		{
			fh_write32(VENCPLL_CON1, (fh_read32(REG_FHCTL5_DVFS)&0x1FFFFF)|(fh_read32(VENCPLL_CON1)&0xFFE00000)|(1U<<31));
			
			if(ilog)
			{
				FH_MSG("VENCPLL_CON1: 0x%08x",(fh_read32(VENCPLL_CON1)&0x1FFFFF));
			}
		}
	

	//                             
	mt_fh_hal_switch_register_to_FHCTL_control(pll_id,0);
	mb();
	
	if(ilog)
	{
		FH_MSG("6. switch to register control");
		FH_MSG("PLL_HP_CON0: 0x%08x",(fh_read32(PLL_HP_CON0)&0x3F));
	}
	
	local_irq_restore(flags);
	return 0;
}

static int mt_fh_hal_dfs_armpll(unsigned int current_freq, unsigned int target_freq)  //               
{
	unsigned long 	flags;
	unsigned int	target_dds=0;
	
	FH_MSG("EN: %s:",__func__);
	FH_MSG("current freq:%d target freq:%d", current_freq, target_freq);
	FH_MSG("current dds(ARMPLL_CON1): 0x%x",(fh_read32(ARMPLL_CON1)&0x1FFFFF));

	//                                                   
	spin_lock_irqsave(&freqhopping_lock, flags);

	fh_set_field(REG_FHCTL0_CFG, FH_FRDDSX_EN, 0);  //                
	fh_set_field(REG_FHCTL0_CFG, FH_SFSTRX_EN, 0);  //                 
    fh_set_field(REG_FHCTL0_CFG, FH_FHCTLX_EN, 0);  //                          
	target_dds = (((target_freq/100) * (((fh_read32(ARMPLL_CON1)&0x1FFFFF)*1000)/(current_freq/100)))/1000);
	FH_MSG("target dds: 0x%x",target_dds);
	
	mt_fh_hal_dvfs(MT658X_FH_ARM_PLL, target_dds); 

	fh_set_field(REG_FHCTL0_CFG, FH_FRDDSX_EN, 0);  //                
	fh_set_field(REG_FHCTL0_CFG, FH_SFSTRX_EN, 0);  //                 
    fh_set_field(REG_FHCTL0_CFG, FH_FHCTLX_EN, 0);  //                       
	spin_unlock_irqrestore(&freqhopping_lock, flags);
	
	return 0;
}
static int mt_fh_hal_l2h_dvfs_mempll(void)  //                                
{
	unsigned long 	flags;
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;
	unsigned int	i=0;

	FH_MSG("EN: %s:",__func__);
	/*                                                                     
                                            */

	if( (get_ddr_type() != LPDDR2) && (get_ddr_type() != LPDDR3) ) {
		FH_MSG("Not LPDDR*");
		return -1;
	}

	FH_MSG("dds: 0x%x",(DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ));

	g_pll_mode = (DRV_Reg32(DDRPHY_BASE+0x60C) & 0x200);
	FH_MSG("g_pll_mode(0:1pll, 1:3pll): %x",g_pll_mode);

	if (g_pll_mode==0) //          
		{
		if ((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )==0x14030C)
			{
				FH_MSG("Already @533MHz");
				return 0;
			}
		else
			FH_MSG("Jump to 533MHz");		
		}
	else if (g_pll_mode==0x200) //          
		{
		if ((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )==0x131A2E)
			{
				FH_MSG("Already @533MHz");
				return 0;
			}
		else
			FH_MSG("Jump to 533MHz");
		}	

	//                                                   
	spin_lock_irqsave(&freqhopping_lock, flags);

	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC)
		{//                        
			//                       
			fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 0);  //                
			fh_set_field(REG_FHCTL2_CFG, FH_SFSTRX_EN, 0);  //                 
    		fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 0);  //                       
		
			//            
			
			pll_dds =  (DRV_Reg32(REG_FHCTL2_DDS)) & 0x1FFFFF;
			fh_dds	=  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
			
			FH_MSG(">p:f< %x:%x",pll_dds,fh_dds);
			
			while((pll_dds != fh_dds) && ( i < 100)){
		
				if(unlikely(i > 100)){
					FH_BUG_ON(i > 100);
					break;
				}
					
				udelay(10);
				fh_dds	=  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
				i++;
			}	
						
			FH_MSG("<p:f:i> %x:%x:%d",pll_dds,fh_dds,i);

		}

	if (g_pll_mode==0) //          
		mt_fh_hal_dvfs(MT658X_FH_MEM_PLL, 0x14030C); //            
	else if (g_pll_mode==0x200) //          
		mt_fh_hal_dvfs(MT658X_FH_MEM_PLL, 0x131A2E); //            

	g_curr_dramc = 266;
		
	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC)
	{
		fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 0);  //                
		fh_set_field(REG_FHCTL2_CFG, FH_SFSTRX_EN, 0);  //                 
    	fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 0);  //                       

		if (!(mt_fh_hal_sync_ncpo_to_FHCTL_DDS(MT658X_FH_MEM_PLL)))
			return 0;
		FH_MSG("1. sync ncpo to DDS of FHCTL");
		FH_MSG("FHCTL2_DDS: 0x%08x", (fh_read32(REG_FHCTL2_DDS)&0x1FFFFF));

		fh_set_field(REG_FHCTL2_CFG,FH_SFSTRX_DYS,mt_ssc_mempll_setting[2].df);
		fh_set_field(REG_FHCTL2_CFG,FH_SFSTRX_DTS,mt_ssc_mempll_setting[2].dt);

		fh_write32(REG_FHCTL2_UPDNLMT, (PERCENT_TO_DDSLMT((fh_read32(REG_FHCTL2_DDS)&0x1FFFFF),mt_ssc_mempll_setting[2].lowbnd) << 16));
		FH_MSG("REG_FHCTL2_UPDNLMT: 0x%08x", fh_read32(REG_FHCTL2_UPDNLMT));
		
		mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL,1);

		fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 1);  //               
    	fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 1);  //                      
    	FH_MSG("REG_FHCTL2_CFG: 0x%08x", fh_read32(REG_FHCTL2_CFG));
		
	}
	//                                                     
	spin_unlock_irqrestore(&freqhopping_lock, flags);
	
	return 0;
}

static int mt_fh_hal_h2l_dvfs_mempll(void)  //                               
{
	unsigned long 	flags;
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;
	unsigned int	i=0;

	FH_MSG("EN: %s:",__func__);
	/*                                                                     
                                            */

	if( (get_ddr_type() != LPDDR2) && (get_ddr_type() != LPDDR3) ) {
		FH_MSG("Not LPDDR*");
		return -1;
	}

	FH_MSG("dds: 0x%x",(DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ));

	g_pll_mode = (DRV_Reg32(DDRPHY_BASE+0x60C) & 0x200);
	FH_MSG("g_pll_mode(0:1pll, 1:3pll): %x",g_pll_mode);

	if (g_pll_mode==0) //          
		{
		if ((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )==0x0F04B0)
			{
				FH_MSG("Already @400MHz");
				return 0;
			}
		else
			FH_MSG("Jump to 400MHz");
		}
	else if (g_pll_mode==0x200) //          
		{
		if ((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )==0x0E55EE)
			{
				FH_MSG("Already @400MHz");
				return 0;
			}
		else
			FH_MSG("Jump to 400MHz");
		}
	
	//                                                   
	spin_lock_irqsave(&freqhopping_lock, flags);

	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC)
		{//                        
			//                       
			fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 0);  //                
			fh_set_field(REG_FHCTL2_CFG, FH_SFSTRX_EN, 0);  //                 
    		fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 0);  //                       
		
			//            
			
			pll_dds =  (DRV_Reg32(REG_FHCTL2_DDS)) & 0x1FFFFF;
			fh_dds	=  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
			
			FH_MSG(">p:f< %x:%x",pll_dds,fh_dds);
			
			while((pll_dds != fh_dds) && ( i < 100)){
		
				if(unlikely(i > 100)){
					FH_BUG_ON(i > 100);
					break;
				}
					
				udelay(10);
				fh_dds	=  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
				i++;
			}	
						
			FH_MSG("<p:f:i> %x:%x:%d",pll_dds,fh_dds,i);
		}

	if (g_pll_mode==0) //          
		mt_fh_hal_dvfs(MT658X_FH_MEM_PLL, 0x0F04B0); //          
	else if (g_pll_mode==0x200) //          
		mt_fh_hal_dvfs(MT658X_FH_MEM_PLL, 0x0E55EE); //          

	g_curr_dramc = 200;

	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC)
	{
		fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 0);  //                
		fh_set_field(REG_FHCTL2_CFG, FH_SFSTRX_EN, 0);  //                 
    	fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 0);  //                       

		if (!(mt_fh_hal_sync_ncpo_to_FHCTL_DDS(MT658X_FH_MEM_PLL)))
			return 0;
		FH_MSG("Enable mempll SSC mode");
		FH_MSG("1. sync ncpo to DDS of FHCTL");
		FH_MSG("FHCTL2_DDS: 0x%08x", (fh_read32(REG_FHCTL2_DDS)&0x1FFFFF));

		fh_set_field(REG_FHCTL2_CFG,FH_SFSTRX_DYS,mt_ssc_mempll_setting[3].df);
		fh_set_field(REG_FHCTL2_CFG,FH_SFSTRX_DTS,mt_ssc_mempll_setting[3].dt);

		fh_write32(REG_FHCTL2_UPDNLMT, (PERCENT_TO_DDSLMT((fh_read32(REG_FHCTL2_DDS)&0x1FFFFF),mt_ssc_mempll_setting[3].lowbnd) << 16));
		FH_MSG("REG_FHCTL2_UPDNLMT: 0x%08x", fh_read32(REG_FHCTL2_UPDNLMT));
		
		mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL,1);

		fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 1);  //               
    	fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 1);  //                      

		FH_MSG("REG_FHCTL2_CFG: 0x%08x", fh_read32(REG_FHCTL2_CFG));
		
	}
	//                                                     
	spin_unlock_irqrestore(&freqhopping_lock, flags);
	
	return 0;
}

static int mt_h2oc_dfs_mempll(void)
{
	unsigned long 	flags;
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;
	unsigned int	i=0;

	FH_MSG("EN: %s:",__func__);
	/*                                                                     
                                            */

	if( (get_ddr_type() != LPDDR2) && (get_ddr_type() != LPDDR3) ) {
		FH_MSG("Not LPDDR*");
		return -1;
	}

	FH_MSG("dds: 0x%x",(DRV_Reg32(DDRPHY_BASE+0x624) >> 11 ));

	g_pll_mode = (DRV_Reg32(DDRPHY_BASE+0x60C) & 0x200);
	FH_MSG("g_pll_mode(0:1pll, 1:3pll): %x",g_pll_mode);

	if (g_pll_mode==0) //          
		{
		if ((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )==0x160077)
			{
				FH_MSG("Already @1172MHz");
				return 0;
			}
		else
			FH_MSG("Jump to 1172MHz");		
		}
	else if (g_pll_mode==0x200) //          
		{
		if ((DRV_Reg32(DDRPHY_BASE+0x624) >> 11 )==0x150071)
			{
				FH_MSG("Already @1172MHz");
				return 0;
			}
		else
			FH_MSG("Jump to 1172MHz");
		}	

	//                                                   
	spin_lock_irqsave(&freqhopping_lock, flags);

	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC)
		{//                        
			//                       
			fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 0);  //                
			fh_set_field(REG_FHCTL2_CFG, FH_SFSTRX_EN, 0);  //                 
    		fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 0);  //                       
		
			//            
			
			pll_dds =  (DRV_Reg32(REG_FHCTL2_DDS)) & 0x1FFFFF;
			fh_dds	=  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
			
			FH_MSG(">p:f< %x:%x",pll_dds,fh_dds);
			
			while((pll_dds != fh_dds) && ( i < 100)){
		
				if(unlikely(i > 100)){
					FH_BUG_ON(i > 100);
					break;
				}
					
				udelay(10);
				fh_dds	=  (DRV_Reg32(REG_FHCTL2_MON)) & 0x1FFFFF;
				i++;
			}	
						
			FH_MSG("<p:f:i> %x:%x:%d",pll_dds,fh_dds,i);

		}

	if (g_pll_mode==0) //          
		mt_fh_hal_dvfs(MT658X_FH_MEM_PLL, 0x160077); //         
	else if (g_pll_mode==0x200) //          
		mt_fh_hal_dvfs(MT658X_FH_MEM_PLL, 0x150071); //          

	if(g_fh_pll[MT658X_FH_MEM_PLL].fh_status == FH_FH_ENABLE_SSC)
	{
		fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 0);  //                
		fh_set_field(REG_FHCTL2_CFG, FH_SFSTRX_EN, 0);  //                 
    	fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 0);  //                       

		if (!(mt_fh_hal_sync_ncpo_to_FHCTL_DDS(MT658X_FH_MEM_PLL)))
			return 0;
		FH_MSG("1. sync ncpo to DDS of FHCTL");
		FH_MSG("FHCTL2_DDS: 0x%08x", (fh_read32(REG_FHCTL2_DDS)&0x1FFFFF));

		fh_set_field(REG_FHCTL2_CFG,FH_SFSTRX_DYS,mt_ssc_mempll_setting[2].df);
		fh_set_field(REG_FHCTL2_CFG,FH_SFSTRX_DTS,mt_ssc_mempll_setting[2].dt);

		fh_write32(REG_FHCTL2_UPDNLMT, (PERCENT_TO_DDSLMT((fh_read32(REG_FHCTL2_DDS)&0x1FFFFF),mt_ssc_mempll_setting[2].lowbnd) << 16));
		FH_MSG("REG_FHCTL2_UPDNLMT: 0x%08x", fh_read32(REG_FHCTL2_UPDNLMT));
		
		mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MEM_PLL,1);

		fh_set_field(REG_FHCTL2_CFG, FH_FRDDSX_EN, 1);  //               
    	fh_set_field(REG_FHCTL2_CFG, FH_FHCTLX_EN, 1);  //                      
    	FH_MSG("REG_FHCTL2_CFG: 0x%08x", fh_read32(REG_FHCTL2_CFG));
		
	}
	//                                                     
	spin_unlock_irqrestore(&freqhopping_lock, flags);
	
	return 0;
}

static int mt_fh_hal_dram_overclock(int clk)
{
	FH_MSG("EN: %s clk:%d",__func__,clk);
if (1)//        
{
	if( clk == 200)
	{
		g_curr_dramc = 200;
		return(mt_fh_hal_h2l_dvfs_mempll());
	}

	if( clk == 266)
	{
		g_curr_dramc = 266;
		return(mt_fh_hal_l2h_dvfs_mempll());	
	}
	
	if( clk == 293)
	{
		g_curr_dramc = 293;
		return(mt_h2oc_dfs_mempll());
	}
}
else	
{
	if( clk == LOW_DRAMC){ //                     
		if( g_curr_dramc != 266 ){ //                 
			FH_BUG_ON(1);
			return -1;
		}
		else{ //                            
			return(mt_fh_hal_h2l_mempll());			
		}
	}
	
	if( clk == 293){ //                   
		if( g_curr_dramc != 266 ){ //               
			FH_BUG_ON(1);
			return -1;
		}
		else{ //                          
			return(mt_h2oc_mempll());			
		}
	}
	
	if( clk == 266){ //                     
		if( g_curr_dramc == 266 ){ //                    
			FH_BUG_ON(1);
			return -1;
		}
		else if( g_curr_dramc == LOW_DRAMC ){ //          
			return(mt_fh_hal_l2h_mempll());			
		}
		else if( g_curr_dramc == 293 ){ //          
			return(mt_oc2h_mempll());			
		}
	}
}	
	FH_BUG_ON(1);
	return(-1);
}

static int mt_fh_hal_get_dramc(void)
{
	return(g_curr_dramc);
}

static void mt_fh_hal_popod_save(void)
{
	unsigned int	fh_dds=0;
	unsigned int	pll_dds=0;
	unsigned int	i=0;
	
	FH_MSG("EN: %s",__func__);
	
	//                       
	if(g_fh_pll[MT658X_FH_MAIN_PLL].fh_status == FH_FH_ENABLE_SSC)//                       
		{	
			//                        
			fh_set_field(REG_FHCTL1_CFG, FH_FRDDSX_EN, 0);  //                
			fh_set_field(REG_FHCTL1_CFG, FH_SFSTRX_EN, 0);  //                 
    		fh_set_field(REG_FHCTL1_CFG, FH_FHCTLX_EN, 0);  //                       
		
			//            
			
			pll_dds =  (DRV_Reg32(REG_FHCTL1_DDS)) & 0x1FFFFF;
			fh_dds	=  (DRV_Reg32(REG_FHCTL1_MON)) & 0x1FFFFF;
			
			FH_MSG("Org pll_dds:%x fh_dds:%x",pll_dds,fh_dds);
			
			while((pll_dds != fh_dds) && ( i < 100))
			{
		
				if(unlikely(i > 100)){
					FH_BUG_ON(i > 100);
					break;
				}
					
				udelay(10);
				fh_dds	=  (DRV_Reg32(REG_FHCTL1_MON)) & 0x1FFFFF;
				i++;
			}	
						
			FH_MSG("pll_dds:%x  fh_dds:%x   i:%d",pll_dds,fh_dds,i);

			//                   
			fh_write32(MAINPLL_CON1, (fh_read32(REG_FHCTL1_DDS)&0x1FFFFF)|(fh_read32(MAINPLL_CON1)&0xFFE00000)|(1U<<31));
			FH_MSG("MAINPLL_CON1: 0x%08x",(fh_read32(MAINPLL_CON1)&0x1FFFFF));

			//                           
			mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MAIN_PLL,0);
			mb();
			FH_MSG("switch to register control PLL_HP_CON0: 0x%08x",(fh_read32(PLL_HP_CON0)&0x3F));
			
		}
	
}

static void mt_fh_hal_popod_restore(void)
{
	
	FH_MSG("EN: %s",__func__);

	//                      
	if(g_fh_pll[MT658X_FH_MAIN_PLL].fh_status == FH_FH_ENABLE_SSC)
	{
		fh_set_field(REG_FHCTL1_CFG, FH_FRDDSX_EN, 0);  //                
		fh_set_field(REG_FHCTL1_CFG, FH_SFSTRX_EN, 0);  //                 
    	fh_set_field(REG_FHCTL1_CFG, FH_FHCTLX_EN, 0);  //                       

		if (!(mt_fh_hal_sync_ncpo_to_FHCTL_DDS(MT658X_FH_MAIN_PLL)))
			return;
		FH_MSG("Enable mainpll SSC mode");
		FH_MSG("sync ncpo to DDS of FHCTL");
		FH_MSG("FHCTL1_DDS: 0x%08x", (fh_read32(REG_FHCTL1_DDS)&0x1FFFFF));

		fh_set_field(REG_FHCTL1_CFG,FH_SFSTRX_DYS,mt_ssc_mainpll_setting[2].df);
		fh_set_field(REG_FHCTL1_CFG,FH_SFSTRX_DTS,mt_ssc_mainpll_setting[2].dt);

		fh_write32(REG_FHCTL1_UPDNLMT, (PERCENT_TO_DDSLMT((fh_read32(REG_FHCTL1_DDS)&0x1FFFFF),mt_ssc_mainpll_setting[2].lowbnd) << 16));
		FH_MSG("REG_FHCTL1_UPDNLMT: 0x%08x", fh_read32(REG_FHCTL1_UPDNLMT));
		
		mt_fh_hal_switch_register_to_FHCTL_control(MT658X_FH_MAIN_PLL,1);

		fh_set_field(REG_FHCTL1_CFG, FH_FRDDSX_EN, 1);  //               
    	fh_set_field(REG_FHCTL1_CFG, FH_FHCTLX_EN, 1);  //                      

		FH_MSG("REG_FHCTL1_CFG: 0x%08x", fh_read32(REG_FHCTL1_CFG));
		
	}

}

//                                                                                                            
static int freqhopping_dramc_proc_read(struct seq_file* m, void* v)
{
	//               
	//            

	FH_MSG("EN: %s",__func__);
	
	seq_printf(m, "DRAMC: %dMHz\r\n",g_curr_dramc);
	seq_printf(m, "mt_get_emi_freq(): %dHz\r\n",mt_get_emi_freq());	
	seq_printf(m, "get_ddr_type(): %d\r\n",get_ddr_type());	
	seq_printf(m, "rank: 0x%x\r\n",(DRV_Reg32(EMI_CONA) & 0x20000));	

#if 0
	*start = page + off;

	len = p - page;

	if (len > off)
		len -= off;
	else
		len = 0;

	return len < count ? len : count;
#endif 
  return 0;		
}


static int freqhopping_dramc_proc_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int len = 0, freq = 0;
	char dramc[32];

	FH_MSG("EN: proc");

	len = (count < (sizeof(dramc) - 1)) ? count : (sizeof(dramc) - 1);

	if (copy_from_user(dramc, buffer, len))
	{
		FH_MSG("copy_from_user fail!");
		return 1;
	}
	
	dramc[len] = '\0';
   
	if (sscanf(dramc, "%d", &freq) == 1)
	{
		if( (freq == 266) || (freq == 200)){
			FH_MSG("dramc:%d ", freq);
			(freq==266) ? mt_fh_hal_l2h_mempll() : mt_fh_hal_h2l_mempll();
		}
		else if(freq == 293){
			mt_fh_hal_dram_overclock(293);
		}
		else{
			FH_MSG("must be 200/266/293!");
		}

#if 0 
		if(freq == 266){
			FH_MSG("==> %d",mt_fh_hal_dram_overclock(266));
		}
		else if(freq == 293){
			FH_MSG("==> %d",mt_fh_hal_dram_overclock(293));
		}
		else if(freq == LOW_DRAMC){
			FH_MSG("==> %d",mt_fh_hal_dram_overclock(208));
		}
#endif

		return count;
	}
	else
	{
		FH_MSG("  bad argument!!");
	}

	return -EINVAL;
}

//                                                                                                           
static int freqhopping_dvfs_proc_read(struct seq_file* m, void* v)
{
	//               
	//            
	int 	i=0;

	FH_MSG("EN: %s",__func__);

	seq_printf(m, "DVFS:\r\n");
	seq_printf(m, "CFG: 0x3 is SSC mode;  0x5 is DVFS mode \r\n");
	for(i=0;i<MT_FHPLL_MAX;i++) {		
		seq_printf(m, "FHCTL%d:   CFG:0x%08x    DVFS:0x%08x\r\n",i, DRV_Reg32(REG_FHCTL0_CFG+(i*0x14)), DRV_Reg32(REG_FHCTL0_DVFS+(i*0x14)));
	}

#if 0
	*start = page + off;

	len = p - page;

	if (len > off)
		len -= off;
	else
		len = 0;

	return len < count ? len : count;
#endif
  return 0;  		
}


static int freqhopping_dvfs_proc_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int 		ret;
	char 		kbuf[256];
	unsigned long 	len = 0;
	unsigned int	p1,p2,p3,p4,p5;
//                                   
	
	p1=0;
	p2=0;
	p3=0;
	p4=0;
	p5=0;

	FH_MSG("EN: %s",__func__);
	
	len = min(count, (unsigned long)(sizeof(kbuf)-1));

	if (count == 0)return -1;
	if(count > 255)count = 255;

	ret = copy_from_user(kbuf, buffer, count);
	if (ret < 0)return -1;
	
	kbuf[count] = '\0';

	sscanf(kbuf, "%d %d %d %d %d", &p1, &p2, &p3, &p4, &p5);

	FH_MSG("EN: p1=%d p2=%d p3=%d", p1, p2, p3);

	if (p1==MT658X_FH_MEM_PLL)
		{
			if (p2==533)
				mt_fh_hal_l2h_dvfs_mempll();
			else if (p2==400)
				mt_fh_hal_h2l_dvfs_mempll();
			else if (p2==586)
				mt_h2oc_dfs_mempll();
			else
				FH_MSG("not define %d freq @mempll",p2);
		}
	else if (p1==MT658X_FH_ARM_PLL)
		{
			mt_fh_hal_dfs_armpll(p2, p3);
		}
	else if (p1==4370)
		{
			FH_MSG("EN: pllid=%d dt=%d df=%d lowbnd=%d", p2, p3, p4, p5);
			fh_set_field(REG_FHCTL0_CFG+(p2*0x14), FH_FRDDSX_EN, 0);  //                
			fh_set_field(REG_FHCTL0_CFG+(p2*0x14), FH_SFSTRX_EN, 0);  //                 
	    	fh_set_field(REG_FHCTL0_CFG+(p2*0x14), FH_FHCTLX_EN, 0);  //                       

			if (!(mt_fh_hal_sync_ncpo_to_FHCTL_DDS(p2)))
				return 0;
			FH_MSG("Enable FHCTL%d SSC mode",p2);
			FH_MSG("1. sync ncpo to DDS of FHCTL");
			FH_MSG("FHCTL%d_DDS: 0x%08x",p2 ,(fh_read32(REG_FHCTL0_DDS+(p2*0x14))&0x1FFFFF));

			fh_set_field(REG_FHCTL0_CFG+(p2*0x14),FH_SFSTRX_DYS,p4);
			fh_set_field(REG_FHCTL0_CFG+(p2*0x14),FH_SFSTRX_DTS,p3);

			fh_write32(REG_FHCTL0_UPDNLMT+(p2*0x14), (PERCENT_TO_DDSLMT((fh_read32(REG_FHCTL0_DDS+(p2*0x14))&0x1FFFFF),p5) << 16));
			FH_MSG("REG_FHCTL%d_UPDNLMT: 0x%08x",p2 , fh_read32(REG_FHCTL0_UPDNLMT+(p2*0x14)));
			
			mt_fh_hal_switch_register_to_FHCTL_control(p2,1);

			fh_set_field(REG_FHCTL0_CFG+(p2*0x14), FH_FRDDSX_EN, 1);  //               
	    	fh_set_field(REG_FHCTL0_CFG+(p2*0x14), FH_FHCTLX_EN, 1);  //                      

			FH_MSG("REG_FHCTL%d_CFG: 0x%08x",p2 , fh_read32(REG_FHCTL0_CFG+(p2*0x14)));
		}
	else if (p1==2222)
		{
			if (p2==0) //       
				mt_fh_hal_popod_save();
			else if (p2==1) //      
				mt_fh_hal_popod_restore();
		}
	else
		mt_fh_hal_dvfs(p1, p2); 

	return count;
}



//                                                                                                               
static int freqhopping_dumpregs_proc_read(struct seq_file* m, void* v)
{
	//                
	//             
	int 	i=0;

	FH_MSG("EN: %s",__func__);

	seq_printf(m, "FHDMA_CFG:\r\n");

	seq_printf(m, "REG_FHDMA_CFG: 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
		DRV_Reg32(REG_FHDMA_CFG),
		DRV_Reg32(REG_FHDMA_2G1BASE),
		DRV_Reg32(REG_FHDMA_2G2BASE),
		DRV_Reg32(REG_FHDMA_INTMDBASE));

	seq_printf(m, "REG_FHDMA_EXTMDBASE: 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
		DRV_Reg32(REG_FHDMA_EXTMDBASE),
		DRV_Reg32(REG_FHDMA_BTBASE),
		DRV_Reg32(REG_FHDMA_WFBASE),
		DRV_Reg32(REG_FHDMA_FMBASE));

	seq_printf(m, "REG_FHSRAM_CON: 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
		DRV_Reg32(REG_FHSRAM_CON),
		DRV_Reg32(REG_FHSRAM_WR),
		DRV_Reg32(REG_FHSRAM_RD),
		DRV_Reg32(REG_FHCTL_CFG),
		DRV_Reg32(REG_FHCTL_CON));

	seq_printf(m, "REG_FHCTL_2G1_CH: 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
		DRV_Reg32(REG_FHCTL_2G1_CH),
		DRV_Reg32(REG_FHCTL_2G2_CH),
		DRV_Reg32(REG_FHCTL_INTMD_CH),
		DRV_Reg32(REG_FHCTL_EXTMD_CH));

	seq_printf(m, "REG_FHCTL_BT_CH: 0x%08x 0x%08x 0x%08x \r\n\r\n",
		DRV_Reg32(REG_FHCTL_BT_CH),
		DRV_Reg32(REG_FHCTL_WF_CH),
		DRV_Reg32(REG_FHCTL_FM_CH));


	for(i=0;i<MT_FHPLL_MAX;i++) {
		
		seq_printf(m, "FHCTL%d_CFG:\r\n",i);
		seq_printf(m, "0x%08x 0x%08x 0x%08x 0x%08x\r\n",
			DRV_Reg32(REG_FHCTL0_CFG+(i*0x14)),
			DRV_Reg32(REG_FHCTL0_UPDNLMT+(i*0x14)),
			DRV_Reg32(REG_FHCTL0_DDS+(i*0x14)),
			DRV_Reg32(REG_FHCTL0_MON+(i*0x14)));
	}

	
	seq_printf(m, "\r\nPLL_HP_CON0:\r\n0x%08x\r\n",
		DRV_Reg32(PLL_HP_CON0));
		
	seq_printf(m, "\r\nPLL_CON0 :\r\nARM:0x%08x MAIN:0x%08x MSDC:0x%08x MM:0x%08x VENC:0x%08x\r\n",
			DRV_Reg32(ARMPLL_CON0),	
			DRV_Reg32(MAINPLL_CON0),	
			DRV_Reg32(MSDCPLL_CON0),	
			DRV_Reg32(MMPLL_CON0),
			DRV_Reg32(VENCPLL_CON0));

	seq_printf(m, "\r\nPLL_CON1 :\r\nARM:0x%08x MAIN:0x%08x MSDC:0x%08x MM:0x%08x VENC:0x%08x\r\n",
			DRV_Reg32(ARMPLL_CON1),	
			DRV_Reg32(MAINPLL_CON1),	
			DRV_Reg32(MSDCPLL_CON1),	
			DRV_Reg32(MMPLL_CON1),
			DRV_Reg32(VENCPLL_CON1));
		

	seq_printf(m, "\r\nMEMPLL :\r\nMEMPLL9: 0x%08x MEMPLL10: 0x%08x MEMPLL11: 0x%08x MEMPLL12: 0x%08x\r\n",
			DRV_Reg32(DDRPHY_BASE+0x624),
			DRV_Reg32(DDRPHY_BASE+0x628),
			DRV_Reg32(DDRPHY_BASE+0x62C),
			DRV_Reg32(DDRPHY_BASE+0x630)); //                          
	seq_printf(m, "\r\nMEMPLL :\r\nMEMPLL13: 0x%08x MEMPLL14: 0x%08x MEMPLL15: 0x%08x MEMPLL16: 0x%08x\r\n",
			DRV_Reg32(DDRPHY_BASE+0x634),
			DRV_Reg32(DDRPHY_BASE+0x638),
			DRV_Reg32(DDRPHY_BASE+0x63C),
			DRV_Reg32(DDRPHY_BASE+0x640)); //                          
		
#if 0
	*start = page + off;

	len = p - page;

	if (len > off)
		len -= off;
	else
		len = 0;

	return len < count ? len : count;
#endif
  return 0;		
}





#if MT_FH_CLK_GEN

//                                                                                                             
static int freqhopping_clkgen_proc_read(struct seq_file* m, void* v)
{
	//               
	//            

	FH_MSG("EN: %s",__func__);
	
	if(g_curr_clkgen > MT658X_FH_PLL_TOTAL_NUM ){
		seq_printf(m, "no clkgen output.\r\n");
	}
	else{
		seq_printf(m, "clkgen:%d\r\n",g_curr_clkgen);
	}

	seq_printf(m, "\r\nMBIST :\r\nMBIST_CFG_2: 0x%08x MBIST_CFG_6: 0x%08x MBIST_CFG_7: 0x%08x\r\n",
			DRV_Reg32(MBIST_CFG_2),
			DRV_Reg32(MBIST_CFG_6),
			DRV_Reg32(MBIST_CFG_7));
			
	seq_printf(m, "\r\nCLK_CFG_3: 0x%08x\r\n",
			DRV_Reg32(CLK_CFG_3));
			
	seq_printf(m, "\r\nTOP_CKMUXSEL: 0x%08x\r\n",
			DRV_Reg32(TOP_CKMUXSEL));

	seq_printf(m, "\r\nGPIO: 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
			DRV_Reg32(GPIO_BASE+0xC60),
			DRV_Reg32(GPIO_BASE+0xC70),
			DRV_Reg32(GPIO_BASE+0xCD0),
			DRV_Reg32(GPIO_BASE+0xD90));
	
	
	seq_printf(m, "\r\nDDRPHY_BASE :\r\n0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\r\n",
			DRV_Reg32(DDRPHY_BASE+0x600),
			DRV_Reg32(DDRPHY_BASE+0x604),
			DRV_Reg32(DDRPHY_BASE+0x608),
			DRV_Reg32(DDRPHY_BASE+0x60C),
			DRV_Reg32(DDRPHY_BASE+0x614),
			DRV_Reg32(DDRPHY_BASE+0x61C));
#if 0
	*start = page + off;

	len = p - page;

	if (len > off)
		len -= off;
	else
		len = 0;

	return len < count ? len : count;
#endif
  return 0;		
}


static int freqhopping_clkgen_proc_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int len = 0, pll_id = 0;
	char clkgen[32];

	FH_MSG("EN: %s",__func__);

	len = (count < (sizeof(clkgen) - 1)) ? count : (sizeof(clkgen) - 1);

	//                                                                
	//                                       

	if (copy_from_user(clkgen, buffer, len))
	{
		FH_MSG("copy_from_user fail!");
		return 1;
	}
	
	clkgen[len] = '\0';
   
	if (sscanf(clkgen, "%d", &pll_id) == 1)
	{
		if(pll_id == MT658X_FH_ARM_PLL){
			fh_write32(MBIST_CFG_2, 0x00000009); //             
			fh_write32(CLK_CFG_3, 0x00000001); //         
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00000001); //                                 
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00000001); //                                 
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00000401); //                                    
			udelay(1000);

		}
		else if(pll_id == MT658X_FH_MAIN_PLL){
			fh_write32(MBIST_CFG_2, 0x00000009); //             
			fh_write32(CLK_CFG_3, 0x00000001); //         
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x000F0001); //                                   
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x000F0001); //                               
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x000F0F01); //                                 
			udelay(1000);

		}
		else if(pll_id == MT658X_FH_MEM_PLL){
						
			fh_write32(DDRPHY_BASE+0x600, ( (DRV_Reg32(DDRPHY_BASE+0x600)) | 0x1<<5));
			
			
			fh_write32(DDRPHY_BASE+0x60C, ( (DRV_Reg32(DDRPHY_BASE+0x60C)) | 0x1<<21));
			fh_write32(DDRPHY_BASE+0x614, ( (DRV_Reg32(DDRPHY_BASE+0x614)) | 0x1<<21));
			fh_write32(DDRPHY_BASE+0x61C, ( (DRV_Reg32(DDRPHY_BASE+0x61C)) | 0x1<<21));

			fh_write32(DDRPHY_BASE+0x60C, ( (DRV_Reg32(DDRPHY_BASE+0x60C)) & ~0x7));
			fh_write32(DDRPHY_BASE+0x60C, ( (DRV_Reg32(DDRPHY_BASE+0x60C)) | 0x2 ));
			fh_write32(DDRPHY_BASE+0x614, ( (DRV_Reg32(DDRPHY_BASE+0x614)) & ~0x7));
			fh_write32(DDRPHY_BASE+0x614, ( (DRV_Reg32(DDRPHY_BASE+0x614)) | 0x2 ));
			fh_write32(DDRPHY_BASE+0x61C, ( (DRV_Reg32(DDRPHY_BASE+0x61C)) & ~0x7));
			fh_write32(DDRPHY_BASE+0x61C, ( (DRV_Reg32(DDRPHY_BASE+0x61C)) | 0x2));

			fh_write32(DDRPHY_BASE+0x604, ( (DRV_Reg32(DDRPHY_BASE+0x604)) | 0x1<<3));
			fh_write32(DDRPHY_BASE+0x604, ( (DRV_Reg32(DDRPHY_BASE+0x604)) | 0x1<<7));
			fh_write32(DDRPHY_BASE+0x604, ( (DRV_Reg32(DDRPHY_BASE+0x604)) | 0x1<<4));
			fh_write32(DDRPHY_BASE+0x604, ( (DRV_Reg32(DDRPHY_BASE+0x604)) | 0x1<<9));

#if 0			
			fh_write32(DDRPHY_BASE+0x608, ( (DRV_Reg32(DDRPHY_BASE+0x608)) & ~0x000E0000 ));
#endif 
			fh_write32(DDRPHY_BASE+0x608, ( (DRV_Reg32(DDRPHY_BASE+0x608)) | 0x00040000 ));

			fh_write32(DDRPHY_BASE+0x608, ( (DRV_Reg32(DDRPHY_BASE+0x608)) & ~0xF0000000 ));
			fh_write32(DDRPHY_BASE+0x608, ( (DRV_Reg32(DDRPHY_BASE+0x608)) | 0x80000000 ));

			//                                                    
			fh_write32(CLK_CFG_3, 0x00000001); //         
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00000001); //                                 
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00000001); //                                 
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00000501); //                                      
			udelay(1000);

		}
		else if(pll_id == MT658X_FH_MSDC_PLL){

			fh_write32(MBIST_CFG_2, 0x00000009); //             
			
			fh_write32(CLK_CFG_3, 0x00000001); //         
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00080001); //                                    
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00080001); //                               
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00080F01); //                                 
			udelay(1000);
						
		}
		else if(pll_id == MT658X_FH_MM_PLL){
			fh_write32(MBIST_CFG_2, 0x00000009); //             
			
			fh_write32(CLK_CFG_3, 0x00000001); //         
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00090001); //                                  
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00090001); //                               
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x00090F01); //                                 
			udelay(1000);

		}
		else if(pll_id == MT658X_FH_VENC_PLL){
			fh_write32(MBIST_CFG_2, 0x00000009); //             
			
			fh_write32(CLK_CFG_3, 0x00000001); //         
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x000A0001); //                                    
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x000A0001); //                               
			udelay(1000);
			fh_write32(CLK_CFG_3, 0x000A0F01); //                                 
			udelay(1000);
			
		}
	}
	else
	{
		FH_MSG("  bad argument!!");
	}
	
	g_curr_clkgen = pll_id;

	return count;

	//               
}

#endif //             
//                                           
static void mt_fh_hal_init(void)
{
	int 		i;
//               
	unsigned long 	flags;
	
	FH_MSG("EN: %s",__func__);
	
	if(g_initialize == 1){
		FH_MSG("already init!");
		return;
	}
		
	//                                       
	memset(g_mempll_fh_table, 0, sizeof(g_mempll_fh_table));

	
	for(i=0;i<MT_FHPLL_MAX;i++) {
		
		//                                          
		spin_lock_irqsave(&freqhopping_lock, flags);
				
		g_fh_pll[i].setting_id = 0;

		fh_write32(REG_FHCTL0_CFG+(i*0x14), 0x00000000); //                     
		fh_write32(REG_FHCTL0_UPDNLMT+(i*0x14), 0x00000000); //                      
		fh_write32(REG_FHCTL0_DDS+(i*0x14), 0x00000000); //                      
		
		//                     
		//                                                                         
		
		spin_unlock_irqrestore(&freqhopping_lock, flags);
	}
	
	//                                                  
	//                                                   
	//                                                                                      
	//                                                
	

	g_initialize = 1;
	//                                                               

}

static void mt_fh_hal_lock(unsigned long *flags)
{
	spin_lock_irqsave(&freqhopping_lock, *flags);	
}

static void mt_fh_hal_unlock(unsigned long *flags)
{
	spin_unlock_irqrestore(&freqhopping_lock, *flags);
}

static int mt_fh_hal_get_init(void)
{
	return(g_initialize);
}

static int mt_fh_hal_is_support_DFS_mode(void)
{
	return TRUE;
}

//                                       
//                                       
//                                

static struct mt_fh_hal_driver g_fh_hal_drv;

struct mt_fh_hal_driver *mt_get_fh_hal_drv(void)
{
	memset(&g_fh_hal_drv, 0, sizeof(g_fh_hal_drv));
	
	g_fh_hal_drv.fh_pll			= g_fh_pll;
	g_fh_hal_drv.fh_usrdef			= mt_ssc_fhpll_userdefined;
	g_fh_hal_drv.pll_cnt 			= MT658X_FH_PLL_TOTAL_NUM;
	g_fh_hal_drv.mempll 			= MT658X_FH_MEM_PLL;
	g_fh_hal_drv.mainpll 			= MT658X_FH_MAIN_PLL;
	g_fh_hal_drv.msdcpll 			= MT658X_FH_MSDC_PLL;
	g_fh_hal_drv.mmpll 			= MT658X_FH_MM_PLL;
	g_fh_hal_drv.vencpll 			= MT658X_FH_VENC_PLL;
	g_fh_hal_drv.lvdspll 			= MT658X_FH_VENC_PLL;
	
	g_fh_hal_drv.mt_fh_hal_init 		=  mt_fh_hal_init;

#if MT_FH_CLK_GEN	
	g_fh_hal_drv.proc.clk_gen_read 		=  freqhopping_clkgen_proc_read;
	g_fh_hal_drv.proc.clk_gen_write 	=  freqhopping_clkgen_proc_write;
#endif 
	
	g_fh_hal_drv.proc.dramc_read 		=  freqhopping_dramc_proc_read;
	g_fh_hal_drv.proc.dramc_write 		=  freqhopping_dramc_proc_write;	
	g_fh_hal_drv.proc.dumpregs_read 	=  freqhopping_dumpregs_proc_read;

	g_fh_hal_drv.proc.dvfs_read 		=  freqhopping_dvfs_proc_read;
	g_fh_hal_drv.proc.dvfs_write 		=  freqhopping_dvfs_proc_write;	

	g_fh_hal_drv.mt_fh_hal_ctrl		= __freqhopping_ctrl;
	g_fh_hal_drv.mt_fh_lock			= mt_fh_hal_lock;
	g_fh_hal_drv.mt_fh_unlock		= mt_fh_hal_unlock;
	g_fh_hal_drv.mt_fh_get_init		= mt_fh_hal_get_init;

	g_fh_hal_drv.mt_fh_popod_restore 	= mt_fh_hal_popod_restore;
	g_fh_hal_drv.mt_fh_popod_save		= mt_fh_hal_popod_save;

	g_fh_hal_drv.mt_l2h_mempll		= mt_fh_hal_l2h_mempll;
	g_fh_hal_drv.mt_h2l_mempll		= mt_fh_hal_h2l_mempll;
	g_fh_hal_drv.mt_l2h_dvfs_mempll		= mt_fh_hal_l2h_dvfs_mempll;
	g_fh_hal_drv.mt_h2l_dvfs_mempll		= mt_fh_hal_h2l_dvfs_mempll;
	g_fh_hal_drv.mt_dfs_armpll		= mt_fh_hal_dfs_armpll;
	g_fh_hal_drv.mt_is_support_DFS_mode		= mt_fh_hal_is_support_DFS_mode;
	g_fh_hal_drv.mt_dram_overclock		= mt_fh_hal_dram_overclock;
	g_fh_hal_drv.mt_get_dramc		= mt_fh_hal_get_dramc;
	g_fh_hal_drv.mt_fh_default_conf	= mt_fh_hal_default_conf;

	return (&g_fh_hal_drv);
}
//                                
