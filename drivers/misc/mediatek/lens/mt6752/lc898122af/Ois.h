//                                                                                
//
//                               
//                      
//                                                                    
//                     
//                                                     
//                                                                                

#ifdef	OISINI
	#define	OISINI__
#else
	#define	OISINI__		extern
#endif







#ifdef	OISCMD
	#define	OISCMD__
#else
	#define	OISCMD__		extern
#endif


//                          

/*                                                                      */
/*                                                   */
/*                                                  */
/*                                       */
/*                                             */
/*                                         */
/*                                      */
/*                                               */
/*                                                                      */

/*                                                */
//                                                      

#define		USE_INVENSENSE		//           
#ifdef USE_INVENSENSE
//                                          
//                                          
//                                           
			#define		FS_SEL		3		/*                */

//                                       
#endif

/*                                           */
#define		MN_3BSD05P1

/*                                           */
#ifdef	MN_3BSD05P1
 #define	MDL_VER			0x06
 #define	FW_VER			0x03
#endif


/*                                         */
#define		STANDBY_MODE		//             
#define		GAIN_CONT			//                  
//                                             
#define		PWM_BREAK			//                                     

#ifdef	MN_3BSD05P1
 #define		ACTREG_10P2OHM		//            
#endif

#define		DEF_SET				//                         
//                                               
//                                               
//                                              
//                                            
#define		NEUTRAL_CENTER		//                                       
#define		H1COEF_CHANGER			/*                   */
#define		MONITOR_OFF			//                       
//                                                            
//                                           
//                                                        



//               
#define		EXE_END		0x02		//                        
#define		EXE_HXADJ	0x06		//                                       
#define		EXE_HYADJ	0x0A		//                                       
#define		EXE_LXADJ	0x12		//                             
#define		EXE_LYADJ	0x22		//                             
#define		EXE_GXADJ	0x42		//                               
#define		EXE_GYADJ	0x82		//                               
#define		EXE_OCADJ	0x402		//                         
#define		EXE_ERR		0x99		//                  

//              
#define	SUCCESS			0x00		//        
#define	FAILURE			0x01		//        

#ifndef ON
 #define	ON				0x01		//   
 #define	OFF				0x00		//    
#endif
 #define	SPC				0x02		//             

#define	X_DIR			0x00		//            
#define	Y_DIR			0x01		//            
#define	X2_DIR			0x10		//            
#define	Y2_DIR			0x11		//            

#define	NOP_TIME		0.00004166F

#ifdef STANDBY_MODE
 //             
 #define		STB1_ON		0x00		//            
 #define		STB1_OFF	0x01		//             
 #define		STB2_ON		0x02		//            
 #define		STB2_OFF	0x03		//             
 #define		STB3_ON		0x04		//            
 #define		STB3_OFF	0x05		//             
 #define		STB4_ON		0x06		//                                          
 #define		STB4_OFF	0x07		//             
 #define		STB2_OISON	0x08		//                       
 #define		STB2_OISOFF	0x09		//                       
 #define		STB2_AFON	0x0A		//                      
 #define		STB2_AFOFF	0x0B		//                      
#endif


//                     
 #define		DAHLXO_INI		0x0000
 #define		DAHLXB_INI		0xE000
 #define		DAHLYO_INI		0x0000
 #define		DAHLYB_INI		0xE000
 #define		SXGAIN_INI		0x3000
 #define		SYGAIN_INI		0x3000
 #define		HXOFF0Z_INI		0x0000
 #define		HYOFF1Z_INI		0x0000

#ifdef ACTREG_6P5OHM		//                                             
 #define		BIAS_CUR_OIS	0x33		//           
 #define		AMP_GAIN_X		0x05		//    
 #define		AMP_GAIN_Y		0x05		//    

/*          */
 #define		OSC_INI			0x2E		/*          */

/*              */
 #define		RWEXD1_L_AF		0x7FFF		//
 #define		RWEXD2_L_AF		0x1094		//
 #define		RWEXD3_L_AF		0x72BA		//
 #define		FSTCTIME_AF		0xED		//
 #define		FSTMODE_AF		0x02		//
 
 /*                                                           */
 #define		A3_IEXP3		0x3EC0017F
 #define		A1_IEXP1		0x3F180130
 
#endif
#ifdef ACTREG_10P2OHM		//                                              
 #define		BIAS_CUR_OIS	0x33		//           
 #define		AMP_GAIN_X		0x05		//    
 #define		AMP_GAIN_Y		0x05		//    

/*          */
 #define		OSC_INI			0x2E		/*          */

