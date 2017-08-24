#ifndef _IOCTL_H_
#define _IOCTL_H_

struct bcm_rdm_buffer {
	unsigned long Register;
	unsigned long Length;
} __packed;

struct bcm_wrm_buffer {
	unsigned long Register;
	unsigned long Length;
	unsigned char Data[4];
} __packed;

struct bcm_ioctl_buffer {
	void __user *InputBuffer;
	unsigned long InputLength;
	void __user *OutputBuffer;
	unsigned long OutputLength;
} __packed;

struct bcm_gpio_info {
	unsigned int uiGpioNumber; /*                    */
	unsigned int uiGpioValue; /*                    */
} __packed;

struct bcm_user_thread_req {
	/*                           */
	/*                            */
	unsigned int ThreadState;
} __packed;

#define LED_THREAD_ACTIVATION_REQ  1
#define BCM_IOCTL				'k'
#define IOCTL_SEND_CONTROL_MESSAGE		_IOW(BCM_IOCTL,	0x801, int)
#define IOCTL_BCM_REGISTER_WRITE		_IOW(BCM_IOCTL, 0x802, int)
#define IOCTL_BCM_REGISTER_READ			_IOR(BCM_IOCTL, 0x803, int)
#define IOCTL_BCM_COMMON_MEMORY_WRITE		_IOW(BCM_IOCTL, 0x804, int)
#define IOCTL_BCM_COMMON_MEMORY_READ		_IOR(BCM_IOCTL, 0x805, int)
#define IOCTL_GET_CONTROL_MESSAGE		_IOR(BCM_IOCTL,	0x806, int)
#define IOCTL_BCM_FIRMWARE_DOWNLOAD		_IOW(BCM_IOCTL, 0x807, int)
#define IOCTL_BCM_SET_SEND_VCID			_IOW(BCM_IOCTL,	0x808, int)
#define IOCTL_BCM_SWITCH_TRANSFER_MODE		_IOW(BCM_IOCTL, 0x809, int)
#define IOCTL_LINK_REQ				_IOW(BCM_IOCTL, 0x80A, int)
#define IOCTL_RSSI_LEVEL_REQ			_IOW(BCM_IOCTL, 0x80B, int)
#define IOCTL_IDLE_REQ				_IOW(BCM_IOCTL, 0x80C, int)
#define IOCTL_SS_INFO_REQ			_IOW(BCM_IOCTL, 0x80D, int)
#define IOCTL_GET_STATISTICS_POINTER		_IOW(BCM_IOCTL, 0x80E, int)
#define IOCTL_CM_REQUEST			_IOW(BCM_IOCTL, 0x80F, int)
#define IOCTL_INIT_PARAM_REQ			_IOW(BCM_IOCTL, 0x810, int)
#define IOCTL_MAC_ADDR_REQ			_IOW(BCM_IOCTL, 0x811, int)
#define IOCTL_MAC_ADDR_RESP			_IOWR(BCM_IOCTL, 0x812, int)
#define IOCTL_CLASSIFICATION_RULE		_IOW(BCM_IOCTL, 0x813, char)
#define IOCTL_CLOSE_NOTIFICATION		_IO(BCM_IOCTL, 0x814)
#define IOCTL_LINK_UP				_IO(BCM_IOCTL, 0x815)
#define IOCTL_LINK_DOWN				_IO(BCM_IOCTL, 0x816, struct bcm_ioctl_buffer)
#define IOCTL_CHIP_RESET			_IO(BCM_IOCTL, 0x816)
#define IOCTL_CINR_LEVEL_REQ			_IOW(BCM_IOCTL, 0x817, char)
#define IOCTL_WTM_CONTROL_REQ			_IOW(BCM_IOCTL, 0x817, char)
#define IOCTL_BE_BUCKET_SIZE			_IOW(BCM_IOCTL, 0x818, unsigned long)
#define IOCTL_RTPS_BUCKET_SIZE			_IOW(BCM_IOCTL, 0x819, unsigned long)
#define IOCTL_QOS_THRESHOLD			_IOW(BCM_IOCTL, 0x820, unsigned long)
#define IOCTL_DUMP_PACKET_INFO			_IO(BCM_IOCTL, 0x821)
#define IOCTL_GET_PACK_INFO			_IOR(BCM_IOCTL, 0x823, int)
#define IOCTL_BCM_GET_DRIVER_VERSION		_IOR(BCM_IOCTL, 0x829, int)
#define IOCTL_BCM_GET_CURRENT_STATUS		_IOW(BCM_IOCTL, 0x828, int)
#define IOCTL_BCM_GPIO_SET_REQUEST		_IOW(BCM_IOCTL, 0x82A, int)
#define IOCTL_BCM_GPIO_STATUS_REQUEST		_IOW(BCM_IOCTL, 0x82b, int)
#define IOCTL_BCM_GET_DSX_INDICATION		_IOR(BCM_IOCTL, 0x854, int)
#define IOCTL_BCM_BUFFER_DOWNLOAD_START		_IOW(BCM_IOCTL, 0x855, int)
#define IOCTL_BCM_BUFFER_DOWNLOAD		_IOW(BCM_IOCTL, 0x856, int)
#define IOCTL_BCM_BUFFER_DOWNLOAD_STOP		_IOW(BCM_IOCTL, 0x857, int)
#define IOCTL_BCM_REGISTER_WRITE_PRIVATE	_IOW(BCM_IOCTL, 0x826, char)
#define IOCTL_BCM_REGISTER_READ_PRIVATE		_IOW(BCM_IOCTL, 0x827, char)
#define IOCTL_BCM_SET_DEBUG			_IOW(BCM_IOCTL, 0x824, struct bcm_ioctl_buffer)
#define IOCTL_BCM_EEPROM_REGISTER_WRITE		_IOW(BCM_IOCTL, 0x858, int)
#define IOCTL_BCM_EEPROM_REGISTER_READ		_IOR(BCM_IOCTL, 0x859, int)
#define IOCTL_BCM_WAKE_UP_DEVICE_FROM_IDLE	_IOR(BCM_IOCTL, 0x860, int)
#define IOCTL_BCM_SET_MAC_TRACING		_IOW(BCM_IOCTL, 0x82c, int)
#define IOCTL_BCM_GET_HOST_MIBS			_IOW(BCM_IOCTL, 0x853, int)
#define IOCTL_BCM_NVM_READ			_IOR(BCM_IOCTL, 0x861, int)
#define IOCTL_BCM_NVM_WRITE			_IOW(BCM_IOCTL, 0x862, int)
#define IOCTL_BCM_GET_NVM_SIZE			_IOR(BCM_IOCTL, 0x863, int)
#define IOCTL_BCM_CAL_INIT			_IOR(BCM_IOCTL, 0x864, int)
#define IOCTL_BCM_BULK_WRM			_IOW(BCM_IOCTL, 0x90B, int)
#define IOCTL_BCM_FLASH2X_SECTION_READ		_IOR(BCM_IOCTL, 0x865, int)
#define IOCTL_BCM_FLASH2X_SECTION_WRITE		_IOW(BCM_IOCTL, 0x866, int)
#define IOCTL_BCM_GET_FLASH2X_SECTION_BITMAP	_IOR(BCM_IOCTL, 0x867, int)
#define IOCTL_BCM_SET_ACTIVE_SECTION		_IOW(BCM_IOCTL, 0x868, int)
#define	IOCTL_BCM_IDENTIFY_ACTIVE_SECTION	_IO(BCM_IOCTL, 0x869)
#define IOCTL_BCM_COPY_SECTION			_IOW(BCM_IOCTL, 0x870, int)
#define	IOCTL_BCM_GET_FLASH_CS_INFO		_IOR(BCM_IOCTL, 0x871, int)
#define IOCTL_BCM_SELECT_DSD			_IOW(BCM_IOCTL, 0x872, int)
#define IOCTL_BCM_NVM_RAW_READ			_IOR(BCM_IOCTL, 0x875, int)
#define IOCTL_BCM_CNTRLMSG_MASK			_IOW(BCM_IOCTL, 0x874, int)
#define IOCTL_BCM_GET_DEVICE_DRIVER_INFO	_IOR(BCM_IOCTL, 0x877, int)
#define IOCTL_BCM_TIME_SINCE_NET_ENTRY		_IOR(BCM_IOCTL, 0x876, int)
#define BCM_LED_THREAD_STATE_CHANGE_REQ		_IOW(BCM_IOCTL, 0x878, int)
#define IOCTL_BCM_GPIO_MULTI_REQUEST		_IOW(BCM_IOCTL, 0x82D, struct bcm_ioctl_buffer)
#define IOCTL_BCM_GPIO_MODE_REQUEST		_IOW(BCM_IOCTL, 0x82E, struct bcm_ioctl_buffer)

