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
#define		OISINI

//                 
//                
#include	"Ois.h"
#include	"OisFil.h"
#include	"OisDef.h"

//                          
//                         
//                          
void	IniClk( void ) ;		//              
void	IniIop( void ) ;		//                         
void	IniMon( void ) ;		//                                
void	IniSrv( void ) ;		//                               
void	IniGyr( void ) ;		//                                     
void	IniFil( void ) ;		//                                      
void	IniAdj( void ) ;		//                         
void	IniCmd( void ) ;		//                                
void	IniDgy( void ) ;		//                             
void	IniAf( void ) ;			//                        
void	IniPtAve( void ) ;		//                


//                                                                                
//                        
//                   
//                    
//                                        
//                                                     
//                                                                                
void	IniSet( void )
{
	//                                
	IniCmd() ;
	//              
	IniClk() ;
	//                         
	IniIop() ;
	//                            
	IniDgy() ;
	//                                
	IniMon() ;
	//                      
	IniSrv() ;
	//                            
	IniGyr() ;
	//                            
	IniFil() ;
	//                         
	IniAdj() ;

}

//                                                                                
//                          
//                   
//                    
//                                           
//                                                      
//                                                                                
void	IniSetAf( void )
{
	//                                
	IniCmd() ;
	//              
	IniClk() ;
	//                   
	IniAf() ;

}



//                                                                                
//                        
//                   
//                    
//                             
//                                                      
//                                                                                
void	IniClk( void )
{
	ChkCvr() ;								/*           */
	
	/*           */
	UcOscAdjFlg	= 0 ;					//              
	
#ifdef	DEF_SET
	/*          */
	RegWriteA( OSCSTOP,		0x00 ) ;			//       
	RegWriteA( OSCSET,		0x90 ) ;			//               
	RegWriteA( OSCCNTEN,	0x00 ) ;			//                       
#endif
	/*             */
	RegWriteA( CLKON,		0x1F ) ;			//       

#ifdef	USE_EXTCLK_ALL
	RegWriteA( CLKSEL,		0x07 ) ;			//           
#else
 #ifdef	USE_EXTCLK_PWM
	RegWriteA( CLKSEL,		0x01 ) ;			//                
 #else
  #ifdef	DEF_SET
	RegWriteA( CLKSEL,		0x00 ) ;			//        
  #endif
 #endif
#endif
	
#ifdef	USE_EXTCLK_ALL	//      
	RegWriteA( PWMDIV,		0x00 ) ;			//               
	RegWriteA( SRVDIV,		0x00 ) ;			//               
	RegWriteA( GIFDIV,		0x02 ) ;			//                       
	RegWriteA( AFPWMDIV,	0x00 ) ;			//                       
	RegWriteA( OPAFDIV,		0x02 ) ;			//                       
#else
 #ifdef	DEF_SET
	RegWriteA( PWMDIV,		0x00 ) ;			//               
	RegWriteA( SRVDIV,		0x00 ) ;			//               
	RegWriteA( GIFDIV,		0x03 ) ;			//                       
  #ifdef	AF_PWMMODE
	RegWriteA( AFPWMDIV,	0x00 ) ;			//               
  #else
	RegWriteA( AFPWMDIV,	0x02 ) ;			//                       
  #endif
	RegWriteA( OPAFDIV,		0x04 ) ;			//                       
 #endif
#endif
}



//                                                                                
//                        
//                   
//                    
//                                        
//                                                      
//                                                                                
void	IniIop( void )
{
#ifdef	DEF_SET
	/*                 */
	RegWriteA( P0LEV,		0x00 ) ;	//                                                                    
	RegWriteA( P0DIR,		0x00 ) ;	//                                                                  
	/*                */
	RegWriteA( P0PON,		0x0F ) ;	//                                                                  
	RegWriteA( P0PUD,		0x0F ) ;	//                                                                  
#endif
	/*                 */
#ifdef	USE_3WIRE_DGYRO
	RegWriteA( IOP1SEL,		0x02 ); 	//                   
#else
	RegWriteA( IOP1SEL,		0x00 ); 	//                                        
#endif
#ifdef	DEF_SET
	RegWriteA( IOP0SEL,		0x02 ); 	//                   
	RegWriteA( IOP2SEL,		0x02 ); 	//                   
	RegWriteA( IOP3SEL,		0x00 ); 	//                        
	RegWriteA( IOP4SEL,		0x00 ); 	//                     
	RegWriteA( IOP5SEL,		0x00 ); 	//                    
	RegWriteA( DGINSEL,		0x00 ); 	//                              
	RegWriteA( I2CSEL,		0x00 );		//                              
	RegWriteA( DLMODE,		0x00 );		//                    
#endif
	
}

//                                                                                
//                        
//                   
//                    
//                                            
//                                                      
//                                                                                
void	IniDgy( void )
{
 #ifdef USE_INVENSENSE
	unsigned char	UcGrini ;
 #endif
	
	/*           */
	/*           */
	/*           */
	
	/*            */
 #ifdef USE_3WIRE_DGYRO
	RegWriteA( SPIM 	, 0x00 );							//                                                
 #else
	RegWriteA( SPIM 	, 0x01 );							//                                                
 #endif
															//                                       

	/*                   */
	RegWriteA( GRSEL	, 0x01 );							//                                                           

	/*                          */
	RegWriteA( GRINI	, 0x80 );							//                                                                    

 #ifdef USE_INVENSENSE

	RegReadA( GRINI	, &UcGrini );					//                                                                    
	RegWriteA( GRINI, ( UcGrini | SLOWMODE) );		//                                                                    
	
	RegWriteA( GRADR0,	0x6A ) ;					//                   
	RegWriteA( GSETDT,	0x10 ) ;					//                      
	RegWriteA( GRACC,	0x10 ) ;					/*                          */
	AccWit( 0x10 ) ;								/*                            */

	RegWriteA( GRADR0,	0x1B ) ;					//                       
	RegWriteA( GSETDT,	( FS_SEL << 3) ) ;			//                      
	RegWriteA( GRACC,	0x10 ) ;					/*                          */
	AccWit( 0x10 ) ;								/*                            */

	RegReadA( GRINI	, &UcGrini );					//                                                                    
	RegWriteA( GRINI, ( UcGrini & ~SLOWMODE) );		//                                                                    

 #endif
	
	RegWriteA( RDSEL,	0x7C ) ;				//                                             
	
	GyOutSignal() ;
	

}


