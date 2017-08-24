#ifndef _DFO_BOOT_H_
#define _DFO_BOOT_H_

/* 
            
 */
#define ATAG_DFO_DATA 0x41000805
typedef struct
{
    char name[32];              //                
    unsigned long value;        //                 
} dfo_boot_info;

typedef struct
{
    dfo_boot_info info[1];      /*                          */
} tag_dfo_boot;

#endif 
