#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/spi/spi.h>
#include <linux/workqueue.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/irqreturn.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/wakelock.h>
#include <mach/irqs.h>

#include <mach/mt_spi.h>
//                         
#include "mach/memory.h"
#include "mt_spi_hal.h"
#include <mach/mt_clkmgr.h>


#if (defined(CONFIG_MT6582_FPGA))
#define  CONFIG_MT_SPI_FPGA_ENABLE
#endif

/*                 */
//                  

/*                    */
//                   

/*                                                 */
//                      

#ifdef SPI_DEBUG
	#define SPI_DBG(fmt, args...)  printk(KERN_ALERT "spi.c:%5d: <%s>" fmt, __LINE__,__func__,##args )


#ifdef SPI_VERBOSE
	#define SPI_INFO(dev,fmt, args...)  dev_alert(dev,"spi.c:%5d: <%s>" fmt, __LINE__,__func__,##args)
#else
	#define SPI_INFO(dev,fmt, args...) 
#endif
#else
	#define SPI_DBG(fmt, args...)  
	#define SPI_INFO(dev,fmt, args...) 
#endif

#ifdef SPI_REC_DEBUG
/*                                                                  
                              
  */
#define SPI_CLOCK_PERIED 100000  //   

#include <linux/syscalls.h>	//        
#define SPI_REC_MSG_MAX 500
#define SPI_REC_NUM 20
#define SPI_REC_STR_LEN 256
static u32 spi_speed;
static char msg_rec[SPI_REC_MSG_MAX][SPI_REC_STR_LEN];
static int rec_count=0;
static int rec_count_tmp=1;
static int rec_msg_count=0;

static atomic_t rec_log_count = ATOMIC_INIT(0);

static unsigned long long rec_msg_time[SPI_REC_MSG_MAX];
static unsigned long long rec_time;

/*                                                   
                                      
                            
*/
unsigned long long spi_rec_t0; //                     


DEFINE_SPINLOCK(msg_rec_lock);
//                                    

/*
                                       
*/

static inline void spi_rec_time(const char *str)
{
	unsigned long flags;
	char tmp[64];
	
	spin_lock_irqsave(&msg_rec_lock,flags);
	
	if(strncmp(str, "msgs", 4) == 0){
		rec_msg_count ++;
		if(rec_msg_count >= SPI_REC_MSG_MAX)
			rec_msg_count = 0;
		rec_msg_time[rec_msg_count] = sched_clock();
		msg_rec[rec_msg_count][0] = '\0';
		sprintf(tmp,"%s,pid:%4d;",str,sys_getpid());
		strcat(msg_rec[rec_msg_count],tmp);
	}else if(strncmp(str, "msgn", 4) == 0){
		rec_count ++;
		if(rec_count >= SPI_REC_MSG_MAX)
			rec_count = 0;
		rec_time = sched_clock();
//                                
		sprintf(tmp,"%s:%8lld;",str,rec_time - rec_msg_time[rec_count]);
		strcat(msg_rec[rec_count],tmp);
//                                                     
/*                                                   */
	}/*                                      
                                                                                         
                                                                             
                                  
        
                                  
   
  */else{
		sprintf(tmp,"%s:%8lld;",str,sched_clock() - rec_time);
		if((strlen(tmp) + strlen(msg_rec[rec_count])) < (SPI_REC_STR_LEN - 64)){			
			strcat(msg_rec[rec_count],tmp);
		}else{
			strcat(msg_rec[rec_count],"@");
		}
	}
	spin_unlock_irqrestore(&msg_rec_lock,flags);
//                                              
	return;
}

void mt_spi_workqueue_handler(void *data)
{
	int i = 0;
	for(i=0;i< SPI_REC_NUM; i++){							
		printk(KERN_ALERT"spi-rec%3d-%3d:%s\n",rec_count,rec_count_tmp,msg_rec[rec_count_tmp]);
		msg_rec[rec_count_tmp][0] = '\0';
		rec_count_tmp ++;
		if(rec_count_tmp >= SPI_REC_MSG_MAX){
			rec_count_tmp = 0;			
		}
		
	}
	atomic_dec(&rec_log_count);
	return;
}

DECLARE_WORK(mt_spi_workqueue,mt_spi_workqueue_handler);

#else
static inline void spi_rec_time(const char *str)
{
	return;
}

#endif

//                              
/*
                                       
 
               
        
 
*/
//                                                              


#define IDLE 0
#define INPROGRESS 1
#define PAUSED 2

#define PACKET_SIZE 0x400

#define SPI_FIFO_SIZE 32
//                  
//                  
enum spi_fifo {
	FIFO_TX,
	FIFO_RX,
	FIFO_ALL
};

#define INVALID_DMA_ADDRESS 0xffffffff


struct mt_spi_t {
	struct platform_device *pdev;
	void __iomem *regs;
	int irq;
	int running;
	struct wake_lock wk_lock;
	struct mt_chip_conf *config;	
	struct spi_master *master;
	
	struct spi_transfer *cur_transfer;
	struct spi_transfer *next_transfer;
	
	spinlock_t lock;
	struct list_head	queue;
	
};

static void spi_gpio_set(struct mt_spi_t *ms)
{
//                                                               
//                                                                
//                                                                    
//                                                                   

	return;
}

static void spi_gpio_reset(struct mt_spi_t *ms)
{
	//                          
//                                                           
//                                                             
//                                                                
//                                                               

	return;
}

