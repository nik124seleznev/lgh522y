/*
 * IOCTL calls for the CED1401 driver
 * Copyright (C) 2010 Cambridge Electronic Design Ltd
 * Author Greg P Smith (greg@ced.co.uk)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __CED_IOCTL_H__
#define __CED_IOCTL_H__

#include <linux/ioctl.h>

/*                                                                   */
#define MODE_CHAR		0
#define MODE_LINEAR		1

/*                                                                           
           
                                                                            */

typedef unsigned short TBLOCKENTRY;	/*                                  */

typedef struct TransferDesc {
	long long lpvBuff;	/*                                             */
	unsigned int dwLength;	/*                    */
	TBLOCKENTRY wAreaNum;	/*                                   */
	short eSize;		/*                                            */
} TRANSFERDESC;

typedef TRANSFERDESC * LPTRANSFERDESC;

typedef struct TransferEvent {
	unsigned int dwStart;		/*                      */
	unsigned int dwLength;		/*                      */
	unsigned short wAreaNum;	/*                 */
	unsigned short wFlags;		/*                      */
	int iSetEvent;			/*                         */
} TRANSFEREVENT;

#define MAX_TRANSFER_SIZE	0x4000		/*                            */
#define MAX_AREA_LENGTH		0x100000	/*                               */
#define MAX_TRANSAREAS		8		/*                             */

typedef struct TGetSelfTest {
	int code;			/*                      */
	int x, y;			/*                        */
} TGET_SELFTEST;

/*                                                                                  */
typedef struct TDbgBlock {
	int iAddr;			/*                         */
	int iRepeats;			/*                   */
	int iWidth;			/*                        */
	int iDefault;			/*               */
	int iMask;			/*               */
	int iData;			/*                                */
} TDBGBLOCK;

/*                                                                           */
typedef struct TCircBlock {
	unsigned int nArea;		/*                                      */
	unsigned int dwOffset;		/*                                             */
	unsigned int dwSize;		/*                  */
} TCIRCBLOCK;

/*                                  */
typedef struct TCSBlock {
	unsigned int uiState;
	unsigned int uiError;
} TCSBLOCK;

/*
                                                                            
                                                                              
                                                                            
                                                                         
 */
#define CED_MAGIC_IOC 0xce

/*                                                                          */
typedef struct ced_ioc_string {
	int nChars;
	char buffer[256];
} CED_IOC_STRING;

#define IOCTL_CED_SENDSTRING(n)		_IOC(_IOC_WRITE, CED_MAGIC_IOC, 2, n)

#define IOCTL_CED_RESET1401		_IO(CED_MAGIC_IOC, 3)
#define IOCTL_CED_GETCHAR		_IO(CED_MAGIC_IOC, 4)
#define IOCTL_CED_SENDCHAR		_IO(CED_MAGIC_IOC, 5)
#define IOCTL_CED_STAT1401		_IO(CED_MAGIC_IOC, 6)
#define IOCTL_CED_LINECOUNT		_IO(CED_MAGIC_IOC, 7)
#define IOCTL_CED_GETSTRING(nMax)	_IOC(_IOC_READ, CED_MAGIC_IOC, 8, nMax)

#define IOCTL_CED_SETTRANSFER		_IOW(CED_MAGIC_IOC, 11, TRANSFERDESC)
#define IOCTL_CED_UNSETTRANSFER		_IO(CED_MAGIC_IOC, 12)
#define IOCTL_CED_SETEVENT		_IOW(CED_MAGIC_IOC, 13, TRANSFEREVENT)
#define IOCTL_CED_GETOUTBUFSPACE	_IO(CED_MAGIC_IOC, 14)
#define IOCTL_CED_GETBASEADDRESS	_IO(CED_MAGIC_IOC, 15)
#define IOCTL_CED_GETDRIVERREVISION	_IO(CED_MAGIC_IOC, 16)

#define IOCTL_CED_GETTRANSFER		_IOR(CED_MAGIC_IOC, 17, TGET_TX_BLOCK)
#define IOCTL_CED_KILLIO1401		_IO(CED_MAGIC_IOC, 18)
#define IOCTL_CED_BLKTRANSSTATE		_IO(CED_MAGIC_IOC, 19)

