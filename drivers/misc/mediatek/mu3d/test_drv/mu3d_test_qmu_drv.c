
#include <linux/mu3d/hal/mu3d_hal_osal.h>
#include <linux/mu3d/hal/mu3d_hal_qmu_drv.h>
#define _QMU_DRV_EXT_
#include <linux/mu3d/test_drv/mu3d_test_qmu_drv.h>
#undef _QMU_DRV_EXT_
#include <linux/mu3d/test_drv/mu3d_test_usb_drv.h>
#include <linux/mu3d/hal/mu3d_hal_hw.h>
#include <linux/mu3d/test_drv/mu3d_test_unified.h>

//      
#include <mach/battery_common.h>

#define TX_FIFO_NUM	 5     /*               */
#define RX_FIFO_NUM 5     /*               */
DEV_UINT32 gpd_num;
DEV_UINT8* qmu_loopback_buffer;
dma_addr_t stress_dma_buffer=0;
dma_addr_t stress_dma_buffer_end=0;
#ifdef _STRESS_ADD_BYPASS
static DEV_UINT8 _fgbypassGPD = 0;
#endif

DEV_UINT8 Rx_gpd_IOC[15];
spinlock_t	lock;


void qmu_proc(DEV_UINT32 wQmuVal);
void set_gpd_hwo(USB_DIR dir,DEV_INT32 Q_num);
void resume_gpd_hwo(USB_DIR dir,DEV_INT32 Q_num);

void dev_insert_stress_gpd(USB_DIR dir,DEV_INT32 ep_num,DEV_UINT8 IOC)
{
	struct USB_REQ *req;
	DEV_UINT32 maxp;
	DEV_UINT8 zlp;

	req = mu3d_hal_get_req(ep_num, dir);
	zlp = (USB_ReadCsr32(U3D_TX1CSR1, ep_num) & TYPE_ISO) ? false : true;
	maxp = USB_ReadCsr32(U3D_RX1CSR0, ep_num) & RX_RXMAXPKTSZ;

#ifdef _STRESS_ADD_BYPASS
	_fgbypassGPD++;
	if(!(_fgbypassGPD%5)){
		mu3d_hal_insert_transfer_gpd(ep_num,dir, req->dma_adr, req->count, true, IOC, true, zlp, maxp);
	}
	else
#endif
	{
		mu3d_hal_insert_transfer_gpd(ep_num,dir, req->dma_adr, req->count, true, IOC, false, zlp, maxp);

		if(req->dma_adr>= stress_dma_buffer_end) {
			req->dma_adr = stress_dma_buffer;
		} else {
			req->dma_adr += TransferLength;
		}
	}
}


/* 
                                                                     
  
 */
void dev_resume_stress_gpd_hwo(USB_DIR dir,DEV_INT32 ep_num){

	resume_gpd_hwo(dir,ep_num);
	mu3d_hal_resume_qmu(ep_num, dir);
}

/* 
                                                                        
  
 */
void dev_insert_stress_gpd_hwo(USB_DIR dir,DEV_INT32 ep_num){

	set_gpd_hwo(dir,ep_num);
	mu3d_hal_resume_qmu(ep_num, dir);
}


void dev_start_stress(USB_DIR dir,DEV_INT32 ep_num){
	mu3d_hal_resume_qmu(ep_num, dir);
}


/* 
                                                       
                                                                          
 */