//                                                                                
//                        
//                   
//                    
//                                               
//                                                      
//                                                                                
void	IniMon( void )
{
	RegWriteA( PWMMONA, 0x00 ) ;				//             
	
	RegWriteA( MONSELA, 0x5C ) ;				//               
	RegWriteA( MONSELB, 0x5D ) ;				//               
	RegWriteA( MONSELC, 0x00 ) ;				//        
	RegWriteA( MONSELD, 0x00 ) ;				//        

	//                
	RegWriteA( WC_PINMON1,	0x00 ) ;			//                       
	RegWriteA( WC_PINMON2,	0x00 ) ;			//         
	RegWriteA( WC_PINMON3,	0x00 ) ;			//         
	RegWriteA( WC_PINMON4,	0x00 ) ;			//         
	/*               */
	RegWriteA( WC_DLYMON11,	0x04 ) ;			//                         
	RegWriteA( WC_DLYMON10,	0x40 ) ;			//                         
	RegWriteA( WC_DLYMON21,	0x04 ) ;			//                         
	RegWriteA( WC_DLYMON20,	0xC0 ) ;			//                         
	RegWriteA( WC_DLYMON31,	0x00 ) ;			//                         
	RegWriteA( WC_DLYMON30,	0x00 ) ;			//                         
	RegWriteA( WC_DLYMON41,	0x00 ) ;			//                         
	RegWriteA( WC_DLYMON40,	0x00 ) ;			//                         

/*         */
	RegWriteA( PWMMONA, 0x80 ) ;				//             
//                                                      
/**/


}

