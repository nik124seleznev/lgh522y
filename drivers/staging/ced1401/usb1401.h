/* usb1401.h
 Header file for the CED 1401 USB device driver for Linux
 Copyright (C) 2010 Cambridge Electronic Design Ltd
 Author Greg P Smith (greg@ced.co.uk)

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef __USB1401_H__
#define __USB1401_H__
#include "use1401.h"
#include "ced_ioctl.h"

#ifndef UINT
#define UINT unsigned int
#endif

//                                                                                  
//                                                                                   
//                                                                                   
//                                                                                   
//                             
#define TYPEUNKNOWN        -1             //          
#define TYPE1401           0              //              
#define TYPEPLUS           1              //          
#define TYPEU1401          2              //      
#define TYPEPOWER          3              //          
#define TYPEU14012         4              //           
#define TYPEPOWER2         5              //                
#define TYPEMICRO3         6              //            
#define TYPEPOWER3         7              //            

//                                                                            
//                                         
#define DRIVERMAJREV      2             //                                            
#define DRIVERMINREV      0             //                            

//                                                         
#define TM_EXTTOHOST    8               //                
#define TM_EXTTO1401    9               //                

//                                                                        
#define H_TO_D 0x00
#define D_TO_H 0x80
#define VENDOR 0x40
#define DEVREQ 0x00
#define INTREQ 0x01
#define ENDREQ 0x02

//                                                                  
#define GET_STATUS      0x00
#define CLEAR_FEATURE   0x01
#define SET_FEATURE     0x03
#define SET_ADDRESS     0x05
#define GET_DESC        0x06
#define SET_DESC        0x07
#define GET_CONF        0x08
#define SET_CONF        0x09
#define GET_INTERFACE   0x0a
#define SET_INTERFACE   0x0b
#define SYNCH_FRAME     0x0c

//                                                                              
//                                                                            
#define DB_GRAB         0x50            /*                       */
#define DB_FREE         0x51            /*                           */
#define DB_SETADD       0x52            /*                            */
#define DB_SELFTEST     0x53            /*                 */
#define DB_SETMASK      0x54            /*                          */
#define DB_SETDEF       0x55            /*                           */
#define DB_PEEK         0x56            /*                           */
#define DB_POKE         0x57            /*                                 */
#define DB_RAMPD        0x58            /*                            */
#define DB_RAMPA        0x59            /*                  */
#define DB_REPEATS      0x5A            /*                                     */
#define DB_WIDTH        0x5B            /*                                 */
#define DB_DATA         0x5C            /*                              */
#define DB_CHARS        0x5D            /*                                  */

#define CR_CHAR          0x0D           /*                               */
#define CR_CHAR_80       0x8d           /*                     */

//                                                                                       
typedef struct circBlk
{
    volatile UINT dwOffset;             /*                                   */
    volatile UINT dwSize;               /*                                          */
} CIRCBLK;

//                                                                               
//                                                                            
typedef struct transarea
{
    void*       lpvBuff;                //                                                 
    UINT        dwBaseOffset;           //                                           
    UINT        dwLength;               //                              
    struct page **pPages;               //                                     
    int         nPages;                 //                                     
    bool        bUsed;                  //                          
    bool        bCircular;              //                                     
    bool        bCircToHost;            //                                        
    bool        bEventToHost;           //                               
    int         iWakeUp;                //                                       
    UINT        dwEventSt;              //                                        
    UINT        dwEventSz;              //                                              
    CIRCBLK     aBlocks[2];             //                                  
    wait_queue_head_t wqEvent;          //                                                    
} TRANSAREA;

//                                                                                   
//                                                                                     
typedef struct dmadesc
{
    unsigned short wTransType;          /*                                      */
    unsigned short wIdent;              /*                                      */
    unsigned int   dwSize;              /*                                      */
    unsigned int   dwOffset;            /*                                      */
    bool           bOutWard;            /*                                      */
} DMADESC;

#define INBUF_SZ         256            /*                   */
#define OUTBUF_SZ        256            /*                    */
#define STAGED_SZ 0x10000               //                                             