static void enable_clk(void)
{
#if (!defined(CONFIG_MT_SPI_FPGA_ENABLE))
	enable_clock(MT_CG_PERI_SPI0, "spi");
#endif
	return;
}

static void disable_clk(void)
{
#if (!defined(CONFIG_MT_SPI_FPGA_ENABLE))
	disable_clock(MT_CG_PERI_SPI0, "spi");
#endif
	return;
}

static int is_pause_mode(struct spi_message	*msg)
{
	struct mt_chip_conf *conf;	
	conf = (struct mt_chip_conf *)msg->state;
	return conf->pause;
}
static int is_fifo_read(struct spi_message *msg)
{
	struct mt_chip_conf *conf;		
	conf = (struct mt_chip_conf *)msg->state;
	return ((conf->com_mod == FIFO_TRANSFER) || (conf->com_mod == OTHER1)) ;
}

static int is_interrupt_enable(struct mt_spi_t *ms)
{
	u32 cmd;
	cmd = spi_readl ( ms, SPI_CMD_REG);

	return (cmd >> SPI_CMD_FINISH_IE_OFFSET) & 1;
}

static void inline set_pause_bit(struct mt_spi_t *ms)
{
	u32 reg_val;
	
	reg_val = spi_readl(ms, SPI_CMD_REG);
	reg_val |= 1 << SPI_CMD_PAUSE_EN_OFFSET;	
	spi_writel(ms, SPI_CMD_REG, reg_val);
	return;
}

static void inline clear_pause_bit(struct mt_spi_t *ms)
{
	u32 reg_val;
	reg_val = spi_readl(ms, SPI_CMD_REG);
	reg_val &= ~SPI_CMD_PAUSE_EN_MASK;
	spi_writel(ms, SPI_CMD_REG, reg_val);

	return;
}

static void inline clear_resume_bit(struct mt_spi_t *ms)
{
	u32 reg_val;

	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val &= ~SPI_CMD_RESUME_MASK;
	spi_writel ( ms, SPI_CMD_REG, reg_val );

	return;
}
static void inline spi_disable_dma(struct mt_spi_t *ms)
{ 
	u32  cmd;

	cmd = spi_readl( ms, SPI_CMD_REG);
	cmd &= ~SPI_CMD_TX_DMA_MASK;
	cmd &= ~SPI_CMD_RX_DMA_MASK;
	spi_writel(ms, SPI_CMD_REG, cmd);
	
	return ;
}

static void inline spi_clear_fifo(struct mt_spi_t *ms, enum spi_fifo fifo)
{ 
	u32 volatile reg_val;
	int i;

	for(i = 0; i < SPI_FIFO_SIZE/4; i++){
		if ( fifo == FIFO_TX )
			spi_writel( ms, SPI_TX_DATA_REG, 0x0 );
		else if( fifo == FIFO_RX )
			reg_val = spi_readl ( ms, SPI_RX_DATA_REG );
		else if(fifo == FIFO_ALL){
			spi_writel( ms, SPI_TX_DATA_REG, 0x0 );
			spi_writel( ms, SPI_RX_DATA_REG, 0x0 );		//          
			reg_val = spi_readl( ms, SPI_RX_DATA_REG );
		}else{
			SPI_DBG("The parameter is not right.\n");
		}
	}	
	return;
}

static void inline spi_enable_dma(struct mt_spi_t *ms,u8 mode)
{ 
	u32  cmd;

	cmd = spi_readl(ms, SPI_CMD_REG);
#define SPI_4B_ALIGN 0x4	
	/*                          */
	if((mode == DMA_TRANSFER) || (mode == OTHER1)){
		if((ms->cur_transfer->tx_buf != NULL)|| ((ms->cur_transfer->tx_dma != INVALID_DMA_ADDRESS) && (ms->cur_transfer->tx_dma != 0))){
			if(ms->cur_transfer->tx_dma & (SPI_4B_ALIGN - 1)){
				dev_err(&ms->pdev->dev, "Warning!Tx_DMA address should be 4Byte alignment,buf:%p,dma:%x\n",ms->cur_transfer->tx_buf,ms->cur_transfer->tx_dma);
			}
			spi_writel(ms, SPI_TX_SRC_REG,cpu_to_le32(ms->cur_transfer->tx_dma));
			cmd |= 1 << SPI_CMD_TX_DMA_OFFSET;
		}		
	}
	if((mode == DMA_TRANSFER) || (mode == OTHER2)){
		if((ms->cur_transfer->rx_buf != NULL)|| ((ms->cur_transfer->rx_dma != INVALID_DMA_ADDRESS) && (ms->cur_transfer->rx_dma != 0))){
			if(ms->cur_transfer->rx_dma & (SPI_4B_ALIGN - 1)){
				dev_err(&ms->pdev->dev, "Warning!Rx_DMA address should be 4Byte alignment,buf:%p,dma:%x\n",ms->cur_transfer->rx_buf,ms->cur_transfer->rx_dma);
			}
			spi_writel(ms, SPI_RX_DST_REG, cpu_to_le32(ms->cur_transfer->rx_dma));	
			cmd |= 1 << SPI_CMD_RX_DMA_OFFSET;
		}		
	}	
	mb();
	spi_writel(ms, SPI_CMD_REG, cmd);
	
	return ;
}

