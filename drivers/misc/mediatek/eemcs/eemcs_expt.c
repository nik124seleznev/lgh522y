#include <linux/kernel.h>
#include <linux/limits.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/proc_fs.h>
#include <linux/aee.h>
#include <linux/timer.h>
#include <linux/rtc.h>

#include "eemcs_expt.h"
#include "eemcs_ccci.h"
#include "eemcs_char.h"
#include "eemcs_file_ops.h"
#include "eemcs_debug.h"
#include "eemcs_state.h"
#include "eemcs_boot.h"
#include "eemcs_md.h"
#include "eemcs_expt_ut.h"


//                                                                   
static volatile EEMCS_EXCEPTION_STATE eemcs_exception_state = EEMCS_EX_INVALID;

static EEMCS_EXCEPTION_SET g_except_inst;
static KAL_UINT32 except_txq = 0, except_rxq = 0;
static KAL_UINT32 nonstop_rxq = 0; /*                                            */
static ccci_expt_port_cfg ccci_expt_port_info[CCCI_PORT_NUM_MAX] = {
    {HIF_SDIO, TX_Q_0, RX_Q_0},              /*              */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*             */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*             */
    {HIF_SDIO, TX_Q_3, RX_Q_3},              /*              */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*             */
    {HIF_SDIO, TX_Q_1, RX_Q_1},              /*            */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*              */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*             */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*             */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*             */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*                  */
    {HIF_SDIO, TX_Q_2, RX_Q_2},              /*                */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*                   */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*                  */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*                   */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*                   */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*                    */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*               */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*              */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*            */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*              */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*              */
    {HIF_SDIO, TR_Q_INVALID, TR_Q_INVALID},  /*              */   
};

//                   
static KAL_CHAR *port_name[CCCI_CDEV_NUM] = {
    "CTRL",
    "SYS",
    "AUD",
    "MDLOGC",
    "MUX",
    "FS",
    "PMIC",
    "UEM",
    "RPC",
    "IPC",
    "AGPS",
    "MDLOG",
    "IMSV",
    "IMSC",     
    "IMSA",
    "IMSD",
    "MUXREP",
    "IOC",
    "RILD",
    "IT",
    "NET1",
    "NET2",
    "NET3",
};


/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */

ccci_expt_port_cfg* get_expt_port_info(KAL_UINT32 port_id){
    DEBUG_LOG_FUNCTION_ENTRY;
    KAL_ASSERT(port_id < CCCI_PORT_NUM_MAX);
    DEBUG_LOG_FUNCTION_LEAVE;
    return &ccci_expt_port_info[port_id];
}

/*
                                      
                 
                                       
 */
KAL_INT32 get_exception_mode(void)
{
    /*                     */
    return eemcs_exception_state;
}

/*
                                  
                 
                                       
 */
KAL_INT32 get_md_expt_type(void)
{
    return g_except_inst.md_ex_type;
}

/* 
                                        
                                                                                  
                                                            
 */
kal_bool set_exception_mode(EEMCS_EXCEPTION_STATE new_mode)
{
    EEMCS_EXCEPTION_STATE cur_mode;
    DEBUG_LOG_FUNCTION_ENTRY;

    KAL_ASSERT((new_mode >= EEMCS_EX_NONE) && (new_mode <= EEMCS_EX_REC_MSG_OK));
	
    cur_mode = get_exception_mode();
	
#if 0
    switch(cur_mode)
    {
        case EEMCS_EX_INVALID:
            goto _err;
            break;
        case EEMCS_EX_NONE:
            if(EEMCS_EX_INIT != new_mode)
                goto _err;
            break;    
        case EEMCS_EX_INIT:
            if(EEMCS_EX_DHL_DL_RDY != new_mode)
                goto _err;
            break;
        case EEMCS_EX_DHL_DL_RDY:
            if(EEMCS_EX_INIT_DONE != new_mode)
                goto _err;
            break;
        case EEMCS_EX_INIT_DONE:
            /*                                       */
            if(EEMCS_EX_INIT != new_mode)
                goto _err;
            break;
        default:
            goto _err;
            break;
    }
#endif
    /*                     */
    eemcs_exception_state = new_mode;
    DBGLOG(EXPT, DBG, "Exception mode: 0x%X -> 0x%X", cur_mode, new_mode);
    DEBUG_LOG_FUNCTION_LEAVE;
    return true;

#if 0
_err:
    DBGLOG(EXPT, ERR, "Invalid exception mode: 0x%X -> 0x%X", cur_mode, new_mode);
    DEBUG_LOG_FUNCTION_LEAVE;
    return false;
#endif
}

/* 
                                                                      
                                                                                     
                                           
                                           
                                                 
                                                
                                                                                  
 */
kal_bool is_exception_mode(EEMCS_EXCEPTION_STATE *mode)
{
    kal_bool ret = false;
    KAL_UINT32 state = EEMCS_INVALID;

    DEBUG_LOG_FUNCTION_ENTRY;

    state = check_device_state();
    if(EEMCS_EXCEPTION == state)
    {
        if(NULL != mode)
            *mode = get_exception_mode();
        ret = true;
    }
    else
    {
        if(NULL != mode)
            *mode = EEMCS_EX_NONE;
    }
    
    DEBUG_LOG_FUNCTION_LEAVE;
    return ret;
}

/* 
                                                                 
                                  
                                                         
                                                              
 */
kal_bool is_valid_exception_port(KAL_UINT32 port_id, kal_bool is_rx)
{
    kal_bool valid = true;
    EEMCS_EXCEPTION_STATE mode = get_exception_mode();

    KAL_ASSERT((port_id >= START_OF_CCCI_CDEV) && (port_id < CCCI_PORT_NUM_MAX));

    do{
        /*                                            */
        //                             
        //          

        //                                 
        if(port_id == CCCI_PORT_CTRL)
			break;    

        if(is_rx && (port_id == CCCI_PORT_MD_LOG))
            break;
        /*                                                      */
        if(mode >= EEMCS_EX_INIT_DONE)
        {
            if((ccci_expt_port_info[port_id].expt_txq_id != TR_Q_INVALID) && (ccci_expt_port_info[port_id].expt_rxq_id != TR_Q_INVALID))
                break;
        }
        valid = false;
    }while(0);

    return valid;
}

kal_bool is_valid_exception_tx_channel(CCCI_CHANNEL_T chn)
{
    KAL_UINT32 port_id = 0;

    port_id = ccci_ch_to_port(chn);

    return is_valid_exception_port(port_id, false);
}

/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*
                                                              
         
                           
          
           
 */
