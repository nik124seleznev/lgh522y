#ifndef __SW_TX_POWER__
#define __SW_TX_POWER__

/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */

//         
#define ENABLE_SW_TX_POWER_2G_NONE      0x00000000
#define ENABLE_SW_TX_POWER_2G_TABLE0    0x00000001
#define ENABLE_SW_TX_POWER_2G_TABLE1    0x00000002
#define ENABLE_SW_TX_POWER_2G_TABLEX    0x00000003

#define ENABLE_SW_TX_POWER_3G_NONE      0x00000000
#define ENABLE_SW_TX_POWER_3G_TABLE0    0x00000004
#define ENABLE_SW_TX_POWER_3G_TABLE1    0x00000008
#define ENABLE_SW_TX_POWER_3G_TABLEX    0x0000000C

#define ENABLE_SW_TX_POWER_4G_NONE      0x00000000
#define ENABLE_SW_TX_POWER_4G_TABLE0    0x00000010
#define ENABLE_SW_TX_POWER_4G_TABLE1    0x00000020
#define ENABLE_SW_TX_POWER_4G_TABLEX    0x00000030

#define MODE_SWTP(v,x,y)    ((ENABLE_SW_TX_POWER_##v)|(ENABLE_SW_TX_POWER_##x)|(ENABLE_SW_TX_POWER_##y))

#define	SWTP_SUPER_MODE    0x00001FFF
#define SWTP_NORMAL_MODE   0x000001FF

#define SWTP_MODE_ON       0x00000001
#define SWTP_MODE_OFF      0x00000000

//                                                     
#define SWTP_2_RF_CON

enum {
    SWTP_CTRL_USER_SET0,
    SWTP_CTRL_USER_SET1,
    SWTP_CTRL_USER_SET2,
    SWTP_CTRL_USER_SET3,
    SWTP_CTRL_USER_SET4,
    SWTP_CTRL_USER_SET5,
    SWTP_CTRL_USER_SET6,
    SWTP_CTRL_USER_SET7,
    SWTP_CTRL_USER_SET8,
    SWTP_CTRL_USER_SET9,
    SWTP_CTRL_USER_SET10,
    SWTP_CTRL_USER_SET11,
    SWTP_CTRL_USER_SET12,
    SWTP_CTRL_USER_SET13,
    SWTP_CTRL_USER_SET14,
    SWTP_CTRL_USER_SET15,
    SWTP_CTRL_USER_SET16,
    SWTP_CTRL_USER_SET17,
    SWTP_CTRL_USER_SET18,
    SWTP_CTRL_USER_SET19,
    SWTP_CTRL_USER_SET20,
    SWTP_CTRL_USER_SET21,
    SWTP_CTRL_USER_SET22,
    SWTP_CTRL_USER_SET23,
    SWTP_CTRL_USER_SET24,
    SWTP_CTRL_USER_SET25,
    SWTP_CTRL_USER_SET26,
    SWTP_CTRL_USER_SET27,
    SWTP_CTRL_USER_SET28,
    SWTP_CTRL_USER_SET29,
    SWTP_CTRL_USER_SET30,
    SWTP_CTRL_USER_SET31,
    SWTP_CTRL_USER_SET32,
    SWTP_CTRL_USER_SET33,
    SWTP_CTRL_USER_SET34,
    SWTP_CTRL_USER_SET35,
    SWTP_CTRL_USER_SET36,
    SWTP_CTRL_USER_SET37,
    SWTP_CTRL_USER_SET38,
    SWTP_CTRL_USER_SET39,
    SWTP_CTRL_USER_SET40,
    SWTP_CTRL_USER_SET41,
    SWTP_CTRL_USER_SET42,
    SWTP_CTRL_USER_SET43,
    SWTP_CTRL_USER_SET44,
    SWTP_CTRL_USER_SET45,
    SWTP_CTRL_USER_SET46,
    SWTP_CTRL_USER_SET47,
    SWTP_CTRL_USER_SET48,
    SWTP_CTRL_USER_SET49,
    SWTP_CTRL_USER_SET50,
    SWTP_CTRL_USER_SET51,
    SWTP_CTRL_USER_SET52,
    SWTP_CTRL_USER_SET53,
    SWTP_CTRL_USER_SET54,
    SWTP_CTRL_USER_SET55,
    SWTP_CTRL_USER_SET56,
    SWTP_CTRL_USER_SET57,
    SWTP_CTRL_USER_SET58,
    SWTP_CTRL_USER_SET59,
    SWTP_CTRL_USER_SET60,
    SWTP_CTRL_USER_SET61,
    SWTP_CTRL_USER_SET62,
    SWTP_CTRL_USER_SET63,	
    SWTP_CTRL_SUPER_SET,
    SWTP_CTRL_MAX_STATE 		
};

typedef struct swtp_state {
    unsigned int enable;
    unsigned int mode;
    unsigned int setvalue;
} swtp_state_type;

extern int switch_MD1_Tx_Power(unsigned int mode);
extern int switch_MD2_Tx_Power(unsigned int mode);

#endif //                