enum bcm_interface_type {
	BCM_MII,
	BCM_CARDBUS,
	BCM_USB,
	BCM_SDIO,
	BCM_PCMCIA
};

struct bcm_driver_info {
	enum bcm_nvm_type	u32NVMType;
	unsigned int		MaxRDMBufferSize;
	enum bcm_interface_type	u32InterfaceType;
	unsigned int		u32DSDStartOffset;
	unsigned int		u32RxAlignmentCorrection;
	unsigned int		u32Reserved[10];
};

struct bcm_nvm_readwrite {
	void __user *pBuffer;
	uint32_t  uiOffset;
	uint32_t uiNumBytes;
	bool bVerify;
};

struct bcm_bulk_wrm_buffer {
	unsigned long Register;
	unsigned long SwapEndian;
	unsigned long Values[1];
};

enum bcm_flash2x_section_val {
	NO_SECTION_VAL = 0, /*                                                              */
	ISO_IMAGE1,
	ISO_IMAGE2,
	DSD0,
	DSD1,
	DSD2,
	VSA0,
	VSA1,
	VSA2,
	SCSI,
	CONTROL_SECTION,
	ISO_IMAGE1_PART2,
	ISO_IMAGE1_PART3,
	ISO_IMAGE2_PART2,
	ISO_IMAGE2_PART3,
	TOTAL_SECTIONS
};

