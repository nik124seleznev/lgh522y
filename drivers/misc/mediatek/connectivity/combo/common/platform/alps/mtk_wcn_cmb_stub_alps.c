/*       
                                            

                                                                                           
*/



/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define CMB_STUB_LOG_INFO(fmt, arg...) pr_warn(fmt, ##arg)
#define CMB_STUB_LOG_WARN(fmt, arg...) pr_err(fmt, ##arg)
#define CMB_STUB_LOG_DBG(fmt, arg...) pr_warn(fmt, ##arg)


/*                                                                              
                                                          
                                                                                
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <mach/mtk_wcn_cmb_stub.h>

#include <cust_gpio_usage.h>
/*                                                                                                                           */ */

#include <mach/mt_dcm.h>


/*                                                                              
                                                
                                                                                
*/



/*                                                                              
                                                 
                                                                                
*/




/*                                                                              
                                                  
                                                                                
*/
char *wmt_uart_port_desc = "ttyMT2";	/*                                                                                        */
EXPORT_SYMBOL(wmt_uart_port_desc);

/*                                                                              
                                                   
                                                                                
*/

static wmt_aif_ctrl_cb cmb_stub_aif_ctrl_cb = NULL;
static wmt_func_ctrl_cb cmb_stub_func_ctrl_cb = NULL;
static CMB_STUB_AIF_X cmb_stub_aif_stat = CMB_STUB_AIF_0;

/*                                                                         
                                                                               
                                                                                
                                    
 */
#if 0
static CMB_STUB_AIF_X audio2aif[] = {
	[COMBO_AUDIO_STATE_0] = CMB_STUB_AIF_0,
	[COMBO_AUDIO_STATE_1] = CMB_STUB_AIF_1,
	[COMBO_AUDIO_STATE_2] = CMB_STUB_AIF_2,
	[COMBO_AUDIO_STATE_3] = CMB_STUB_AIF_3,
};
#endif
/*                                                                              
                                                            
                                                                                
*/

/*                                                                                                       
  
                                                            
                                                           
  
                               
 */
extern unsigned int mtk_uart_pdn_enable(char *port, int enable);


/*                                                                              
                                                
                                                                                
*/

/* 
                                                                              
  
                                                                             
                                                                              
  
                                                              
  
                              
                                
 */
int mtk_wcn_cmb_stub_reg(P_CMB_STUB_CB p_stub_cb)
{
	if ((!p_stub_cb)
	    || (p_stub_cb->size != sizeof(CMB_STUB_CB))) {
		CMB_STUB_LOG_WARN("[cmb_stub] invalid p_stub_cb:0x%p size(%d)\n",
				  p_stub_cb, (p_stub_cb) ? p_stub_cb->size : 0);
		return -1;
	}

	CMB_STUB_LOG_DBG("[cmb_stub] registered, p_stub_cb:0x%p size(%d)\n",
			 p_stub_cb, p_stub_cb->size);

	cmb_stub_aif_ctrl_cb = p_stub_cb->aif_ctrl_cb;
	cmb_stub_func_ctrl_cb = p_stub_cb->func_ctrl_cb;
	cmb_stub_do_reset_cb = p_stub_cb->wmt_do_reset_cb;
	return 0;
}

/* 
                                                                             
  
                                                                      
                                                            
  
                              
 */
int mtk_wcn_cmb_stub_unreg(void)
{
	cmb_stub_aif_ctrl_cb = NULL;
	cmb_stub_func_ctrl_cb = NULL;
	cmb_stub_do_reset_cb = NULL;

	CMB_STUB_LOG_INFO("[cmb_stub] unregistered\n");	/*            */

	return 0;
}

/*                                                         */
int mtk_wcn_cmb_stub_aif_ctrl(CMB_STUB_AIF_X state, CMB_STUB_AIF_CTRL ctrl)
{
	int ret;

	if ((CMB_STUB_AIF_MAX <= state)
	    || (CMB_STUB_AIF_CTRL_MAX <= ctrl)) {

		CMB_STUB_LOG_WARN("[cmb_stub] aif_ctrl invalid (%d, %d)\n", state, ctrl);
		return -1;
	}

	/*                                                               */
	if (cmb_stub_aif_ctrl_cb) {
		ret = (*cmb_stub_aif_ctrl_cb) (state, ctrl);
		CMB_STUB_LOG_INFO("[cmb_stub] aif_ctrl_cb state(%d->%d) ctrl(%d) ret(%d)\n", cmb_stub_aif_stat, state, ctrl, ret);	/*            */

		cmb_stub_aif_stat = state;
		return ret;
	} else {
		CMB_STUB_LOG_WARN("[cmb_stub] aif_ctrl_cb null\n");
		return -2;
	}
}

/*                                                                            
                                                                             
                                                                             
                            
 */

void mtk_wcn_cmb_stub_func_ctrl(unsigned int type, unsigned int on)
{
	if (cmb_stub_func_ctrl_cb) {
		(*cmb_stub_func_ctrl_cb) (type, on);
	} else {
		CMB_STUB_LOG_WARN("[cmb_stub] func_ctrl_cb null\n");
	}
}

/*                     */
/*                                                                      */
/*                                                                      */

static int _mt_combo_plt_do_deep_idle(COMBO_IF src, int enter)
{
	int ret = -1;

#if 0
	const char *combo_if_name[] = { "COMBO_IF_UART",
		"COMBO_IF_MSDC"
	};
#endif

	if (src != COMBO_IF_UART && src != COMBO_IF_MSDC) {
		CMB_STUB_LOG_WARN("src = %d is error\n", src);
		return ret;
	}
#if 0
	if (src >= 0 && src < COMBO_IF_MAX) {
		CMB_STUB_LOG_INFO("src = %s, to enter deep idle? %d\n", combo_if_name[src], enter);
	}
#endif
	/*                                                                                      
                                                      */

	switch (src) {
	case COMBO_IF_UART:
		if (enter == 0) {
			/*                                                                     */
			/*                                               */
			ret = mtk_uart_pdn_enable(wmt_uart_port_desc, 0);
			if (ret < 0) {
				CMB_STUB_LOG_WARN("[CMB] %s exit deep idle failed\n",
						  wmt_uart_port_desc);
			}
		} else {
			/*                                                                     */
			/*                                              */
			ret = mtk_uart_pdn_enable(wmt_uart_port_desc, 1);
			if (ret < 0) {
				CMB_STUB_LOG_WARN("[CMB] %s enter deep idle failed\n",
						  wmt_uart_port_desc);
			}
		}
		ret = 0;
		break;

	case COMBO_IF_MSDC:
		if (enter == 0) {
			/*                                                                     */
		} else {
			/*                                                                     */
		}
		ret = 0;
		break;

	default:
		ret = -1;
		break;
	}

	return ret;
}

int mt_combo_plt_enter_deep_idle(COMBO_IF src)
{
	/*           */
	/*                                                                              */
	return _mt_combo_plt_do_deep_idle(src, 1);
}

int mt_combo_plt_exit_deep_idle(COMBO_IF src)
{
	/*           */
	/*                                                                              */
	return _mt_combo_plt_do_deep_idle(src, 0);
}

int
mtk_wcn_cmb_stub_do_reset (unsigned int type)
{
	if (cmb_stub_do_reset_cb)
		return (*cmb_stub_do_reset_cb)(type);
	else
		return -1;
}

EXPORT_SYMBOL(mt_combo_plt_exit_deep_idle);
EXPORT_SYMBOL(mt_combo_plt_enter_deep_idle);
EXPORT_SYMBOL(mtk_wcn_cmb_stub_func_ctrl);
EXPORT_SYMBOL(mtk_wcn_cmb_stub_aif_ctrl);
EXPORT_SYMBOL(mtk_wcn_cmb_stub_do_reset);
EXPORT_SYMBOL(mtk_wcn_cmb_stub_unreg);
EXPORT_SYMBOL(mtk_wcn_cmb_stub_reg);