static int inline spi_setup_packet(struct mt_spi_t *ms)
{ 
	u32  packet_size,packet_loop, cfg1;

	//                              
	if(ms->cur_transfer->len < PACKET_SIZE) 
		packet_size = ms->cur_transfer->len;
	else
		packet_size = PACKET_SIZE;
	
	if(ms->cur_transfer->len % packet_size){
		packet_loop = ms->cur_transfer->len/packet_size + 1;
		/*                                                                             */			
		dev_err(&ms->pdev->dev, "ERROR!!The lens must be a multiple of %d, your len %u\n\n",PACKET_SIZE,ms->cur_transfer->len);
		return -EINVAL;
	}else
		packet_loop = (ms->cur_transfer->len)/packet_size;

	SPI_DBG("The packet_size:0x%x packet_loop:0x%x\n", packet_size,packet_loop);

	cfg1 = spi_readl(ms, SPI_CFG1_REG);
	cfg1 &= ~(SPI_CFG1_PACKET_LENGTH_MASK + SPI_CFG1_PACKET_LOOP_MASK);
	cfg1 |= (packet_size - 1)<<SPI_CFG1_PACKET_LENGTH_OFFSET;
	cfg1 |= (packet_loop - 1)<<SPI_CFG1_PACKET_LOOP_OFFSET;
	spi_writel ( ms, SPI_CFG1_REG, cfg1 );

	return 0;
}

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
//         
//                         
//            
//   
//  

static void inline spi_start_transfer(struct mt_spi_t *ms)
{
	u32 reg_val;
	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val |= 1 << SPI_CMD_ACT_OFFSET;

	/*                                                                */
	mb();
	spi_writel ( ms, SPI_CMD_REG, reg_val);	

	return;
}

static void  inline spi_resume_transfer(struct mt_spi_t *ms)
{
	u32 reg_val;

	reg_val = spi_readl(ms, SPI_CMD_REG);
	reg_val &= ~SPI_CMD_RESUME_MASK;
	reg_val |= 1 << SPI_CMD_RESUME_OFFSET;
	/*                                                                */
	mb();
	spi_writel ( ms,  SPI_CMD_REG, reg_val );	

	return;	
}

static void reset_spi(struct mt_spi_t *ms)
{
	u32 reg_val;

	/*                                          */
	reg_val=spi_readl(ms, SPI_CMD_REG);
	reg_val &= ~SPI_CMD_RST_MASK;
	reg_val |= 1 << SPI_CMD_RST_OFFSET;
	spi_writel(ms, SPI_CMD_REG, reg_val);
	
	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val &= ~SPI_CMD_RST_MASK;
	spi_writel(ms, SPI_CMD_REG, reg_val );

	return;
}

static inline int 
is_last_xfer(struct spi_message *msg, struct spi_transfer *xfer)
{
	return msg->transfers.prev == &xfer->transfer_list;
}

static int 
transfer_dma_mapping(struct mt_spi_t *ms, u8 mode, struct spi_transfer *xfer )
{	
	struct device *dev = &ms->pdev->dev;
		
//                     

	xfer->tx_dma = xfer->rx_dma = INVALID_DMA_ADDRESS;
	
	if((mode == DMA_TRANSFER) || (mode == OTHER1)){
		if(xfer->tx_buf){
			xfer->tx_dma = dma_map_single(dev, (void *)xfer->tx_buf, 	
				xfer->len,  DMA_TO_DEVICE);				
			
			if (dma_mapping_error(dev, xfer->tx_dma)) {
				dev_err(&ms->pdev->dev,"dma mapping tx_buf error.\n");
				return -ENOMEM;	
			}
//                                          
		}
//                                               
	
	}
	if((mode == DMA_TRANSFER) || (mode == OTHER2)){
		if(xfer->rx_buf){
			xfer->rx_dma = dma_map_single(dev, xfer->rx_buf, 
				xfer->len, DMA_FROM_DEVICE );	

			if ( dma_mapping_error ( dev, 
				xfer->rx_dma ) ) {
				if ( xfer->tx_buf ) 
					dma_unmap_single ( dev, xfer->tx_dma, 
						xfer->len, DMA_TO_DEVICE );

				dev_err(&ms->pdev->dev,"dma mapping rx_buf error.\n");
				return -ENOMEM;
			}
//                                          
		}
//                                                
	
	}
	if(mode != FIFO_TRANSFER){
//                                              
	}
	return 0;	
}

static void 
transfer_dma_unmapping(struct mt_spi_t *ms,struct spi_transfer *xfer )
{
	struct device *dev=&ms->pdev->dev;
	
	if((xfer->tx_dma != INVALID_DMA_ADDRESS) && (xfer->tx_dma != 0)){ 
		dma_unmap_single(dev, xfer->tx_dma, xfer->len, DMA_TO_DEVICE);
		SPI_INFO(dev,"tx_dma:0x%x,unmapping\n",xfer->tx_dma);
		xfer->tx_dma = INVALID_DMA_ADDRESS;
	}
	if((xfer->rx_dma != INVALID_DMA_ADDRESS) && (xfer->rx_dma != 0)){
		dma_unmap_single(dev, xfer->rx_dma, xfer->len, DMA_FROM_DEVICE );		
		SPI_INFO(dev,"rx_dma:0x%x,unmapping\n",xfer->rx_dma);
		xfer->rx_dma = INVALID_DMA_ADDRESS;
	}

	return;
}


