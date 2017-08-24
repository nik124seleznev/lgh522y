/****************************************************************************
** use1401.c
** Copyright (C) Cambridge Electronic Design Ltd, 1992-2010
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
** Contact CED: Cambridge Electronic Design Limited, Science Park, Milton Road
**              Cambridge, CB6 0FE.
**              www.ced.co.uk
**              greg@ced.co.uk
**
**  Title:      USE1401.C
**  Version:    4.00
**  Author:     Paul Cox, Tim Bergel, Greg Smith
**
** The code was vigorously pruned in DEC 2010 to remove the macintosh options
** and to get rid of the 16-bit support. It has also been aligned with the
** Linux version. See CVS for revisions. This will work for Win 9x onwards.
****************************************************************************
**
** Notes on Windows interface to driver
** ************************************
**
** Under Windows 9x and NT, Use1401 uses DeviceIoControl to get access to
** the 1401 driver. This has parameters for the device handle, the function
** code, an input pointer and byte count, an output pointer and byte count
** and a pointer to a DWORD to hold the output byte count. Note that input
** and output are from the point-of-view of the driver, so the output stuff
** is used to read values from the 1401, not send to the 1401. The use of
** these parameters varies with the function in use and the operating
** system; there are five separate DIOC calls SendString, GetString and
** SetTransferArea all have their own specialised calls, the rest use the
** Status1401 or Control1401 functions.
**
** There are two basic styles of DIOC call used, one for Win9x VxD drivers
** and one for NT Kernel-mode and WDM drivers (see below for tables showing
** the different parameters used. The array bUseNTDIOC[] selects between
** these two calling styles.
**
** Function codes
** In Win3.x, simple function codes from 0 to 40 were used, shifted left 8
** bits with a sub-function code in the lower 8 bits. These were also used
** in the Windows 95 driver, though we had to add 1 to the code value to
** avoid problems (Open from CreateFile is zero), and the sub-function code
** is now unused. We found that this gave some problems with Windows 98
** as the function code values are reserved by microsoft, so we switched to
** using the NT function codes instead. The NT codes are generated using the
** CTL_CODE macro, essentially this gives 0x80012000 | (func << 2), where
** func is the original 0 to 34 value. The driver will handle both types of
** code and Use1432 only uses the NT codes if it knows the driver is new
** enough. The array bUseNTCodes[] holds flags on the type of codes required.
** GPS/TDB Dec 2010: we removed the bUseNTCodes array as this is always true
** as we no longer support ancient versions.
**
** The CreateFile and CloseFile function calls are also handled
** by DIOC, using the special function codes 0 and -1 respectively.
**
** Input pointer and buffer size
** These are intended for data sent to the device driver. In nearly all cases
** they are unused in calls to the Win95 driver, the NT driver uses them
** for all information sent to the driver. The table below shows the pointer
** and byte count used for the various calls:
**
**                      Win 95                  Win NT
** SendString           NULL, 0                 pStr, nStr
** GetString            NULL, 0                 NULL, 0
** SetTransferArea      pBuf, nBuf (unused?)    pDesc, nDesc
** GetTransfer          NULL, 0                 NULL, 0
** Status1401           NULL, 0                 NULL, 0
** Control1401          NULL, 0                 pBlk, nBlk
**
** pStr and nStr are pointers to a char buffer and the buffer length for
** string I/O, note that these are temporary buffers owned by the DLL, not
** application memory, pBuf and nBuf are the transfer area buffer (I think
** these are unused), pDesc and nDesc are the TRANSFERDESC structure, pBlk
** and nBlk are the TCSBLOCK structure.
**
**
** Output pointer and buffer size
** These are intended for data read from the device driver. These are used
** for almost all information sent to the Win95 driver, the NT driver uses
** them for information read from the driver, chiefly the error code. The
** table below shows the pointer and byte count used for the various calls:
**
**                      Win 95                  Win NT
** SendString           pStr, nStr              pPar, nPar
** GetString            pStr, nStr+2            pStr, nStr+2
** SetTransferArea      pDesc, nDesc            pPar, nPar
** GetTransfer          pGet, nGet              pGet, nGet
** Status1401           pBlk, nBlk              pPar, nPar
** Control1401          pBlk, nBlk              pPar, nPar
**
** pStr and nStr are pointers to a char buffer and the buffer length for
** string I/O, the +2 for GetString refers to two spare bytes at the start
** used to hold the string length and returning an error code for NT. Note
** again that these are (and must be) DLL-owned temporary buffers. pPar
** and nPar are a PARAM structure used in NT (it holds an error code and a 
** TCSBLOCK structure). pDesc and nDesc are the VXTRANSFERDESC structure,
** pBlk and nBlk are the TCSBLOCK structure. pGet and nGet indicate the
** TGET_TX_BLOCK structure used for GetTransfer.
**
**
** The output byte count
** Both drivers return the output buffer size here, regardless of the actual
** bytes output. This is used to check that we did get through to the driver.
**
** Multiple 1401s
** **************
**
** We have code that tries to support the use of multiple 1401s, but there
** are problems: The lDriverVersion and lDriverType variables are global, not
** per-1401 (a particular problem as the U14 functions that use them don't
** have a hand parameter). In addition, the mechansim for finding a free
** 1401 depends upon the 1401 device driver open operation failing if it's
** already in use, which doesn't always happen, particularly with the VxDs.
** The code in TryToOpen tries to fix this by relying on TYPEOF1401 to detect
** the 1401-in-use state - the VxDs contain special code to help this. This is
** working OK but multiple 1401 support works better with the Win2000 drivers.
**
** USB driver
** **********
**
** The USB driver, which runs on both Win98 and NT2000, uses the NT-style
** calling convention, both for the DIOC codes and the DIOC parameters. The
** TryToOpen function has been altered to look for an NT driver first in
** the appropriate circumstances, and to set the driver DIOC flags up in
** the correct state.
**
** Adding a new 1401 type - now almost nothing to do
** *************************************************
**
** The 1401 types are defined by a set of U14TYPExxxx codes in USE1401.H.
** You should add a new one of these to keep things tidy for applications.
**
** DRIVERET_MAX (below) specifies the maximum allowed type code from the
** 1401 driver; I have set this high to accommodate as yet undesigned 1401
** types. Similarly, as long as the command file names follow the ARM,
** ARN, ARO sequence, these are calculated by the ExtForType function, so
** you don't need to do anything here either.
**
** Version number
** **************
** The new U14InitLib() function returns 0 if the OS is incapable of use,
** otherwise is returns the version of the USE1401 library. This is done
** in three parts: Major(31-24).Minor(23-16).Revision.(15-0) (brackets are
** the bits used). The Major number starts at 2 for the first revision with
** the U14InitLib() function. Changes to the Major version means that we
** have broken backwards compatibility. Minor number changes mean that we
** have added new functionality that does not break backwards compatibility.
** we starts at 0. Revision changes mean we have fixed something. Each index
** returns to 0 when a higher one changes.
*/
#define U14LIB_MAJOR 4
#define U14LIB_MINOR 0
#define U14LIB_REVISION 0
#define U14LIB_VERSION ((U14LIB_MAJOR<<24) | (U14LIB_MINOR<<16) | U14LIB_REVISION)

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "USE1401.H"

#ifdef _IS_WINDOWS_
#include <io.h>
#include <windows.h>
#pragma warning(disable: 4100) /*                                           */
#include <assert.h>
#include "process.h"


#define sprintf wsprintf
#define PATHSEP '\\'
#define PATHSEPSTR "\\"
#define DEFCMDPATH "\\1401\\"   //                                       
#define MINDRIVERMAJREV 1       //                                      
#define __packed                //                        

#include "use14_ioc.h"          //                             
#endif

#ifdef LINUX
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/time.h>
#include <sched.h>
#include <libgen.h>
#define PATHSEP '/'
#define PATHSEPSTR "/"
#define DEFCMDPATH "/var/1401/" //                                       
#define MINDRIVERMAJREV 2       //                                      

#include "ced_ioctl.h"          //                             
#endif

#define MAX1401         8       //                                          

/*
                                                                            
                                                                         
                                                                  
*/
#define DRIVRET_STD     4       //                                            
#define DRIVRET_U1401   5       //                                       
#define DRIVRET_PLUS    6       //                             
#define DRIVRET_POWER   7       //                                      
#define DRIVRET_MAX     26      //                                        

/*
                                                                    
                                                                        
*/
static char szLastName[20];     //                            

/*
                                                                               
                                                                               
                                                                              
                                                                           
                                                           
*/
static short asDriverType[MAX1401] = {0};
static int lLastDriverVersion = U14ERR_NO1401DRIV;
static int lLastDriverType = U14TYPEUNKNOWN;
static int alDriverVersion[MAX1401];            //                            
static int alTimeOutPeriod[MAX1401];            //                             
static short asLastRetCode[MAX1401];            //                         
static short asType1401[MAX1401] = {0};         //                     
static BOOL abGrabbed[MAX1401] = {0};           //                                       
static int iAttached = 0;                       //                                      

#ifdef _IS_WINDOWS_
/*                                                                           
                                                   
                                                                           */
static HANDLE aHand1401[MAX1401] = {0};         //                  
static HANDLE aXferEvent[MAX1401] = {0};        //                              
static LPVOID apAreas[MAX1401][MAX_TRANSAREAS]; //             
static DWORD  auAreas[MAX1401][MAX_TRANSAREAS]; //                     
static BOOL   bWindows9x = FALSE;               //                               
#ifdef _WIN64
#define USE_NT_DIOC(ind) TRUE
#else
static BOOL   abUseNTDIOC[MAX1401];             //                                
#define USE_NT_DIOC(ind) abUseNTDIOC[ind]
#endif

#endif

#ifdef LINUX
static int aHand1401[MAX1401] = {0};    //                  
#define INVALID_HANDLE_VALUE 0          //                          
#endif


/*
                                                                                      
                                                                                  
          
*/
#pragma pack(1)                 //                   
typedef struct CmdHead          //                                
{                               //                
   char   acBasic[5];           //                                           
   WORD   wBasicSz;             //                      
   WORD   wCmdSize;             //                           
} __packed CMDHEAD;
#pragma pack()                  //               

/*
                                            
                                                                  
                                                               
                                                      
                                                      
*/

typedef CMDHEAD *LPCMDHEAD;     //                            

#define  MAXSTRLEN   255        //                             
#define  TOHOST      FALSE
#define  TO1401      TRUE

static short CheckHandle(short h)
{
    if ((h < 0) || (h >= MAX1401))  //                       
        return U14ERR_BADHAND;
    if (aHand1401[h] <= 0)          //             
        return U14ERR_BADHAND;
    return U14ERR_NOERROR;
}

#ifdef _IS_WINDOWS_
/*                                                                           
                                                                        
                                 
                                                                           */
