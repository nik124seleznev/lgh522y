#ifndef SEC_PAL_H
#define SEC_PAL_H

#include "sec_osal_light.h"
#include <mach/sec_osal.h>  

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef unsigned int uint32;
typedef unsigned char uchar;

/*                                                                             
         
                                                                              */
/*                     */
#define DBG_EVT_NONE        (0)       /*          */
#define DBG_EVT_CMD         (1 << 0)  /*                       */
#define DBG_EVT_FUNC        (1 << 1)  /*                    */
#define DBG_EVT_INFO        (1 << 2)  /*                       */
#define DBG_EVT_WRN         (1 << 30) /*               */
#define DBG_EVT_ERR         (1 << 31) /*             */
#define DBG_EVT_ALL         (0xffffffff)

#define DBG_EVT_MASK        (DBG_EVT_ALL)

#ifdef SEC_DEBUG
#define MSG(evt, fmt, args...) \
do {    \
    if ((DBG_EVT_##evt) & DBG_EVT_MASK) { \
        printk(fmt, ##args); \
    }   \
} while(0)
#else
#define MSG(evt, fmt, args...)  do{}while(0)
#endif

#define MSG_FUNC(mod) MSG(FUNC, "[%s] %s\n", mod, __FUNCTION__)

/*                                                                             
                   
                                                                              */
extern void *mcpy(void *dest, const void *src, int  count);
extern int mcmp (const void *cs, const void *ct, int count);
extern void dump_buf(unsigned char* buf, unsigned int len);

#endif /*                  */
