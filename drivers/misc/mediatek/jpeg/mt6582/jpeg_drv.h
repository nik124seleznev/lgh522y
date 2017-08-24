#include <linux/ioctl.h>                                                                       
                                                                                               
#ifndef __JPEG_DRV_H__                                                                         
#define __JPEG_DRV_H__                                                                         



#define TO_CEIL(x,a) ( ((x) + ((a)-1)) & ~((a)-1) )
#define TO_FLOOR(x,a) ( (x) & ~((a)-1) )
#define TO_MASK(x,a) ( (x) & (a) )

#define DUNUM_MAPPING(value) (((value)-1)&0x3)
                                                                                                               
                                                                                           
                                                                                                               
typedef struct                                                                                                 
{                                                                                                              
    long timeout;                                                                                              
    unsigned int *result;                                                                                      
                                                                                                               
}JPEG_DEC_DRV_OUT;                                                                                             


//                                                                                           




#define MAX_JPEG_COMPONENT	     4
#define MAX_NUM_SCAN		        32

#define D_MAX_JPEG_HW_COMP		   3
//                               
#define D_MAX_JPEG_QTBL			     4
#define D_MAX_JPEG_HW_QTBL		   2
#define D_MAX_JPEG_BLOCK_NUM		10
#define D_HUFF_LOOKAHEAD_BITS	   6	   /*                        */

#define D_DCTSIZE				         8
#define D_JPEG_DRAM_ALIGN_MASK	0xf
#define D_JPEG_DRAM_ALIGN_SIZE	(D_JPEG_DRAM_ALIGN_MASK + 1)

#define D_JPEG_BASELINE			0
#define D_JPEG_DC_REFINE		1
#define D_JPEG_AC_FIRST			2
#define D_JPEG_AC_REFINE		3
#define D_JPEG_DC_FIRST			4

/*                         */
#define D_FLAG_NONE				(0)

#define D_GLOB_PROGRESSIVE		(1 << 0)
#define D_GLOB_NCOMP			(1 << 1)
#define D_GLOB_BLK_W			(1 << 2)
#define D_GLOB_BLK_H			(1 << 3)
#define D_GLOB_PIC_W			(1 << 4)
#define D_GLOB_PIC_H			(1 << 5)
#define D_GLOB_DCHUFF			(1 << 6)	/*         */
#define D_GLOB_ACHUFF			(1 << 7)	/*         */
#define D_GLOB_NZBUFF			(1 << 8)	/*         */
#define D_GLOB_QTBL				(1 << 9)	/*         */
#define D_GLOB_RESTART			(1 << 10)
/*                       */
#define D_SCAN_DATASTART		(1 << 0)
#define D_SCAN_NCOMP			(1 << 1)
#define D_SCAN_COMPIN			(1 << 2)
#define D_SCAN_COMPQ			(1 << 3)
#define D_SCAN_BLKDC			(1 << 4)
#define D_SCAN_BLKAC			(1 << 5)
#define D_SCAN_ALAH				(1 << 6)
#define D_SCAN_SESS				(1 << 7)
#define D_SCAN_LAST				(1 << 8)
#define D_SCAN_ROWSTART			(1 << 9)
#define D_SCAN_ROWEND			(1 << 10)
#define D_SCAN_DCHUFF			(1 << 11)
#define D_SCAN_ACHUFF			(1 << 12)
#define D_SCAN_QTBL				(1 << 13)
#define D_SCAN_NZBUFF			(1 << 14)
/*                     */
#define D_ROW_CURR				(1 << 0)
#define D_ROW_OUTBUF			(1 << 1)
#define D_ROW_OUTBUFSIZE		(1 << 2)
/*                 */
#define D_MCU_OUTADDR			(1 << 0)
#define D_MCU_COEFADDR			(1 << 1)
/*                  */
#define D_MISC_ADDR_X			(1 << 0)
#define D_MISC_ADDR_Y			(1 << 1)


#define M_JPEG_INTERFACE_START() \
	int				i4Ret = (int)E_HWJPG_OK;