#define IOCTL_CED_STATEOF1401		_IO(CED_MAGIC_IOC, 23)
#define IOCTL_CED_GRAB1401		_IO(CED_MAGIC_IOC, 25)
#define IOCTL_CED_FREE1401		_IO(CED_MAGIC_IOC, 26)
#define IOCTL_CED_STARTSELFTEST		_IO(CED_MAGIC_IOC, 31)
#define IOCTL_CED_CHECKSELFTEST		_IOR(CED_MAGIC_IOC, 32, TGET_SELFTEST)
#define IOCTL_CED_TYPEOF1401		_IO(CED_MAGIC_IOC, 33)
#define IOCTL_CED_TRANSFERFLAGS		_IO(CED_MAGIC_IOC, 34)

#define IOCTL_CED_DBGPEEK		_IOW(CED_MAGIC_IOC, 35, TDBGBLOCK)
#define IOCTL_CED_DBGPOKE		_IOW(CED_MAGIC_IOC, 36, TDBGBLOCK)
#define IOCTL_CED_DBGRAMPDATA		_IOW(CED_MAGIC_IOC, 37, TDBGBLOCK)
#define IOCTL_CED_DBGRAMPADDR		_IOW(CED_MAGIC_IOC, 38, TDBGBLOCK)
#define IOCTL_CED_DBGGETDATA		_IOR(CED_MAGIC_IOC, 39, TDBGBLOCK)
#define IOCTL_CED_DBGSTOPLOOP		_IO(CED_MAGIC_IOC, 40)
#define IOCTL_CED_FULLRESET		_IO(CED_MAGIC_IOC, 41)
#define IOCTL_CED_SETCIRCULAR		_IOW(CED_MAGIC_IOC, 42, TRANSFERDESC)
#define IOCTL_CED_GETCIRCBLOCK		_IOWR(CED_MAGIC_IOC, 43, TCIRCBLOCK)
#define IOCTL_CED_FREECIRCBLOCK		_IOWR(CED_MAGIC_IOC, 44, TCIRCBLOCK)
#define IOCTL_CED_WAITEVENT		_IO(CED_MAGIC_IOC, 45)
#define IOCTL_CED_TESTEVENT		_IO(CED_MAGIC_IOC, 46)

#ifndef __KERNEL__
/*
                                                                    
                            
 */
inline int CED_SendString(int fh, const char *szText, int n)
{
	return ioctl(fh, IOCTL_CED_SENDSTRING(n), szText);
}

inline int CED_Reset1401(int fh)
{
	return ioctl(fh, IOCTL_CED_RESET1401);
}

/*                                                 */
inline int CED_GetChar(int fh)
{
	return ioctl(fh, IOCTL_CED_GETCHAR);
}

/*                                        */
inline int CED_Stat1401(int fh)
{
	return ioctl(fh, IOCTL_CED_STAT1401);
}

inline int CED_SendChar(int fh, char c)
{
	return ioctl(fh, IOCTL_CED_SENDCHAR, c);
}

inline int CED_LineCount(int fh)
{
	return ioctl(fh, IOCTL_CED_LINECOUNT);
}

/*
                                                                              
                                                                          
                                                                             
                                
 */
inline int CED_GetString(int fh, char *szText, int nMax)
{
	return ioctl(fh, IOCTL_CED_GETSTRING(nMax), szText);
}

/*                                     */
inline int CED_GetOutBufSpace(int fh)
{
	return ioctl(fh, IOCTL_CED_GETOUTBUFSPACE);
}

/*                                            */
inline int CED_GetBaseAddress(int fh)
{
	return ioctl(fh, IOCTL_CED_GETBASEADDRESS);
}

/*                                                   */
inline int CED_GetDriverRevision(int fh)
{
	return ioctl(fh, IOCTL_CED_GETDRIVERREVISION);
}

inline int CED_SetTransfer(int fh, TRANSFERDESC *pTD)
{
	return ioctl(fh, IOCTL_CED_SETTRANSFER, pTD);
}

inline int CED_UnsetTransfer(int fh, int nArea)
{
	return ioctl(fh, IOCTL_CED_UNSETTRANSFER, nArea);
}

inline int CED_SetEvent(int fh, TRANSFEREVENT *pTE)
{
	return ioctl(fh, IOCTL_CED_SETEVENT, pTE);
}

inline int CED_GetTransfer(int fh, TGET_TX_BLOCK *pTX)
{
	return ioctl(fh, IOCTL_CED_GETTRANSFER, pTX);
}

inline int CED_KillIO1401(int fh)
{
	return ioctl(fh, IOCTL_CED_KILLIO1401);
}