#ifdef SPI_VERBOSE
static void spi_dump_reg(struct mt_spi_t *ms)
{

	SPI_DBG("cfg0:0x%x\n", spi_readl(ms, SPI_CFG0_REG));
	SPI_DBG("cfg1:0x%x\n", spi_readl(ms, SPI_CFG1_REG));
	SPI_DBG("cmd:0x%x\n", spi_readl(ms, SPI_CMD_REG));
//                                                                    
//                                                                    
	SPI_DBG("spi_tx_src:0x%x\n", spi_readl( ms, SPI_TX_SRC_REG));
	SPI_DBG("spi_rx_dst:0x%x\n", spi_readl(ms, SPI_RX_DST_REG) );
	SPI_DBG("spi_busy:0x%x\n", spi_readl(ms, SPI_STATUS1_REG));

	return;
}
#endif

static void mt_spi_msg_done(struct mt_spi_t *ms,struct spi_message *msg, int status);
static void mt_spi_next_message(struct mt_spi_t *ms);
static int	mt_do_spi_setup(struct mt_spi_t *ms, struct mt_chip_conf *chip_config);


static int 
mt_spi_next_xfer(struct mt_spi_t *ms, struct spi_message *msg)
{
	struct spi_transfer	*xfer;
	struct mt_chip_conf *chip_config = (struct mt_chip_conf *)msg->state;
  	u8  mode,cnt,i;
	int ret = 0;	
	char xfer_rec[16];

	if(unlikely(!ms)){
		dev_err(&msg->spi->dev, "master wrapper is invalid \n");
		ret = -EINVAL;
		goto fail;
	}
	if(unlikely(!msg)){
		dev_err(&msg->spi->dev, "msg is invalid \n");
		ret = -EINVAL;
		goto fail;
	}
	if(unlikely(!msg->state)){
		dev_err(&msg->spi->dev, "msg config is invalid \n");
		ret = -EINVAL;
		goto fail;
	}
	
	if(unlikely(!is_interrupt_enable(ms))){
		dev_err(&msg->spi->dev, "interrupt is disable\n");
		ret = -EINVAL;
		goto fail;
	}

	
	mode = chip_config->com_mod;
		
	xfer = ms->cur_transfer;
	
	SPI_DBG("start xfer 0x%p, mode %d, len %u\n",xfer,mode,xfer->len);

	if((mode == FIFO_TRANSFER) || (mode == OTHER1) || (mode == OTHER2)){
		if(xfer->len > SPI_FIFO_SIZE ){ 
			ret = -EINVAL;
			dev_err(&msg->spi->dev, "xfer len is invalid over fifo size\n");
			goto fail;
		}
	}
	
	if(is_last_xfer(msg,xfer)){
		SPI_DBG("The last xfer.\n");	
		ms->next_transfer = NULL;
		clear_pause_bit( ms );
	}else{
		SPI_DBG("Not the last xfer.\n");
		ms->next_transfer = list_entry(xfer->transfer_list.next, 
				struct spi_transfer, transfer_list );
	}
	//           
	spi_disable_dma(ms);
	spi_clear_fifo(ms, FIFO_ALL);

	ret = spi_setup_packet(ms);
	if(ret < 0){
		goto fail;
	}
	
	/*                       */	
	if((mode == FIFO_TRANSFER) || (mode == OTHER2)){
		cnt = (xfer->len%4)?(xfer->len/4 + 1):(xfer->len/4);
		for(i = 0; i < cnt; i++){
			spi_writel(ms, SPI_TX_DATA_REG,*( ( u32 * ) xfer->tx_buf + i ) );
			SPI_INFO(&msg->spi->dev,"tx_buf data is:%x\n",*( ( u32 * ) xfer->tx_buf + i ) );
			SPI_INFO(&msg->spi->dev,"tx_buf addr is:%p\n", (u32 *)xfer->tx_buf + i);			
		}
	}
	/*                      */	
	if((mode == DMA_TRANSFER) || (mode == OTHER1) || (mode == OTHER2)){
		spi_enable_dma(ms, mode);
	}
	
#ifdef SPI_VERBOSE
	spi_dump_reg(ms); /*                             */
#endif	

	if(ms->running == PAUSED){
		SPI_DBG("pause status to resume.\n");
		
		spi_resume_transfer(ms);
	}else if(ms->running == IDLE){
		SPI_DBG("The xfer start\n");
		/*                                                           */
		if(is_pause_mode(msg) && !is_last_xfer(msg,xfer)){
			SPI_DBG("set pause mode.\n");
			set_pause_bit(ms);
		}
		/*                                                                */

		spi_start_transfer(ms);
		
	}else{
		dev_err(&msg->spi->dev, "Wrong status\n");
		ret = -1;
		goto fail;
	}
	sprintf(xfer_rec,"xfer,%3d",xfer->len);
	spi_rec_time(xfer_rec);

	ms->running = INPROGRESS;		

	/*               */
	if(is_pause_mode(msg) && is_last_xfer(msg,xfer)){		
		clear_resume_bit(ms);
	}

	return 0;
fail:
	
	list_for_each_entry(xfer, &msg->transfers, transfer_list) {
		if ((!msg->is_dma_mapped)) {
			transfer_dma_unmapping(ms, xfer);
		}
	}
	ms->running = IDLE;
	mt_spi_msg_done(ms, msg, ret);
	return ret;
}