void dev_prepare_gpd_short(DEV_INT32 num,USB_DIR dir,DEV_INT32 ep_num,DEV_UINT8* buf){
	struct USB_REQ *req;
	DEV_UINT32 i, maxp;
	dma_addr_t mapping;
	DEV_UINT8 IOC, zlp;

	os_printk(K_DEBUG,"dev_prepare_gpd\n");
	os_printk(K_DEBUG,"buf[%d]=%p\n", ep_num, buf);
	os_printk(K_DEBUG,"dir=%x\n", dir);

	req = mu3d_hal_get_req(ep_num, dir);
	req->buf = buf;
	req->count=TransferLength;
	os_memset(req->buf, 0xFF , g_dma_buffer_size);
	mapping = dma_map_single(NULL, req->buf,g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_sync_single_for_device(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	req->dma_adr=mapping;
	stress_dma_buffer =req->dma_adr;
	stress_dma_buffer_end = stress_dma_buffer+(MAX_GPD_NUM-1)*TransferLength;
	zlp = (USB_ReadCsr32(U3D_TX1CSR1, ep_num) & TYPE_ISO) ? false : true;
	maxp = USB_ReadCsr32(U3D_RX1CSR0, ep_num) & RX_RXMAXPKTSZ;


	if(dir==USB_TX){

		mu3d_hal_insert_transfer_gpd(ep_num,dir, req->dma_adr, 256, true, false, false, zlp, maxp);

		req->dma_adr += TransferLength;
		for(i=1;i<num;i++){
			IOC = ((i%STRESS_IOC_TH)==(STRESS_IOC_TH/2)) ? true : false;
			mu3d_hal_insert_transfer_gpd(ep_num,dir, req->dma_adr, req->count, true, IOC, false, zlp, maxp);

			if(req->dma_adr>=stress_dma_buffer_end){
				req->dma_adr=stress_dma_buffer;
			}
			else{
				req->dma_adr += TransferLength;
			}
		}
	}
	else{
		for(i=0;i<num;i++){
			IOC = ((i%STRESS_IOC_TH)==(STRESS_IOC_TH/2)) ? true : false;
			mu3d_hal_insert_transfer_gpd(ep_num,dir, req->dma_adr, req->count, true, IOC, false, zlp, maxp);

			if(req->dma_adr>=stress_dma_buffer_end){
				req->dma_adr=stress_dma_buffer;
			}
			else{
			 	req->dma_adr += TransferLength;
			}
		}
	}
	dma_sync_single_for_cpu(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_unmap_single(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);

	if(dir==USB_TX)
		os_printk(K_DEBUG,"TX[%d] GPD Start : 0x%08X\n", ep_num, os_readl(USB_QMU_TQSAR(ep_num)));
	else
		os_printk(K_DEBUG,"RX[%d] GPD Start : 0x%08X\n", ep_num, os_readl(USB_QMU_RQSAR(ep_num)));

}


/* 
                                                 
                                                                          
 */
void dev_prepare_gpd(DEV_INT32 num,USB_DIR dir,DEV_INT32 ep_num,DEV_UINT8* buf){
	struct USB_REQ *req;
	DEV_UINT32 i, maxp;
	dma_addr_t mapping;
	DEV_UINT8 IOC, zlp;

	os_printk(K_DEBUG,"dev_prepare_gpd\n");
	os_printk(K_DEBUG,"buf[%d]=%p\n",ep_num,buf);
	os_printk(K_DEBUG,"dir=%x\n",dir);

	req = mu3d_hal_get_req(ep_num, dir);
	req->buf = buf;
	req->count=TransferLength;
	os_memset(req->buf, 0xFF , g_dma_buffer_size);
	mapping = dma_map_single(NULL, req->buf,g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_sync_single_for_device(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	req->dma_adr=mapping;
	stress_dma_buffer = req->dma_adr;
	stress_dma_buffer_end = stress_dma_buffer+(MAX_GPD_NUM-1)*TransferLength;
	zlp = (USB_ReadCsr32(U3D_TX1CSR1, ep_num) & TYPE_ISO) ? false : true;
	maxp = USB_ReadCsr32(U3D_RX1CSR0, ep_num) & RX_RXMAXPKTSZ;

	for(i=0;i<num;i++){
		IOC = ((i%STRESS_IOC_TH)==(STRESS_IOC_TH/2)) ? true : false;
		mu3d_hal_insert_transfer_gpd(ep_num,dir, req->dma_adr, req->count, true, IOC, false, zlp, maxp);

		if(req->dma_adr>=stress_dma_buffer_end){
			req->dma_adr=stress_dma_buffer;
		}
		else{
			 req->dma_adr += TransferLength;
		}
	}
	dma_sync_single_for_cpu(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_unmap_single(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);

	if(dir==USB_TX)
		os_printk(K_DEBUG,"TX[%d] GPD Start : 0x%08X\n", ep_num, os_readl(USB_QMU_TQSAR(ep_num)));
	else
		os_printk(K_DEBUG,"RX[%d] GPD Start : 0x%08X\n", ep_num, os_readl(USB_QMU_RQSAR(ep_num)));

}


/* 
                                                             
                                                                          
 */
void dev_prepare_stress_gpd(DEV_INT32 num,USB_DIR dir,DEV_INT32 ep_num,DEV_UINT8* buf){
	struct USB_REQ *req;
	DEV_UINT32 i;
	dma_addr_t mapping;
	DEV_UINT8 IOC=false;

	os_printk(K_ERR,"dev_prepare_gpd\n");
	os_printk(K_ERR,"buf[%d]=%p\n",ep_num,buf);
	os_printk(K_ERR,"dir=%x\n",dir);

	req = mu3d_hal_get_req(ep_num, dir);
	req->buf = buf;
	req->count=TransferLength;

	mapping = dma_map_single(NULL, req->buf,g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_sync_single_for_device(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	req->dma_adr=mapping;

	os_ms_delay(100);
	stress_dma_buffer =req->dma_adr;
	stress_dma_buffer_end = stress_dma_buffer+(num-1)*TransferLength;

	for(i=0;i<num;i++){

		if(dir==USB_RX){
			IOC = (i==(num-1)) ? true : false;
		}
		insert_stress_gpd(ep_num,dir, req->dma_adr, req->count, false, IOC);

		if(req->dma_adr>=stress_dma_buffer_end){
			req->dma_adr=stress_dma_buffer;
		}
		else{
			 req->dma_adr += TransferLength;
		}
	}
	Rx_gpd_IOC[ep_num] =0;
	if(dir==USB_TX)
		os_printk(K_DEBUG,"TX[%d] GPD Start : 0x%08X\n", ep_num, os_readl(USB_QMU_TQSAR(ep_num)));
	else
		os_printk(K_DEBUG,"RX[%d] GPD Start : 0x%08X\n", ep_num, os_readl(USB_QMU_RQSAR(ep_num)));

}


/* 
                                            
                                                 
 */
void dev_qmu_loopback_ext(DEV_INT32 ep_rx,DEV_INT32 ep_tx){

 	struct USB_REQ *treq, *rreq;
	dma_addr_t mapping;
	DEV_UINT32 i, j, gpd_num,bps_num, maxp;
    DEV_UINT8* dma_buf, zlp;

	os_printk(K_INFO,"ep_rx :%d\r\n",ep_rx);
	os_printk(K_INFO,"ep_tx :%d\r\n",ep_tx);
	rreq = mu3d_hal_get_req(ep_rx, USB_RX);
	treq = mu3d_hal_get_req(ep_tx, USB_TX);

	dma_buf = g_loopback_buffer[0];
#ifdef BOUNDARY_4K
	qmu_loopback_buffer = g_loopback_buffer[0]+(0x1000-(DEV_INT32)(unsigned long)g_loopback_buffer[0]%0x1000)-0x20+bDramOffset;
	treq->buf =rreq->buf =qmu_loopback_buffer;
#else
	treq->buf =rreq->buf =g_loopback_buffer[0];
#endif

	os_printk(K_INFO,"rreq->buf=%p\n",rreq->buf);
	os_printk(K_INFO,"treq->buf=%p\n",treq->buf);

	rreq->count=TransferLength;//      
	rreq->transferCount=0;
	rreq->complete=0;
	treq->complete=0;
	os_memset(dma_buf, 0 , g_dma_buffer_size);
	mapping = dma_map_single(NULL, dma_buf,g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_sync_single_for_device(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	treq->dma_adr=rreq->dma_adr=mapping;
	gpd_num = (TransferLength/gpd_buf_size);
	if(TransferLength%gpd_buf_size){
		gpd_num++;
	}

	os_printk(K_INFO,"gpd_num :%x\r\n",gpd_num);
 	zlp = (USB_ReadCsr32(U3D_TX1CSR1, ep_tx) & TYPE_ISO) ? false : true;
 	maxp = USB_ReadCsr32(U3D_RX1CSR0, ep_rx) & RX_RXMAXPKTSZ;

	for(i=0 ;i<gpd_num ; i++){

		mu3d_hal_insert_transfer_gpd(ep_rx,USB_RX, (rreq->dma_adr+i*gpd_buf_size), gpd_buf_size, true, true, false, zlp, maxp);
		os_get_random_bytes(&bps_num,1);
		bps_num %= 3;
		for(j=0 ;j<bps_num ;j++){
			mu3d_hal_insert_transfer_gpd(ep_rx,USB_RX, rreq->dma_adr, gpd_buf_size, true,true, true, zlp, maxp);
		}
	}

	mu3d_hal_resume_qmu(ep_rx, USB_RX);
	os_printk(K_INFO,"rx start\r\n");
 	while(!req_complete(ep_rx, USB_RX));
	os_printk(K_INFO,"rx complete...\r\n");
	treq->count=TransferLength;
	gpd_num = (TransferLength/gpd_buf_size);

	if(TransferLength%gpd_buf_size){
		gpd_num++;
	}

	for(i=0 ;i<gpd_num ; i++){

		if(treq->count>gpd_buf_size){
			mu3d_hal_insert_transfer_gpd(ep_tx,USB_TX, (treq->dma_adr+i*gpd_buf_size), gpd_buf_size, true, true, false, zlp, maxp);
		}
		else{
			mu3d_hal_insert_transfer_gpd(ep_tx,USB_TX, (treq->dma_adr+i*gpd_buf_size),treq->count , true, true, false, zlp, maxp);
		}

		os_get_random_bytes(&bps_num,1);
		bps_num %= 3;

		for(j=0 ;j<bps_num ;j++){
			mu3d_hal_insert_transfer_gpd(ep_tx,USB_TX, treq->dma_adr, gpd_buf_size, true,true, true, zlp, maxp);
		}
		treq->count-= gpd_buf_size;
	}

	#if !ISO_UPDATE_TEST
	mu3d_hal_resume_qmu(ep_tx, USB_TX);
	os_printk(K_INFO,"tx start...length : %d\r\n",TransferLength);
	while(!req_complete(ep_tx, USB_TX));
	os_printk(K_INFO,"tx complete...\r\n");
	#endif

	mapping=rreq->dma_adr;
	dma_sync_single_for_cpu(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_unmap_single(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);

}

/* 
                                                     
                                                                                                          
 */
void dev_notification(DEV_INT8 type,DEV_INT32 valuel,DEV_INT32 valueh){
	DEV_INT32 temp;

	temp = ((type<<4)&DEV_NOTIF_TYPE)|((valuel<<8)&DEV_NOTIF_TYPE_SPECIFIC_LOW);
	os_writel(U3D_DEV_NOTIF_0, temp);
	temp = ((valuel>>24)&0xFF)|((valueh<<8)&0xFFFFFF00);
	os_writel(U3D_DEV_NOTIF_1, temp);
	os_writel(U3D_DEV_NOTIF_0, os_readl(U3D_DEV_NOTIF_0)|SEND_DEV_NOTIF);
	while(os_readl(U3D_DEV_NOTIF_0)&SEND_DEV_NOTIF);
}

/*
                           
                              
 */
void dev_otg(DEV_UINT8 mode)
{
	printk("1: iddig_a\n");
	printk("2: iddig_a\n");
	printk("3: srp_a\n");
	printk("4: srp_b\n");
	printk("5: hnp_a\n");
	printk("6: hnp_b\n");
}


/* 
                           
                             
 */
void dev_qmu_rx(DEV_INT32 ep_rx){
	struct USB_REQ *req;
	dma_addr_t mapping;
	DEV_UINT32 i, gpd_num, QCR, maxp;
	DEV_UINT8 zlp;

	req = mu3d_hal_get_req(ep_rx, USB_RX);
	req->buf =g_loopback_buffer[0];
	req->count=TransferLength;;
	req->transferCount=0;
	req->complete=0;

	if(cfg_rx_zlp_en)
	{
		QCR = os_readl(U3D_QCR3);
		os_writel(U3D_QCR3, QCR|QMU_RX_ZLP(ep_rx));
	}
	else
	{
		QCR = os_readl(U3D_QCR3);
		os_writel(U3D_QCR3, QCR&~(QMU_RX_ZLP(ep_rx)));
	}

	if(cfg_rx_coz_en)
	{
		QCR = os_readl(U3D_QCR3);
		os_writel(U3D_QCR3, QCR|QMU_RX_COZ(ep_rx));
	}
	else
	{
		QCR = os_readl(U3D_QCR3);
		os_writel(U3D_QCR3, QCR&~(QMU_RX_COZ(ep_rx)));
	}
	os_memset(req->buf, 0 , 1000000);
	mapping = dma_map_single(NULL, req->buf,g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_sync_single_for_device(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	req->dma_adr=mapping;
	gpd_num = (TransferLength/gpd_buf_size);
	if(cfg_rx_coz_en){
		gpd_num*=2;
	}
	if(TransferLength%gpd_buf_size){
		gpd_num++;
	}
	zlp =  true;
	maxp = USB_ReadCsr32(U3D_RX1CSR0, ep_rx) & RX_RXMAXPKTSZ;
	for(i=0 ;i<gpd_num ; i++){
		mu3d_hal_insert_transfer_gpd(ep_rx,USB_RX, (req->dma_adr+i*gpd_buf_size), gpd_buf_size, true, true, false, zlp, maxp);
	}
	mu3d_hal_resume_qmu(ep_rx, USB_RX);
	os_ms_delay(500);
	rx_done_count=(os_readl(USB_QMU_RQCPR(ep_rx))-os_readl(USB_QMU_RQSAR(ep_rx)))/0x10;
	mapping=req->dma_adr;
	dma_sync_single_for_cpu(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_unmap_single(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);

}

/* 
                                
          
 */
DEV_UINT8 dev_ep_reset(void){

	DEV_UINT32 i, tx_ep_num, rx_ep_num, csr0, csr1, csr2;
	DEV_UINT8 ret;

#ifdef HARDCODE_EP
 	tx_ep_num = MAX_QMU_EP;//                                         
	rx_ep_num = MAX_QMU_EP;//                                                
#else
 	tx_ep_num = os_readl(U3D_CAP_EPINFO) & CAP_TX_EP_NUM;
	rx_ep_num = (os_readl(U3D_CAP_EPINFO) & CAP_RX_EP_NUM) >> 8;
#endif
	ret = 0;

	for(i=1; i<=tx_ep_num; i++){
	   	USB_WriteCsr32(U3D_TX1CSR0, i, USB_ReadCsr32(U3D_TX1CSR0, i) | TX_EP_RESET);
		USB_WriteCsr32(U3D_TX1CSR0, i, USB_ReadCsr32(U3D_TX1CSR0, i) &~ TX_EP_RESET);
		csr0 = USB_ReadCsr32(U3D_TX1CSR0, i);
		csr1 = USB_ReadCsr32(U3D_TX1CSR1, i);
		csr2 = USB_ReadCsr32(U3D_TX1CSR2, i);
		USB_WriteCsr32(U3D_TX1CSR0, i, 0x7FFFFFFF);
		USB_WriteCsr32(U3D_TX1CSR1, i, 0xFFFFFFFF);
		USB_WriteCsr32(U3D_TX1CSR2, i, 0xFFFFFFFF);

		if(csr0 == USB_ReadCsr32(U3D_TX1CSR0, i)){
			ret = ERROR;
		}

	   	if(csr1 == USB_ReadCsr32(U3D_TX1CSR1, i)){
			ret = ERROR;
		}
	   	if(csr2 == USB_ReadCsr32(U3D_TX1CSR2, i)){
			ret = ERROR;
		}
		USB_WriteCsr32(U3D_TX1CSR0, i, USB_ReadCsr32(U3D_TX1CSR0, i) | TX_EP_RESET);
		USB_WriteCsr32(U3D_TX1CSR0, i, USB_ReadCsr32(U3D_TX1CSR0, i) &~ TX_EP_RESET);
		if(csr0 != USB_ReadCsr32(U3D_TX1CSR0, i)){
			ret = ERROR;
		}

		if(csr1 != USB_ReadCsr32(U3D_TX1CSR1, i)){
			ret = ERROR;
		}
		if(csr2 != USB_ReadCsr32(U3D_TX1CSR2, i)){
			ret = ERROR;
		}
	}
 	for(i=1; i<=rx_ep_num; i++){
		USB_WriteCsr32(U3D_RX1CSR0, i, USB_ReadCsr32(U3D_RX1CSR0, i) | RX_EP_RESET);
		USB_WriteCsr32(U3D_RX1CSR0, i, USB_ReadCsr32(U3D_RX1CSR0, i) &~ RX_EP_RESET);
		csr0 = USB_ReadCsr32(U3D_RX1CSR0, i);
		csr1 = USB_ReadCsr32(U3D_RX1CSR1, i);
		csr2 = USB_ReadCsr32(U3D_RX1CSR2, i);
		USB_WriteCsr32(U3D_RX1CSR0, i, 0x7FFFFFFF);
		USB_WriteCsr32(U3D_RX1CSR1, i, 0xFFFFFFFF);
		USB_WriteCsr32(U3D_RX1CSR2, i, 0xFFFFFFFF);
		if(csr0 == USB_ReadCsr32(U3D_RX1CSR0, i)){
			ret = ERROR;
		}
	   	if(csr1 == USB_ReadCsr32(U3D_RX1CSR1, i)){
			ret = ERROR;
		}
	   	if(csr2 == USB_ReadCsr32(U3D_RX1CSR2, i)){
			ret = ERROR;
		}
		USB_WriteCsr32(U3D_RX1CSR0, i, USB_ReadCsr32(U3D_RX1CSR0, i) | RX_EP_RESET);
		USB_WriteCsr32(U3D_RX1CSR0, i, USB_ReadCsr32(U3D_RX1CSR0, i) &~ RX_EP_RESET);


		if(csr0 != USB_ReadCsr32(U3D_RX1CSR0, i)){
			ret = ERROR;
		}

		if(csr1 != USB_ReadCsr32(U3D_RX1CSR1, i)){
			ret = ERROR;
		}
		if(csr2 != USB_ReadCsr32(U3D_RX1CSR2, i)){
			ret = ERROR;
		}
	}
	return ret;
}

/* 
                          
                                                 
 */
void dev_tx_rx(DEV_INT32 ep_rx,DEV_INT32 ep_tx){

 	struct USB_REQ *treq, *rreq;
	dma_addr_t mapping;
	DEV_UINT32 i, gpd_num, maxp;
    DEV_UINT8* dma_buf, zlp;

	os_printk(K_INFO,"ep_rx :%d\r\n",ep_rx);
	os_printk(K_INFO,"ep_tx :%d\r\n",ep_tx);

	rreq = mu3d_hal_get_req(ep_rx, USB_RX);
	treq = mu3d_hal_get_req(ep_tx, USB_TX);
	dma_buf = g_loopback_buffer[0];
#ifdef BOUNDARY_4K
	qmu_loopback_buffer = g_loopback_buffer[0]+(0x1000-(DEV_INT32)(unsigned long)g_loopback_buffer[0]%0x1000)-0x20+bDramOffset;
	treq->buf =rreq->buf =qmu_loopback_buffer;
#else
	treq->buf =rreq->buf =g_loopback_buffer[0];
#endif

	os_printk(K_INFO,"rreq->buf=%p\n",rreq->buf);
	os_printk(K_INFO,"treq->buf=%p\n",treq->buf);

	rreq->count=TransferLength;
	rreq->transferCount=0;
	rreq->complete=0;
	treq->complete=0;
	os_memset(rreq->buf, 0 , 1000000);
	mapping = dma_map_single(NULL, dma_buf,g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_sync_single_for_device(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	treq->dma_adr=rreq->dma_adr=mapping;

	gpd_num = (TransferLength/gpd_buf_size);
	if(TransferLength%gpd_buf_size){
		gpd_num++;
	}

	zlp = (USB_ReadCsr32(U3D_TX1CSR1, ep_tx) & TYPE_ISO) ? false : true;
	maxp = USB_ReadCsr32(U3D_RX1CSR0, ep_rx) & RX_RXMAXPKTSZ;

	for(i=0 ;i<gpd_num ; i++){
		mu3d_hal_insert_transfer_gpd(ep_rx,USB_RX, (rreq->dma_adr+i*gpd_buf_size), gpd_buf_size, true, true, false, zlp, maxp);
	}

	mu3d_hal_resume_qmu(ep_rx, USB_RX);
	while(!req_complete(ep_rx, USB_RX));

	treq->count=TransferLength;
	gpd_num = (TransferLength/gpd_buf_size);

	if(TransferLength%gpd_buf_size){
		gpd_num++;
	}

	for(i=0 ;i<gpd_num ; i++){

		os_printk(K_INFO,"treq->count : %d\r\n",treq->count);
		if(treq->count>gpd_buf_size){
			mu3d_hal_insert_transfer_gpd(ep_tx,USB_TX, (treq->dma_adr+i*gpd_buf_size), gpd_buf_size, true, true, false, zlp, maxp);
		}
		else{
			mu3d_hal_insert_transfer_gpd(ep_tx,USB_TX, (treq->dma_adr+i*gpd_buf_size),treq->count , true, true, false, zlp, maxp);
		}
		treq->count-= gpd_buf_size;
	}

	mu3d_hal_resume_qmu(ep_tx, USB_TX);
	while(!req_complete(ep_tx, USB_TX));

	mapping=rreq->dma_adr;
	dma_sync_single_for_cpu(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_unmap_single(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
}

/* 
                                       
                                             
 */
void dev_set_dma_busrt_limiter(DEV_INT8 busrt,DEV_INT8 limiter){
	DEV_UINT32 temp;

	temp = ((busrt<<DMA_BURST_OFST)&DMA_BURST) | ((limiter<<DMALIMITER_OFST)&DMALIMITER);

	/*                                                                                                        */
	os_writel(U3D_TXDMARLCOUNT, os_readl(U3D_TXDMARLCOUNT) | temp);

	/*                                                                                                        */
	os_writel(U3D_RXDMARLCOUNT, os_readl(U3D_TXDMARLCOUNT) | temp);
}

/* 
                                       
                                                 
 */
void dev_qmu_loopback(DEV_INT32 ep_rx,DEV_INT32 ep_tx){
 	struct USB_REQ *treq, *rreq;
	dma_addr_t mapping;
	DEV_UINT32 i, gpd_num, maxp;
    DEV_UINT8* dma_buf,zlp;

	os_printk(K_INFO,"ep_rx :%d\r\n",ep_rx);
	os_printk(K_INFO,"ep_tx :%d\r\n",ep_tx);

	rreq = mu3d_hal_get_req(ep_rx, USB_RX);
	treq = mu3d_hal_get_req(ep_tx, USB_TX);
	dma_buf = g_loopback_buffer[0];
#ifdef BOUNDARY_4K
	qmu_loopback_buffer = g_loopback_buffer[0]+(0x1000-(DEV_INT32)(unsigned long)g_loopback_buffer[0]%0x1000)-0x20+bDramOffset;
	treq->buf =rreq->buf =qmu_loopback_buffer;
#else
	treq->buf =rreq->buf =g_loopback_buffer[0];
#endif

	os_printk(K_INFO,"rreq->buf=%p\n",rreq->buf);
	os_printk(K_INFO,"treq->buf=%p\n",treq->buf);

	rreq->count=TransferLength;
	rreq->transferCount=0;
	rreq->complete=0;
	treq->complete=0;
	os_memset(rreq->buf, 0 , 1000000);
	mapping = dma_map_single(NULL, dma_buf,g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_sync_single_for_device(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	treq->dma_adr=rreq->dma_adr=mapping;

	gpd_num = (TransferLength/gpd_buf_size);
	if(TransferLength%gpd_buf_size){
		gpd_num++;
	}

	zlp = (USB_ReadCsr32(U3D_TX1CSR1, ep_tx) & TYPE_ISO) ? false : true;
	maxp = USB_ReadCsr32(U3D_RX1CSR0, ep_rx) & RX_RXMAXPKTSZ;

	for( i=0; i<gpd_num; i++) {
		mu3d_hal_insert_transfer_gpd(ep_rx, USB_RX, (rreq->dma_adr+i*gpd_buf_size), gpd_buf_size, true, true, false, zlp, maxp);
	}

	#if LPM_STRESS
	os_ms_delay(1000);
	#endif
	mu3d_hal_resume_qmu(ep_rx, USB_RX);
	os_printk(K_INFO,"rx start\r\n");
 	while(!req_complete(ep_rx, USB_RX));
	os_printk(K_INFO,"rx complete...\r\n");
	treq->count=TransferLength;
	gpd_num = (TransferLength/gpd_buf_size);

	if(TransferLength%gpd_buf_size){
		gpd_num++;
	}

	for(i=0 ;i<gpd_num ; i++){

		os_printk(K_INFO,"treq->count : %d\r\n",treq->count);
		if(treq->count>gpd_buf_size){
			mu3d_hal_insert_transfer_gpd(ep_tx,USB_TX, (treq->dma_adr+i*gpd_buf_size), gpd_buf_size, true, true, false, zlp, maxp);
		}
		else{
			mu3d_hal_insert_transfer_gpd(ep_tx,USB_TX, (treq->dma_adr+i*gpd_buf_size),treq->count , true, true, false, zlp, maxp);
		}
		treq->count-= gpd_buf_size;
	}

	#if LPM_STRESS
	os_ms_delay(1000);
	#endif

	#if !ISO_UPDATE_TEST
	mu3d_hal_resume_qmu(ep_tx, USB_TX);
	os_printk(K_INFO,"tx start...length : %d\r\n",TransferLength);
	while(!req_complete(ep_tx, USB_TX));
	os_printk(K_INFO,"tx complete...\r\n");
	#endif

	mapping=rreq->dma_adr;
	dma_sync_single_for_cpu(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	dma_unmap_single(NULL, mapping, g_dma_buffer_size, DMA_BIDIRECTIONAL);

}

/* 
                                                       
                                                                                                                                                                          
 */
TGPD* prepare_tx_stress_gpd(TGPD* gpd, dma_addr_t pBuf, DEV_UINT32 data_length, DEV_UINT8 ep_num, DEV_UINT8 _is_bdp, DEV_UINT8 isHWO,DEV_UINT8 IOC){
	DEV_UINT32  offset;
	DEV_INT32 i,bd_num;

    TBD * bd_next;
	TBD * bd_head, *bd;
	DEV_UINT32 length;
	DEV_UINT8* pBuffer;
    DEV_UINT8 *_tmp;

  	os_printk(K_INFO,"mu3d_hal_prepare_tx_gpd\r\n");

	if(data_length<= bGPD_Extension){
		_is_bdp=0;
	}

    if(!_is_bdp){

        TGPD_SET_DATA(gpd, (unsigned long)(pBuf+bGPD_Extension));
        TGPD_CLR_FORMAT_BDP(gpd);
    }
    else{

		bd_head=(TBD*)get_bd(USB_TX,ep_num);
		os_printk(K_INFO,"Malloc Tx 01 (BD) : 0x%p\r\n", bd_head);
		bd=bd_head;
		os_memset(bd, 0, sizeof(TBD)+bBD_Extension);
        length=data_length-bGPD_Extension;
        pBuffer= (DEV_UINT8*)(unsigned long)(pBuf+bGPD_Extension);
		offset=bd_buf_size+bBD_Extension;
		bd_num = (!(length%offset)) ? (length/offset) : ((length/offset)+1);

		os_printk(K_INFO,"bd_num : 0x%x\r\n", (DEV_UINT32)bd_num);

		if(offset>length){
			offset=length;
		}

		for(i=0; i<bd_num; i++){
			if(i==(bd_num-1)){
				if(length<bBD_Extension){
					TBD_SET_EXT_LEN(bd, length);
					TBD_SET_BUF_LEN(bd, 0);
					TBD_SET_DATA(bd, (unsigned long)(pBuffer+bBD_Extension));
				}
				else{
					TBD_SET_EXT_LEN(bd, bBD_Extension);
					TBD_SET_BUF_LEN(bd, length-bBD_Extension);
					TBD_SET_DATA(bd, (unsigned long)(pBuffer+bBD_Extension));
				}

				TBD_SET_FLAGS_EOL(bd);
				TBD_SET_NEXT(bd, 0);
				TBD_SET_CHKSUM(bd, CHECKSUM_LENGTH);
				if(bBD_Extension){
					dma_sync_single_for_cpu(NULL, (dma_addr_t)pBuf, g_dma_buffer_size, DMA_BIDIRECTIONAL);
					_tmp=TBD_GET_EXT(bd);
					os_memcpy(_tmp, os_phys_to_virt(pBuffer), bBD_Extension);
					dma_sync_single_for_device(NULL, (dma_addr_t)pBuf, g_dma_buffer_size, DMA_BIDIRECTIONAL);
				}
				os_printk(K_INFO,"BD number %d\r\n", i+1);
				data_length=length+bGPD_Extension;
				length = 0;

				break;
			}else{

				TBD_SET_EXT_LEN(bd, bBD_Extension);
				TBD_SET_BUF_LEN(bd, offset-bBD_Extension);
				TBD_SET_DATA(bd, (unsigned long)(pBuffer+bBD_Extension));
				TBD_CLR_FLAGS_EOL(bd);
                bd_next = (TBD*)get_bd(USB_TX,ep_num);
                os_memset(bd_next, 0, sizeof(TBD)+bBD_Extension);
				TBD_SET_NEXT(bd, (unsigned long)bd_virt_to_phys(bd_next,USB_TX,ep_num));
				TBD_SET_CHKSUM(bd, CHECKSUM_LENGTH);

				if(bBD_Extension){
					dma_sync_single_for_cpu(NULL, (dma_addr_t)pBuf, g_dma_buffer_size, DMA_BIDIRECTIONAL);
					_tmp=TBD_GET_EXT(bd);
					os_memcpy(_tmp, os_phys_to_virt(pBuffer), bBD_Extension);
					dma_sync_single_for_device(NULL, (dma_addr_t)pBuf, g_dma_buffer_size, DMA_BIDIRECTIONAL);
				}
				length -= offset;
				pBuffer += offset;
				bd=bd_next;
			}
		}

		TGPD_SET_DATA(gpd, (unsigned long)bd_virt_to_phys(bd_head,USB_TX,ep_num));
		TGPD_SET_FORMAT_BDP(gpd);
	}

	os_printk(K_INFO,"GPD data_length %d \r\n", (data_length-bGPD_Extension));

	if(data_length<bGPD_Extension){
		TGPD_SET_BUF_LEN(gpd, 0);
		TGPD_SET_EXT_LEN(gpd, data_length);
	}
	else{
		TGPD_SET_BUF_LEN(gpd, data_length-bGPD_Extension);
		TGPD_SET_EXT_LEN(gpd, bGPD_Extension);
	}

	if(bGPD_Extension){

		dma_sync_single_for_cpu(NULL, pBuf, g_dma_buffer_size, DMA_BIDIRECTIONAL);
		_tmp=TGPD_GET_EXT(gpd);
		os_memcpy(_tmp, (DEV_UINT8 *)os_phys_to_virt((void *)(unsigned long)pBuf), bGPD_Extension);
		dma_sync_single_for_device(NULL, pBuf, g_dma_buffer_size, DMA_BIDIRECTIONAL);
	}
	if(!(USB_ReadCsr32(U3D_TX1CSR1, ep_num) & TYPE_ISO)){
		TGPD_SET_FORMAT_ZLP(gpd);
	}

	if(IOC){

		TGPD_SET_FORMAT_IOC(gpd);
    }
	else{

	  	TGPD_CLR_FORMAT_IOC(gpd);
	}

    //               
    Tx_gpd_end[ep_num]=get_gpd(USB_TX ,ep_num);
	os_printk(K_INFO,"Malloc Tx 01 (GPD+EXT) (Tx_gpd_end) : 0x%p\r\n", Tx_gpd_end[ep_num]);
	TGPD_SET_NEXT(gpd, (unsigned long)mu3d_hal_gpd_virt_to_phys(Tx_gpd_end[ep_num],USB_TX,ep_num));

	if(isHWO){
		TGPD_SET_CHKSUM(gpd, CHECKSUM_LENGTH);

		TGPD_SET_FLAGS_HWO(gpd);

	}else{
		TGPD_CLR_FLAGS_HWO(gpd);
        TGPD_SET_CHKSUM_HWO(gpd, CHECKSUM_LENGTH);
	}

    #if defined(USB_RISC_CACHE_ENABLED)
    os_flushinvalidateDcache();
    #endif

	return gpd;
}

/* 
                                                            
                                     
 */
void resume_gpd_hwo(USB_DIR dir,DEV_INT32 Q_num){
	TGPD* gpd_current;
	DEV_INT32 i;

	if(dir == USB_RX){
			gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_RQSAR(Q_num)));
		}
		else{
			gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_TQSAR(Q_num)));
		}

		gpd_current = gpd_phys_to_virt(gpd_current,dir,Q_num);

		if(dir == USB_RX){
			gpd_current = (TGPD*)gpd_current+STRESS_IOC_TH*Rx_gpd_IOC[Q_num]*sizeof(TGPD);
		}
		if(dir == USB_TX){
			gpd_current = (TGPD*)gpd_current+STRESS_IOC_TH*Rx_gpd_IOC[Q_num]*sizeof(TGPD);
			gpd_current = (TGPD*)gpd_current+STRESS_IOC_TH*Rx_gpd_IOC[Q_num]*AT_GPD_EXT_LEN;
		}

	for(i=0;i<(STRESS_IOC_TH);i++){
		if(dir == USB_RX){
			TGPD_SET_BUF_LEN(gpd_current, 0);
		}
		TGPD_SET_FLAGS_HWO(gpd_current);
		gpd_current++;
		if(dir == USB_TX){
			gpd_current = (TGPD*)gpd_current+AT_GPD_EXT_LEN;
		}
	}

	if(dir == USB_RX){

		printk("Rx_gpd_IOC[%d] :%d\n", Q_num,Rx_gpd_IOC[Q_num]);
		if(Rx_gpd_IOC[Q_num]>=STRESS_IOC_TH){
			Rx_gpd_IOC[Q_num]=0;
		}
		else{
			Rx_gpd_IOC[Q_num]++;
		}
	}

}

/* 
                                          
                                     
 */
void set_gpd_hwo(USB_DIR dir,DEV_INT32 Q_num){
	TGPD *gpd_current=NULL; 
	TGPD *gpd_current_tx=NULL;
	DEV_INT32 i;
	int cur_length;

	if(dir == USB_RX){
		gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_RQSAR(Q_num)));
		gpd_current_tx = (TGPD*)(unsigned long)(os_readl(USB_QMU_TQSAR(Q_num)));
		gpd_current_tx = gpd_phys_to_virt(gpd_current_tx,USB_TX,Q_num);
	}
	else{
		gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_TQSAR(Q_num)));
	}
	gpd_current = gpd_phys_to_virt(gpd_current,dir,Q_num);
	for(i=0;i<(MAX_GPD_NUM);i++){
		if(dir == USB_RX){
			//                        
			cur_length = TGPD_GET_BUF_LEN(gpd_current);
			TGPD_SET_BUF_LEN(gpd_current_tx, cur_length);
			TGPD_SET_BUF_LEN(gpd_current, 0);
			gpd_current_tx++;
			gpd_current_tx = (TGPD *)gpd_current_tx+AT_GPD_EXT_LEN;

		}
		if(dir == USB_RX){
			TGPD_SET_FLAGS_HWO(gpd_current);
		}
		else{
			TGPD_SET_CHKSUM_HWO(gpd_current,CHECKSUM_LENGTH);
			TGPD_SET_FLAGS_HWO(gpd_current);
		}
		gpd_current++;
		if(dir == USB_TX){
			gpd_current = (TGPD *)gpd_current+AT_GPD_EXT_LEN;
		}
	}

}

/* 
                                                       
                                                                                                                                                                          
 */
TGPD* prepare_rx_stress_gpd(TGPD*gpd, dma_addr_t pBuf, DEV_UINT32 data_len, DEV_UINT8 ep_num, DEV_UINT8 _is_bdp, DEV_UINT8 isHWO, DEV_UINT8 IOC){
	DEV_UINT32  offset;
	DEV_INT32 i,bd_num;

	TBD * bd_next;
	TBD * bd_head, *bd;
	DEV_UINT8* pBuffer;

	os_printk(K_DEBUG,"GPD 0x%p\r\n", gpd);

	if(!_is_bdp){

		TGPD_SET_DATA(gpd, (unsigned long)pBuf);
		TGPD_CLR_FORMAT_BDP(gpd);
	}
	else{

		bd_head=(TBD*)get_bd(USB_RX,ep_num);
		os_printk(K_DEBUG,"Malloc Rx 01 (BD) : 0x%p\r\n", bd_head);
		bd=bd_head;
		os_memset(bd, 0, sizeof(TBD));
		offset=bd_buf_size;
		pBuffer= (DEV_UINT8*)(unsigned long)(pBuf);
		bd_num = (!(data_len%offset)) ? (data_len/offset) : ((data_len/offset)+1);

		for(i=0; i<bd_num; i++){

			TBD_SET_BUF_LEN(bd, 0);
			TBD_SET_DATA(bd, (unsigned long)pBuffer);
			if(i==(bd_num-1)){
				TBD_SET_DataBUF_LEN(bd, data_len);
				TBD_SET_FLAGS_EOL(bd);
				TBD_SET_NEXT(bd, 0);
				TBD_SET_CHKSUM(bd, CHECKSUM_LENGTH);
				os_printk(K_DEBUG,"BD number %d\r\n", i+1);
				break;
			}else{
				TBD_SET_DataBUF_LEN(bd, offset);
				TBD_CLR_FLAGS_EOL(bd);
				bd_next = (TBD*)get_bd(USB_RX,ep_num);
				os_memset(bd_next, 0, sizeof(TBD));
				TBD_SET_NEXT(bd, (unsigned long)bd_virt_to_phys(bd_next,USB_RX,ep_num));
				TBD_SET_CHKSUM(bd, CHECKSUM_LENGTH);
				pBuffer += offset;
				data_len-=offset;
				bd=bd_next;

			}
		}

		TGPD_SET_DATA(gpd, (unsigned long)bd_virt_to_phys(bd_head,USB_RX,ep_num));
		TGPD_SET_FORMAT_BDP(gpd);
	}

	os_printk(K_DEBUG,"data_len 0x%x\r\n", data_len);
	TGPD_SET_DataBUF_LEN(gpd, gpd_buf_size);
	TGPD_SET_BUF_LEN(gpd, 0);
	if(IOC)
	{
		TGPD_SET_FORMAT_IOC(gpd);
	}
	else{
	  	TGPD_CLR_FORMAT_IOC(gpd);
	}

	Rx_gpd_end[ep_num]=get_gpd(USB_RX ,ep_num);
	os_printk(K_DEBUG,"Rx Next GPD 0x%p\r\n",Rx_gpd_end[ep_num]);
	TGPD_SET_NEXT(gpd, (unsigned long)mu3d_hal_gpd_virt_to_phys(Rx_gpd_end[ep_num],USB_RX,ep_num));

	if(isHWO){
		TGPD_SET_CHKSUM(gpd, CHECKSUM_LENGTH);
		TGPD_SET_FLAGS_HWO(gpd);

	}else{
		TGPD_CLR_FLAGS_HWO(gpd);
		TGPD_SET_CHKSUM_HWO(gpd, CHECKSUM_LENGTH);
	}
	os_printk(K_DEBUG,"Rx gpd info { HWO %d, Next_GPD %p ,DataBufferLength %d, DataBuffer %p, Recived Len %d, Endpoint %d, TGL %d, ZLP %d}\r\n",
					(DEV_UINT32)TGPD_GET_FLAG(gpd), TGPD_GET_NEXT(gpd),
					(DEV_UINT32)TGPD_GET_DataBUF_LEN(gpd), TGPD_GET_DATA(gpd),
					(DEV_UINT32)TGPD_GET_BUF_LEN(gpd), (DEV_UINT32)TGPD_GET_EPaddr(gpd),
					(DEV_UINT32)TGPD_GET_TGL(gpd), (DEV_UINT32)TGPD_GET_ZLP(gpd));

	return gpd;
}

/* 
                                           
                                                                                                                                     
 */
void insert_stress_gpd(DEV_INT32 ep_num,USB_DIR dir, dma_addr_t buf, DEV_UINT32 count, DEV_UINT8 isHWO, DEV_UINT8 IOC){

 	TGPD* gpd;
	os_printk(K_INFO,"mu3d_hal_insert_transfer_gpd\n");
	os_printk(K_INFO,"ep_num=%d\n",ep_num);
	os_printk(K_INFO,"dir=%d\n",dir);
	os_printk(K_INFO,"buf=%p\n",(void *)(unsigned long)buf);
	os_printk(K_INFO,"count=%x\n",count);

 	if(dir == USB_TX){
		TGPD* gpd = Tx_gpd_end[ep_num];
		os_printk(K_INFO,"TX gpd=%p\n",gpd);
		prepare_tx_stress_gpd(gpd, buf, count, ep_num, is_bdp, isHWO, IOC);
	}
 	else if(dir == USB_RX){
		gpd = Rx_gpd_end[ep_num];
		os_printk(K_INFO,"RX gpd=%p\n",gpd);
	 	prepare_rx_stress_gpd(gpd, buf, count, ep_num, is_bdp, isHWO, IOC);
	}

}

/* 
                                                                         
                                                              
 */
void mu3d_hal_restart_qmu_no_flush(DEV_INT32 Q_num, USB_DIR dir, DEV_INT8 method){


    TGPD* gpd_current;
	unsigned long flags;

	if(dir == USB_TX){
		spin_lock_irqsave(&lock, flags);
		mu3d_hal_stop_qmu(Q_num, USB_TX);
		while((os_readl(USB_QMU_TQCSR(Q_num)) & (QMU_Q_ACTIVE)));
		if(method==1){//                                              
			while((USB_ReadCsr32(U3D_TX1CSR0, Q_num) & TX_FIFOFULL));
			USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) &~ TX_DMAREQEN);
			USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) | TX_TXPKTRDY);
			USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) | TX_DMAREQEN);
			while((USB_ReadCsr32(U3D_TX1CSR0, Q_num) & TX_FIFOFULL));
			USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) &~ TX_DMAREQEN);
			USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) | TX_TXPKTRDY);
			USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) | TX_DMAREQEN);
			os_printk(K_DEBUG,"method 01\r\n");
		}
		if(method==2){
			if(!(USB_ReadCsr32(U3D_TX1CSR0, Q_num) & TX_FIFOFULL)){
				USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) &~ TX_DMAREQEN);
				USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) | TX_TXPKTRDY);
				USB_WriteCsr32(U3D_TX1CSR0, Q_num, USB_ReadCsr32(U3D_TX1CSR0, Q_num) | TX_DMAREQEN);
				printk("STOP TX=> TXPKTRDY\n");
			}
			os_printk(K_DEBUG,"method 02\r\n");
		}

		spin_unlock_irqrestore(&lock, flags);
		gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_TQCPR(Q_num)));
		if(!gpd_current){
			gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_TQSAR(Q_num)));
		}
		os_printk(K_DEBUG,"gpd_current(P) %p\n", gpd_current);
		gpd_current = gpd_phys_to_virt(gpd_current,USB_TX,Q_num);
		os_printk(K_DEBUG,"gpd_current(V) %p\n", gpd_current);
		Tx_gpd_end[Q_num] = Tx_gpd_last[Q_num] = gpd_current;
		gpd_ptr_align(dir,Q_num,Tx_gpd_end[Q_num]);
		os_memset(Tx_gpd_end[Q_num], 0 , sizeof(TGPD));
		//                    
		os_writel(USB_QMU_TQSAR(Q_num), mu3d_hal_gpd_virt_to_phys(Tx_gpd_last[Q_num],USB_TX,Q_num));
		mu3d_hal_start_qmu(Q_num, USB_TX);
	}else{

		spin_lock_irqsave(&lock, flags);
		mu3d_hal_stop_qmu(Q_num, USB_RX);
		while((os_readl(USB_QMU_RQCSR(Q_num)) & (QMU_Q_ACTIVE)));
		if(!(USB_ReadCsr32(U3D_RX1CSR0, Q_num) & RX_FIFOEMPTY)){

			os_printk(K_DEBUG, "fifo not empty\n");
			USB_WriteCsr32(U3D_RX1CSR0, Q_num, USB_ReadCsr32(U3D_RX1CSR0, Q_num) | RX_RXPKTRDY);
		}
		spin_unlock_irqrestore(&lock, flags);

		gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_RQCPR(Q_num)));
		if(!gpd_current){
			gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_RQSAR(Q_num)));
		}
		os_printk(K_DEBUG,"gpd_current(P) %p\n", gpd_current);
		gpd_current = gpd_phys_to_virt(gpd_current,USB_RX,Q_num);
		os_printk(K_DEBUG,"gpd_current(V) %p\n", gpd_current);
		Rx_gpd_end[Q_num] = Rx_gpd_last[Q_num] = gpd_current;
		gpd_ptr_align(dir,Q_num,Rx_gpd_end[Q_num]);
		//                    
		os_memset(Rx_gpd_end[Q_num], 0 , sizeof(TGPD));
		os_writel(USB_QMU_RQSAR(Q_num), mu3d_hal_gpd_virt_to_phys(Rx_gpd_end[Q_num],USB_RX,Q_num));
		mu3d_hal_start_qmu(Q_num, USB_RX);
	}

}