/*
                                             
 */
struct bcm_flash2x_readwrite {
	enum bcm_flash2x_section_val Section; /*                                      */
	u32 offset;	     /*                        */
	u32 numOfBytes;	     /*                     */
	u32 bVerify;
	void __user *pDataBuff;	     /*                            */
};

/*
                                                          
                                                   
                                                          
                                                             
 */

struct bcm_flash2x_copy_section {
	enum bcm_flash2x_section_val SrcSection;
	enum bcm_flash2x_section_val DstSection;
	u32 offset;
	u32 numOfBytes;
};

/*
                                                         
                                                         
                          
                                                            
                                                       
                                           
                                                                 
                         
 */

struct bcm_flash2x_bitmap {
	unsigned char ISO_IMAGE1;
	unsigned char ISO_IMAGE2;
	unsigned char DSD0;
	unsigned char DSD1;
	unsigned char DSD2;
	unsigned char VSA0;
	unsigned char VSA1;
	unsigned char VSA2;
	unsigned char SCSI;
	unsigned char CONTROL_SECTION;
	/*                         */
	unsigned char Reserved0;
	unsigned char Reserved1;
	unsigned char Reserved2;
};

struct bcm_time_elapsed {
	u64 ul64TimeElapsedSinceNetEntry;
	u32 uiReserved[4];
};

enum {
	WIMAX_IDX = 0,  /*                                                                    */
	HOST_IDX,	/*                                                                   */
	MAX_IDX
};

struct bcm_gpio_multi_info {
	unsigned int uiGPIOCommand; /*                         */
	unsigned int uiGPIOMask;    /*                                              */
	unsigned int uiGPIOValue;   /*                                            */
} __packed;

struct bcm_gpio_multi_mode {
	unsigned int uiGPIOMode;    /*                               */
	unsigned int uiGPIOMask;    /*                       */
} __packed;

#endif