static void
mt_spi_msg_done(struct mt_spi_t *ms, struct spi_message *msg, int status)
{
	
	list_del(&msg->queue);
	msg->status = status;

	SPI_DBG("msg:%p complete(%d): %u bytes transferred\n",msg,status,msg->actual_length);

	spi_disable_dma(ms);
	spi_clear_fifo(ms, FIFO_ALL);

	msg->complete(msg->context);

	ms->running			= IDLE;
	ms->cur_transfer	= NULL;
	ms->next_transfer	= NULL;
	
	
//                
	spi_rec_time("msge");
#ifdef SPI_REC_DEBUG

	if(!(rec_count%SPI_REC_NUM)){
		atomic_inc(&rec_log_count);
		schedule_work(&mt_spi_workqueue);
	}		
//                                      
//                                     
//                                     
//                       
//                    
//   
#endif

	/*                    */
	if(list_empty(&ms->queue)){		
		SPI_DBG( "All msg is completion.\n\n");
		/*                    */
		spi_gpio_reset(ms);
			disable_clk();
//                                                          

		wake_unlock(&ms->wk_lock);
	}else
		mt_spi_next_message(ms);
}

static void mt_spi_next_message(struct mt_spi_t *ms)
{
	struct spi_message	*msg;
	struct mt_chip_conf *chip_config;
	char msg_addr[16];

	msg = list_entry(ms->queue.next, struct spi_message, queue);
	chip_config = (struct mt_chip_conf *)msg->state;
	
#ifdef SPI_REC_DEBUG
		spi_speed = SPI_CLOCK_PERIED/(chip_config->low_time + chip_config->high_time);
	sprintf(msg_addr,"msgn,%4dKHz",spi_speed);
#else
	sprintf(msg_addr,"msgn");

#endif

	//                           
	
	spi_rec_time(msg_addr);	
//                           
//                                                                    
	
	
	SPI_DBG("start transfer message:0x%p\n", msg);
	ms->cur_transfer = list_entry( msg->transfers.next, struct spi_transfer, transfer_list);

	/*                  */
//                   
//               
//                           
//                       
//                           
//                                                                    
		reset_spi(ms);
	mt_do_spi_setup(ms, chip_config);	
	mt_spi_next_xfer(ms, msg);
	
}

static int 
mt_spi_transfer(struct spi_device *spidev, struct spi_message *msg )
{
	struct spi_master *master ;
	struct mt_spi_t *ms;	
	struct spi_transfer *xfer;
	struct mt_chip_conf *chip_config;
	unsigned long		flags;
	char msg_addr[16];	
	master = spidev->master;
	ms = spi_master_get_devdata(master);	
	
//                             
	SPI_DBG("enter,start add msg:0x%p\n",msg);
	
	if(unlikely(!msg)){
		dev_err(&spidev->dev, "msg is NULL pointer. \n" );
		msg->status = -EINVAL;
		goto out;
	}
	if(unlikely(list_empty(&msg->transfers))){		
		dev_err(&spidev->dev, "the message is NULL.\n" );
		msg->status = -EINVAL;
		msg->actual_length = 0;
		goto out;
	}

	/*                                         */
	if(master->setup(spidev)){
		dev_err(&spidev->dev, "set up error.\n");
		msg->status = -EINVAL;
		msg->actual_length = 0;
		goto out;
	}
	sprintf(msg_addr,"msgs:%p",msg);
	spi_rec_time(msg_addr);
	
	chip_config	= (struct mt_chip_conf *)spidev->controller_data;
	msg->state	= chip_config;

	list_for_each_entry(xfer, &msg->transfers, transfer_list) {
		if (!((xfer->tx_buf || xfer->rx_buf) && xfer->len)) {
			dev_err(&spidev->dev,"missing tx %p or rx %p buf, len%d\n",xfer->tx_buf,xfer->rx_buf,xfer->len);
			msg->status = -EINVAL;
			goto out;
		}

		/*
                                                             
                             
    
                                                              
                                                               
                                                 
   */
		if ((!msg->is_dma_mapped)) {
			if (transfer_dma_mapping(ms, chip_config->com_mod, xfer) < 0)
				return -ENOMEM;
		}
	}
#ifdef SPI_VERBOSE
	list_for_each_entry(xfer, &msg->transfers, transfer_list) {
		SPI_INFO(&spidev->dev,"xfer %p: len %04u tx %p/%08x rx %p/%08x\n",
			xfer, xfer->len,
			xfer->tx_buf, xfer->tx_dma,
			xfer->rx_buf, xfer->rx_dma);
	}
#endif
	msg->status = -EINPROGRESS;
	msg->actual_length = 0;	

	spin_lock_irqsave(&ms->lock, flags);
	list_add_tail(&msg->queue, &ms->queue);
	SPI_DBG("add msg %p to queue\n",msg);
	if (!ms->cur_transfer){
		wake_lock(&ms->wk_lock);
		spi_gpio_set(ms);
		enable_clk();
		mt_spi_next_message(ms);
	}
	spin_unlock_irqrestore(&ms->lock, flags);

	return 0;

out:
	return -1;

}
static irqreturn_t mt_spi_interrupt(int irq, void *dev_id)
{
	struct mt_spi_t *ms = (struct mt_spi_t *)dev_id;
	struct spi_message	*msg;
	struct spi_transfer	*xfer;
	struct mt_chip_conf *chip_config;

	unsigned long flags;
	u32 reg_val,cnt;
	u8 mode,i;	
	spi_rec_time("irqs");
	
	spin_lock_irqsave(&ms->lock,flags);
	xfer = ms->cur_transfer;
	msg = list_entry(ms->queue.next, struct spi_message, queue);

	//                                                    
	reg_val = spi_readl(ms,SPI_STATUS0_REG);	
	SPI_DBG("xfer:0x%p interrupt status:%x\n",xfer,reg_val & 0x3);
//                                                                         
	
	if(unlikely(!msg)){
		printk(KERN_ERR"msg in interrupt %d is NULL pointer. \n",reg_val & 0x3 );		
		goto out;
	}
	if(unlikely(!xfer)){
		printk(KERN_ERR"xfer in interrupt %d is NULL pointer. \n",reg_val & 0x3 );		
		goto out;
	}
	
	chip_config = (struct mt_chip_conf *)msg->state;
	mode = chip_config->com_mod;
//                                                             
//                                           
//                                                                
//                                                                         
	if((reg_val & 0x03) == 0)
		goto out;
	
	if(!msg->is_dma_mapped)
		transfer_dma_unmapping(ms, ms->cur_transfer);
	
	if(is_pause_mode(msg)){
		if( ms->running == INPROGRESS ){
			ms->running = PAUSED;
		}else{
			dev_err(&msg->spi->dev, "Wrong spi status.\n" );
		}
	}else 
		ms->running = IDLE;
	
	if(is_fifo_read(msg) && xfer->rx_buf){		
		cnt = (xfer->len%4)?(xfer->len/4 + 1):(xfer->len/4);
		for(i = 0; i < cnt; i++){			
			reg_val = spi_readl(ms, SPI_RX_DATA_REG); /*                    */	
			SPI_INFO(&msg->spi->dev,"SPI_RX_DATA_REG:0x%x", reg_val);
			*((u32 *)xfer->rx_buf + i) = reg_val;
		}
	}

	msg->actual_length += xfer->len;	

	if(is_last_xfer(msg, xfer)){	
		mt_spi_msg_done(ms, msg, 0);
	}else{	
		ms->cur_transfer = ms->next_transfer;		
		mt_spi_next_xfer(ms, msg);
				
	}

	spin_unlock_irqrestore(&ms->lock, flags);
	return IRQ_HANDLED;
out:
	spin_unlock_irqrestore(&ms->lock, flags);
	SPI_DBG("return IRQ_NONE.\n");	
	return IRQ_NONE;
}