/*                                         */
inline int CED_BlkTransState(int fh)
{
	return ioctl(fh, IOCTL_CED_BLKTRANSSTATE);
}

inline int CED_StateOf1401(int fh)
{
	return ioctl(fh, IOCTL_CED_STATEOF1401);
}

inline int CED_Grab1401(int fh)
{
	return ioctl(fh, IOCTL_CED_GRAB1401);
}

inline int CED_Free1401(int fh)
{
	return ioctl(fh, IOCTL_CED_FREE1401);
}

inline int CED_StartSelfTest(int fh)
{
	return ioctl(fh, IOCTL_CED_STARTSELFTEST);
}

inline int CED_CheckSelfTest(int fh, TGET_SELFTEST *pGST)
{
	return ioctl(fh, IOCTL_CED_CHECKSELFTEST, pGST);
}

inline int CED_TypeOf1401(int fh)
{
	return ioctl(fh, IOCTL_CED_TYPEOF1401);
}

inline int CED_TransferFlags(int fh)
{
	return ioctl(fh, IOCTL_CED_TRANSFERFLAGS);
}

inline int CED_DbgPeek(int fh, TDBGBLOCK *pDB)
{
	return ioctl(fh, IOCTL_CED_DBGPEEK, pDB);
}

inline int CED_DbgPoke(int fh, TDBGBLOCK *pDB)
{
	return ioctl(fh, IOCTL_CED_DBGPOKE, pDB);
}

inline int CED_DbgRampData(int fh, TDBGBLOCK *pDB)
{
	return ioctl(fh, IOCTL_CED_DBGRAMPDATA, pDB);
}

inline int CED_DbgRampAddr(int fh, TDBGBLOCK *pDB)
{
	return ioctl(fh, IOCTL_CED_DBGRAMPADDR, pDB);
}

inline int CED_DbgGetData(int fh, TDBGBLOCK *pDB)
{
	return ioctl(fh, IOCTL_CED_DBGGETDATA, pDB);
}

inline int CED_DbgStopLoop(int fh)
{
	return ioctl(fh, IOCTL_CED_DBGSTOPLOOP);
}

inline int CED_FullReset(int fh)
{
	return ioctl(fh, IOCTL_CED_FULLRESET);
}

inline int CED_SetCircular(int fh, TRANSFERDESC *pTD)
{
	return ioctl(fh, IOCTL_CED_SETCIRCULAR, pTD);
}

inline int CED_GetCircBlock(int fh, TCIRCBLOCK *pCB)
{
	return ioctl(fh, IOCTL_CED_GETCIRCBLOCK, pCB);
}

inline int CED_FreeCircBlock(int fh, TCIRCBLOCK *pCB)
{
	return ioctl(fh, IOCTL_CED_FREECIRCBLOCK, pCB);
}

inline int CED_WaitEvent(int fh, int nArea, int msTimeOut)
{
	return ioctl(fh, IOCTL_CED_WAITEVENT, (nArea & 0xff)|(msTimeOut << 8));
}

inline int CED_TestEvent(int fh, int nArea)
{
	return ioctl(fh, IOCTL_CED_TESTEVENT, nArea);
}
#endif

#ifdef NOTWANTEDYET
#define IOCTL_CED_REGCALLBACK		_IO(CED_MAGIC_IOC, 9)	/*          */
#define IOCTL_CED_GETMONITORBUF		_IO(CED_MAGIC_IOC, 10)	/*          */

#define IOCTL_CED_BYTECOUNT		_IO(CED_MAGIC_IOC, 20)	/*          */
#define IOCTL_CED_ZEROBLOCKCOUNT	_IO(CED_MAGIC_IOC, 21)	/*          */
#define IOCTL_CED_STOPCIRCULAR		_IO(CED_MAGIC_IOC, 22)	/*          */

#define IOCTL_CED_REGISTERS1401		_IO(CED_MAGIC_IOC, 24)	/*          */
#define IOCTL_CED_STEP1401		_IO(CED_MAGIC_IOC, 27)	/*          */
#define IOCTL_CED_SET1401REGISTERS	_IO(CED_MAGIC_IOC, 28)	/*          */
#define IOCTL_CED_STEPTILL1401		_IO(CED_MAGIC_IOC, 29)	/*          */
#define IOCTL_CED_SETORIN		_IO(CED_MAGIC_IOC, 30)	/*          */

#endif

/*                 */
#endif