void eemcs_expt_dev_tx_block(KAL_UINT32 port_id)
{
    atomic_inc(&g_except_inst.ptx_blk_cnt[port_id]);
}

/*
                                                                  
         
                           
          
           
 */
void eemcs_expt_dev_tx_drop(KAL_UINT32 port_id)
{
    atomic_inc(&g_except_inst.ptx_drp_cnt[port_id]);
}

/*
                                                            
         
                           
          
           
 */
void eemcs_expt_dev_rx_block(KAL_UINT32 port_id)
{
    atomic_inc(&g_except_inst.prx_blk_cnt[port_id]);
}

/*
                                                                
         
                           
          
           
 */
void eemcs_expt_dev_rx_drop(KAL_UINT32 port_id)
{
    atomic_inc(&g_except_inst.prx_drp_cnt[port_id]);
}

/*
                                                               
         
                           
          
           
 */
void eemcs_expt_ccci_tx_drop(KAL_UINT32 port_id)
{
    atomic_inc(&g_except_inst.ccci_tx_drp_cnt[port_id]);
}

/*
                                                              
         
                           
          
           
 */
void eemcs_expt_ccci_rx_drop(KAL_UINT32 port_id)
{
    atomic_inc(&g_except_inst.ccci_rx_drp_cnt[port_id]);
}

/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
static void eemcs_mem_dump(void *start_addr, int len)
{
	unsigned int *curr_p = (unsigned int *)start_addr;
	unsigned char *curr_ch_p;
	int _16_fix_num = len/16;
	int tail_num = len%16;
	char buf[16];
	int i,j;

	if(NULL == curr_p) {
		DBGLOG(EXPT, ERR,"NULL point!");
		return;
	}
	if(0 == len){
		DBGLOG(EXPT, ERR,"Not need to dump");
		return;
	}

	DBGLOG(EXPT, DBG,"Base: %08x", (unsigned int)start_addr);
	//            
	for(i=0; i<_16_fix_num; i++){
		DBGLOG(EXPT, DBG,"%03X: %08X %08X %08X %08X", 
				i*16, *curr_p, *(curr_p+1), *(curr_p+2), *(curr_p+3) );
		curr_p+=4;
	}

	//             
	if(tail_num > 0){
		curr_ch_p = (unsigned char*)curr_p;
		for(j=0; j<tail_num; j++){
			buf[j] = *curr_ch_p;
			curr_ch_p++;
		}
		for(; j<16; j++)
			buf[j] = 0;
		curr_p = (unsigned int*)buf;
		DBGLOG(EXPT, DBG,"%03X: %08X %08X %08X %08X", 
				i*16, *curr_p, *(curr_p+1), *(curr_p+2), *(curr_p+3) );
	}
}


/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */

/*
                                                      
         
                                                                
                                                                 
     
          
            
 */
void eemcs_ee_info_dump(DEBUG_INFO_T *debug_info, char* except_info_addr)
{
	char ex_info[EE_BUF_LEN]="";
	char i_bit_ex_info[EE_BUF_LEN]="\nMay I-Bit dis too long\n";
	
	struct rtc_time		tm;
	struct timeval		tv = {0};
	struct timeval		tv_android = {0};
	struct rtc_time		tm_android;

	do_gettimeofday(&tv);
	tv_android = tv;
	rtc_time_to_tm(tv.tv_sec, &tm);
	tv_android.tv_sec -= sys_tz.tz_minuteswest*60;
	rtc_time_to_tm(tv_android.tv_sec, &tm_android);
	DBGLOG(EXPT, DBG, "");
	DBGLOG(EXPT, DBG, "Sync:%d%02d%02d %02d:%02d:%02d.%u(%02d:%02d:%02d.%03d(TZone))", 
		   tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
		   tm.tm_hour, tm.tm_min, tm.tm_sec,
		   (unsigned int) tv.tv_usec,
		   tm_android.tm_hour, tm_android.tm_min, tm_android.tm_sec,
		   (unsigned int) tv_android.tv_usec);
	DBGLOG(EXPT, DBG, "exception type(%d):%s",debug_info->type,debug_info->name?:"Unknown");

	switch(debug_info->type)
	{
		case MD_EX_TYPE_ASSERT_DUMP:
		case MD_EX_TYPE_ASSERT:
			DBGLOG(EXPT, DBG, "filename = %s", debug_info->assert.file_name);
			DBGLOG(EXPT, DBG, "line = %d", debug_info->assert.line_num);
			DBGLOG(EXPT, DBG, "para0 = %d, para1 = %d, para2 = %d", 
					debug_info->assert.parameters[0],
					debug_info->assert.parameters[1],
					debug_info->assert.parameters[2]);
			snprintf(ex_info,EE_BUF_LEN,"\n[%s] file:%s line:%d\np1:0x%08x\np2:0x%08x\np3:0x%08x\n",
					debug_info->name, 
					debug_info->assert.file_name,
					debug_info->assert.line_num, 
					debug_info->assert.parameters[0],
					debug_info->assert.parameters[1],
					debug_info->assert.parameters[2]);
			break;
		case MD_EX_TYPE_UNDEF:
		case MD_EX_TYPE_SWI:
		case MD_EX_TYPE_PREF_ABT:
		case MD_EX_TYPE_DATA_ABT:
		case MD_EX_TYPE_FATALERR_BUF:
		case MD_EX_TYPE_FATALERR_TASK:
			DBGLOG(EXPT, DBG, "fatal error code 1 = %d", debug_info->fatal_error.err_code1);
			DBGLOG(EXPT, DBG, "fatal error code 2 = %d", debug_info->fatal_error.err_code2);
			snprintf(ex_info,EE_BUF_LEN,"\n[%s] err_code1:%d err_code2:%d\n", debug_info->name, 
					debug_info->fatal_error.err_code1, debug_info->fatal_error.err_code2);
			break;
		case MD_EX_TYPE_EMI_CHECK:
			DBGLOG(EXPT, ERR, "md_emi_check: %08X, %08X, %02d, %08X", 
					debug_info->data.data0, debug_info->data.data1,
					debug_info->data.channel, debug_info->data.reserved);
			snprintf(ex_info,EE_BUF_LEN,"\n[emi_chk] %08X, %08X, %02d, %08X\n", 
					debug_info->data.data0, debug_info->data.data1,
					debug_info->data.channel, debug_info->data.reserved);
			break;
		case DSP_EX_TYPE_ASSERT:
			DBGLOG(EXPT, DBG, "filename = %s", debug_info->dsp_assert.file_name);
			DBGLOG(EXPT, DBG, "line = %d", debug_info->dsp_assert.line_num);
			DBGLOG(EXPT, DBG, "exec unit = %s", debug_info->dsp_assert.execution_unit);
			DBGLOG(EXPT, DBG, "para0 = %d, para1 = %d, para2 = %d", 
					debug_info->dsp_assert.parameters[0],
					debug_info->dsp_assert.parameters[1],
					debug_info->dsp_assert.parameters[2]);
			snprintf(ex_info,EE_BUF_LEN,"\n[%s] file:%s line:%d\nexec:%s\np1:%d\np2:%d\np3:%d\n",
					debug_info->name, debug_info->assert.file_name, debug_info->assert.line_num,
					debug_info->dsp_assert.execution_unit, 
					debug_info->dsp_assert.parameters[0],
					debug_info->dsp_assert.parameters[1],
					debug_info->dsp_assert.parameters[2]);
			break;
		case DSP_EX_TYPE_EXCEPTION:
			DBGLOG(EXPT, DBG, "exec unit = %s, code1:0x%08x", debug_info->dsp_exception.execution_unit,
					debug_info->dsp_exception.code1);
			snprintf(ex_info,EE_BUF_LEN,"\n[%s] exec:%s code1:0x%08x\n",
					debug_info->name, debug_info->dsp_exception.execution_unit,
					debug_info->dsp_exception.code1);
			break;
		case DSP_EX_FATAL_ERROR:
			DBGLOG(EXPT, DBG, "exec unit = %s", debug_info->dsp_fatal_err.execution_unit);
			DBGLOG(EXPT, DBG, "err_code0 = 0x%08x, err_code1 = 0x%08x", 
					debug_info->dsp_fatal_err.err_code[0],
					debug_info->dsp_fatal_err.err_code[1]);

			snprintf(ex_info,EE_BUF_LEN,"\n[%s] exec:%s err_code1:0x%08x err_code2:0x%08x\n",
					debug_info->name, debug_info->dsp_fatal_err.execution_unit, 
					debug_info->dsp_fatal_err.err_code[0],
					debug_info->dsp_fatal_err.err_code[1]);
			break;
		default: //                            
			snprintf(ex_info,EE_BUF_LEN,"\n[%s]\n", debug_info->name);
			break;
	}

	//                    
	switch(debug_info->more_info)
	{
		case MD_EE_CASE_ONLY_EX:
			strcat(ex_info, "\nTime out case\n");
			break;
			
		case MD_EE_CASE_ONLY_EX_OK:
			strcat(ex_info, "\nOnly EX_OK case\n");
			break;
		case MD_EE_CASE_AP_MASK_I_BIT_TOO_LONG:
			strcat(i_bit_ex_info, ex_info);
			strcpy(ex_info, i_bit_ex_info);
			break;
		case MD_EE_CASE_TX_TRG:
		case MD_EE_CASE_ISR_TRG:
			//                                              
			break;

		case MD_EE_CASE_NO_RESPONSE:
			strcat(ex_info, "\nMD long time no response\n");
			break;
			
		default:
			break;
	}

	//                            
	
	memcpy(g_except_inst.expt_info_mem, except_info_addr, MD_EX_LOG_SIZE);
	eemcs_aed(CCCI_AED_DUMP_EX_MEM, ex_info);
}


