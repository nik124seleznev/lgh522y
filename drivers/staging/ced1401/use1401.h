/****************************************************************************
** use1401.h
** Copyright (C) Cambridge Electronic Design Ltd, 1992-2010
** Authors: Paul Cox, Tim Bergel, Greg Smith
** See CVS for revisions.
**
** Because the size of a long is different between 32-bit and 64-bit on some
** systems, we avoid this in this interface.
****************************************************************************/
#ifndef __USE1401_H__
#define __USE1401_H__
#include "machine.h"

//                                                                                
//                                                                              
//                                                                       
//                                                                             
#ifdef _IS_WINDOWS_
#ifndef U14_NOT_DLL
#ifdef DLL_USE1401
#define U14API(retType) retType DllExport __stdcall
#else
#define U14API(retType) retType DllImport __stdcall
#endif
#endif

#define U14ERRBASE -500
#define U14LONG long
#endif

#ifdef LINUX
#define U14ERRBASE -1000
#define U14LONG int
#endif

#ifdef _QT
#ifndef U14_NOT_DLL
#undef U14API
#define U14API(retType) retType __declspec(dllimport) __stdcall
#endif
#undef U14LONG
#define U14LONG int
#endif

#ifndef U14API
#define U14API(retType) retType
#endif

#ifndef U14LONG
#define U14LONG long
#endif

//                                                            
#define U14ERR_NOERROR        0             //            

//                                                                                   
#define U14ERR_STD            4              //                        
#define U14ERR_U1401          5              //                
#define U14ERR_PLUS           6              //                    
#define U14ERR_POWER          7              //                    
#define U14ERR_U14012         8              //                     
#define U14ERR_POWER2         9
#define U14ERR_U14013        10
#define U14ERR_POWER3        11

//                                                                         
#define U14ERR(n)             (n+U14ERRBASE)
#define U14ERR_OFF            U14ERR(0)      /*                                */
#define U14ERR_NC             U14ERR(-1)     /*                                */
#define U14ERR_ILL            U14ERR(-2)     /*                                */
#define U14ERR_NOIF           U14ERR(-3)     /*                                */
#define U14ERR_TIME           U14ERR(-4)     /*                                */
#define U14ERR_BADSW          U14ERR(-5)     /*                                */
#define U14ERR_PTIME          U14ERR(-6)     /*                                */
#define U14ERR_NOINT          U14ERR(-7)     /*                                */
#define U14ERR_INUSE          U14ERR(-8)     /*                                */
#define U14ERR_NODMA          U14ERR(-9)     /*                                */
#define U14ERR_BADHAND        U14ERR(-10)    /*                                */
#define U14ERR_BAD1401NUM     U14ERR(-11)    /*                                */

#define U14ERR_NO_SUCH_FN     U14ERR(-20)    /*                                */
#define U14ERR_NO_SUCH_SUBFN  U14ERR(-21)    /*                                */
#define U14ERR_NOOUT          U14ERR(-22)    /*                                */
#define U14ERR_NOIN           U14ERR(-23)    /*                                */
#define U14ERR_STRLEN         U14ERR(-24)    /*                                */
#define U14ERR_ERR_STRLEN     U14ERR(-24)    /*                                */
#define U14ERR_LOCKFAIL       U14ERR(-25)    /*                                */
#define U14ERR_UNLOCKFAIL     U14ERR(-26)    /*                                */
#define U14ERR_ALREADYSET     U14ERR(-27)    /*                                */
#define U14ERR_NOTSET         U14ERR(-28)    /*                                */
#define U14ERR_BADAREA        U14ERR(-29)    /*                                */
#define U14ERR_FAIL           U14ERR(-30)    /*                                */

#define U14ERR_NOFILE         U14ERR(-40)    /*                                */
#define U14ERR_READERR        U14ERR(-41)    /*                                */
#define U14ERR_UNKNOWN        U14ERR(-42)    /*                                */
#define U14ERR_HOSTSPACE      U14ERR(-43)    /*                                */
#define U14ERR_LOCKERR        U14ERR(-44)    /*                                */
#define U14ERR_CLOADERR       U14ERR(-45)    /*                                */