#define M_JPEG_INTERFACE_END() \
	return i4Ret


#define HORI 0
#define VERT 1

#define JPEG_ENC_DST_ADDR_OFFSET_MASK (0x0f)





typedef enum _ENUM_JPEG_RET_CODE_T
{
	E_JPG_OK,
	E_JPG_ERR_NONFATAL,
	E_JPG_ERR_FATAL,
	E_JPG_ERR_PARAM,
	E_JPG_ERR_NOT_INITED,
	E_JPG_ERR_ALREADY, // 
	/*         */
	E_JPG_ERR_NOT_JPEG_IMAGE,
	E_JPG_ERR_NOT_A_MARKER,
	E_JPG_ERR_PASS_END,
	E_JPG_ERR_MULTI_SOI,
	E_JPG_ERR_UNSUPPORT_SOF, //  
	E_JPG_ERR_MULTI_SOF,
	E_JPG_ERR_INVALID_SOF,
	E_JPG_ERR_SOS_BEFORE_SOF,
	E_JPG_ERR_INVALID_SOS,
	E_JPG_ERR_INVALID_DHT, //  
	E_JPG_ERR_INVALID_DRI,
	E_JPG_ERR_MULTI_APP1,
	E_JPG_ERR_INVALID_APP0,
	E_JPG_ERR_INVALID_APP1,
	E_JPG_ERR_PARSE_FAILED, //  
	E_JPG_ERR_NOT_PARSED,
	E_JPG_ERR_MULTI_EOI,
	/*           */
	E_JPG_ERR_ALLOC_PARSER,
	E_JPG_ERR_ALLOC_DECODER,
	E_JPG_ERR_ALLOC_MEM, //  
	E_JPG_ERR_ALLOC_FLOW,
	/*         */
	E_JPG_ERR_UNSUPPORT,
	E_JPG_ERR_GENERAL,
	E_JPG_ERR_LAST,
	E_JPG_ERR_DISPLAY_ADDR, //  
	E_JPG_ERR_INVALID_COMPONENT	//         
} JpegDecRetCode;


typedef enum 
{
	E_HWJPG_OK = 0,
	E_HWJPG_BUSY,

	E_HWJPG_ERR_FETCH_TIMEOIT = E_JPG_ERR_LAST + 1,
	E_HWJPG_ERR_SET_BS,
	E_HWJPG_ERR_LOAD_Q,
	E_HWJPG_ERR_QTBL_INDEX,
	E_HWJPG_ERR_QTBL_NUM,

	E_HWJPG_ERR_PARAM,
	E_HWJPG_ERR_TIMEOUT,

	E_HWJPG_ERR_COMP_RANGE,
	E_HWJPG_ERR_TBL_RANGE,
	E_HWJPG_ERR_BLOCK,
	E_HWJPG_ERR_SIZE,
	E_HWJPG_ERR_OUTBUF,

	E_HWJPG_ERR_NULL_SCAN,

	E_HWJPG_ERR_GLOB_NOT_READY,
	E_HWJPG_ERR_SCAN_NOT_READY,
	E_HWJPG_ERR_ROW_NOT_READY,
	E_HWJPG_ERR_MCU_NOT_READY,
	E_HWJPG_ERR_MISC_NOT_READY,
	E_HWJPG_ERR_HUFF_ADDR,
	/*      */
	E_HWJPG_ERR_GENERAL
} JpegDrvDecRetCode;


typedef enum
{
    JDEC_HAL_DEC_MODE_NONE,
    JDEC_HAL_DEC_MODE_BASELINE_PIC,                        //                                              
    JDEC_HAL_DEC_MODE_BASELINE_MCU,                        //                                        
    JDEC_HAL_DEC_MODE_BASELINE_MCU_ROW,                    //                                    
    JDEC_HAL_DEC_MODE_BASELINE_VIDEO_OUTPUT,               
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_MULTI_COLLECT,       //                                                     
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ENHANCE,             //                                                        
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_MULTI_COLLECT,   //                              
    JDEC_HAL_DEC_MODE_PROGRESSIVE_MCU_ROW_ENHANCE,
    JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_MULTI_COLLECT,
    JDEC_HAL_DEC_MODE_PROGRESSIVE_SCAN_ENHANCE
} JpegDrvDecMode;