static short U14Status1401(short sHand, LONG lCode, TCSBLOCK* pBlk)
{
    DWORD dwBytes = 0;

    if ((sHand < 0) || (sHand >= MAX1401))  /*                  */
        return U14ERR_BADHAND;
#ifndef _WIN64
    if (!USE_NT_DIOC(sHand)) 
    {   /*                          */
        if (DeviceIoControl(aHand1401[sHand], lCode, NULL, 0, pBlk,sizeof(TCSBLOCK),&dwBytes,NULL))
            return (short)((dwBytes>=sizeof(TCSBLOCK)) ? U14ERR_NOERROR : U14ERR_DRIVCOMMS);
        else
            return (short)GetLastError();
    }
    else
#endif
    {                                       /*                          */
        PARAMBLK rWork;
        rWork.sState = U14ERR_DRIVCOMMS;
        if (DeviceIoControl(aHand1401[sHand], lCode, NULL, 0, &rWork,sizeof(PARAMBLK),&dwBytes,NULL) &&
            (dwBytes >= sizeof(PARAMBLK)))
        {
            *pBlk = rWork.csBlock;
            return rWork.sState;
        }
    }

    return U14ERR_DRIVCOMMS;
}

/*                                                                           
                                                                         
                                      
                                                                           */
static short U14Control1401(short sHand, LONG lCode, TCSBLOCK* pBlk)
{
    DWORD dwBytes = 0;

    if ((sHand < 0) || (sHand >= MAX1401))              /*                  */
        return U14ERR_BADHAND;

#ifndef _WIN64
    if (!USE_NT_DIOC(sHand))                    
    {                            /*                         */
        if (DeviceIoControl(aHand1401[sHand], lCode, NULL, 0, pBlk, sizeof(TCSBLOCK), &dwBytes, NULL))
            return (short)(dwBytes >= sizeof(TCSBLOCK) ? U14ERR_NOERROR : U14ERR_DRIVCOMMS);
        else
            return (short)GetLastError();
    }
    else
#endif
    {                            /*                     */
        PARAMBLK rWork;
        rWork.sState = U14ERR_DRIVCOMMS;
        if (DeviceIoControl(aHand1401[sHand], lCode, pBlk, sizeof(TCSBLOCK), &rWork, sizeof(PARAMBLK), &dwBytes, NULL) &&
            (dwBytes >= sizeof(PARAMBLK)))
            return rWork.sState;
    }

    return U14ERR_DRIVCOMMS;
}
#endif

/*                                                                           
                
                                                     
                                                                            */
static long SafeTickCount()
{
#ifdef _IS_WINDOWS_
    return GetTickCount();
#endif
#ifdef LINUX
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec*1000 + tv.tv_usec/1000);
#endif
}

/*                                                                           
                                                                       
                                                     
                                                                           */
static int ExtForType(short sType, char* szExt)
{
    szExt[0] = 0;                       /*                                  */
    switch (sType)
    {
    case U14TYPE1401: strcpy(szExt, ".CMD");  break;    //              
    case U14TYPEPLUS: strcpy(szExt, ".GXC");  break;    //          
    default:               //                                         
        strcpy(szExt, ".ARM");
            szExt[3] = (char)('M' + sType - U14TYPEU1401);
        if (szExt[3] > 'Z')             //                            
                szExt[3] = (char)(szExt[3] - 26);
    }
    return 0;
}

/*                                                                           
                     
                                                                            
                                                       
                                                                           */
U14API(int) U14WhenToTimeOut(short hand)
{
    int iNow = SafeTickCount();
    if ((hand >= 0) && (hand < MAX1401))
        iNow += alTimeOutPeriod[hand];
    return iNow;
}

/*                                                                           
                
                                                                   
                                                                           */
U14API(short) U14PassedTime(int lCheckTime)
{
    return (short)((SafeTickCount()-lCheckTime) > 0);
}

/*                                                                           
                  
                                                                           
                                                                             
                                                                           */
static void TranslateString(char* pStr)
{
    int i = 0;
    while (pStr[i])
    {
        if (pStr[i] == ',')
            pStr[i] = ' ';              /*                        */
        ++i;
    }

    if ((i > 0) && (pStr[i-1] == '\n'))  /*                     */
        pStr[i-1] = (char)0;
}

/*                                                                           
                
                                                                          
                                                                           */
U14API(short) U14StrToLongs(const char* pszBuff, U14LONG *palNums, short sMaxLongs)
{
    WORD wChInd = 0;                //                  
    short sLgInd = 0;               //                        

    while (pszBuff[wChInd] &&       //                                 
           (sLgInd < sMaxLongs))    //                        
    {
        //                                   
        switch (pszBuff[wChInd])
        {
        case '-':
        case '0': case '1':   case '2': case '3':   case '4':
        case '5': case '6':   case '7': case '8':   case '9':
            {
                BOOL bDone = FALSE; //                      
                int iSign = 1;      //               
                long lValue = 0;

                while ((!bDone) && pszBuff[wChInd])
                {
                    switch (pszBuff[wChInd])
                    {
                    case '-':
                        iSign = -1; //          
                        break;

                    case '0': case '1':   case '2': case '3':   case '4':
                    case '5': case '6':   case '7': case '8':   case '9':
                        lValue *= 10;   //                           
                        lValue += ((int)pszBuff[wChInd]-(int)'0');
                        break;

                    default:        //              
                        bDone = TRUE;
                        break;
                    }
                    wChInd++;       //                         
                }
                palNums[sLgInd] = lValue * iSign;
                sLgInd++;
            }
            break;

        default:
            wChInd++;               //                  
            break;
        }
    }
    return (sLgInd);
}


/*                                                                           
                   
                                                                 
                                                  
                                                                           */
U14API(short) U14LongsFrom1401(short hand, U14LONG *palBuff, short sMaxLongs)
{
    char szWork[MAXSTRLEN];
    short sResult = U14GetString(hand, szWork, MAXSTRLEN);/*                       */
    if (sResult == U14ERR_NOERROR)                  /*                       */
        sResult = U14StrToLongs(szWork, palBuff, sMaxLongs);
    return sResult;
}

/*                                                                           
                
                                                                        
                                                   
                                                                           */
U14API(short) U14CheckErr(short hand)
{
    short sResult = U14SendString(hand, ";ERR;");
    if (sResult == U14ERR_NOERROR)
    {
        U14LONG er[3];
        sResult = U14LongsFrom1401(hand, er, 3);
        if (sResult > 0)
        {
            sResult = (short)er[0];        /*                               */
#ifdef _DEBUG
            if (er[0] != 0)
            {
                char szMsg[50];
                sprintf(szMsg, "U14CheckErr returned %d,%d\n", er[0], er[1]);
                OutputDebugString(szMsg);
            }
#endif
        }
        else
        {
            if (sResult == 0)
                sResult = U14ERR_TIMEOUT;      /*                           */
        }
    }

    return sResult;
}

/*                                                                           
                 
                                                                             
                                                                        
                                                                           */
U14API(short) U14LastErrCode(short hand)
{
    if ((hand < 0) || (hand >= MAX1401))
        return U14ERR_BADHAND;
    return asLastRetCode[hand];
}

/*                                                                           
                
                                                        
                                                                           */
U14API(void) U14SetTimeout(short hand, int lTimeOut)
{
    if ((hand < 0) || (hand >= MAX1401))
        return;
    alTimeOutPeriod[hand] = lTimeOut;
}

/*                                                                           
                
                                                        
                                                                           */
U14API(int) U14GetTimeout(short hand)
{
    if ((hand < 0) || (hand >= MAX1401))
        return U14ERR_BADHAND;
    return alTimeOutPeriod[hand];
}

/*                                                                           
                 
                                                      
                                                                           */
U14API(short) U14OutBufSpace(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    short sErr = U14Status1401(hand, U14_GETOUTBUFSPACE,&csBlock);
    if (sErr == U14ERR_NOERROR)
        sErr = csBlock.ints[0];
    return sErr;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_GetOutBufSpace(aHand1401[hand]) : sErr;
#endif
}


/*                                                                           
                  
                                                                       
                                                                           */
U14API(int) U14BaseAddr1401(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    int iError = U14Status1401(hand, U14_GETBASEADDRESS,&csBlock);
    if (iError == U14ERR_NOERROR)
        iError = csBlock.longs[0];
    return iError;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_GetBaseAddress(aHand1401[hand]) : sErr;
#endif
}

/*                                                                           
                 
                                                         
                                                                           */
U14API(short) U14StateOf1401(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    short sErr = U14Status1401(hand, U14_STATEOF1401, &csBlock);
    if (sErr == U14ERR_NOERROR)
    {
        sErr = csBlock.ints[0];      //                    
        if ((sErr >= DRIVRET_STD) && (sErr <= DRIVRET_MAX))
            sErr = U14ERR_NOERROR;
    }
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
        sErr = (short)CED_StateOf1401(aHand1401[hand]);
        if ((sErr >= DRIVRET_STD) && (sErr <= DRIVRET_MAX))
            sErr = U14ERR_NOERROR;
    }
#endif
    return sErr;
}

/*                                                                           
                   
                                                                            
                                                                             
                                                             
                                                                           */
U14API(int) U14DriverVersion(short hand)
{
    return CheckHandle(hand) != U14ERR_NOERROR ? lLastDriverVersion : alDriverVersion[hand];
}

/*                                                                           
                
                                                                       
                                                                          
                                                             
                                                                           */
U14API(int) U14DriverType(short hand)
{
    return CheckHandle(hand) != U14ERR_NOERROR ? lLastDriverType : asDriverType[hand];
}

/*                                                                           
                
                                                                 
                                                                           */
U14API(short) U14DriverName(short hand, char* pBuf, WORD wMax)
{
    char* pName;
    *pBuf = 0;                             //                              
    switch (U14DriverType(hand))           //                          
    {
    case 0:  pName = "ISA"; break;
    case 1:  pName = "PCI"; break;
    case 2:  pName = "USB"; break;
    case 3:  pName = "HSS"; break;
    default: pName = "???"; break;
    }
    strncpy(pBuf, pName, wMax);            //                                

    return U14ERR_NOERROR;
}

/*                                                                           
                   
                                                                             
                                                                           */
U14API(short) U14BlkTransState(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    short sErr = U14Status1401(hand, U14_BLKTRANSSTATE, &csBlock);
    if (sErr == U14ERR_NOERROR)
        sErr = csBlock.ints[0];
    return sErr;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_BlkTransState(aHand1401[hand]) : sErr;
#endif
}

/*                                                                           
              
                                                                       
                                                                           */
U14API(short) U14Grab1401(short hand)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
#ifdef _IS_WINDOWS_
        if (abGrabbed[hand])            //                                  
            sErr = U14ERR_ALREADYSET;   //                            
        else
        {
            TCSBLOCK csBlock;
            sErr = U14Control1401(hand, U14_GRAB1401, &csBlock);
        }
#endif
#ifdef LINUX
        //                                  
        sErr = abGrabbed[hand] ? U14ERR_ALREADYSET : CED_Grab1401(aHand1401[hand]);
#endif
        if (sErr == U14ERR_NOERROR)
            abGrabbed[hand] = TRUE;
    }
    return sErr;
}

/*                                                                           
              
                                                                           */
U14API(short)  U14Free1401(short hand)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
#ifdef _IS_WINDOWS_
        if (abGrabbed[hand])    //                              
        {
            TCSBLOCK csBlock;
            sErr = U14Control1401(hand, U14_FREE1401, &csBlock);
        }
        else
            sErr = U14ERR_NOTSET;
#endif
#ifdef LINUX
        //                                  
        sErr = abGrabbed[hand] ? CED_Free1401(aHand1401[hand]) : U14ERR_NOTSET;
#endif
        if (sErr == U14ERR_NOERROR)
            abGrabbed[hand] = FALSE;
    }
    return sErr;
}

