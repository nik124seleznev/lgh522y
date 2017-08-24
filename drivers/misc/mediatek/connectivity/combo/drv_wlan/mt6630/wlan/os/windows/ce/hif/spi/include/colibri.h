/*
                                                                                                         
*/

/*                     
                                               

*/

/*                                                                              
                                      
                                                                                
*/

/*
                    
  
                      
                                                             
                                                  
                                                                      
  
                   
  
                                                                               
  
                         
                                                
                                   
                                                                           
                   
*/

#ifndef _COLIBRI_H
#define _COLIBRI_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#define CONFIG_ARCH_PXA270      1
#define CONFIG_SPI_8_BIT_MODE   1

#if CONFIG_SPI_8_BIT_MODE
#define SPI_LOOP_COUNT          4
#else				/*                        */
#define SPI_LOOP_COUNT          1
#endif

#define CLKMGR_BASE_ADD         0x41300000
#define SSP1_BASE_ADD           0x41000000
#define SSP2_BASE_ADD           0x41700000
#define SSP3_BASE_ADD           0x41900000
#define GPIO_BASE_ADD           0x40E00000

/*                  */
#define GPIO23_SCLK             23
#define GPIO24_SFRM             24
#define GPIO25_STXD             25
#define GPIO26_SRXD             26

/*                  */
#define SSSR_TNF                   (0x1<<2)
#define SSSR_RNE                   (0x1<<3)
#define SSSR_BSY                   (0x1<<4)

/*                                         */
#define SSCR0_MOD                  (0x1<<31)	/*                    */
#define SSCR0_ACS                  (0x1<<30)	/*                                           */
#define SSCR0_TIM                  (0x1<<23)	/*                                                                           */
#define SSCR0_RIM                  (0x1<<22)	/*                                                                 */
#define SSCR0_NCS                  (0x1<<21)	/*                                                           */
#define SSCR0_EDSS                 (0x1<<20)	/*                                                                                                   */
						     /*                                                                             */
#define SSCR0_SCR_512K             (0x19<<8)	/*                                                    */
#define SSCR0_SCR_128K             (0x67<<8)
#define SSCR0_SSE                  (0x1<<7)	/*                             */
#define SSCR0_ECS                  (0x1<<6)	/*                                                                                     */
#define SSCR0_FRF_TISSP            (0x1<<4)	/*        */
#define SSCR0_FRF_PSP              (0x3<<4)	/*                                   */
#define SSCR0_DSS_14BIT            0xD	/*                      */
#define SSCR0_DSS_32BIT            0xF	/*                      */
#define SSCR0_DSS_8BIT             0x7	/*            */
#define SSCR0_DSS_16BIT            0xF	/*                      */

/*       */
#define SSCR1_THLD_8               (0x8<<10)	/*                                  */
#define SSCR1_TTELP                (0x1<<31)	/*                                                                    */
#define SSCR1_TTE                  (0x1<<30)	/*                                                                                             */
#define SSCR1_EBCE1                (0x1<<29)	/*                                                                                           */
#define SSCR1_SCFR                 (0x1<<28)	/*                                                          */
#define SSCR1_ECRA                 (0x1<<27)	/*                                                                          */
#define SSCR1_ECRB                 (0x1<<26)	/*                        */
#define SSCR1_SCLKDIR              (0x1<<25)	/*                                                                                        */
#define SSCR1_SFRMDIR              (0x1<<24)	/*                                                                                 */
#define SSCR1_RWOT                 (0x1<<23)	/*                                                             */
#define SSCR1_TRAIL                (0x1<<22)	/*                                                                                                                              */
#define SSCR1_TSRE                 (0x1<<21)	/*                                                                                                          */
#define SSCR1_RSRE                 (0x1<<20)	/*                                                                      */
#define SSCR1_TINTE                (0x1<<19)	/*                                                                                                                                                                      */
#define SSCR1_PINTE                (0x1<<18)	/*                                                                                   */
#define SSCR1_STRF                 (0x1<<15)	/*                                                                                              */
#define SSCR1_EFWR                 (0x1<<14)	/*                                                         */
#define SSCR1_MWDS                 (0x1<<5)	/*                                                                     */
#define SSCR1_SPH                  (0x1<<4)	/*                                                                                                                      */
#define SSCR1_SPO                  (0x1<<3)	/*                                                                                                       */
#define SSCR1_LBM                  (0x1<<2)	/*                */
#define SSCR1_TIE                  (0x1<<1)	/*                                               */
#define SSCR1_RIE                  (0x1)	/*                                                                       */

/*                                               */
/*                                                                 
                                          */

/*                            */
#define SSSP_BCE                   (0x1<<23)	/*                                                                                        */
#define SSSP_CSS                   (0x1<<22)	/*                                                                                            */
#define SSSP_TUR                   (0x1<<21)	/*                                                                */
#define SSSP_EOC                   (0x1<<20)	/*                                                                                      */
#define SSSP_TINT                  (0x1<<19)	/*                              */
#define SSSP_PINT                  (0x1<<18)	/*                                               */
#define SSSP_RFL_MASK              (0xf<<12)	/*                                                    */
#define SSSP_TFL_MASK              (0xf<<8)	/*                                                                                    */
#define SSSP_ROR                   (0x1<<7)	/*                                              */
#define SSSP_RFS                   (0x1<<6)	/*                                                  */
#define SSSP_TFS                   (0x1<<5)	/*                                                                               */
#define SSSP_BSY                   (0x1<<4)	/*                                                            */
#define SSSP_RNE                   (0x1<<3)	/*                              */
#define SSSP_TNF                   (0x1<<2)	/*                              */

