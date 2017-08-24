#ifdef __cplusplus
extern "C" {
#endif


#define MaxFindFileClusNum 100

#define DEBUG_KDUMP
#ifdef DEBUG_KDUMP
#define DBGKDUMP_PRINTK printk
#endif

#ifndef UINT32
typedef unsigned int UINT32 ; 
#endif

#ifndef INT32
typedef signed int INT32 ; 
#endif


#ifndef BOOL
typedef unsigned int BOOL ; 
#endif

#ifndef WORD
typedef unsigned short WORD ; 
#endif

#ifndef DWORD
typedef unsigned int DWORD ; 
#endif

#ifndef BYTE
typedef unsigned char BYTE ; 
#endif

#ifndef PBYTE
typedef unsigned char* PBYTE ; 
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef enum  
{
    WRITE_FILE_DIRECT = 0,
    FORMAT_BEF_WRITE = 1
}FileWriteType;



typedef enum FileSysType 
{
    FAT_16 = 0,
    FAT_32 = 1
}FATType;

typedef struct 
{
    DWORD   BPB_BytsPerSec;
    DWORD   BPB_SecPerClus;
    DWORD   BPB_RsvdSecCnt;
    DWORD   BPB_NumFATs;
    DWORD   BPB_FATSz;
    DWORD   BPB_RootEntCnt;
    DWORD   BPB_RootClus;
	DWORD   BPB_TotSec;
    FATType	FileSysType;
    DWORD   BootStartSec;
    DWORD   FATStartSec;
    DWORD   RootDirStartSec;
    DWORD   ClusStartSec;
} FAT_Para;



typedef struct {
    BYTE    name[11];            //          
    BYTE    attr;                //                                           
    BYTE    NTflags;             //    
    BYTE    createdTimeMsec;     //                                            
    WORD    createdTime;         //                      
    WORD    createdDate;         //                      
    WORD    lastAccessDate;      //                         
    WORD    clusFirstHigh;       //                           
    WORD    time;                //                         
    WORD    date;                //                         
    WORD    clusFirst;           //                          
    DWORD   size;                //                   
} DirEntry;

typedef struct {
    BYTE    seqNum;              //                
    BYTE    name1[10];           //                                         
    BYTE    attr;                //                         
    BYTE    NTflags;             //                       
    BYTE    checksum;            //                          
    BYTE    name2[12];           //                                        
    WORD    clusFirst;           //                                      
    BYTE    name3[4];            //                                      
} LfnEntry;

#define buf_size 64*1024      //                             

typedef struct {
	BYTE   FileBuffer[buf_size];	//                                                        
	BYTE   FATBuffer[512];		//          
	DWORD  BufferLen;			//                                 
	DWORD  TotalLen;			//                  
	DWORD  PrevClusterNum;		//                    
	DWORD  CurrClusterNum;		//                       
	DWORD  FATSector;			//                          
	DWORD  CheckSum;			//                            
	BOOL   DiskFull;
} FileHandler;

extern BOOL MSDC_Init(void);
extern BOOL MSDC_DeInit(void);
extern BOOL OpenDumpFile_sd(FileHandler *pFileHandler);
extern BOOL WriteDumpFile_sd(FileHandler *pFileHandler, BYTE *Ptr, DWORD Length, DWORD Total);
extern BOOL CloseDumpFile_sd(FileHandler *pFileHandler);

BOOL WriteSectorToSD(UINT32 sector_addr, PBYTE pdBuf, INT32 blockLen) ;
BOOL ReadSectorFromSD(UINT32 sector_addr,PBYTE pdBuf,INT32 blockLen) ;
#ifdef __cplusplus
}
#endif
