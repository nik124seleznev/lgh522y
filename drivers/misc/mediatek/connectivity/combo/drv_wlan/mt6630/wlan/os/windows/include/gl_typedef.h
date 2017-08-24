/*
                                                                                                 
*/

/*                      
                                                        

                                               
*/



/*
                       
  
                      
                                                             
                                                  
                                                                      
  
                   
                                                                                                          
                                                  
  
                   
  
                                                                               
  
                         
                                                
                                   
  
                   
                                                                          
                                                                                                       
                                                                           
                                           
                                                                           
                                                 
                                                                           
                    
  
*/


#ifndef _GL_TYPEDEF_H
#define _GL_TYPEDEF_H

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
LINT_EXT_HEADER_BEGIN
#include <ndis.h>		/*             */
#include <intsafe.h>
#include <stddef.h>
    LINT_EXT_HEADER_END
/*                                                                              
                                                
                                                                                
*/
/*                                                       */
#define KAL_HZ                  1000
/*                       */
#ifndef FALSE
#define FALSE               ((BOOL) 0)
#define TRUE                ((BOOL) 1)
#endif				/*       */
#ifndef NULL
#if defined(__cplusplus)
#define NULL            0
#else
#define NULL            ((void *) 0)
#endif
#endif
/*                                                                              
                                                 
                                                                                
*/
#ifdef _lint
typedef void VOID, *PVOID;
typedef unsigned char BOOLEAN, *PBOOLEAN;
typedef BOOLEAN BOOL;
typedef signed char CHAR;
typedef signed short SHORT;
typedef signed long LONG;
typedef signed long long LONGLONG;
typedef unsigned char UCHAR, *PUCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef unsigned long long ULONGLONG;
#endif				/*       */


/*                          */
typedef VOID * *PPVOID;
#if defined(WINDOWS_DDK)
typedef BOOLEAN BOOL;
#endif

/*                                     */
typedef CHAR INT_8, *PINT_8, **PPINT_8;	/*                               */
typedef SHORT INT_16, *PINT_16, **PPINT_16;	/*                               */
typedef LONG INT_32, *PINT_32, **PPINT_32;	/*                               */
typedef LONGLONG INT_64, *PINT_64, **PPINT_64;	/*                               */

/*                                       */
typedef UCHAR UINT_8, *PUINT_8, **PPUINT_8, *P_UINT_8;	/*                                 */
typedef USHORT UINT_16, *PUINT_16, **PPUINT_16;	/*                                 */
typedef ULONG UINT_32, *PUINT_32, **PPUINT_32;	/*                                 */
typedef ULONGLONG UINT_64, *PUINT_64, **PPUINT_64;	/*                                 */

typedef UINT_32 OS_SYSTIME, *POS_SYSTIME;


#ifdef _lint
typedef UINT_32 NDIS_STATUS;
typedef UINT_32 NDIS_SPIN_LOCK;
typedef UINT_32 NDIS_EVENT;

typedef union _LARGE_INTEGER {
	struct {
		UINT_32 LowPart;
		INT_32 HighPart;
	} u;
	INT_64 QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
	struct {
		UINT_32 LowPart;
		UINT_32 HighPart;
	} u;
	UINT_64 QuadPart;
} ULARGE_INTEGER, *PULARGE_INTEGER;

typedef struct _NDIS_PACKET {
	UCHAR MiniportReservedEx[16];
} NDIS_PACKET, *PNDIS_PACKET;


extern void DbgPrint(PINT_8 Format, ...);
extern void DbgBreakPoint(void);

#if 0				/*                                                      */
#define CONTAINING_RECORD(address, type, field) \
	((type *)((PUCHAR)(address) - (PUCHAR)(&((type *)0)->field)))
#else
#define CONTAINING_RECORD(address, type, field) \
	((type *)(address))
#endif


#endif				/*       */

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define IN
#define OUT

#define __KAL_INLINE__          inline
#define __KAL_ATTRIB_PACKED__
#define __KAL_ATTRIB_ALIGN_4__


#ifndef BIT
#define BIT(n)                          ((UINT_32) 1 << (n))
#define BITS2(m, n)                      (BIT(m) | BIT(n))
#define BITS3(m, n, o)                    (BIT(m) | BIT (n) | BIT (o))
#define BITS4(m, n, o, p)                  (BIT(m) | BIT (n) | BIT (o) | BIT(p))

/*                                               
                                                                   
                                   
 */
#define BITS(m, n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif				/*     */


/*                                                                         
        
                          
                                        */
#ifndef OFFSET_OF
    /*                                                                                        */
#ifdef _lint
#define OFFSET_OF(_type, _field)    4
#else
#define OFFSET_OF(_type, _field)    ((UINT_32)&(((_type *)0)->_field))
#endif				/*       */
#endif				/*           */


/*                                                                  
                                                                        
                        
                                                            
                          
                                       
 */
#ifndef ENTRY_OF
    /*                                                          */
#ifdef _lint
#define ENTRY_OF(_addrOfField, _type, _field) \
	((_type *)(_addrOfField))
#else
#define ENTRY_OF(_addrOfField, _type, _field) \
	((_type *)((PINT_8)(_addrOfField) - (PINT_8)OFFSET_OF(_type, _field)))
#endif				/*       */
#endif				/*          */


/*                                                     
                               
 */
#ifndef ALIGN_4
#define ALIGN_4(_value)            (((_value) + 3) & ~BITS(0, 1))
#endif				/*         */

/*                                                      
                                          
 */
#ifndef IS_ALIGN_4
#define IS_ALIGN_4(_value)          (((_value) & 0x3) ? FALSE : TRUE)
#endif				/*            */

#ifndef IS_NOT_ALIGN_4
#define IS_NOT_ALIGN_4(_value)      (((_value) & 0x3) ? TRUE : FALSE)
#endif				/*                */


/*                                                                      
                                                                       
 */
#ifndef BYTE_TO_DWORD
#define BYTE_TO_DWORD(_value)       ((_value + 3) >> 2)
#endif				/*               */

/*                                                      
                                                           
 */
#ifndef DWORD_TO_BYTE
#define DWORD_TO_BYTE(_value)       ((_value) << 2)
#endif				/*               */


#define SWAP16(_x)   \
	((UINT_16)((((UINT_16)(_x) & 0x00FF) << 8) | \
		    (((UINT_16)(_x) & 0xFF00) >> 8)))

#define SWAP32(_x)   \
	((UINT_32)((((UINT_32)(_x) & 0x000000FF) << 24) | \
		    (((UINT_32)(_x) & 0x0000FF00) << 8) | \
		    (((UINT_32)(_x) & 0x00FF0000) >> 8) | \
		    (((UINT_32)(_x) & 0xFF000000) >> 24)))

/*                                    
                                           
 */
#if 1				/*               */
#define CONST_NTOHS(_x)     SWAP16(_x)

#define CONST_HTONS(_x)     SWAP16(_x)

#define NTOHS(_x)           SWAP16(_x)

#define HTONS(_x)           SWAP16(_x)

#define NTOHL(_x)           SWAP32(_x)

#define HTONL(_x)           SWAP32(_x)

#else				/*            */

#define CONST_NTOHS(_x)

#define CONST_HTONS(_x)

#define NTOHS(_x)

#define HTONS(_x)

#endif

/*                                                                              
                                                            
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/

#endif				/*               */
