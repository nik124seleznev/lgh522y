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
#include "gl_os.h"
#include "gl_kal.h"

#include "wlan_lib.h"
#include "debug.h"


/*                                                                              
                                                
                                                                                
*/
#define PROC_MCR_ACCESS                         "mcr"
#define PROC_DRV_STATUS                         "status"
#define PROC_RX_STATISTICS                      "rx_statistics"
#define PROC_TX_STATISTICS                      "tx_statistics"
#define PROC_DBG_LEVEL                          "dbg_level"

#define PROC_MCR_ACCESS_MAX_USER_INPUT_LEN      20
#define PROC_RX_STATISTICS_MAX_USER_INPUT_LEN   10
#define PROC_TX_STATISTICS_MAX_USER_INPUT_LEN   10
#define PROC_DBG_LEVEL_MAX_USER_INPUT_LEN       20
#define PROC_DBG_LEVEL_MAX_DISPLAY_STR_LEN      30


/*                                                                              
                                                 
                                                                                
*/

/*                                                                              
                                                  
                                                                                
*/

/*                                                                              
                                                   
                                                                                
*/
static UINT_32 u4McrOffset = 0;

/*                                                                              
                                             
                                                                                
*/

/*                                                                              
                                                             
                                                                                
*/

/*                                                                              
                                                
                                                                                
*/
/*                                                                            */
/* 
                                                                                
                                             
 
                                                  
                                                         
                               
                                                 
                                               
                                                              
 
                                                                   
*/
/*                                                                            */
static int
procMCRRead (
    char *page,
    char **start,
    off_t off,
    int count,
    int *eof,
    void *data
    )
{
    P_GLUE_INFO_T prGlueInfo; 
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrInfo;
    UINT_32 u4BufLen;
    char *p = page;
    UINT_32 u4Count;
    WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;


    ASSERT(data);

    //                                 
    if (off != 0) {
        return 0; //                         
    }

    prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv((struct net_device *)data));

    rMcrInfo.u4McrOffset = u4McrOffset;

    rStatus = kalIoctl(prGlueInfo,
                        wlanoidQueryMcrRead,
                        (PVOID)&rMcrInfo,
                        sizeof(rMcrInfo),
                        TRUE,
                        TRUE,
                        TRUE,
                        FALSE,
                        &u4BufLen);


    SPRINTF(p, ("MCR (0x%08xh): 0x%08x\n",
        rMcrInfo.u4McrOffset, rMcrInfo.u4McrData));

    u4Count = (UINT_32)((ULONG)p - (ULONG)page);

    *eof = 1;

    return (int)u4Count;

} /*                      */


/*                                                                            */
/* 
                                                                               
                               
 
                                    
                                           
                                                 
                                                          
 
                                                     
*/
/*                                                                            */
static int
procMCRWrite (
    struct file *file,
    const char *buffer,
    unsigned long count,
    void *data
    )
{
    P_GLUE_INFO_T prGlueInfo; 
    char acBuf[PROC_MCR_ACCESS_MAX_USER_INPUT_LEN + 1]; //             
    int i4CopySize;
    PARAM_CUSTOM_MCR_RW_STRUC_T rMcrInfo;
    UINT_32 u4BufLen;
    WLAN_STATUS rStatus = WLAN_STATUS_SUCCESS;


    ASSERT(data);

    i4CopySize = (count < (sizeof(acBuf) - 1)) ? count : (sizeof(acBuf) - 1);
    if (copy_from_user(acBuf, buffer, i4CopySize)) {
        return 0;
    }
    acBuf[i4CopySize] = '\0';

    switch (sscanf(acBuf, "0x%x 0x%x",
                   &rMcrInfo.u4McrOffset, &rMcrInfo.u4McrData)) {
    case 2:
        /*                                                                         
                                                     
         */
        //                                     
        {
            prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv((struct net_device *)data));
 
            u4McrOffset = rMcrInfo.u4McrOffset;

            //                                      
                //                                          

            rStatus = kalIoctl(prGlueInfo,
                                wlanoidSetMcrWrite,
                                (PVOID)&rMcrInfo,
                                sizeof(rMcrInfo),
                                FALSE,
                                FALSE,
                                TRUE,
                                FALSE,
                                &u4BufLen);

        }
        break;

    case 1:
        //                                     
        {
            u4McrOffset = rMcrInfo.u4McrOffset;
        }
        break;

    default:
        break;
    }

    return count;

} /*                       */