/*                                         */
static int 
mt_do_spi_setup(struct mt_spi_t *ms, struct mt_chip_conf *chip_config)
{
	u32 reg_val;
	
#ifdef SPI_VERBOSE
	u32 speed;
#define SPI_MODULE_CLOCK 134300
	speed= SPI_MODULE_CLOCK/(chip_config->low_time + chip_config->high_time);
	SPI_DBG("mode:%d, speed:%d KHz,CPOL%d,CPHA%d\n", chip_config->com_mod, speed,chip_config->cpol,chip_config->cpha);
#endif
	/*              */
	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val &= ~ SPI_CMD_RST_MASK;
	spi_writel ( ms, SPI_CMD_REG, reg_val );
		
	/*              */
	reg_val = spi_readl ( ms, SPI_CFG0_REG );
	reg_val &= ~ ( SPI_CFG0_SCK_HIGH_MASK |SPI_CFG0_SCK_LOW_MASK );
	reg_val &= ~ ( SPI_CFG0_CS_HOLD_MASK |SPI_CFG0_CS_SETUP_MASK );
	reg_val |= ( (chip_config->high_time-1) << SPI_CFG0_SCK_HIGH_OFFSET );
	reg_val |= ( (chip_config->low_time-1) << SPI_CFG0_SCK_LOW_OFFSET );
	reg_val |= ( (chip_config->holdtime-1) << SPI_CFG0_CS_HOLD_OFFSET );
	reg_val |= ( (chip_config->setuptime-1) << SPI_CFG0_CS_SETUP_OFFSET );
	spi_writel ( ms, SPI_CFG0_REG, reg_val );
	
	reg_val = spi_readl ( ms, SPI_CFG1_REG ); 
	reg_val &= ~(SPI_CFG1_CS_IDLE_MASK);
	reg_val |= ( (chip_config->cs_idletime-1 ) << SPI_CFG1_CS_IDLE_OFFSET );
	
	reg_val &= ~(SPI_CFG1_GET_TICK_DLY_MASK);
	reg_val |= ( ( chip_config->tckdly ) << SPI_CFG1_GET_TICK_DLY_OFFSET );
	spi_writel ( ms, SPI_CFG1_REG, reg_val );

	/*                        */
	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val &= ~ ( SPI_CMD_TX_ENDIAN_MASK | SPI_CMD_RX_ENDIAN_MASK );
	reg_val &= ~ ( SPI_CMD_TXMSBF_MASK| SPI_CMD_RXMSBF_MASK );
	reg_val &= ~ ( SPI_CMD_CPHA_MASK | SPI_CMD_CPOL_MASK );
	reg_val |= ( chip_config->tx_mlsb << SPI_CMD_TXMSBF_OFFSET );
	reg_val |= ( chip_config->rx_mlsb << SPI_CMD_RXMSBF_OFFSET );
	reg_val |= (chip_config->tx_endian << SPI_CMD_TX_ENDIAN_OFFSET );
	reg_val |= (chip_config->rx_endian << SPI_CMD_RX_ENDIAN_OFFSET );
	reg_val |= (chip_config->cpha << SPI_CMD_CPHA_OFFSET );
	reg_val |= (chip_config->cpol << SPI_CMD_CPOL_OFFSET );
	spi_writel(ms, SPI_CMD_REG, reg_val);

	/*              */
	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val &= ~SPI_CMD_PAUSE_EN_MASK;
	reg_val &= ~ SPI_CMD_PAUSE_IE_MASK;
	//                                            
	reg_val |= ( chip_config->pause << SPI_CMD_PAUSE_IE_OFFSET );
	spi_writel ( ms, SPI_CMD_REG, reg_val );

	/*                                  */
	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val &= ~ SPI_CMD_FINISH_IE_MASK;
//                                                                      
	reg_val |= ( 1 << SPI_CMD_FINISH_IE_OFFSET );

	spi_writel ( ms, SPI_CMD_REG, reg_val );
	
	/*                             */
	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val &= ~ SPI_CMD_TX_DMA_MASK;
	reg_val &= ~ SPI_CMD_RX_DMA_MASK;
	spi_writel ( ms, SPI_CMD_REG, reg_val );

	/*                 */
	reg_val = spi_readl ( ms, SPI_CMD_REG );
	reg_val &= ~SPI_CMD_DEASSERT_MASK;
	reg_val |= ( chip_config->deassert << SPI_CMD_DEASSERT_OFFSET );
	spi_writel ( ms, SPI_CMD_REG, reg_val );

	reg_val = spi_readl ( ms, SPI_GMC_SLOW_REG );
	
	/*                       */
	reg_val &= ~SPI_ULTRA_HIGH_EN_MASK;
	reg_val |=  chip_config->ulthigh << SPI_ULTRA_HIGH_EN_OFFSET;

	reg_val &= ~SPI_ULTRA_HIGH_THRESH_MASK; 
	reg_val |= ( chip_config->ulthgh_thrsh << SPI_ULTRA_HIGH_THRESH_OFFSET );

	spi_writel ( ms, SPI_ULTRA_HIGH_REG, reg_val );

	return 0;
}