/*                                        */
#define CLKEN_PWM0_2        (0x1u << 0)
#define CLKEN_PWM1_3        (0x1u << 1)
#define CLKEN_AC97          (0x1u << 2)
#define CLKEN_SSP2          (0x1u << 3)
#define CLKEN_SSP3          (0x1u << 4)
#define CLKEN_STUART        (0x1u << 5)
#define CLKEN_FFUART        (0x1u << 6)
#define CLKEN_BTUART        (0x1u << 7)
#define CLKEN_I2S           (0x1u << 8)
#define CLKEN_OST           (0x1u << 9)
#define CLKEN_USBHOST       (0x1u << 10)
#define CLKEN_USBCLIENT     (0x1u << 11)
#define CLKEN_MMC           (0x1u << 12)
#define CLKEN_ICP           (0x1u << 13)
#define CLKEN_I2C           (0x1u << 14)
#define CLKEN_PWRI2C        (0x1u << 15)
#define CLKEN_LCD           (0x1u << 16)
#define CLKEN_BASEBAND      (0x1u << 17)
#define CLKEN_USIM          (0x1u << 18)
#define CLKEN_KEYPAD        (0x1u << 19)
#define CLKEN_MEMCLOCK      (0x1u << 20)
#define CLKEN_MEMSTICK      (0x1u << 21)
#define CLKEN_MEMC          (0x1u << 22)
#define CLKEN_SSP1          (0x1u << 23)
#define CLKEN_CAMERA        (0x1u << 24)	/*                          */
#define CLKEN_TPM           (0x1u << 25)	/*                                 */

#define SSP_BASE_ADD        SSP1_BASE_ADD	/*           */
#define CLKEN_SSP           CLKEN_SSP1	/*           */
#define GPIO_INTR           27	/*                                    */

#define IO_MASK_INTR        BIT(27)	/*             */
#define FUN_MASK_INTR       BITS(22, 23)	/*               */

/*                                                                              
                                                 
                                                                                
*/

typedef volatile UINT_32 VUINT_32;

/*                          */
typedef struct {
	VUINT_32 osmr0;		/*                           */
	VUINT_32 osmr1;		/*                           */
	VUINT_32 osmr2;		/*                           */
	VUINT_32 osmr3;		/*                           */
	VUINT_32 oscr0;		/*                                         */
	VUINT_32 ossr;		/*                          */
	VUINT_32 ower;		/*                                   */
	VUINT_32 oier;		/*                                    */
	VUINT_32 osnr;		/*                            */
	VUINT_32 reserved1[7];
	VUINT_32 oscr4;		/*                             */
	VUINT_32 oscr5;		/*                              */
	VUINT_32 oscr6;		/*                              */
	VUINT_32 oscr7;		/*                              */
	VUINT_32 oscr8;		/*                              */
	VUINT_32 oscr9;		/*                              */
	VUINT_32 oscr10;	/*                               */
	VUINT_32 oscr11;	/*                               */
	VUINT_32 reserved2[8];
	VUINT_32 osmr4;		/*                           */
	VUINT_32 osmr5;		/*                           */
	VUINT_32 osmr6;		/*                           */
	VUINT_32 osmr7;		/*                           */
	VUINT_32 osmr8;		/*                           */
	VUINT_32 osmr9;		/*                           */
	VUINT_32 osmr10;	/*                            */
	VUINT_32 osmr11;	/*                            */
	VUINT_32 reserved3[8];
	VUINT_32 omcr4;		/*                                   */
	VUINT_32 omcr5;		/*                                   */
	VUINT_32 omcr6;		/*                                   */
	VUINT_32 omcr7;		/*                                   */
	VUINT_32 omcr8;		/*                                   */
	VUINT_32 omcr9;		/*                                   */
	VUINT_32 omcr10;	/*                                    */
	VUINT_32 omcr11;	/*                                    */
} OST_Timer, *P_OST_Timer;


/*                                      */
typedef struct {
	VUINT_32 cccr;		/*                                   */
	VUINT_32 cken;		/*                       */
	VUINT_32 oscc;		/*                                   */
	VUINT_32 ccsr;		/*                            */
} CLK_MGR, *P_CLK_MGR;


/*     */
typedef struct {
	unsigned long sscr0;	/*                        */
	unsigned long sscr1;	/*                        */
	unsigned long sssr;	/*                     */
	unsigned long ssitr;	/*                             */
	unsigned long ssdr;	/*                              */
	unsigned long reserved1;
	unsigned long reserved2;
	unsigned long reserved3;
	unsigned long reserved4;
	unsigned long reserved5;
	unsigned long ssto;	/*                   */
	unsigned long sspsp;	/*                                  */
	unsigned long sstsa;	/*                          */
	unsigned long ssrsa;	/*                          */
} SSP_REG, *P_SSP_REG;