/* 
                                    
                          
 */
void proc_qmu_rx(DEV_INT32 ep_num){

	DEV_UINT32 bufferlength=0,recivedlength=0,i;
	DEV_UINT8 IOC;

    struct USB_REQ *req = mu3d_hal_get_req(ep_num, USB_RX);
    TGPD* gpd=(TGPD*)Rx_gpd_last[ep_num];
    TGPD* gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_RQCPR(ep_num)));
    TBD*	bd;

	if(g_run_stress){

		if(g_insert_hwo){
			dev_insert_stress_gpd_hwo(USB_RX, ep_num);
			dev_insert_stress_gpd_hwo(USB_TX, ep_num);
		}
		else{
			for(i=0;i<STRESS_IOC_TH;i++){
				IOC = ((i%STRESS_IOC_TH)==(STRESS_IOC_TH/2)) ? true : false;
				dev_insert_stress_gpd(USB_RX,ep_num,IOC);
			}
			mu3d_hal_resume_qmu(ep_num, USB_RX);
		}
	}
	else{

		gpd_current = gpd_phys_to_virt(gpd_current,USB_RX,ep_num);
   	 	os_printk(K_INFO,"ep_num : %d ,Rx_gpd_last : 0x%p, gpd_current : 0x%p, gpd_end : 0x%p \r\n",ep_num, gpd, gpd_current, Rx_gpd_end[ep_num]);

		if(gpd==gpd_current){
    		return;
    	}

		//                       
		dma_sync_single_for_cpu(NULL, mu3d_hal_gpd_virt_to_phys(gpd,USB_RX,ep_num), sizeof(TGPD), DMA_BIDIRECTIONAL);

   	 	while(gpd != gpd_current && !TGPD_IS_FLAGS_HWO(gpd)){

			if(TGPD_IS_FORMAT_BDP(gpd)){

				bd = (TBD *)TGPD_GET_DATA(gpd);

				//                             
				dma_sync_single_for_cpu(NULL, (dma_addr_t)(unsigned long)bd, sizeof(TBD), DMA_BIDIRECTIONAL);

				bd=bd_phys_to_virt(bd,USB_RX,ep_num);
				while(1){
					os_printk(K_INFO,"BD : 0x%p\r\n",bd);
					os_printk(K_INFO,"Buffer Len : 0x%x\r\n",(DEV_UINT32)TBD_GET_BUF_LEN(bd));
					req->transferCount += TBD_GET_BUF_LEN(bd);
					os_printk(K_INFO,"Total Len : 0x%x\r\n",req->transferCount);
					if(TBD_IS_FLAGS_EOL(bd)){
						break;
					}
					bd= TBD_GET_NEXT(bd);

					//                             
					dma_sync_single_for_cpu(NULL, (dma_addr_t)(unsigned long)bd, sizeof(TBD), DMA_BIDIRECTIONAL);

					bd=bd_phys_to_virt(bd,USB_RX,ep_num);
				}
			}
			else{
				recivedlength = (DEV_UINT32)TGPD_GET_BUF_LEN(gpd);
				bufferlength  = (DEV_UINT32)TGPD_GET_DataBUF_LEN(gpd);
				req->transferCount += recivedlength;
			}
			os_printk(K_INFO,"Rx gpd info { HWO %d, Next_GPD %p ,DataBufferLength %d, DataBuffer %p, Recived Len %d, Endpoint %d, TGL %d, ZLP %d}\r\n",
		        (DEV_UINT32)TGPD_GET_FLAG(gpd), TGPD_GET_NEXT(gpd),
		        (DEV_UINT32)TGPD_GET_DataBUF_LEN(gpd), TGPD_GET_DATA(gpd),
		        (DEV_UINT32)TGPD_GET_BUF_LEN(gpd), (DEV_UINT32)TGPD_GET_EPaddr(gpd),
		        (DEV_UINT32)TGPD_GET_TGL(gpd), (DEV_UINT32)TGPD_GET_ZLP(gpd));

			gpd=TGPD_GET_NEXT(gpd);

			//                               
			if(gpd) dma_sync_single_for_cpu(NULL, (dma_addr_t)(unsigned long)gpd, sizeof(TGPD), DMA_BIDIRECTIONAL);

			gpd=gpd_phys_to_virt(gpd,USB_RX,ep_num);
    	}

		Rx_gpd_last[ep_num]=gpd;
		os_printk(K_DEBUG,"Rx_gpd_last[%d] : 0x%p\r\n", ep_num, Rx_gpd_last[ep_num]);
		os_printk(K_DEBUG,"Rx_gpd_end[%d] : 0x%p\r\n", ep_num, Rx_gpd_end[ep_num]);
		rx_IOC_count++;

		if((req->transferCount==req->count)||(recivedlength<=bufferlength)){
			req->complete = true;
			os_printk(K_DEBUG,"Rx %d complete\r\n", ep_num);
		}
	}
}