#if 0
/*                                                                            */
/* 
                                                                   
 
                                                  
                                                         
                               
                                                 
                                               
                                                              
 
                                                                   
*/
/*                                                                            */
static int
procDrvStatusRead (
    char *page,
    char **start,
    off_t off,
    int count,
    int *eof,
    void *data
    )
{
    P_GLUE_INFO_T prGlueInfo = ((struct net_device *)data)->priv;
    char *p = page;
    UINT_32 u4Count;

    GLUE_SPIN_LOCK_DECLARATION();


    ASSERT(data);

    //                                 
    if (off != 0) {
        return 0; //                         
    }


    SPRINTF(p, ("GLUE LAYER STATUS:"));
    SPRINTF(p, ("\n=================="));

    SPRINTF(p, ("\n* Number of Pending Frames: %ld\n",
        prGlueInfo->u4TxPendingFrameNum));

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    wlanoidQueryDrvStatusForLinuxProc(prGlueInfo->prAdapter, p, &u4Count);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    u4Count += (UINT_32)(p - page);

    *eof = 1;

    return (int)u4Count;

} /*                            */


/*                                                                            */
/* 
                                                                                  
 
                                                  
                                                         
                               
                                                 
                                               
                                                              
 
                                                                   
*/
/*                                                                            */
static int
procRxStatisticsRead (
    char *page,
    char **start,
    off_t off,
    int count,
    int *eof,
    void *data
    )
{
    P_GLUE_INFO_T prGlueInfo = ((struct net_device *)data)->priv;
    char *p = page;
    UINT_32 u4Count;

    GLUE_SPIN_LOCK_DECLARATION();


    ASSERT(data);

    //                                 
    if (off != 0) {
        return 0; //                         
    }


    SPRINTF(p, ("RX STATISTICS (Write 1 to clear):"));
    SPRINTF(p, ("\n=================================\n"));

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    wlanoidQueryRxStatisticsForLinuxProc(prGlueInfo->prAdapter, p, &u4Count);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    u4Count += (UINT_32)(p - page);

    *eof = 1;

    return (int)u4Count;

} /*                               */


/*                                                                            */
/* 
                                                                  
 
                                    
                                           
                                                 
                                                          
 
                                                     
*/
/*                                                                            */
static int
procRxStatisticsWrite (
    struct file *file,
    const char *buffer,
    unsigned long count,
    void *data
    )
{
    P_GLUE_INFO_T prGlueInfo = ((struct net_device *)data)->priv;
    char acBuf[PROC_RX_STATISTICS_MAX_USER_INPUT_LEN + 1]; //             
    UINT_32 u4CopySize;
    UINT_32 u4ClearCounter;

    GLUE_SPIN_LOCK_DECLARATION();


    ASSERT(data);

    u4CopySize = (count < (sizeof(acBuf) - 1)) ? count : (sizeof(acBuf) - 1);
    copy_from_user(acBuf, buffer, u4CopySize);
    acBuf[u4CopySize] = '\0';

    if (sscanf(acBuf, "%ld", &u4ClearCounter) == 1) {
        if (u4ClearCounter == 1) {
            GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

            wlanoidSetRxStatisticsForLinuxProc(prGlueInfo->prAdapter);

            GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
        }
    }

    return count;

} /*                                */


/*                                                                            */
/* 
                                                                                  
 
                                                  
                                                         
                               
                                                 
                                               
                                                              
 
                                                                   
*/
/*                                                                            */
static int
procTxStatisticsRead (
    char *page,
    char **start,
    off_t off,
    int count,
    int *eof,
    void *data
    )
{
    P_GLUE_INFO_T prGlueInfo = ((struct net_device *)data)->priv;
    char *p = page;
    UINT_32 u4Count;

    GLUE_SPIN_LOCK_DECLARATION();


    ASSERT(data);

    //                                 
    if (off != 0) {
        return 0; //                         
    }


    SPRINTF(p, ("TX STATISTICS (Write 1 to clear):"));
    SPRINTF(p, ("\n=================================\n"));

    GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    wlanoidQueryTxStatisticsForLinuxProc(prGlueInfo->prAdapter, p, &u4Count);

    GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

    u4Count += (UINT_32)(p - page);

    *eof = 1;

    return (int)u4Count;

} /*                               */