#define U14ERR_TOXXXERR       U14ERR(-60)    /*                                */
#define U14ERR_NO386ENH       U14ERR(-80)    /*                                */
#define U14ERR_NO1401DRIV     U14ERR(-81)    /*                                */
#define U14ERR_DRIVTOOOLD     U14ERR(-82)    /*                                */

#define U14ERR_TIMEOUT        U14ERR(-90)    /*                                */

#define U14ERR_BUFF_SMALL     U14ERR(-100)   /*                                */
#define U14ERR_CBALREADY      U14ERR(-101)   /*                                */
#define U14ERR_BADDEREG       U14ERR(-102)   /*                                */
#define U14ERR_NOMEMORY       U14ERR(-103)   /*                                */

#define U14ERR_DRIVCOMMS      U14ERR(-110)   /*                                */
#define U14ERR_OUTOFMEMORY    U14ERR(-111)   /*                                 */

//                  
#define U14TYPE1401           0           /*                                */
#define U14TYPEPLUS           1           /*                                */
#define U14TYPEU1401          2           /*                                */
#define U14TYPEPOWER          3           /*                                */
#define U14TYPEU14012         4           /*                                */
#define U14TYPEPOWER2         5           /*                                */
#define U14TYPEU14013         6           /*                                */
#define U14TYPEPOWER3         7           /*                                */
#define U14TYPEUNKNOWN        -1          /*                                */

//                                                                

//                              
#define U14TF_USEDMA          1           /*                                */
#define U14TF_MULTIA          2           /*                                */
#define U14TF_FIFO            4           /*                                */
#define U14TF_USB2            8           /*                                */
#define U14TF_NOTIFY          16          /*                                */
#define U14TF_SHORT           32          /*                                */
#define U14TF_PCI2            64          /*                                */
#define U14TF_CIRCTH          128         /*                                */
#define U14TF_DIAG            256         /*                                */
#define U14TF_CIRC14          512         /*                                */

//                                                                         
#define ESZBYTES              0           /*                                */
#define ESZWORDS              1           /*                                */
#define ESZLONGS              2           /*                                */
#define ESZUNKNOWN            0           /*                                */

//                                                                       
#define BYTE_SIZE             1           /*                                */
#define WORD_SIZE             2           /*                                */
#define LONG_SIZE             3           /*                                */

//                               
#define GET_TX_MAXENTRIES  257          /*                              */

#ifdef _IS_WINDOWS_
#pragma pack(1)

typedef struct                          /*                                  */
{                                          /*                               */
   U14LONG physical;
   U14LONG size;
} TXENTRY;

typedef struct TGetTxBlock              /*                                  */
{                                               /*                          */
   U14LONG size;
   U14LONG linear;
   short   seg;
   short   reserved;
   short   avail;                      /*                             */
   short   used;                       /*                        */
   TXENTRY entries[GET_TX_MAXENTRIES];       /*                            */
} TGET_TX_BLOCK;

typedef TGET_TX_BLOCK *LPGET_TX_BLOCK;

#pragma pack()
#endif

#ifdef LINUX
typedef struct                          /*                                  */
{                                       /*                               */
   long long physical;
   long     size;
} TXENTRY;

typedef struct TGetTxBlock              /*                                  */
{                                       /*                          */
   long long linear;                    /*                */
   long     size;                       /*                                                     */
   short    seg;                        /*                                  */
   short    reserved;
   short    avail;                      /*                             */
   short    used;                       /*                        */
   TXENTRY  entries[GET_TX_MAXENTRIES]; /*                            */
} TGET_TX_BLOCK;
#endif