/*                                                                           
              
                                                                 
                                                                     
                                                                     
                           
                                                                           */
U14API(short) U14Peek1401(short hand, DWORD dwAddr, int nSize, int nRepeats)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
        if (abGrabbed[hand])    //                              
        {
#ifdef _IS_WINDOWS_
            TCSBLOCK csBlock;
            csBlock.longs[0] = (long)dwAddr;
            csBlock.longs[1] = nSize;
            csBlock.longs[2] = nRepeats;
            sErr = U14Control1401(hand, U14_DBGPEEK, &csBlock);
#endif
#ifdef LINUX
            TDBGBLOCK dbb;
            dbb.iAddr = (int)dwAddr;
            dbb.iWidth = nSize;
            dbb.iRepeats = nRepeats;
            sErr = CED_DbgPeek(aHand1401[hand], &dbb);
#endif
        }
        else
            sErr = U14ERR_NOTSET;
    }
    return sErr;
}

/*                                                                           
              
                                                                 
                                                                     
             
                                                                           */
U14API(short) U14Poke1401(short hand, DWORD dwAddr, DWORD dwValue,
                                      int nSize, int nRepeats)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
        if (abGrabbed[hand])    //                              
        {
#ifdef _IS_WINDOWS_
            TCSBLOCK csBlock;
            csBlock.longs[0] = (long)dwAddr;
            csBlock.longs[1] = nSize;
            csBlock.longs[2] = nRepeats;
            csBlock.longs[3] = (long)dwValue;
            sErr = U14Control1401(hand, U14_DBGPOKE, &csBlock);
#endif
#ifdef LINUX
            TDBGBLOCK dbb;
            dbb.iAddr = (int)dwAddr;
            dbb.iWidth = nSize;
            dbb.iRepeats= nRepeats;
            dbb.iData = (int)dwValue;
            sErr = CED_DbgPoke(aHand1401[hand], &dbb);
#endif
        }
        else
            sErr = U14ERR_NOTSET;
    }
    return sErr;
}

/*                                                                           
              
                                                                     
                                                                      
                                                                           */
U14API(short) U14Ramp1401(short hand, DWORD dwAddr, DWORD dwDef, DWORD dwEnable,
                                      int nSize, int nRepeats)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
        if (abGrabbed[hand])    //                              
        {
#ifdef _IS_WINDOWS_
            TCSBLOCK csBlock;
            csBlock.longs[0] = (long)dwAddr;
            csBlock.longs[1] = (long)dwDef;
            csBlock.longs[2] = (long)dwEnable;
            csBlock.longs[3] = nSize;
            csBlock.longs[4] = nRepeats;
            sErr = U14Control1401(hand, U14_DBGRAMPDATA, &csBlock);
#endif
#ifdef LINUX
            TDBGBLOCK dbb;
            dbb.iAddr = (int)dwAddr;
            dbb.iDefault = (int)dwDef;
            dbb.iMask = (int)dwEnable;
            dbb.iWidth = nSize;
            dbb.iRepeats = nRepeats;
            sErr = CED_DbgRampAddr(aHand1401[hand], &dbb);
#endif
        }
        else
            sErr = U14ERR_NOTSET;
    }
    return sErr;
}

/*                                                                           
              
                                                                        
                                                                     
                                                                           */
U14API(short) U14RampAddr(short hand, DWORD dwDef, DWORD dwEnable,
                                      int nSize, int nRepeats)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
        if (abGrabbed[hand])    //                              
        {
#ifdef _IS_WINDOWS_
            TCSBLOCK csBlock;
            csBlock.longs[0] = (long)dwDef;
            csBlock.longs[1] = (long)dwEnable;
            csBlock.longs[2] = nSize;
            csBlock.longs[3] = nRepeats;
            sErr = U14Control1401(hand, U14_DBGRAMPADDR, &csBlock);
#endif
#ifdef LINUX
            TDBGBLOCK dbb;
            dbb.iDefault = (int)dwDef;
            dbb.iMask = (int)dwEnable;
            dbb.iWidth = nSize;
            dbb.iRepeats = nRepeats;
            sErr = CED_DbgRampAddr(aHand1401[hand], &dbb);
#endif
        }
        else
            sErr = U14ERR_NOTSET;
    }
    return sErr;
}

/*                                                                           
                      
                                                                        
                                      
                                                                           */
U14API(short) U14StopDebugLoop(short hand)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
#ifdef _IS_WINDOWS_
    {
        if (abGrabbed[hand])    //                              
        {
            TCSBLOCK csBlock;
            sErr = U14Control1401(hand, U14_DBGSTOPLOOP, &csBlock);
        }
        else
            sErr = U14ERR_NOTSET;
    }
#endif
#ifdef LINUX
        sErr = abGrabbed[hand] ? CED_DbgStopLoop(aHand1401[hand]) : U14ERR_NOTSET;
#endif
    return sErr;
}

/*                                                                           
                  
                                                                 
                                                                           */
U14API(short) U14GetDebugData(short hand, U14LONG* plValue)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
        if (abGrabbed[hand])    //                              
        {
#ifdef _IS_WINDOWS_
            TCSBLOCK csBlock;
            sErr = U14Status1401(hand, U14_DBGGETDATA, &csBlock);
            if (sErr == U14ERR_NOERROR)
                *plValue = csBlock.longs[0];    //                
#endif
#ifdef LINUX
            TDBGBLOCK dbb;
            sErr = CED_DbgGetData(aHand1401[hand], &dbb);
            if (sErr == U14ERR_NOERROR)
                *plValue = dbb.iData;                     /*                 */
#endif
        }
        else
            sErr = U14ERR_NOTSET;
    }
    return sErr;
}

/*                                                                           
                   
                                                                           */
U14API(short) U14StartSelfTest(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    return U14Control1401(hand, U14_STARTSELFTEST, &csBlock);
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_StartSelfTest(aHand1401[hand]) : sErr;
#endif
}

/*                                                                           
                   
                                                                           */
U14API(short) U14CheckSelfTest(short hand, U14LONG *pData)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    short sErr = U14Status1401(hand, U14_CHECKSELFTEST, &csBlock);
    if (sErr == U14ERR_NOERROR)
    {
        pData[0] = csBlock.longs[0];        /*                            */
        pData[1] = csBlock.longs[1];
        pData[2] = csBlock.longs[2];
    }
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)                /*                  */
    {
        TGET_SELFTEST gst;
        sErr = CED_CheckSelfTest(aHand1401[hand], &gst);
        if (sErr == U14ERR_NOERROR)
        {
            pData[0] = gst.code;        /*                            */
            pData[1] = gst.x;
            pData[2] = gst.y;
        }
    }
#endif
    return sErr;
}

/*                                                                           
                       
                                                                           */
U14API(short) U14GetUserMemorySize(short hand, DWORD *pMemorySize)
{
    //                                                                
    short sErr = U14SendString(hand, (asType1401[hand] == U14TYPE1401) ? "MEMTOP;" : "MEMTOP,?;");
    *pMemorySize = 0;         /*                                           */
    if (sErr == U14ERR_NOERROR)
    {
        U14LONG alLimits[4];
        sErr = U14LongsFrom1401(hand, alLimits, 4);
        if (sErr > 0)              /*                                       */
        {
            sErr = U14ERR_NOERROR;                  /*                      */
            if (asType1401[hand] == U14TYPE1401)    /*                      */
                *pMemorySize = alLimits[0] - alLimits[1]; /*               */
            else
                *pMemorySize = alLimits[0];   /*                           */
        }
    }
    return sErr;
}

/*                                                                           
                
                                              
                                                                           */
U14API(short) U14TypeOf1401(short hand)
{
    if ((hand < 0) || (hand >= MAX1401))                /*                  */
        return U14ERR_BADHAND;
    else
        return asType1401[hand];
}

/*                                                                           
                
                                                             
                                                                           */
U14API(short) U14NameOf1401(short hand, char* pBuf, WORD wMax)
{
    short sErr = CheckHandle(hand);
    if (sErr == U14ERR_NOERROR)
    {
    char* pName;
    switch (asType1401[hand])               //                          
    {
    case U14TYPE1401:  pName = "Std 1401"; break;
    case U14TYPEPLUS:  pName = "1401plus"; break;
    case U14TYPEU1401: pName = "micro1401"; break;
    case U14TYPEPOWER: pName = "Power1401"; break;
    case U14TYPEU14012:pName = "Micro1401 mk II"; break;
    case U14TYPEPOWER2:pName = "Power1401 mk II"; break;
    case U14TYPEU14013:pName = "Micro1401-3"; break;
    case U14TYPEPOWER3:pName = "Power1401-3"; break;
    default:           pName = "Unknown";
    }
        strncpy(pBuf, pName, wMax);
    }
    return sErr;
}

/*                                                                           
                   
                                            
                                                       
                                                                            */
U14API(short) U14TransferFlags(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    short sErr = U14Status1401(hand, U14_TRANSFERFLAGS, &csBlock);
    return (sErr == U14ERR_NOERROR) ? (short)csBlock.ints[0] : sErr;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_TransferFlags(aHand1401[hand]) : sErr;
#endif
}

/*                                                                           
                   
                                                        
                                                         
                                                                            
                                                                            */
static int GetDriverVersion(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    int iErr = U14Status1401(hand, U14_GETDRIVERREVISION, &csBlock);
    if (iErr == U14ERR_NOERROR)
        iErr = csBlock.longs[0];
    return iErr;
#endif
#ifdef LINUX
    return CED_GetDriverRevision(aHand1401[hand]);
#endif
}

/*                                                                           
                
                                            
                                                                 
                                                      
                                                                            */
U14API(int) U14MonitorRev(short hand)
{
    int iRev = 0;
    int iErr = CheckHandle(hand);
    if (iErr != U14ERR_NOERROR)                 //                      
        return iErr;

    if (asType1401[hand] >= U14TYPEPOWER2)      //                                           
    {                                           //                                    
        iErr = U14SendString(hand, "INFO,S,28;");
        if (iErr == U14ERR_NOERROR)
        {
            U14LONG lVals[2];                   //                                        
            iErr = U14LongsFrom1401(hand, lVals, 1);
            if (iErr > 0)
            {
                iErr = U14ERR_NOERROR;
                iRev = lVals[0];                //                                       
                iRev += asType1401[hand] * 10000;
            }
        }
    }
    else
    {                                           /*                                       */
        iErr = U14SendString(hand, ";CLIST;");     /*                        */
        if (iErr == U14ERR_NOERROR)
        {     
            while (iErr == U14ERR_NOERROR)
            {
                char wstr[50];
                iErr = U14GetString(hand, wstr, 45);
                if (iErr == U14ERR_NOERROR)
                {
                    char *pstr = strstr(wstr,"RESET");  /*                            */
                    if ((pstr == wstr) && (wstr[5] == ' '))
                    {
                        char *pstr2;
                        size_t l;
                        pstr += 6;       /*                                     */
                        l = strlen(pstr);       /*                              */
                        while (((pstr[l-1] == ' ') || (pstr[l-1] == 13)) && (l > 0))
                        {
                            pstr[l-1] = 0;         /*                           */
                            l--;                  /*                            */
                        }
                        pstr2 = strchr(pstr, '.');    /*                        */
                        if (pstr2 != NULL)                /*                    */
                        {
                            *pstr2 = 0;                /*                       */
                            pstr2++;              /*                            */
                            iRev = atoi(pstr2);   /*                            */
                        }
                        iRev += (atoi(pstr) * 1000);    /*                      */
                    }
                    if ((strlen(wstr) < 3) && (wstr[0] == ' '))
                        break;              /*                               */
                }
            }
        }
    }
    if (iErr == U14ERR_NOERROR)            /*                             */
        iErr = iRev;

    return iErr;
}