/* 
                                    
                          
 */
void proc_qmu_tx(DEV_INT32 ep_num){
    struct USB_REQ *req = mu3d_hal_get_req(ep_num, USB_TX);
    TGPD* gpd=Tx_gpd_last[ep_num];
    TGPD* gpd_current = (TGPD*)(unsigned long)(os_readl(USB_QMU_TQCPR(ep_num)));
 	DEV_UINT32 i;
	DEV_UINT8 IOC;

	if(g_run_stress){

		if(!g_insert_hwo){
			for(i=0;i<STRESS_IOC_TH;i++){
				IOC = ((i%STRESS_IOC_TH)==(STRESS_IOC_TH/2)) ? true : false;
				dev_insert_stress_gpd(USB_TX,ep_num,IOC);
			}
			mu3d_hal_resume_qmu(ep_num, USB_TX);
		}
	}
	else{
		gpd_current = gpd_phys_to_virt(gpd_current,USB_TX,ep_num);
   		os_printk(K_DEBUG,"Tx_gpd_last 0x%p, gpd_current 0x%p, gpd_end 0x%p, \r\n", gpd, gpd_current, Tx_gpd_end[ep_num]);

		if(gpd==gpd_current){
			return;
    	}

		//                             
		dma_sync_single_for_cpu(NULL, mu3d_hal_gpd_virt_to_phys(gpd,USB_TX,ep_num), sizeof(TGPD), DMA_BIDIRECTIONAL);

    	while(gpd!=gpd_current && !TGPD_IS_FLAGS_HWO(gpd)){
		 os_printk(K_DEBUG,"Tx gpd %p info { HWO %d, BPD %d, Next_GPD %p , DataBuffer %p, BufferLen %d, Endpoint %d}\r\n",
		 	gpd, (DEV_UINT32)TGPD_GET_FLAG(gpd), (DEV_UINT32)TGPD_GET_FORMAT(gpd), TGPD_GET_NEXT(gpd),
		 	TGPD_GET_DATA(gpd), (DEV_UINT32)TGPD_GET_BUF_LEN(gpd), (DEV_UINT32)TGPD_GET_EPaddr(gpd));
			gpd=TGPD_GET_NEXT(gpd);
			gpd=gpd_phys_to_virt(gpd,USB_TX,ep_num);
   		 }

		Tx_gpd_last[ep_num]=gpd;
    	req->complete = true;
		os_printk(K_DEBUG,"Tx_gpd_last[%d] : 0x%p\r\n", ep_num, Tx_gpd_last[ep_num]);
		os_printk(K_DEBUG,"Tx_gpd_end[%d] : 0x%p\r\n", ep_num, Tx_gpd_end[ep_num]);
    	os_printk(K_DEBUG,"Tx %d complete\r\n", ep_num);
	}
}