/*             */
typedef enum _JpegDecFormat
{
	E_JPG_UNKNOWN_FORMAT,
	E_JPG_BASELINE,
	E_JPG_EX_SEQ_HUFFMAN,
	E_JPG_PROGRESSIVE_HUFFMAN,
	E_JPG_EX_SEQ_ARITHMETIC,
	E_JPG_PROGRESSIVE_ARITHMETIC,
	E_JPG_LOSSLESS_HUFFMAN,
	E_JPG_DIFFERENTIAL_SEQ_HUFFMAN,
	E_JPG_DIFF_PROG_HUFFMAN,
	E_JPG_DIFF_LLESS_HUFFMAN,
	E_JPG_RESERVED_FOR_EXTENSIONS,
	E_JPG_LOSSLESS_ARITHMETIC,
	E_JPG_DIFF_SEQ_ARITHMETIC,
	E_JPG_DIFF_PROG_ARITHMETIC,
	E_JPG_UNSUPPORT_FORMAT
} JpegDecFormat;


/*                              */
typedef struct _JpegDecSOFComp
{
	unsigned char		u1CompId;
	unsigned char		u1HSampFactor;
	unsigned char		u1VSampFactor;
	unsigned char		u1QuantTblNo;
} JpegDecSOFComp;

typedef struct _JpegDecSOS
{
	unsigned int		 u4ScanPass;			/*           */
	unsigned char		 u1CompInScan;
	unsigned char		 au1CompNoList[MAX_JPEG_COMPONENT];
	unsigned char		 au1DcId[MAX_JPEG_COMPONENT];
	unsigned char		 au1AcId[MAX_JPEG_COMPONENT];
 	unsigned char    u1Ss, u1Se, u1Ah, u1Al, u1AhAl;        /*            */
	unsigned char*   pu1ScanTableStart;
	unsigned char*   pu1ScanDataStart;
} JpegDecSOS;


/*                      */
typedef struct
{
	unsigned char au1Bits[17];
	unsigned char au1HuffVal[256];
} JpegDecDhtHuffTbl;



/*          */
typedef struct _JpegDecSOF
{
	int 					      fgSOF;	         /*                                          */
	JpegDecFormat   		eJpegFormat;
	unsigned char			  u1DataPrecision;
	unsigned short			u2ImageHeight;
	unsigned short			u2ImageWidth;
	unsigned char			  u1NumComponents;
	unsigned char			  au1MapId2Index[256];
	JpegDecSOFComp			arSofComp[MAX_JPEG_COMPONENT];
} JpegDecSOF;



/*          */
typedef struct 
{
	unsigned int			  u4NumDcTbl;
	unsigned int			  u4NumAcTbl;
	unsigned int			  fgDcTblLoaded;     /*                              */
	unsigned int			  fgAcTblLoaded;     /*                              */
	JpegDecDhtHuffTbl		arDcTbl[4];
	JpegDecDhtHuffTbl		arAcTbl[4];
} JpegDecDHT;

/*          */
typedef struct
{
	/*
                                          
                                           
                                
 */
	unsigned char		aau1Qtbl[4][128];
	unsigned int		afgPrec[4];
	unsigned char		u1NumQ;
	unsigned int		fgDQT;
} JpegDecDQT;


/*                            */
typedef struct 
{
    void             *pvFreePoint;           /*            */
    unsigned int     u4Width;                /*            */
    unsigned int     u4Height;               /*            */
    unsigned int     u4McuNumInRow;          /*            */
    unsigned int     u4McuRows;              /*            */
    unsigned int     u4TotalMcuNum;          /*            */
    unsigned int     aau4SampleFactor[3][2]; /*            */
    unsigned int     au4CompBlock[3];        /*            */
    unsigned int     au4MaxFactor[2];        /*            */
    unsigned int     outputBuffer0[3];
    unsigned int     outputBuffer1[3];                         /*                                                                     */
    unsigned int     au1CoffBuffer[3];       /*            */
    int              fgProgScan;             /*            */
	  /*              */
	  unsigned int     apvNzBuffer[3];         /*            */
	  
} JpegDrvDecFlow;