/*                                                                           
                                                        
                                                                  
                                                                    
                                                         
                                                                           */
#ifdef _IS_WINDOWS_
#define U14NAMEOLD "\\\\.\\CED_140%d"
#define U14NAMENEW "\\\\.\\CED%d"
static short U14TryToOpen(int n1401, long* plRetVal, short* psHandle)
{
    short sErr = U14ERR_NOERROR;
    HANDLE hDevice = INVALID_HANDLE_VALUE;
    DWORD dwErr = 0;
    int nFirst, nLast, nDev = 0;        /*                                */
    BOOL bOldName = FALSE;               /*                                      */

    if (n1401 == 0)                             /*                               */
    {
        nFirst = 1;                             /*                      */
        nLast = MAX1401;                        /*                                */
    }
    else
        nFirst = nLast = n1401;                 /*                         */

    while (hDevice == INVALID_HANDLE_VALUE)     /*                        */
    {
        for (nDev = nFirst; nDev <= nLast; nDev++)
        {
            char szDevName[40];                 /*                            */
            sprintf(szDevName, bOldName ? U14NAMEOLD : U14NAMENEW, nDev);
            hDevice = CreateFile(szDevName, GENERIC_WRITE | GENERIC_READ,
                                 0, 0,          /*                                                */
                                 OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (hDevice != INVALID_HANDLE_VALUE)/*                      */
            {
                TCSBLOCK csBlock;
                assert(aHand1401[nDev-1] == INVALID_HANDLE_VALUE);  //                       
                aHand1401[nDev-1] = hDevice;    /*                     */

#ifndef _WIN64
                //                                                              
                abUseNTDIOC[nDev-1] = (BOOL)(!bWindows9x || !bOldName);
#endif
                sErr = U14Status1401((short)(nDev-1), U14_TYPEOF1401, &csBlock);
                if (sErr == U14ERR_NOERROR)
                {
                    *plRetVal = csBlock.ints[0];
                    if (csBlock.ints[0] == U14ERR_INUSE)/*                     */
                    {
                        CloseHandle(hDevice);   /*                       */
                        hDevice = INVALID_HANDLE_VALUE;
                        aHand1401[nDev-1] = INVALID_HANDLE_VALUE;
                        sErr = U14ERR_INUSE;
                    }
                    else
                        break;                  /*                               */
                }
                else
                {
                    CloseHandle(hDevice);       /*                       */
                    hDevice = INVALID_HANDLE_VALUE;
                    aHand1401[nDev-1] = INVALID_HANDLE_VALUE;
                }
            }
            else
            {
                DWORD dwe = GetLastError();     /*                          */
                if ((dwe != ERROR_FILE_NOT_FOUND) || (dwErr == 0))
                    dwErr = dwe;                /*                               */
            }
        }

        if ((hDevice == INVALID_HANDLE_VALUE) &&/*                         */
            (bWindows9x) &&                     /*                                  */
            (bOldName == FALSE))                /*                            */
            bOldName = TRUE;                    /*                             */
        else
            break;                              /*                            */
    }

    if (hDevice != INVALID_HANDLE_VALUE)        /*                           */
        *psHandle = (short)(nDev-1);            /*                           */
    else
    {
        if (dwErr == ERROR_FILE_NOT_FOUND)      /*                          */
            sErr = U14ERR_NO1401DRIV;           /*                   */
        else if (dwErr == ERROR_NOT_SUPPORTED)
            sErr = U14ERR_DRIVTOOOLD;           /*                       */
        else if (dwErr == ERROR_ACCESS_DENIED)
            sErr = U14ERR_INUSE;
        else
            sErr = U14ERR_DRIVCOMMS;            /*                                */
    }
    return sErr;
}
#endif
#ifdef LINUX
static short U14TryToOpen(int n1401, long* plRetVal, short* psHandle)
{
    short sErr = U14ERR_NOERROR;
    int fh = 0;                             //                    
    int iErr = 0;
    int nFirst, nLast, nDev = 0;            //                               

    if (n1401 == 0)                         //                              
    {
        nFirst = 1;                             /*                      */
        nLast = MAX1401;                        /*                                */
    }
    else
        nFirst = nLast = n1401;                 /*                         */

    for (nDev = nFirst; nDev <= nLast; nDev++)
    {
        char szDevName[40];                 //                           
        sprintf(szDevName,"/dev/cedusb/%d", nDev-1);
        fh = open(szDevName, O_RDWR);       //                                  
        if (fh > 0)                         //                     
        {
            int iType1401 = CED_TypeOf1401(fh); //              
            aHand1401[nDev-1] = fh;         //                    
            if (iType1401 >= 0)
            {
                *plRetVal = iType1401;
                 break;                     //                              
            }
            else
            {
                close(fh);                  //                      
                fh = 0;
                aHand1401[nDev-1] = 0;
            }
        }
        else
        {
            if (((errno != ENODEV) && (errno != ENOENT)) || (iErr == 0))
                iErr = errno;                //                              
        }
    }


    if (fh)                                 //                          
        *psHandle = (short)(nDev-1);        //                          
    else
    {
        if ((iErr == ENODEV) || (iErr == ENOENT)) //                         
            sErr = U14ERR_NO1401DRIV;       //                  
        else if (iErr == EBUSY)
            sErr = U14ERR_INUSE;
        else
            sErr = U14ERR_DRIVCOMMS;        //                               
    }

    return sErr;
}
#endif
/*                                                                           
              
                                                    
                                                                            */
U14API(short) U14Open1401(short n1401)
{
    long     lRetVal = -1;
    short    sErr;
    short    hand = 0;
    
    if ((n1401 < 0) || (n1401 > MAX1401))       //                           
        return U14ERR_BAD1401NUM;

    szLastName[0] = 0;          /*                                  */

    sErr = U14TryToOpen(n1401, &lRetVal, &hand);
    if (sErr == U14ERR_NOERROR)
    {
        long lDriverVersion = GetDriverVersion(hand);   /*                     */
        long lDriverRev = -1;
		if (lDriverVersion >= 0)                    /*                      */
        {
            lLastDriverType = (lDriverVersion >> 24) & 0x000000FF;
            asDriverType[hand] = (short)lLastDriverType;    /*          */
            lLastDriverVersion = lDriverVersion & 0x00FFFFFF;
            alDriverVersion[hand] = lLastDriverVersion;     /*                */
            lDriverRev = ((lDriverVersion>>16) & 0x00FF);    /*             */
        }
        else
        {
            U14Close1401(hand);    /*                                       */
            return (short)lDriverVersion;      /*                           */
        }
    
        if (lDriverRev < MINDRIVERMAJREV)       /*                          */
        {
            U14Close1401(hand);    /*                                       */
            return U14ERR_DRIVTOOOLD;           /*                          */
        }
    
        asLastRetCode[hand] = U14ERR_NOERROR; /*                            */
        abGrabbed[hand] = FALSE;          /*                                */
        U14SetTimeout(hand, 3000);      /*                                  */

        switch (lRetVal)
        {
        case DRIVRET_STD:  asType1401[hand] = U14TYPE1401; break;      /*                    */
        case DRIVRET_U1401:asType1401[hand] = U14TYPEU1401; break;
        case DRIVRET_PLUS: asType1401[hand] = U14TYPEPLUS; break;
        default:  //                                                  
                if ((lRetVal >= DRIVRET_POWER) && (lRetVal <= DRIVRET_MAX))
                    asType1401[hand] = (short)(lRetVal - (DRIVRET_POWER - U14TYPEPOWER));
                else
                    asType1401[hand] = U14TYPEUNKNOWN;
                break;
            }
        U14KillIO1401(hand);                     /*                         */

        if (asType1401[hand] != U14TYPEUNKNOWN)   /*                        */
        {
            sErr = U14CheckErr(hand);        /*                             */
            if (sErr != 0)                       /*                         */
                U14Reset1401(hand); /*                                      */
        }

        sErr = U14StateOf1401(hand);/*                                      */
        if (sErr == U14ERR_NOERROR)
            sErr = hand;                 /*                                 */
        else
            U14Close1401(hand);    /*                                       */
    }

    return sErr;
}


/*                                                                           
               
                                              
                                                                           */
U14API(short) U14Close1401(short hand)
{
    int j;
    int iAreaMask = 0;                          //                      
    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)                 //                      
        return sErr;

    for (j = 0; j<MAX_TRANSAREAS; ++j)
    {
        TGET_TX_BLOCK gtb;
        int iReturn = U14GetTransfer(hand, &gtb);   //                     
        if (iReturn == U14ERR_NOERROR)          //                      
            if (gtb.used)
                iAreaMask |= (1 << j);          //                             
    }

    if (iAreaMask)                              //                        
    {
        U14Reset1401(hand);                     //                                   
        for (j = 0; j < MAX_TRANSAREAS; ++j)    //                    
            if (iAreaMask & (1 << j))           //                           
                U14UnSetTransfer(hand, (WORD)j);
    }

#ifdef _IS_WINDOWS_
    if (aXferEvent[hand])                       //                                      
    {
        CloseHandle(aXferEvent[hand]);          //                      
        aXferEvent[hand] = NULL;                //                    
    }

    if (CloseHandle(aHand1401[hand]))
#endif
#ifdef LINUX
    if (close(aHand1401[hand]) == 0)            //                           
#endif
    {
        aHand1401[hand] = INVALID_HANDLE_VALUE;
        asType1401[hand] = U14TYPEUNKNOWN;
        return U14ERR_NOERROR;
    }
    else
        return U14ERR_BADHAND;     /*                         */
}

/*                                                                         
  
                                                                           
                                                                         */
U14API(void) U14CloseAll(void)
{
    int i;
    for (i = 0; i < MAX1401; i++)       //                      
        if (aHand1401[i] != INVALID_HANDLE_VALUE)
            U14Close1401((short)i);     //                      
}

/*                                                                           
               
                  
                                                                           */
U14API(short) U14Reset1401(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    return U14Control1401(hand, U14_RESET1401, &csBlock);
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_Reset1401(aHand1401[hand]) : sErr;
#endif
}

/*                                                                           
                
                                                                        
                                    
                                                                            */
U14API(short) U14ForceReset(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    return U14Control1401(hand, U14_FULLRESET, &csBlock);
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_FullReset(aHand1401[hand]) : sErr;
#endif
}

/*                                                                           
                
                                              
                                                                            */
U14API(short) U14KillIO1401(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    return U14Control1401(hand, U14_KILLIO1401, &csBlock);
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_KillIO1401(aHand1401[hand]) : sErr;
#endif
}


/*                                                                           
                
                              
                                                                            */
U14API(short) U14SendString(short hand, const char* pString)
{
    int nChars;                     //                      
    long lTimeOutTicks;             //                 
    BOOL bSpaceToSend;              //                  
    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)
        return sErr;

    nChars = (int)strlen(pString);  //                                  
    if (nChars > MAXSTRLEN)
        return U14ERR_STRLEN;       //                