//                                                                                         
//                                                                             
typedef struct _DEVICE_EXTENSION
{
    char inputBuffer[INBUF_SZ];         /*                 */
    char outputBuffer[OUTBUF_SZ];       /*                                */
    volatile unsigned int dwNumInput;   /*                                */
    volatile unsigned int dwInBuffGet;  /*                                */
    volatile unsigned int dwInBuffPut;  /*                                */
    volatile unsigned int dwNumOutput;  /*                                */
    volatile unsigned int dwOutBuffGet; /*                                */
    volatile unsigned int dwOutBuffPut; /*                                */

    volatile bool bSendCharsPending;    /*                                  */
    volatile bool bReadCharsPending;    /*                                   */
    char* pCoherCharOut;                /*                                          */
    struct urb* pUrbCharOut;            /*                            */
    char* pCoherCharIn;                 /*                                          */
    struct urb* pUrbCharIn;             /*                            */

    spinlock_t charOutLock;             /*                                            */
    spinlock_t charInLock;              /*                                           */
    __u8 bInterval;                     /*                              */

    volatile unsigned int dwDMAFlag;    /*              */
    TRANSAREA rTransDef[MAX_TRANSAREAS];/*                    */
    volatile DMADESC rDMAInfo;          //                             
    volatile bool bXFerWaiting;         //                                 
    volatile bool bInDrawDown;          //                                    

    //                                                                                    
    //                                                                                     
    //                                                                                 
    //                                                                                                
    //                                              
    volatile short StagedId;            //                                       
    volatile bool StagedRead;           //                                              
    volatile unsigned int StagedLength; //                              
    volatile unsigned int StagedOffset; //                                             
    volatile unsigned int StagedDone;   //                         
    volatile bool bStagedUrbPending;    //                        
    char* pCoherStagedIO;               //                                
    struct urb* pStagedUrb;             //               
    spinlock_t stagedLock;              //                                                  

    short s1401Type;                    //                      
    short sCurrentState;                //                    
    bool bIsUSB2;                       //                                    
    bool bForceReset;                   //                                      
    __u32 statBuf[2];                   //                           

    unsigned long ulSelfTestTime;       //                          

    int nPipes;                         //                                            
    int bPipeError[4];                  //                                            
    __u8 epAddr[4];                     //                                

    struct usb_device *udev;            //                               
    struct usb_interface *interface;    //                                               
    struct usb_anchor submitted;        //                                           
    struct mutex io_mutex;              //                                                                

    int    errors;                      //                        
    int    open_count;                  //                            
    spinlock_t err_lock;                //                
    struct kref kref;
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;
#define to_DEVICE_EXTENSION(d) container_of(d, DEVICE_EXTENSION, kref)

//                                                               
//             
extern int Allowi(DEVICE_EXTENSION* pdx);
extern int SendChars(DEVICE_EXTENSION* pdx);
extern void ced_draw_down(DEVICE_EXTENSION *pdx);
extern int ReadWriteMem(DEVICE_EXTENSION *pdx, bool Read, unsigned short wIdent,
                      unsigned int dwOffs, unsigned int dwLen);

//             
extern int ClearArea(DEVICE_EXTENSION *pdx, int nArea);
extern int SendString(DEVICE_EXTENSION* pdx, const char __user* pData, unsigned int n);
extern int SendChar(DEVICE_EXTENSION *pdx, char c);
extern int Get1401State(DEVICE_EXTENSION* pdx, __u32* state, __u32* error);
extern int ReadWrite_Cancel(DEVICE_EXTENSION *pdx);
extern bool Is1401(DEVICE_EXTENSION* pdx);
extern bool QuickCheck(DEVICE_EXTENSION* pdx, bool bTestBuff, bool bCanReset);
extern int Reset1401(DEVICE_EXTENSION *pdx);
extern int GetChar(DEVICE_EXTENSION *pdx);
extern int GetString(DEVICE_EXTENSION *pdx, char __user* pUser, int n);
extern int SetTransfer(DEVICE_EXTENSION *pdx, TRANSFERDESC __user *pTD);
extern int UnsetTransfer(DEVICE_EXTENSION *pdx, int nArea);
extern int SetEvent(DEVICE_EXTENSION *pdx, TRANSFEREVENT __user*pTE);
extern int Stat1401(DEVICE_EXTENSION *pdx);
extern int LineCount(DEVICE_EXTENSION *pdx);
extern int GetOutBufSpace(DEVICE_EXTENSION *pdx);
extern int GetTransfer(DEVICE_EXTENSION *pdx, TGET_TX_BLOCK __user *pGTB);
extern int KillIO1401(DEVICE_EXTENSION *pdx);
extern int BlkTransState(DEVICE_EXTENSION *pdx);
extern int StateOf1401(DEVICE_EXTENSION *pdx);
extern int StartSelfTest(DEVICE_EXTENSION *pdx);
extern int CheckSelfTest(DEVICE_EXTENSION *pdx, TGET_SELFTEST __user *pGST);
extern int TypeOf1401(DEVICE_EXTENSION *pdx);
extern int TransferFlags(DEVICE_EXTENSION *pdx);
extern int DbgPeek(DEVICE_EXTENSION *pdx, TDBGBLOCK __user* pDB);
extern int DbgPoke(DEVICE_EXTENSION *pdx, TDBGBLOCK __user *pDB);
extern int DbgRampData(DEVICE_EXTENSION *pdx, TDBGBLOCK __user *pDB);
extern int DbgRampAddr(DEVICE_EXTENSION *pdx, TDBGBLOCK __user *pDB);
extern int DbgGetData(DEVICE_EXTENSION *pdx, TDBGBLOCK __user *pDB);
extern int DbgStopLoop(DEVICE_EXTENSION *pdx);
extern int SetCircular(DEVICE_EXTENSION *pdx, TRANSFERDESC __user *pTD);
extern int GetCircBlock(DEVICE_EXTENSION *pdx, TCIRCBLOCK __user* pCB);
extern int FreeCircBlock(DEVICE_EXTENSION *pdx, TCIRCBLOCK __user* pCB);
extern int WaitEvent(DEVICE_EXTENSION *pdx, int nArea, int msTimeOut);
extern int TestEvent(DEVICE_EXTENSION *pdx, int nArea);
#endif