static int mt_spi_setup ( struct spi_device *spidev )
{
	struct spi_master *master;
	struct mt_spi_t *ms;

	struct mt_chip_conf *chip_config = NULL;

	master = spidev->master;
	ms = spi_master_get_devdata(master);
	
	if(!spidev){
		dev_err( &spidev->dev,"spi device %s: error.\n", 
			dev_name( &spidev->dev ) );
	}
	if(spidev->chip_select >= master->num_chipselect){
		dev_err( &spidev->dev,"spi device chip select excesses \
			the number of master's chipselect number.\n");
		return -EINVAL;
	}
	
	chip_config = ( struct mt_chip_conf * ) spidev->controller_data;
	if ( !chip_config ) {
		chip_config =  ( struct mt_chip_conf *) kzalloc ( 
			sizeof ( struct mt_chip_conf ),  GFP_KERNEL );
		if (!chip_config){
			dev_err( &spidev->dev, 
				" spidev %s: can not get enough memory.\n", 
				dev_name ( &spidev->dev ) );
			return -ENOMEM;
		}
		SPI_DBG("device %s: set default at chip's runtime state\n", 
			dev_name ( &spidev->dev ) );

		chip_config->setuptime = 3;
		chip_config->holdtime = 3;
#if defined(CONFIG_SPI_PEEL_IR)
		chip_config->high_time = 34;
		chip_config->low_time = 34;
#else
		chip_config->high_time = 10;
		chip_config->low_time = 10;
#endif
		chip_config->cs_idletime = 2;
		chip_config->ulthgh_thrsh = 0;

		chip_config->cpol = 0;
		chip_config->cpha = 1;
		
		chip_config->rx_mlsb = 1; 
		chip_config->tx_mlsb = 1;

		chip_config->tx_endian = 0;
		chip_config->rx_endian = 0;

		chip_config->com_mod = DMA_TRANSFER;
		chip_config->pause = 0;
		chip_config->finish_intr = 1;
		chip_config->deassert = 0;
		chip_config->ulthigh = 0;
		chip_config->tckdly = 0;
		
		spidev->controller_data = chip_config;
	}

	SPI_INFO(&spidev->dev,"set up chip config,mode:%d\n", chip_config->com_mod);
	
//                     
//                                                                      
//       
	/*                              */
	
	ms->config = chip_config;
	if( !((chip_config->pause == PAUSE_MODE_ENABLE) || (chip_config->pause == PAUSE_MODE_DISABLE)) ||
		!((chip_config->cpol == SPI_CPOL_0) || (chip_config->cpol == SPI_CPOL_1)) ||
		!((chip_config->cpha == SPI_CPHA_0) || (chip_config->cpha == SPI_CPHA_1)) ||
		!((chip_config->tx_mlsb == SPI_LSB) || (chip_config->tx_mlsb == SPI_MSB)) ||
		!((chip_config->com_mod == FIFO_TRANSFER) || (chip_config->com_mod == DMA_TRANSFER) ||
		 (chip_config->com_mod == OTHER1) || (chip_config->com_mod == OTHER2))){
		return -EINVAL;
	}
	return 0;
}

