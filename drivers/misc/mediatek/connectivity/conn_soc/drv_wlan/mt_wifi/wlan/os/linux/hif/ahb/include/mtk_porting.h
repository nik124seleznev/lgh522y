/*               */
/*         */

#ifndef _MTK_PORTING_H_
#define _MTK_PORTING_H_

#include <linux/kernel.h> /*                           */

#define CONF_MTK_AHB_DMA            1

/*                                     */
/*                                 
                                    
                                   */

/*                                       */
/*                                     
                                        
                                      */

#ifndef VOID
/*                          */
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INTOUT
#define INOUT
#endif

#ifndef TRUE
#define TRUE        1
#endif

#ifndef FALSE
#define FALSE       0
#endif

#ifndef BIT
#define BIT(n)                          ((UINT_32) 1U << (n))
#endif /*     */

#ifndef BITS
/*                                               
                                                                   
                                   
 */
#define BITS(m,n)                       (~(BIT(m)-1) & ((BIT(n) - 1) | BIT(n)))
#endif /*     */

#ifndef BOOLEAN
#define BOOLEAN         unsigned char
#endif

typedef int MTK_WCN_BOOL;
#ifndef MTK_WCN_BOOL_TRUE
#define MTK_WCN_BOOL_FALSE               ((MTK_WCN_BOOL) 0)
#define MTK_WCN_BOOL_TRUE                ((MTK_WCN_BOOL) 1)
#endif

typedef int MTK_WCN_MUTEX;

typedef int MTK_WCN_TIMER;

/*             */
/*       */
typedef MTK_WCN_MUTEX (*MUTEX_CREATE)(const char * const name);
typedef INT_32 (*MUTEX_DESTROY)(MTK_WCN_MUTEX mtx);
typedef INT_32 (*MUTEX_LOCK)(MTK_WCN_MUTEX mtx);
typedef INT_32 (*MUTEX_UNLOCK)(MTK_WCN_MUTEX mtx, unsigned long flags);
/*       */
typedef INT_32 (*DBG_PRINT)(const char *str, ...);
typedef INT_32 (*DBG_ASSERT)(INT_32 expr, const char *file, INT_32 line);
/*       */
typedef void (*MTK_WCN_TIMER_CB)(void);
typedef MTK_WCN_TIMER (*TIMER_CREATE)(const char * const name);
typedef INT_32 (*TIMER_DESTROY)(MTK_WCN_TIMER tmr);
typedef INT_32 (*TIMER_START)(MTK_WCN_TIMER tmr, UINT_32 timeout, MTK_WCN_TIMER_CB tmr_cb, void *param);
typedef INT_32 (*TIMER_STOP)(MTK_WCN_TIMER tmr);
/*            */
typedef void* (*SYS_MEMCPY)(void *dest, const void *src, UINT_32 n);
typedef void* (*SYS_MEMSET)(void *s, INT_32 c, UINT_32 n);
typedef INT_32 (*SYS_SPRINTF)(char *str, const char *format, ...);

#endif /*                 */