/* 
                                        
                          
 */
void qmu_handler(DEV_UINT32 wQmuVal){
    DEV_INT32 i = 0;
    DEV_INT32 wErrVal = 0;

    os_printk(K_DEBUG, "qmu_handler %x\r\n", wQmuVal);

	if(wQmuVal & RXQ_CSERR_INT){
		os_printk(K_ALET,"Rx checksum error!\r\n");
		//         
	}
	if(wQmuVal & RXQ_LENERR_INT){
		os_printk(K_ALET,"Rx length error!\r\n");

		g_rx_len_err_cnt++;
		//         
	}
	if(wQmuVal & TXQ_CSERR_INT){
		os_printk(K_ALET,"Tx checksum error!\r\n");
		//         
	}
	if(wQmuVal & TXQ_LENERR_INT){
		os_printk(K_ALET,"Tx length error!\r\n");
		//         
	}


    if((wQmuVal & RXQ_CSERR_INT)||(wQmuVal & RXQ_LENERR_INT)){
     	wErrVal=os_readl(U3D_RQERRIR0);
     	os_printk(K_INFO,"U3D_RQERRIR0 : [0x%x]\r\n", wErrVal);
     	for(i=1; i<=MAX_QMU_EP; i++){
     		if(wErrVal &QMU_RX_CS_ERR(i)){
     			os_printk(K_ALET,"Rx %d length error!\r\n", i);
				//         
     		}
     		if(wErrVal &QMU_RX_LEN_ERR(i)){
     			os_printk(K_ALET,"Rx %d recieve length error!\r\n", i);
				//         
     		}
     	}
     	os_writel(U3D_RQERRIR0, wErrVal);
    }

    if(wQmuVal & RXQ_ZLPERR_INT){
		wErrVal=os_readl(U3D_RQERRIR1);
		os_printk(K_INFO,"U3D_RQERRIR1 : [0x%x]\r\n", wErrVal);

     	for(i=1; i<=MAX_QMU_EP; i++){
     		if(wErrVal &QMU_RX_ZLP_ERR(i)){
     			os_printk(K_INFO,"Rx %d recieve an zlp packet!\r\n", i);
     		}
     	}
     	os_writel(U3D_RQERRIR1, wErrVal);
    }

    if((wQmuVal & TXQ_CSERR_INT)||(wQmuVal & TXQ_LENERR_INT)){

 		wErrVal=os_readl(U3D_TQERRIR0);
 		os_printk(K_ALET,"Tx Queue error in QMU mode![0x%x]\r\n", wErrVal);

		for(i=1; i<=MAX_QMU_EP; i++){
 			if(wErrVal &QMU_TX_CS_ERR(i)){
 				os_printk(K_ALET,"Tx %d checksum error!\r\n", i);
				os_printk(K_ALET,"\r\n");
				//         
 			}

 			if(wErrVal &QMU_TX_LEN_ERR(i)){
 				os_printk(K_ALET,"Tx %d buffer length error!\r\n", i);
				os_printk(K_ALET,"\r\n");
				//         
 			}
 		}
 		os_writel(U3D_TQERRIR0, wErrVal);

    }

	if((wQmuVal & RXQ_EMPTY_INT)||(wQmuVal & TXQ_EMPTY_INT)){
 		DEV_UINT32 wEmptyVal=os_readl(U3D_QEMIR);
 		os_printk(K_INFO,"Rx Empty in QMU mode![0x%x]\r\n", wEmptyVal);
 		os_writel(U3D_QEMIR, wEmptyVal);
	}
}


