/*
                                                                               
*/

/*                  
                                                       

                                               
*/



/*
                 
  
                   
                                                                                          
                                       
  
                   
                                                                                          
                                            
  
                   
                                                                                          
                                   
  
                   
   
                                                                               
  
                         
                                                 
                                   
                                                                           
                
  
*/

/*                                                                              
                                                     
                                                                                
*/

/*                                                                              
                                                          
                                                                                
*/
#include "precomp.h"

/*                                                                              
                                                
                                                                                
*/

/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
#if DBG
/*                                                                            */
/* 
                                                                     
 
                                                                                       
                                                              
 
                
*/
/*                                                                            */
VOID
dumpMemory8 (
    IN PUINT_8  pucStartAddr,
    IN UINT_32  u4Length
    )
{
    ASSERT(pucStartAddr);

    LOG_FUNC("DUMP8 ADDRESS: %p, Length: %u\n", pucStartAddr, u4Length);

    while (u4Length > 0) {
        if (u4Length >= 16) {
            LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x %02x %02x\n",
                pucStartAddr,
                pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                pucStartAddr[12], pucStartAddr[13], pucStartAddr[14], pucStartAddr[15]);
            u4Length -= 16;
            pucStartAddr += 16;
        }
        else {
            switch (u4Length) {
            case 1:
                LOG_FUNC("(%p) %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0]);
                break;
            case 2:
                LOG_FUNC("(%p) %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1]);
                break;
            case 3:
                LOG_FUNC("(%p) %02x %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2]);
                break;
            case 4:
                LOG_FUNC("(%p) %02x %02x %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3]);
                break;
            case 5:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4]);
                break;
            case 6:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5]);
                break;
            case 7:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6]);
                break;
            case 8:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7]);
                break;
            case 9:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8]);
                break;
            case 10:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x\n",
                   	pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9]);
                break;
            case 11:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10]);
                break;
            case 12:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11]);
                break;
            case 13:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                    pucStartAddr[12]);
                break;
            case 14:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                    pucStartAddr[12], pucStartAddr[13]);
                break;
            case 15:
                LOG_FUNC("(%p) %02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x %02x\n",
                    pucStartAddr,
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                    pucStartAddr[12], pucStartAddr[13], pucStartAddr[14]);
                break;
            default:
                break;
            }
            u4Length = 0;
        }
    }

    LOG_FUNC("\n");

    return;
} /*                      */


