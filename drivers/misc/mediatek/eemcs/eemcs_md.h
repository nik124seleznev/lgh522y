#ifndef __EEMCS_MD_H__
#define __EEMCS_MD_H__

#include "eemcs_kal.h"

#define NORMAL_BOOT_ID 0
#define META_BOOT_ID 1
#define UART_MAX_PORT_NUM 8


/*                                               */
enum {
     CCCI_MD_MSG_BOOT_READY          = 0xFAF50001,
     CCCI_MD_MSG_BOOT_UP             = 0xFAF50002,
     CCCI_MD_MSG_EXCEPTION           = 0xFAF50003,
     CCCI_MD_MSG_RESET               = 0xFAF50004,
     CCCI_MD_MSG_RESET_RETRY         = 0xFAF50005,
     CCCI_MD_MSG_READY_TO_RESET      = 0xFAF50006,
     CCCI_MD_MSG_BOOT_TIMEOUT        = 0xFAF50007,
     CCCI_MD_MSG_STOP_MD_REQUEST     = 0xFAF50008,
     CCCI_MD_MSG_START_MD_REQUEST    = 0xFAF50009,
     CCCI_MD_MSG_ENTER_FLIGHT_MODE   = 0xFAF5000A,
     CCCI_MD_MSG_LEAVE_FLIGHT_MODE   = 0xFAF5000B,
     CCCI_MD_MSG_POWER_ON_REQUEST    = 0xFAF5000C,
     CCCI_MD_MSG_POWER_DOWN_REQUEST  = 0xFAF5000D,
     CCCI_MD_MSG_SEND_BATTERY_INFO   = 0xFAF5000E,
     CCCI_MD_MSG_NOTIFY              = 0xFAF5000F,
     CCCI_MD_MSG_STORE_NVRAM_MD_TYPE = 0xFAF50010,
};

typedef enum LOGGING_MODE_e {
    MODE_UNKNOWN = -1,      //   
    MODE_IDLE,              //  
    MODE_USB,               //  
    MODE_SD,                //  
    MODE_POLLING,           //  
    MODE_WAITSD,            //  
} LOGGING_MODE;

typedef struct RUNTIME_BUFF_st {
    KAL_UINT32 len;
    KAL_UINT8  buf[0];
} RUNTIME_BUFF;

typedef struct MODEM_RUNTIME_st {
    int Prefix;             //       
    int Platform_L;         //                                        
    int Platform_H;
    int DriverVersion;      //                        
    int BootChannel;        //                                  
    int BootingStartID;     //                                               
    int BootAttributes;     //                                         
    int BootReadyID;        //                                            
    int MdlogShareMemBase;
    int MdlogShareMemSize; 
    int PcmShareMemBase;
    int PcmShareMemSize;
    int UartPortNum;
    int UartShareMemBase[UART_MAX_PORT_NUM];
    int UartShareMemSize[UART_MAX_PORT_NUM];    
    int FileShareMemBase;
    int FileShareMemSize;
    int RpcShareMemBase;
    int RpcShareMemSize;	
    int PmicShareMemBase;
    int PmicShareMemSize;
    int ExceShareMemBase;
    int ExceShareMemSize;   //                    
    int SysShareMemBase;
    int SysShareMemSize;
    int IPCShareMemBase;
    int IPCShareMemSize;
    int CheckSum;
    int Postfix;            //       
} MODEM_RUNTIME;

KAL_UINT32 eemcs_md_gen_runtime_data(void **data);
void eemcs_md_destroy_runtime_data(void *runtime_data);

#endif //               