//                                                                                
//                        
//                   
//                    
//                                     
//                                                      
//                                                                                
void	IniSrv( void )
{
	unsigned char	UcStbb0 ;

	UcPwmMod = INIT_PWMMODE ;					//                   

	RegWriteA( WC_EQON,		0x00 ) ;				//                     
	RegWriteA( WC_RAMINITON,0x00 ) ;				//         
	ClrGyr( 0x0000 , CLR_ALL_RAM );					//          
	
	RegWriteA( WH_EQSWX,	0x02 ) ;				//                                                       
	RegWriteA( WH_EQSWY,	0x02 ) ;				//                                                       
	
	RamAccFixMod( OFF ) ;							//                 
	
	/*              */
	RamWrite32A( dm1g, 0x3F800000 ) ;			//       
	RamWrite32A( dm2g, 0x3F800000 ) ;			//       
	RamWrite32A( dm3g, 0x3F800000 ) ;			//       
	RamWrite32A( dm4g, 0x3F800000 ) ;			//       
	
	/*                      */
	RamWrite32A( sxlmta1,   0x3F800000 ) ;			//                            
	RamWrite32A( sylmta1,   0x3F800000 ) ;			//                            
	
	/*                */
	RegWriteA( WH_EMGSTPON,	0x00 ) ;				//                           
	RegWriteA( WH_EMGSTPTMR,0xFF ) ;				//                                  
	
	RamWrite32A( sxemglev,   0x3F800000 ) ;			//                                   
	RamWrite32A( syemglev,   0x3F800000 ) ;			//                                   
	
	/*                      */
	RegWriteA( WH_SMTSRVON,	0x00 ) ;				//                         
#ifdef	USE_EXTCLK_ALL	//      
	RegWriteA( WH_SMTSRVSMP,0x03 ) ;				//                             
	RegWriteA( WH_SMTTMR,	0x00 ) ;				//                                 
#else
	RegWriteA( WH_SMTSRVSMP,0x06 ) ;				//                              
	RegWriteA( WH_SMTTMR,	0x01 ) ;				//                                  
#endif
	
	RamWrite32A( sxsmtav,   0xBC800000 ) ;			//                                         
	RamWrite32A( sysmtav,   0xBC800000 ) ;			//                                         
	RamWrite32A( sxsmtstp,  0x3AE90466 ) ;			//                                    
	RamWrite32A( sysmtstp,  0x3AE90466 ) ;			//                                    
	
	/*                              */
	RegWriteA( WH_HOFCON,	0x11 ) ;				//                
	
	/*       */
	RamWrite32A( sxiexp3,   A3_IEXP3 ) ;			//         
	RamWrite32A( sxiexp2,   0x00000000 ) ;			//         
	RamWrite32A( sxiexp1,   A1_IEXP1 ) ;			//         
	RamWrite32A( sxiexp0,   0x00000000 ) ;			//         
	RamWrite32A( sxiexp,    0x3F800000 ) ;			//         

	RamWrite32A( syiexp3,   A3_IEXP3 ) ;			//         
	RamWrite32A( syiexp2,   0x00000000 ) ;			//         
	RamWrite32A( syiexp1,   A1_IEXP1 ) ;			//         
	RamWrite32A( syiexp0,   0x00000000 ) ;			//         
	RamWrite32A( syiexp,    0x3F800000 ) ;			//         

	/*      */
	RamWrite32A( sxoexp3,   A3_IEXP3 ) ;			//         
	RamWrite32A( sxoexp2,   0x00000000 ) ;			//         
	RamWrite32A( sxoexp1,   A1_IEXP1 ) ;			//         
	RamWrite32A( sxoexp0,   0x00000000 ) ;			//         
	RamWrite32A( sxoexp,    0x3F800000 ) ;			//         

	RamWrite32A( syoexp3,   A3_IEXP3 ) ;			//         
	RamWrite32A( syoexp2,   0x00000000 ) ;			//         
	RamWrite32A( syoexp1,   A1_IEXP1 ) ;			//         
	RamWrite32A( syoexp0,   0x00000000 ) ;			//         
	RamWrite32A( syoexp,    0x3F800000 ) ;			//         
	
	/*           */
#ifdef	DEF_SET
	RegWriteA( WC_SINON,	0x00 ) ;				//                     
	RegWriteA( WC_SINFRQ0,	0x00 ) ;				//         
	RegWriteA( WC_SINFRQ1,	0x60 ) ;				//         
	RegWriteA( WC_SINPHSX,	0x00 ) ;				//         
	RegWriteA( WC_SINPHSY,	0x20 ) ;				//         
	
	/*                  */
	RegWriteA( WC_ADMODE,	0x06 ) ;				//                         
	
	/*              */
	RegWriteA( WC_MESMODE,		0x00 ) ;				//                         
	RegWriteA( WC_MESSINMODE,	0x00 ) ;				//         
	RegWriteA( WC_MESLOOP0,		0x08 ) ;				//         
	RegWriteA( WC_MESLOOP1,		0x02 ) ;				//         
	RegWriteA( WC_MES1ADD0,		0x00 ) ;				//         
	RegWriteA( WC_MES1ADD1,		0x00 ) ;				//         
	RegWriteA( WC_MES2ADD0,		0x00 ) ;				//         
	RegWriteA( WC_MES2ADD1,		0x00 ) ;				//         
	RegWriteA( WC_MESABS,		0x00 ) ;				//         
	RegWriteA( WC_MESWAIT,		0x00 ) ;				//         
	
	/*              */
	RegWriteA( WC_AMJMODE,		0x00 ) ;				//                                   
	
	RegWriteA( WC_AMJLOOP0,		0x08 ) ;				//                         
	RegWriteA( WC_AMJLOOP1,		0x02 ) ;				//         
	RegWriteA( WC_AMJIDL0,		0x02 ) ;				//         
	RegWriteA( WC_AMJIDL1,		0x00 ) ;				//         
	RegWriteA( WC_AMJ1ADD0,		0x00 ) ;				//         
	RegWriteA( WC_AMJ1ADD1,		0x00 ) ;				//         
	RegWriteA( WC_AMJ2ADD0,		0x00 ) ;				//         
	RegWriteA( WC_AMJ2ADD1,		0x00 ) ;				//         
	
	/*           */
	RegWriteA( WC_DPI1ADD0,		0x00 ) ;				//                  
	RegWriteA( WC_DPI1ADD1,		0x00 ) ;				//         
	RegWriteA( WC_DPI2ADD0,		0x00 ) ;				//         
	RegWriteA( WC_DPI2ADD1,		0x00 ) ;				//         
	RegWriteA( WC_DPI3ADD0,		0x00 ) ;				//         
	RegWriteA( WC_DPI3ADD1,		0x00 ) ;				//         
	RegWriteA( WC_DPI4ADD0,		0x00 ) ;				//         
	RegWriteA( WC_DPI4ADD1,		0x00 ) ;				//         
	RegWriteA( WC_DPO1ADD0,		0x00 ) ;				//                  
	RegWriteA( WC_DPO1ADD1,		0x00 ) ;				//         
	RegWriteA( WC_DPO2ADD0,		0x00 ) ;				//         
	RegWriteA( WC_DPO2ADD1,		0x00 ) ;				//         
	RegWriteA( WC_DPO3ADD0,		0x00 ) ;				//         
	RegWriteA( WC_DPO3ADD1,		0x00 ) ;				//         
	RegWriteA( WC_DPO4ADD0,		0x00 ) ;				//         
	RegWriteA( WC_DPO4ADD1,		0x00 ) ;				//         
	RegWriteA( WC_DPON,			0x00 ) ;				//                      
	
	/*                */
	RegWriteA( WC_INTMSK,	0xFF ) ;				//                 
	
#endif
	
	/*            */
	RamAccFixMod( OFF ) ;							//                 

	//                    
	DrvSw( OFF ) ;									/*                           */
	RegWriteA( DRVFC2	, 0x90 );					//                                  
	RegWriteA( DRVSELX	, 0xFF );					//                                           
	RegWriteA( DRVSELY	, 0xFF );					//                                           

#ifdef	PWM_BREAK
 #ifdef	PWM_CAREER_TEST
	RegWriteA( PWMFC,		0x7C ) ;				//                                                                                            
 #else		//               
	if( UcCvrCod == CVER122 ) {
		RegWriteA( PWMFC,   0x2D ) ;					//                                                
	} else {
		RegWriteA( PWMFC,   0x3D ) ;					//                                                
	}
 #endif	//               
#else
	RegWriteA( PWMFC,   0x21 ) ;					//                                               
#endif

#ifdef	USE_VH_SYNC
	RegWriteA( STROBEFC,	0x80 ) ;				//                                
	RegWriteA( STROBEDLYX,	0x00 ) ;				//             
	RegWriteA( STROBEDLYY,	0x00 ) ;				//             
#endif	//           

	RegWriteA( PWMA,    0x00 ) ;					//                       
	RegWriteA( PWMDLYX,  0x04 ) ;					//                             
	RegWriteA( PWMDLYY,  0x04 ) ;					//                             
	
#ifdef	DEF_SET
	RegWriteA( DRVCH1SEL,	0x00 ) ;				//                        
	RegWriteA( DRVCH2SEL,	0x00 ) ;				//                        
	
	RegWriteA( PWMDLYTIMX,	0x00 ) ;				//                   
	RegWriteA( PWMDLYTIMY,	0x00 ) ;				//                   
#endif
	
	if( UcCvrCod == CVER122 ) {
#ifdef	PWM_CAREER_TEST
		RegWriteA( PWMPERIODY,	0xD0 ) ;				//                                                 
		RegWriteA( PWMPERIODY2,	0xD0 ) ;				//                                                 
#else		//               
		RegWriteA( PWMPERIODY,	0x00 ) ;				//                         
		RegWriteA( PWMPERIODY2,	0x00 ) ;				//                         
#endif
	} else {
#ifdef	PWM_CAREER_TEST
		RegWriteA( PWMPERIODX,	0xF2 ) ;				//                         
		RegWriteA( PWMPERIODX2,	0x00 ) ;				//                         
		RegWriteA( PWMPERIODY,	0xF2 ) ;				//                         
		RegWriteA( PWMPERIODY2,	0x00 ) ;				//                         
#else		//               
		RegWriteA( PWMPERIODX,	0x00 ) ;				//                         
		RegWriteA( PWMPERIODX2,	0x00 ) ;				//                         
		RegWriteA( PWMPERIODY,	0x00 ) ;				//                         
		RegWriteA( PWMPERIODY2,	0x00 ) ;				//                         
#endif
	}
	
	/*                            */
	RegWriteA( CVA		, 0xC0 );			//                              

	if( UcCvrCod == CVER122 ) {
		RegWriteA( CVFC 	, 0x22 );			//        
	}

	RegWriteA( CVFC2 	, 0x80 );			//       
	if( UcCvrCod == CVER122 ) {
		RegWriteA( CVSMTHX	, 0x00 );			//                  
		RegWriteA( CVSMTHY	, 0x00 );			//                  
	}

	RegReadA( STBB0 	, &UcStbb0 );		//                                                                                              
	UcStbb0 &= 0x80 ;
	RegWriteA( STBB0, UcStbb0 ) ;			//                   
	
}



