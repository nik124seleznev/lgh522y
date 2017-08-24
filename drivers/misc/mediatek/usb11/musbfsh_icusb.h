#ifndef _MUSBFSH_ICUSB_H
#define _MUSBFSH_ICUSB_H

enum PHY_VOLTAGE_TYPE
{
	VOL_18 = 0,
	VOL_33,
	VOL_50,
};

enum SESSION_CONTROL_ACTION
{
	STOP_SESSION = 0,
	START_SESSION,
};

enum WAIT_DISCONNECT_DONE_ACTION
{
	WAIT_DISCONNECT_DONE_DFT_ACTION = 0,
};

#define IC_USB_CMD_LEN 255
struct IC_USB_CMD
{
	unsigned char type;
	unsigned char length;
	unsigned char data[IC_USB_CMD_LEN];
};


enum IC_USB_CMD_TYPE
{
	USB11_SESSION_CONTROL = 0,
	USB11_INIT_PHY_BY_VOLTAGE,
	USB11_WAIT_DISCONNECT_DONE,
};

/*                    */
/*                                   */
#define MTK_ICUSB_POWER_AND_RESUME_TIME_NEOGO_SUPPORT
#define MTK_ICUSB_SKIP_SESSION_REQ
#define MTK_ICUSB_SKIP_ENABLE_SESSION
#define MTK_ICUSB_SKIP_MAC_INIT
#define MTK_ICUSB_RESISTOR_CONTROL
#define MTK_ICUSB_HW_DBG
//                              

/*                                       */
//                                
//                                

struct my_attr {
	struct attribute attr;
	int value;
};

/*             */
#define IC_USB_REQ_TYPE_GET_INTERFACE_POWER  0xC0
#define IC_USB_REQ_TYPE_SET_INTERFACE_POWER  0x40
#define IC_USB_REQ_GET_INTERFACE_POWER 0x01
#define IC_USB_REQ_SET_INTERFACE_POWER 0x02
#define IC_USB_WVALUE_POWER_NEGOTIATION 0
#define IC_USB_WINDEX_POWER_NEGOTIATION 0
#define IC_USB_LEN_POWER_NEGOTIATION 2
#define IC_USB_PREFER_CLASSB_ENABLE_BIT 0x80
#define IC_USB_RETRIES_POWER_NEGOTIATION 3
#define IC_USB_CLASSB (1<<1)
#define IC_USB_CLASSC (1<<2)
#define IC_USB_CURRENT 100		//                                 


/*                   */
#define IC_USB_REQ_TYPE_GET_INTERFACE_RESUME_TIME  0xC0
#define IC_USB_REQ_GET_INTERFACE_RESUME_TIME 0x03
#define IC_USB_WVALUE_RESUME_TIME_NEGOTIATION 0
#define IC_USB_WINDEX_RESUME_TIME_NEGOTIATION 0
#define IC_USB_LEN_RESUME_TIME_NEGOTIATION 3
#define IC_USB_RETRIES_RESUME_TIME_NEGOTIATION 3


//                      
//                 
//                                 
//                                                     
//                                  
//
//                     

#define PREFER_VOL_STS_SHIFT (0)
#define PREFER_VOL_STS_MSK (0x3)

#define PREFER_VOL_NOT_INITED  0x0
#define PREFER_VOL_PWR_NEG_FAIL 0x1
#define PREFER_VOL_PWR_NEG_OK 0x2


#define PREFER_VOL_CLASS_SHIFT (8)
#define PREFER_VOL_CLASS_MSK (0xff)

#define USB_PORT1_STS_SHIFT (24)
#define USB_PORT1_STS_MSK (0xf)

#define USB_PORT1_DISCONNECTING 0x0
#define USB_PORT1_DISCONNECT_DONE 0x1
#define USB_PORT1_CONNECT 0x2

#endif