#ifdef _IS_WINDOWS_
    //                                                           
    lTimeOutTicks = U14WhenToTimeOut(hand);
    do
    {
        bSpaceToSend = (BOOL)((long)U14OutBufSpace(hand) >= nChars);
    }
    while (!bSpaceToSend && !U14PassedTime(lTimeOutTicks));

    if (!bSpaceToSend)             /*                                     */
    {           /*                                                      */
        int i;
        for (i = 0; (i < 4) && (!bSpaceToSend); ++i)
        {
            Sleep(25);       /*                                         */
            bSpaceToSend = (BOOL)((long)U14OutBufSpace(hand) >= nChars);
        }
    }

    if (asLastRetCode[hand] == U14ERR_NOERROR)      /*            */
    {
        if (bSpaceToSend)
        {
            PARAMBLK    rData;
            DWORD       dwBytes;
            char        tstr[MAXSTRLEN+5];          /*                  */

            if ((hand < 0) || (hand >= MAX1401))
                sErr = U14ERR_BADHAND;
            else
            {
                strcpy(tstr, pString);              /*                */
#ifndef _WIN64
                if (!USE_NT_DIOC(hand))             /*                             */
                {
                    int iOK = DeviceIoControl(aHand1401[hand], (DWORD)U14_SENDSTRING,
                                    NULL, 0, tstr, nChars,
                                    &dwBytes, NULL);
                    if (iOK)
                        sErr = (dwBytes >= (DWORD)nChars) ? U14ERR_NOERROR : U14ERR_DRIVCOMMS;
                    else
                        sErr = (short)GetLastError();
                }
                else
#endif
                {
                    int iOK = DeviceIoControl(aHand1401[hand],(DWORD)U14_SENDSTRING,
                                    tstr, nChars,
                                    &rData,sizeof(PARAMBLK),&dwBytes,NULL);
                    if (iOK && (dwBytes >= sizeof(PARAMBLK)))
                        sErr = rData.sState;
                    else
                        sErr = U14ERR_DRIVCOMMS;
                }

                if (sErr != U14ERR_NOERROR) //                             
                    U14ForceReset(hand);    //                             
            }

            return sErr;

        }
        else
        {
            U14ForceReset(hand);                //                             
            return U14ERR_TIMEOUT;
        }
    }
    else
        return asLastRetCode[hand];
#endif
#ifdef LINUX
    //                                          
    sErr = CED_SendString(aHand1401[hand], pString, nChars);
    if (sErr != U14ERR_NOOUT)       //                                            
    {
        if (sErr != U14ERR_NOERROR) //                
             U14ForceReset(hand);   //                                         
        return sErr;                //                                     
    }

    //                                                           
    lTimeOutTicks = U14WhenToTimeOut(hand);
    do
    {
        bSpaceToSend = (BOOL)((long)U14OutBufSpace(hand) >= nChars);
        if (!bSpaceToSend)
            sched_yield();          //                                  
    }
    while (!bSpaceToSend && !U14PassedTime(lTimeOutTicks));

    if (asLastRetCode[hand] == U14ERR_NOERROR)                /*            */
    {
        if (bSpaceToSend)
        {
            sErr = CED_SendString(aHand1401[hand], pString, nChars);
            if (sErr != U14ERR_NOERROR) //                             
                U14ForceReset(hand);    //                             
            return sErr;
        }
        else
        {
            U14ForceReset(hand);                //                             
            return U14ERR_TIMEOUT;
        }
    }
    else
        return asLastRetCode[hand];
#endif
}

/*                                                                           
              
                             
                                                                            */
U14API(short) U14SendChar(short hand, char cChar)
{
#ifdef _IS_WINDOWS_
    char sz[2]=" ";                         //                             
    sz[0] = cChar;
    sz[1] = 0;
    return(U14SendString(hand, sz));        //                           
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_SendChar(aHand1401[hand], cChar) : sErr;
#endif
}

/*                                                                           
               
                                                                
                                                                   
                                                                       
                                                                           
                                                                         
                                                                          
                                                                         
                         
                                                                           */
U14API(short) U14GetString(short hand, char* pBuffer, WORD wMaxLen)
{
    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)             //               
        return sErr;                        //             

#ifdef _IS_WINDOWS_
    if (wMaxLen>1)                          //                                
    {
        BOOL bLineToGet;                    //                        
        long lTimeOutTicks = U14WhenToTimeOut(hand);
        do
            bLineToGet = (BOOL)(U14LineCount(hand) != 0);
        while (!bLineToGet && !U14PassedTime(lTimeOutTicks));

        if (!bLineToGet)             /*                                     */
        {           /*                                                      */
            int i;
            for (i = 0; (i < 4) && (!bLineToGet); ++i)
            {
                Sleep(25);       /*                                         */
                bLineToGet = (BOOL)(U14LineCount(hand) != 0);
            }
        }

        if (bLineToGet)
        {
            if (asLastRetCode[hand] == U14ERR_NOERROR)     /*               */
            {
                DWORD       dwBytes = 0;
                *((WORD *)pBuffer) = wMaxLen;       /*               */
#ifndef _WIN64
                if (!USE_NT_DIOC(hand))             /*                    */
                {
                    char tstr[MAXSTRLEN+5];         /*                        */
                    int iOK;

                    if (wMaxLen > MAXSTRLEN)        /*                 */
                        wMaxLen = MAXSTRLEN;    

                    *((WORD *)tstr) = wMaxLen;      /*         */

                    iOK = DeviceIoControl(aHand1401[hand],(DWORD)U14_GETSTRING,
                                    NULL, 0, tstr, wMaxLen+sizeof(short),
                                    &dwBytes, NULL);
                    if (iOK)                        /*                        */
                    {
                        if (dwBytes >= 0)           /*              */
                        {
                            strcpy(pBuffer, tstr);
                            sErr = U14ERR_NOERROR;
                        }
                        else
                            sErr = U14ERR_DRIVCOMMS;
                    }
                    else
                    {
                        sErr = (short)GetLastError();
                        if (sErr > 0)               /*                */
                            sErr = (short)-sErr;
                    }
                }
                else
#endif
                {       /*                                          */
                    HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE,wMaxLen+sizeof(short));
                    if (hMem)
                    {
                        char* pMem = (char*)GlobalLock(hMem);
                        if (pMem)
                        {
                            int iOK = DeviceIoControl(aHand1401[hand],(DWORD)U14_GETSTRING,
                                            NULL, 0, pMem, wMaxLen+sizeof(short),
                                            &dwBytes, NULL);
                            if (iOK)                /*                        */
                            {
                                if (dwBytes >= wMaxLen)
                                {
                                    strcpy(pBuffer, pMem+sizeof(short));
                                    sErr = *((SHORT*)pMem);
                                }
                                else
                                    sErr = U14ERR_DRIVCOMMS;
                            }
                            else
                                sErr = U14ERR_DRIVCOMMS;

                            GlobalUnlock(hMem);
                        }
                        else
                            sErr = U14ERR_OUTOFMEMORY;

                        GlobalFree(hMem);
                    }
                    else
                        sErr = U14ERR_OUTOFMEMORY;
                }

                if (sErr == U14ERR_NOERROR)     //             
                    TranslateString(pBuffer);   //                                
                else                            //                                
                    U14ForceReset(hand);        //                               

            }
            else
                sErr = asLastRetCode[hand];
        }
        else
        {
            sErr = U14ERR_TIMEOUT;
            U14ForceReset(hand);            //                             
        }
    }
    else
        sErr = U14ERR_BUFF_SMALL;
    return sErr;
#endif
#ifdef LINUX
    if (wMaxLen>1)                          //                                
    {
        BOOL bLineToGet;                    //                        
        long lTimeOutTicks = U14WhenToTimeOut(hand);
        do
        {
            bLineToGet = (BOOL)(U14LineCount(hand) != 0);
            if (!bLineToGet)
                sched_yield();

        }
        while (!bLineToGet && !U14PassedTime(lTimeOutTicks));

        if (bLineToGet)
        {
            sErr = CED_GetString(aHand1401[hand], pBuffer, wMaxLen-1);   //                     
            if (sErr >=0)                    //                 
            {
                if (sErr >= wMaxLen)         //                              
                    sErr = U14ERR_DRIVCOMMS; //                             
                else
                {
                    pBuffer[sErr] = 0;      //                               
                    TranslateString(pBuffer);  //                                 
                }
            }

            if (sErr < U14ERR_NOERROR)       //                             
                U14ForceReset(hand);            //                             
        }
        else
        {
            sErr = U14ERR_TIMEOUT;
            U14ForceReset(hand);            //                             
        }
    }
    else
        sErr = U14ERR_BUFF_SMALL;

    return sErr >= U14ERR_NOERROR ? U14ERR_NOERROR : sErr;
#endif
}

/*                                                                           
             
                                                    
                                                                            */
U14API(short) U14GetChar(short hand, char* pcChar)
{
#ifdef _IS_WINDOWS_
    char sz[2];                             //                         
    short sErr = U14GetString(hand, sz, 2); //                                   
    *pcChar = sz[0];    //                                                    
    if (sErr == U14ERR_NOERROR)
    {                                       //                             
        if (*pcChar == '\0')                //                   
            *pcChar = '\n';                 //                                 
    }
    return sErr;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)             //                 
        return sErr;
    sErr = CED_GetChar(aHand1401[hand]);    //                           
    if (sErr >= 0)
    {
        *pcChar = (char)sErr;              //                         
        return U14ERR_NOERROR;              //           
    }
    else
        return sErr;
#endif
}

/*                                                                           
              
                                                                    
                                                                           */
U14API(short) U14Stat1401(short hand)
{
    return ((short)(U14LineCount(hand) > 0));
}

/*                                                                           
               
                                                       
                                                                            */
U14API(short) U14CharCount(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    short sErr = U14Status1401(hand, U14_STAT1401, &csBlock);
    if (sErr == U14ERR_NOERROR)
        sErr = csBlock.ints[0];
    return sErr;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_Stat1401(aHand1401[hand]) : sErr;
#endif
}

/*                                                                           
               
                                                          
                                                                            */
U14API(short) U14LineCount(short hand)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    short sErr = U14Status1401(hand, U14_LINECOUNT, &csBlock);
    if (sErr == U14ERR_NOERROR)
        sErr = csBlock.ints[0];
    return sErr;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_LineCount(aHand1401[hand]) : sErr;
#endif
}

/*                                                                           
                    
                                                               
                                                            
                                                               
                                                               
                                                                
                       
                                                                           */