/*              */
  #define		RWEXD1_L_AF		0x7FFF		//
  #define		RWEXD2_L_AF		0x75FE		//
  #define		RWEXD3_L_AF		0x7F32		//
  #define		FSTCTIME_AF		0xF1		//
  #define		FSTMODE_AF		0x00		//

 /*                                                  */
 #define		A3_IEXP3		0x3EC0017F
 #define		A1_IEXP1		0x3F0CCCCD
 
#endif
#ifdef ACTREG_15OHM					//                                              
 #define		BIAS_CUR_OIS	0x22		//           
 #define		AMP_GAIN_X		0x04		//    
 #define		AMP_GAIN_Y		0x04		//    

/*          */
 #define		OSC_INI			0x2E		/*          */

/*              */
 #define		RWEXD1_L_AF		0x7FFF		//
 #define		RWEXD2_L_AF		0x5A00		//
 #define		RWEXD3_L_AF		0x7000		//
 #define		FSTCTIME_AF		0x5F		//
 #define		FSTMODE_AF		0x00		//
 
 /*                                                          */
 #define		A3_IEXP3		0x3EE801CF
 #define		A1_IEXP1		0x3EE801CF
 
#endif

/*                     */
#define		DAHLZB_INI		0x9000
#define		DAHLZO_INI		0x0000
#define		BIAS_CUR_AF		0x00		//      
#define		AMP_GAIN_AF		0x00		//  

//                                   
#define		DGYRO_OFST_XH	0x00
#define		DGYRO_OFST_XL	0x00
#define		DGYRO_OFST_YH	0x00
#define		DGYRO_OFST_YL	0x00

#define		SXGAIN_LOP		0x3000
#define		SYGAIN_LOP		0x3000

#define		TCODEH_ADJ		0x0000

#define		GYRLMT1H		0x3DCCCCCD		//    

#ifdef	CORRECT_1DEG
 #define		GYRLMT3_S1		0x3F19999A		//     
 #define		GYRLMT3_S2		0x3F19999A		//     

 #define		GYRLMT4_S1		0x40400000		//    
 #define		GYRLMT4_S2		0x40400000		//    

 #define		GYRA12_HGH		0x40000000		/*       */
 #define		GYRA12_MID		0x3F800000		/*      */
 #define		GYRA34_HGH		0x3F000000		/*      */
 #define		GYRA34_MID		0x3DCCCCCD		/*      */

 #define		GYRB12_HGH		0x3E4CCCCD		/*       */
 #define		GYRB12_MID		0x3CA3D70A		/*       */
 #define		GYRB34_HGH		0x3CA3D70A		/*       */
 #define		GYRB34_MID		0x3C23D70A		/*        */

#else
 #define		GYRLMT3_S1		0x3ECCCCCD		//     
 #define		GYRLMT3_S2		0x3ECCCCCD		//     

 #define		GYRLMT4_S1		0x40000000		//    
 #define		GYRLMT4_S2		0x40000000		//    

 #define		GYRA12_HGH		0x3FC00000		/*       */
 #define		GYRA12_MID		0x3F800000		/*      */
 #define		GYRA34_HGH		0x3F000000		/*      */
 #define		GYRA34_MID		0x3DCCCCCD		/*      */

 #define		GYRB12_HGH		0x3E4CCCCD		/*       */
 #define		GYRB12_MID		0x3CA3D70A		/*       */
 #define		GYRB34_HGH		0x3CA3D70A		/*       */
 #define		GYRB34_MID		0x3C23D70A		/*        */

#endif


//                         
//                         

#ifdef USE_INVENSENSE
 #ifdef	MN_3BSD05P1
  #define		SXQ_INI			0x3F800000
  #define		SYQ_INI			0xBF800000

  #define		GXGAIN_INI		0xBF147AE1
  #define		GYGAIN_INI		0xBF147AE1

  #define		GYROX_INI		0x45
  #define		GYROY_INI		0x43
  
  #define		GXHY_GYHX		1
 #endif
#endif


/*                                     */
 #define	VAL_SET				0x00		//             
 #define	VAL_FIX				0x01		//              
 #define	VAL_SPC				0x02		//             


struct STFILREG {
	unsigned short	UsRegAdd ;
	unsigned char	UcRegDat ;
} ;													//                    

struct STFILRAM {
	unsigned short	UsRamAdd ;
	unsigned long	UlRamDat ;
} ;													//                         

struct STCMDTBL
{
	unsigned short Cmd ;
	unsigned int UiCmdStf ;
	void ( *UcCmdPtr )( void ) ;
} ;