//                                                                                
//                        
//                   
//                    
//                                                       
//                                                      
//                                                                                
#ifdef GAIN_CONT
  #define	TRI_LEVEL		0x3A031280		/*        */
  #define	TIMELOW			0x50			/* */
  #define	TIMEHGH			0x05			/* */
 #ifdef	USE_EXTCLK_ALL	//      
  #define	TIMEBSE			0x2F			/*       */
 #else
  #define	TIMEBSE			0x5D			/*        */
 #endif
  #define	MONADR			GXXFZ
  #define	GANADR			gxadj
  #define	XMINGAIN		0x00000000
  #define	XMAXGAIN		0x3F800000
  #define	YMINGAIN		0x00000000
  #define	YMAXGAIN		0x3F800000
  #define	XSTEPUP			0x38D1B717		/*         */
  #define	XSTEPDN			0xBD4CCCCD		/*         */
  #define	YSTEPUP			0x38D1B717		/*         */
  #define	YSTEPDN			0xBD4CCCCD		/*         */
#endif


void	IniGyr( void )
{
	
	
	/*                   */
	RegWriteA( WG_EQSW	, 0x03 );		//                                                         
	
	/*                         */
	
	RegWriteA( WG_SHTON	, 0x10 );		//                                                              
										//                                                                  
										
#ifdef	DEF_SET
	RegWriteA( WG_SHTDLYTMR , 0x00 );	//                       
	RegWriteA( WG_GADSMP, 	  0x00 );	//                        
	RegWriteA( WG_HCHR, 	  0x00 );	//                                          
	RegWriteA( WG_LMT3MOD , 0x00 );		//                                                   
										//                                                              
	RegWriteA( WG_VREFADD , 0x12 );		//                                                                                        
#endif
	RegWriteA( WG_SHTMOD , 0x06 );		//                           

	//        
	RamWrite32A( gxlmt1H, GYRLMT1H ) ;			//       
	RamWrite32A( gylmt1H, GYRLMT1H ) ;			//       

	RamWrite32A( gxlmt3HS0, GYRLMT3_S1 ) ;		//       
	RamWrite32A( gylmt3HS0, GYRLMT3_S1 ) ;		//       
	
	RamWrite32A( gxlmt3HS1, GYRLMT3_S2 ) ;		//       
	RamWrite32A( gylmt3HS1, GYRLMT3_S2 ) ;		//       

	RamWrite32A( gylmt4HS0, GYRLMT4_S1 ) ;		//                            
	RamWrite32A( gxlmt4HS0, GYRLMT4_S1 ) ;		//                            
	
	RamWrite32A( gxlmt4HS1, GYRLMT4_S2 ) ;		//                            
	RamWrite32A( gylmt4HS1, GYRLMT4_S2 ) ;		//                            

	
	/*                    */
	RegWriteA( WG_PANADDA, 		0x12 );		//                            
	RegWriteA( WG_PANADDB, 		0x09 );		//                        
	
	 //         
	RamWrite32A( SttxHis, 	0x00000000 );			//       
	RamWrite32A( SttxaL, 	0x00000000 );			//       
	RamWrite32A( SttxbL, 	0x00000000 );			//       
	RamWrite32A( Sttx12aM, 	GYRA12_MID );	//       
	RamWrite32A( Sttx12aH, 	GYRA12_HGH );	//       
	RamWrite32A( Sttx12bM, 	GYRB12_MID );	//       
	RamWrite32A( Sttx12bH, 	GYRB12_HGH );	//       
	RamWrite32A( Sttx34aM, 	GYRA34_MID );	//       
	RamWrite32A( Sttx34aH, 	GYRA34_HGH );	//       
	RamWrite32A( Sttx34bM, 	GYRB34_MID );	//       
	RamWrite32A( Sttx34bH, 	GYRB34_HGH );	//       
	RamWrite32A( SttyaL, 	0x00000000 );			//       
	RamWrite32A( SttybL, 	0x00000000 );			//       
	RamWrite32A( Stty12aM, 	GYRA12_MID );	//       
	RamWrite32A( Stty12aH, 	GYRA12_HGH );	//       
	RamWrite32A( Stty12bM, 	GYRB12_MID );	//       
	RamWrite32A( Stty12bH, 	GYRB12_HGH );	//       
	RamWrite32A( Stty34aM, 	GYRA34_MID );	//       
	RamWrite32A( Stty34aH, 	GYRA34_HGH );	//       
	RamWrite32A( Stty34bM, 	GYRB34_MID );	//       
	RamWrite32A( Stty34bH, 	GYRB34_HGH );	//       
	
	//          
	RegWriteA( WG_PANLEVABS, 		0x00 );		//       
	
	//                                 

	//                         
	//             
	RegWriteA( WG_PANSTT21JUG0, 	0x00 );		//       
	RegWriteA( WG_PANSTT21JUG1, 	0x00 );		//       
	//             
	RegWriteA( WG_PANSTT31JUG0, 	0x00 );		//       
	RegWriteA( WG_PANSTT31JUG1, 	0x00 );		//       
	//             
	RegWriteA( WG_PANSTT41JUG0, 	0x01 );		//       
	RegWriteA( WG_PANSTT41JUG1, 	0x00 );		//       
	//             
	RegWriteA( WG_PANSTT12JUG0, 	0x00 );		//       
	RegWriteA( WG_PANSTT12JUG1, 	0x07 );		//       
	//             
	RegWriteA( WG_PANSTT13JUG0, 	0x00 );		//       
	RegWriteA( WG_PANSTT13JUG1, 	0x00 );		//       
	//             
	RegWriteA( WG_PANSTT23JUG0, 	0x11 );		//       
	RegWriteA( WG_PANSTT23JUG1, 	0x00 );		//       
	//             
	RegWriteA( WG_PANSTT43JUG0, 	0x00 );		//       
	RegWriteA( WG_PANSTT43JUG1, 	0x00 );		//       
	//             
	RegWriteA( WG_PANSTT34JUG0, 	0x01 );		//       
	RegWriteA( WG_PANSTT34JUG1, 	0x00 );		//       
	//             
	RegWriteA( WG_PANSTT24JUG0, 	0x00 );		//       
	RegWriteA( WG_PANSTT24JUG1, 	0x00 );		//       
	//             
	RegWriteA( WG_PANSTT42JUG0, 	0x44 );		//       
	RegWriteA( WG_PANSTT42JUG1, 	0x04 );		//       

	//            
	RegWriteA( WG_PANSTT1LEVTMR, 	0x00 );		//       
	RegWriteA( WG_PANSTT2LEVTMR, 	0x00 );		//       
	RegWriteA( WG_PANSTT3LEVTMR, 	0x00 );		//       
	RegWriteA( WG_PANSTT4LEVTMR, 	0x03 );		//       
	
	//               
	RegWriteA( WG_PANTRSON0, 		0x11 );		//                                
	
	//              
	IniPtMovMod( OFF ) ;							//                         
	
	//     
	RegWriteA( WG_PANSTTSETILHLD,	0x00 );		//       
	
	
	//                           
	RegWriteA( WG_PANSTT2TMR0,	0x01 );		//       
	RegWriteA( WG_PANSTT2TMR1,	0x00 );		//        
	RegWriteA( WG_PANSTT4TMR0,	0x02 );		//       
	RegWriteA( WG_PANSTT4TMR1,	0x07 );		//        
	
	RegWriteA( WG_PANSTTXXXTH,	0x00 );		//       

#ifdef GAIN_CONT
	RamWrite32A( gxlevlow, TRI_LEVEL );					//              
	RamWrite32A( gylevlow, TRI_LEVEL );					//              
	RamWrite32A( gxadjmin, XMINGAIN );					//                
	RamWrite32A( gxadjmax, XMAXGAIN );					//                
	RamWrite32A( gxadjdn, XSTEPDN );					//             
	RamWrite32A( gxadjup, XSTEPUP );					//             
	RamWrite32A( gyadjmin, YMINGAIN );					//                
	RamWrite32A( gyadjmax, YMAXGAIN );					//                
	RamWrite32A( gyadjdn, YSTEPDN );					//             
	RamWrite32A( gyadjup, YSTEPUP );					//             
	
	RegWriteA( WG_LEVADD, (unsigned char)MONADR );		//                    
	RegWriteA( WG_LEVTMR, 		TIMEBSE );				//                 
	RegWriteA( WG_LEVTMRLOW, 	TIMELOW );				//                  
	RegWriteA( WG_LEVTMRHGH, 	TIMEHGH );				//                  
	RegWriteA( WG_ADJGANADD, (unsigned char)GANADR );		//                       
	RegWriteA( WG_ADJGANGO, 		0x00 );					//                  

	/*              */
//                                                                  
	AutoGainControlSw( ON ) ;							/*                            */
#endif
	
}