U14API(void)  U14GetErrorString(short nErr, char* pStr, WORD wMax)
{
    char    wstr[150];

    switch (nErr)              /*                                           */
    {
    case U14ERR_OFF:
        sprintf(wstr, "The 1401 is apparently switched off (code %d)", nErr);
        break;

    case U14ERR_NC:
        sprintf(wstr, "The 1401 is not connected to the interface card (code %d)", nErr);
        break;

    case U14ERR_ILL:
        sprintf(wstr, "The 1401 is not working correctly (code %d)", nErr);
        break;

    case U14ERR_NOIF:
        sprintf(wstr, "The 1401 interface card was not detected (code %d)", nErr);
        break;

    case U14ERR_TIME:
        sprintf(wstr, "The 1401 fails to become ready for use (code %d)", nErr);
        break;

    case U14ERR_BADSW:
        sprintf(wstr, "The 1401 interface card jumpers are incorrect (code %d)", nErr);
        break;

    case U14ERR_NOINT:
        sprintf(wstr, "The 1401 interrupt is not available for use (code %d)", nErr);
        break;

    case U14ERR_INUSE:
        sprintf(wstr, "The 1401 is already in use by another program (code %d)", nErr);
        break;

    case U14ERR_NODMA:
        sprintf(wstr, "The 1401 DMA channel is not available for use (code %d)", nErr);
        break;

    case U14ERR_BADHAND:
        sprintf(wstr, "The application supplied an incorrect 1401 handle (code %d)", nErr);
        break;

    case U14ERR_BAD1401NUM:
        sprintf(wstr, "The application used an incorrect 1401 number (code %d)", nErr);
        break;

    case U14ERR_NO_SUCH_FN:
        sprintf(wstr, "The code passed to the 1401 driver is invalid (code %d)", nErr);
        break;

    case U14ERR_NO_SUCH_SUBFN:
        sprintf(wstr, "The sub-code passed to the 1401 driver is invalid (code %d)", nErr);
        break;

    case U14ERR_NOOUT:
        sprintf(wstr, "No room in buffer for characters for the 1401 (code %d)", nErr);
        break;

    case U14ERR_NOIN:
        sprintf(wstr, "No characters from the 1401 are available (code %d)", nErr);
        break;

    case U14ERR_STRLEN:
        sprintf(wstr, "A string sent to or read from the 1401 was too long (code %d)", nErr);
        break;

    case U14ERR_LOCKFAIL:
        sprintf(wstr, "Failed to lock host memory for data transfer (code %d)", nErr);
        break;

    case U14ERR_UNLOCKFAIL:
        sprintf(wstr, "Failed to unlock host memory after data transfer (code %d)", nErr);
        break;

    case U14ERR_ALREADYSET:
        sprintf(wstr, "The transfer area used is already set up (code %d)", nErr);
        break;

    case U14ERR_NOTSET:
        sprintf(wstr, "The transfer area used has not been set up (code %d)", nErr);
        break;

    case U14ERR_BADAREA:
        sprintf(wstr, "The transfer area number is incorrect (code %d)", nErr);
        break;

    case U14ERR_NOFILE:
        sprintf(wstr, "The command file %s could not be opened (code %d)", szLastName, nErr);
        break;

    case U14ERR_READERR:
        sprintf(wstr, "The command file %s could not be read (code %d)", szLastName, nErr);
        break;

    case U14ERR_UNKNOWN:
        sprintf(wstr, "The %s command resource could not be found (code %d)", szLastName, nErr);
        break;

    case U14ERR_HOSTSPACE:
        sprintf(wstr, "Unable to allocate memory for loading command %s (code %d)", szLastName, nErr);
        break;

    case U14ERR_LOCKERR:
        sprintf(wstr, "Unable to lock memory for loading command %s (code %d)", szLastName, nErr);
        break;

    case U14ERR_CLOADERR:
        sprintf(wstr, "Error in loading command %s, bad command format (code %d)", szLastName, nErr);
        break;

    case U14ERR_TOXXXERR:
        sprintf(wstr, "Error detected after data transfer to or from the 1401 (code %d)", nErr);
        break;

    case U14ERR_NO386ENH:
        sprintf(wstr, "Windows 3.1 is not running in 386 enhanced mode (code %d)", nErr);
        break;

    case U14ERR_NO1401DRIV:
        sprintf(wstr, "The 1401 device driver cannot be found (code %d)\nUSB:   check plugged in and powered\nOther: not installed?", nErr);
        break;

    case U14ERR_DRIVTOOOLD:
        sprintf(wstr, "The 1401 device driver is too old for use (code %d)", nErr);
        break;

    case U14ERR_TIMEOUT:
        sprintf(wstr, "Character transmissions to the 1401 timed-out (code %d)", nErr);
        break;

    case U14ERR_BUFF_SMALL:
        sprintf(wstr, "Buffer for text from the 1401 was too small (code %d)", nErr);
        break;

    case U14ERR_CBALREADY:
        sprintf(wstr, "1401 monitor callback already set up (code %d)", nErr);
        break;

    case U14ERR_BADDEREG:
        sprintf(wstr, "1401 monitor callback deregister invalid (code %d)", nErr);
        break;

    case U14ERR_DRIVCOMMS:
        sprintf(wstr, "1401 device driver communications failed (code %d)", nErr);
        break;

    case U14ERR_OUTOFMEMORY:
        sprintf(wstr, "Failed to allocate or lock memory for text from the 1401 (code %d)", nErr);
        break;

    default:
        sprintf(wstr, "1401 error code %d returned; this code is unknown", nErr);
        break;

    }
    if ((WORD)strlen(wstr) >= wMax-1)  /*                                 */
        wstr[wMax-1] = 0;                          /*                       */
    strcpy(pStr, wstr);                       /*                         */
}

/*                                                                          
                 
                                                                     
                                                                          */
U14API(short) U14GetTransfer(short hand, TGET_TX_BLOCK *pTransBlock)
{
    short sErr = CheckHandle(hand);
#ifdef _IS_WINDOWS_
    if (sErr == U14ERR_NOERROR)
    { 
        DWORD dwBytes = 0;
        BOOL bOK = DeviceIoControl(aHand1401[hand], (DWORD)U14_GETTRANSFER, NULL, 0, pTransBlock,
                              sizeof(TGET_TX_BLOCK), &dwBytes, NULL);
    
        if (bOK && (dwBytes >= sizeof(TGET_TX_BLOCK)))
            sErr = U14ERR_NOERROR;
        else
            sErr = U14ERR_DRIVCOMMS;
    }
    return sErr;
#endif
#ifdef LINUX
    return (sErr == U14ERR_NOERROR) ? CED_GetTransfer(aHand1401[hand], pTransBlock) : sErr;
#endif
}
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
U14API(short) U14WorkingSet(DWORD dwMinKb, DWORD dwMaxKb)
{
#ifdef _IS_WINDOWS_
    short sRetVal = 0;                      //                  
    HANDLE hProcess;
    DWORD dwVer = GetVersion();
	if (dwVer & 0x80000000)                 //                
        return 0;                           //                       

    //                                                   
    hProcess = OpenProcess(STANDARD_RIGHTS_REQUIRED |
                                  PROCESS_QUERY_INFORMATION |
                                  PROCESS_SET_QUOTA,
                                  FALSE, _getpid());
    if (hProcess)
    {
        SIZE_T dwMinSize,dwMaxSize;
        if (GetProcessWorkingSetSize(hProcess, &dwMinSize, &dwMaxSize))
        {
            DWORD dwMin = dwMinKb << 10;    //                         
            DWORD dwMax = dwMaxKb << 10;

            //                                                         
            if (dwMin > dwMinSize)          //                      
                dwMinSize = dwMin;

            if (dwMax > dwMaxSize)
                dwMaxSize = dwMax;

            if (!SetProcessWorkingSetSize(hProcess, dwMinSize, dwMaxSize))
                sRetVal = 3;                //                      
        }
        else
            sRetVal = 2;                    //                             

        CloseHandle(hProcess);
    }
    else
        sRetVal = 1;            //                     

    return sRetVal;
#endif
#ifdef LINUX
    if (dwMinKb | dwMaxKb)
    {
        //                         
    }
    return U14ERR_NOERROR;
#endif
}

/*                                                                           
                                            
                                                                  
                                                                            */
U14API(short) U14UnSetTransfer(short hand, WORD wArea)
{
    short sErr = CheckHandle(hand);
#ifdef _IS_WINDOWS_
    if (sErr == U14ERR_NOERROR)
    {
       TCSBLOCK csBlock;
       csBlock.ints[0] = (short)wArea;       /*                                */
       sErr = U14Control1401(hand, U14_UNSETTRANSFER, &csBlock);  /*           */
   
       VirtualUnlock(apAreas[hand][wArea], auAreas[hand][wArea]);/*        */
       apAreas[hand][wArea] = NULL;                         /*                 */
       auAreas[hand][wArea] = 0;
    }
    return sErr;
#endif
#ifdef LINUX
    return (sErr == U14ERR_NOERROR) ? CED_UnsetTransfer(aHand1401[hand], wArea) : sErr;
#endif
}

/*                                                                           
                                                                
                                            
                                                          
                                                        
                                                                       
                                                                           */
U14API(short) U14SetTransArea(short hand, WORD wArea, void *pvBuff,
                                          DWORD dwLength, short eSz)
{
    TRANSFERDESC td;
    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)
        return sErr;
    if (wArea >= MAX_TRANSAREAS)                    //                            
        return U14ERR_BADAREA;

#ifdef _IS_WINDOWS_
    assert(apAreas[hand][wArea] == NULL);
    assert(auAreas[hand][wArea] == 0);

    apAreas[hand][wArea] = pvBuff;                  /*                     */
    auAreas[hand][wArea] = dwLength;

    if (!VirtualLock(pvBuff, dwLength))             /*                        */
    {
        apAreas[hand][wArea] = NULL;                /*                 */
        auAreas[hand][wArea] = 0;
        return U14ERR_LOCKERR;                      /*                    */
    }
#ifndef _WIN64
    if (!USE_NT_DIOC(hand))                         /*                  */
    {
        DWORD dwBytes;
        VXTRANSFERDESC vxDesc;                      /*                          */
        vxDesc.wArea = wArea;                       /*                           */
        vxDesc.dwLength = dwLength;

        //                                                           
        if ((wArea != 0) && (U14DriverVersion(hand) < 0x00010002L))
            sErr = U14ERR_DRIVTOOOLD;
        else
        {
            vxDesc.dwAddrOfs = (DWORD)pvBuff;       /*               */
            vxDesc.wAddrSel  = 0;

            if (DeviceIoControl(aHand1401[hand], (DWORD)U14_SETTRANSFER,
                                pvBuff,dwLength,    /*                        */
                                &vxDesc,sizeof(VXTRANSFERDESC),
                                &dwBytes,NULL))
            {
                if (dwBytes >= sizeof(VXTRANSFERDESC)) /*             */
                    sErr = U14ERR_NOERROR;
                else
                    sErr = U14ERR_DRIVCOMMS;        /*                      */
            }
            else
                sErr = (short)GetLastError();
        }
    }
    else
#endif
    {
        PARAMBLK rWork;
        DWORD dwBytes;
        td.wArea = wArea;     /*                                */
        td.lpvBuff = pvBuff;
        td.dwLength = dwLength;
        td.eSize = 0;                //                   

        if (DeviceIoControl(aHand1401[hand],(DWORD)U14_SETTRANSFER,
                            &td,sizeof(TRANSFERDESC),
                            &rWork,sizeof(PARAMBLK),&dwBytes,NULL))
        {
            if (dwBytes >= sizeof(PARAMBLK))    //                         
                sErr = rWork.sState;            //                      
            else
                sErr = U14ERR_DRIVCOMMS;        //                     
        }
        else
            sErr = U14ERR_DRIVCOMMS;
    }

    if (sErr != U14ERR_NOERROR)
    {
        if (sErr != U14ERR_LOCKERR)             //                      
            VirtualUnlock(pvBuff, dwLength);    //                    
        apAreas[hand][wArea] = NULL;            //                
        auAreas[hand][wArea] = 0;
    }

    return sErr;