/*                                                                            */
/* 
                                                                            
 
                                                                                       
                                                              
 
                
*/
/*                                                                            */
VOID
dumpMemory32 (
    IN PUINT_32 pu4StartAddr,
    IN UINT_32  u4Length)
{
    PUINT_8 pucAddr;


    ASSERT(pu4StartAddr);

    LOG_FUNC("DUMP32 ADDRESS: %p, Length: %u\n", pu4StartAddr, u4Length);

    if (IS_NOT_ALIGN_4((UINT_32)pu4StartAddr)) {
        UINT_32 u4ProtrudeLen = sizeof(UINT_32) - ((ULONG)pu4StartAddr % 4);


        u4ProtrudeLen = ((u4Length < u4ProtrudeLen) ? u4Length: u4ProtrudeLen);
        LOG_FUNC("pu4StartAddr is not at DW boundary.\n");
        pucAddr = (PUINT_8) &pu4StartAddr[0];

        switch (u4ProtrudeLen) {
            case 1:
                LOG_FUNC("(%p) %02x------\n",
                   pu4StartAddr,
                   pucAddr[0]);
                break;
            case 2:
                LOG_FUNC("(%p) %02x%02x----\n",
                   pu4StartAddr,
                   pucAddr[1], pucAddr[0]);
                break;
            case 3:
                LOG_FUNC("(%p) %02x%02x%02x--\n",
                   pu4StartAddr,
                   pucAddr[2], pucAddr[1], pucAddr[0]);
                break;
            default:
                break;
        }

        u4Length -= u4ProtrudeLen;
        pu4StartAddr = (PUINT_32)((ULONG)pu4StartAddr + u4ProtrudeLen);
    }

    while (u4Length > 0) {
        if (u4Length >= 16) {
            LOG_FUNC("(%p) %08x %08x %08x %08x\n",
                pu4StartAddr,
                pu4StartAddr[0], pu4StartAddr[1], pu4StartAddr[2], pu4StartAddr[3]);
            pu4StartAddr += 4;
            u4Length -= 16;
        }
        else {
            switch (u4Length) {
            case 1:
                 pucAddr = (PUINT_8) &pu4StartAddr[0];
                 LOG_FUNC("(%p) ------%02x\n",
                    pu4StartAddr,
                    pucAddr[0]);
                 break;
            case 2:
                 pucAddr = (PUINT_8) &pu4StartAddr[0];
                 LOG_FUNC("(%p) ----%02x%02x\n",
                    pu4StartAddr,
                    pucAddr[1], pucAddr[0]);
                 break;
            case 3:
                 pucAddr = (PUINT_8) &pu4StartAddr[0];
                 LOG_FUNC("(%p) --%02x%02x%02x\n",
                    pu4StartAddr,
                    pucAddr[2], pucAddr[1], pucAddr[0]);
                 break;
            case 4:
                 LOG_FUNC("(%p) %08x\n",
                    pu4StartAddr,
                    pu4StartAddr[0]);
                 break;
            case 5:
                 pucAddr = (PUINT_8) &pu4StartAddr[1];
                 LOG_FUNC("(%p) %08x ------%02x\n",
                    pu4StartAddr,
                    pu4StartAddr[0],
                    pucAddr[0]);
                 break;
            case 6:
                 pucAddr = (PUINT_8) &pu4StartAddr[1];
                 LOG_FUNC("(%p) %08x ----%02x%02x\n",
                    pu4StartAddr,
                    pu4StartAddr[0],
                    pucAddr[1], pucAddr[0]);
                 break;
            case 7:
                 pucAddr = (PUINT_8) &pu4StartAddr[1];
                 LOG_FUNC("(%p) %08x --%02x%02x%02x\n",
                    pu4StartAddr,
                    pu4StartAddr[0],
                    pucAddr[2], pucAddr[1], pucAddr[0]);
                 break;
            case 8:
                 LOG_FUNC("(%p) %08x %08x\n",
                    pu4StartAddr,
                    pu4StartAddr[0], pu4StartAddr[1]);
                 break;
            case 9:
                 pucAddr = (PUINT_8) &pu4StartAddr[2];
                 LOG_FUNC("(%p) %08x %08x ------%02x\n",
                    pu4StartAddr,
                    pu4StartAddr[0], pu4StartAddr[1],
                    pucAddr[0]);
                 break;
            case 10:
                 pucAddr = (PUINT_8) &pu4StartAddr[2];
                 LOG_FUNC("(%p) %08x %08x ----%02x%02x\n",
                    pu4StartAddr,
                    pu4StartAddr[0], pu4StartAddr[1],
                    pucAddr[1], pucAddr[0]);
                 break;
            case 11:
                 pucAddr = (PUINT_8) &pu4StartAddr[2];
                 LOG_FUNC("(%p) %08x %08x --%02x%02x%02x\n",
                    pu4StartAddr,
                    pu4StartAddr[0], pu4StartAddr[1],
                    pucAddr[2], pucAddr[1], pucAddr[0]);
                 break;
            case 12:
                 LOG_FUNC("(%p) %08x %08x %08x\n",
                    pu4StartAddr,
                    pu4StartAddr[0], pu4StartAddr[1], pu4StartAddr[2]);
                 break;
            case 13:
                 pucAddr = (PUINT_8) &pu4StartAddr[3];
                 LOG_FUNC("(%p) %08x %08x %08x ------%02x\n",
                    pu4StartAddr,
                    pu4StartAddr[0], pu4StartAddr[1], pu4StartAddr[2],
                    pucAddr[0]);
                 break;
            case 14:
                 pucAddr = (PUINT_8) &pu4StartAddr[3];
                 LOG_FUNC("(%p) %08x %08x %08x ----%02x%02x\n",
                     pu4StartAddr,
                     pu4StartAddr[0], pu4StartAddr[1], pu4StartAddr[2],
                     pucAddr[1], pucAddr[0]);
                 break;
            case 15:
                 pucAddr = (PUINT_8) &pu4StartAddr[3];
                 LOG_FUNC("(%p) %08x %08x %08x --%02x%02x%02x\n",
                     pu4StartAddr,
                     pu4StartAddr[0], pu4StartAddr[1], pu4StartAddr[2],
                     pucAddr[2], pucAddr[1], pucAddr[0]);
                 break;
            default:
                break;
            }
            u4Length = 0;
        }
    }

    return;
} /*                       */
#elif CFG_SUPPORT_XLOG