static void mt_spi_cleanup(struct spi_device *spidev)
{
	struct spi_master *master;
	struct mt_spi_t *ms;

	master=spidev->master;
	ms=spi_master_get_devdata(master);

	SPI_DBG("Calling mt_spi_cleanup.\n");

	spidev->controller_data = NULL;
	spidev->master = NULL;
	
	return;	
}

static int __init mt_spi_probe(struct platform_device *pdev)
{
	int	ret = 0;
	int	irq;
	struct resource *regs;

	struct spi_master *master;
	struct mt_spi_t *ms;

	if(!request_mem_region(pdev->resource[0].start, 
						   pdev->resource[0].end-pdev->resource[0].start + 1, 
						   pdev->name)){
		dev_err(&pdev->dev,"request_mem_region busy.\n");
		return -EBUSY;
	}
	
	regs = platform_get_resource ( pdev,IORESOURCE_MEM, 0 );
	if(!regs){
		dev_err(&pdev->dev,"get resource regs NULL.\n");
		return -ENXIO;
	}

	irq = platform_get_irq(pdev,0);
	if(irq  < 0) {
		dev_err(&pdev->dev,"platform_get_irq error. get invalid irq\n");
		return irq;
	}

	master = spi_alloc_master(&pdev->dev, sizeof(struct mt_spi_t));
	if(!master){
		dev_err(&pdev->dev, " device %s: alloc spi master fail.\n", dev_name ( &pdev->dev ) );
		goto out;
	}
	/*                                                                       */
	master->num_chipselect = 2; 

	master->mode_bits	=	(SPI_CPOL | SPI_CPHA);	
	master->bus_num		=	pdev->id;          
	master->setup 		=	mt_spi_setup;  
	master->transfer	=	mt_spi_transfer;
	master->cleanup		=	mt_spi_cleanup;
	platform_set_drvdata(pdev, master);
	
	ms = spi_master_get_devdata(master);
	ms->regs	=	(void *) (IO_PHYS_TO_VIRT(regs->start));
	ms->pdev	=	pdev;
	ms->irq		=	irq;
	ms->running =	IDLE;
	ms->cur_transfer	= NULL;
	ms->next_transfer	= NULL;
	wake_lock_init (&ms->wk_lock, WAKE_LOCK_SUSPEND, "spi_wakelock");

	spin_lock_init(&ms->lock);
	INIT_LIST_HEAD(&ms->queue);
	
	SPI_INFO(&pdev->dev, "Controller at 0x%p (irq %d)\n", ms->regs, irq);
		
	ret = request_irq(irq, mt_spi_interrupt, IRQF_TRIGGER_LOW, dev_name(&pdev->dev), ms);
	if(ret){
		dev_err(&pdev->dev,"registering interrupt handler fails.\n");
		goto out;
	}

	spi_master_set_devdata(master, ms);
	reset_spi(ms);

	ret = spi_register_master ( master );
	if ( ret ) {
		dev_err(&pdev->dev,"spi_register_master fails.\n" );
		goto out_free;
	} else {
		SPI_DBG("spi register master success.\n" );
//                                           
//                               
//                                                   
		return 0;
	}
out_free:
	free_irq(irq,ms);
out:
	spi_master_put(master);
	return ret;
}

static int __exit mt_spi_remove ( struct platform_device *pdev )
{
	struct mt_spi_t *ms;
	struct spi_message *msg;
	struct spi_master *master = platform_get_drvdata ( pdev );
	if (!master ) {
		dev_err(&pdev->dev, 
			"master %s: is invalid. \n", 
			dev_name ( &pdev->dev ) );
		return -EINVAL;
	}
	ms = spi_master_get_devdata ( master );
	
	list_for_each_entry(msg, &ms->queue, queue) {
		msg->status = -ESHUTDOWN;
		msg->complete(msg->context);
	}
	ms->cur_transfer = NULL;
	ms->running = IDLE;
		
	reset_spi(ms);
	
	free_irq(ms->irq, master);
	spi_unregister_master(master);
	return 0;
}

#ifdef CONFIG_PM
static int 
mt_spi_suspend(struct platform_device *pdev, pm_message_t message )
{
	/*                          
                                       */
//                                                              
	return 0;	
}
static int mt_spi_resume ( struct platform_device *pdev )
{
//                                    
	return 0;
}
#else
#define mt_spi_suspend NULL
#define mt_spi_resume NULL
#endif

struct platform_driver mt_spi_driver = {
	.driver = {
		.name = "mt-spi",
		.owner = THIS_MODULE,
	},
	.probe = mt_spi_probe,
	.suspend = mt_spi_suspend,
	.resume = mt_spi_resume,
	.remove = __exit_p ( mt_spi_remove ),
};
static int __init mt_spi_init ( void )
{
	int ret;
	
	ret = platform_driver_register ( &mt_spi_driver );
	return ret;
}

static void __init mt_spi_exit ( void )
{
	platform_driver_unregister ( &mt_spi_driver );
}

module_init ( mt_spi_init );
module_exit ( mt_spi_exit );

MODULE_DESCRIPTION ( "mt SPI Controller driver" );
MODULE_AUTHOR ( "Ranran Lu <ranran.lu@mediatek.com>" );
MODULE_LICENSE ( "GPL" );
MODULE_ALIAS ( "platform: mt_spi" );