//                                                                                
//                        
//                   
//                    
//                                                     
//                                                     
//                                                                                
void	IniFil( void )
{
 	unsigned short	UsAryId ;

	//                                  
	UsAryId	= 0 ;
	while( CsFilReg[ UsAryId ].UsRegAdd != 0xFFFF )
	{
		RegWriteA( CsFilReg[ UsAryId ].UsRegAdd, CsFilReg[ UsAryId ].UcRegDat ) ;
		UsAryId++ ;
	}

	//                             
	UsAryId	= 0 ;
	while( CsFilRam[ UsAryId ].UsRamAdd != 0xFFFF )
	{
		RamWrite32A( CsFilRam[ UsAryId ].UsRamAdd, CsFilRam[ UsAryId ].UlRamDat ) ;
		UsAryId++ ;
	}
	
}



//                                                                                
//                        
//                   
//                    
//                                    
//                                                     
//                                                                                
void	IniAdj( void )
{
	RegWriteA( WC_RAMACCXY, 0x00 ) ;			//                       

	IniPtAve( ) ;								//                
	
	/*     */
	RegWriteA( CMSDAC0, BIAS_CUR_OIS ) ;		//                    
	RegWriteA( OPGSEL0, AMP_GAIN_X ) ;			//                       
	RegWriteA( OPGSEL1, AMP_GAIN_Y ) ;			//                       
	/*    */
	RegWriteA( CMSDAC1, BIAS_CUR_AF ) ;			//                    
	RegWriteA( OPGSEL2, AMP_GAIN_AF ) ;			//                        

	RegWriteA( OSCSET, OSC_INI ) ;				//               
	
	/*                */
	RegWriteA( IZAH,	DGYRO_OFST_XH ) ;	//                             
	RegWriteA( IZAL,	DGYRO_OFST_XL ) ;	//                            
	RegWriteA( IZBH,	DGYRO_OFST_YH ) ;	//                             
	RegWriteA( IZBL,	DGYRO_OFST_YL ) ;	//                            
	
	/*            */
	RamAccFixMod( ON ) ;							//               
	
	/*                        */
	RamWriteA( DAXHLO,		DAHLXO_INI ) ;		//       
	RamWriteA( DAXHLB,		DAHLXB_INI ) ;		//       
	RamWriteA( DAYHLO,		DAHLYO_INI ) ;		//       
	RamWriteA( DAYHLB,		DAHLYB_INI ) ;		//       
	RamWriteA( OFF0Z,		HXOFF0Z_INI ) ;		//       
	RamWriteA( OFF1Z,		HYOFF1Z_INI ) ;		//       
	RamWriteA( sxg,			SXGAIN_INI ) ;		//       
	RamWriteA( syg,			SYGAIN_INI ) ;		//       
//                                                              
//                                                              
//                                             
//                                             

	/*                       */
	RamWriteA( DAZHLO,		DAHLZO_INI ) ;		//       
	RamWriteA( DAZHLB,		DAHLZB_INI ) ;		//       

	/*            */
	RamAccFixMod( OFF ) ;							//                 
	
	RamWrite32A( gxzoom, GXGAIN_INI ) ;		//                                       
	RamWrite32A( gyzoom, GYGAIN_INI ) ;		//                                       

	RamWrite32A( sxq, SXQ_INI ) ;			//                                             
	RamWrite32A( syq, SYQ_INI ) ;			//                                             
	
	if( GXHY_GYHX ){			/*                     */
		RamWrite32A( sxgx, 0x00000000 ) ;			//       
		RamWrite32A( sxgy, 0x3F800000 ) ;			//       
		
		RamWrite32A( sygy, 0x00000000 ) ;			//       
		RamWrite32A( sygx, 0x3F800000 ) ;			//       
	}
	
	SetZsp(0) ;								//                                 
	
	RegWriteA( PWMA 	, 0xC0 );			//                   

	RegWriteA( STBB0 	, 0xDF );			//                                                                                              
	RegWriteA( WC_EQSW	, 0x02 ) ;			//       
	RegWriteA( WC_MESLOOP1	, 0x02 ) ;		//       
	RegWriteA( WC_MESLOOP0	, 0x00 ) ;		//       
	RegWriteA( WC_AMJLOOP1	, 0x02 ) ;		//       
	RegWriteA( WC_AMJLOOP0	, 0x00 ) ;		//       
	
	
	SetPanTiltMode( OFF ) ;					/*              */
	
	SetGcf( 0 ) ;							/*                  */
#ifdef H1COEF_CHANGER
	SetH1cMod( ACTMODE ) ;					/*                        */
#endif
	
	DrvSw( ON ) ;							/*                             */
	
	RegWriteA( WC_EQON, 0x01 ) ;			//                 
}