void qmu_proc(DEV_UINT32 wQmuVal){
    DEV_INT32 i;
	os_printk(K_DEBUG,"qmu_proc\n");

    for(i=1; i<MAX_QMU_EP+1; i++){
    	if(wQmuVal & QMU_RX_DONE(i)){
			os_printk(K_DEBUG,"RX %d\n",i);
    		proc_qmu_rx(i);
    	}
    	if(wQmuVal & QMU_TX_DONE(i)){
			os_printk(K_DEBUG,"TX %d\n", i);
    		proc_qmu_tx(i);
    	}
    }
}

DEV_INT32 u3d_dev_suspend(void)
{
    return g_usb_status.suspend;
}


#ifdef EXT_VBUS_DET
irqreturn_t u3d_vbus_rise_handler(int irq, void *dev_id){
	os_printk(K_ERR, "u3d_vbus_rise_handler\n");
//                                
	reset_dev(U3D_DFT_SPEED, 0, 0);


	os_writel(FPGA_REG, (os_readl(FPGA_REG) &~ VBUS_MSK ) | VBUS_RISE_BIT);
	return IRQ_HANDLED;
}

irqreturn_t u3d_vbus_fall_handler(int irq, void *dev_id){
	DEV_UINT32 temp;

	os_printk(K_ERR, "u3d_vbus_fall_handler\n");

	os_disableIrq(USB_IRQ);
	g_usb_irq = 0;

	#ifdef POWER_SAVING_MODE
	os_printk(K_ERR, "power down u2/u3 ports, device module\n");
	#ifdef SUPPORT_U3
	os_setmsk(U3D_SSUSB_U3_CTRL_0P, (SSUSB_U3_PORT_DIS | SSUSB_U3_PORT_PDN));
	#endif
	os_setmsk(U3D_SSUSB_U2_CTRL_0P, (SSUSB_U2_PORT_DIS | SSUSB_U2_PORT_PDN));
	os_setmsk(U3D_SSUSB_IP_PW_CTRL2, SSUSB_IP_DEV_PDN);
	#endif

	//                                                  
 	u3d_rst_request();

	os_writel(FPGA_REG, (os_readl(FPGA_REG) &~ VBUS_MSK ) | VBUS_FALL_BIT);
	return IRQ_HANDLED;
}
#endif