/*
                                                              
         
                                                  
                                                                
     
          
            
 */
static void eemcs_md_exception_data_parse(char* md_ex_addr, DEBUG_INFO_T *debug_info)
{
	EX_LOG_T	*ex_info;
	int			ee_type;

	if(debug_info == NULL) {
		return;
	}

	ex_info = (EX_LOG_T*)md_ex_addr;
	memset(debug_info,0,sizeof(DEBUG_INFO_T));
	ee_type = ex_info->header.ex_type;
	debug_info->type = ee_type;
    
	g_except_inst.md_ex_type = ee_type;

	switch (ee_type) 
	{
		case MD_EX_TYPE_INVALID:
			debug_info->name="INVALID";
			break;

		case MD_EX_TYPE_UNDEF:
			debug_info->name="Fatal error (undefine)";
			debug_info->fatal_error.err_code1=ex_info->content.fatalerr.error_code.code1;
			debug_info->fatal_error.err_code2=ex_info->content.fatalerr.error_code.code2;
			break;

		case MD_EX_TYPE_SWI:
			debug_info->name="Fatal error (swi)";
			debug_info->fatal_error.err_code1=ex_info->content.fatalerr.error_code.code1;
			debug_info->fatal_error.err_code2=ex_info->content.fatalerr.error_code.code2;
			break;

		case MD_EX_TYPE_PREF_ABT:
			debug_info->name="Fatal error (prefetch abort)";
			debug_info->fatal_error.err_code1=ex_info->content.fatalerr.error_code.code1;
			debug_info->fatal_error.err_code2=ex_info->content.fatalerr.error_code.code2;
			break;

		case MD_EX_TYPE_DATA_ABT:
			debug_info->name="Fatal error (data abort)";
			debug_info->fatal_error.err_code1=ex_info->content.fatalerr.error_code.code1;
			debug_info->fatal_error.err_code2=ex_info->content.fatalerr.error_code.code2;
			break;

		case MD_EX_TYPE_ASSERT:
			debug_info->name="ASSERT";
			snprintf(debug_info->assert.file_name,sizeof(debug_info->assert.file_name),
					ex_info->content.assert.filename);	
			debug_info->assert.line_num = ex_info->content.assert.linenumber;
			debug_info->assert.parameters[0] = ex_info->content.assert.parameters[0];
			debug_info->assert.parameters[1] = ex_info->content.assert.parameters[1];
			debug_info->assert.parameters[2] = ex_info->content.assert.parameters[2];
			break;

		case MD_EX_TYPE_FATALERR_TASK:
			debug_info->name="Fatal error (task)";
			debug_info->fatal_error.err_code1=ex_info->content.fatalerr.error_code.code1;
			debug_info->fatal_error.err_code2=ex_info->content.fatalerr.error_code.code2;
			break;

		case MD_EX_TYPE_FATALERR_BUF:
			debug_info->name="Fatal error (buff)";
			debug_info->fatal_error.err_code1=ex_info->content.fatalerr.error_code.code1;
			debug_info->fatal_error.err_code2=ex_info->content.fatalerr.error_code.code2;
			break;

		case MD_EX_TYPE_LOCKUP:
			debug_info->name="Lockup";
			break;

		case MD_EX_TYPE_ASSERT_DUMP:
			debug_info->name="ASSERT DUMP";
			snprintf(debug_info->assert.file_name,sizeof(debug_info->assert.file_name),
					ex_info->content.assert.filename);
			debug_info->assert.line_num=ex_info->content.assert.linenumber;
			break;

		case DSP_EX_TYPE_ASSERT:
			debug_info->name="MD DMD ASSERT";
			snprintf(debug_info->dsp_assert.file_name,sizeof(debug_info->dsp_assert.file_name),
					ex_info->content.assert.filename);
			debug_info->dsp_assert.line_num = ex_info->content.assert.linenumber;
			snprintf(debug_info->dsp_assert.execution_unit,sizeof(debug_info->dsp_assert.execution_unit),
					ex_info->envinfo.execution_unit);	
			debug_info->dsp_assert.parameters[0] = ex_info->content.assert.parameters[0];
			debug_info->dsp_assert.parameters[1] = ex_info->content.assert.parameters[1];
			debug_info->dsp_assert.parameters[2] = ex_info->content.assert.parameters[2];
			break;

		case DSP_EX_TYPE_EXCEPTION:
			debug_info->name="MD DMD Exception";
			snprintf(debug_info->dsp_exception.execution_unit,sizeof(debug_info->dsp_exception.execution_unit),
					ex_info->envinfo.execution_unit);
			debug_info->dsp_exception.code1 = ex_info->content.fatalerr.error_code.code1;
			break;

		case DSP_EX_FATAL_ERROR:
			debug_info->name="MD DMD FATAL ERROR";
			snprintf(debug_info->dsp_fatal_err.execution_unit,sizeof(debug_info->dsp_fatal_err.execution_unit),
					ex_info->envinfo.execution_unit);	
			debug_info->dsp_fatal_err.err_code[0] = ex_info->content.fatalerr.error_code.code1;
			debug_info->dsp_fatal_err.err_code[1] = ex_info->content.fatalerr.error_code.code2;
			break;

		default:
			debug_info->name= "UNKNOW Excep";
			break;
	}

	debug_info->ext_mem=(int*)ex_info;
	debug_info->ext_size=MD_EX_LOG_SIZE;

	return;
}