//                   
typedef enum _JpegDecMode
{
    JPEG_DEC_MODE_NONE,
    JPEG_DEC_MODE_FRAME,                   //                                              
    JPEG_DEC_MODE_DC_LINK,                 //                                              
    JPEG_DEC_MODE_MCU_ROW                 //                                        
} JpegDecMode;


//                         
typedef struct
{

    /*        */
    unsigned int    decodeMode;        //   
    unsigned int    reg_OpMode ;       //  
    unsigned int    regDecDumpEn;
	  unsigned int    totalMCU ;         //  
	  unsigned int    comp0_DU ;

    unsigned int    membershipList ;   //                                                  

    /*                                               */
    unsigned char   lumaHorDecimate;   //   
    unsigned char   lumaVerDecimate;   //  
    unsigned char   cbcrHorDecimate;   //  
    unsigned char   cbcrVerDecimate;   //  
	  
    unsigned int    srcStreamAddrBase;     //  
    unsigned int    srcStreamSize;         //  
    unsigned int    srcStreamAddrWritePtr; //   
                    
    unsigned int    outputBuffer0[3];    //                                                                                                    
    unsigned int    outputBuffer1[3];    //                                                                                                 
    

    //                             
    unsigned int    componentNum;        //    
    unsigned int    componentID[3];      //                                                                                       
    unsigned int    hSamplingFactor[3];  //                                                                                       
    unsigned int    vSamplingFactor[3];  //                                                                                       
    unsigned int    qTableSelector[3];   //                                                     
    
    unsigned int    dma_McuInGroup;       //  
    unsigned int    dma_GroupNum ;        //  
    unsigned int    dma_LastMcuNum ;      //  
    unsigned int    gmcEn ;               //  


    //                                                                                                                             

	  unsigned int	  compImgStride[D_MAX_JPEG_HW_COMP];		   //                                                                              
	  unsigned int	  compMemStride[D_MAX_JPEG_HW_COMP];       //  
	  unsigned int	  compTileBufStride[D_MAX_JPEG_HW_COMP];		         //                                                               


    
    unsigned int    mcuPerRow;            //                                                                                          
    unsigned int    pauseRow_en;          //  
    unsigned int    pauseRowCnt;                   //           
    unsigned int    pauseMCU;             //  
    unsigned int    tileBufRowNum ;               //           
    unsigned int    buffer_Y_PA ;                  //           
    unsigned int    buffer_Cb_PA;                  //           
    unsigned int    buffer_Cr_PA;                  //           
    unsigned int    buffer_Y_row_size ;            //           
    unsigned int    buffer_C_row_size ;            //           

                                                                                                                    
    //                                                                                                                            
    //                                                                                                                                           
    //                                                                                                                                            
    //                                                                                 
                                                                           



}JPEG_DEC_DRV_IN;





typedef struct 
{
    unsigned int    decRowBuf[3];    //  
    unsigned int    pauseMCU;             //  


}JPEG_DEC_CONFIG_ROW; 