//                                                                                
//                        
//                   
//                    
//                                               
//                                                     
//                                                                                
void	IniCmd( void )
{

	MemClr( ( unsigned char * )&StAdjPar, sizeof( stAdjPar ) ) ;	//                       
	
}


//                                                                                
//                        
//                   
//                                                                
//                                  
//                                                      
//                                                                                
void	BsyWit( unsigned short	UsTrgAdr, unsigned char	UcTrgDat )
{
	unsigned char	UcFlgVal ;

	RegWriteA( UsTrgAdr, UcTrgDat ) ;	//                         

	UcFlgVal	= 1 ;

	while( UcFlgVal ) {

		RegReadA( UsTrgAdr, &UcFlgVal ) ;
		UcFlgVal	&= 	( UcTrgDat & 0x0F ) ;
	} ;

}


//                                                                                
//                        
//                    
//                                                        
//                                     
//                                                     
//                                                                                
void	MemClr( unsigned char	*NcTgtPtr, unsigned short	UsClrSiz )
{
	unsigned short	UsClrIdx ;

	for ( UsClrIdx = 0 ; UsClrIdx < UsClrSiz ; UsClrIdx++ )
	{
		*NcTgtPtr	= 0 ;
		NcTgtPtr++ ;
	}
}



//                                                                                
//                        
//                   
//                              
//                                   
//                                                     
//                                                                                
/*                                      
 
                                   

                                                                             

                                                       
  
   
  
 */

//                                                                                
//                             
//                   
//                    
//                                           
//                                                      
//                                                                                
void	GyOutSignal( void )
{

	RegWriteA( GRADR0,	GYROX_INI ) ;			//                         
	RegWriteA( GRADR1,	GYROY_INI ) ;			//                         
	
	/*                 */
	RegWriteA( GRSEL	, 0x02 );			//                                                           

}

//                                                                                
//                                 
//                   
//                    
//                                               
//                                                      
//                                                                                
void	GyOutSignalCont( void )
{

	/*                 */
	RegWriteA( GRSEL	, 0x04 );			//                                                           

}

#ifdef STANDBY_MODE
//                                                                                
//                        
//                   
//                                      
//                                 
//                                                      
//                                                                                
void	AccWit( unsigned char UcTrgDat )
{
	unsigned char	UcFlgVal ;

	UcFlgVal	= 1 ;

	while( UcFlgVal ) {
		RegReadA( GRACC, &UcFlgVal ) ;			//       
		UcFlgVal	&= UcTrgDat ;
	} ;

}