/*
                                           
         
                                                  

          
            
 */
void eemcs_aed(unsigned int dump_flag, char *aed_str)
{
	#define AED_STR_LEN		(512)
	int *ex_log_addr = NULL;
	int ex_log_len = 0;
	int *md_img_addr = NULL;
	int md_img_len = 0;
	int info_str_len = 0;
	char buff[AED_STR_LEN];
	char img_inf[MD_INFO_STR_LEN]="";
	
	eemcs_get_md_info_str(img_inf);
	info_str_len = strlen(aed_str);
	info_str_len += strlen(img_inf);

	if(info_str_len > AED_STR_LEN){
		buff[AED_STR_LEN-1] = '\0'; //                                 
	}

	snprintf(buff, AED_STR_LEN, "\n%s%s\n", aed_str, img_inf);

	if(dump_flag & CCCI_AED_DUMP_EX_MEM){
		ex_log_addr = (int *)g_except_inst.expt_info_mem;
		ex_log_len = MD_EX_MEM_SIZE;
		
		DBGLOG(EXPT, DBG, "");
		DBGLOG(EXPT, DBG, "Dump MD Exception share memory");
		eemcs_mem_dump(g_except_inst.expt_info_mem, MD_EX_MEM_SIZE);
	}

	#if defined (CONFIG_MTK_AEE_FEATURE) && defined (ENABLE_AEE_MD_EE)
	aed_md_exception_api(ex_log_addr, ex_log_len, md_img_addr, md_img_len, buff, DB_OPT_FTRACE);
	#endif
}

void md_emi_check(CCCI_BUFF_T *buff, DEBUG_INFO_T *debug_info)
{
    DBGLOG(EXPT, DBG, "md_emi_check");
    if ((buff==NULL) || (debug_info==NULL))
		return;
    
	memset(debug_info,0,sizeof(DEBUG_INFO_T));
	debug_info->type=MD_EX_TYPE_EMI_CHECK;
	debug_info->name="EMI_CHK";
	debug_info->data=*buff;
    return;
}


static void ex_monitor_func(unsigned long data)
{
	char ex_info[EE_BUF_LEN] = "";

	DBGLOG(EXPT, DBG, "md exception timout at exp_state=%d", eemcs_exception_state);
	if ((eemcs_exception_state < EEMCS_EX_INIT_DONE) && \
		(eemcs_exception_state >= EEMCS_EX_INIT)) {
		snprintf(ex_info, EE_BUF_LEN,"\n[INVALID ASSERT] lte sdio exception handshake timeout(>%ds)\n", \
				EE_INIT_TIMER/HZ);
	} else if (eemcs_exception_state == EEMCS_EX_INIT_DONE) {
		snprintf(ex_info, EE_BUF_LEN,"\n[INVALID ASSERT] AP receive MD_EX msg timeout(>%ds)\n", \
				EE_HS1_TIMER/HZ);
	} else if (eemcs_exception_state == EEMCS_EX_MSG_OK) {
		snprintf(ex_info, EE_BUF_LEN,"\n[INVALID ASSERT] AP receive MD_EX_REC_OK msg timeout(>%ds)\n", \
				EE_HS2_TIMER/HZ);
	}

	eemcs_aed(0, ex_info);
}

/*
                                     
         
                                                                       
                                                        
  
          
                            
                         
 */
KAL_INT32 eemcs_expt_handshake(struct sk_buff *skb){
    CCCI_BUFF_T *msg = (CCCI_BUFF_T *)skb->data;
    DEBUG_INFO_T debug_info = {0};
    KAL_INT32 ret = KAL_SUCCESS;

    switch(msg->id){
       case MD_EX :
            if(msg->reserved != MD_EX_CHK_ID){
                DBGLOG(EXPT, ERR, "Invalid MD_EX: %08X, %08X, %08X, %08X",
                	msg->magic, msg->id, msg->channel, msg->reserved);
                ret = KAL_FAIL;
                break;
            }
            DBGLOG(EXPT, DBG, "receive MD_EX");
           
            //                                     
            /*                                          */
            //                                                         

            msg->channel = CH_CTRL_TX;
            eemcs_ccci_UL_write_skb_to_swq(CH_CTRL_TX, skb);
			
            //                                            
            if (eemcs_exception_state != EEMCS_EX_REC_MSG_OK) {
                set_exception_mode(EEMCS_EX_MSG_OK);
                mod_timer(&g_except_inst.md_ex_monitor, jiffies+EE_HS2_TIMER);
            }
			
            return ret;
            
        case MD_EX_REC_OK :
            if(msg->reserved != MD_EX_REC_OK_CHK_ID){
                DBGLOG(EXPT, ERR, "Invalid MD_EX_REC_OK: %08X, %08X, %08X, %08X",
                	msg->magic, msg->id, msg->channel, msg->reserved);
                ret = KAL_FAIL;
                break;
            }
            DBGLOG(EXPT, DBG, "receive MD_EX_REC_OK");

            set_exception_mode(EEMCS_EX_REC_MSG_OK);
            del_timer(&g_except_inst.md_ex_monitor);

            DBGLOG(EXPT, DBG, "eemcs_md_exception_data_parse");
            eemcs_md_exception_data_parse((char*)(msg+1), &debug_info);
            eemcs_ee_info_dump(&debug_info, (char*)(msg+1));
            break;
            
        case MD_EX_RESUME_CHK_ID :
            DBGLOG(EXPT, DBG, "receive MD_EX_RESUME_CHK_ID");
            md_emi_check(msg, &debug_info);
            eemcs_ee_info_dump(&debug_info, (char*)(msg+1));
            break;
            
        case CCCI_DRV_VER_ERROR :
            DBGLOG(EXPT, ERR, "AP EEMCS driver version mis-match to MD!!");
            eemcs_aed(0, "AP/MD driver version mis-match\n");
            break;
            
        default:
            DBGLOG(EXPT, ERR, "[EXPT] Invalid MD_EX_MAGIC: %08X, %08X, %02d, %08X",
            	msg->magic, msg->id, msg->channel, msg->reserved);
            break;
    }
	
    dev_kfree_skb(skb);
    return ret;
}