/*                             */

//                
union	WRDVAL{
	unsigned short	UsWrdVal ;
	unsigned char	UcWrkVal[ 2 ] ;
	struct {
		unsigned char	UcLowVal ;
		unsigned char	UcHigVal ;
	} StWrdVal ;
} ;

typedef union WRDVAL	UnWrdVal ;

union	DWDVAL {
	unsigned long	UlDwdVal ;
	unsigned short	UsDwdVal[ 2 ] ;
	struct {
		unsigned short	UsLowVal ;
		unsigned short	UsHigVal ;
	} StDwdVal ;
	struct {
		unsigned char	UcRamVa0 ;
		unsigned char	UcRamVa1 ;
		unsigned char	UcRamVa2 ;
		unsigned char	UcRamVa3 ;
	} StCdwVal ;
} ;

typedef union DWDVAL	UnDwdVal;

//                 
union	FLTVAL {
	float			SfFltVal ;
	unsigned long	UlLngVal ;
	unsigned short	UsDwdVal[ 2 ] ;
	struct {
		unsigned short	UsLowVal ;
		unsigned short	UsHigVal ;
	} StFltVal ;
} ;

typedef union FLTVAL	UnFltVal ;


typedef struct STADJPAR {
	struct {
		unsigned char	UcAdjPhs ;				//                  

		unsigned short	UsHlxCna ;				//                                    
		unsigned short	UsHlxMax ;				//               
		unsigned short	UsHlxMxa ;				//                                 
		unsigned short	UsHlxMin ;				//               
		unsigned short	UsHlxMna ;				//                                 
		unsigned short	UsHlxGan ;				//                
		unsigned short	UsHlxOff ;				//                  
		unsigned short	UsAdxOff ;				//                      
		unsigned short	UsHlxCen ;				//                  

		unsigned short	UsHlyCna ;				//                                    
		unsigned short	UsHlyMax ;				//               
		unsigned short	UsHlyMxa ;				//                                 
		unsigned short	UsHlyMin ;				//               
		unsigned short	UsHlyMna ;				//                                 
		unsigned short	UsHlyGan ;				//                
		unsigned short	UsHlyOff ;				//                  
		unsigned short	UsAdyOff ;				//                      
		unsigned short	UsHlyCen ;				//                  
	} StHalAdj ;

	struct {
		unsigned short	UsLxgVal ;				//            
		unsigned short	UsLygVal ;				//            
		unsigned short	UsLxgSts ;				//                   
		unsigned short	UsLygSts ;				//                   
	} StLopGan ;

	struct {
		unsigned short	UsGxoVal ;				//                  
		unsigned short	UsGyoVal ;				//                  
		unsigned short	UsGxoSts ;				//                     
		unsigned short	UsGyoSts ;				//                     
	} StGvcOff ;
	
	unsigned char		UcOscVal ;				//          

} stAdjPar ;

OISCMD__	stAdjPar	StAdjPar ;				//                          

OISCMD__	unsigned char	UcOscAdjFlg ;		//                    
  #define	MEASSTR		0x01
  #define	MEASCNT		0x08
  #define	MEASFIX		0x80

OISINI__	unsigned short	UsCntXof ;				/*                       */
OISINI__	unsigned short	UsCntYof ;				/*                       */

OISINI__	unsigned char	UcPwmMod ;				/*          */
#define		PWMMOD_CVL	0x00		//             
#define		PWMMOD_PWM	0x01		//         

#define		INIT_PWMMODE	PWMMOD_CVL		//                    

OISINI__	unsigned char	UcCvrCod ;				/*          */
 #define	CVER122		0x93		 //         
 #define	CVER122A	0xA1		 //          


//                   
OISINI__ void	IniSet( void ) ;													//                     
OISINI__ void	IniSetAf( void ) ;													//                     

OISINI__ void	ClrGyr( unsigned short, unsigned char ); 							   //               
	#define CLR_FRAM0		 	0x01
	#define CLR_FRAM1 			0x02
	#define CLR_ALL_RAM 		0x03
OISINI__ void	BsyWit( unsigned short, unsigned char ) ;				//                   
//                                                          
OISINI__ void	MemClr( unsigned char *, unsigned short ) ;							//                      
OISINI__ void	GyOutSignal( void ) ;									//                                  
OISINI__ void	GyOutSignalCont( void ) ;								//                                     
#ifdef STANDBY_MODE
OISINI__ void	AccWit( unsigned char ) ;								//                  
OISINI__ void	SelectGySleep( unsigned char ) ;						//                          
#endif
#ifdef	GAIN_CONT
OISINI__ void	AutoGainControlSw( unsigned char ) ;							//                     
#endif
OISINI__ void	DrvSw( unsigned char UcDrvSw ) ;						//                             
OISINI__ void	AfDrvSw( unsigned char UcDrvSw ) ;						//                                
OISINI__ void	RamAccFixMod( unsigned char ) ;							//                                     
OISINI__ void	IniPtMovMod( unsigned char ) ;							//                                            
OISINI__ void	ChkCvr( void ) ;													//               
	
