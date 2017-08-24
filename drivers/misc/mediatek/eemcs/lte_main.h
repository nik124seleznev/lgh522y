#ifndef __LTE_MAIN_H__
#define __LTE_MAIN_H__

#define MT_LTE_SDIO_KBUILD_MODNAME	"mtlte_sdio"

/*                */
#define MT_LTE_RXQ0_MAX_PKT_REPORT_NUM	(16)

#define MT_LTE_RXQ1_MAX_PKT_REPORT_NUM	(16)

#define MT_LTE_RXQ2_MAX_PKT_REPORT_NUM	(16)

#define MT_LTE_RXQ3_MAX_PKT_REPORT_NUM	(16)


//                                                                                   
//                                                   


int mtlte_sys_sdio_driver_init(void);
void mtlte_sys_sdio_driver_exit(void);

void mtlte_sys_sdio_driver_init_after_phase2(void);

#ifdef CONFIG_MTK_SDIOAUTOK_SUPPORT
//                                   
#define MT_LTE_AUTO_CALIBRATION
#define NATIVE_AUTOK

//                                  
#endif  //                      

#ifdef MT_LTE_AUTO_CALIBRATION
void mtlte_sys_sdio_wait_probe_done(void);
#endif

#endif