#endif
#ifdef LINUX
    //                                                                               
    //                       
    td.lpvBuff = (long long)((unsigned long)pvBuff);
    td.wAreaNum = wArea;
    td.dwLength = dwLength;
    td.eSize = eSz;                //                   
    return CED_SetTransfer(aHand1401[hand], &td);
#endif
}

/*                                                                           
                                                                     
                                                            
                                                             
                                                                  
                                                          
                                                        
                                                      
  
                                                                       
                                                                       
                                                                        
                                                                          
                                                                            
                                            
  
                                                                          
                                
                                                                           */
U14API(short) U14SetTransferEvent(short hand, WORD wArea, BOOL bEvent,
                                  BOOL bToHost, DWORD dwStart, DWORD dwLength)
{
#ifdef _IS_WINDOWS_
    TCSBLOCK csBlock;
    short sErr = U14TransferFlags(hand);        //                            
    if (sErr >= U14ERR_NOERROR)                 //                   
    {
        bEvent = bEvent && ((sErr & U14TF_NOTIFY) != 0); //                                      
        if (wArea >= MAX_TRANSAREAS)            //                      
            return U14ERR_BADAREA;              //                   

        //                                                                    
        //                    
        if ((bEvent != 0) != (aXferEvent[hand] != 0))    //                       
        {
            if (bEvent)                         //                          
                aXferEvent[hand] = CreateEvent(NULL, FALSE, FALSE, NULL);
            else
            {
                CloseHandle(aXferEvent[hand]);  //                         
                aXferEvent[hand] = NULL;        //                 
            }
        }

        //                                                                  
        //                                                                
        //                                       
#ifdef _WIN64
        csBlock.longs[0] = wArea;               //                                   
        if (bToHost != 0)                       //                                  
            csBlock.longs[0] |= 0x10000;        //                                       
        *((HANDLE*)&csBlock.longs[1]) = aXferEvent[hand];  //                            
        csBlock.longs[3] = dwStart;             //                            
        csBlock.longs[4] = dwLength;            //                           
#else
        csBlock.longs[0] = wArea;               //                                   
        csBlock.longs[1] = (long)aXferEvent[hand];    //                               
        csBlock.longs[2] = bToHost;
        csBlock.longs[3] = dwStart;
        csBlock.longs[4] = dwLength;
#endif
        sErr = U14Control1401(hand, U14_SETTRANSEVENT, &csBlock);
        if (sErr == U14ERR_NOERROR)
            sErr = (short)(aXferEvent[hand] != NULL);    //                         
    }

    return sErr;
#endif
#ifdef LINUX
    TRANSFEREVENT te;
    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)
        return sErr;

    if (wArea >= MAX_TRANSAREAS)            //                            
        return U14ERR_BADAREA;

    te.wAreaNum = wArea;                    //                                     
    te.wFlags = bToHost ? 1 : 0;            //                         
    te.dwStart = dwStart;                   //                               
    te.dwLength = dwLength;                 //                       
    te.iSetEvent = bEvent;                  //                                            
    return CED_SetEvent(aHand1401[hand], &te);
#endif
}

/*                                                                           
                       
                                                                          
                                            
                                                                           */
U14API(int) U14TestTransferEvent(short hand, WORD wArea)
{
#ifdef _IS_WINDOWS_
    int iErr = CheckHandle(hand);
    if (iErr == U14ERR_NOERROR)
    {
        if (aXferEvent[hand])           //                      
            iErr = WaitForSingleObject(aXferEvent[hand], 0) == WAIT_OBJECT_0;
    }
    return iErr;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_TestEvent(aHand1401[hand], wArea) : sErr;
#endif
}

/*                                                                           
                       
                                            
                                                                           
                                                   
                                                                         
                                                                    
                                                                           */
U14API(int) U14WaitTransferEvent(short hand, WORD wArea, int msTimeOut)
{
#ifdef _IS_WINDOWS_
    int iErr = CheckHandle(hand);
    if (iErr == U14ERR_NOERROR)
    {
        if (aXferEvent[hand])
        {
            if (msTimeOut == 0)
                msTimeOut = INFINITE;
            iErr = WaitForSingleObject(aXferEvent[hand], msTimeOut) != WAIT_OBJECT_0;
        }
        else
            iErr = TRUE;                //                             
    }
    return iErr;
#endif
#ifdef LINUX
    short sErr = CheckHandle(hand);
    return (sErr == U14ERR_NOERROR) ? CED_WaitEvent(aHand1401[hand], wArea, msTimeOut) : sErr;
#endif
}

/*                                                                           
                                                               
                                                 
                                                           
                                                             
                                                             
                                                                           */
U14API(short) U14SetCircular(short hand, WORD wArea, BOOL bToHost,
									void *pvBuff, DWORD dwLength)
{
    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)
        return sErr;

    if (wArea >= MAX_TRANSAREAS)         /*                             */
        return U14ERR_BADAREA;

	if (!bToHost)             /*                                        */
        return U14ERR_BADAREA;            /*                            */
#ifdef _IS_WINDOWS_
    assert(apAreas[hand][wArea] == NULL);
    assert(auAreas[hand][wArea] == 0);

    apAreas[hand][wArea] = pvBuff;              /*                     */
    auAreas[hand][wArea] = dwLength;

    if (!VirtualLock(pvBuff, dwLength))      /*                        */
        sErr = U14ERR_LOCKERR;                    /*                    */
    else
    {
        PARAMBLK rWork;
        DWORD dwBytes;
        TRANSFERDESC txDesc;
        txDesc.wArea = wArea;             /*                                */
        txDesc.lpvBuff = pvBuff;
        txDesc.dwLength = dwLength;
        txDesc.eSize = (short)bToHost;       /*                             */
   
        if (DeviceIoControl(aHand1401[hand],(DWORD)U14_SETCIRCULAR,
                           &txDesc, sizeof(TRANSFERDESC),
                           &rWork, sizeof(PARAMBLK),&dwBytes,NULL))
        {
           if (dwBytes >= sizeof(PARAMBLK))          /*                    */
               sErr = rWork.sState;         /*                             */
           else
               sErr = U14ERR_DRIVCOMMS;            /*                      */
        }
        else
            sErr = U14ERR_DRIVCOMMS;
    }

    if (sErr != U14ERR_NOERROR)
    {
        if (sErr != U14ERR_LOCKERR)
            VirtualUnlock(pvBuff, dwLength);         /*                 */
        apAreas[hand][wArea] = NULL;                 /*                 */
        auAreas[hand][wArea] = 0;
    }

    return sErr;
#endif
#ifdef LINUX
    else
    {
        TRANSFERDESC td;
        td.lpvBuff = (long long)((unsigned long)pvBuff);
        td.wAreaNum = wArea;
        td.dwLength = dwLength;
        td.eSize = (short)bToHost;       /*                             */
        return CED_SetCircular(aHand1401[hand], &td);
    }
#endif
}

/*                                                                           
                                                                     
                                              
                                                                           */
U14API(int) U14GetCircBlk(short hand, WORD wArea, DWORD *pdwOffs)
{
    int lErr = CheckHandle(hand);
    if (lErr != U14ERR_NOERROR)
        return lErr;

    if (wArea >= MAX_TRANSAREAS)            //                             
        return U14ERR_BADAREA;
    else
    {
#ifdef _IS_WINDOWS_
        PARAMBLK rWork;
        TCSBLOCK csBlock;
        DWORD dwBytes;
        csBlock.longs[0] = wArea;               //                               
        rWork.sState = U14ERR_DRIVCOMMS;
        if (DeviceIoControl(aHand1401[hand], (DWORD)U14_GETCIRCBLK, &csBlock, sizeof(TCSBLOCK), &rWork, sizeof(PARAMBLK), &dwBytes, NULL) &&
           (dwBytes >= sizeof(PARAMBLK)))
            lErr = rWork.sState;
        else
            lErr = U14ERR_DRIVCOMMS;
   
        if (lErr == U14ERR_NOERROR)             //                      
        {                                       //                                  
            lErr = rWork.csBlock.longs[1];      //                             
            *pdwOffs = rWork.csBlock.longs[0];  //                         
        }
#endif
#ifdef LINUX
        TCIRCBLOCK cb;
        cb.nArea = wArea;                       //                               
        cb.dwOffset = 0;
        cb.dwSize = 0;
        lErr = CED_GetCircBlock(aHand1401[hand], &cb);
        if (lErr == U14ERR_NOERROR)             //                      
        {                                       //                                  
            lErr = cb.dwSize;                   //                
            *pdwOffs = cb.dwOffset;             //               
        }
#endif
    }
    return lErr;
}

/*                                                                           
                                                                       
                                                                        
                                                                  
                                                                           */
U14API(int) U14FreeCircBlk(short hand, WORD wArea, DWORD dwOffs, DWORD dwSize,
                                        DWORD *pdwOffs)
{
    int lErr = CheckHandle(hand);
    if (lErr != U14ERR_NOERROR)
        return lErr;

    if (wArea < MAX_TRANSAREAS)                 //                            
    {
#ifdef _IS_WINDOWS_
        PARAMBLK rWork;
        TCSBLOCK csBlock;
        DWORD dwBytes;
        csBlock.longs[0] = wArea;               //                               
        csBlock.longs[1] = dwOffs;
        csBlock.longs[2] = dwSize;
        rWork.sState = U14ERR_DRIVCOMMS;
        if (DeviceIoControl(aHand1401[hand], (DWORD)U14_FREECIRCBLK, &csBlock, sizeof(TCSBLOCK),
                           &rWork, sizeof(PARAMBLK), &dwBytes, NULL) &&
           (dwBytes >= sizeof(PARAMBLK)))
           lErr = rWork.sState;
        else
           lErr = U14ERR_DRIVCOMMS;
       if (lErr == U14ERR_NOERROR)             //                        
       {                                       //                                  
           lErr = rWork.csBlock.longs[1];      //                             
           *pdwOffs = rWork.csBlock.longs[0];  //                         
       }
#endif
#ifdef LINUX
        TCIRCBLOCK cb;
        cb.nArea = wArea;                       //                               
        cb.dwOffset = dwOffs;
        cb.dwSize = dwSize;
    
        lErr = CED_FreeCircBlock(aHand1401[hand], &cb);
        if (lErr == U14ERR_NOERROR)             //                        
        {                                       //                                  
            lErr = cb.dwSize;                   //                             
            *pdwOffs = cb.dwOffset;             //                         
        }
#endif
    }
    else
        lErr = U14ERR_BADAREA;

    return lErr;
}

/*                                                                           
           
                                         
                                          
                                      
                                                                            */
static short Transfer(short hand, BOOL bTo1401, char* pData,
                       DWORD dwSize, DWORD dw1401, short eSz)
{
    char strcopy[MAXSTRLEN+1];          //                            
    short sResult = U14SetTransArea(hand, 0, (void *)pData, dwSize, eSz);
    if (sResult == U14ERR_NOERROR)      //         
    {
        sprintf(strcopy,                //                        
                "TO%s,$%X,$%X,0;", bTo1401 ? "1401" : "HOST", dw1401, dwSize);

        U14SendString(hand, strcopy);   //                     

        sResult = U14CheckErr(hand);    //                                  
        if (sResult > 0)
            sResult = U14ERR_TOXXXERR;  //                               

        U14UnSetTransfer(hand, 0);
    }
    return sResult;
}