irqreturn_t u3d_inter_handler(int irq, void *dev_id){

    DEV_UINT32 dwIntrUsbValue;
    DEV_UINT32 dwDmaIntrValue;
    DEV_UINT32 dwIntrEPValue;
    DEV_UINT16 wIntrTxValue;
    DEV_UINT16 wIntrRxValue;
    DEV_UINT32 wIntrQMUValue;
    DEV_UINT32 wIntrQMUDoneValue;
    DEV_UINT32 dwLtssmValue;
    DEV_UINT32 ep_num;
    DEV_UINT32 dwTemp;
    DEV_UINT32 dwLinkIntValue;
    DEV_UINT32 dwOtgIntValue;
	DEV_UINT32 timer;

	/*              */
	if(os_readl(U3D_LV1ISR) & MAC2_INTR) {
		dwIntrUsbValue = os_readl(U3D_COMMON_USB_INTR) & os_readl(U3D_COMMON_USB_INTR_ENABLE);
	} else {
		dwIntrUsbValue = 0;
	}

	#ifdef SUPPORT_U3
	if(os_readl(U3D_LV1ISR) & MAC3_INTR) {
		dwLtssmValue = os_readl(U3D_LTSSM_INTR) & os_readl(U3D_LTSSM_INTR_ENABLE);
	} else {
		dwLtssmValue = 0;
	}
	#else
	dwLtssmValue = 0;
	#endif

	dwDmaIntrValue = os_readl(U3D_DMAISR)& os_readl(U3D_DMAIER);
	dwIntrEPValue = os_readl(U3D_EPISR)& os_readl(U3D_EPIER);
	wIntrQMUValue = os_readl(U3D_QISAR1);
	wIntrQMUDoneValue = os_readl(U3D_QISAR0) & os_readl(U3D_QIER0);
	wIntrTxValue = dwIntrEPValue&0xFFFF;
	wIntrRxValue = (dwIntrEPValue>>16);
	dwLinkIntValue = os_readl(U3D_DEV_LINK_INTR) & os_readl(U3D_DEV_LINK_INTR_ENABLE);
	#ifdef SUPPORT_OTG
	dwOtgIntValue = os_readl(U3D_SSUSB_OTG_STS);
	#else
	dwOtgIntValue = 0;
	#endif

	os_printk(K_DEBUG,"wIntrQMUDoneValue :%x\n",wIntrQMUDoneValue);
	os_printk(K_DEBUG,"dwIntrEPValue :%x\n",dwIntrEPValue);

	os_printk(K_DEBUG,"Interrupt: IntrUsb [%x] IntrTx[%x] IntrRx [%x] IntrDMA[%x] IntrQMU [%x] IntrLTSSM [%x]\r\n", dwIntrUsbValue, wIntrTxValue, wIntrRxValue, dwDmaIntrValue, wIntrQMUValue, dwLtssmValue);
	if( !(dwIntrUsbValue || dwIntrEPValue || dwDmaIntrValue || wIntrQMUValue || dwLtssmValue)) {
		os_printk(K_DEBUG,"[NULL INTR] REG_INTRL1 = 0x%08X\n", (DEV_UINT32)os_readl(U3D_LV1ISR));
	}

	/*               */
	os_writel(U3D_QISAR0, wIntrQMUDoneValue);

	if(os_readl(U3D_LV1ISR) & MAC2_INTR){
   		os_writel(U3D_COMMON_USB_INTR, dwIntrUsbValue);
	}
	#ifdef SUPPORT_U3
	if(os_readl(U3D_LV1ISR) & MAC3_INTR){
		os_writel(U3D_LTSSM_INTR, dwLtssmValue);
	}
	#endif
   	os_writel(U3D_EPISR, dwIntrEPValue);
 	os_writel(U3D_DMAISR, dwDmaIntrValue);
 	os_writel(U3D_DEV_LINK_INTR, dwLinkIntValue);

	os_printk(K_DEBUG,"Intr: CMN[%x] Tx[%x] Rx[%x] DMA[%x] QMU[%x] QMUDn[%x] LTSSM[%x] SpdChg[%x]\n", \
				dwIntrUsbValue, wIntrTxValue, wIntrRxValue, dwDmaIntrValue, wIntrQMUValue, \
				wIntrQMUDoneValue, dwLtssmValue, dwLinkIntValue);

	#ifdef SUPPORT_OTG
	os_printk(K_DEBUG,"SSUSB_OTG_STS :%x\n",dwOtgIntValue);
	#endif

	os_printk(K_DEBUG,"EPIER=%x, EPISR=%x\n", os_readl(U3D_EPIER), os_readl(U3D_EPISR));

	if (!(dwLtssmValue | dwIntrUsbValue | dwDmaIntrValue | wIntrTxValue | \
		 wIntrRxValue | wIntrQMUValue | wIntrQMUDoneValue | dwLinkIntValue | dwOtgIntValue)) {
		os_printk(K_ERR, "<<<<Where the interrupt comes from???>>>>");
		return IRQ_NONE;
	}
	os_printk(K_INFO, "---START INTR---\n");

	if(wIntrQMUDoneValue) {
	 	qmu_proc(wIntrQMUDoneValue);
	}
 	if(wIntrQMUValue) {
		qmu_handler(wIntrQMUValue);
 	}

 	if (dwLinkIntValue & SSUSB_DEV_SPEED_CHG_INTR)
	{
		os_printk(K_ALET,"Speed Change Interrupt, Current speed=");

		dwTemp = os_readl(U3D_DEVICE_CONF) & SSUSB_DEV_SPEED;
		switch (dwTemp)
		{
 			case SSUSB_SPEED_FULL:
				os_printk(K_ALET,"FS\n");
				break;
			case SSUSB_SPEED_HIGH:
				os_printk(K_ALET,"HS\n");
				break;
			case SSUSB_SPEED_SUPER:
				os_printk(K_ALET,"SS\n");
				break;
			default:
				os_printk(K_ALET,"Invalid\n");
		}
	}

	/*                           */
	if (dwIntrUsbValue & RESET_INTR){
		#ifdef SUPPORT_OTG
			g_otg_reset = 1;
			
			//                                                               
			g_otg_suspend = 0;
			g_otg_srp_reqd = 0;
			g_otg_hnp_reqd = 0; 		
			
			g_otg_b_hnp_enable = 0;
			g_otg_config = 0;
			mb();
			os_clrmsk(U3D_DEVICE_CONTROL, HOSTREQ); 		
			os_printk(K_ERR,"RESET_INTR, g_otg_reset = 1, OTG_STS is 0x%x\n", os_readl(U3D_SSUSB_OTG_STS));
		#endif

		if(os_readl(U3D_POWER_MANAGEMENT) & HS_MODE){
			os_printk(K_NOTICE,"Device: High-speed mode\n");
			g_usb_status.speed = SSUSB_SPEED_HIGH;
		}
		else{
			os_printk(K_NOTICE,"Device: Full-speed mode\n");
			g_usb_status.speed = SSUSB_SPEED_FULL;
		}
		g_usb_status.reset_received = 1;
		#if 0		
			mu3d_hal_set_speed(SSUSB_SPEED_HIGH);
			#if 1 //      
			//                                   
			os_printk(K_ERR, "u3d_set_address to 0 (%s)\n", __func__);
			u3d_set_address(0);
			#endif
		#endif
		
		

		//            

		//                                    
		#ifdef POWER_SAVING_MODE
		#ifdef SUPPORT_U3
		os_writel(U3D_SSUSB_U3_CTRL_0P, os_readl(U3D_SSUSB_U3_CTRL_0P) &~ SSUSB_U3_PORT_PDN);
		#endif
		os_writel(U3D_SSUSB_U2_CTRL_0P, os_readl(U3D_SSUSB_U2_CTRL_0P) &~ SSUSB_U2_PORT_PDN);
		os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) &~ SSUSB_IP_DEV_PDN);
		#endif
		g_usb_status.suspend = 0;

		os_printk(K_ERR, " OTG_STS is 0x%x\n", os_readl(U3D_SSUSB_OTG_STS)) ;
		
		//            
		os_clrmsk(U3D_SSUSB_U2_CTRL_0P, SSUSB_U2_PORT_HOST_SEL);
		#if 0 //      
		//                                   
		u3d_set_address(0);
		#endif		
		//      
		u3d_sync_with_bat(USB_UNCONFIGURED) ; //                                                                         
	}

	#ifdef SUPPORT_U3
	//                       
	if (dwLtssmValue){
		if(dwLtssmValue & SS_DISABLE_INTR){
			os_printk(K_ALET,"Device: SS Disable, %d\n",
				(os_readl(U3D_LTSSM_INFO) & DISABLE_CNT) >> DISABLE_CNT_OFST);

			#ifdef U2_U3_SWITCH
			#ifdef POWER_SAVING_MODE
			os_writel(U3D_SSUSB_U2_CTRL_0P, os_readl(U3D_SSUSB_U2_CTRL_0P) &~ SSUSB_U2_PORT_DIS);
			#endif

			#ifdef U2_U3_SWITCH_AUTO
			os_setmsk(U3D_POWER_MANAGEMENT, SUSPENDM_ENABLE);
			#else
			mu3d_hal_u2dev_connect();
			#endif

			u3d_ep0en();
			#endif

			dwLtssmValue =0;
		}
		if(dwLtssmValue & RXDET_SUCCESS_INTR){
			os_printk(K_NOTICE,"Device: RX Detect Success\n");
		}
		if(dwLtssmValue & HOT_RST_INTR){
			g_hot_rst_cnt++;
			os_printk(K_NOTICE,"Device: Hot Reset, %x\n", g_hot_rst_cnt);
		}
		if(dwLtssmValue & WARM_RST_INTR){
			g_warm_rst_cnt++;
			os_printk(K_NOTICE,"Device: Warm Reset, %x\n", g_warm_rst_cnt);
		}
		if(dwLtssmValue & ENTER_U0_INTR){
			os_printk(K_ERR,"Device: Enter U0\n");
			g_usb_status.enterU0 = 1;
		}

		#ifndef EXT_VBUS_DET
		if(dwLtssmValue & VBUS_RISE_INTR){
			os_printk(K_ERR,"Device: Vbus Rise\n");
			g_usb_status.vbus_valid= 1;
		}
		if(dwLtssmValue & VBUS_FALL_INTR){
			os_printk(K_ERR,"Device: Vbus Fall\n");
			g_usb_status.vbus_valid= 0;

			//             
			#if 0 //                   
			os_printk(K_ERR, "SOFTCONN = 0\n");
			mu3d_hal_u2dev_disconn();

			//                         
			os_printk(K_ERR, "Toggle USB3_EN\n");
			os_writel(U3D_USB3_CONFIG, 0);
			os_ms_delay(50);
			os_writel(U3D_USB3_CONFIG, USB3_EN);
			#endif
		}
		#endif
		if(dwLtssmValue & ENTER_U1_INTR){
			os_printk(K_ERR,"Device: Enter U1\n");
			g_usb_status.suspend = 1;
		}
		if(dwLtssmValue & ENTER_U2_INTR){
			os_printk(K_ERR,"Device: Enter U2\n");
			g_usb_status.suspend = 1;
		}
		if(dwLtssmValue & ENTER_U3_INTR){
			os_printk(K_ERR,"Device: Enter U3\n");
			g_usb_status.suspend = 1;

#ifdef POWER_SAVING_MODE
			os_writel(U3D_SSUSB_U3_CTRL_0P, os_readl(U3D_SSUSB_U3_CTRL_0P) | SSUSB_U3_PORT_PDN);
			os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) | SSUSB_IP_DEV_PDN);
#endif
		}
		if(dwLtssmValue & EXIT_U1_INTR){
			os_printk(K_ERR,"Device: Exit U1\n");
			g_usb_status.suspend = 0;
		}
		if(dwLtssmValue & EXIT_U2_INTR){
			os_printk(K_ERR,"Device: Exit U2\n");
			g_usb_status.suspend = 0;
		}
		if(dwLtssmValue & EXIT_U3_INTR){
			os_printk(K_ERR,"Device: Exit U3\n");
			g_usb_status.suspend = 0;
#ifdef POWER_SAVING_MODE
			os_writel(U3D_SSUSB_U3_CTRL_0P, os_readl(U3D_SSUSB_U3_CTRL_0P) &~ SSUSB_U3_PORT_PDN);
			os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) &~ SSUSB_IP_DEV_PDN);
			while(!(os_readl(U3D_SSUSB_IP_PW_STS1) & SSUSB_U3_MAC_RST_B_STS));
#endif

		}
#ifndef POWER_SAVING_MODE
		if(dwLtssmValue & U3_RESUME_INTR){
			g_usb_status.suspend= 0;
			os_writel(U3D_SSUSB_U3_CTRL_0P, os_readl(U3D_SSUSB_U3_CTRL_0P) &~ SSUSB_U3_PORT_PDN);
			os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) &~ SSUSB_IP_DEV_PDN);
			while(!(os_readl(U3D_SSUSB_IP_PW_STS1) & SSUSB_U3_MAC_RST_B_STS));
			os_writel(U3D_LINK_POWER_CONTROL, os_readl(U3D_LINK_POWER_CONTROL) | UX_EXIT);
		}