/*                                                                            */
/* 
                                                                  
 
                                    
                                           
                                                 
                                                          
 
                                                     
*/
/*                                                                            */
static int
procTxStatisticsWrite (
    struct file *file,
    const char *buffer,
    unsigned long count,
    void *data
    )
{
    P_GLUE_INFO_T prGlueInfo = ((struct net_device *)data)->priv;
    char acBuf[PROC_RX_STATISTICS_MAX_USER_INPUT_LEN + 1]; //             
    UINT_32 u4CopySize;
    UINT_32 u4ClearCounter;

    GLUE_SPIN_LOCK_DECLARATION();


    ASSERT(data);

    u4CopySize = (count < (sizeof(acBuf) - 1)) ? count : (sizeof(acBuf) - 1);
    copy_from_user(acBuf, buffer, u4CopySize);
    acBuf[u4CopySize] = '\0';

    if (sscanf(acBuf, "%ld", &u4ClearCounter) == 1) {
        if (u4ClearCounter == 1) {
            GLUE_ACQUIRE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);

            wlanoidSetTxStatisticsForLinuxProc(prGlueInfo->prAdapter);

            GLUE_RELEASE_SPIN_LOCK(prGlueInfo, SPIN_LOCK_FSM);
        }
    }

    return count;

} /*                                */
#endif


#if DBG
static UINT_8 aucDbModuleName[][PROC_DBG_LEVEL_MAX_DISPLAY_STR_LEN] = {
    "DBG_INIT_IDX",
    "DBG_HAL_IDX",
    "DBG_INTR_IDX",
    "DBG_REQ_IDX",    
    "DBG_TX_IDX",
    "DBG_RX_IDX",
    "DBG_RFTEST_IDX",
    "DBG_EMU_IDX",    
    "DBG_SW1_IDX",
    "DBG_SW2_IDX",
    "DBG_SW3_IDX",
    "DBG_SW4_IDX",    
    "DBG_HEM_IDX",
    "DBG_AIS_IDX",
    "DBG_RLM_IDX",
    "DBG_MEM_IDX",
    "DBG_CNM_IDX",    
    "DBG_RSN_IDX",
    "DBG_BSS_IDX",
    "DBG_SCN_IDX",
    "DBG_SAA_IDX",
    "DBG_AAA_IDX",
    "DBG_P2P_IDX",
    "DBG_QM_IDX",
    "DBG_SEC_IDX",
    "DBG_BOW_IDX"
    };
    
extern UINT_8 aucDebugModule[];


/*                                                                            */
/* 
                                                              
 
                                                  
                                                         
                               
                                                 
                                               
                                                              
 
                                                                   
*/
/*                                                                            */
static int
procDbgLevelRead (
    char *page,
    char **start,
    off_t off,
    int count,
    int *eof,
    void *data
    )
{
    char *p = page;
    int i;



    //                                 
    if (off != 0) {
        return 0; //                         
    }

    for (i = 0; i < (sizeof(aucDbModuleName)/PROC_DBG_LEVEL_MAX_DISPLAY_STR_LEN); i++) {
        SPRINTF(p, ("%c %-15s(0x%02x): %02x\n",
            ((i == u4DebugModule) ? '*' : ' '),
            &aucDbModuleName[i][0],
            i,
            aucDebugModule[i]));
    }

    *eof = 1;
    return (int)(p - page);
}


/*                                                                            */
/* 
                                                                                      
 
                                    
                                           
                                                 
                                                          
 
                                                     
*/
/*                                                                            */
static int
procDbgLevelWrite (
    struct file *file,
    const char *buffer,
    unsigned long count,
    void *data
    )
{
    char acBuf[PROC_DBG_LEVEL_MAX_USER_INPUT_LEN + 1]; //             
    UINT_32 u4CopySize;
    UINT_32 u4NewDbgModule, u4NewDbgLevel;


    u4CopySize = (count < (sizeof(acBuf) - 1)) ? count : (sizeof(acBuf) - 1);
    copy_from_user(acBuf, buffer, u4CopySize);
    acBuf[u4CopySize] = '\0';

    if (sscanf(acBuf, "0x%x 0x%x", &u4NewDbgModule, &u4NewDbgLevel) == 2) {
        if (u4NewDbgModule < DBG_MODULE_NUM) {
            u4DebugModule = u4NewDbgModule;
            u4NewDbgLevel &= DBG_CLASS_MASK;
            aucDebugModule[u4DebugModule] = (UINT_8)u4NewDbgLevel;
        }
    }

    return count;
}
#endif /*     */