typedef struct 
{
    //             
    unsigned int srcStreamAddrBase;                                                                                
    unsigned int srcStreamSize;    
    unsigned int srcStreamAddrWritePtr;                                                                            
    
    unsigned int outputBuffer0[3];
    unsigned int outputBuffer1[3];                         /*                                                                     */    
                                                                                                               
    unsigned int mcuPerRow;                  //                                                                   
    unsigned int totalMcuRows;               //                                                                   
                                                                                                               
    unsigned int compDU[3];              //                                                                   
    unsigned int duPerMCURow[3];          //                                                                                 
    unsigned int dummyDU[3];              //                                                                                 
                                                                                                               
    unsigned int samplingFormat;          //                                                                                        
                                                                                                               
    //                                                                                                         
    unsigned int  componentNum;                                                                                
    unsigned int  componentID[3];          //                                                                  
    unsigned int  hSamplingFactor[3];      //                                                                  
    unsigned int  vSamplingFactor[3];      //                                                                  
    unsigned int  qTableSelector[3];       //                                

/*                                                                               */

    /*        */
	  unsigned int    totalMCU ;
	  unsigned char	  blkNumInMCU;          //                

	  unsigned char   u1MaxHorSample, u1MaxVerSample;			        //                
	  unsigned int	  u4PicWidth, u4PicHeight;				            //                         
	  unsigned int	  compImgStride[D_MAX_JPEG_HW_COMP];		      //                                                               
	  unsigned int	  compTileBufStride[D_MAX_JPEG_HW_COMP];		      //                                                               
    unsigned int    au4PicWidthInBuf[D_MAX_JPEG_HW_COMP];    
    

	  //                             
	  unsigned char	  au1MemberShip[D_MAX_JPEG_BLOCK_NUM];		//                         
	  unsigned char	  aau1Qtbl[D_MAX_JPEG_QTBL][128];				//                   
	  unsigned int	  u4RestartInterval;
	  
	  /*              */
	  unsigned char   u1NumCompsInCurrScan;					            //                                     
	  int             afgCompInScan[D_MAX_JPEG_HW_COMP + 1];
	  //                                                   
	  unsigned char   au1BlkDc[D_MAX_JPEG_BLOCK_NUM];
	  unsigned char   au1BlkAc[D_MAX_JPEG_BLOCK_NUM];
    unsigned char   u1Se;                          /*            */
    unsigned char   u1Ss;                          /*            */
    
	  /*                 */
	  unsigned int	  u4CurrMcuRow;
    unsigned int    u4ModTotalRows;
    
	  
	  /*                                */
	  unsigned char	  u1FirstCompInScan;   
	  
    /*         */
    unsigned char   lumaHorDecimate;      //                                    
    unsigned char   lumaVerDecimate;
    unsigned char   cbcrHorDecimate;
    unsigned char   cbcrVerDecimate;
    unsigned int    srcColorFormat;
    unsigned int    dstColorFormat;
    unsigned int    u4isColorConv;
    unsigned int    u4ds_width[3] ;
    unsigned int    u4ds_height[3] ;
    
    unsigned int    decodeMode;
    unsigned int    gdmaBypassEn;
    unsigned int    regDecDumpEn;

    unsigned int    pauseRow_en;
    unsigned int    pauseRowCnt;
    unsigned int    tileBufRowNum ;
    unsigned int    buffer_Y_PA ;
    unsigned int    buffer_Cb_PA;
    unsigned int    buffer_Cr_PA;
    unsigned int    buffer_Y_row_size ;
    unsigned int    buffer_C_row_size ;
  
} JpegDrvDecConfig;

typedef struct
{
    unsigned int *pChksum;  
} JpegDrvDecResult;



















//                                                                                          


//                        
typedef struct 
{

    unsigned int dstBufferAddr; 
    unsigned int dstBufferSize; 

    unsigned int encWidth;    //                          
    unsigned int encHeight;   //                          

    unsigned char enableEXIF;
    unsigned char allocBuffer;
    //                                                    

    unsigned int encQuality; 
    unsigned int encFormat;
        
    //                
    unsigned int disableGMC;     //                
    unsigned int restartInterval;      
    unsigned int srcBufferAddr;   //              
    unsigned int srcChromaAddr;    
    unsigned int imgStride ;
    unsigned int memStride ;
    unsigned int totalEncDU ;
    unsigned int dstBufAddrOffset; 
    unsigned int dstBufAddrOffsetMask; 
    
}JPEG_ENC_DRV_IN; 





typedef struct                                                                                                 
{                                                                                                              
    long timeout;                                                                                              
    unsigned int *fileSize;                                                                                    
    unsigned int *result;
    unsigned int *cycleCount;                                                                                      
                                                                                                               
}JPEG_ENC_DRV_OUT;                                                                                             
                                                                                                               