OISCMD__ void			SrvCon( unsigned char, unsigned char ) ;					//             
OISCMD__ unsigned short	TneRun( void ) ;											//                                     
OISCMD__ unsigned char	RtnCen( unsigned char ) ;									//                          
OISCMD__ void			OisEna( void ) ;											//                    
OISCMD__ void			OisEnaLin( void ) ;											//                                        
OISCMD__ void			TimPro( void ) ;											//                                 
OISCMD__ void			S2cPro( unsigned char ) ;									//                            

 #ifdef	MN_3BSD05P1
	#define		DIFIL_S2		0x3F7FFE00
 #endif
OISCMD__ void			SetSinWavePara( unsigned char , unsigned char ) ;			//                       
	#define		SINEWAVE	0
	#define		XHALWAVE	1
	#define		YHALWAVE	2
	#define		CIRCWAVE	255
OISCMD__ unsigned char	TneGvc( void ) ;											//                      

OISCMD__ void			SetZsp( unsigned char ) ;									//                                 
OISCMD__ void			OptCen( unsigned char, unsigned short, unsigned short ) ;	//                                           
OISCMD__ void			StbOnnN( unsigned char , unsigned char ) ;					//                                 
#ifdef	MODULE_CALIBRATION
OISCMD__ unsigned char	LopGan( unsigned char ) ;									//                 
#endif
#ifdef STANDBY_MODE
 OISCMD__ void			SetStandby( unsigned char ) ;								/*                 */
#endif
#ifdef	MODULE_CALIBRATION
OISCMD__ unsigned short	OscAdj( void ) ;											/*                      */
#endif

#ifdef	HALLADJ_HW
 #ifdef	MODULE_CALIBRATION
 OISCMD__ unsigned char	LoopGainAdj(   unsigned char );
 #endif
 OISCMD__ unsigned char	BiasOffsetAdj( unsigned char , unsigned char );
#endif
OISCMD__ void			GyrGan( unsigned char , unsigned long , unsigned long ) ;	/*                        */
OISCMD__ void			SetPanTiltMode( unsigned char ) ;							/*                           */
#ifndef	HALLADJ_HW
 OISCMD__ unsigned long	TnePtp( unsigned char, unsigned char ) ;					//                             
 #ifdef	MN_3BSD05P1
	#define		HALL_H_VAL	0x3F800000			/*     */
 #endif
 OISCMD__ unsigned char	TneCen( unsigned char, UnDwdVal ) ;							//                   
 #define		PTP_BEFORE		0
 #define		PTP_AFTER		1
#endif
#ifdef GAIN_CONT
OISCMD__ unsigned char	TriSts( void ) ;													//                                    
#endif
OISCMD__ unsigned char	DrvPwmSw( unsigned char ) ;											//                            
	#define		Mlnp		0					//           
	#define		Mpwm		1					//    
 #ifdef	NEUTRAL_CENTER											//                      
 OISCMD__ unsigned char	TneHvc( void ) ;											//                      
 #endif	//              
OISCMD__ void			SetGcf( unsigned char ) ;									//                                   
OISCMD__	unsigned long	UlH1Coefval ;		//                     
#ifdef H1COEF_CHANGER
 OISCMD__	unsigned char	UcH1LvlMod ;		//                   
 OISCMD__	void			SetH1cMod( unsigned char ) ;								//                                         
 #define		S2MODE		0x40
 #define		ACTMODE		0x80
 #define		MOVMODE		0xFF
#endif
OISCMD__	unsigned short	RdFwVr( void ) ;
void RegWriteA(unsigned short RegAddr, unsigned char RegData);
void RegReadA(unsigned short RegAddr, unsigned char *RegData);
void RamWriteA( unsigned short RamAddr, unsigned short RamData );
void RamReadA( unsigned short RamAddr, void * ReadData );
void RamWrite32A(unsigned short RamAddr, unsigned long RamData );
void RamRead32A(unsigned short RamAddr, void * ReadData );
void WitTim(unsigned short  UsWitTim );
void LC898prtvalue(unsigned short  value );
										//                         