KAL_INT32 eemcs_bootup_trace(struct sk_buff *skb){
    CCCI_BUFF_T *msg = (CCCI_BUFF_T *)skb->data;	
	static KAL_UINT32 mem_offset = MD_EX_BOOT_TRA_START_OFF;
	MD_BOOTUP_TRACE *mem_addr = NULL;
	
	if (msg->magic == MD_TSID_MAGIC) {
		mem_addr = (MD_BOOTUP_TRACE *)(g_except_inst.expt_info_mem + mem_offset);
		mem_addr->TsId = msg->id;
		mem_addr->TimeStp = msg->reserved;

		/*
                                                                                           
                                                     
  */
		mem_offset += sizeof(MD_BOOTUP_TRACE);
		if (mem_offset > MD_EX_BOOT_TRA_END_OFF)
			mem_offset = MD_EX_BOOT_TRA_START_OFF;
	}

	dev_kfree_skb(skb);
	return KAL_SUCCESS;
}

static void eemcs_wdt_reset_work_func(struct work_struct *work){
    DBGLOG(EXPT, DBG, "eemcs_wdt_reset_work_func modem reset");
    eemcs_md_reset();
}

KAL_INT32 eemcs_wdt_reset_callback(void){
    static DECLARE_WORK(eemcs_wdt_reset_work, eemcs_wdt_reset_work_func);
    DBGLOG(EXPT, DBG, "eemcs_wdt_reset_callback");
    schedule_work(&eemcs_wdt_reset_work);
    return 0;
}


/*
                                                   
         
                                            
                                                                  
          
                                                
 */
KAL_INT32 eemcs_expt_log_port(struct sk_buff *skb, KAL_UINT32 port_id)
{
    CCCI_BUFF_T *ccci_h = NULL;
    DEBUG_LOG_FUNCTION_ENTRY;

    ccci_h = (CCCI_BUFF_T *)skb->data;
    DBGLOG(EXPT, DBG, "PORT%d add to expt list: CCCI_H(0x%08X, 0x%08X, %02d, 0x%08X) ",\
		port_id, \
        ccci_h->data[0], ccci_h->data[1], ccci_h->channel, ccci_h->reserved);
    skb_queue_tail(&g_except_inst.port[port_id].skb_list, skb);
    atomic_inc(&g_except_inst.port[port_id].pkt_cnt);
    DEBUG_LOG_FUNCTION_LEAVE;
    return KAL_SUCCESS;
}

/*
                                                   
         
                                       
                                                                  
          
                                                
 */
KAL_INT32 eemcs_expt_log_txq(struct sk_buff *skb, KAL_UINT32 qno)
{
    CCCI_BUFF_T *ccci_h = NULL;
    DEBUG_LOG_FUNCTION_ENTRY;

    ccci_h = (CCCI_BUFF_T *)skb->data;
    DBGLOG(EXPT, DBG, "[EXPT] Log CCCI_H(0x%X)(0x%X)(0x%X)(0x%X) to Tx-Q(%d)",
        ccci_h->data[0], ccci_h->data[1], ccci_h->channel, ccci_h->reserved,
        qno);
    g_except_inst.txq[qno].id = qno;
    skb_queue_tail(&g_except_inst.txq[qno].skb_list, skb);
    atomic_inc(&g_except_inst.txq[qno].pkt_cnt);
    DEBUG_LOG_FUNCTION_LEAVE;
    return KAL_SUCCESS;
}

/*
                                                   
         
                                       
                                                                  
          
                                                
 */
KAL_INT32 eemcs_expt_log_rxq(struct sk_buff *skb, KAL_UINT32 qno)
{
    CCCI_BUFF_T *ccci_h = NULL;
    DEBUG_LOG_FUNCTION_ENTRY;

    ccci_h = (CCCI_BUFF_T *)skb->data;
    DBGLOG(EXPT, DBG, "[EXPT] Log CCCI_H(0x%X)(0x%X)(0x%X)(0x%X) to Rx-Q(%d)",
        ccci_h->data[0], ccci_h->data[1], ccci_h->channel, ccci_h->reserved,
        qno);
    g_except_inst.rxq[qno].id = qno;
    skb_queue_tail(&g_except_inst.rxq[qno].skb_list, skb);
    atomic_inc(&g_except_inst.rxq[qno].pkt_cnt);
    DEBUG_LOG_FUNCTION_LEAVE;
    return KAL_SUCCESS;
}


/*
                                                                    
         
           
          
           
 */
