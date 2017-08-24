/*
                                                                                                      
*/

/*                      
          


*/

/*
          
  
                      
                                                             
                                                  
                                                                      
  
                        
       
                           
                                                      
  
                      
       
                                          
  
                      
       
                          
                                                              

  
                      
       
                           
  
                           
       
                              
  
                        
  
                                           
  
                        
  
                                
                                                                                                                           
  
  
*/

#ifndef _HIF_SDIO_H
#define _HIF_SDIO_H
/*                                                                              
                                                     
                                                                                
*/
#define HIF_SDIO_DEBUG  (0)	/*                                                                  */

/*                                                                              
                                                          
                                                                                
*/
#include "mtk_porting.h"

/*                                                                              
                                                
                                                                                
*/
#define CFG_CLIENT_COUNT  (8)

#define HIF_DEFAULT_BLK_SIZE  (256)
#define HIF_DEFAULT_VENDOR    (0x037A)

/*                                                                              
                                                 
                                                                                
*/

/*                                         */
typedef struct _MTK_WCN_HIF_SDIO_FUNCINFO MTK_WCN_HIF_SDIO_FUNCINFO;

/*                                                                            */
typedef ULONG MTK_WCN_HIF_SDIO_CLTCTX;

/*                                              */
typedef INT_32(*MTK_WCN_HIF_SDIO_PROBE) (MTK_WCN_HIF_SDIO_CLTCTX, const MTK_WCN_HIF_SDIO_FUNCINFO*);
typedef INT_32(*MTK_WCN_HIF_SDIO_REMOVE) (MTK_WCN_HIF_SDIO_CLTCTX);
typedef INT_32(*MTK_WCN_HIF_SDIO_IRQ) (MTK_WCN_HIF_SDIO_CLTCTX);

/*                                         */
struct _MTK_WCN_HIF_SDIO_FUNCINFO {
	UINT_16 manf_id;		/*                              */
	UINT_16 card_id;		/*                      */
	UINT_16 func_num;	/*                 */
	UINT_16 blk_sz;		/*                     */
};

/*                                       */
typedef struct _MTK_WCN_HIF_SDIO_CLTINFO {
	const MTK_WCN_HIF_SDIO_FUNCINFO *func_tbl;	/*                               */
	UINT_32 func_tbl_size;	/*                                              */
	MTK_WCN_HIF_SDIO_PROBE hif_clt_probe;	/*                               */
	MTK_WCN_HIF_SDIO_REMOVE hif_clt_remove;	/*                                */
	MTK_WCN_HIF_SDIO_IRQ hif_clt_irq;	/*                                          */
} MTK_WCN_HIF_SDIO_CLTINFO;

/*                                             */
typedef struct _MTK_WCN_HIF_SDIO_REGISTINFO {
	const MTK_WCN_HIF_SDIO_CLTINFO *sdio_cltinfo;	/*                                           */
	const MTK_WCN_HIF_SDIO_FUNCINFO *func_info;	/*                                 */
} MTK_WCN_HIF_SDIO_REGISTINFO;

/*                                       */
typedef struct _MTK_WCN_HIF_SDIO_PROBEINFO {
	struct sdio_func *func;	/*                              */
	INT_8 clt_idx;		/*                                                                     */
	MTK_WCN_BOOL interrupted;	/*                                           */
	void *private_data_p;	/*                            */
} MTK_WCN_HIF_SDIO_PROBEINFO;

/*                                  */
typedef struct _MTK_WCN_HIF_SDIO_CLT_PROBE_WORKERINFO {
	struct work_struct probe_work;	/*                      */
	MTK_WCN_HIF_SDIO_REGISTINFO *registinfo_p;	/*                                                   */
	INT_8 probe_idx;		/*                                                                 */
} MTK_WCN_HIF_SDIO_CLT_PROBE_WORKERINFO;

/*                                                              */
typedef enum {
	HIF_SDIO_ERR_SUCCESS = 0,
	HIF_SDIO_ERR_FAIL,	/*               */
	HIF_SDIO_ERR_INVALID_PARAM,
	HIF_SDIO_ERR_DUPLICATED,
	HIF_SDIO_ERR_UNSUP_MANF_ID,
	HIF_SDIO_ERR_UNSUP_CARD_ID,
	HIF_SDIO_ERR_INVALID_FUNC_NUM,
	HIF_SDIO_ERR_INVALID_BLK_SZ,
} MTK_WCN_HIF_SDIO_ERR;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/* 
                                                   
  
                                                                                 
  
                                              
                                      
                                              
                                                
 */
#define MTK_WCN_HIF_SDIO_FUNC(manf, card, func, b_sz) \
	.manf_id = (manf), .card_id = (card), .func_num = (func), .blk_sz = (b_sz)

/* 
                                                                                           
  
 */
#if HIF_SDIO_DEBUG
#define DPRINTK(fmt, args...) pr_debug("%s: " fmt, __func__, ## args)
#else
#define DPRINTK(fmt, args...)
#endif

/* 
                                     
  
 */
#if HIF_SDIO_DEBUG
#define ASSERT(expr)    if (!(expr)) { \
			    printk("assertion failed! %s[%d]: %s\n",\
				__func__, __LINE__, #expr); \
			    BUG_ON(!(expr));\
			}
#else
#define ASSERT(expr)    do {} while (0)
#endif

/*                                                                              
                                                             
                                                                                
*/

/* 
                                                   
  
                                                        
  
                                                     
  
                                     
                          
 */
extern INT_32 mtk_wcn_hif_sdio_client_reg(const MTK_WCN_HIF_SDIO_CLTINFO *pinfo);

extern INT_32 mtk_wcn_hif_sdio_client_unreg(const MTK_WCN_HIF_SDIO_CLTINFO *pinfo);

extern INT_32 mtk_wcn_hif_sdio_readb(MTK_WCN_HIF_SDIO_CLTCTX ctx, UINT_32 offset, PUINT_8 pvb);

extern INT_32 mtk_wcn_hif_sdio_writeb(MTK_WCN_HIF_SDIO_CLTCTX ctx, UINT_32 offset, UINT_8 vb);

extern INT_32 mtk_wcn_hif_sdio_readl(MTK_WCN_HIF_SDIO_CLTCTX ctx, UINT_32 offset, PUINT_32 pvl);

extern INT_32 mtk_wcn_hif_sdio_writel(MTK_WCN_HIF_SDIO_CLTCTX ctx, UINT_32 offset, UINT_32 vl);

extern INT_32 mtk_wcn_hif_sdio_read_buf(MTK_WCN_HIF_SDIO_CLTCTX ctx,
				       UINT_32 offset, PUINT_32 pbuf, UINT_32 len);

extern INT_32 mtk_wcn_hif_sdio_write_buf(MTK_WCN_HIF_SDIO_CLTCTX ctx,
					UINT_32 offset, PUINT_32 pbuf, UINT_32 len);

extern void mtk_wcn_hif_sdio_set_drvdata(MTK_WCN_HIF_SDIO_CLTCTX ctx, void *private_data_p);

extern void *mtk_wcn_hif_sdio_get_drvdata(MTK_WCN_HIF_SDIO_CLTCTX ctx);

extern void mtk_wcn_hif_sdio_get_dev(MTK_WCN_HIF_SDIO_CLTCTX ctx, struct device **dev);

extern void mtk_wcn_hif_sdio_enable_irq(MTK_WCN_HIF_SDIO_CLTCTX ctx, MTK_WCN_BOOL enable);

/*                                                                              
                                                
                                                                                
*/
#endif				/*             */