//                                                                                
//                               
//                   
//                      
//                                         
//                                                      
//                                                                                
void	SelectGySleep( unsigned char UcSelMode )
{
 #ifdef USE_INVENSENSE
	unsigned char	UcRamIni ;
	unsigned char	UcGrini ;

	if(UcSelMode == ON)
	{
		RegWriteA( WC_EQON, 0x00 ) ;		//                     
		RegWriteA( GRSEL,	0x01 ) ;		/*                           */

		RegReadA( GRINI	, &UcGrini );					//                                                                    
		RegWriteA( GRINI, ( UcGrini | SLOWMODE) );		//                                                                    
		
		RegWriteA( GRADR0,	0x6B ) ;		/*                            */
		RegWriteA( GRACC,	0x01 ) ;		/*                               */
		AccWit( 0x01 ) ;					/*                            */
		RegReadA( GRDAT0H, &UcRamIni ) ;	/*        */
		
		UcRamIni |= 0x40 ;					/*               */
  #ifdef GYROSTBY
		UcRamIni &= ~0x01 ;					/*                                   */
  #endif
		
		RegWriteA( GRADR0,	0x6B ) ;		/*                            */
		RegWriteA( GSETDT,	UcRamIni ) ;	/*                                 */
		RegWriteA( GRACC,	0x10 ) ;		/*                          */
		AccWit( 0x10 ) ;					/*                            */

  #ifdef GYROSTBY
		RegWriteA( GRADR0,	0x6C ) ;		/*                            */
		RegWriteA( GSETDT,	0x07 ) ;		/*                                */
		RegWriteA( GRACC,	0x10 ) ;		/*                          */
		AccWit( 0x10 ) ;					/*                            */
  #endif
	}
	else
	{
  #ifdef GYROSTBY
		RegWriteA( GRADR0,	0x6C ) ;		/*                            */
		RegWriteA( GSETDT,	0x00 ) ;		/*                                 */
		RegWriteA( GRACC,	0x10 ) ;		/*                          */
		AccWit( 0x10 ) ;					/*                            */
  #endif
		RegWriteA( GRADR0,	0x6B ) ;		/*                            */
		RegWriteA( GRACC,	0x01 ) ;		/*                               */
		AccWit( 0x01 ) ;					/*                            */
		RegReadA( GRDAT0H, &UcRamIni ) ;	/*        */
		
		UcRamIni &= ~0x40 ;					/*                 */
  #ifdef GYROSTBY
		UcRamIni |=  0x01 ;					/*             */
  #endif
		
		RegWriteA( GSETDT,	UcRamIni ) ;	//                                 
		RegWriteA( GRACC,	0x10 ) ;		/*                          */
		AccWit( 0x10 ) ;					/*                            */
		
		RegReadA( GRINI	, &UcGrini );					//                                                                             
		RegWriteA( GRINI, ( UcGrini & ~SLOWMODE) );		//                                                                             
		
		GyOutSignal( ) ;					/*                              */
		
		WitTim( 50 ) ;						//          
		
		RegWriteA( WC_EQON, 0x01 ) ;		//                         

		ClrGyr( 0x007F , CLR_FRAM1 );		//                     
	}
 #else									/*           */
	
//                         


	if(UcSelMode == ON)
	{
		RegWriteA( WC_EQON, 0x00 ) ;		//                          
		RegWriteA( GRSEL,	0x01 ) ;		/*                           */
		RegWriteA( GRADR0,	0x4C ) ;		/*                            */
		RegWriteA( GSETDT,	0x02 ) ;		/*                                 */
		RegWriteA( GRACC,	0x10 ) ;		/*                          */
		AccWit( 0x10 ) ;					/*                            */
	}
	else
	{
		RegWriteA( GRADR0,	0x4C ) ;		//                         
		RegWriteA( GSETDT,	0x00 ) ;		//                                 
		RegWriteA( GRACC,	0x10 ) ;		/*                          */
		AccWit( 0x10 ) ;					/*                            */
		GyOutSignal( ) ;					/*                              */
		
		WitTim( 50 ) ;						//          
		
		RegWriteA( WC_EQON, 0x01 ) ;		//                         
		ClrGyr( 0x007F , CLR_FRAM1 );		//                     
	}
 #endif
}
#endif

#ifdef	GAIN_CONT
//                                                                                
//                                   
//                   
//                             
//                                           
//                                                      
//                                                                                
void	AutoGainControlSw( unsigned char UcModeSw )
{

	if( UcModeSw == OFF )
	{
		RegWriteA( WG_ADJGANGXATO, 	0xA0 );					//                 
		RegWriteA( WG_ADJGANGYATO, 	0xA0 );					//                 
		RamWrite32A( GANADR			 , XMAXGAIN ) ;			//             
		RamWrite32A( GANADR | 0x0100 , YMAXGAIN ) ;			//             
	}
	else
	{
		RegWriteA( WG_ADJGANGXATO, 	0xA3 );					//                
		RegWriteA( WG_ADJGANGYATO, 	0xA3 );					//                
	}

}
#endif


//                                                                                
//                        
//                   
//                                                                                    
//                                                                     
//                                       
//                                                      
//                                                                                
void	ClrGyr( unsigned short UsClrFil , unsigned char UcClrMod )
{
	unsigned char	UcRamClr;

	/*                      */
	RegWriteA( WC_RAMDLYMOD1,	(unsigned char)(UsClrFil >> 8) ) ;		//                              
	RegWriteA( WC_RAMDLYMOD0,	(unsigned char)UsClrFil ) ;				//                              

	/*            */
	RegWriteA( WC_RAMINITON	, UcClrMod ) ;	//                                                      
	
	/*                        */
	do{
		RegReadA( WC_RAMINITON, &UcRamClr );
		UcRamClr &= UcClrMod;
	}while( UcRamClr != 0x00 );
}


//                                                                                
//                       
//                   
//                            
//                                            
//                                                      
//                                                                                
void	DrvSw( unsigned char UcDrvSw )
{
	if( UcDrvSw == ON )
	{
		if( UcPwmMod == PWMMOD_CVL ) {
			RegWriteA( DRVFC	, 0xF0 );			//                                       
		} else {
#ifdef	PWM_BREAK
			RegWriteA( DRVFC	, 0x00 );			//                                   
#else
			RegWriteA( DRVFC	, 0xC0 );			//                                  
#endif
		}
	}
	else
	{
		if( UcPwmMod == PWMMOD_CVL ) {
			RegWriteA( DRVFC	, 0x30 );				//                          
		} else {
#ifdef	PWM_BREAK
			RegWriteA( DRVFC	, 0x00 );				//                                   
#else
			RegWriteA( DRVFC	, 0x00 );				//                          
#endif
		}
	}
}