void eemcs_ccci_expt_log_df_q_pkts(void)
{
    KAL_INT32 pkts = 0;
    KAL_INT32 i = 0, j = 0;
    struct sk_buff *skb = NULL;
    DEBUG_LOG_FUNCTION_ENTRY;

    /*                              */
    for (i = 0; i < SDIO_RX_Q_NUM; i++) {
        //                                        
        if (!test_bit(i, (unsigned long *)&nonstop_rxq)) {
            pkts = hif_expt_dl_pkt_in_swq(i);
            if (pkts == 0) {
                DBGLOG(EXPT, TRA, "Nothing in DL SWQ %d", i);
                continue;
            } else {
                DBGLOG(EXPT, TRA, "Unhandled %d packets in DL SWQ %d", pkts, i);
            }
            /*                       */
            for (j = 0; j < pkts; j++) {
                skb = hif_expt_dl_read_swq(i);
                if (skb == NULL) {
                    DBGLOG(EXPT, ERR, "Failed to read skb from RX SWQ %d", i);
                } else {
                    hif_expt_dl_pkt_handle_complete(i);
                    //                                            
                    //                   
                    eemcs_expt_log_rxq(skb, i);
                }
            }
        } else {
            DBGLOG(EXPT, TRA, "Skip logging unhandled packets in DL SWQ %d", i);
        }
    }
#if 0
    /*                              */
    for (i = 0; i < SDIO_TX_Q_NUM; i++) {
        pkts = hif_expt_ul_pkt_in_swq(i);
        if (pkts == 0) {
            DBGLOG(CCCI, WAR, "[CCCI] Nothing in UL SWQ %d", i);
            continue;
        } else {
            DBGLOG(CCCI, TRA, "[CCCI] Unhandled %d packets in UL SWQ %d", pkts, i);
        }
        /*                       */
        for (j = 0; j < pkts; j++) {
            skb = hif_expt_ul_read_swq(i);
            if (skb == NULL) {
                DBGLOG(CCCI, ERR, "[CCCI] Failed to read skb from TX SWQ %d", i);
            } else {
                eemcs_expt_log_txq(skb, i);
            }
        }
    }
#endif
    DEBUG_LOG_FUNCTION_LEAVE;
}


/*
                                               
         
                                                  
           
                                                             
                                       
 */
KAL_UINT32 eemcs_register_expt_callback(EEMCS_CCCI_EXCEPTION_IND_CALLBACK func_ptr)
{
    KAL_UINT32 i = 0;
    DEBUG_LOG_FUNCTION_ENTRY;

    do {
        for (i = 0; i < CCCI_PORT_NUM_MAX; i++) {
            spin_lock(&g_except_inst.expt_cb_lock);
            if (g_except_inst.expt_cb[i] == NULL) {
                g_except_inst.expt_cb[i] = func_ptr;
                spin_unlock(&g_except_inst.expt_cb_lock);
                DBGLOG(EXPT, DBG, "Register exception callback(%d)", i);
                break;
            }
            spin_unlock(&g_except_inst.expt_cb_lock);
        }

    } while (0);

    DEBUG_LOG_FUNCTION_LEAVE;
    if (i < CCCI_PORT_NUM_MAX) {
        return i;
    } else {
        DBGLOG(EXPT, ERR, "register exception callback fail");
        return KAL_FAIL;
    }
}

/*
                                                 
         
                                                                              
           
                                      
 */
KAL_UINT32 eemcs_unregister_expt_callback(KAL_UINT32 id)
{
    DEBUG_LOG_FUNCTION_ENTRY;
    KAL_ASSERT(id >= 0);
    KAL_ASSERT(id < CCCI_PORT_NUM_MAX);
    spin_lock(&g_except_inst.expt_cb_lock);
    g_except_inst.expt_cb[id] = NULL;
    spin_unlock(&g_except_inst.expt_cb_lock);
    DBGLOG(EXPT, DBG, "[EXPT] Unregister exception callback (%d)", id);
    DEBUG_LOG_FUNCTION_LEAVE;
    return KAL_SUCCESS;
}

/*
                                            
         
                                  
          
              
 */
void eemcs_exec_expt_callback(KAL_UINT32 msg_id)
{
    KAL_UINT32 i=0;

    DEBUG_LOG_FUNCTION_ENTRY;
    for(i = 0; i < CCCI_PORT_NUM_MAX; i++)
    {
        spin_lock(&g_except_inst.expt_cb_lock);
        if(g_except_inst.expt_cb[i] != NULL)
        {
            g_except_inst.expt_cb[i](msg_id);
        }
        spin_unlock(&g_except_inst.expt_cb_lock);
    }
    DEBUG_LOG_FUNCTION_LEAVE;
}

/*
                                                                                
         
           
          
           
 */
ssize_t eemcs_expt_show_statistics(char *buf)
{
    KAL_UINT32 i = 0;
    ssize_t pos = 0;

    DEBUG_LOG_FUNCTION_ENTRY;
    pos += snprintf(buf + pos, PAGE_SIZE, "\n==================================== Exception Mode Statistics ====================================\n\n");
    pos += snprintf(buf + pos, PAGE_SIZE, "%6s %12s %12s %12s %12s %12s %12s %12s\n", "Port", "DEV_TX_BLK", "DEV_TX_DRP", "DEV_RX_BLK", "DEV_RX_DRP", "CCCI_TX_DRP", "CCCI_RX_DRP", "Unhandled");
    for (i = 0; i < CCCI_PORT_NUM; i++) {
        pos += snprintf(buf + pos, PAGE_SIZE, "%6s %12d %12d %12d %12d %12d %12d %12d\n",
            port_name[i],
            atomic_read(&g_except_inst.ptx_blk_cnt[i]),
            atomic_read(&g_except_inst.ptx_drp_cnt[i]),
            atomic_read(&g_except_inst.prx_blk_cnt[i]),
            atomic_read(&g_except_inst.prx_drp_cnt[i]),
            atomic_read(&g_except_inst.ccci_tx_drp_cnt[i]),
            atomic_read(&g_except_inst.ccci_rx_drp_cnt[i]),
            atomic_read(&g_except_inst.port[i].pkt_cnt));
    }
    DEBUG_LOG_FUNCTION_ENTRY;
    return pos;
}

/*
                                                                              
         
           
          
           
 */
void eemcs_expt_reset_statistics(void)
{
    KAL_UINT32 i = 0;

    DEBUG_LOG_FUNCTION_ENTRY;
    for (i = 0; i < CCCI_PORT_NUM; i++) {
        atomic_set(&g_except_inst.ptx_blk_cnt[i], 0);
        atomic_set(&g_except_inst.prx_blk_cnt[i], 0);
        atomic_set(&g_except_inst.ptx_drp_cnt[i], 0);
        atomic_set(&g_except_inst.prx_drp_cnt[i], 0);
        atomic_set(&g_except_inst.ccci_tx_drp_cnt[i], 0);
        atomic_set(&g_except_inst.ccci_rx_drp_cnt[i], 0);
    }
    DEBUG_LOG_FUNCTION_LEAVE;
}

#define EEMCS_EXCEPTION_LOG_TO_FILE  1
#define EEMCS_EXCEPTION_LOG_PATH    "/system/etc/firmware"

/*
                                                                        
         
           
          
                                                
 */