/*                                                                           
                                                              
                                                                           */
U14API(short) U14ToHost(short hand, char* pAddrHost, DWORD dwSize,
                                            DWORD dw1401, short eSz)
{
    short sErr = CheckHandle(hand);
    if ((sErr == U14ERR_NOERROR) && dwSize) //                     
        sErr = Transfer(hand, TOHOST, pAddrHost, dwSize, dw1401, eSz);
    return sErr;
}

/*                                                                           
                                                              
                                                                           */
U14API(short) U14To1401(short hand, const char* pAddrHost,DWORD dwSize,
                                    DWORD dw1401, short eSz)
{
    short sErr = CheckHandle(hand);
    if ((sErr == U14ERR_NOERROR) && dwSize) //                     
        sErr = Transfer(hand, TO1401, (char*)pAddrHost, dwSize, dw1401, eSz);
    return sErr;
}

/*                                                                           
                                                                     
                                                                            */
#ifdef _IS_WINDOWS_
#define file_exist(name) (_access(name, 0) != -1)
#define file_open(name) _lopen(name, OF_READ)
#define file_close(h)   _lclose(h)
#define file_seek(h, pos) _llseek(h, pos, FILE_BEGIN) 
#define file_read(h, buffer, size) (_lread(h, buffer, size) == size)
#endif
#ifdef LINUX
#define file_exist(name) (access(name, F_OK) != -1)
#define file_open(name) open(name, O_RDONLY)
#define file_close(h)   close(h)
#define file_seek(h, pos) lseek(h, pos, SEEK_SET) 
#define file_read(h, buffer, size) (read(h, buffer, size) == (ssize_t)size)
static DWORD GetModuleFileName(void* dummy, char* buffer, int max)
{
    //                                                                
    char szProcPath[32];
    sprintf(szProcPath, "/proc/%d/exe", getpid());  //                     
    if (readlink(szProcPath, buffer, max) != -1)
    {
        dirname (buffer);
        strcat  (buffer, "/");
        return strlen(buffer);
    }
    return 0;
}
#endif

U14API(short) U14LdCmd(short hand, const char* command)
{
    char strcopy[MAXSTRLEN+1];      //                            
    BOOL bGotIt = FALSE;            //                                
    int iFHandle;                   //                       
#define FNSZ 260
    char filnam[FNSZ];              //                       
    char szCmd[25];                 //                                     

    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)
        return sErr;

    if (strchr(command, '.') != NULL)       //                         
    {
        if (file_exist(command))            //                   
        {
            strcpy(filnam, command);        //               
            bGotIt = TRUE;                  //                       
        }
        else                                //                                    
        {
            char* pStr = strrchr(command, PATHSEP);  //                        
            if (pStr != NULL)               //                
            {
                pStr++;                     //                        
                strcpy(szCmd, pStr);        //                     
            }
            else
                strcpy(szCmd, command);     //          
        }
    }
    else    //                                                            
    {
        char szExt[8];
        strcpy(szCmd, command);             //                        
        ExtForType(asType1401[hand], szExt);//                      
        strcat(szCmd, szExt);               //                  
    }

    //                                                                  
    //                          
    if (!bGotIt)                            //                  
    {
        DWORD dwLen = GetModuleFileName(NULL, filnam, FNSZ); //             
        if (dwLen > 0)                      //                            
        {
            char* pStr = strrchr(filnam, PATHSEP);    //                        
            if (pStr != NULL)
            {
                *(++pStr) = 0;                  //                       
                if (strlen(filnam) < FNSZ-6)    //                        
                {
                    strcat(filnam, "1401" PATHSEPSTR);  //                   
                    strcat(filnam,szCmd);
                    bGotIt = (BOOL)file_exist(filnam);  //                   
                }
            }
        }
    }

    //                                                                         
    //                        
    if (!bGotIt)                            //                           
    {
        char* pStr = getenv("1401DIR");     //                            
        if (pStr != NULL)                   //                            
        {
            strcpy(filnam, pStr);                   //                        
            if (filnam[strlen(filnam)-1] != PATHSEP)//                  
                strcat(filnam, PATHSEPSTR);
            strcat(filnam, szCmd);
            bGotIt = (BOOL)file_exist(filnam); //              
        }
    }

    //                                            
    if (!bGotIt)                        //                          
    {
        strcpy(filnam, DEFCMDPATH);     //                 
        strcat(filnam, szCmd);
        bGotIt = file_exist(filnam);    //              
    }

    iFHandle = file_open(filnam);
    if (iFHandle == -1)
        sErr = U14ERR_NOFILE;
    else
    {                                   //                               
        CMDHEAD rCmdHead;               //                           
        if (file_read(iFHandle, &rCmdHead, sizeof(CMDHEAD)))
        {
            size_t nComSize = rCmdHead.wCmdSize;
            char* pMem = malloc(nComSize);
            if (pMem != NULL)
            {
                file_seek(iFHandle, sizeof(CMDHEAD));
                if (file_read(iFHandle, pMem, (UINT)nComSize))
                {
                    sErr = U14SetTransArea(hand, 0, (void *)pMem, (DWORD)nComSize, ESZBYTES);
                    if (sErr == U14ERR_NOERROR)
                    {
                        sprintf(strcopy, "CLOAD,0,$%X;", (int)nComSize);
                        sErr = U14SendString(hand, strcopy);
                        if (sErr == U14ERR_NOERROR)
                        {
                            sErr = U14CheckErr(hand);     //                          
                            if (sErr > 0)
                                sErr = U14ERR_CLOADERR;   //                       
                        }
                        U14UnSetTransfer(hand, 0);  //                      
                    }
                }
                else
                    sErr = U14ERR_READERR;
                free(pMem);
            }
            else
                sErr = U14ERR_HOSTSPACE;    //                       
        }
        else
            sErr = U14ERR_READERR;

        file_close(iFHandle);               //               
    }

    return sErr;
}


/*                                                                           
     
                                
                                                                    
                                   
                                                                           */
U14API(DWORD) U14Ld(short hand, const char* vl, const char* str)
{
    DWORD dwIndex = 0;              //                         
    long lErr = U14ERR_NOERROR;     //                                   
    char strcopy[MAXSTRLEN+1];      //                                
    char szFExt[8];                 //                           
    short sErr = CheckHandle(hand);
    if (sErr != U14ERR_NOERROR)
        return sErr;

    ExtForType(asType1401[hand], szFExt);   //                      
    strcpy(strcopy, str);               //                           

    //                                                      
    if (*str)                           //                  
    {
        BOOL bDone = FALSE;             //                                
        int iLoop1 = 0;                 //                                          
        int iLoop2 = 0;                 //                              
        do                              //                        
        {
            char filnam[MAXSTRLEN+1];   //                
            char szFName[MAXSTRLEN+1];  //                     

            if (!strcopy[iLoop1])       //                          
                bDone = TRUE;           //                    

            if (bDone || (strcopy[iLoop1] == ','))  //            
            {
                U14LONG er[5];                  //                                
                ++dwIndex;                      //                                         
                szFName[iLoop2]=(char)0;        //                               

                strncpy(szLastName, szFName, sizeof(szLastName));    //                    
                szLastName[sizeof(szLastName)-1] = 0;
                strncat(szLastName, szFExt, sizeof(szLastName));     //                        
                szLastName[sizeof(szLastName)-1] = 0;

                U14SendString(hand, szFName);   //              
                U14SendString(hand, ";ERR;");   //               

                lErr = U14LongsFrom1401(hand, er, 5);
                if (lErr > 0)
                {
                    lErr = U14ERR_NOERROR;
                    if (er[0] == 255)           //                             
                    {
                        if (vl && *vl)          //                       
                        {
                            strcpy(filnam, vl);
                            if (strchr("\\/:", filnam[strlen(filnam)-1]) == NULL)
                                strcat(filnam, PATHSEPSTR); //                            
                            strcat(filnam, szFName);    //                  
                            strcat(filnam, szFExt);     //              
                        }
                        else
                            strcpy(filnam, szFName);    //            

                        lErr = U14LdCmd(hand, filnam);  //         
                        if (lErr != U14ERR_NOERROR)     //                
                            bDone = TRUE;               //                    
                    }
                }
                else
                    bDone = TRUE;       //                    

                iLoop2 = 0;             //                                     
                ++iLoop1;               //                                  
            }
            else
                szFName[iLoop2++] = strcopy[iLoop1++];  //                               
        }
        while (!bDone);
    }

    if (lErr == U14ERR_NOERROR)
    {
        szLastName[0] = 0;      //                                         
        return lErr;
    }
    else
        return ((dwIndex<<16) | ((DWORD)lErr & 0x0000FFFF));
}

//                                                                           
U14API(int) U14InitLib(void)
{
    int iRetVal = U14LIB_VERSION;
    if (iAttached == 0)         //                                   
    {
        int i;
#ifdef _IS_WINDOWS_
        int j;
        DWORD   dwVersion = GetVersion();
        bWindows9x = FALSE;                  //                 

        if (dwVersion & 0x80000000)                 //                  
        {
            if ((LOBYTE(LOWORD(dwVersion)) < 4) &&  //                
                 (HIBYTE(LOWORD(dwVersion)) < 95))  //                    
            iRetVal = 0;                            //                       
        else
            bWindows9x = TRUE;                      //                   
        }
#endif
        
        for (i = 0; i < MAX1401; i++)               //                           
        {
            aHand1401[i] = INVALID_HANDLE_VALUE;    //                    
            asType1401[i] = U14TYPEUNKNOWN;         //                    
            alTimeOutPeriod[i] = 3000;              //                  
#ifdef _IS_WINDOWS_
#ifndef _WIN64
            abUseNTDIOC[i] = (BOOL)!bWindows9x;
#endif
            aXferEvent[i] = NULL;                   //                         
            for (j = 0; j < MAX_TRANSAREAS; j++)    //                           
            {
                apAreas[i][j] = NULL;
                auAreas[i][j] = 0;
            }
#endif
        }
    }
    return iRetVal;
}

//                                                                                 
//                                                                                 
//                    


#ifdef _IS_WINDOWS_
#ifndef U14_NOT_DLL
/*                                                                           
                                           
                                                                             
                                                                             
                 
                                                                           */
INT APIENTRY DllMain(HANDLE hInst, DWORD ul_reason_being_called, LPVOID lpReserved)
{
    int iRetVal = 1;

    switch (ul_reason_being_called)
    {
    case DLL_PROCESS_ATTACH:
        iRetVal = U14InitLib() > 0;         //                               
        ++iAttached;                        //                     
        break;

    case DLL_PROCESS_DETACH:
        if (--iAttached == 0)               //              
            U14CloseAll();                  //                         
        break;
    }
    return iRetVal;

    UNREFERENCED_PARAMETER(lpReserved);
}
#endif
#endif
#ifdef LINUX
void __attribute__((constructor)) use1401_load(void)
{
    U14InitLib();
    ++iAttached;
}

void __attribute__((destructor)) use1401_unload(void)
{
        if (--iAttached == 0)               //              
            U14CloseAll();                  //                         
}
#endif