#ifdef __cplusplus
extern "C" {
#endif

U14API(int)   U14WhenToTimeOut(short hand);         //                      
U14API(short) U14PassedTime(int iTime);             //                         

U14API(short) U14LastErrCode(short hand);

U14API(short) U14Open1401(short n1401);
U14API(short) U14Close1401(short hand);
U14API(short) U14Reset1401(short hand);
U14API(short) U14ForceReset(short hand);
U14API(short) U14TypeOf1401(short hand);
U14API(short) U14NameOf1401(short hand, char* pBuf, WORD wMax);

U14API(short) U14Stat1401(short hand);
U14API(short) U14CharCount(short hand);
U14API(short) U14LineCount(short hand);

U14API(short) U14SendString(short hand, const char* pString);
U14API(short) U14GetString(short hand, char* pBuffer, WORD wMaxLen);
U14API(short) U14SendChar(short hand, char cChar);
U14API(short) U14GetChar(short hand, char* pcChar);

U14API(short) U14LdCmd(short hand, const char* command);
U14API(DWORD) U14Ld(short hand, const char* vl, const char* str);

U14API(short) U14SetTransArea(short hand, WORD wArea, void *pvBuff,
                                            DWORD dwLength, short eSz);
U14API(short) U14UnSetTransfer(short hand, WORD wArea);
U14API(short) U14SetTransferEvent(short hand, WORD wArea, BOOL bEvent,
                                  BOOL bToHost, DWORD dwStart, DWORD dwLength);
U14API(int)   U14TestTransferEvent(short hand, WORD wArea);
U14API(int)   U14WaitTransferEvent(short hand, WORD wArea, int msTimeOut);
U14API(short) U14GetTransfer(short hand, TGET_TX_BLOCK *pTransBlock);

U14API(short) U14ToHost(short hand, char* pAddrHost,DWORD dwSize,DWORD dw1401,
                                                            short eSz);
U14API(short) U14To1401(short hand, const char* pAddrHost,DWORD dwSize,DWORD dw1401,
                                                            short eSz);

U14API(short) U14SetCircular(short hand, WORD wArea, BOOL bToHost, void *pvBuff,
                                         DWORD dwLength);

U14API(int)   U14GetCircBlk(short hand, WORD wArea, DWORD *pdwOffs);
U14API(int)   U14FreeCircBlk(short hand, WORD wArea, DWORD dwOffs, DWORD dwSize,
                                         DWORD *pdwOffs);

U14API(short) U14StrToLongs(const char* pszBuff, U14LONG *palNums, short sMaxLongs);
U14API(short) U14LongsFrom1401(short hand, U14LONG *palBuff, short sMaxLongs);

U14API(void)  U14SetTimeout(short hand, int lTimeout);
U14API(int)   U14GetTimeout(short hand);
U14API(short) U14OutBufSpace(short hand);
U14API(int)   U14BaseAddr1401(short hand);
U14API(int)   U14DriverVersion(short hand);
U14API(int)   U14DriverType(short hand);
U14API(short) U14DriverName(short hand, char* pBuf, WORD wMax);
U14API(short) U14GetUserMemorySize(short hand, DWORD *pMemorySize);
U14API(short) U14KillIO1401(short hand);

U14API(short) U14BlkTransState(short hand);
U14API(short) U14StateOf1401(short hand);

U14API(short) U14Grab1401(short hand);
U14API(short) U14Free1401(short hand);
U14API(short) U14Peek1401(short hand, DWORD dwAddr, int nSize, int nRepeats);
U14API(short) U14Poke1401(short hand, DWORD dwAddr, DWORD dwValue, int nSize, int nRepeats);
U14API(short) U14Ramp1401(short hand, DWORD dwAddr, DWORD dwDef, DWORD dwEnable, int nSize, int nRepeats);
U14API(short) U14RampAddr(short hand, DWORD dwDef, DWORD dwEnable, int nSize, int nRepeats);
U14API(short) U14StopDebugLoop(short hand);
U14API(short) U14GetDebugData(short hand, U14LONG *plValue);

U14API(short) U14StartSelfTest(short hand);
U14API(short) U14CheckSelfTest(short hand, U14LONG *pData);
U14API(short) U14TransferFlags(short hand);
U14API(void)  U14GetErrorString(short nErr, char* pStr, WORD wMax);
U14API(int)   U14MonitorRev(short hand);
U14API(void)  U14CloseAll(void);

U14API(short) U14WorkingSet(DWORD dwMinKb, DWORD dwMaxKb);
U14API(int)   U14InitLib(void);

#ifdef __cplusplus
}
#endif

#endif /*                             */