KAL_INT32 eemcs_expt_flush()
{
    KAL_UINT32 pkts = 0;
    KAL_UINT32 i = 0;
//                               

	/*                                        */
	for (i = 0; i < CCCI_PORT_NUM; i++) {
		pkts = atomic_read(&g_except_inst.port[i].pkt_cnt);

		/*                 */
		if (pkts == 0)
			continue;

		DBGLOG(EXPT, DBG, "free %d skb in port%d expt list", pkts, i);
		skb_queue_purge(&g_except_inst.port[i].skb_list);
		atomic_set(&g_except_inst.port[i].pkt_cnt, 0);
	}

	/*                                      */
	for (i = 0; i < SDIO_RX_Q_NUM; i++) {
		pkts = atomic_read(&g_except_inst.rxq[i].pkt_cnt);

		/*                 */
		if (pkts == 0)
			continue;

		DBGLOG(EXPT, DBG, "free %d skb in rxq%d expt list", pkts, i);
		skb_queue_purge(&g_except_inst.rxq[i].skb_list);
		atomic_set(&g_except_inst.rxq[i].pkt_cnt, 0);
	}

	/*                                      */
	for (i = 0; i < SDIO_TX_Q_NUM; i++) {
		pkts = atomic_read(&g_except_inst.txq[i].pkt_cnt);

		/*                 */
		if (pkts == 0)
			continue;

		DBGLOG(EXPT, DBG, "free %d skb in txq%d expt list", pkts, i);
		skb_queue_purge(&g_except_inst.txq[i].skb_list);
		atomic_set(&g_except_inst.txq[i].pkt_cnt, 0);
	}

#if 0    
    char log_file[NAME_MAX] = {0};
    struct file *fp = NULL;
    KAL_UINT32 pkts = 0;
    KAL_UINT32 i = 0, j = 0;
    struct sk_buff *skb = NULL;

    DEBUG_LOG_FUNCTION_ENTRY;
    /*                                */
    for (i = 0; i < SDIO_RX_Q_NUM; i++) {
        pkts = atomic_read(&g_except_inst.rxq[i].pkt_cnt);
        DBGLOG(EXPT, DBG, "[EXPT] %d packets in DL SWQ %d", pkts, i);
        /*                 */
        if (pkts == 0)
            continue;

        sprintf(log_file, "%s/eemcs_expt_rx-%02d_%d.bak", EEMCS_EXCEPTION_LOG_PATH, g_except_inst.rxq[i].id, pkts);
        fp = file_open(log_file, O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (fp == NULL) {
            DBGLOG(EXPT, ERR, "[EXPT] Failed to open file %s", log_file);
            continue;
        }
        //                     
        file_write(fp, (char*)&pkts, sizeof(KAL_UINT32));
        /*                        */
        for (j = 0; j < pkts; j++) {
            skb = skb_dequeue(&g_except_inst.rxq[i].skb_list);
            if (skb == NULL) {
                DBGLOG(EXPT, WAR, "[EXPT] Failed to read skb from RX list %d", i);
            } else {
                hif_dl_pkt_handle_complete(i);
                //                      
                file_write(fp, (char*)&skb->len, sizeof(unsigned int));
                //               
                file_write(fp, skb->data, skb->len);
                atomic_dec(&g_except_inst.rxq[i].pkt_cnt);
            }
        }
        file_close(fp);
        DBGLOG(EXPT, TRA, "[EXPT] All unhandled DL packets in Q are saved to %s", log_file);
    }
    /*                                */
    for (i = 0; i < SDIO_TX_Q_NUM; i++) {
        pkts = atomic_read(&g_except_inst.txq[i].pkt_cnt);
        DBGLOG(EXPT, DBG, "[EXPT] %d packets in UL SWQ %d", pkts, i);
        /*                 */
        if (pkts == 0)
            continue;

        sprintf(log_file, "%s/eemcs_expt_tx-%02d_%d.bak", EEMCS_EXCEPTION_LOG_PATH, g_except_inst.txq[i].id, pkts);
        fp = file_open(log_file, O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (fp == NULL) {
            DBGLOG(EXPT, ERR, "[EXPT] Failed to open file %s", log_file);
            continue;
        }
        //                     
        file_write(fp, (char*)&pkts, sizeof(KAL_UINT32));
        /*                        */
        for (j = 0; j < pkts; j++) {
            skb = skb_dequeue(&g_except_inst.txq[i].skb_list);
            if (skb == NULL) {
                DBGLOG(EXPT, WAR, "[EXPT] Failed to read skb from TX list %d", i);
            } else {
                //                      
                file_write(fp, (char*)&skb->len, sizeof(unsigned int));
                //               
                file_write(fp, skb->data, skb->len);
                atomic_dec(&g_except_inst.txq[i].pkt_cnt);
            }
        }
        file_close(fp);
        DBGLOG(EXPT, TRA, "[EXPT] All unhandled UL packets in Q are saved to %s", log_file);
    }
    /*                                  */
    for (i = 0; i < CCCI_CDEV_NUM; i++) {
        pkts = atomic_read(&g_except_inst.port[i].pkt_cnt);
        DBGLOG(EXPT, DBG, "[EXPT] %d packets in port %d", pkts, i);
        /*                 */
        if (pkts == 0)
            continue;

        sprintf(log_file, "%s/eemcs_expt_port-%02d_%d.bak", EEMCS_EXCEPTION_LOG_PATH, i, pkts);
        fp = file_open(log_file, O_RDWR | O_CREAT, 0777);
        if (fp == NULL) {
            DBGLOG(EXPT, ERR, "[EXPT] Failed to open file %s", log_file);
            continue;
        }
        //                     
        file_write(fp, (char*)&pkts, sizeof(KAL_UINT32));
        /*                        */
        for (j = 0; j < pkts; j++) {
            skb = skb_dequeue(&g_except_inst.port[i].skb_list);
            if (skb == NULL) {
                DBGLOG(EXPT, WAR, "[EXPT] Failed to read skb from port list %d", i);
            } else {
                //                      
                file_write(fp, (char*)&skb->len, sizeof(unsigned int));
                //               
                file_write(fp, skb->data, skb->len);
                atomic_dec(&g_except_inst.port[i].pkt_cnt);
            }
        }
        file_close(fp);
        DBGLOG(EXPT, TRA, "[EXPT] All unhandled UL packets in port are saved to %s", log_file);
    }
    DBGLOG(EXPT, TRA, "[EXPT] eemcs_expt_flush() Finished !!");

    DEBUG_LOG_FUNCTION_LEAVE;
#else
    DEBUG_LOG_FUNCTION_ENTRY;
    DEBUG_LOG_FUNCTION_LEAVE;
#endif
    return KAL_SUCCESS;
}


/*
                                                          
         
                                  
          
           
 */
void ccci_df_to_ccci_exception_callback(KAL_UINT32 msg_id)
{
    DEBUG_LOG_FUNCTION_ENTRY;
    DBGLOG(EXPT, TRA, "CCCI Exception Callback (%d)", msg_id);
    switch (msg_id) {
        case EX_INIT:
        {
            //                          
            eemcs_expt_reset_statistics();
            
            //                                  
            change_device_state(EEMCS_EXCEPTION);

            //                                                  
            mod_timer(&g_except_inst.md_ex_monitor, jiffies+EE_INIT_TIMER);

            //                                                                                                   
            set_exception_mode(EEMCS_EX_INIT);

            //                                    
            eemcs_exec_expt_callback(EEMCS_EX_INIT);

            //                      
            eemcs_cdev_msg(CCCI_PORT_CTRL, CCCI_MD_MSG_EXCEPTION, 0);

            //                 
            eemcs_ccci_expt_log_df_q_pkts();

            break;
        }
        case EX_DHL_DL_RDY:
            //                                                                      
            set_exception_mode(EEMCS_EX_DHL_DL_RDY);

            //                                       
            eemcs_exec_expt_callback(EEMCS_EX_DHL_DL_RDY);
            break;
        case EX_INIT_DONE:
            //                                                                           
            set_exception_mode(EEMCS_EX_INIT_DONE);

            //                                     
            mod_timer(&g_except_inst.md_ex_monitor, jiffies+EE_HS1_TIMER);

            //                                   
            eemcs_expt_flush();
            
            //                                       
            eemcs_exec_expt_callback(EEMCS_EX_INIT_DONE);
            break;
        default:
            DBGLOG(EXPT, ERR, "Unknown exception callback id %d", msg_id);
    }
	
    DEBUG_LOG_FUNCTION_LEAVE;
    return;
}

void eemcs_expt_reset(void)
{
	memset(g_except_inst.expt_info_mem, 0, MD_EX_MEM_SIZE);
	
	return;
}

/*
                                              
         
           
          
                                                
 */
KAL_INT32 eemcs_expt_mod_init(void)
{
    KAL_UINT32 i = 0;
    //                                          
    //                                                                            
    KAL_INT32  ret;
    ccci_port_cfg *log_queue_config;
    DEBUG_LOG_FUNCTION_ENTRY;

    //                      
    g_except_inst.md_ex_type = 0;

    /*                              */
    spin_lock_init(&g_except_inst.expt_cb_lock);
    for(i = 0; i < CCCI_PORT_NUM_MAX; i++) {
        g_except_inst.expt_cb[i] = NULL;

        if(TR_Q_INVALID != ccci_expt_port_info[i].expt_txq_id)
        {
            set_bit(SDIO_TXQ(ccci_expt_port_info[i].expt_txq_id), (unsigned long *)&except_txq);
        }
        if(TR_Q_INVALID != ccci_expt_port_info[i].expt_rxq_id)
        {
            set_bit(SDIO_RXQ(ccci_expt_port_info[i].expt_rxq_id), (unsigned long *)&except_rxq);
        }
    }

    eemcs_expt_ut_init();

    log_queue_config = ccci_get_port_info(CCCI_PORT_MD_LOG);
    set_bit(SDIO_RXQ(log_queue_config->rxq_id), (unsigned long *)&nonstop_rxq);
    hif_except_init(nonstop_rxq, (except_txq << 16) | except_rxq);

    ret = hif_reg_expt_cb(ccci_df_to_ccci_exception_callback);
    KAL_ASSERT(ret == KAL_SUCCESS);
    
    DBGLOG(EXPT, TRA, "nonstop_txq=%d, nonstop_rxq=%d, exp_txq=%d, exp_rxq=%d", \
		0, nonstop_rxq, except_txq, except_rxq);


    /*                */
    for (i = 0; i < SDIO_TX_Q_NUM; i++) {
        g_except_inst.txq[i].id = -1;
        atomic_set(&g_except_inst.txq[i].pkt_cnt, 0);
        skb_queue_head_init(&g_except_inst.txq[i].skb_list);
    }
    /*                */
    for (i = 0; i < SDIO_RX_Q_NUM; i++) {
        g_except_inst.rxq[i].id = -1;
        atomic_set(&g_except_inst.rxq[i].pkt_cnt, 0);
        skb_queue_head_init(&g_except_inst.rxq[i].skb_list);
    }
    /*                */
    for (i = 0; i < CCCI_PORT_NUM; i++) {
        atomic_set(&g_except_inst.port[i].pkt_cnt, 0);
        skb_queue_head_init(&g_except_inst.port[i].skb_list);
    }
    /*                       */
    eemcs_expt_reset_statistics();

    /*                     */
    eemcs_exception_state = EEMCS_EX_NONE;

    /*                           */
    init_timer(&g_except_inst.md_ex_monitor);
    g_except_inst.md_ex_monitor.function = ex_monitor_func;
    g_except_inst.md_ex_monitor.data = (unsigned long)&g_except_inst;

    /*                                      */
    g_except_inst.expt_info_mem = (KAL_UINT8 *)kmalloc(MD_EX_MEM_SIZE, GFP_KERNEL);
    if (g_except_inst.expt_info_mem) {
        DBGLOG(EXPT, ERR, "alloc exception info memory ok: %p", g_except_inst.expt_info_mem);
        memset(g_except_inst.expt_info_mem, 0, MD_EX_MEM_SIZE);
    } else {
        DBGLOG(EXPT, ERR, "alloc exception info memory fail");
    }
    
#ifdef ENABLE_MD_WDT_PROCESS
    eemcs_ccci_register_WDT_callback(eemcs_wdt_reset_callback);
#endif

    DEBUG_LOG_FUNCTION_LEAVE;

    return KAL_SUCCESS;
}

/*
                                                 
         
           
          
           
 */
void eemcs_expt_exit(void)
{
    KAL_UINT32 i = 0;
    DEBUG_LOG_FUNCTION_ENTRY;

    if (g_except_inst.expt_info_mem) {
        kfree(g_except_inst.expt_info_mem);
        g_except_inst.expt_info_mem = NULL;
    }

    del_timer(&g_except_inst.md_ex_monitor);
	
    /*                */
    for (i = 0; i < SDIO_TX_Q_NUM; i++)
        skb_queue_purge(&g_except_inst.txq[i].skb_list);
    for (i = 0; i < SDIO_RX_Q_NUM; i++)
        skb_queue_purge(&g_except_inst.rxq[i].skb_list);
    for (i = 0; i < CCCI_PORT_NUM; i++)
        skb_queue_purge(&g_except_inst.port[i].skb_list);

    DEBUG_LOG_FUNCTION_LEAVE;
}