#endif
	}
	#endif

	if(dwIntrUsbValue & DISCONN_INTR) {
		#ifdef SUPPORT_OTG
		//                                                     
//        
		g_otg_reset = 0;
		g_otg_disconnect = 1;		
		//                   
		g_otg_hnp_reqd = 0; 		
		os_printk(K_ERR, "g_otg_hnp_reqd,g_otg_srp_reqd,g_otg_b_hnp_enable = 0 (%s)\n", __func__);				
		g_otg_b_hnp_enable = 0;
		g_otg_config = 0;
		mb();
//           
		#endif
		
		os_printk(K_NOTICE,"Device: Disconnect\n");
#ifdef POWER_SAVING_MODE
		os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) & ~SSUSB_IP_DEV_PDN);
		#ifdef SUPPORT_U3
		os_writel(U3D_SSUSB_U3_CTRL_0P, os_readl(U3D_SSUSB_U3_CTRL_0P) & ~SSUSB_U3_PORT_PDN);
		#endif
		os_writel(U3D_SSUSB_U2_CTRL_0P, os_readl(U3D_SSUSB_U2_CTRL_0P) & ~SSUSB_U2_PORT_PDN);
#endif
		//      
		u3d_sync_with_bat(USB_SUSPEND) ;
	}

	if(dwIntrUsbValue & CONN_INTR) {
		#ifdef SUPPORT_OTG
		g_otg_connect = 1;
		mb();
		#endif
		os_printk(K_NOTICE,"Device: Connect\n");
	}

	if(dwIntrUsbValue & SUSPEND_INTR) {
		os_printk(K_NOTICE,"Suspend Interrupt\n");
#ifdef POWER_SAVING_MODE
		os_writel(U3D_SSUSB_U2_CTRL_0P, os_readl(U3D_SSUSB_U2_CTRL_0P) | SSUSB_U2_PORT_PDN);
		os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) | SSUSB_IP_DEV_PDN);
#endif
		g_usb_status.suspend = 1;
#ifdef SUPPORT_OTG
		g_otg_suspend = 1;
		mb();
		os_printk(K_ERR, "OTG: suspend\n");
#endif
		//      
		u3d_sync_with_bat(USB_SUSPEND) ;
	}

	if(dwIntrUsbValue & LPM_INTR) {
		os_printk(K_NOTICE,"LPM Interrupt\n");

		dwTemp = os_readl(U3D_USB20_LPM_PARAMETER);
		os_printk(K_NOTICE, "BESL: %x, %x <= %x <= %x\n", dwTemp&0xf, (dwTemp>>8)&0xf, (dwTemp>>12)&0xf, (dwTemp>>4)&0xf);

		dwTemp = os_readl(U3D_POWER_MANAGEMENT);
		os_printk(K_NOTICE, "RWP: %x\n", (dwTemp&LPM_RWP)>>11);
#ifdef POWER_SAVING_MODE
		if(!((os_readl(U3D_POWER_MANAGEMENT) & LPM_HRWE))){
			os_writel(U3D_SSUSB_U2_CTRL_0P, os_readl(U3D_SSUSB_U2_CTRL_0P) | SSUSB_U2_PORT_PDN);
			os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) | SSUSB_IP_DEV_PDN);
		}
#endif
		if (g_sw_rw)
		{
#ifdef POWER_SAVING_MODE
			os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) & ~SSUSB_IP_DEV_PDN);
			os_writel(U3D_SSUSB_U2_CTRL_0P, os_readl(U3D_SSUSB_U2_CTRL_0P) & ~SSUSB_U2_PORT_PDN);
			while(!(os_readl(U3D_SSUSB_IP_PW_STS2) & SSUSB_U2_MAC_SYS_RST_B_STS));
#endif
			os_writel(U3D_USB20_MISC_CONTROL, os_readl(U3D_USB20_MISC_CONTROL) | LPM_U3_ACK_EN);//             

			//                                                                     
			os_ms_delay(20);
			os_writel(U3D_POWER_MANAGEMENT, os_readl(U3D_POWER_MANAGEMENT) | RESUME);

			os_printk(K_NOTICE, "RESUME: %d\n", os_readl(U3D_POWER_MANAGEMENT) & RESUME);
		}
	}

	if(dwIntrUsbValue & LPM_RESUME_INTR) {
		#ifdef SUPPORT_OTG
		g_otg_resume = 1;
		mb();
		#endif
		if(!(os_readl(U3D_POWER_MANAGEMENT) & LPM_HRWE)){
#ifdef POWER_SAVING_MODE
			os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) & ~SSUSB_IP_DEV_PDN);
			os_writel(U3D_SSUSB_U2_CTRL_0P, os_readl(U3D_SSUSB_U2_CTRL_0P) & ~SSUSB_U2_PORT_PDN);
			while(!(os_readl(U3D_SSUSB_IP_PW_STS2) & SSUSB_U2_MAC_SYS_RST_B_STS));
#endif
			os_writel(U3D_USB20_MISC_CONTROL, os_readl(U3D_USB20_MISC_CONTROL) | LPM_U3_ACK_EN);//             
		}
	}

	/*                                    */
	if (dwIntrUsbValue & RESUME_INTR) {
		os_printk(K_NOTICE, "Resume Interrupt\n");
#ifdef POWER_SAVING_MODE
		os_writel(U3D_SSUSB_IP_PW_CTRL2, os_readl(U3D_SSUSB_IP_PW_CTRL2) & ~SSUSB_IP_DEV_PDN);
 		os_writel(U3D_SSUSB_U2_CTRL_0P, os_readl(U3D_SSUSB_U2_CTRL_0P) & ~SSUSB_U2_PORT_PDN);
		while(!(os_readl(U3D_SSUSB_IP_PW_STS2) & SSUSB_U2_MAC_SYS_RST_B_STS));
#endif
		g_usb_status.suspend = 0;

		//      
		u3d_sync_with_bat(USB_CONFIGURED) ;
	}



	#ifdef SUPPORT_OTG
	if (dwOtgIntValue)
	{
		os_printk(K_ERR, "OTG HW: %x\n", dwOtgIntValue);	
		if (dwOtgIntValue & VBUS_CHG_INTR)
		{
			g_otg_vbus_chg = 1;
			mb();
//        
#if 0
			if(!(dwOtgIntValue & SSUSB_VBUS_VALID)){
				g_otg_disconnect = 1;
				mb();
			}
#endif
//           
			os_printk(K_ERR, "OTG: VBUS_CHG_INTR\n");
			os_setmsk(U3D_SSUSB_OTG_STS_CLR, SSUSB_VBUS_INTR_CLR);
			
			//     
			/*
                                                                             
                                                                
                                                      
    
   */
		}

		//                                                     
		if (dwOtgIntValue & SSUSB_CHG_B_ROLE_B)
		{
			//            
			//                       
			//     
			os_printk(K_NOTICE, "OTG: CHG_B_ROLE_B\n");
			os_setmsk(U3D_SSUSB_OTG_STS_CLR, SSUSB_CHG_B_ROLE_B_CLR);


			timer = 0;
			//                           
			//                                           
			while (os_readl(U3D_SSUSB_OTG_STS) & SSUSB_XHCI_MAS_DMA_REQ)
			{
				timer++;
				os_ms_delay(10);
				if (!(timer % 5))
					os_printk(K_ERR, "DMA not ready(%X) %d\n", os_readl(U3D_SSUSB_OTG_STS) & SSUSB_XHCI_MAS_DMA_REQ, timer);

				if (timer > 10)
					break;			
			}			
			os_clrmsk(U3D_SSUSB_U2_CTRL_0P, SSUSB_U2_PORT_HOST_SEL);
			os_printk(K_ERR, "Switch DMA to device done\n");
			//            
			g_otg_vbus_chg = 0;	
			g_otg_reset = 0;	
			g_otg_suspend = 0;	
			g_otg_resume = 0;
			g_otg_connect = 0;
			g_otg_disconnect = 0;	
			g_otg_chg_a_role_b = 0;
			g_otg_attach_b_role = 0;
			//                                                      
			//                                                          
	
			g_otg_chg_b_role_b = 1;
			mb();
		}

		//                                                   
		if (dwOtgIntValue & SSUSB_CHG_A_ROLE_B)
		{
			g_otg_chg_a_role_b = 1;
			mb();
			os_printk(K_NOTICE, "OTG: CHG_A_ROLE_B\n");
			os_setmsk(U3D_SSUSB_OTG_STS_CLR, SSUSB_CHG_A_ROLE_B_CLR);
		}

		//                                           
		if (dwOtgIntValue & SSUSB_ATTACH_B_ROLE)
		{
			g_otg_attach_b_role = 1;
			mb();
			os_printk(K_NOTICE, "U3D_T, OTG: CHG_ATTACH_B_ROLE\n");
			os_setmsk(U3D_SSUSB_OTG_STS_CLR, SSUSB_ATTACH_B_ROLE_CLR);

			//                           
			os_printk(K_NOTICE, "U3D_T, Switch DMA to device\n");
			os_clrmsk(U3D_SSUSB_U2_CTRL_0P, SSUSB_U2_PORT_HOST_SEL);
		}
	}
	#endif


    if(dwDmaIntrValue) {
      	u3d_dma_handler(dwDmaIntrValue);
    }

	if((wIntrTxValue & 1) || (wIntrRxValue & 1)) {
		if (wIntrRxValue & 1)
		{
			os_printk(K_ERR, "Service SETUPEND\n");
		}
		//                                                     
		//                                                             

		u3d_ep0_handler();
	}

    if(wIntrTxValue) {
    	for(ep_num = 1; ep_num <= TX_FIFO_NUM; ep_num++) {
          	if(wIntrTxValue & (1<<ep_num)) {
				os_printk(K_NOTICE, "Tx Intr=%x\n",wIntrTxValue);

				if(USB_ReadCsr32(U3D_TX1CSR0, ep_num) & TX_SENTSTALL) {
					USB_WriteCsr32(U3D_TX1CSR0, ep_num, USB_ReadCsr32(U3D_TX1CSR0, ep_num) | TX_SENTSTALL);
				} else {
					u3d_epx_handler(ep_num, USB_TX);
				}
			}
       	}
  	}
    if(wIntrRxValue) {
     	for(ep_num = 1; ep_num <= RX_FIFO_NUM; ep_num++) {
         	if(wIntrRxValue & (1<<ep_num)) {
				os_printk(K_NOTICE, "Rx Intr=%x\n",wIntrRxValue);

				if(USB_ReadCsr32(U3D_RX1CSR0, ep_num) & RX_SENTSTALL){
					USB_WriteCsr32(U3D_RX1CSR0, ep_num, USB_ReadCsr32(USB_RX, ep_num) | RX_SENTSTALL);
				} else {
          			u3d_epx_handler(ep_num, USB_RX);
				}
			}
    	}
 	}

	os_printk(K_INFO, "---END INTR--\n");

	return IRQ_HANDLED;
}