//                                                                                
//                         
//                   
//                            
//                                               
//                                                      
//                                                                                
void	AfDrvSw( unsigned char UcDrvSw )
{
	if( UcDrvSw == ON )
	{
#ifdef	AF_PWMMODE
		RegWriteA( DRVFCAF	, 0x00 );				//                                       
#else
		RegWriteA( DRVFCAF	, 0x20 );				//                                       
#endif
		RegWriteA( CCAAF,   0x80 ) ;				//                      
	}
	else
	{
		RegWriteA( CCAAF,   0x00 ) ;				//                      
	}
}

//                                                                                
//                              
//                   
//                            
//                                                    
//                                                      
//                                                                                
void	RamAccFixMod( unsigned char UcAccMod )
{
	switch ( UcAccMod ) {
		case OFF :
			RegWriteA( WC_RAMACCMOD,	0x00 ) ;		//                               
			break ;
		case ON :
			RegWriteA( WC_RAMACCMOD,	0x31 ) ;		//                             
			break ;
	}
}
	

//                                                                                
//                       
//                   
//                    
//                                       
//                                                      
//                                                                                
void	IniAf( void )
{
	unsigned char	UcStbb0 ;
	
	AfDrvSw( OFF ) ;								/*                       */
#ifdef	AF_PWMMODE
	RegWriteA( DRVFCAF	, 0x00 );					//                                       
#else
	RegWriteA( DRVFCAF	, 0x20 );					//                                       
#endif
	RegWriteA( DRVFC3AF	, 0x00 );					//                       
	RegWriteA( DRVFC4AF	, 0x80 );					//                
	RegWriteA( PWMAAF,    0x00 ) ;					//                      
	RegWriteA( AFFC,   0x80 ) ;						//                  
#ifdef	AF_PWMMODE
	RegWriteA( DRVFC2AF,    0x82 ) ;				//                 
	RegWriteA( DRVCH3SEL,   0x02 ) ;				//                           
	RegWriteA( PWMFCAF,     0x89 ) ;				//                                 
	RegWriteA( PWMPERIODAF, 0xA0 ) ;				//                           
#else
	RegWriteA( DRVFC2AF,    0x00 ) ;				//                 
	RegWriteA( DRVCH3SEL,   0x00 ) ;				//                           
	RegWriteA( PWMFCAF,     0x01 ) ;				//                                 
	RegWriteA( PWMPERIODAF, 0x20 ) ;				//                           
#endif
	RegWriteA( CCFCAF,   0x40 ) ;					//             
	
	RegReadA( STBB0 	, &UcStbb0 );		//                                                                                              
	UcStbb0 &= 0x7F ;
	RegWriteA( STBB0, UcStbb0 ) ;			//                   
	RegWriteA( STBB1, 0x00 ) ;				//                   
	
	/*                    */
	RegWriteA( FSTMODE,		FSTMODE_AF ) ;		//       
	RamWriteA( RWEXD1_L,	RWEXD1_L_AF ) ;		//                                           
	RamWriteA( RWEXD2_L,	RWEXD2_L_AF ) ;		//                                           
	RamWriteA( RWEXD3_L,	RWEXD3_L_AF ) ;		//                                           
	RegWriteA( FSTCTIME,	FSTCTIME_AF ) ;		//         
	RamWriteA( TCODEH,		0x0000 ) ;			//                                           
	
#ifdef	AF_PWMMODE
	RegWriteA( PWMAAF,    0x80 ) ;			//                     
#endif

	UcStbb0 |= 0x80 ;
	RegWriteA( STBB0, UcStbb0 ) ;			//        
	RegWriteA( STBB1	, 0x05 ) ;			//                                                        

	AfDrvSw( ON ) ;								/*                       */
}



//                                                                                
//                          
//                   
//                    
//                                                           
//                                                      
//                                                                                
void	IniPtAve( void )
{
	RegWriteA( WG_PANSTT1DWNSMP0, 0x00 );		//       
	RegWriteA( WG_PANSTT1DWNSMP1, 0x00 );		//       
	RegWriteA( WG_PANSTT2DWNSMP0, 0x90 );		//           
	RegWriteA( WG_PANSTT2DWNSMP1, 0x01 );		//       
	RegWriteA( WG_PANSTT3DWNSMP0, 0x64 );		//           
	RegWriteA( WG_PANSTT3DWNSMP1, 0x00 );		//       
	RegWriteA( WG_PANSTT4DWNSMP0, 0x00 );		//       
	RegWriteA( WG_PANSTT4DWNSMP1, 0x00 );		//       

	RamWrite32A( st1mean, 0x3f800000 );		//       
	RamWrite32A( st2mean, 0x3B23D700 );		//             
	RamWrite32A( st3mean, 0x3C23D700 );		//             
	RamWrite32A( st4mean, 0x3f800000 );		//       
			
}
	
//                                                                                
//                             
//                   
//                                    
//                                                           
//                                                      
//                                                                                
void	IniPtMovMod( unsigned char UcPtMod )
{
	switch ( UcPtMod ) {
		case OFF :
			RegWriteA( WG_PANSTTSETGYRO, 	0x00 );		//       
			RegWriteA( WG_PANSTTSETGAIN, 	0x54 );		//       
			RegWriteA( WG_PANSTTSETISTP, 	0x14 );		//       
			RegWriteA( WG_PANSTTSETIFTR,	0x94 );		//       
			RegWriteA( WG_PANSTTSETLFTR,	0x00 );		//       

			break ;
		case ON :
			RegWriteA( WG_PANSTTSETGYRO, 	0x00 );		//       
			RegWriteA( WG_PANSTTSETGAIN, 	0x00 );		//       
			RegWriteA( WG_PANSTTSETISTP, 	0x14 );		//       
			RegWriteA( WG_PANSTTSETIFTR,	0x94 );		//       
			RegWriteA( WG_PANSTTSETLFTR,	0x00 );		//       
			break ;
	}
}

//                                                                                
//                        
//                   
//                    
//                                   
//                                                      
//                                                                                
void	ChkCvr( void )
{
	RegReadA( CVER ,	&UcCvrCod );		//       
	RegWriteA( MDLREG ,	MDL_VER );			//             
	RegWriteA( VRREG ,	FW_VER );			//               
}