/*                           */
typedef struct {
	VUINT_32 GPLR0;		/*                          */
	VUINT_32 GPLR1;		/*                          */
	VUINT_32 GPLR2;		/*                          */
	VUINT_32 GPDR0;		/*                            */
	VUINT_32 GPDR1;		/*                            */
	VUINT_32 GPDR2;		/*                            */
	VUINT_32 GPSR0;		/*                            */
	VUINT_32 GPSR1;		/*                            */
	VUINT_32 GPSR2;		/*                            */
	VUINT_32 GPCR0;		/*                            */
	VUINT_32 GPCR1;		/*                            */
	VUINT_32 GPCR2;		/*                            */
	VUINT_32 GRER0;		/*                                     */
	VUINT_32 GRER1;		/*                                     */
	VUINT_32 GRER2;		/*                                     */
	VUINT_32 GFER0;		/*                                     */
	VUINT_32 GFER1;		/*                                     */
	VUINT_32 GFER2;		/*                                     */
	VUINT_32 GEDR0;		/*                                */
	VUINT_32 GEDR1;		/*                                */
	VUINT_32 GEDR2;		/*                                */
	VUINT_32 GAFR0_L;	/*                                    */
	VUINT_32 GAFR0_U;	/*                                    */
	VUINT_32 GAFR1_L;	/*                                    */
	VUINT_32 GAFR1_U;	/*                                    */
	VUINT_32 GAFR2_L;	/*                                    */
	VUINT_32 GAFR2_U;	/*                                    */
	VUINT_32 GAFR3_L;	/*                                    */
	VUINT_32 GAFR3_U;	/*                                    */
	VUINT_32 RESERVED1[35];	/*                          */
	VUINT_32 GPLR3;		/*                          */
	VUINT_32 RESERVED2[2];	/*                          */
	VUINT_32 GPDR3;		/*                            */
	VUINT_32 RESERVED3[2];	/*                          */
	VUINT_32 GPSR3;		/*                            */
	VUINT_32 RESERVED4[2];	/*                          */
	VUINT_32 GPCR3;		/*                            */
	VUINT_32 RESERVED5[2];	/*                          */
	VUINT_32 GRER3;		/*                                     */
	VUINT_32 RESERVED6[2];	/*                          */
	VUINT_32 GFER3;		/*                                     */
	VUINT_32 RESERVED7[2];	/*                          */
	VUINT_32 GEDR3;		/*                                */
} GPIO_REG, *P_GPIO_REG;

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

#define SSCR0_DSS_SIZE(x) ((x > 16) ? (((x-17) & BITS(0, 3)) | (1<<20)) : ((x-1) & BITS(0, 3)))

#define WAIT_BUS_CLEAR(cmdRespBuff)    do { \
	    unsigned long u4Value; \
            while ((pVSsp->sssr & BITS(8, 11)) || !(pVSsp->sssr & SSSR_TNF));  \
	    while (pVSsp->sssr & SSSR_RNE) {    \
		u4Value = pVSsp->ssdr;   \
	    }   \
	} while (0)

#define WAIT_BUS_DONE() do { \
            while ((pVSsp->sssr & BITS(8, 11)) || !(pVSsp->sssr & SSSR_TNF));  \
	    while (pVSsp->sssr & SSSR_BSY); \
	} while (0)

#define WAIT_BUS_READY_TX()    do { \
	    while (!(pVSsp->sssr & SSSR_TNF));   \
	} while (0)

#define WAIT_BUS_READY_RX(x)    do { \
	    x = 10000;   \
	    while (!(pVSsp->sssr & SSSR_RNE) && (x--));   \
	} while (0)

/*                                                                              
                                                             
                                                                                
*/

void platformBusInit(IN P_GLUE_INFO_T prGlueInfo);

static INT_32 SpiSetBusWidth(UINT_32 u4BusSize);

static VOID SpiSetOpMode(VOID);

static VOID GpioSetDirectionIn(P_GPIO_REG prGPIO, UINT_32 au4GpioPinArray[]
    );

static VOID GpioSetDirectionOut(P_GPIO_REG prGPIO, UINT_32 au4GpioPinArray[]
    );

static VOID
GpioSetAlternateFn(P_GPIO_REG prGPIO, UINT_32 au4GpioPinArray[], UINT_32 au4AfValueArray[]
    );

static VOID GpioSetFallingEdgeDetectEnable(P_GPIO_REG prGPIO, UINT_32 au4GpioPinArray[]
    );

VOID GPIOConfigBackup(IN VOID);

VOID GPIOConfigRestore(IN VOID);

VOID SpiSendCmd32(UINT_32 u4Cmd, UINT_32 u4Offset, UINT_32 *pU4Value);

void SpiReadWriteData32(UINT_32 u4Cmd, UINT_32 u4Offset, UINT_8 *pucDataBuff, UINT_32 u4Size);

/*                                                                              
                                                
                                                                                
*/

#endif				/*            */