/*                                                                            */
/* 
                                                                        
 
                                                         
                                                          
 
             
*/
/*                                                                            */
INT_32
procInitProcfs (
    struct net_device *prDev,
    char *pucDevName
    )
{
    P_GLUE_INFO_T prGlueInfo;
    struct proc_dir_entry *prEntry;


    ASSERT(prDev);

    if (init_net.proc_net == (struct proc_dir_entry *)NULL) {
        DBGLOG(INIT, INFO, ("init proc fs fail: proc_net == NULL\n"));
        return -ENOENT;
    }

    prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));

    if (!prGlueInfo) {
        DBGLOG(INIT, WARN, ("The OS context is NULL\n"));
        return -ENOENT;
    }


    /*
                   
                                                     
                                                     
                                                        
                                                        
                                                    
                        
     */

    /*
                                       
    */

    prGlueInfo->pProcRoot = proc_mkdir(pucDevName, init_net.proc_net);
    if (prGlueInfo->pProcRoot == NULL) {
        return -ENOENT;
    }

    /*                    */
    prEntry = create_proc_entry(PROC_MCR_ACCESS, 0, prGlueInfo->pProcRoot);
    if (prEntry) {
        prEntry->read_proc = procMCRRead;
        prEntry->write_proc = procMCRWrite;
        prEntry->data = (void *)prDev;
    }

#if 0
    /*                       */
    prEntry = create_proc_read_entry(PROC_DRV_STATUS, 0, prGlueInfo->pProcRoot,
                                     procDrvStatusRead, prDev);

    /*                              */
    prEntry = create_proc_entry(PROC_RX_STATISTICS, 0, prGlueInfo->pProcRoot);
    if (prEntry) {
        prEntry->read_proc = procRxStatisticsRead;
        prEntry->write_proc = procRxStatisticsWrite;
        prEntry->data = (void *)prDev;
    }

    /*                              */
    prEntry = create_proc_entry(PROC_TX_STATISTICS, 0, prGlueInfo->pProcRoot);
    if (prEntry) {
        prEntry->read_proc = procTxStatisticsRead;
        prEntry->write_proc = procTxStatisticsWrite;
        prEntry->data = (void *)prDev;
    }

#if DBG
    /*                          */
    prEntry = create_proc_entry(PROC_DBG_LEVEL, 0644, prGlueInfo->pProcRoot);
    if (prEntry) {
        prEntry->read_proc = procDbgLevelRead;
        prEntry->write_proc = procDbgLevelWrite;
    }
#endif /*     */
#endif
    return 0;

} /*                         */


/*                                                                            */
/* 
                                                                      
 
                                                         
                                                          
 
             
*/
/*                                                                            */
INT_32
procRemoveProcfs (
    struct net_device *prDev,
    char *pucDevName
    )
{
    P_GLUE_INFO_T prGlueInfo = NULL;


    ASSERT(prDev);

    if (!prDev) {
        return -ENOENT;
    }

    if (init_net.proc_net == (struct proc_dir_entry *)NULL) {
        DBGLOG(INIT, WARN, ("remove proc fs fail: proc_net == NULL\n"));
        return -ENOENT;
    }

    prGlueInfo = *((P_GLUE_INFO_T *) netdev_priv(prDev));
    if (!prGlueInfo->pProcRoot) {
        DBGLOG(INIT, WARN, ("The procfs root is NULL\n"));
        return -ENOENT;
    }
#if 0
#if DBG
    remove_proc_entry(PROC_DBG_LEVEL,       prGlueInfo->pProcRoot);
#endif /*     */
    remove_proc_entry(PROC_TX_STATISTICS,   prGlueInfo->pProcRoot);
    remove_proc_entry(PROC_RX_STATISTICS,   prGlueInfo->pProcRoot);
    remove_proc_entry(PROC_DRV_STATUS,      prGlueInfo->pProcRoot);
#endif
    remove_proc_entry(PROC_MCR_ACCESS,      prGlueInfo->pProcRoot);

    /*                                        */
    remove_proc_entry(pucDevName, init_net.proc_net);

    return 0;

} /*                           */