typedef struct {                                                                                               
    unsigned long startAddr;//                                                                                 
    unsigned long size;                                                                                        
    unsigned long result;//                                                                                    
} JPEG_PMEM_RANGE;                                                                                             

//                                                                                    


typedef struct
{
    long timeout;
    unsigned int *pFileSize;  
    unsigned int *pResult;
    unsigned int *pCycleCount;
    
} JpegDrvEncResult;






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
//
//                  



//                                                                                      






                                                                                                               
#define JPEG_IOCTL_MAGIC        'x'                                                                            

#if 0                                                                                                               
  #define JPEG_DEC_IOCTL_INIT     _IO  (JPEG_IOCTL_MAGIC, 1)                                                     
  //                                                                                                               
  #define JPEG_DEC_IOCTL_START    _IO  (JPEG_IOCTL_MAGIC, 3)                                                     
  #define JPEG_DEC_IOCTL_WAIT     _IOWR(JPEG_IOCTL_MAGIC, 6, JPEG_DEC_DRV_OUT)                                   
  #define JPEG_DEC_IOCTL_DEINIT   _IO  (JPEG_IOCTL_MAGIC, 8)                                                     
  //                                                                                                             
  //                                                                                                             
  //                                                                                                             
#endif

//                                                                        
#define JPEG_DEC_IOCTL_INIT     _IO    (JPEG_IOCTL_MAGIC,  1)
#define JPEG_DEC_IOCTL_CONFIG   _IOW   (JPEG_IOCTL_MAGIC,  2, JPEG_DEC_DRV_IN)
#define JPEG_DEC_IOCTL_FLOW     _IOW   (JPEG_IOCTL_MAGIC,  3, JpegDrvDecFlow)
#define JPEG_DEC_IOCTL_START    _IO    (JPEG_IOCTL_MAGIC,  4)
#define JPEG_DEC_IOCTL_WAIT     _IOWR  (JPEG_IOCTL_MAGIC,  5, JPEG_DEC_DRV_OUT)
#define JPEG_DEC_IOCTL_DEINIT   _IO    (JPEG_IOCTL_MAGIC,  6)

#define JPEG_DEC_IOCTL_RESET    _IO    (JPEG_IOCTL_MAGIC,  7)
#define JPEG_DEC_IOCTL_CHKSUM   _IOWR  (JPEG_IOCTL_MAGIC,  8, JpegDrvDecResult)
#define JPEG_DEC_IOCTL_BREAK    _IO    (JPEG_IOCTL_MAGIC,  9)
#define JPEG_DEC_IOCTL_RW_REG   _IO    (JPEG_IOCTL_MAGIC, 10)
#define JPEG_DEC_IOCTL_RESUME   _IOW   (JPEG_IOCTL_MAGIC, 11, JPEG_DEC_CONFIG_ROW)

#define JPEG_DEC_IOCTL_DUMP_REG _IO  (JPEG_IOCTL_MAGIC, 30)
//                                                                          



//                                                                        

                                                                                                               
#define JPEG_ENC_IOCTL_INIT     _IO  (JPEG_IOCTL_MAGIC, 11)                                                    
#define JPEG_ENC_IOCTL_CONFIG   _IOW (JPEG_IOCTL_MAGIC, 12, JPEG_ENC_DRV_IN)                                   
#define JPEG_ENC_IOCTL_WAIT     _IOWR(JPEG_IOCTL_MAGIC, 13, JPEG_ENC_DRV_OUT)                                  
#define JPEG_ENC_IOCTL_DEINIT   _IO  (JPEG_IOCTL_MAGIC, 14)                                                    
#define JPEG_ENC_IOCTL_START    _IO  (JPEG_IOCTL_MAGIC, 15)                                                    


#define JPEG_ENC_IOCTL_WARM_RESET   _IO(JPEG_IOCTL_MAGIC, 20)
#define JPEG_ENC_IOCTL_DUMP_REG     _IO(JPEG_IOCTL_MAGIC, 21)
#define JPEG_ENC_IOCTL_RW_REG       _IO(JPEG_IOCTL_MAGIC, 22)

                                                                                                               
#endif                                                                                                         
                                                                                                               
