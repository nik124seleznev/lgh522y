/*
                                                                                               
*/

/*                      
                                                        

                                               
*/



/*
                       
  
                   
                                                                                             
                                                  
  
                   
                                                                                
                          
  
                           
       
                                      
  
                   
  
                                                                               
  
                         
                                                
                                   
  
                           
                                             
                                
                                                             
                                           
                                     
                                 
                                                             
                                  
                                                             
                                 
                                                             
                                                                               
                                              
                                                                          
  
                                              
                                                                          
  
*/

#ifndef _GL_TYPEDEF_H
#define _GL_TYPEDEF_H

#if defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#endif

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                                                       */
#define KAL_HZ                  (1000)

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

#if defined(CONFIG_HAS_EARLYSUSPEND)
typedef void (*early_suspend_callback) (struct early_suspend *h);
typedef void (*late_resume_callback) (struct early_suspend *h);
#endif


/*                                                                              
                                                 
                                                                                
*/
/*                          */
typedef void VOID, *PVOID, **PPVOID;

/*                             */
typedef unsigned char BOOL, *PBOOL, BOOLEAN, *PBOOLEAN;

/*                                     */
typedef signed char CHAR, *PCHAR, **PPCHAR;
typedef signed char INT_8, *PINT_8, **PPINT_8;
typedef signed short INT_16, *PINT_16, **PPINT_16;
typedef signed long INT_32, *PINT_32, **PPINT_32;
typedef signed long long INT_64, *PINT_64, **PPINT_64;

/*                                       */
typedef unsigned char UCHAR, *PUCHAR, **PPUCHAR;
typedef unsigned char UINT_8, *PUINT_8, **PPUINT_8, *P_UINT_8;
typedef unsigned short UINT_16, *PUINT_16, **PPUINT_16;
typedef unsigned long ULONG, UINT_32, *PUINT_32, **PPUINT_32;
typedef unsigned long long UINT_64, *PUINT_64, **PPUINT_64;

typedef unsigned long OS_SYSTIME, *POS_SYSTIME, **PPOS_SYSTIME;


/*                                                                      
                                                                                  
                                                     
                                              
 */
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


typedef INT_32(*probe_card) (PVOID pvData);
typedef VOID(*remove_card) (VOID);

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/
#define IN			/*          */
#define OUT			/*          */

#define __KAL_INLINE__                  static inline
#define __KAL_ATTRIB_PACKED__           __attribute__((__packed__))
#define __KAL_ATTRIB_ALIGN_4__          __attribute__ ((aligned(4)))


#ifndef BIT
#define BIT(n)                          ((UINT_32) 1UL << (n))
#endif				/*     */

#ifndef BITS
/*                                               
                                                                   
                                   
 */
#define BITS(m, n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif				/*     */


/*                                                                         
        
                          
                                        */
#ifndef OFFSET_OF
#define OFFSET_OF(_type, _field)    ((UINT_32)&(((_type *)0)->_field))
#endif				/*           */


/*                                                                  
                                                                        
                        
                                                            
                          
                                       
 */
#ifndef ENTRY_OF
#define ENTRY_OF(_addrOfField, _type, _field) \
	((_type *)((PINT_8)(_addrOfField) - (PINT_8)OFFSET_OF(_type, _field)))
#endif				/*          */


/*                                                     
                               
 */
#ifndef ALIGN_4
#define ALIGN_4(_value)             (((_value) + 3) & ~3u)
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

#if 1				/*               */
#define CONST_NTOHS(_x)     __constant_ntohs(_x)

#define CONST_HTONS(_x)     __constant_htons(_x)

#define NTOHS(_x)           ntohs(_x)

#define HTONS(_x)           htons(_x)

#define NTOHL(_x)           ntohl(_x)

#define HTONL(_x)           htonl(_x)

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