/*                                                                            */
/* 
                                                                     
 
                                                                                       
                                                              
 
                
*/
/*                                                                            */
VOID
dumpMemory8 (
    IN UINT_32  log_level,
    IN PUINT_8  pucStartAddr,
    IN UINT_32  u4Length
    )
{
    ASSERT(pucStartAddr);

    if (log_level == ANDROID_LOG_ERROR) {
        xlog_printk(ANDROID_LOG_ERROR, XLOG_TAG, "DUMP8 ADDRESS: %p, Length: %u\n", pucStartAddr, u4Length);
    }
    else if (log_level == ANDROID_LOG_WARN) {
        xlog_printk(ANDROID_LOG_WARN, XLOG_TAG, "DUMP8 ADDRESS: %p, Length: %u\n", pucStartAddr, u4Length);
    }
    else if (log_level == ANDROID_LOG_INFO) {
        xlog_printk(ANDROID_LOG_INFO, XLOG_TAG, "DUMP8 ADDRESS: %p, Length: %u\n", pucStartAddr, u4Length);
    }
    else if (log_level == ANDROID_LOG_DEBUG) {
        xlog_printk(ANDROID_LOG_DEBUG, XLOG_TAG, "DUMP8 ADDRESS: %p, Length: %u\n", pucStartAddr, u4Length);
    }
    else if (log_level == ANDROID_LOG_VERBOSE) {
        xlog_printk(ANDROID_LOG_VERBOSE, XLOG_TAG, "DUMP8 ADDRESS: %p, Length: %u\n", pucStartAddr, u4Length);
    }

    while (u4Length > 0) {
        if (u4Length >= 16) {
            XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x %02x %02x\n",
                pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                pucStartAddr[12], pucStartAddr[13], pucStartAddr[14], pucStartAddr[15]);
            u4Length -= 16;
            pucStartAddr += 16;
        }
        else {
            switch (u4Length) {
            case 1:
                XLOG_FUNC(log_level, "%02x\n",
                    pucStartAddr[ 0]);
                break;
            case 2:
                XLOG_FUNC(log_level, "%02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1]);
                break;
            case 3:
                XLOG_FUNC(log_level, "%02x %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2]);
                break;
            case 4:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3]);
                break;
            case 5:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4]);
                break;
            case 6:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5]);
                break;
            case 7:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6]);
                break;
            case 8:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7]);
                break;
            case 9:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x - %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8]);
                break;
            case 10:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9]);
                break;
            case 11:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10]);
                break;
            case 12:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11]);
                break;
            case 13:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                    pucStartAddr[12]);
                break;
            case 14:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                    pucStartAddr[12], pucStartAddr[13]);
                break;
            case 15:
                XLOG_FUNC(log_level, "%02x %02x %02x %02x  %02x %02x %02x %02x - %02x %02x %02x %02x  %02x %02x %02x\n",
                    pucStartAddr[ 0], pucStartAddr[ 1], pucStartAddr[ 2], pucStartAddr[ 3],
                    pucStartAddr[ 4], pucStartAddr[ 5], pucStartAddr[ 6], pucStartAddr[ 7],
                    pucStartAddr[ 8], pucStartAddr[ 9], pucStartAddr[10], pucStartAddr[11],
                    pucStartAddr[12], pucStartAddr[13], pucStartAddr[14]);
                break;
            default:
                break;
            }
            u4Length = 0;
        }
    }


    return;
} /*                      */
#endif /*     */


